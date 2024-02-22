/**************************************************************************
    Плагин тушения подсветки.
    Версия 0.2 (Maleksm) для ёРадио 21.10.2023
***************************************************************************/
#include <Ticker.h>

/* Основные константы настроек */
const uint8_t brightness_down_level = 2; 	/* Нижний уровень яркости (от 0 до 255) */
const uint16_t Out_Interval = 60; 	      /* Интервал для тушения подсветки в секундах (60 sec = 1 min) */

#if BRIGHTNESS_PIN!=255
  Ticker backlightTicker;

  void backlightDown()        /* Функция тушения подсветки */
  {
  if(network.status!=SOFT_AP){
    backlightTicker.detach();
    int current_brightness = map(config.store.brightness, 0, 100, 0, 255);
    while (true) {
      if (current_brightness <= (brightness_down_level) - 5) { analogWrite(BRIGHTNESS_PIN, brightness_down_level); break; }
      else {
        current_brightness = current_brightness - 5;
        analogWrite(BRIGHTNESS_PIN, current_brightness);
        delay(100);
           }
                  }
                                       }
  }

  void brightnessOn()          /* Функция включения подсветки */
  { backlightTicker.detach();
    analogWrite(BRIGHTNESS_PIN, map(config.store.brightness, 0, 100, 0, 255));
    backlightTicker.attach(Out_Interval, backlightDown);
  }

  void yoradio_on_setup() { brightnessOn(); }       /* Включение подсветки при регулировке в Setup */

  void player_on_track_change() { brightnessOn(); }    /* Включение подсветки при смене трека */

  void player_on_start_play() { brightnessOn(); }     /* Включение подсветки при запуске воспроизведения */

  void player_on_stop_play() { brightnessOn(); }      /* Включение подсветки при остановке воспроизведения */

  void ctrls_on_loop() {                              /* Включение подсветки при регулирующих операциях */
    static uint32_t prevBlPinMillis;
    if((display.mode()!=PLAYER) && (millis()-prevBlPinMillis>1000))
    { prevBlPinMillis=millis();
      brightnessOn(); }
                     }
#endif  /*  #if BRIGHTNESS_PIN!=255 */
