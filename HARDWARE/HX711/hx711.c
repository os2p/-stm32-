#include "hx711.h"
#include "GPIO.h"
#include "delay.h"
#include "sys.h"

unsigned long ReadCount(void)//��ο�HX711оƬ�ֲ�
{
   u32 Count = 0;
   u8 i;
   ADSK=0;
   while(ADDO);//�ȴ�ADת������
   for (i=0;i<24;i++)
   {
      ADSK=1;
      Count=Count<<1;//��������һλ���Ҳಹ��
		  delay_us(1);
      ADSK=0;
      if(ADDO) 
				Count++;
			delay_us(1);
   }
   ADSK=1;
   Count=Count^0x800000;//��25�������½�����ʱ��ת������
	 delay_us(1);
   ADSK=0;
	 delay_us(1);
   return(Count);
}

