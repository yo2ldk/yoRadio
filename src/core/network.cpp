#include "network.h"
#include "display.h"
#include "options.h"
#include "config.h"
#include "telnet.h"
#include "netserver.h"
#include "player.h"
#include "mqtt.h"

Network network;

TaskHandle_t syncTaskHandle;
bool getWeather(char *wstr);
void doSync(void * pvParameters);

void ticks() {
  static const uint16_t weatherSyncInterval=1800;
  //static const uint16_t weatherSyncIntervalFail=10;
  static const uint16_t timeSyncInterval=3600;
  static uint16_t timeSyncTicks = 0;
  static uint16_t weatherSyncTicks = 0;
  static bool divrssi;
  timeSyncTicks++;
  weatherSyncTicks++;
  divrssi = !divrssi;
  if(network.forceTimeSync || network.forceWeather){
    xTaskCreatePinnedToCore(doSync, "doSync", 1024 * 4, NULL, 0, &syncTaskHandle, 0);
  }
  if(timeSyncTicks >= timeSyncInterval){
    timeSyncTicks=0;
    network.forceTimeSync = true;
  }
  if(weatherSyncTicks >= weatherSyncInterval){
    weatherSyncTicks=0;
    network.forceWeather = true;
  }
  
  if(network.timeinfo.tm_year>100) {
    network.timeinfo.tm_sec++;
    mktime(&network.timeinfo);
    display.putRequest(CLOCK);
  }
  if(player.isRunning() && config.store.play_mode==PM_SDCARD) netserver.requestOnChange(SDPOS, 0);
  if(divrssi) {
    int rs = WiFi.RSSI();
    netserver.setRSSI(rs);
    netserver.requestOnChange(NRSSI, 0);
    display.putRequest(DSPRSSI, rs);
    
  }
}

void Network::WiFiReconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  network.beginReconnect = false;
  player.lockOutput = false;
  delay(100);
  display.putRequest(NEWMODE, PLAYER);
  if (network.lostPlaying) player.sendCommand({PR_PLAY, config.store.lastStation});
  #ifdef MQTT_ROOT_TOPIC
    connectToMqtt();
  #endif
}

void Network::WiFiLostConnection(WiFiEvent_t event, WiFiEventInfo_t info){
  if(!network.beginReconnect){
    Serial.println("Lost connection, reconnecting...");
    network.lostPlaying = player.isRunning();
    if (network.lostPlaying) { player.lockOutput = true; player.sendCommand({PR_STOP, 0}); }
    display.putRequest(NEWMODE, LOST);
  }
  network.beginReconnect = true;
  WiFi.begin(config.ssids[config.store.lastSSID].ssid, config.ssids[config.store.lastSSID].password);
}

#define DBGAP false

void Network::begin() {
  BOOTLOG("network.begin");
  config.initNetwork();
  ctimer.detach();
  forceTimeSync = forceWeather = true;
  if (config.ssidsCount == 0 || DBGAP) {
    raiseSoftAP();
    return;
  }
  byte ls = (config.store.lastSSID == 0 || config.store.lastSSID > config.ssidsCount) ? 0 : config.store.lastSSID - 1;
  byte startedls = ls;
  byte errcnt = 0;
  WiFi.mode(WIFI_STA);
  while (true) {
    Serial.printf("##[BOOT]#\tAttempt to connect to %s\n", config.ssids[ls].ssid);
    Serial.print("##[BOOT]#\t");
    display.putRequest(BOOTSTRING, ls);
    WiFi.begin(config.ssids[ls].ssid, config.ssids[ls].password);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
      if(LED_BUILTIN!=255) digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      errcnt++;
      if (errcnt > 16) {
        errcnt = 0;
        ls++;
        if (ls > config.ssidsCount - 1) ls = 0;
        Serial.println();
        break;
      }
    }
    if (WiFi.status() != WL_CONNECTED && ls == startedls) {
      raiseSoftAP();
      Serial.println("##[BOOT]#\tdone");
      return;
    }
    if (WiFi.status() == WL_CONNECTED) {
      config.setLastSSID(ls + 1);
      break; // РѕС‚СЃС‚СЂРµР»СЏР»РёСЃСЊ
    }
  }
  Serial.println(".");
  Serial.println("##[BOOT]#\tdone");
  if(LED_BUILTIN!=255) digitalWrite(LED_BUILTIN, LOW);
  status = CONNECTED;
  WiFi.setSleep(false);
  WiFi.onEvent(WiFiReconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiLostConnection, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  weatherBuf=NULL;
  trueWeather = false;
  #if (DSP_MODEL!=DSP_DUMMY || defined(USE_NEXTION)) && !defined(HIDE_WEATHER)
    weatherBuf = (char *) malloc(sizeof(char) * WEATHER_STRING_L);
    memset(weatherBuf, 0, WEATHER_STRING_L);
  #endif
  
  if(strlen(config.store.sntp1)>0 && strlen(config.store.sntp2)>0){
    configTime(config.store.tzHour * 3600 + config.store.tzMin * 60, config.getTimezoneOffset(), config.store.sntp1, config.store.sntp2);
  }else if(strlen(config.store.sntp1)>0){
    configTime(config.store.tzHour * 3600 + config.store.tzMin * 60, config.getTimezoneOffset(), config.store.sntp1);
  }
  ctimer.attach(1, ticks);
  if (network_on_connect) network_on_connect();
}

