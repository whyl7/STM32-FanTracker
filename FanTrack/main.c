/*****************************************************************************
* �ļ���  ��main.c
* ����    ��LCD����ʵ�飬������У����Ļ������ͨ��������ȡɫ��         
**********************************************************************************/
#include "stm32f10x.h"
#include "lcd.h"
#include "sd_fs_app.h"
#include "SPI_Flash.h"
#include "systick.h"
#include "usart1.h"
#include "Touch.h"
#include "led.h"
#include "24l01.h"				
void RCC_Configuration(void);
extern volatile unsigned char touch_flag;
extern unsigned int flagflag,flagflag1,flagflag2;	
extern unsigned int flagflag3,flagflag4,flagflag5,flagflag6;
extern unsigned char emission[5];
extern unsigned char emission1[4];
extern unsigned char emission2[5];
extern unsigned char emission3[3];
/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 */
int main(void) 
{	
	unsigned char five=0;
    RCC_Configuration();   				 					/*ʱ������*/
    LCD_Init();												/*LCD��ʼ��*/
    Touch_init();										 	/*������ʼ��*/
   while(Touchl_Calibrate() !=0);	 						/*�ȴ�������У׼���*/ 
/*******************************��ʼ����**************************************************/
    while (1)																		
    {	 
		Init_Menu();	 
		//������ʼ�˵�
		while(flagflag==0)
		{	
				if((touch_flag==1)&&(flagflag3==1))							
			    {    
					{																
			            if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)									
			            { 		if((display.y>90)&&(display.y<140)||((240-display.x)>110)&&((240-display.x)<190) )
								{
												#ifdef HX8347    												   //
											    LCD_draw_rectangle(0,0,240,320,BLACK);	/*�����*/			   //
												#else														       //
												LCD_draw_rectangle(0,0,240,320,BLACK);	/*�����*/		   //
												#endif 		
								     	Init_Menu1();  
													 LED_GPIO_Config();
													 NRF24L01Int1();
													 while(five<=20)
													 {  
													 	five++;  
														NRFSetTxMode1(emission);//�����¶�
												       	while(CheckACK1());	//����Ƿ������
														 GPIO_SetBits(GPIOC,GPIO_Pin_0);NRFDelay(1000);
														 GPIO_ResetBits(GPIOC,GPIO_Pin_0);NRFDelay(1000);	
													 } 		
									
									five=0;
									flagflag3=0;
									delay_ms(500);
									Init_Menu();
								/*************��������5����Ϣ����emission*******************/
											LCD_Show_8x16_String(180,10,1,emission1); 			 	   //
											LCD_Show_8x16_String(180,33,1,emission2); 				   //
											LCD_Show_8x16_String(180,56,1,emission3); 				   //					 	
								}							 			 			
			            }																			
			        } 
				}	
				//															
		        if(touch_flag == 1)									/*������ʰ�����*/	//����ľ����ж�������	 	
		        {																	
		            if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)									
		            { 									 	
		              		Palette_draw_point(display.x,display.y);	 			 			
		            }																			
		        }
		}  //��������˵�
		while(flagflag1==1)			
		{	   
			flagflag2=1;
			  if(touch_flag == 1)	
			  {
			  	 if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)
				 {						 	
	              		Palette_draw_point(display.x,display.y); 				 	
				 }
			  }
		}																				 
    }
}		
/*****************************��֤����ͨ��**********************************************************/
/*int main(void)
{  	    
	uint i;	
	unsigned char yanzheng[5]={0};	
	 SystemInit();
	 LED_GPIO_Config();
	 NRF24L01Int();  
   while(1)
    {
		NRFSetRXMode();//����Ϊ����ģʽ
		GetDate();     //��ʼ������	  

  	     for(i=0;i<5;i++)
		 { 	 		      
		   yanzheng[i]=RevTempDate0[i]; //A��
		   if(yanzheng[i]!=0)
		   {   
//		   	GPIO_ResetBits(GPIOC,GPIO_Pin_0);NRFDelay(9900000);
//		      	GPIO_SetBits(GPIOC,GPIO_Pin_0);NRFDelay(9900000);					
			  
		   }
		 } 			  
	}

}*/  
//int main()
//{
//	 SystemInit();
//	 LED_GPIO_Config();
//	 NRF24L01Int1();
//	emission[0]=0x01;
//	 while(1)
//	 {  
//		NRFSetTxMode1(emission);//�����¶�
//        while(CheckACK1());	//����Ƿ������
//		 GPIO_SetBits(GPIOC,GPIO_Pin_0);NRFDelay(1000);
//		 GPIO_ResetBits(GPIOC,GPIO_Pin_0);NRFDelay(1000);	
//	 } 	
//}
/* config the sysclock to 72M */
void RCC_Configuration(void)
{ 
    SystemInit();
}
