//ICC-AVR application builder : 2017/11/23 0:25:20
// Target : M8
// Crystal: 8.0000Mhz

#include "lcd1602.h"
uint16_t temp=6;
float temp1=12.352;

void do_LSCELLINFO();

uint8_t gFSM = 0;

uint8_t ericsson[]={"    ERICSSON    "};
uint8_t PING_DNS[]={"AT+LSPING=\"114.114.114.114\"\r\n"};
uint8_t tool_name[]={" NB-IoT Utility "};

const uint8_t xin[]={0x00,0x1B,0x1F,0x1F,0x1F,0x0E,0x04,0x00};

char g_MCC[10]="-";
char g_MNC[10]="-";
char g_EARFCN[10]="-";
char g_GCELLID[10]="-";
char g_TAC[10]="-";
char g_CAT[10]="-";
char g_SINR[10]="-";
char g_BAND[10]="-";
char g_PCI[10]="-"; 
char g_RSRP[10]="-";
char g_RSRQ[10]="-";
char g_RSSI[10]="-";

#define USART_REC_LEN 90
#define NULL 0

uint8_t USART_RX_BUF[USART_REC_LEN]={0};
uint8_t RX_BUF[15]={0};

uint8_t USART_RX_STA=0;
uint16_t g_cnt=0;

void delay_N(int cnt)
{
 int i;
 for(i=0;i<cnt;i++)
  delay_xms(200);
}

//UART0 initialize
// desired baud rate: 9600
// actual: baud rate:9615 (0.2%)
void uart0_init(void)
{
 //UCSRB = 0x00; //disable while setting baud rate
 //UCSRA = 0x00;
 //UCSRC = BIT(URSEL) | 0x06;
 //UBRRL = 0x33; //set baud rate lo
 //UBRRH = 0x00; //set baud rate hi
 //UCSRB = 0x98;

 //UCSRB = 0x00; //disable while setting baud rate
 //UCSRA = 0x00;
 //UCSRC = BIT(URSEL) | 0x06;
 //UBRRL = 0x33; //set baud rate lo
 //UBRRH = 0x00; //set baud rate hi
 //UCSRB = 0x98;
 
 //115200
 UCSRB = 0x00; //disable while setting baud rate
 UCSRA = 0x00;
 UCSRC = BIT(URSEL) | 0x06;
 UBRRL = 0x05; //set baud rate lo
 UBRRH = 0x00; //set baud rate hi
 UCSRB = 0x98;
 
}
void empty_buf()
{
    int i;
	USART_RX_STA=0;
    for(i=0;i<USART_REC_LEN;i++)
	    USART_RX_BUF[i]=0;     
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
		while(*p!=':'&&*p!='\r'&&*p!='\n') p++; p++;
		while(*p!=' '&&*p!='\r'&&*p!='\n'){
			target[idx++]=*p++;
			//printf("%c",*p);
		}
		target[idx]='\0';
		
		//printf("%s=%s\n",ITEM,target);
		return 1;
	}else
	{
	 //target[0]='-';
	 //target[1]=0;
	}
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
 uint8_t i;
  UCSRB &= ~(1 << RXCIE); //Disable interrupt.
 while(!(UCSRA&BIT(RXC)));
 //Shift to left
 for(i=0;i<USART_REC_LEN-1;i++)
 {
  USART_RX_BUF[i]=USART_RX_BUF[i+1];
 }
 USART_RX_BUF[USART_REC_LEN-1]=UDR;
 RX_BUF[USART_RX_STA++]=UDR;
 g_cnt++;
 if(g_cnt==USART_REC_LEN)
 	{		  check_cellinfo();
			  g_cnt=0;
	}
  UCSRB |=  (1 << RXCIE);         //打开串口中断

}


void port_init(void)
{
 PORTB = 0xFF;
 DDRB  = 0x00;
 PORTC = 0x10; //m103 output only
 DDRC  = 0x10;
 PORTD = 0x00;
 DDRD  = 0xFC;
}

