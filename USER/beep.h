#ifndef __BEEP_H
#define __BEEP_H

#include "sys.h"

//ʹ�� PA8
#define BEEP_Pin GPIO_Pin_8
#define BEEP_GPIO_Port GPIOA

void BEEP_Init(void);
void BEEP_Set(uint8_t status);  // 1 = ����0 = ��

#endif
