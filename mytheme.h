#ifndef _my_theme_h
#define _my_theme_h

#define ENABLE_THEME
#ifdef  ENABLE_THEME

/*----------------------------------------------------------------------------------------------------------------*/
/*       | COLORS             |   values (0-255)  |                                                               */
/*       | color name         |    R    G    B    |                                                               */
/*----------------------------------------------------------------------------------------------------------------*/
//#define COLOR_BACKGROUND           54, 54,   54     /*  background                                                */
//#define COLOR_STATION_NAME          0,   0,   0     /*  station name                                              */
#define COLOR_STATION_NAME        255, 142,   0     /*  station name background                                   */
#define COLOR_STATION_BG           22,  26,  30     /*  station name background                                   */
//#define COLOR_STATION_FILL        150, 150,  50     /*  station name fill background                              */
#define COLOR_STATION_FILL         22,  26,  30     /*  station name fill background                              */
#define COLOR_SNG_TITLE_1         250, 250, 100    /*  first title                                               */
#define COLOR_SNG_TITLE_2         255, 200, 55     /*  second title                                              */
#define COLOR_WEATHER             238, 221, 130     /*  weather string                                            */
#define COLOR_VU_MAX                0,   0, 250     /*  max of VU meter                                           */
#define COLOR_VU_MIN                0, 200, 200     /*  min of VU meter                                           */
#define COLOR_CLOCK                 0, 255,   0     /*  clock color        */
#define COLOR_SECONDS               0, 255,   0     /*  seconds color (DSP_ST7789, DSP_ILI9341, DSP_ILI9225)      */
#define COLOR_DAY_OF_W            250, 250, 200     /*  day of week color (DSP_ST7789, DSP_ILI9341, DSP_ILI9225)  */
#define COLOR_DATE                  0, 200,  35     /*  date color (DSP_ST7789, DSP_ILI9341, DSP_ILI9225)         */
#define COLOR_CLOCK_BG              0,   0,   0                                       
//#define COLOR_CLOCK_BG          162, 205,  90                                       
#define COLOR_HEAP                255,  69,  30      /*  heap fill                                               */
#define COLOR_HEAPBAROUT          255, 182, 193    /*  heapbar outline                                               */
#define COLOR_IP                  255, 168, 162     /*  ip address                                                */
#define COLOR_VOLUME_VALUE          0, 250, 255     /*  volume string (DSP_ST7789, DSP_ILI9341, DSP_ILI9225)      */
#define COLOR_RSSI                255, 168, 162     /*  rssi                                                      */
#define COLOR_VOLBAR_OUT           0, 250,  255     /*  volume bar outline                                        */
#define COLOR_VOLBAR_IN            0, 250,  255    /*  volume bar fill                                           */
#define COLOR_DIGITS               0, 250,  255     /*  volume / station number                                   */
#define COLOR_DIVIDER              0, 255,  100     /*  divider color (DSP_ST7789, DSP_ILI9341, DSP_ILI9225)      */
#define COLOR_BITRATE            255, 168,  162     /*  bitrate                                                   */
#define COLOR_PL_CURRENT         250, 250,  250     /*  playlist current item                                     */
#define COLOR_PL_CURRENT_BG      150, 150,    0      /*  playlist current item background                          */
#define COLOR_PL_CURRENT_FILL      0,   0,    0     /*  playlist current item fill background                     */
#define COLOR_PLAYLIST_0          165, 165, 165     /*  playlist string 0                                         */
#define COLOR_PLAYLIST_1          145, 145, 145     /*  playlist string 1                                         */
#define COLOR_PLAYLIST_2          120, 120, 120     /*  playlist string 2                                         */
#define COLOR_PLAYLIST_3          100, 100, 100     /*  playlist string 3                                         */
#define COLOR_PLAYLIST_4           80,  80,  80     /*  playlist string 4                                         */

#endif  /* #ifdef  ENABLE_THEME */
#endif  /* #define _my_theme_h  */