//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 CLI(); //disable all interrupts
 port_init();
 init_lcd();
 uart0_init();
 
 MCUCR = 0x00;
 GICR  = 0x00;
 TIMSK = 0x00; //timer interrupt sources
 SEI(); //re-enable interrupts
 //all peripherals are now initialized
}
void check_network_info()
{
    int i;
	uint8_t key;
	
	while(1)
	{
	display_one_string("Collect Info ...", 0,0);
	USART_Transmit_string("AT+LSCELLINFO\r\n"); 
	
	for(i=0;i<30;i++)
	{
	  dis_progress(i);
	  delay_N(2);
	}
	
    display_clear();
	display_one_string("MCC:", 0,0); display_one_string(g_MCC, 4,0);
	display_one_string("MNC:", 8,0); display_one_string(g_MNC, 12,0);
	display_one_string("EARFCN:", 0,1); display_one_string(g_EARFCN, 7,1);
	delay_N(100);
    display_clear();
	display_one_string("TAC:", 0,0); display_one_string(g_TAC, 4,0);
	display_one_string("CAT:", 8,0); display_one_string(g_CAT, 12,0);
	display_one_string("GCELLID:", 0,1); display_one_string(g_GCELLID, 8,1);
	delay_N(100);
    display_clear();
	display_one_string("SINR:", 0,0); display_one_string(g_SINR, 5,0);
	display_one_string("BAND:", 0,1); display_one_string(g_BAND, 5,1);
	delay_N(100);
    display_clear();
	display_one_string("PCI:", 0,0); display_one_string(g_PCI, 4,0);
	display_one_string("RSRP:", 0,1); display_one_string(g_RSRP, 5,1);
	delay_N(100);
    display_clear();
	display_one_string("RSRQ:", 0,0); display_one_string(g_RSRQ, 5,0);
	display_one_string("RSSI:", 0,1); display_one_string(g_RSSI, 5,1);
	delay_N(100);
	//////////////////////
	  key=PINB;
	  key=key&0x0F;
	  
	  if(key==0x0E)
	  {
	  
	  }else if(key==0x0D)
	  {
        
	  }else if(key==0x0B)
	  {
	   display_clear();
	   display_one_string("Back main menu.", 0,0);
	   delay_N(100);
	   break;  
	  }else if(key==0x07){
 	
	   
	  }else
	  {
       
	  }		
	}
}


void pwr_a9500()
{
     int i;
	 
 	 PORTC |=(0x1<<4);
	 PORTC &=~(0x1<<4);
	 //Delay 1000ms
	 for(i=0;i<20;i++)
	 delay_xms(200);
	 PORTC |=(0x1<<4);
	 for(i=0;i<20;i++)
	 delay_xms(200);
	 PORTC &=~(0x1<<4);
}

void chg_band_9600()
{
	    USART_Transmit_string("AT+IPR=9600\r\n");
	    CLI();
	    delay_xms(500);delay_xms(500);delay_xms(500);delay_xms(500);delay_xms(500);delay_xms(500);
		PORTD&=~(0x01<<0);
		PORTD&=~(0x01<<1);
		
 UCSRB = 0x00; //disable while setting baud rate
 UCSRA = 0x00;
 UCSRC = BIT(URSEL) | 0x06;
 UBRRL = 0x47; //set baud rate lo
 UBRRH = 0x00; //set baud rate hi
 UCSRB = 0x98;
 
		MCUCR = 0x00;
        GICR  = 0x00;
        TIMSK = 0x00; //timer interrupt sources
		
		delay_xms(500);
		SEI();
	    USART_Transmit_string("AT\r\n");
}

void check_stat(char *AT_CMD)
{
	//char *p,*q;
	int i;
//	empty_buf();
//	strcpy(USART_RX_BUF,"     OK\r\n");
	USART_Transmit_string(AT_CMD);
	
	//q = RX_BUF;
	display_one_string(RX_BUF, 0,0);
	//Go to the header of the queue.
	//for(i=0;i<USART_REC_LEN;i++)
	//{
	//    if(*q !='\0'||*q != ' ' )
	//        break;
	//    q++;
	//}
	
	//Check error and pin status.
	//p = my_strstr(q,"ERROR");
	//if(p != NULL){
	    //printf("%s\n", p);
	//	display_one_string(p, 0,0);
	//}
	
	//p = my_strstr(q,AT_CMD);
	//if(p != NULL){
	    //printf("%s\n", p);
		//display_one_string(p, 0,0);
	//}	
	 for(i=0;i<50;i++)
	  delay_xms(200);
}

