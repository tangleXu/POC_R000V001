//ICC-AVR application builder : 2017/11/5 14:26:21
// Target : M8
// Crystal: 8.0000Mhz


#include "oled.h"
#include "bmp.h" 

void do_LSCELLINFO();

char g_MCC[10]="460";
char g_MNC[10]="012";
char g_EARFCN[10]="1058";
char g_GCELLID[10]="1015";
char g_TAC[10]="256";
char g_CAT[10]="255";
char g_SINR[10]="5.1";
char g_BAND[10]="8";
char g_PCI[10]="20"; 
char g_RSRP[10]="-30";
char g_RSRQ[10]="-50";
char g_RSSI[10]="-90";

#define USART_REC_LEN 100
#define NULL 0

u8 USART_RX_BUF[USART_REC_LEN]={0};

u16 USART_RX_STA=0;
u16 g_cnt=0;

 char* strstr( char *str1,  char *str2)
{     
      while(*str1 != '\0')
      {
           char *p = str1;
           char *q = str2;
           char *res = NULL;
          if(*p == *q)
          {
                res = p;
                while(*p && *q && *p++ == *q++)
                ;
                
                if(*q == '\0')
                      return res;                    
          }
          str1++;
      }
      return NULL;
}

 char *my_strstr( char *str,  char *sub_str)
{
 	int i;
	for( i = 0; str[i] != '\0'; i++)
	{
		int tem = i; //tem保留主串中的起始判断下标位置 
		int j = 0;
		while(str[i++] == sub_str[j++])
		{
			if(sub_str[j] == '\0')
			{
				return &str[tem];
			}
		}
		i = tem;
	}

	return NULL;
}


int parseCellInfo(unsigned char *SRC_BUF, unsigned char *ITEM, char *target)
{
	char *p;
	int idx=0;
	int i;
	
	p = my_strstr(SRC_BUF, ITEM);
	if(p != NULL) {
		for(i=0;i<10;i++) target[i]=0;
		idx = 0;
		while(*p!=':') p++; p++;
		while(*p!=' '&&*p!='\r'){
			target[idx++]=*p++;
			//printf("%c",*p);
		}
		target[idx]='\0';
		
		//printf("%s=%s\n",ITEM,target);
		return 1;
	}else
		return 0;
}

void USART_Transmit(unsigned char data) 
{
  while(!(UCSRA&(1<<UDRE)));
  UDR=data; 
 
}
void USART_Transmit_string(unsigned char *ptr)
{
 while (*ptr)
  {
    USART_Transmit(*ptr++);
  }
 //USART_Transmit(0x0D);
 //USART_Transmit(0x0A);
}

void port_init(void)
{
 PORTB = 0x00;
 DDRB  = 0xFF;
 PORTC = 0x00; //m103 output only
 DDRC  = 0x00;
 PORTD = 0x00;
 DDRD  = 0x00;
}

//UART0 initialize
// desired baud rate: 9600
// actual: baud rate:9615 (0.2%)
void uart1_init(void)
{
 UCSRB = 0x00; //disable while setting baud rate
 UCSRA = 0x00;
 UCSRC = BIT(URSEL) | 0x06;
 UBRRL = 0x33; //set baud rate lo
 UBRRH = 0x00; //set baud rate hi
 UCSRB = 0x98;
}

//UART0 initialize
// desired baud rate: 115200
// actual: baud rate:115198 (0.0%)
void uart0_init(void)
{
 UCSRB = 0x00; //disable while setting baud rate
 UCSRA = 0x00;
 UCSRC = BIT(URSEL) | 0x06;
 UBRRL = 0x05; //set baud rate lo
 UBRRH = 0x00; //set baud rate hi
 UCSRB = 0x98;
}


u8 find_str(u8 *s, u8 *t)//óDò???·μ???μ￡?0/1
{
	u8 *s_temp;
	u8 *m_temp;
	u8 *t_temp;
	
	if (s == 0 ||t == 0)
		return 0;

	for (s_temp = s; *s_temp != '\0'; s_temp++)
	{
		m_temp = s_temp;
		for (t_temp = t; *t_temp == *m_temp; t_temp++, m_temp++)//?e???D??ê?·??àí?
			if (*t_temp == '\0') //ê?·?ê??áê?·?		 
				return 1;
	}
	return 0;
}

