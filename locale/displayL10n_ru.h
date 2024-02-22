#ifndef dsp_full_loc
#define dsp_full_loc
#include <pgmspace.h>
/*************************************************************************************
    HOWTO:
    Copy this file to yoRadio/locale/displayL10n_custom.h
    and modify it
*************************************************************************************/
const char mon[] PROGMEM = "пнд";
const char tue[] PROGMEM = "втр";
const char wed[] PROGMEM = "срд";
const char thu[] PROGMEM = "чтв";
const char fri[] PROGMEM = "птн";
const char sat[] PROGMEM = "суб";
const char sun[] PROGMEM = "вск";

const char monf[] PROGMEM = "понедельник";
const char tuef[] PROGMEM = "вторник";
const char wedf[] PROGMEM = "среда";
const char thuf[] PROGMEM = "четверг";
const char frif[] PROGMEM = "пятница";
const char satf[] PROGMEM = "суббота";
const char sunf[] PROGMEM = "воскресенье";

const char jan[] PROGMEM = "января";
const char feb[] PROGMEM = "февраля";
const char mar[] PROGMEM = "марта";
const char apr[] PROGMEM = "апреля";
const char may[] PROGMEM = "мая";
const char jun[] PROGMEM = "июня";
const char jul[] PROGMEM = "июля";
const char aug[] PROGMEM = "августа";
const char sep[] PROGMEM = "сентября";
const char oct[] PROGMEM = "октября";
const char nov[] PROGMEM = "ноября";
const char dec[] PROGMEM = "декабря";

const char wn_N[]      PROGMEM = "северный";
const char wn_NE[]     PROGMEM = "северо-восточный";
const char wn_E[]      PROGMEM = "восточный";
const char wn_SE[]     PROGMEM = "юго-восточный";
const char wn_S[]      PROGMEM = "южный";
const char wn_SW[]     PROGMEM = "юго-западный";
const char wn_W[]      PROGMEM = "западный";
const char wn_NW[]     PROGMEM = "северо-западный";
const char prv[]    PROGMEM = ", Порывы ";

const char* const dow[]     PROGMEM = { sun, mon, tue, wed, thu, fri, sat };
const char* const dowf[]    PROGMEM = { sunf, monf, tuef, wedf, thuf, frif, satf };
const char* const mnths[]   PROGMEM = { jan, feb, mar, apr, may, jun, jul, aug, sep, oct, nov, dec };
const char* const wind[]    PROGMEM = { wn_N, wn_NE, wn_NE, wn_NE, wn_E, wn_SE, wn_SE, wn_SE, wn_S, wn_SW, wn_SW, wn_SW, wn_W, wn_NW, wn_NW, wn_NW, wn_N };

const char    const_PlReady[]    PROGMEM = "[Готов]";
const char  const_PlStopped[]    PROGMEM = "[Остановлено]";
const char  const_PlConnect[]    PROGMEM = "[Соединение]";
const char  const_DlgVolume[]    PROGMEM = "Громкость";
const char    const_DlgLost[]    PROGMEM = "Отключено";
const char  const_DlgUpdate[]    PROGMEM = "Обновление";
const char const_DlgNextion[]    PROGMEM = "NEXTION";
const char const_getWeather[]    PROGMEM = "";

const char        apNameTxt[]    PROGMEM = "Точка доступа";
const char        apPassTxt[]    PROGMEM = "Пароль";
const char       bootstrFmt[]    PROGMEM = "Соединяюсь с %s";
const char        apSettFmt[]    PROGMEM = "Настройки: HTTP://%s/";
#if EXT_WEATHER
// const char       weatherFmt[]    PROGMEM = "%s \007 Температура: %s%.1f\011C \007 Ощущается как %s%.0f\011C \007 Давление: %d мм \007 Влажность: %s%% \007 Ветер %s% m/c.0f%s \007 %s"  ;
const char       weatherFmt[]    PROGMEM = "%s \007 Температура: %s%.1f\011C (Ощущается как %s%.0f\011C) \007 Давление: %d мм \007 Влажность: %s%% \007 Ветер %s %.0f%s м/с \007 м.ст. %s";

#else
const char       weatherFmt[]    PROGMEM = "%s, %.1f\011C \007 Давление: %d mm \007 Влажность: %s%%";
#endif
const char     weatherUnits[]    PROGMEM = "metric";   /* standard, metric, imperial */
const char      weatherLang[]    PROGMEM = "ru";       /* https://openweathermap.org/current#multi */

#endif
