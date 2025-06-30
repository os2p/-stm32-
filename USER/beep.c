#include "beep.h"

void BEEP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // ����GPIOAʱ��

    GPIO_InitStructure.GPIO_Pin = BEEP_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BEEP_GPIO_Port, &GPIO_InitStructure);

    GPIO_ResetBits(BEEP_GPIO_Port, BEEP_Pin); // Ĭ�Ϲرշ�����
}

void BEEP_Set(uint8_t status)
{
    if (status)
        GPIO_SetBits(BEEP_GPIO_Port, BEEP_Pin);
    else
        GPIO_ResetBits(BEEP_GPIO_Port, BEEP_Pin);
}
