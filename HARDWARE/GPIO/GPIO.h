#ifndef __GPIO_H
#define __GPIO_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 

#define ADSK   PBout(13)	
#define ADDO   PBin(12)	
#define PERSON_LNF   PBin(14)	
#define RELAY   PBout(7)	

// 4×4矩阵键盘 GPIO宏定义

#define		BUTTON_GPIO_CLK											RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB
#define 	BUTTON_ROW1_GPIO_PORT								GPIOA
#define 	BUTTON_ROW1_GPIO_PIN							  GPIO_Pin_15	
#define 	BUTTON_ROW2_GPIO_PORT								GPIOB
#define 	BUTTON_ROW2_GPIO_PIN								GPIO_Pin_3
#define 	BUTTON_ROW3_GPIO_PORT								GPIOB
#define 	BUTTON_ROW3_GPIO_PIN								GPIO_Pin_4
#define 	BUTTON_ROW4_GPIO_PORT								GPIOB
#define 	BUTTON_ROW4_GPIO_PIN								GPIO_Pin_5

#define 	BUTTON_COL1_GPIO_PORT								GPIOB
#define 	BUTTON_COL1_GPIO_PIN								GPIO_Pin_6
#define 	BUTTON_COL2_GPIO_PORT								GPIOB
#define 	BUTTON_COL2_GPIO_PIN								GPIO_Pin_7
#define 	BUTTON_COL3_GPIO_PORT								GPIOB
#define 	BUTTON_COL3_GPIO_PIN								GPIO_Pin_8
#define 	BUTTON_COL4_GPIO_PORT								GPIOB
#define 	BUTTON_COL4_GPIO_PIN								GPIO_Pin_9

	
/*********************END**********************/

void Button4_4_Init(void);
int Button4_4_Scan(void);

void BEEP_Init(void);//初始化

void KEY_GPIO_Init(void);

void HX711_GPIO_Init(void);

	 				    
#endif