//
void main(void)
{
 uint8_t key,i;
 uint8_t k_lock=0;
 init_devices();
 
 init_CGRAM(); //Custom LCD1602 chars.
 write_byte(1, 0x0);
 
	while(1)
	{
	  delay_xms(20);
      
	  switch(gFSM)
	  {
	      case 0: //Index page.
		      display_one_string(ericsson, 0,0);
			  display_one_string(tool_name, 0,1);
		      break;
		  case 1: //Power on and Init module.
		      display_one_string("PWR ON A9500.", 0,0);
		      break;
		  case 2: 
		      display_one_string("Check SIM card.", 0,0);
			  //dis_progress(temp++%30);
		      break;
		  case 3: 
		      display_one_string("Check IMSI.", 0,0);
		      break;
		  case 4: 
		      display_one_string("Check OP stat.", 0,0);
		      break;
		  case 5: 
		      display_one_string("Check attach.", 0,0);
		      break;
		  case 6:
		      display_one_string("NB Info Auto", 0,0);
		      break;
	      case 7:
		      display_one_string("Detach", 0,0);
		      break;
		  case 8:
		      display_one_string("Manual set OP.", 0,0);
		      break;
		  case 9:
		      display_one_string("Disable RF.", 0,0);
		      break;
		  case 10:
		      display_one_string("Enable RF.", 0,0);
		      break;
		  case 11:
		      display_one_string("Set to NB-IoT.", 0,0);
		      break;
		  case 12:
		      display_one_string("Set to CatM1.", 0,0);
		      break;
		  case 13:
		      display_one_string("Set to GPRS.", 0,0);
		      break;
		  case 14:
		      display_one_string("Start PPP.", 0,0);
		      break;
		  case 15:
		      display_one_string("Close PPP.", 0,0);
		      break;
		  case 16:
		      display_one_string("Ping (114 DNS)", 0,0);
		      break;
		  case 17:
		      display_one_string("PPP Status.", 0,0);
		      break;
		  default://Idle
		      display_one_string("  Idle Status.  ", 0,0);
		      break;               
		  
	  }
	  key=PINB;
	  key=key&0x0F;
	  
	  if(key==0x0E)
	  {
	   gFSM++;
	   if(gFSM>17) gFSM=17;
       delay_xms(200);
	   display_clear();
	  }else if(key==0x0D)
	  {
	   gFSM--;
	   if(gFSM<0) gFSM=0;
       delay_xms(200);
	   display_clear();
	  }else if(key==0x0B)
	  {

	   if(gFSM == 1) {
	     display_one_string("    Waiting...  ", 0,0);
		 pwr_a9500();
		 for(i=0;i<200;i++)
		    delay_xms(200);
		for(i=0;i<200;i++)
		    delay_xms(200);
	     USART_Transmit_string("AT\r\n");
		 for(i=0;i<20;i++)
		    delay_xms(200);	 
		 chg_band_9600();
		 
	     //Display NB-IoT Parameters
		 USART_Transmit_string("ATE1\r\n");
		 delay_xms(50);
		 USART_Transmit_string("AT+CEREG=1\r\n");
		 delay_xms(50);
		 USART_Transmit_string("AT+CGREG=1\r\n");
		 //delay_xms(50);
		 //USART_Transmit_string("AT+CGDCONT=1,'IP','internet'\r\n");
		 delay_xms(50);
		 empty_buf();
		 check_stat("AT\r\n");
	   }
	   if(gFSM == 2) {
	     empty_buf();
		 check_stat("AT+CPIN?\r\n");
	   }
	   if(gFSM == 3) {
	     empty_buf();
		 check_stat("AT+CIMI\r\n");
		 }
	   if(gFSM == 4) {
	     empty_buf();
		 check_stat("AT+COPS?\r\n");
         }
	   if(gFSM == 5) {
	     empty_buf();
		 check_stat("AT+CGATT?\r\n");
         }
	   if(gFSM == 6) {
	     //Display NB-IoT Parameters
	     check_network_info();
	   }
	   if(gFSM == 7) {
	     empty_buf();
		 check_stat("AT+CGATT=0\r\n");
		 }
	   if(gFSM == 8) {
	     empty_buf();
		 check_stat("AT+COPS=1,2,'46012'\r\n");
         }
	   if(gFSM == 9) {
	     empty_buf();
		 check_stat("AT+CFUN=0\r\n");
         }		 
	   if(gFSM == 10) {
	     empty_buf();
		 check_stat("AT+CFUN=1\r\n");
	   }
	   if(gFSM == 11) {
	     empty_buf();
		 check_stat("AT+MODODR=5\r\n"); //3 GSM only  8 GSM and LTE  2 auto
		 delay_xms(200);delay_xms(200);delay_xms(200);delay_xms(200);delay_xms(200);
		 empty_buf();
		 check_stat("AT+LTEOPMOD=2\r\n");
		 }
	   if(gFSM == 12) {
	     empty_buf();
		 check_stat("AT+MODODR=5\r\n"); //3 GSM only  8 GSM and LTE  2 auto
		 delay_xms(200);delay_xms(200);delay_xms(200);delay_xms(200);delay_xms(200);
		 empty_buf();
		 check_stat("AT+LTEOPMOD=1\r\n");
         }
	   if(gFSM == 13) {
	     empty_buf();
		 check_stat("AT+MODODR=3\r\n");
         }
		  
	   if(gFSM == 14) {
	     empty_buf();
		 check_stat("AT+LSIPCALL=1\r\n");
	   }

       if(gFSM == 15) {
         empty_buf();
		 check_stat("AT+LSIPCALL=0\r\n");
	   }
	   if(gFSM == 16) {
	   empty_buf();
	     USART_Transmit_string(PING_DNS);
		 for(i=0;i<50;i++)
		     delay_xms(200);
		 check_stat("AT\r\n");
         }
	   if(gFSM == 17) {
	     empty_buf();
		 check_stat("AT+LSIPCALL?\r\n");
         }
		 
	  }else if(key==0x07){   	
	   
	  }else
	  {
       
	  }

	}
}

/*
	  delay_xms(200);
	  delay_xms(200);
	  delay_xms(200);
	  delay_xms(200);
	  delay_xms(200);
	  //display_clear();
	  //delay_xms(200);
	  
	  write_byte(1, '%');
	  dis_progress(temp%30);
	  display_one_int_num(temp++,1,0);
	  display_one_float_num(temp1,6,0);
	  temp1+=0.01;
*/