void Network::requestTimeSync(bool withTelnetOutput, uint8_t clientId) {
  if (withTelnetOutput) {
    char timeStringBuff[50];
    strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%dT%H:%M:%S", &timeinfo);
    if (config.store.tzHour < 0) {
      telnet.printf(clientId, "##SYS.DATE#: %s%03d:%02d\n> ", timeStringBuff, config.store.tzHour, config.store.tzMin);
    } else {
      telnet.printf(clientId, "##SYS.DATE#: %s+%02d:%02d\n> ", timeStringBuff, config.store.tzHour, config.store.tzMin);
    }
  }
}

void rebootTime() {
  ESP.restart();
}

void Network::raiseSoftAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSsid, apPassword);
  Serial.println("##[BOOT]#");
  BOOTLOG("************************************************");
  BOOTLOG("Running in AP mode");
  BOOTLOG("Connect to AP %s with password %s", apSsid, apPassword);
  BOOTLOG("and go to http:/192.168.4.1/ to configure");
  BOOTLOG("************************************************");
  status = SOFT_AP;
  if(config.store.softapdelay>0)
    rtimer.once(config.store.softapdelay*60, rebootTime);
}

void Network::requestWeatherSync(){
  display.putRequest(NEWWEATHER);
}


void doSync( void * pvParameters ) {
  static uint8_t tsFailCnt = 0;
  //static uint8_t wsFailCnt = 0;
  if(network.forceTimeSync){
    network.forceTimeSync = false;
    if(getLocalTime(&network.timeinfo)){
      tsFailCnt = 0;
      network.forceTimeSync = false;
      mktime(&network.timeinfo);
      display.putRequest(CLOCK);
      network.requestTimeSync(true);
    }else{
      if(tsFailCnt<4){
        network.forceTimeSync = true;
        tsFailCnt++;
      }else{
        network.forceTimeSync = false;
        tsFailCnt=0;
      }
    }
  }
  if(network.weatherBuf && (strlen(config.store.weatherkey)!=0 && config.store.showweather) && network.forceWeather){
    network.forceWeather = false;
    network.trueWeather=getWeather(network.weatherBuf);
  }
  vTaskDelete( NULL );
}

