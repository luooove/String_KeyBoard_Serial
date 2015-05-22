//用来测试按键发送字符串给PC机，不同的按键发送不同的指令。
//下一步用按键控制液晶

#include <msp430.h> 
#define uchar unsigned char
#define uint unsigned int
uchar key_table[16]={1,2,3,10,4,5,6,11,7,8,9,12,15,0,14,13};  //键盘映射
#define nullkey 20;
uchar key_value=nullkey;

unsigned char str[10]="abc\n";
unsigned char KEY1[10]="1\n";
unsigned char KEY2[10]="2\n";
unsigned char KEY3[10]="3\n";
unsigned char KEY4[10]="4\n";
unsigned char KEY5[10]="5\n";
unsigned char KEY6[10]="6\n";
unsigned char KEY7[10]="7\n";
unsigned char KEY8[10]="8\n";
unsigned char KEY9[10]="9\n";
unsigned char KEY10[10]="10\n";
unsigned char KEY11[10]="11\n";
unsigned char KEY12[10]="12\n";
unsigned char KEY13[10]="13\n";
unsigned char KEY14[10]="14\n";
unsigned char KEY15[10]="15\n";
unsigned char KEY16[10]="16\n";

/* MCLK=16MHZ 下测定 */

void delay_ms(uint i)
{
  uint j=1328;
  while(i--)
  {
       for(j=1328;j>0;j--);
  }
}

/* MCLK=16MHZ 下测定 */

void delay_us(uint i)
{
  uchar j=1;
  while(i--)
  {
       for(j=1;j>0;j--);
  }
}
//#include "Key.c"

///* MCLK=16MHZ 下测定 */
//
//void delay_ms(uint i)
//{
//  uint j=1328;
//  while(i--)
//  {
//       for(j=1328;j>0;j--);
//  }
//}

//---------------------键盘端口初始
void init_Key(void)
{
// P10SEL &=~(BIT0+BIT1+BIT2+BIT3);
// P10DIR &=~(BIT0+BIT1+BIT2+BIT3);
// P5SEL  &=~(BIT4+BIT5+BIT6+BIT7);
// P5DIR  |=BIT4+BIT5+BIT6+BIT7;
// P5OUT  &=~(BIT4+BIT5+BIT6+BIT7);

   P1SEL &=~(BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7);
   P1DIR &=~(BIT0+BIT1+BIT2+BIT3); //set IO input
   P1DIR  |=(BIT4+BIT5+BIT6+BIT7);   //set IO output
}

//---------------------键盘查询 若有键按下取出键值
void checkkey(void)
{
  if((P1IN&0X0F)<0X0F)
  {
    delay_ms(25);
    if((P1IN&0X0F)<0X0F)
    {
//      keysound();
      uchar i,j;
      uchar temp=0X10;
      for(i=0;i<4;i++)   //列扫描
      {
        uchar temp1=0X01;           //00000001
        P1OUT |=0XF0;               //1111xxxx
        P1OUT &=~temp;              //1111xxxx&11101111=1110xxxx
        for(j=0;j<4;j++)     //行扫描
        {
          if((P1IN&temp1)==0)      //xxxx1110&00000001
          {
            key_value=i+4*j;        //读取键值
            break;
          }
          temp1 <<=1;
        }
        temp <<=1;
      }
    }
    P1OUT &=0X0F;            //0000xxxx
    while((P1IN&0X0F)<0X0F); //等待按键弹起
  }
}


//选择对应的端口
#define KEYOUT P1OUT
#define KEYIN P1IN
#define KEYDIR P1DIR



