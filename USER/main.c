	#include "sys.h"
	#include "GPIO.h"
	#include "LCD1602.h"
	#include "delay.h"
	#include "sys.h"
	#include "stdio.h"
	#include "usart.h"
	#include "timer.h"
	#include "string.h"
	#include "hx711.h"
  #include "beep.h"


	#define uchar unsigned char
	#define uint  unsigned int
	//��������ϵ��
	#define RATIO    0.00423     //

	//�����ʶ
	volatile unsigned char FlagKeyPress = 0;  //�м����±�־�����������0
	volatile unsigned char FlagSetPrice = 0;  //�۸�����״̬��־�����ú�Ϊ1��


	//��ʾ�ñ���
	int Counter;
	int i, iTemp;
	//�����ñ���
	unsigned long FullScale; //������ADֵ/1000
	unsigned long AdVal;     //AD����ֵ
	unsigned long weight;    //����ֵ����λg
	unsigned long   price;     //���ۣ�������ֵ����λΪ��
	unsigned long   money;     //�ܼۣ�������ֵ����λΪ��


	unsigned long   pre_weight;    //����ֵ����λg
	unsigned long   pre_money;

	//���̴������
	unsigned char keycode;
	unsigned char DotPos;				   //С�����־��λ��


	/*��ʱ*/
	void delay(int i)
	{
			int j,k;
			for(j=0; j<i; j++)
					for(k=0; k<500; k++);
	}

	//�����һ���㣬ÿ�β���ǰ����
	void To_Zero()
	{
			FullScale=ReadCount();
			price=0;
	}

	//��ʾ���ۣ���λΪԪ����λ������һλС��
	void Display_Price()
	{
			LCD1602_write_com(0x8c);
			LCD1602_write_data(price/100 + 0x30);
			LCD1602_write_data(price%100/10 + 0x30);
			LCD1602_write_data('.');
			LCD1602_write_data(price%10 + 0x30);
	}

	//��ʾ��������λkg��һλ��������λС��
	void Display_Weight()
	{
			LCD1602_write_com(0x83);
			LCD1602_write_data(weight/1000 + 0x30);
			LCD1602_write_data('.');
			LCD1602_write_data(weight%1000/100 + 0x30);
			LCD1602_write_data(weight%100/10 + 0x30);
			LCD1602_write_data(weight%10 + 0x30);
	}

	//��ʾ�ܼۣ���λΪԪ����λ������һλС��
	void Display_Money()
	{
			if (money>9999) 	//������ʾ����
			{
					LCD1602_write_com(0x80+0x40+4);
					LCD1602_write_word("---.-");
					return;
			}

			if (money>=1000)
			{
					LCD1602_write_com(0x80+0x40+4);
					LCD1602_write_data(money/1000 + 0x30);
					LCD1602_write_data(money%1000/100 + 0x30);
					LCD1602_write_data(money%100/10 + 0x30);
					LCD1602_write_data('.');
					LCD1602_write_data(money%10 + 0x30);
			}
			else if (money>=100)
			{
					LCD1602_write_com(0x80+0x40+4);
					LCD1602_write_data(0x20);
					LCD1602_write_data(money%1000/100 + 0x30);
					LCD1602_write_data(money%100/10 + 0x30);
					LCD1602_write_data('.');
					LCD1602_write_data(money%10 + 0x30);
			}
			else if(money>=10)
			{
					LCD1602_write_com(0x80+0x40+4);
					LCD1602_write_data(0x20);
					LCD1602_write_com(0x80+0x40+5);
					LCD1602_write_data(0x20);
					LCD1602_write_data(money%100/10 + 0x30);
					LCD1602_write_data('.');
					LCD1602_write_data(money%10+ 0x30);
			}
			else
			{
					LCD1602_write_com(0x80+0x40+4);
					LCD1602_write_data(0x20);
					LCD1602_write_com(0x80+0x40+5);
					LCD1602_write_data(0x20);
					LCD1602_write_com(0x80+0x40+6);
					LCD1602_write_data(0 + 0x30);
					LCD1602_write_data('.');
					LCD1602_write_data(money%10 + 0x30);
			}
	}

	//���ݳ�ʼ��
	void Data_Init()
	{
			price = 0;
			DotPos = 0;
	}

	//������Ӧ���򣬲����Ǽ�ֵ
	//���ؼ�ֵ��
	//         7          8    9      10(��0)
	//         4          5    6      11(ɾ��)
	//         1          2    3      12(δ����)
	//         14(δ����) 0    15(.)  13(ȷ���۸�)

	void KeyPress(uchar keycode)
	{
			switch (keycode)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
					//Ŀǰ����������λ��Ҫע��price�����ͣ��洢��λΪ��
					if (DotPos == 0)
					{
							//���ֻ�����õ�ǧλ
							if (price<100)
							{
									price=price*10+keycode*10;
							}
					}
					//Ŀǰ������С��λ
					else if (DotPos==1)  //С������һλ
					{
							price=price+keycode;
							DotPos=2;
					}
					Display_Price();
					break;
			case 10:   //�����
					To_Zero();
					Display_Price();
					FlagSetPrice = 0;
					DotPos = 0;
					break;
			case 11:	//ɾ��������һ��ɾ������һ������
					if (DotPos==2)
					{
							price=price/10;
							price=price*10;
							DotPos--;
					}
					else if (DotPos==1)
					{
							price=price/100;
							price=price*10;
							DotPos--;
					}
					else if (DotPos==0)
					{
							price=price/100;
							price=price*100;
					}
					Display_Price();
					break;
			case 13:   //ȷ�ϼ�
					FlagSetPrice = 1;
					DotPos=0;
					break;
			case 15:   //С���㰴��
					DotPos = 1;      //С������һλ
					break;

			}
	}
	unsigned short weight_value_filter(unsigned short wvalue)//����ֵ�˲�
	{
			/*----------------------------------------------------------------*/
			/* Local Variables                                                */
			/*----------------------------------------------------------------*/
			unsigned char i,j;
			static unsigned short   lastest_5times_value[5] = {0};
			static unsigned char   filter_count = 0;
			static unsigned short   calibration_voltage = 0;
			unsigned char max_equal_index = 0;
			unsigned char value_equal_count[5] = {0};
			/*----------------------------------------------------------------*/
			/* Code Body                                                      */
			/*----------------------------------------------------------------*/
			if(filter_count < 5)
			{
					filter_count ++;
					lastest_5times_value[filter_count -1] = wvalue;
					return lastest_5times_value[filter_count -1];
			}
			else
			{
					i = 0;
					while(i < 4)
					{
							lastest_5times_value[i] = lastest_5times_value[i + 1];
							i ++;
					}

					lastest_5times_value[4] = wvalue;
			}
			for(i = 0; i < 5; i ++)
			{
					for(j = 0; j < 5; j ++)
					{
							if(lastest_5times_value[i] == lastest_5times_value[j])
							{
									value_equal_count[i] ++;
							}
					}
			}
			for(i = 0; i < 4; i ++)
			{
					if(value_equal_count[max_equal_index] < value_equal_count[i + 1])
					{
							max_equal_index = i + 1;
					}
			}
			for(i = 0; i < 4; i ++)
			{
					if(calibration_voltage == lastest_5times_value[i])
					{
							return calibration_voltage;
					}
			}

			calibration_voltage = lastest_5times_value[max_equal_index];

			return calibration_voltage;
	}
	void  get_weight(void)				    //��ó���
	{
			volatile unsigned long cur_adc_value = 0;

			//���أ��õ�����ֵweight����λΪg
			
			cur_adc_value = ReadCount();

			if (cur_adc_value <= FullScale)
			{
					weight = 0;
			}
			else
			{
					cur_adc_value = cur_adc_value - FullScale;
					if (cur_adc_value > 55)
					{
							if (cur_adc_value % 100 >= 55)
							{
									cur_adc_value = cur_adc_value + (100 - (cur_adc_value % 100));
							}
							else
							{
									cur_adc_value = cur_adc_value - (cur_adc_value % 100);
							}
					}
					else
					{
							cur_adc_value = 0;
					}

					weight = cur_adc_value * RATIO;//��������
			}
			weight = weight_value_filter(weight);//����ֵ�˲�
	}


	 int main(void)
	 {	
		u8 test_interval = 0;

		delay_init();	    //��ʱ������ʼ��	  
		KEY_GPIO_Init();    //������ʼ��
		Button4_4_Init();
		HX711_GPIO_Init();
		BEEP_Init();  // ��ʼ��������

		delay_ms(500);       //�ϵ�˲�����һ����ʱ�ڳ�ʼ��
		LCD_Init();         //��Ļ��ʼ��
		delay_ms(300); 
		LCD1602_write_com(0x80);						//ָ������
		LCD1602_write_word("Welcome to use! ");
		To_Zero();
		LCD1602_write_com(0x80);						//ָ������
		LCD1602_write_word("WE: .    PR:  . ");
		LCD1602_write_com(0x80+0x40);				//ָ������
		LCD1602_write_word("MON:   .        ");
		Display_Price();


		while(1)
		{     
				 //ÿ0.5�����һ��
					if (test_interval ++ > 25)
					{
							test_interval = 0;
							//���أ��õ�����ֵweight����λΪg
				get_weight();

							//��������̣���ʾ��-.---��
							if (weight >= 10000)
							{
									pre_weight = 10000;
									LCD1602_write_com(0x83);
									LCD1602_write_word("-.---");
								  BEEP_Set(1);
								
							}
							//�����������
							else
							{
								  BEEP_Set(0);
									//��ʾ����ֵ
									if (pre_weight != weight)
									{
											pre_weight = weight;
											Display_Weight();
									}
									//��������趨���ˣ������۸�
									if (FlagSetPrice == 1)
									{
											money = weight*price/1000;  //money��λΪ��
											//��ʾ�ܽ��
											if (pre_money != money)
											{
													pre_money = money;
													Display_Money();
											}
									}
									else
									{
											LCD1602_write_com(0x80+0x40+4);
											LCD1602_write_word("   . ");
									}
							}
					}
					//��ȡ����
					keycode = Button4_4_Scan();
					delay(100);
					//��Ч��ֵ0-15
					if ((keycode<16)&&(FlagKeyPress==0))
					{
							FlagKeyPress = 1;
							KeyPress(keycode);
							FlagKeyPress = 0;
						 delay(100);
					}
					delay(1);
			}
			
		}
	 

