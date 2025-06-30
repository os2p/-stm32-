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
	//定义量程系数
	#define RATIO    0.00423     //

	//定义标识
	volatile unsigned char FlagKeyPress = 0;  //有键按下标志，处理完毕清0
	volatile unsigned char FlagSetPrice = 0;  //价格设置状态标志，设置好为1。


	//显示用变量
	int Counter;
	int i, iTemp;
	//称重用变量
	unsigned long FullScale; //满量程AD值/1000
	unsigned long AdVal;     //AD采样值
	unsigned long weight;    //重量值，单位g
	unsigned long   price;     //单价，长整型值，单位为分
	unsigned long   money;     //总价，长整型值，单位为分


	unsigned long   pre_weight;    //重量值，单位g
	unsigned long   pre_money;

	//键盘处理变量
	unsigned char keycode;
	unsigned char DotPos;				   //小数点标志及位置


	/*延时*/
	void delay(int i)
	{
			int j,k;
			for(j=0; j<i; j++)
					for(k=0; k<500; k++);
	}

	//重新找回零点，每次测量前调用
	void To_Zero()
	{
			FullScale=ReadCount();
			price=0;
	}

	//显示单价，单位为元，两位整数，一位小数
	void Display_Price()
	{
			LCD1602_write_com(0x8c);
			LCD1602_write_data(price/100 + 0x30);
			LCD1602_write_data(price%100/10 + 0x30);
			LCD1602_write_data('.');
			LCD1602_write_data(price%10 + 0x30);
	}

	//显示重量，单位kg，一位整数，三位小数
	void Display_Weight()
	{
			LCD1602_write_com(0x83);
			LCD1602_write_data(weight/1000 + 0x30);
			LCD1602_write_data('.');
			LCD1602_write_data(weight%1000/100 + 0x30);
			LCD1602_write_data(weight%100/10 + 0x30);
			LCD1602_write_data(weight%10 + 0x30);
	}

	//显示总价，单位为元，三位整数，一位小数
	void Display_Money()
	{
			if (money>9999) 	//超出显示量程
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

	//数据初始化
	void Data_Init()
	{
			price = 0;
			DotPos = 0;
	}

	//按键响应程序，参数是键值
	//返回键值：
	//         7          8    9      10(清0)
	//         4          5    6      11(删除)
	//         1          2    3      12(未定义)
	//         14(未定义) 0    15(.)  13(确定价格)

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
					//目前在设置整数位，要注意price是整型，存储单位为分
					if (DotPos == 0)
					{
							//最多只能设置到千位
							if (price<100)
							{
									price=price*10+keycode*10;
							}
					}
					//目前在设置小数位
					else if (DotPos==1)  //小数点后第一位
					{
							price=price+keycode;
							DotPos=2;
					}
					Display_Price();
					break;
			case 10:   //清零键
					To_Zero();
					Display_Price();
					FlagSetPrice = 0;
					DotPos = 0;
					break;
			case 11:	//删除键，按一次删除最右一个数字
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
			case 13:   //确认键
					FlagSetPrice = 1;
					DotPos=0;
					break;
			case 15:   //小数点按下
					DotPos = 1;      //小数点后第一位
					break;

			}
	}
	unsigned short weight_value_filter(unsigned short wvalue)//重量值滤波
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
	void  get_weight(void)				    //获得称重
	{
			volatile unsigned long cur_adc_value = 0;

			//称重，得到重量值weight，单位为g
			
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

					weight = cur_adc_value * RATIO;//计算重量
			}
			weight = weight_value_filter(weight);//重量值滤波
	}


	 int main(void)
	 {	
		u8 test_interval = 0;

		delay_init();	    //延时函数初始化	  
		KEY_GPIO_Init();    //按键初始化
		Button4_4_Init();
		HX711_GPIO_Init();
		BEEP_Init();  // 初始化蜂鸣器

		delay_ms(500);       //上电瞬间加入一定延时在初始化
		LCD_Init();         //屏幕初始化
		delay_ms(300); 
		LCD1602_write_com(0x80);						//指针设置
		LCD1602_write_word("Welcome to use! ");
		To_Zero();
		LCD1602_write_com(0x80);						//指针设置
		LCD1602_write_word("WE: .    PR:  . ");
		LCD1602_write_com(0x80+0x40);				//指针设置
		LCD1602_write_word("MON:   .        ");
		Display_Price();


		while(1)
		{     
				 //每0.5秒称重一次
					if (test_interval ++ > 25)
					{
							test_interval = 0;
							//称重，得到重量值weight，单位为g
				get_weight();

							//如果超量程，显示‘-.---’
							if (weight >= 10000)
							{
									pre_weight = 10000;
									LCD1602_write_com(0x83);
									LCD1602_write_word("-.---");
								  BEEP_Set(1);
								
							}
							//如果不超量程
							else
							{
								  BEEP_Set(0);
									//显示重量值
									if (pre_weight != weight)
									{
											pre_weight = weight;
											Display_Weight();
									}
									//如果单价设定好了，则计算价格
									if (FlagSetPrice == 1)
									{
											money = weight*price/1000;  //money单位为分
											//显示总金额
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
					//获取按键
					keycode = Button4_4_Scan();
					delay(100);
					//有效键值0-15
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
	 

