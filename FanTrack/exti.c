/****************************************************************
 * �ļ���  ��exti.c
 * ����    ��I/O���жϺ���
 
*****************************************************************
#include "exti.h"

/*
 * ��������Key_NVIC_Configuration
 * ����  ������Ƕ�������жϿ�����NVIC
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void Key_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* ����P[A|B|C|D|E]0Ϊ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;	// 1���ⲿ�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*
 * ��������EXTI_PB0_Config
 * ����  ������ PB0 Ϊ���жϿڣ��������ж����ȼ�
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void Key_EXTI_PB1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline(PB0) clock and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);

	/* config the NVIC(PB0) */
	Key_NVIC_Configuration();

	/* EXTI line gpio config(PB1) */	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 // ��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* EXTI line(PB1) mode config */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1); 
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½����ж�
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure); 
}



