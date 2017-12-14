/************************************************ *以下是1602头文件  *************************************************/ 
/**    
******************************************************   
* @file    lcd1602.h    
* @brief   This file contains all  lcd1602 functions declaration.  
**/    
#ifndef __LCD1602_H 
#define __LCD1602_H 

#include <iom8v.h>
#include <macros.h>

#define uint8_t unsigned char 
#define uint16_t unsigned int

//LCD1602/0802 define 
//定义MCU与LCD的接口
#define LCD_EN_PORT    PORTD
#define LCD_EN_DDR     DDRD
#define LCD_RS_PORT    PORTD
#define LCD_RS_DDR     DDRD
#define LCD_D4_PORT    PORTD
#define LCD_D4_DDR     DDRD
#define LCD_D5_PORT    PORTD
#define LCD_D5_DDR     DDRD
#define LCD_D6_PORT    PORTD
#define LCD_D6_DDR     DDRD
#define LCD_D7_PORT    PORTD
#define LCD_D7_DDR     DDRD

#define LCD_EN         0x40   //PORTD6       
#define LCD_RS         0x80   //PORTD7    
//#define LCD_D4         0x20   //PORTD5        
//#define LCD_D5         0x10   //PORTD4        
//#define LCD_D6         0x08   //PORTD3      
//#define LCD_D7         0x04   //PORTD2  

#define LCD_D4         0x04   //PORTD2         
#define LCD_D5         0x08   //PORTD3         
#define LCD_D6         0x10   //PORTD4     
#define LCD_D7         0x20   //PORTD5 
 
//#define LCD_RW 0 
//Only Write 
//define LCD_PORT P0   
//input mode set  
#define LCD_AC_AUTO_INCREMENT  0x06//AC auto increment 
#define LCD_AC_AUTO_DECREMENT  0x04//AC auto decrement 
#define LCD_MOVE_ENABLE 0x05  //picture move  
#define LCD_MOVE_DISABLE 0x04// picture do not move

#define LCD_GO_HOME 0x02//AC=0;reset picture and cursor   

//output mode set  
#define LCD_DISPLAY_ON 0x0c// open display 
#define LCD_CURSOR_ON 0x0f //open cursor  
#define LCD_CURSOR_BLINK_ON 0x0a//cursor flash 
#define LCD_CURSOR_OFF 0x08  //close display  
#define LCD_LEFT_MOVE 0x18  //lcd left move one bit 
#define LCD_RIGHT_MOVE 0x1c //lcd right move one bit  
#define LCD_CURSOR_LEFT_MOVE 0x10//lcd cursor left move one bit 
#define LCD_CURSOR_RIGHT_MOV+E 0x14 //lcd cursor right move one bit 
#define LCD_DISPLAY_2L8B  0x38 //8 data bits 2*16 
#define LCD_DISPLAY_2L4B  0x28 //4 data bits 2*16 
#define LCD_DISPLAY_1L8B  0x30 //8 data bits 1*16 
#define LCD_DISPLAY_1L4B  0x20 //4 data bits 1*16 
#define LCD_CLEAR_SCREEN 0x01 //clear screen 
  
void init_lcd();  //init lcd  
void write_byte(uint8_t com,uint8_t x); //write one byte   
void write_address(uint8_t x,uint8_t y); //write address            
void display_one_byte(uint8_t byte,uint8_t x,uint8_t y); //display one byte   
void display_one_array(uint8_t *array,uint8_t n,uint8_t x,uint8_t y); //display one string  
void display_one_string(uint8_t *str,uint8_t x,uint8_t y); //display one string  
void display_one_int_num(uint16_t num,uint8_t x,uint8_t y); //display one int num from 0 to 65,535  
void display_one_float_num(float num,uint8_t x,uint8_t y); //display one  num with 2 decimal places,the num<655 
void delay_xms(uint8_t x);  //delay  x ms,delay a long time 
void delay_xus(uint8_t x);  // delay x us,delay a short time   
void display_clear();
void LCD_write_DATA_PORT(unsigned char data);
void store_char(uint8_t addr,uint8_t length,const uint8_t *pbuf);
void dis_progress(uint8_t X);
void init_CGRAM();

#endif 