#include <Ticker.h>

const uint8_t brightness = 100;           /* Normal brightness level on wake up (100%) */
const uint8_t min_brightness = 20;        /* Minimum brightness level when fading (25%) */
const uint16_t turnBlOffInterval = 30;   /* 1 min = 60 sec */

Ticker backlightTicker;

void backlightMin(){
    backlightTicker.detach();
    analogWrite(BRIGHTNESS_PIN, map(min_brightness, 0, 100, 0, 255));
}

void yoradio_on_setup() {
  backlightTicker.attach(turnBlOffInterval, backlightMin);
}

// Расскоментировать эту функцию, если хотите, чтобы экран
// автоматически "просыпался" при смене каждого трека во время вещания.
// Тогда рекомендую уменьшить turnBlOffInterval до 60 секунд. 

void player_on_track_change(){
analogWrite(BRIGHTNESS_PIN, map(brightness, 0, 100, 0, 255));
backlightTicker.detach();
backlightTicker.attach(turnBlOffInterval, backlightMin);
}

void player_on_start_play(){
  analogWrite(BRIGHTNESS_PIN, map(brightness, 0, 100, 0, 255));
  backlightTicker.detach();
  backlightTicker.attach(turnBlOffInterval, backlightMin);
}

void player_on_stop_play(){
  analogWrite(BRIGHTNESS_PIN, map(brightness, 0, 100, 0, 255));
  backlightTicker.detach();
  backlightTicker.attach(turnBlOffInterval, backlightMin);
}

void ctrls_on_loop() {
  static uint32_t prevBlPinMillis;
  if((display.mode()!=PLAYER) && (millis()-prevBlPinMillis>1000)){
    prevBlPinMillis=millis();
    analogWrite(BRIGHTNESS_PIN, map(brightness, 0, 100, 0, 255));
    backlightTicker.detach();
    backlightTicker.attach(turnBlOffInterval, backlightMin);
  }
}
