#include "stm32f1xx_hal.h"  //Esto debo cambiarlo cuando use la bluepill, aqui
//Yo antes habi escrito "stm32f4xx_hal_confi.h" como el archivo .h pero me trajo errores
//Copie el del vidio y a partir de ahi quedo, igual en el caso de cambiar de placa de
//desarrollo debo cambiar el modelo

void lcd_init (void); // iniciliza mi lcd

void lcd_send_cmd(char cmd); //manda comando a lcd

void lcd_send_data(char data); //mando data al lcd

void lcd_send_string (char *str); //manod string al lcd

void lcd_put_cur(int row, int col); //pone el cursor en la posciison (0 or 1), columna (0-15)

void lcd_clear(void); //Limpia el LCD


