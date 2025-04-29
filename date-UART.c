#include <xc.h>
#include<stdio.h>
#pragma config OSC = HS 
#pragma config WDT = OFF  
#pragma config PBADEN=OFF
#define _XTAL_FREQ 20000000
#define SW RB0
#define LCD PORTD 
#define RS RC1
#define RW RC0
#define EN RC2
#define RTC_ADD 0xD0
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void LCD_StringDisplay(char str2);
void lcd_init(void);
void i2c_start(void);
void i2c_restart(void);
void i2c_stop(void);
void i2c_write(unsigned	 char);
unsigned char i2c_read(void);
void i2c_init(void);
void set_reg(unsigned char, unsigned char[]);
unsigned char get_reg(unsigned char);
void write_str_to_rtc(unsigned char start_address,char *data);
static unsigned char str[14]={'\0'}, str2[6]={'\0'},str3[14]={'\0'};
unsigned int flag = 0, j = 0, k = 0, flg = 0, i = 0, a = 0, b = 0, c = 0,flag2=0,n=0;
unsigned int count =0,flag3=0;
unsigned char strt_add = 0x00;
 unsigned char date,mnt,yr,sec,min,hrs,h;
void __interrupt(high_priority) myHighIsr(void)
{
    if(INT0IF==1)
    { 
        INT0IF=0;
        flg=1;
        flag2=0;
    }
//    if(INT1IF==1)
//    {
//        count++;
//        INT1IF=0;
//        flag3=1;
//        if(count==2){
//            count=0;
//            flag3=0;
//        }
//    }
    if(RCIF == 1)  
    {
        if(FERR || OERR)
        {
            CREN = 0;
            CREN = 1;
        }             

        str[i++] = RCREG;
        if(i == 12)
        {      
            flag = 1;  
            i = 0;
            CREN  =0;
            RCIE = 0;
        }

    }

    if(TXIF == 1)  
    {
        if(str[c] != '\0')  
        {
            TXREG = str[c];  
            c++;
        }
        else
        {
            TXIE = 0;  
            c=0;
            TXEN =0;
        }
    }
}

void main(void)
{
    ADCON1 = 0x0f;
    //TRISB=0x00;
    TRISC1 = 0;
    TRISC0 = 0;
    TRISC2 = 0;
    //TRISC3 = 1;
    //TRISC4 = 1;
    TRISB0=1;
    nRBPU=0;
    TRISD = 0x00;  
    SPEN = 1;    
    SYNC = 0;    
    TX9 = 0;     
    RX9=0;
    BRGH = 1;    
    BRG16 = 0;   
    SPBRG = 129; 
    TXEN = 0;    
    TXIE = 1;
    INT0IF=0;
    INT0IE=1;
    INT1IE=1;
    INT1IF=0;
    RCIE = 1;    
    PEIE = 1;    
    GIE = 1;   
    lcd_init();
       
    while(1)
    {
        //while(flag3==1);
        if (flag2 == 0)  
        {
           while(flg==0);
           {
           flg=0;
            CREN = 1;  
           }

        while(flag == 0);  
        flag=0;
            k=0;
            
          for (j = 0; j < 7; j++) 
           // while(j<7)
            {
                b = str[n] & 0x0F;        
                a = str[n + 1] & 0x0F;    
                b = b << 4; 
                if(j==3)
                {
                    str2[j]='0';
                    j++;
                }
                str2[j]= (a | b);                
                n = n + 2;  
                //j++;
            }
            TXEN = 1;
        
            i2c_init();
            set_reg(0x00,str2); 
        } 
            sec = get_reg(0);
            min = get_reg(1);	
            hrs = get_reg(2)&0x3F; 
            date= get_reg(4);
            mnt = get_reg(5); 
            yr  = get_reg(6);

sprintf(str3,"DATE:%02x:%02x:%02x",date,mnt,yr);
lcd_cmd(0x80); 
    for(i=0;i<13;i++)
    {
        lcd_data(str3[i]); 
    }

sprintf(str3,"TIME:%02x:%02x:%02x",hrs,min,sec);
 lcd_cmd(0xC0);
    for(i=0;i<13;i++)
    {
        lcd_data(str3[i]);
    }
 flag2=1;
     __delay_ms(1000);
    // lcd_cmd(0x01);
     
 }
    
}

void lcd_init(void)
{
    lcd_cmd(0x38);
    __delay_ms(2);
    lcd_cmd(0x0E);
    __delay_ms(2);  
    lcd_cmd(0x01); 
    __delay_ms(2);
    lcd_cmd(0x06);
    __delay_ms(2); 
}
void lcd_cmd(unsigned char cmd)
{
    LCD = cmd;
    RS = 0;
    RW = 0;
    EN = 1;
  __delay_ms(2);
    EN = 0;
}
void lcd_data(unsigned char data)
{
    RS = 1;
    RW = 0;
    LCD = data;
    EN = 1;
   __delay_ms(2);
    EN = 0;
}

 void i2c_start(void)    // Initiate a Start sequence
{   
    SSPIF=0; 
    SEN=1;
    while(SSPIF == 0);
}

void i2c_restart(void)  // Initiate a Restart sequence
{
    SSPIF=0;
    RSEN=1;
    while(SSPIF==0);
}
void i2c_stop(void)     //Initiate a Stop sequence.
{
    SSPIF=0; 
    PEN=1;
    while(SSPIF==0);
}

void i2c_write(unsigned	char data)      // Write data to slave.
{
    SSPIF=0;
    SSPBUF=data;
    while(SSPIF==0);
}

unsigned char i2c_read(	void)     //Read data from I2C bus.
{
    unsigned char data;
    SSPIF=0;
    RCEN=1;
    while(SSPIF==0);
    data=SSPBUF;
    ACKEN=1;
    SSPIF=0;
    while(SSPIF==0);
    return data;
}

void i2c_init(void)
{
    TRISC3=1;
    TRISC4=1;
    SSPADD = 49;         //set i2c clock 100kHz
    SSPCON1= 0x08;       //set i2c hardware controlled master
    SMP = 1;  //standard mode 100kHz
    SSPEN = 1;
}
unsigned char get_reg(unsigned char address)     // Gets data from RTC
{
    unsigned char data;
    i2c_start();
    i2c_write(RTC_ADD);
    i2c_write(address);
    i2c_restart();
    i2c_write(RTC_ADD | 0x01);
    ACKDT=1;
    data=i2c_read();
    i2c_stop();
    return data;
}

void set_reg(unsigned char address, unsigned char x[])     // sets data into RTC
{
    i2c_start();
    i2c_write(RTC_ADD); 
    i2c_write(address);
    for(h=0;h<7;h++) 
    {    
        if(address == 0x03)
        {
            address ++;
        } 
    i2c_write((x[h]));
    address++;
    }
    i2c_stop();
}
