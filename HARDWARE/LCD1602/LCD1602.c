#include "LCD1602.h"
#include "delay.h"
#include "sys.h"

u8 Dao_xu(u8 data)//倒序函数
{
    u8 i = 0 ,temp = 0;;
	  
	  for(i = 0; i < 8; i++)
	 {
	   temp += (((data >> i) & 0x01) << (7 - i));
	 }
	 return temp;
}
/*-----------------------------------------------
  名称：LCD1602

  引脚定义如下：1-VSS 2-VDD 3-V0 4-RS 5-R/W 6-E 7-14 DB0-DB7 15-BLA 16-
              判忙函数
------------------------------------------------*/
 void  LCD_Check_Busy(void) 
 
{
  LCD1602_RS0;	
	delay_ms(1);
  LCD1602_RW1;    
	delay_ms(1);
  LCD1602_EN0;
	delay_ms(1);
  GPIO_Write(GPIOC,0Xff); 
	delay_ms(1);
  LCD1602_EN1; 
	delay_ms(100);
}

/*------------------------------------------------
              写入命令函数
------------------------------------------------*/
 void LCD1602_write_com(unsigned char com) 
{                               
  LCD1602_RS0;	
	delay_ms(1);
  LCD1602_RW0;
  delay_ms(1);	
  LCD1602_EN1;                    
	delay_ms(1);
  GPIO_Write(GPIOA,(GPIO_ReadOutputData(GPIOA)&0XFF00)+Dao_xu(com));              
	delay_ms(1);
  LCD1602_EN0;
}

/*------------------------------------------------
              写入数据函数
------------------------------------------------*/
 void LCD1602_write_data(unsigned char Data) 
{                                    
  LCD1602_RS1;                
	delay_ms(1);
  LCD1602_RW0;
  delay_ms(1);	
  LCD1602_EN1;                    
	delay_ms(1);
  GPIO_Write(GPIOA,(GPIO_ReadOutputData(GPIOA)&0XFF00)+Dao_xu(Data));
	delay_ms(1);
  LCD1602_EN0;
}
/*------------------------------------------------
              写入字符串函数
------------------------------------------------*/
 void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s) 
 {     
 if (y == 0) 
 	{     
	 LCD1602_write_com(0x80 + x);     
 	}
 else 
 	{     
 	LCD1602_write_com(0xC0 + x);     
 	}        
 while (*s) 
 	{     
 LCD1602_write_data( *s);     
 s ++;     
 	}
 }
/*------------------------------------------------
              写入字符函数
------------------------------------------------*/
 void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data) 
 {     
 if (y == 0) 
 	{     
 	LCD1602_write_com(0x80 + x);     
 	}    
 else 
 	{     
 	LCD1602_write_com(0xC0 + x);     
 	}        
 LCD1602_write_data( Data);  
 }
 
 
 void LCD1602_write_long(unsigned char x,unsigned char y,u32 data,unsigned char num) 
{
    unsigned char temp[12],i = 12,temp_num = num;
    while(i--)
    {
         temp[i] = ' ';
    }
    temp[num] = '\0';
    while(num--)
    {
      if(data || data%10)
         temp[num] = data % 10 + 0x30;
      data = data/10;
    }
		if(temp[temp_num - 1] == ' ')  temp[temp_num - 1] = '0';
    LCD_Write_String(x,y,temp);   
}

 /*------------------------------------------------
                清屏函数
------------------------------------------------*/
 void LCD_Clear(void) 
 { 
 LCD1602_write_com(0x01); 
 delay_ms(5);
 }
   

void LCD1602_write_word(unsigned char *s)
{
	while(*s>0)
	{
		LCD1602_write_data(*s);
		s++;
	}
}

void LCD_Init()
{
		GPIO_InitTypeDef GPIO_InitStructure;
	  
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);  //开启GPIOA GPIOB GPIOC时钟
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
    GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //输出速度50MHZ
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化GPIOA
	
	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 |GPIO_Pin_14|GPIO_Pin_15;  // LCD1602 RS-RW-EN?
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //输出速度50MHZ
    GPIO_Init(GPIOC, &GPIO_InitStructure);   //GPIOC
	
	LCD1602_EN0;
	LCD1602_RW0;						//设置为写状态
	LCD1602_write_com(0x38);			//显示模式设定
	LCD1602_write_com(0x0c);			//开关显示、光标有无设置、光标闪烁设置
	LCD1602_write_com(0x06);			//写一个字符后指针加一
	LCD1602_write_com(0x01);			//清屏指令
}

