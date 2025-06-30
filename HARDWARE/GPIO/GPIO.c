#include "GPIO.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//按键的GPIO设置								  
////////////////////////////////////////////////////////////////////////////////// 	   

struct IO_PORT
{                                            
        GPIO_TypeDef *GPIO_x;                 
        unsigned short GPIO_pin;
};
static struct IO_PORT KEY_OUT[4] = {
        {BUTTON_ROW1_GPIO_PORT, BUTTON_ROW1_GPIO_PIN},
				{BUTTON_ROW2_GPIO_PORT, BUTTON_ROW2_GPIO_PIN},
        {BUTTON_ROW3_GPIO_PORT, BUTTON_ROW3_GPIO_PIN}, 
				{BUTTON_ROW4_GPIO_PORT, BUTTON_ROW4_GPIO_PIN}
};
static struct IO_PORT KEY_IN[4] = {
        {BUTTON_COL1_GPIO_PORT, BUTTON_COL1_GPIO_PIN}, 
				{BUTTON_COL2_GPIO_PORT, BUTTON_COL2_GPIO_PIN},
        {BUTTON_COL3_GPIO_PORT, BUTTON_COL3_GPIO_PIN}, 
				{BUTTON_COL4_GPIO_PORT, BUTTON_COL4_GPIO_PIN}
};
unsigned char key[4][4];
void HX711_GPIO_Init(void)
{

 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;				 // 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);		

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 // 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);			
	
}
void KEY_GPIO_Init(void)
{
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO |RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB , ENABLE); 
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);  //关闭JTAG模式 使PB3，PB4变成普通IO口
}




void Button4_4_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    unsigned char i;
	
//    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 

    RCC_APB2PeriphClockCmd(BUTTON_GPIO_CLK, ENABLE);      

		for(i=0;i<4;i++)
	{
		GPIO_InitStructure.GPIO_Pin = KEY_OUT[i].GPIO_pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_Init(KEY_OUT[i].GPIO_x, &GPIO_InitStructure);
	}
	
	for(i=0;i<4;i++)
	{
		GPIO_InitStructure.GPIO_Pin = KEY_IN[i].GPIO_pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_Init(KEY_IN[i].GPIO_x, &GPIO_InitStructure);
	}
  	

	for(i = 0; i < 4; i++)
	{
		GPIO_SetBits(KEY_OUT[i].GPIO_x, KEY_OUT[i].GPIO_pin);
	}
}
int Button4_4_Scan(void)
{
        unsigned char i, j;
        for(i = 0; i < 4; i++)            
        {
          delay_ms(5);
					GPIO_ResetBits(KEY_OUT[i].GPIO_x, KEY_OUT[i].GPIO_pin);   
          for(j = 0; j < 4; j++)            
          {
                  delay_ms(5); 
						      if(GPIO_ReadInputDataBit(KEY_IN[j].GPIO_x, KEY_IN[j].GPIO_pin) == 0)
                   {
                                key[i][j] = 1;
                   }else{
                                key[i][j] = 0;
                   }
          }
          GPIO_SetBits(KEY_OUT[i].GPIO_x, KEY_OUT[i].GPIO_pin);
        }
        if(key[0][0]==1)return 12;
        else if(key[0][1]==1)return 13;
        else if(key[0][2]==1)return 14;
        else if(key[0][3]==1)return 15;
        else if(key[1][0]==1)return 8;
        else if(key[1][1]==1)return 9;
        else if(key[1][2]==1)return 10;
        else if(key[1][3]==1)return 11;
        else if(key[2][0]==1)return 4;
        else if(key[2][1]==1)return 5;
        else if(key[2][2]==1)return 6;
        else if(key[2][3]==1)return 7;
        else if(key[3][0]==1)return 0;
        else if(key[3][1]==1)return 1;
        else if(key[3][2]==1)return 2;
        else if(key[3][3]==1)return 3;
				
				else return 99;
				
       
}

 