void check_cellinfo()
{
				parseCellInfo(USART_RX_BUF, "EARFCN", g_EARFCN);
				parseCellInfo(USART_RX_BUF, "GCELLID", g_GCELLID);
				parseCellInfo(USART_RX_BUF, "TAC", g_TAC);
				parseCellInfo(USART_RX_BUF, "MCC", g_MCC);
				parseCellInfo(USART_RX_BUF, "MNC", g_MNC);
				parseCellInfo(USART_RX_BUF, "SINR", g_SINR);
				parseCellInfo(USART_RX_BUF, "CAT", g_CAT);
				parseCellInfo(USART_RX_BUF, "BAND", g_BAND);
				parseCellInfo(USART_RX_BUF, "PCI:", g_PCI);
				parseCellInfo(USART_RX_BUF, "RSRP:", g_RSRP);
				parseCellInfo(USART_RX_BUF, "RSRQ:", g_RSRQ);
				parseCellInfo(USART_RX_BUF, "RSSI:", g_RSSI);	
}

#pragma interrupt_handler uart0_rx_isr:iv_USART0_RXC
void uart0_rx_isr(void)
{
 //u8 res;
 u8 i;
  UCSRB &= ~(1 << RXCIE); //Disable interrupt.
 while(!(UCSRA&BIT(RXC)));
 //Shift to left
 for(i=0;i<USART_REC_LEN-1;i++)
 {
  USART_RX_BUF[i]=USART_RX_BUF[i+1];
 }
 USART_RX_BUF[USART_REC_LEN-1]=UDR;
 g_cnt++;
 if(g_cnt==USART_REC_LEN)
 	{		  check_cellinfo();
			  g_cnt=0;
	}
  UCSRB |=  (1 << RXCIE);         //打开串口中断

 /*
 while(!(UCSRA&BIT(RXC))); 
 //uart has received a character in UDR
 if(USART_RX_STA < USART_REC_LEN)
 {
   res = UDR;
   
   if(res == '\n')// && USART_RX_BUF[USART_RX_STA-1]=='\r')
   {
   	USART_RX_BUF[USART_RX_STA]=res;
	USART_RX_STA|=1<<15;
	USART_Transmit_string(USART_RX_BUF);
	do_LSCELLINFO();
   }
	
	USART_RX_BUF[USART_RX_STA++]=res;
		
 }else
 {
       //The buffer is full.
 	   //USART_RX_STA=0;
	   
	   USART_RX_STA|=1<<15;
 } */
}


//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 CLI(); //disable all interrupts
 port_init();
 uart0_init();
 OLED_Init();			//初始化OLED 
 
 MCUCR = 0x00;
 GICR  = 0x00;
 TIMSK = 0x00; //timer interrupt sources
 SEI(); //re-enable interrupts
 //all peripherals are now initialized
}

