#include "lcd1602.h"


const uint8_t table_tou[]={0x1f,0x10,0x17,0x17,0x17,0x17,0x10,0xff};
const uint8_t table_zhong_man[]={0x1f,0x00,0xff,0xff,0xff,0xff,0x00,0xff};
const uint8_t table_zhong_kong[]={0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0xff};
const uint8_t table_wei[]={0x18,0x08,0x08,0x08,0x08,0x08,0x08,0x18};


//delay xus,supposs X1=11.0592M Hz 
void delay_xus(uint8_t x) 
{   
	uint8_t i;   
	for(i=x;i>0;i--)  ; 
	
}

//delay xms,supposs X1=11.0592M Hz 
void delay_xms(uint8_t x) 
{
    uint8_t i,j;    
	for(i=240;i>0;i--)    
	for(j=x;j>0;j--)   
	{    ;   }
}

void display_clear()
{
	write_byte(0,0x01); //Clear SCREEN.
}

//init lcd  
void init_lcd() {
    //init for 8bits data  
  	PORTD=0x0;
	
     LCD_EN_DDR|=LCD_EN;//设定该引脚方向为输出
	 LCD_RS_DDR|=LCD_RS;//设定该引脚方向为输出
	 LCD_D4_DDR|=LCD_D4;//设定数据口为输出 
	 LCD_D5_DDR|=LCD_D5;//设定数据口为输出  
	 LCD_D6_DDR|=LCD_D6;//设定数据口为输出  
	 LCD_D7_DDR|=LCD_D7;//设定数据口为输出
	 
	  LCD_EN_PORT&=~LCD_EN;//EN设为低电平
	  LCD_RS_PORT&=~LCD_RS;//RS设为低电平
	  LCD_D4_PORT&=~LCD_D4;//数据口置低电平
	  LCD_D5_PORT&=~LCD_D5;//数据口置低电平
	  LCD_D6_PORT&=~LCD_D6;//数据口置低电平
	  LCD_D7_PORT&=~LCD_D7;//数据口置低电平
	  
	//init lcd for 4bits data  
	 delay_xms(15);//delay 15ms   
	 write_byte(0,0x03);//do not test busy signal   
	 delay_xms(5);//delay 5ms   
	 write_byte(0,0x03);//do not test busy signal  
	 delay_xms(5);//delay 5ms  
	 write_byte(0,0x03);//do not test busy signal   
	 delay_xms(5); //delay 5ms  
	 write_byte(0,0x02);//set display mode  
	 delay_xms(5); //delay 5ms   
	 write_byte(0,0x28);

     delay_xms(5); //delay 5ms  
	 //16X2,5*7 dots, 4 Port Data    
	 write_byte(0,0x08);//close display  
	 write_byte(0,0x01);//clear display  
	 write_byte(0,0x06);// display cursor  
	 write_byte(0,0x0c);//open display 
}

//write one byte in 8bits data   
void write_byte(uint8_t com,uint8_t x) {  

    if (com == 0)
   	LCD_RS_PORT &= ~LCD_RS;   //RS=0  写地址
	else
    LCD_RS_PORT |= LCD_RS;    //RS=1 写数据
	
    LCD_write_DATA_PORT(x);//写高4位
    LCD_write_DATA_PORT(x<<4);//写低4位 
}
void LCD_write_DATA_PORT(unsigned char data)
{   
    if((data>>4)%2)
	  LCD_D4_PORT |=LCD_D4;
	else
	  LCD_D4_PORT &=~LCD_D4;
	
	if((data>>5)%2)
	  LCD_D5_PORT |=LCD_D5;
	else
	  LCD_D5_PORT &=~LCD_D5;
	  
	if((data>>6)%2)
	  LCD_D6_PORT |=LCD_D6;
	else
	  LCD_D6_PORT &=~LCD_D6;
	  
	if((data>>7)%2)
	  LCD_D7_PORT |=LCD_D7;
	else
	  LCD_D7_PORT &=~LCD_D7;
	  
	  LCD_EN_PORT |= LCD_EN; //EN端产生一个由低电平变高电平，写LCD
	  delay_xms(20);
      LCD_EN_PORT &= ~LCD_EN; //EN端产生一个由高电平变低电平，写LCD
	  delay_xus(50);
}
//write address  
void write_address(uint8_t x,uint8_t y) {
// for 0802  
//write address   
if((x<=16)&(y==0))   
	write_byte(0,x+0x80);  
//else if((x>7)&(y==0))   
//	write_byte(0,(x-8)+0xc0);  
else    
	write_byte(0,x+0xc0);  
	delay_xus(100);//delay 100us
}

