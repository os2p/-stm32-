#ifndef __LCD1602_H
#define __LCD1602_H
#include "delay.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
/*-----------------------------------------------
  名称：LCD1602
  论坛：www.doflye.net
  编写：shifang
  日期：2009.5
  修改：无
  内容：通过标准程序静态显示字符
  引脚定义如下：1-VSS 2-VDD 3-V0 4-RS 5-R/W 6-E 7-14 DB0-DB7 15-BLA 16-BLK
------------------------------------------------*/

#define      LCD1602_RS0           GPIOC->BRR  = 0x00002000 //PC13
#define      LCD1602_RW0           GPIOC->BRR  = 0x00004000 //PC14
#define      LCD1602_EN0           GPIOC->BRR  = 0x00008000 //PC15

#define      LCD1602_RS1           GPIOC->BSRR = 0x00002000 //PC13
#define      LCD1602_RW1           GPIOC->BSRR = 0x00004000 //PC14
#define      LCD1602_EN1           GPIOC->BSRR = 0x00008000 //PC15

 
 void LCD_Init(void); 

 void LCD_Clear(void); 
 
 void  LCD_Check_Busy(void); 
 
 void LCD1602_write_com(unsigned char com); 
 
 void LCD1602_write_data(unsigned char Data); 
 
 void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s);
	
 void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data); 

 void LCD1602_write_long(unsigned char x,unsigned char y,u32 data,unsigned char num) ;
 
 void LCD1602_write_word(unsigned char *s);
 
#endif



