#ifndef _TOUCHPANEL_H_
#define _TOUCHPANEL_H_

#include "stm32f10x.h"


typedef	struct POINT 
{
   uint16_t x;		
   uint16_t y;
}Coordinate;


typedef struct Parameter 
{						
long double An,  			 //注:sizeof(long double) = 8
            Bn,     
            Cn,   
            Dn,    
            En,    
            Fn,     
            Divider ;
}Parameter ;




extern Coordinate ScreenSample[4];
extern Coordinate DisplaySample[4];
extern Parameter touch_para ;
extern Coordinate  display ;



/* AD通道选择命令字和工作寄存器 */
#define	CHX 	0x90 	/* 通道Y+的选择控制字 */	
#define	CHY 	0xd0	/* 通道X+的选择控制字 */



#define CS_2046(a)	if (a)	\
					GPIO_SetBits(GPIOB,GPIO_Pin_7);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_7)



#define INT_IN_2046   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)



			
void Touch_init(void);
void Init_Palette(void);
void Init_Menu(void);
void Init_Menu1(void);
void Init_Smenu(void);
void page1(unsigned int a,unsigned int b,unsigned int c);
void page2(void);
void page3(void);
void page4(void);
void page5(void);
void keyborad()	;
char  Get_touch_place(u16 *x, u16 *y);
void TOUCH_NVIC_Configuration(void);	
Coordinate *Read_2046(void);
Coordinate *Read_2046_2(void);
int Touchl_Calibrate(void);
void DrawCross(uint16_t Xpos,uint16_t Ypos);
void Palette_draw_point(uint16_t Xpos,uint16_t Ypos);
FunctionalState Cal_touch_para( Coordinate * displayPtr,Coordinate * screenPtr,Parameter * para);
FunctionalState Get_touch_point(Coordinate * displayPtr,Coordinate * screenPtr,Parameter * para );


#endif