//display one byte    
void display_one_byte(uint8_t byte,uint8_t x,uint8_t y ) {   
	write_address(x,y); //write address  
	write_byte(1,byte);//write data  
	delay_xus(100);//delay 100us 
}

//display one string  
void display_one_array(uint8_t *array,uint8_t n,uint8_t x,uint8_t y) {
   uint8_t i;  // write_address(x,y); //write address
	for(i=0;i<n;i++)  
	{   write_address(x+i,y); //write address   
		write_byte(1,array[i]);//write data   
	}  
}

//display one string  
void display_one_string(uint8_t *str,uint8_t x,uint8_t y) {
   uint8_t i=0;//define i and init i=0  
   while(*str)  {   
	   write_address(x+i,y); //write address   
	   write_byte(1,*str);//write data    
	   str++;//   
	   i++;  //  
   } 
}

//display one unsigned int num from 0 to 65,535  
void display_one_int_num(uint16_t num,uint8_t x,uint8_t y) 
{
   uint8_t i,j,wei,str[5];   
   uint16_t q; //attention:q from 1 to10000  
   //judge num has how much wei  
   if(num>=10000) 
   	wei=5;  
   else if(num>=1000) 
   	wei=4;  
   else if(num>=100) 
   	wei=3;  
   else if(num>=10) 
   	wei=2;  
   else 
   	wei=1;
	  
   for(i=wei;i>0;i--)  
   {   q=1;   
       for(j=1;j<i;j++)    
	   	q*=10;  
	   str[wei-i]=num/q+'0';   
		num%=q;  
	}   
	display_one_array(str,wei,x,y);  
}

//display one  num with 2 decimal places,the num<655
void display_one_float_num(float num,uint8_t x,uint8_t y)
{  
	 uint8_t i,j,wei,str[6];  
	 uint16_t temp,q; //attention:q from 1 to10000  
	 //judge num has how much wei  
	 temp=(uint16_t)(num*100);  
	 if(temp>=10000) 
	 	wei=5;  
	 else if(temp>=1000) 
	 	wei=4;  
	 else if(temp>=100) 
	 	wei=3;  
	 else if(temp>=10) 
	 	wei=2; 
	  else 
	  	wei=1;   
	  
	 for(i=wei;i>2;i--)  {   
	  q=1;   
	  for(j=1;j<i;j++)    
		  q*=10;   
	  str[wei-i]=temp/q+'0';   
	  temp%=q;  
	  }   
	  str[wei-2]='.';   
	  str[wei-1]=temp/10+'0';  
	  str[wei]=  temp%10+'0';   
	  display_one_array(str,wei+1,x,y); 
  
 }

void store_char(uint8_t addr,uint8_t length,const uint8_t *pbuf)
{
 uint8_t i;
 write_byte(0,addr);
 for(i=0;i<length;i++)
 {
  write_byte(1,pbuf[i]);
 }
}

void init_CGRAM()
{
  uint8_t j;
 //store_char(0x40, 8, xin);
 //write_byte(0, 0x80);
 write_byte(0, 0x60);
 for(j=0;j<8;j++)
        write_byte(1, table_tou[j]);
 write_byte(0, 0x68);
 for(j=0;j<8;j++)
        write_byte(1, table_zhong_man[j]); 
 write_byte(0, 0x70);
 for(j=0;j<8;j++)
        write_byte(1, table_zhong_kong[j]); 
 write_byte(0, 0x78);
 for(j=0;j<8;j++)
        write_byte(1, table_wei[j]);		
}

void dis_progress(uint8_t X)
{
 uint8_t i,a;
 i=X/2;
 write_byte(0, 0xc0);
 write_byte(1, 0x04);
 for(a=1;a<=i;a++)
   write_byte(1, 0x05);  
 for(a=1;a<=(14-i);a++)
   write_byte(1, 0x06);
   
  write_byte(1, 0x07); 
}