void main()
{
     unsigned char t,i;
	 
     init_devices();
	 USART_Transmit_string("AT\r\n"); 
	 delay_ms(500);
	 USART_Transmit_string("AT+IPR=9600\r\n");
	 delay_ms(500);
	 uart1_init();
	 delay_ms(1000);
	 PORTC&=~(1<<4);
	 delay_ms(500);
	 PORTC|=(1<<4);
	 
	 OLED_Clear();	
	 //OLED_DrawBMP(0,0,128,8,BMP2);
	 OLED_ShowString(30,4,"ERICSSON");
	 delay_ms(500);
	 
 	 while(1)
	 {
	   USART_Transmit_string("AT+LSCELLINFO\r\n"); 
	    //for(i=0;i<USART_REC_LEN;i++)
		//	USART_Transmit(USART_RX_BUF[i]);  
		t++;
		if(t>'~')t=' ';
//		OLED_ShowNum(103,6,t,3,16);//显示ASCII字符的码值 	
			
//		delay_ms(100);
		OLED_Clear();
		if(t%5==0)
		OLED_DrawBMP(2,0,20,2,ANTANA_FULL01);
		else if(t%5==1)
		OLED_DrawBMP(2,0,20,2,ANTANA_FULL02);
		else if(t%5==2)
		OLED_DrawBMP(2,0,20,2,ANTANA_FULL03);
		else if(t%5==3)
		OLED_DrawBMP(2,0,20,2,ANTANA_FULL04);
		else
		OLED_DrawBMP(2,0,20,2,ANTANA_FULL05);
		
		OLED_DrawBMP(24,0,30,2,BLUETOOTH_ICON);
		OLED_DrawBMP(40,0,70,2,NBIOT_ICON);
		
		if(t%5==0)
		OLED_DrawBMP(106,0,106+21,2,BAT_ICON0);
		else if(t%5==1)
		OLED_DrawBMP(106,0,106+21,2,BAT_ICON1);
		else if(t%5==2)
		OLED_DrawBMP(106,0,106+21,2,BAT_ICON2);
		else if(t%5==3)
		OLED_DrawBMP(106,0,106+21,2,BAT_ICON3);
		else
		OLED_DrawBMP(106,0,106+21,2,BAT_ICON4);
		
		OLED_DrawBMP(85,0,85+16,2,LOGO_5G);

		OLED_ShowCHinese(18+0,3,0);//中
		OLED_ShowCHinese(18+18,3,1);//景
		OLED_ShowCHinese(18+36,3,2);//园
		OLED_ShowCHinese(18+54,3,3);//电
		OLED_ShowCHinese(18+72,3,4);//子
		
		OLED_ShowString(0,6,"NB-IoT Utility");
		
		//do_LSCELLINFO();
		
		delay_ms(4000);
		OLED_Clear();
		OLED_DrawBMP(2,0,20,2,ANTANA_FULL05);
		OLED_DrawBMP(40,0,70,2,NBIOT_ICON);
		OLED_DrawBMP(106,0,106+21,2,BAT_ICON4);
		OLED_DrawBMP(85,0,85+16,2,LOGO_5G);
		OLED_DrawBMP(24,0,30,2,BLUETOOTH_ICON);
		
		OLED_ShowString(0,2,"MCC:");
		OLED_ShowString(30,2,g_MCC);

		OLED_ShowString(70,2,"MNC:");
		OLED_ShowString(100,2,g_MNC);

		OLED_ShowString(0,4,"EARFCN:");
		OLED_ShowString(60,4,g_EARFCN);

		OLED_ShowString(0,6,"GCELLID:");
		OLED_ShowString(70,6,g_GCELLID);
						
		delay_ms(3500);
		OLED_Clear();
		OLED_DrawBMP(2,0,20,2,ANTANA_FULL05);
		OLED_DrawBMP(40,0,70,2,NBIOT_ICON);
		OLED_DrawBMP(106,0,106+21,2,BAT_ICON4);
		OLED_DrawBMP(85,0,85+16,2,LOGO_5G);
		OLED_DrawBMP(24,0,30,2,BLUETOOTH_ICON);
		
		OLED_ShowString(0,2,"TAC:");
		OLED_ShowString(30,2,g_TAC);

		OLED_ShowString(60,2,"CAT:");
		OLED_ShowString(100,2,g_CAT);

		OLED_ShowString(0,4,"SINR:");
		OLED_ShowString(40,4,g_SINR);

		OLED_ShowString(70,4,"BAND:");
		OLED_ShowString(110,4,g_BAND);
		
		OLED_ShowString(0,6,"PCI:");
		OLED_ShowString(30,6,g_PCI);
		
		delay_ms(3500);	
		OLED_Clear();
		OLED_DrawBMP(2,0,20,2,ANTANA_FULL05);
		OLED_DrawBMP(40,0,70,2,NBIOT_ICON);
		OLED_DrawBMP(106,0,106+21,2,BAT_ICON4);
		OLED_DrawBMP(85,0,85+16,2,LOGO_5G);
		OLED_DrawBMP(24,0,30,2,BLUETOOTH_ICON);
				
		OLED_ShowString(0,2,"RSRP:");
		OLED_ShowString(50,2,g_RSRP);		

		OLED_ShowString(0,4,"RSRQ:");
		OLED_ShowString(50,4,g_RSRQ);
		
		OLED_ShowString(0,6,"RSSI:");
		OLED_ShowString(50,6,g_RSSI);
				
		delay_ms(3500);
		
		//OLED_DrawBMP(0,0,128,8,BMP2);
		//delay_ms(1500);
	 }
}