bool getWeather(char *wstr) {
#if (DSP_MODEL!=DSP_DUMMY || defined(USE_NEXTION)) && !defined(HIDE_WEATHER)
  WiFiClient client;
  const char* host  = "api.openweathermap.org";
  
  if (!client.connect(host, 80)) {
    Serial.println("##WEATHER###: connection  failed");
    return false;
  }
  char httpget[250] = {0};
  sprintf(httpget, "GET /data/2.5/weather?lat=%s&lon=%s&units=%s&lang=%s&appid=%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", config.store.weatherlat, config.store.weatherlon, weatherUnits, weatherLang, config.store.weatherkey, host);
  client.print(httpget);
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 2000UL) {
      Serial.println("##WEATHER###: client available timeout !");
      client.stop();
      return false;
    }
  }
  timeout = millis();
  String line = "";
  if (client.connected()) {
    while (client.available())
    {
      line = client.readStringUntil('\n');
      if (strstr(line.c_str(), "\"temp\"") != NULL) {
        client.stop();
        break;
      }
      if ((millis() - timeout) > 500)
      {
        client.stop();
        Serial.println("##WEATHER###: client read timeout !");
        return false;
      }
    }
  }
  if (strstr(line.c_str(), "\"temp\"") == NULL) {
    Serial.println("##WEATHER###: weather not found !");
    return false;
  }

		  Serial.printf("## OPENWEATHERMAP ###: *\n%s,\n*\n", line.c_str());

  char *tmpe;
  char *tmps;
  char *tmpc;
  char *pp = "+";
  const char* cursor = line.c_str();
  char desc[120], temp[20], hum[20], press[20], icon[5], gust[20], porv[10], stanc[50], znakt[5], znakf[5];

  tmps = strstr(cursor, "\"description\":\"");
  if (tmps == NULL) { Serial.println("##WEATHER###: description not found !"); return false;}
  tmps += 15;
  tmpe = strstr(tmps, "\",\"");
  if (tmpe == NULL) { Serial.println("##WEATHER###: description not found !"); return false;}
  strlcpy(desc, tmps, tmpe - tmps + 1);
  cursor = tmps;
  
  // "СЏСЃРЅРѕ","icon":"01d"}],
  tmps = strstr(cursor, "\"icon\":\"");
  if (tmps == NULL) { Serial.println("##WEATHER###: icon not found !"); return false;}
  tmps += 8;
  tmpe = strstr(tmps, "\"}");
  if (tmpe == NULL) { Serial.println("##WEATHER###: icon not found !"); return false;}
  strlcpy(icon, tmps, tmpe - tmps + 1);
  cursor = tmps;
  
  tmps = strstr(cursor, "\"temp\":");
  if (tmps == NULL) { Serial.println("##WEATHER###: temp not found !"); return false;}
  tmps += 7;
  tmpe = strstr(tmps, ",\"");
  if (tmpe == NULL) { Serial.println("##WEATHER###: temp not found !"); return false;}
  strlcpy(temp, tmps, tmpe - tmps + 1);
  cursor = tmps;
  float tempf = atof(temp);
  if (tempf > 0) strlcpy(znakt, pp , sizeof(znakt));		// вписали в znakt ("+")
  else strlcpy(znakt, const_getWeather, sizeof(znakt));	// вписали в znakt ("")

  tmps = strstr(cursor, "\"feels_like\":");
  if (tmps == NULL) { Serial.println("##WEATHER###: feels_like not found !"); return false;}
  tmps += 13;
  tmpe = strstr(tmps, ",\"");
  if (tmpe == NULL) { Serial.println("##WEATHER###: feels_like not found !"); return false;}
  strlcpy(temp, tmps, tmpe - tmps + 1);
  cursor = tmps;
  float tempfl = atof(temp);
  if (tempfl > 0) strlcpy(znakf, pp , sizeof(znakf));		// вписали в znakf ("+")
  else strlcpy(znakf, const_getWeather, sizeof(znakf));	// вписали в znakf ("")
//  (void)tempfl;						// ?

  tmps = strstr(cursor, "\"pressure\":");
  if (tmps == NULL) { Serial.println("##WEATHER###: pressure not found !"); return false;}
  tmps += 11;
  tmpe = strstr(tmps, ",\"");
  if (tmpe == NULL) { Serial.println("##WEATHER###: pressure not found !"); return false;}
  strlcpy(press, tmps, tmpe - tmps + 1);
  cursor = tmps;
  int pressi = (float)atoi(press) / 1.333 - 1.00;		// перевод в мм.рт.ст., ввод в целое число (pressi-21) поправку на выс. местности
  
  tmps = strstr(cursor, "humidity\":");
  if (tmps == NULL) { Serial.println("##WEATHER###: humidity not found !"); return false;}
  tmps += 10;
  tmpe = strstr(tmps, ",\"");
  tmpc = strstr(tmps, "}");
  if (tmpe == NULL) { Serial.println("##WEATHER###: humidity not found !"); return false;}
  cursor = tmps;
  strlcpy(hum, tmps, tmpe - tmps + (tmpc>tmpe?1:(tmpc - tmpe +1)));
  
  tmps = strstr(cursor, "\"grnd_level\":");
  bool grnd_level_pr = (tmps != NULL);
  if(grnd_level_pr){
    tmps += 13;
    tmpe = strstr(tmps, ",\"");
    tmpc = strstr(tmps, "},");						// или адрес до[},]
    if (tmpe == NULL) { Serial.println("##WEATHER###: grnd_level not found ! Use pressure");}
    strlcpy(press, tmps, tmpe - tmps + (tmpc>tmpe?1:(tmpc - tmpe +1)));	// вписали в press строку данных (press="991")
    cursor = tmps;
    pressi = (float)atoi(press) / 1.333;			// преобразовали в целое число, перевели в мм.рт.ст. (pressi=743)
 			 }
  
  tmps = strstr(cursor, "\"speed\":");
  if (tmps == NULL) { Serial.println("##WEATHER###: wind speed not found !"); return false;}
  tmps += 8;
  tmpe = strstr(tmps, ",\"");
  if (tmpe == NULL) { Serial.println("##WEATHER###: wind speed not found !"); return false;}
  strlcpy(temp, tmps, tmpe - tmps + 1);
  cursor = tmps;
  float wind_speed = atof(temp);
