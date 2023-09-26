/*****************************************************************************
* 文件名  ：main.c
* 描述    ：LCD触摸实验，开机先校验屏幕，画笔通过触摸板取色。         
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
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  ：无
 */
int main(void) 
{	
	unsigned char five=0;
    RCC_Configuration();   				 					/*时钟配置*/
    LCD_Init();												/*LCD初始化*/
    Touch_init();										 	/*触摸初始化*/
   while(Touchl_Calibrate() !=0);	 						/*等待触摸屏校准完毕*/ 
/*******************************开始工作**************************************************/
    while (1)																		
    {	 
		Init_Menu();	 
		//进入起始菜单
		while(flagflag==0)
		{	
				if((touch_flag==1)&&(flagflag3==1))							
			    {    
					{																
			            if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)									
			            { 		if((display.y>90)&&(display.y<140)||((240-display.x)>110)&&((240-display.x)<190) )
								{
												#ifdef HX8347    												   //
											    LCD_draw_rectangle(0,0,240,320,BLACK);	/*清白屏*/			   //
												#else														       //
												LCD_draw_rectangle(0,0,240,320,BLACK);	/*清白屏*/		   //
												#endif 		
								     	Init_Menu1();  
													 LED_GPIO_Config();
													 NRF24L01Int1();
													 while(five<=20)
													 {  
													 	five++;  
														NRFSetTxMode1(emission);//发送温度
												       	while(CheckACK1());	//检测是否发送完毕
														 GPIO_SetBits(GPIOC,GPIO_Pin_0);NRFDelay(1000);
														 GPIO_ResetBits(GPIOC,GPIO_Pin_0);NRFDelay(1000);	
													 } 		
									
									five=0;
									flagflag3=0;
									delay_ms(500);
									Init_Menu();
								/*************连续发送5次信息数组emission*******************/
											LCD_Show_8x16_String(180,10,1,emission1); 			 	   //
											LCD_Show_8x16_String(180,33,1,emission2); 				   //
											LCD_Show_8x16_String(180,56,1,emission3); 				   //					 	
								}							 			 			
			            }																			
			        } 
				}	
				//															
		        if(touch_flag == 1)									/*如果触笔按下了*/	//下面的就是判断坐标了	 	
		        {																	
		            if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)									
		            { 									 	
		              		Palette_draw_point(display.x,display.y);	 			 			
		            }																			
		        }
		}  //进入二级菜单
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
/*****************************验证各个通道**********************************************************/
/*int main(void)
{  	    
	uint i;	
	unsigned char yanzheng[5]={0};	
	 SystemInit();
	 LED_GPIO_Config();
	 NRF24L01Int();  
   while(1)
    {
		NRFSetRXMode();//设置为接收模式
		GetDate();     //开始接受数	  

  	     for(i=0;i<5;i++)
		 { 	 		      
		   yanzheng[i]=RevTempDate0[i]; //A点
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
//		NRFSetTxMode1(emission);//发送温度
//        while(CheckACK1());	//检测是否发送完毕
//		 GPIO_SetBits(GPIOC,GPIO_Pin_0);NRFDelay(1000);
//		 GPIO_ResetBits(GPIOC,GPIO_Pin_0);NRFDelay(1000);	
//	 } 	
//}
/* config the sysclock to 72M */
void RCC_Configuration(void)
{ 
    SystemInit();
}
