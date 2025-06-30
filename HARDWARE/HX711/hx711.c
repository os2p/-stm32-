#include "hx711.h"
#include "GPIO.h"
#include "delay.h"
#include "sys.h"

unsigned long ReadCount(void)//请参考HX711芯片手册
{
   u32 Count = 0;
   u8 i;
   ADSK=0;
   while(ADDO);//等待AD转换结束
   for (i=0;i<24;i++)
   {
      ADSK=1;
      Count=Count<<1;//变量左移一位，右侧补零
		  delay_us(1);
      ADSK=0;
      if(ADDO) 
				Count++;
			delay_us(1);
   }
   ADSK=1;
   Count=Count^0x800000;//第25个脉冲下降沿来时，转换数据
	 delay_us(1);
   ADSK=0;
	 delay_us(1);
   return(Count);
}