//  (void)wind_speed;					// ?
  
  tmps = strstr(cursor, "\"deg\":");
  if (tmps == NULL) { Serial.println("##WEATHER###: wind deg not found !"); return false;}
  tmps += 6;
  tmpe = strstr(tmps, ",\"");
  tmpc = strstr(tmps, "},");				// или адрес до[},]
  if (tmpe == NULL) { Serial.println("## WEATHER ###: deg content not found !"); return false;}
  strlcpy(temp, tmps, tmpe - tmps + (tmpc>tmpe?1:(tmpc - tmpe +1)));	// вписали в temp строку данных (temp="316")
  cursor = tmps;
  int deg = atof(temp);
  int wind_deg = atof(temp)/22.5;		// преобр. в целое число и перевели в полурумбы (wind_deg=14) 
//  if(wind_deg<0) wind_deg = 16+wind_deg;			//отрицательным не бывает
  
  		// Проверяем наличие ["gust":13.09}] и добавляем его целое текстовое в строку gust
  tmps = strstr(cursor, "\"gust\":");			// поиск ["gust":] 7
  strlcpy(gust, const_getWeather, sizeof(gust));	// вписали в gust ("")
  if (tmps == NULL) { Serial.println("## WEATHER ###: gust not found !\n");}
  else {
	  tmps += 7;						// добавили 7
	  tmpe = strstr(tmps, "},");				// до [},]
	  tmpc = strstr(tmps, ",\"");				// адрес до[,"] если таковой здесь будет
	  if (tmpe == NULL) { Serial.println("## WEATHER ###: gust content not found !");}
	  else {
		  strlcpy(temp, tmps, tmpe - tmps + (tmpc>tmpe?1:(tmpc - tmpe +1)));	// вписали в temp текстовую строку (temp="13.09")
		  int gusti = (float)atoi(temp);		// преобразовали в целое число (gusti=13)
		  if (gusti == 0) { Serial.println("## WEATHER ###: gust content is 0 !");}
		  else {
			  strlcpy(gust, prv, sizeof(gust));	// вписали в gust константу *prv (", ПОРЫВЫ ")
			  itoa(gusti, porv, 10);				// преобразовали gusti в текстовую строку (porv="13")
			  strlcat(gust, porv, sizeof(gust));		// добавили к gust текстовую строку porv (", ПОРЫВЫ 13")
			  }
		  }
	 }
  cursor = tmps;

  tmps = strstr(cursor, "\"name\":\"");
  if (tmps == NULL) { Serial.println("##WEATHER###: name station not found !"); return false;}
  tmps += 8;//8
  tmpe = strstr(tmps, "\",\"");
  if (tmpe == NULL) { Serial.println("##WEATHER###: name station not found !"); return false;}
  strlcpy(stanc, tmps, tmpe - tmps+1);		// вписали в stanc метеостанцию +1
  
  #ifdef USE_NEXTION
    nextion.putcmdf("press_txt.txt=\"%dmm\"", pressi);
    nextion.putcmdf("hum_txt.txt=\"%d%%\"", atoi(hum));
    char cmd[30];
    snprintf(cmd, sizeof(cmd)-1,"temp_txt.txt=\"%.1f\"", tempf);
    nextion.putcmd(cmd);
    int iconofset;
    if(strstr(icon,"01")!=NULL)      iconofset = 0;
    else if(strstr(icon,"02")!=NULL) iconofset = 1;
    else if(strstr(icon,"03")!=NULL) iconofset = 2;
    else if(strstr(icon,"04")!=NULL) iconofset = 3;
    else if(strstr(icon,"09")!=NULL) iconofset = 4;
    else if(strstr(icon,"10")!=NULL) iconofset = 5;
    else if(strstr(icon,"11")!=NULL) iconofset = 6;
    else if(strstr(icon,"13")!=NULL) iconofset = 7;
    else if(strstr(icon,"50")!=NULL) iconofset = 8;
    else                             iconofset = 9;
    nextion.putcmd("cond_img.pic", 50+iconofset);
    nextion.weatherVisible(1);
  #endif
  
  Serial.printf("##WEATHER###: descr.: %s, temp: %s%.1fC, pres.: %d mmHg, humidity: %s %%, wind: %s (*%d*) %.0f%s m/s (%s)\n", desc, znakt, tempf, pressi, hum, wind[wind_deg], deg, wind_speed, gust, stanc);
  #ifdef WEATHER_FMT_SHORT
  sprintf(wstr, weatherFmt, tempf, pressi, hum);
  #else
    #if EXT_WEATHER
      sprintf(wstr, weatherFmt, desc, znakt, tempf, znakf, tempfl, pressi, hum, wind[wind_deg], wind_speed, gust, stanc);
    #else
      sprintf(wstr, weatherFmt, desc, tempf, pressi, hum);
    #endif
  #endif
  network.requestWeatherSync();
  return true;
#endif // if (DSP_MODEL!=DSP_DUMMY || defined(USE_NEXTION)) && !defined(HIDE_WEATHER)
  return false;
}