/***************全局变量***************/
uchar key_Pressed;      //按键是否被按下:1--是，0--否
uchar key_val;          //存放键值
uchar key_Flag;         //按键是否已放开：1--是，0--否
//设置键盘逻辑键值与程序计算键值的映射
uchar key_Map[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

/*******************************************
函数名称：Init_Keypad
功    能：初始化扫描键盘的IO端口
参    数：无
返回值  ：无
********************************************/
void Init_Keypad(void)
{
    KEYDIR = 0xf0;       //KEY.0~KEY.3设置为输入状态, KEY.4~KEY.7设置为输出状态
    KEYOUT |= 0xf0;      // KEY.4~KEY.7输出高电平
    key_Flag = 0;
    key_Pressed = 0;
    key_val = 0;
}
/*********************************************
* Check_Key(),检查按键，确认键值
*********************************************/
/*******************************************
函数名称：Check_Key
功    能：扫描键盘的IO端口，获得键值
参    数：无
返回值  ：无
********************************************/
void Check_Key(void)
{
    uchar row ,col,tmp1,tmp2;

    tmp1 = 0x80;
    for(row = 0;row < 4;row++)              //行扫描
    {
        KEYOUT = 0xf0;                      //KEY.4~KEY.7输出全1
        KEYOUT -= tmp1;                      //KEY.4~p1.7输出四位中有一个为0
        tmp1 >>=1;
        if ((KEYIN & 0x0f) < 0x0f)           //是否KEYIN的KEY.0~KEY.3中有一位为0
        {
            tmp2 = 0x01;                         // tmp2用于检测出那一位为0
            for(col = 0;col < 4;col++)              // 列检测
            {
                if((KEYIN & tmp2) == 0x00)           // 是否是该列,等于0为是
                {
                    key_val = key_Map[row * 4 + col];  // 获取键值
                    return;                         // 退出循环
                }
                tmp2 <<= 1;                        // tmp2右移1位
            }
        }
    }
}
/*******************************************
函数名称：delay
功    能：延时约15ms，完成消抖功能
参    数：无
返回值  ：无
********************************************/
void delay()
{
    uint tmp;

    for(tmp = 12000;tmp > 0;tmp--);
}
/*******************************************
函数名称：Key_Event
功    能：检测按键，并获取键值
参    数：无
返回值  ：无
********************************************/
void Key_Event(void)
{
    uchar tmp;

    KEYOUT &= 0x00;              // 设置KEYOUT全为0，等待按键输入
    tmp = KEYIN;                 // 获取 p1IN
    if ((key_Pressed == 0x00)&&((tmp & 0x0f) < 0x0f)) //如果有键按下
    {
        key_Pressed = 1;        // 如果有按键按下，设置key_Pressed标识
        delay();                //消除抖动
        Check_Key();            // 调用check_Key(),获取键值
    }
    else if ((key_Pressed == 1)&&((tmp & 0x0f) == 0x0f)) //如果按键已经释放
    {
        key_Pressed = 0;        // 清除key_Pressed标识
        key_Flag    = 1;        // 设置key_Flag标识
    }
    else
    {
        _NOP();
    }
}

void Serial_init()
{
	P10SEL = 0x30;                             // P10.4,5 = USCI_A0 TXD/RXD
	UCA3CTL1 |= UCSWRST;                      // **Put state machine in reset**
	UCA3CTL1 |= UCSSEL_2;                     // SMCLK
	UCA3BR0 = 6;                              // 1MHz 9600 (see User's Guide)
	UCA3BR1 = 0;                              // 1MHz 9600
	UCA3MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0,
											// over sampling
	UCA3CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	//UCA3IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

}


void UartTX_Send(unsigned char *Data,int len)
{
	int j;
	for(j=0;j<len;j++)
		{
			while (!(UCA3IFG&UCTXIFG));
			UCA3TXBUF =*Data++;
		}
}

void KeyBoard_Test()
{
	checkkey();  //等待按键
	if(key_value==1)                 //供电长度设置
		{
		  key_value=nullkey;
		  UartTX_Send(KEY1,3);
		}
	else if(key_value==2)                 //供电长度设置
		{
		  key_value=nullkey;
		  UartTX_Send(KEY2,3);
		}
	else if(key_value==3)                 //供电长度设置
		{
		  key_value=nullkey;
		  UartTX_Send(KEY3,3);
		}
	else if(key_value==4)                 //供电长度设置
		{
		  key_value=nullkey;
		  UartTX_Send(KEY4,3);
		}
	else if(key_value==5)                 //供电长度设置
			{
			  key_value=nullkey;
			  UartTX_Send(KEY5,3);
			}
	else if(key_value==6)                 //供电长度设置
			{
			  key_value=nullkey;
			  UartTX_Send(KEY6,3);
			}
	else if(key_value==7)                 //供电长度设置
			{
			  key_value=nullkey;
			  UartTX_Send(KEY7,3);
			}
	else if(key_value==8)                 //供电长度设置
			{
			  key_value=nullkey;
			  UartTX_Send(KEY8,3);
			}
	else if(key_value==9)                 //供电长度设置
			{
			  key_value=nullkey;
			  UartTX_Send(KEY9,3);
			}
	else if(key_value==10)                 //供电长度设置
			{
			  key_value=nullkey;
			  UartTX_Send(KEY10,3);
			}
	else if(key_value==11)                 //供电长度设置
			{
			  key_value=nullkey;
			  UartTX_Send(KEY11,3);
			}
	else if(key_value==12)                 //供电长度设置
		{
		  key_value=nullkey;
		  UartTX_Send(KEY12,3);
		}
	else if(key_value==13)                 //供电长度设置
		{
		  key_value=nullkey;
		  UartTX_Send(KEY13,3);
		}
	else if(key_value==14)                 //供电长度设置
		{
		  key_value=nullkey;
		  UartTX_Send(KEY14,3);
		}
	else if(key_value==15)                 //供电长度设置
		{
		  key_value=nullkey;
		  UartTX_Send(KEY15,3);
		}
	else if(key_value==16)                 //供电长度设置
		{
		  key_value=nullkey;
		  UartTX_Send(KEY16,3);
		}
}

void SendData()
{
	switch (key_val)
	{
		case 1: UartTX_Send(KEY1,3);break;
		case 2: UartTX_Send(KEY2,3);break;
		case 3: UartTX_Send(KEY3,3);break;
		case 4: UartTX_Send(KEY4,3);break;
		case 5: UartTX_Send(KEY5,3);break;
		case 6: UartTX_Send(KEY6,3);break;
		case 7: UartTX_Send(KEY7,3);break;
		case 8: UartTX_Send(KEY8,3);break;
		case 9: UartTX_Send(KEY9,3);break;
		case 10: UartTX_Send(KEY10,3);break;
		case 11: UartTX_Send(KEY11,3);break;
		case 12: UartTX_Send(KEY12,3);break;
		case 13: UartTX_Send(KEY13,3);break;
		case 14: UartTX_Send(KEY14,3);break;
		case 15: UartTX_Send(KEY15,3);break;
		case 16: UartTX_Send(KEY16,3);break;
	}
}


/*
 * main.c
 */
void main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    Serial_init();
    Init_Keypad();
    UartTX_Send(str,4);
    UartTX_Send(str,4);
    UartTX_Send(str,4);
    while(1)
      {
          Key_Event();  //在死循环中进行键值扫描

          if(key_Flag == 1)//用来判断有无按键按下，从而读取键值进行操作
          {
        	  //UCA3TXBUF = key_val;
        	  SendData();
              key_Flag = 0;//key_Flag需要手动清零
          }
      }

}
