/**********************************
 * 文件名  ：Touch.c
 * 描述    ：LCD触摸应用函数库         
***********************************/
#include "Touch.h"
#include "systick.h"
#include "lcd.h"
#include "stm32f10x_spi.h"
#include "misc.h"
#include "stm32f10x_exti.h"
#include "led.h"
#include "24l01.h"	
unsigned int flagflag=0,flagflag1=0,flagflag2=0;
unsigned int flagflag3=0,flagflag4=0,flagflag5=0,flagflag6=0;	
extern volatile unsigned char touch_flag;
unsigned char emission[5]={0};					 //待发送的数组
unsigned char emission1[4]={0};
unsigned char emission2[5]={0};
unsigned char emission3[3]={0};

unsigned char change1[5]={0};
unsigned char change2[2]={0};
unsigned char change3[2]={0};

unsigned char change4[5]={0};
unsigned char change5[2]={0};
unsigned char change6[2]={0};

unsigned char change7[5]={0};
unsigned char change8[2]={0};
unsigned char change9[2]={0};
unsigned char total1[2]={0};
unsigned char total2[2]={0};
Parameter touch_para ;
Coordinate  display ;



/* DisplaySample LCD坐标上对应的ads7843采样AD值 如：LCD 坐标45,45 应该的X Y采样ADC分别为3388,920 */	
Coordinate ScreenSample[4];



/* LCD上的坐标 */
Coordinate DisplaySample[4] =   
{
    { 45, 45 },
    { 45, 270},
    { 190,190},
    {210,280}
} ;



/* 差值门限 */
#define THRESHOLD 2   





/******************************************************
* 函数名：Touch_spi_init
* 描述  ：2046 Touch SPI 初始化
* 输入  : 无
* 输出  ：无
* 举例  ：无
* 注意  ：无
*********************************************************/    
static void Touch_spi_init(void) 
{ 
    SPI_InitTypeDef  SPI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    
    /* DISABLE SPI1 */ 
    SPI_Cmd(SPI1, DISABLE); 											//SPI失能
    
    /* SPI1 配置 --*/ 
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 	//SPI 设置为双线双向全双工 
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 						//设置为主SPI 
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 					//SPI 发送接收8 位帧结构 
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; 							//时钟悬空低 
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; 						//数据捕获于第一个时钟沿 
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 							//内部NSS 信号有SSI 位控制 
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//波特率预分频值为256  
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 					//数据传输从MSB位开始 
    SPI_InitStructure.SPI_CRCPolynomial = 7; 							//
    SPI_Init(SPI1, &SPI_InitStructure); 
    
		/* Enable SPI1 */ 
    SPI_Cmd(SPI1, ENABLE); 												//使能
} 





/******************************************************
* 函数名：Touch_init
* 描述  ：2046 Touch 端口初始化
* 输入  : 无
* 输出  ：无
* 举例  ：无
* 注意  ：无
*********************************************************/    
void Touch_init(void) 
{ 
    EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
    
    /* 配置SPI1管脚: SCK--PA5, MISO--PA6 and MOSI--PA7 ---------------------------------*/ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5  | GPIO_Pin_6 | GPIO_Pin_7; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    /* 2046CS --PB7 */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* 2046 INT_IRQ --PB6*/
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD ;						//下拉输入 	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);				//功能复用IO时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);					//
    TOUCH_NVIC_Configuration();
    EXTI_InitStructure.EXTI_Line = EXTI_Line6;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource6);
    
    CS_2046(1); 
    Touch_spi_init(); 
} 





/******************************************************
* 函数名：TOUCH_NVIC_Configuration
* 描述  ：2046 Touch INT 嵌套中断配置
* 输入  : 无
* 输出  ：无
* 举例  ：无
* 注意  ：留意一下优先级
*********************************************************/    
void TOUCH_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    /*使能EXTI9_5 中断 */		      
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn  ;    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
}




/******************************************************
* 函数名：DelayUS
* 描述  ：粗略延时cnt 微妙
* 输入  : cnt
* 输出  ：无
* 举例  ：无
* 注意  ：该函数创建的目的是局部使用,从而不必特意去使用定时器来延时
*********************************************************/    
static void DelayUS(vu32 cnt)
{
    uint16_t i;
    for(i = 0;i<cnt;i++)
    {
        uint8_t us = 12; /* 设置值为12，大约延1微秒 */    
        while (us--)     /* 延1微秒	*/
        {
            ;   
        }
    }
}
/******************************************************
* 函数名：WR_CMD
* 描述  ：向 Touch 2046写数据
* 输入  : cmd 		--需要写入的数据
* 输出  ：无
* 举例  ：无
* 注意  ：内部使用
*********************************************************/    
static void WR_CMD (uint8_t cmd)  
{ 
    /* Wait for SPI1 Tx buffer empty */ 
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
    /* Send SPI1 data */ 
    SPI_I2S_SendData(SPI1,cmd); 
    /* Wait for SPI1 data reception */ 
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
    /* Read SPI1 received data */ 
    SPI_I2S_ReceiveData(SPI1); 
} 





/******************************************************
* 函数名：Read_touch_ad
* 描述  ：通过SPI方式读取 2046所获得的触摸AD转换值
* 输入  : 无
* 输出  ：无
* 举例  ：无
* 注意  ：内部使用
*********************************************************/    
static int Read_touch_ad(void)  
{ 
    unsigned short buf,temp; 
    /*  等待数据寄存器空 */  
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 

    /* 通过SPI1发送数据 */  
    SPI_I2S_SendData(SPI1,0x0000); 

    /* 等待接收到一个字节的数据 */ 
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 

    /* 读取SPI1接收的数据 */		 
    temp=SPI_I2S_ReceiveData(SPI1); 

    buf=temp<<8; 
    DelayUS(1);

		/*  等待数据寄存器空 */  
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 

    /* 通过SPI1发送数据 */ 
    SPI_I2S_SendData(SPI1,0x0000); 

    /* 等待接收到一个字节的数据 */ 
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
 
    /* 读取SPI1接收的数据 */ 
    temp=SPI_I2S_ReceiveData(SPI1); 
 
    buf |= temp; 
    buf>>=3; 
    buf&=0xfff; 
    return buf; 

} 







/******************************************************
* 函数名：Read_X
* 描述  ：通过SPI方式读取 2046所获得的触摸 通道X+ AD转换值
* 输入  : 无
* 输出  ：X+通道AD转换值
* 举例  ：无
* 注意  ：无
*********************************************************/    
int Read_X(void)  
{  
    int i; 
    CS_2046(0); 
    DelayUS(1); 
    WR_CMD(CHX); 
    DelayUS(1); 
    i=Read_touch_ad(); 
    CS_2046(1); 
    return i;    
} 






/******************************************************
* 函数名：Read_Y
* 描述  ：通过SPI方式读取 2046所获得的触摸 通道Y+ AD转换值
* 输入  : 无
* 输出  ：Y+通道AD转换值
* 举例  ：无
* 注意  ：无
*********************************************************/    
int Read_Y(void)  
{  
    int i; 
    CS_2046(0); 
    DelayUS(1); 
    WR_CMD(CHY); 
    DelayUS(1); 
    i=Read_touch_ad(); 
    CS_2046(1); 
    return i;     
} 






/******************************************************
* 函数名：Touch_GetAdXY
* 描述  ：SPI方式 读取2046 通道X+ 通道Y+的ADC值
* 输入  : 无
* 输出  ：通道X+ 通道Y+的ADC值
* 举例  ：无
* 注意  ：无
*********************************************************/    
void Touch_GetAdXY(int *x,int *y)  
{ 
    int adx,ady; 
    adx=Read_X(); 
    DelayUS(1); 
    ady=Read_Y(); 
    *x=adx; 
    *y=ady; 
} 






/******************************************************
* 函数名：Palette_draw_point
* 描述  ：在LCD指定位置画一个大点(包含四个小点)
* 输入  : Xpos		--X方向位置
Ypos		--Y方向位置
* 输出  ：无
* 举例  ：Palette_draw_point(100,100);
* 注意  ：该函数是 "触摸画板应用实例" 专用函数
*********************************************************/    
void Palette_draw_point(uint16_t Xpos,uint16_t Ypos)
{
	unsigned int flag=0,flag1=0,flag2=0,flag3=0,falg4=0;
	u8 i=0;
	unsigned char yanzheng[5]={0};
	unsigned char electricprice1=0,electricprice2=0,electricprice3=0,electricprice4=0;

    static u16 Pen_color=0;
    u16 Tmp_xpos = 240-Xpos;

	if((Ypos<60)&&(flagflag==0))
	{	  			

			if((Tmp_xpos>210)&&(Tmp_xpos<240))
			{
					Init_Smenu();
					flagflag=1;	   //退出第一个大循环
					flagflag1=1;	//进入二级菜单的标志位	
			}  
			Ypos=240;
	}
	if(((Ypos>0)&&(Ypos<140))&&(flagflag1==1)&&(flagflag2=1))
	{
			if((Tmp_xpos>210)&&(Tmp_xpos<240))
			{
				#ifdef HX8347    												   //
				    LCD_draw_rectangle(0,0,240,320,BLACK);	/*清白屏*/			   //
				#else														       //
						LCD_draw_rectangle(0,0,240,320,BLACK);	/*清白屏*/		   //
				#endif															   //  
										/*nrf24l01初始化*/
				for(i=0;i<7;i++)
				{
					  RevTempDate0[i]='0';
					  RevTempDate1[i]='0';
					  RevTempDate2[i]='0';
				}
				for(i=0;i<4;i++)
				{
					change1[i]='0';
					change4[i]='0';
					change7[i]='0';
				}
				while(1)
				{  	
/************************显示nrf24l01接收到的数据******（还需要解码）**********************/	
		LED_GPIO_Config(); 
		NRF24L01Int();	
		NRFSetRXMode();//设置为接收模式													  //
		GetDate();     //开始接受数	  								  						  //
//  	     for(i=0;i<5;i++)																  //
//		 { 																				  //
//		   yanzheng[i]=RevTempDate2[i]; //A点											  //
//		   if(yanzheng[i]!=0)															  //
//		   {																			  //
//		      GPIO_SetBits(GPIOC,GPIO_Pin_0);NRFDelay(9900000);							  //
//			  GPIO_ResetBits(GPIOC,GPIO_Pin_0);NRFDelay(9900000);						  //
//		   }																			  //
//		 } 																				  //
/******************************************************************************************/
			switch(RevTempDate0[0])														  //
					{																	  //
						case 0x01: change1[0]='1'; break;								  //
						case 0x02: change1[0]='2'; break;								  //
						case 0x03: change1[0]='3'; break;								  //
						case 0x04: change1[0]='4'; break;								  //
					}																	  //
					switch(RevTempDate0[1])												  //
					{																	  //
						case 0x00: change1[1]='0'; break;								  //
						case 0x01: change1[1]='1'; break;								  //
						case 0x02: change1[1]='2'; break;								  //
						case 0x03: change1[1]='3'; break;								  //
						case 0x04: change1[1]='4'; break;								  //
						case 0x05: change1[1]='5'; break;								  //
						case 0x06: change1[1]='6'; break;								  //
						case 0x07: change1[1]='7'; break;								  //
						case 0x08: change1[1]='8'; break;								  //
						case 0x09: change1[1]='9'; break;								  //
					}																	  //
					change1[2]='.';														  //
					switch(RevTempDate0[2])												  //
					{																	  //
						case 0x00: change1[3]='0'; break;								  //
						case 0x01: change1[3]='1'; break;								  //
						case 0x02: change1[3]='2'; break;								  //
						case 0x03: change1[3]='3'; break;								  //
						case 0x04: change1[3]='4'; break;								  //
						case 0x05: change1[3]='5'; break;								  //
						case 0x06: change1[3]='6'; break;								  //
						case 0x07: change1[3]='7'; break;								  //
						case 0x08: change1[3]='8'; break;								  //
						case 0x09: change1[3]='9'; break;								  //
					}																	  //
					/**************转速级别**************/								  //
					switch(RevTempDate0[3])												  //
					{																	  //
						case 0x00: change2[0]='0'; break;								  //
						case 0x01: change2[0]='1'; break;								  //
						case 0x02: change2[0]='2'; break;								  //
						case 0x03: change2[0]='3'; break;								  //
						case 0x04: change2[0]='4'; break;								  //
						case 0x05: change2[0]='5'; break;								  //
						case 0x06: change2[0]='6'; break;								  //
						case 0x07: change2[0]='7'; break;								  //
						case 0x08: change2[0]='8'; break;								  //
						case 0x09: change2[0]='9'; break;								  //
					}																	  //
																						  //
		switch(RevTempDate1[0])															  //
					{																	  //
						case 0x01: change4[0]='1'; break;								  //
						case 0x02: change4[0]='2'; break;								  //
						case 0x03: change4[0]='3'; break;								  //
						case 0x04: change4[0]='4'; break;								  //
					}																	  //
					switch(RevTempDate1[1])												  //
					{																	  //
						case 0x00: change4[1]='0'; break;								  //
						case 0x01: change4[1]='1'; break;								  //
						case 0x02: change4[1]='2'; break;								  //
						case 0x03: change4[1]='3'; break;								  //
						case 0x04: change4[1]='4'; break;								  //
						case 0x05: change4[1]='5'; break;								  //
						case 0x06: change4[1]='6'; break;								  //
						case 0x07: change4[1]='7'; break;								  //
						case 0x08: change4[1]='8'; break;								  //
						case 0x09: change4[1]='9'; break;								  //
					}																	  //
					change4[2]='.';														  //
					switch(RevTempDate1[2])												  //
					{																	  //
						case 0x00: change4[3]='0'; break;								  //
						case 0x01: change4[3]='1'; break;								  //
						case 0x02: change4[3]='2'; break;								  //
						case 0x03: change4[3]='3'; break;								  //
						case 0x04: change4[3]='4'; break;								  //
						case 0x05: change4[3]='5'; break;								  //
						case 0x06: change4[3]='6'; break;								  //
						case 0x07: change4[3]='7'; break;								  //
						case 0x08: change4[3]='8'; break;								  //
						case 0x09: change4[3]='9'; break;								  //
					}																	  //
					/**************转速级别**************/								  //
					switch(RevTempDate1[3])												  //
					{																	  //
						case 0x00: change5[0]='0'; break;								  //
						case 0x01: change5[0]='1'; break;								  //
						case 0x02: change5[0]='2'; break;								  //
						case 0x03: change5[0]='3'; break;								  //
						case 0x04: change5[0]='4'; break;								  //
						case 0x05: change5[0]='5'; break;								  //
						case 0x06: change5[0]='6'; break;								  //
						case 0x07: change5[0]='7'; break;								  //
						case 0x08: change5[0]='8'; break;								  //
						case 0x09: change5[0]='9'; break;								  //
					}																	  //
																						  //
		switch(RevTempDate2[0])															  //
					{																	  //
						case 0x01: change7[0]='1'; break;								  //
						case 0x02: change7[0]='2'; break;								  //
						case 0x03: change7[0]='3'; break;								  //
						case 0x04: change7[0]='4'; break;								  //
					}																	  //
					switch(RevTempDate2[1])												  //
					{																	  //
						case 0x00: change7[1]='0'; break;								  //
						case 0x01: change7[1]='1'; break;								  //
						case 0x02: change7[1]='2'; break;								  //
						case 0x03: change7[1]='3'; break;								  //
						case 0x04: change7[1]='4'; break;								  //
						case 0x05: change7[1]='5'; break;								  //
						case 0x06: change7[1]='6'; break;								  //
						case 0x07: change7[1]='7'; break;								  //
						case 0x08: change7[1]='8'; break;								  //
						case 0x09: change7[1]='9'; break;								  //
					}																	  //
					change7[2]='.';														  //
					switch(RevTempDate2[2])												  //
					{																	  //
						case 0x00: change7[3]='0'; break;								  //
						case 0x01: change7[3]='1'; break;								  //
						case 0x02: change7[3]='2'; break;								  //
						case 0x03: change7[3]='3'; break;								  //
						case 0x04: change7[3]='4'; break;								  //
						case 0x05: change7[3]='5'; break;								  //
						case 0x06: change7[3]='6'; break;								  //
						case 0x07: change7[3]='7'; break;								  //
						case 0x08: change7[3]='8'; break;								  //
						case 0x09: change7[3]='9'; break;								  //
					}																	  //
					/**************转速级别**************/								  //
					switch(RevTempDate2[3])												  //
					{																	  //
						case 0x00: change8[0]='0'; break;								  //
						case 0x01: change8[0]='1'; break;								  //
						case 0x02: change8[0]='2'; break;								  //
						case 0x03: change8[0]='3'; break;								  //
						case 0x04: change8[0]='4'; break;								  //
						case 0x05: change8[0]='5'; break;								  // 
						case 0x06: change8[0]='6'; break;								  //
						case 0x07: change8[0]='7'; break;								  //
						case 0x08: change8[0]='8'; break;								  //
						case 0x09: change8[0]='9'; break;								  //
					}
				page1(RevTempDate0[5],RevTempDate1[5],RevTempDate2[5]);	
				page1(1,0,0);									 								  //
				LCD_Show_8x16_String(118,170,1,change1);								  //
				NRFDelay(9);
				LCD_Show_8x16_String(118,150,1,change2);								  //
				NRFDelay(9);
				LCD_Show_8x16_String(228,120,1,change4); 								  //
				NRFDelay(9);
				LCD_Show_8x16_String(228,100,1,change5); 								  //
				NRFDelay(9);
				LCD_Show_8x16_String(228,210,1,change7); 								  //
				NRFDelay(9);
				LCD_Show_8x16_String(228,190,1,change8);  								  //
				NRFDelay(9);
/******************************************************************************************/
					if(touch_flag == 1)									/*如果触笔按下了*/	//下面的就是判断坐标了	 	
			        {																	
			            if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)									
			            { 									 	
			              	Tmp_xpos = 240-display.x;	Ypos=display.y;	 
			            }																			
			        } 
					if(((Ypos>240)&&((Tmp_xpos>210)&&(Tmp_xpos<240))))
					{			break;}
				}
			}
			if((Tmp_xpos>178)&&(Tmp_xpos<208))
			{
				#ifdef HX8347    											
				    LCD_draw_rectangle(0,0,240,320,BLACK);	
				#else														   
						LCD_draw_rectangle(0,0,240,320,BLACK);	
				#endif	
				page2();
										/*nrf24l01初始化*/	
				while(1)
				{ 		
			LED_GPIO_Config();
			NRF24L01Int();	
/************************显示nrf24l01接收到的数据******（还需要解码）**********************/
		NRFSetRXMode();//设置为接收模式													  //
		GetDate();     //开始接受数	  							  						  //
//  	     for(i=0;i<5;i++)																  //
//		 { 																				  //
//		   yanzheng[i]=RevTempDate2[i]; //A点											  //
//		   if(yanzheng[i]!=0)															  //
//		   {																			  //
//		      GPIO_SetBits(GPIOC,GPIO_Pin_0);NRFDelay(9900000);							  //
//			  GPIO_ResetBits(GPIOC,GPIO_Pin_0);NRFDelay(9900000);						  //
//		   }																			  //
//		 } 																				  //
/******************************************************************************************/
					switch(RevTempDate0[4])												  //
					{																	  //
						case 0x01: change3[0]='1';electricprice1=1; break;		      //
						default :electricprice1=0;break;		
					}																	  //
					switch(RevTempDate1[4])												  //
					{																	  //
						case 0x01: change6[0]='1';electricprice2=1; break;		      //
						default :electricprice2=0;break;		
					}																	  //
					switch(RevTempDate2[4])												  //
					{																	  //
						case 0x01: change9[0]='1';electricprice3=1; break;		      //
						default :electricprice3=0;break;				
					}																	  //
   				electricprice4=electricprice4+electricprice1+electricprice2+electricprice3;
					electricprice1=0;
					electricprice2=0;
					electricprice3=0;	
					total1[0]=electricprice4+0x30;
					total2[0]=electricprice4*emission[1]+0x30;
					LCD_Show_8x16_String(10,50,1,total1);
					LCD_Show_8x16_String(10,70,1,total1);						  
					if(touch_flag == 1)									/*如果触笔按下了*/	//下面的就是判断坐标了	 	
			        {																	
			            if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)									
			            { 									 	
			              	Tmp_xpos = 240-display.x;	Ypos=display.y;	 			 			
			            }																			
			        } 
					if(((Ypos>240)&&((Tmp_xpos>210)&&(Tmp_xpos<240))))
					{break;}
				}
			}
			if((Tmp_xpos>146)&&(Tmp_xpos<176))
			{
				#ifdef HX8347    												   //
				    LCD_draw_rectangle(0,0,240,320,BLACK);	/*清白屏*/			   //
				#else														       //
						LCD_draw_rectangle(0,0,240,320,BLACK);	/*清白屏*/		   //
				#endif	
				page3();
				 emission[0]=0x01; 	
				while(1)
				{
					if(touch_flag == 1)									/*如果触笔按下了*/	//下面的就是判断坐标了	 	
			        {																	
			            if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)									
			            { 									 	
			              	Tmp_xpos = 240-display.x;	Ypos=display.y;	 			 			
			            }																			
			        } 
				/************************温度差值存数组emission[0]**************************/
				if(((Tmp_xpos>100)&&(Tmp_xpos<130))&&(((Ypos>10)&&(Ypos<40))))
				{emission[0]=0x01;LCD_Show_8x16_String(20,12,1,"0.5");
				emission1[0]='0';emission1[1]='.';emission1[2]='5';} 
				if(((Tmp_xpos>100)&&(Tmp_xpos<130))&&(((Ypos>50)&&(Ypos<80))))
				{emission[0]=0x02;LCD_Show_8x16_String(20,12,1,"1.0");
				emission1[0]='1';emission1[1]='.';emission1[2]='0';}
				if(((Tmp_xpos>100)&&(Tmp_xpos<130))&&(((Ypos>90)&&(Ypos<120))))
				{emission[0]=0x03;LCD_Show_8x16_String(20,12,1,"1.5");
				emission1[0]='1';emission1[1]='.';emission1[2]='5';}		
				if(((Tmp_xpos>100)&&(Tmp_xpos<130))&&(((Ypos>130)&&(Ypos<160))))
				{emission[0]=0x04;LCD_Show_8x16_String(20,12,1,"2.0");
				emission1[0]='2';emission1[1]='.';emission1[2]='0';}
				if(((Tmp_xpos>100)&&(Tmp_xpos<130))&&(((Ypos>170)&&(Ypos<200))))
				{emission[0]=0x05;LCD_Show_8x16_String(20,12,1,"2.5");
				emission1[0]='2';emission1[1]='.';emission1[2]='5';}
				if(((Tmp_xpos>150)&&(Tmp_xpos<180))&&(((Ypos>10)&&(Ypos<40))))
				{emission[0]=0x06;LCD_Show_8x16_String(20,12,1,"3.0");
				emission1[0]='3';emission1[1]='.';emission1[2]='0';}
				if(((Tmp_xpos>150)&&(Tmp_xpos<180))&&(((Ypos>50)&&(Ypos<80))))
				{emission[0]=0x07;LCD_Show_8x16_String(20,12,1,"3.5");
				emission1[0]='3';emission1[1]='.';emission1[2]='5';}
				if(((Tmp_xpos>150)&&(Tmp_xpos<180))&&(((Ypos>90)&&(Ypos<120))))
				{emission[0]=0x08;LCD_Show_8x16_String(20,12,1,"4.0");
				emission1[0]='4';emission1[1]='.';emission1[2]='0';}		
				if(((Tmp_xpos>150)&&(Tmp_xpos<180))&&(((Ypos>130)&&(Ypos<160))))
				{emission[0]=0x09;LCD_Show_8x16_String(20,12,1,"4.5");
				emission1[0]='4';emission1[1]='.';emission1[2]='5';}
				if(((Tmp_xpos>150)&&(Tmp_xpos<180))&&(((Ypos>170)&&(Ypos<200))))
				{emission[0]=0x10;LCD_Show_8x16_String(20,12,1,"5.0");
				emission1[0]='5';emission1[1]='.';emission1[2]='0';}
				/***************************************************************************/
					if(((Ypos>240)&&((Tmp_xpos>210)&&(Tmp_xpos<240))))
					{flagflag6=1;break;}
				}
			}
			if((Tmp_xpos>114)&&(Tmp_xpos<144))
			{
				#ifdef HX8347    												   //
				    LCD_draw_rectangle(0,0,240,320,BLACK);	/*清白屏*/			   //
				#else														       //
						LCD_draw_rectangle(0,0,240,320,BLACK);	/*清白屏*/		   //
				#endif	
				page4();
				while(1)
				{
					if(touch_flag == 1)									/*如果触笔按下了*/	//下面的就是判断坐标了	 	
			        {																	
			            if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)									
			            { 									 	
			              	Tmp_xpos = 240-display.x;	Ypos=display.y;	 			 			
			            }																			
			        } 
					/************************价格*********************/
				if(((Tmp_xpos>100)&&(Tmp_xpos<130))&&(((Ypos>10)&&(Ypos<40))))
				{LCD_Show_8x16_String(15,12,1,"0.58");			 //emission[1]=0x01;
				emission2[0]='0';emission2[1]='.';emission2[2]='5';emission2[3]='8';}
				if(((Tmp_xpos>100)&&(Tmp_xpos<130))&&(((Ypos>50)&&(Ypos<80))))
				{LCD_Show_8x16_String(15,12,1,"0.59");	      	 //emission[1]=0x02;
				emission2[0]='0';emission2[1]='.';emission2[2]='5';emission2[3]='9';}
				if(((Tmp_xpos>100)&&(Tmp_xpos<130))&&(((Ypos>90)&&(Ypos<120))))
				{LCD_Show_8x16_String(15,12,1,"0.60");		 	 //emission[1]=0x03;
				emission2[0]='0';emission2[1]='.';emission2[2]='6';emission2[3]='0';}		
				if(((Tmp_xpos>100)&&(Tmp_xpos<130))&&(((Ypos>130)&&(Ypos<160))))
				{LCD_Show_8x16_String(15,12,1,"0.61");	 	  	 //emission[1]=0x04;
				emission2[0]='0';emission2[1]='.';emission2[2]='6';emission2[3]='1';}
				if(((Tmp_xpos>100)&&(Tmp_xpos<130))&&(((Ypos>170)&&(Ypos<200))))
				{LCD_Show_8x16_String(15,12,1,"0.62");		     //emission[1]=0x05;
				emission2[0]='0';emission2[1]='.';emission2[2]='6';emission2[3]='2';}
				if(((Tmp_xpos>150)&&(Tmp_xpos<180))&&(((Ypos>10)&&(Ypos<40))))
				{LCD_Show_8x16_String(15,12,1,"0.63");			 //emission[1]=0x06;
				emission2[0]='0';emission2[1]='.';emission2[2]='6';emission2[3]='3';}
				if(((Tmp_xpos>150)&&(Tmp_xpos<180))&&(((Ypos>50)&&(Ypos<80))))
				{LCD_Show_8x16_String(15,12,1,"0.64");			 //emission[1]=0x07;
				emission2[0]='0';emission2[1]='.';emission2[2]='6';emission2[3]='4';}
				if(((Tmp_xpos>150)&&(Tmp_xpos<180))&&(((Ypos>90)&&(Ypos<120))))
				{LCD_Show_8x16_String(15,12,1,"0.65");			 //emission[1]=0x08;
				emission2[0]='0';emission2[1]='.';emission2[2]='6';emission2[3]='5';}		
				if(((Tmp_xpos>150)&&(Tmp_xpos<180))&&(((Ypos>130)&&(Ypos<160))))
				{LCD_Show_8x16_String(15,12,1,"0.66");			// emission[1]=0x09;
				emission2[0]='0';emission2[1]='.';emission2[2]='6';emission2[3]='6';}
				if(((Tmp_xpos>150)&&(Tmp_xpos<180))&&(((Ypos>170)&&(Ypos<200))))
				{LCD_Show_8x16_String(15,12,1,"0.67");		 	 //emission[1]=0x10;
				emission2[0]='0';emission2[1]='.';emission2[2]='6';emission2[3]='7';}

					/*******************************************************************/
					if(((Ypos>240)&&((Tmp_xpos>210)&&(Tmp_xpos<240))))
					{flagflag4=1;break;}
				}
			}
			if((Tmp_xpos>82)&&(Tmp_xpos<112))
			{
				#ifdef HX8347    												   //
				    LCD_draw_rectangle(0,0,240,320,BLACK);	/*清白屏*/			   //
				#else														       //
						LCD_draw_rectangle(0,0,240,320,BLACK);	/*清白屏*/		   //
				#endif	
				page5();
				i=0;
				while(1)
				{
					if(touch_flag == 1)									/*如果触笔按下了*/	//下面的就是判断坐标了	 	
			        {																	
			            if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)									
			            { 									 	
			              	Tmp_xpos = 240-display.x;	Ypos=display.y;	
							if((Tmp_xpos>150)&&(Tmp_xpos<180)&&(Ypos>260)&&(Ypos<310))
							{	i++;
								if(i==1)
								{
									#ifdef HX8347    											  
									    LCD_draw_rectangle(0,0,240,320,BLACK);	
									#else														  
											LCD_draw_rectangle(0,0,240,320,BLACK);	
									#endif													
								}
								keyborad();
							}		 			
			            }																			
			        }
					/************************阈值温度存数组emisson[1]*******************/
				if(((Tmp_xpos>100)&&(Tmp_xpos<130))&&(((Ypos>10)&&(Ypos<40))))
				{emission[1]=0x01;LCD_Show_8x16_String(20,12,1,"25");
				emission3[0]='2';emission3[1]='5';}
				if(((Tmp_xpos>100)&&(Tmp_xpos<130))&&(((Ypos>50)&&(Ypos<80))))
				{emission[1]=0x02;LCD_Show_8x16_String(20,12,1,"26");
				emission3[0]='2';emission3[1]='6';}
				if(((Tmp_xpos>100)&&(Tmp_xpos<130))&&(((Ypos>90)&&(Ypos<120))))
				{emission[1]=0x03;LCD_Show_8x16_String(20,12,1,"27");
				emission3[0]='2';emission3[1]='7';}		
				if(((Tmp_xpos>100)&&(Tmp_xpos<130))&&(((Ypos>130)&&(Ypos<160))))
				{emission[1]=0x04;LCD_Show_8x16_String(20,12,1,"28");
				emission3[0]='2';emission3[1]='8';}
				if(((Tmp_xpos>100)&&(Tmp_xpos<130))&&(((Ypos>170)&&(Ypos<200))))
				{emission[1]=0x05;LCD_Show_8x16_String(20,12,1,"29");
				emission3[0]='2';emission3[1]='9';}
				if(((Tmp_xpos>150)&&(Tmp_xpos<180))&&(((Ypos>10)&&(Ypos<40))))
				{emission[1]=0x06;LCD_Show_8x16_String(20,12,1,"30");
				emission3[0]='3';emission3[1]='0';}
				if(((Tmp_xpos>150)&&(Tmp_xpos<180))&&(((Ypos>50)&&(Ypos<80))))
				{emission[1]=0x07;LCD_Show_8x16_String(20,12,1,"31");
				emission3[0]='3';emission3[1]='1';}
				if(((Tmp_xpos>150)&&(Tmp_xpos<180))&&(((Ypos>90)&&(Ypos<120))))
				{emission[1]=0x08;LCD_Show_8x16_String(20,12,1,"32");
				emission3[0]='3';emission3[1]='2';}		
				if(((Tmp_xpos>150)&&(Tmp_xpos<180))&&(((Ypos>130)&&(Ypos<160))))
				{emission[1]=0x09;LCD_Show_8x16_String(20,12,1,"33");
				emission3[0]='3';emission3[1]='3';}
				if(((Tmp_xpos>150)&&(Tmp_xpos<180))&&(((Ypos>170)&&(Ypos<200))))
				{emission[1]=0x10;LCD_Show_8x16_String(20,12,1,"34");
				emission3[0]='3';emission3[1]='4';}

					/*******************************************************************/
					if(((Ypos>240)&&((Tmp_xpos>210)&&(Tmp_xpos<240))))
					{flagflag5=1;break;}
				}
			}
	}	  			/*返回*/		
		if((Ypos>240)&&((Tmp_xpos>210)&&(Tmp_xpos<240)))
		{
			if(flagflag==1)
			{
				#ifdef HX8347    												   //
				    LCD_draw_rectangle(0,0,240,320,BLACK);	/*清白屏*/			   //
				#else														       //
						LCD_draw_rectangle(0,0,240,320,BLACK);	/*清白屏*/		   //
				#endif
				flagflag=0;
				flagflag1=0; 
				flagflag2=0; 
				if((flagflag4==1)&&(flagflag5==1)&&(flagflag6==1))
				{	
					flagflag4=0;
					flagflag5=0;
					flagflag6=0;
					flagflag3=1;   
				}
			 }
			 else
			 {
				#ifdef HX8347    												   //
				    LCD_draw_rectangle(0,0,240,320,BLACK);	/*清白屏*/			   //
				#else														       //
						LCD_draw_rectangle(0,0,240,320,BLACK);	/*清白屏*/		   //
				#endif			 
			 }
		}		 						
}	
/******************************************************
* 函数名：DrawCross
* 描述  ：在LCD指定位置画十字
* 输入  : Xpos		--X方向位置
Ypos		--Y方向位置
* 输出  ：无
* 举例  ：DrawCross(100,100);
* 注意  ：触摸校正专用
*********************************************************/    
void DrawCross(uint16_t Xpos,uint16_t Ypos)
{
    u8 b;    
    for(b=0; b<10; b++)
    {
        LCD_DrawPoint(240-(Xpos),Ypos-b);
        LCD_DrawPoint(240-(Xpos),Ypos+b);
        LCD_DrawPoint(240-(Xpos+b),Ypos);
        LCD_DrawPoint(240-(Xpos-b),Ypos);
    }
    
    
}	



/******************************************************
* 函数名：Read_2046
* 描述  ：得到滤波之后的X Y
* 输入  : 无
* 输出  ：Coordinate结构体地址
* 举例  ：无
* 注意  ：速度相对比较慢
*********************************************************/    
Coordinate *Read_2046(void)
{
    static Coordinate  screen;
    int m0,m1,m2,TP_X[1],TP_Y[1],temp[3];
    uint8_t count=0;
		
		/* 坐标X和Y进行9次采样*/
    int buffer[2][9]={{0},{0}};  	
    do
    {		   
        Touch_GetAdXY(TP_X,TP_Y);  
        buffer[0][count]=TP_X[0];  
        buffer[1][count]=TP_Y[0];
        count++; 
				 
    }	/*用户点击触摸屏时即TP_INT_IN信号为低 并且 count<9*/
    while(!INT_IN_2046&& count<9);  
    
		
		/*如果触笔弹起*/
		if(INT_IN_2046 ==1)
    {
        /*中断标志复位*/
				touch_flag =0;						
    }

		/* 如果成功采样9次,进行滤波 */ 
    if(count==9)   								
    {  
        /* 为减少运算量,分别分3组取平均值 */
        temp[0]=(buffer[0][0]+buffer[0][1]+buffer[0][2])/3;
        temp[1]=(buffer[0][3]+buffer[0][4]+buffer[0][5])/3;
        temp[2]=(buffer[0][6]+buffer[0][7]+buffer[0][8])/3;
        
        /* 计算3组数据的差值 */
        m0=temp[0]-temp[1];
        m1=temp[1]-temp[2];
        m2=temp[2]-temp[0];
        
        /* 对上述差值取绝对值 */
        m0=m0>0?m0:(-m0);
        m1=m1>0?m1:(-m1);
        m2=m2>0?m2:(-m2);
        
        /* 判断绝对差值是否都超过差值门限，如果这3个绝对差值都超过门限值，则判定这次采样点为野点,抛弃采样点，差值门限取为2 */
        if( m0>THRESHOLD  &&  m1>THRESHOLD  &&  m2>THRESHOLD ) 
            return 0;
        
        /* 计算它们的平均值，同时赋值给screen */ 
        if(m0<m1)
        {
            if(m2<m0) 
                screen.x=(temp[0]+temp[2])/2;
            else 
                screen.x=(temp[0]+temp[1])/2;	
        }
        else if(m2<m1) 
            screen.x=(temp[0]+temp[2])/2;
        else 
            screen.x=(temp[1]+temp[2])/2;
        
        /* 同上 计算Y的平均值 */
        temp[0]=(buffer[1][0]+buffer[1][1]+buffer[1][2])/3;
        temp[1]=(buffer[1][3]+buffer[1][4]+buffer[1][5])/3;
        temp[2]=(buffer[1][6]+buffer[1][7]+buffer[1][8])/3;
        m0=temp[0]-temp[1];
        m1=temp[1]-temp[2];
        m2=temp[2]-temp[0];
        m0=m0>0?m0:(-m0);
        m1=m1>0?m1:(-m1);
        m2=m2>0?m2:(-m2);
        if(m0>THRESHOLD&&m1>THRESHOLD&&m2>THRESHOLD) 
            return 0;
        
        if(m0<m1)
        {
            if(m2<m0) 
                screen.y=(temp[0]+temp[2])/2;
            else 
                screen.y=(temp[0]+temp[1])/2;	
        }
        else if(m2<m1) 
            screen.y=(temp[0]+temp[2])/2;
        else
            screen.y=(temp[1]+temp[2])/2;
        
        return &screen;
    }
    
    else if(count>1)
    {
        screen.x=buffer[0][0];
        screen.y=buffer[1][0];
        return &screen;
    }  
    return 0; 
}




/******************************************************
* 函数名：Read_2046
*  描述 ：得到简单滤波之后的X Y
* 输入  : 无
* 输出  ：Coordinate结构体地址
* 举例  ：无
* 注意  ：”画板应用实例"专用,不是很精准，但是速度比较快
*********************************************************/    
Coordinate *Read_2046_2(void)
{
    static Coordinate  screen2;
    int TP_X[1],TP_Y[1];
    uint8_t count=0;
    int buffer[2][10]={{0},{0}};  /*坐标X和Y进行多次采样*/
    int min_x,max_x;
    int min_y,max_y;
    int	i=0;
    
    do					       				
    {		/* 循环采样10次 */   
        Touch_GetAdXY(TP_X,TP_Y);  
        buffer[0][count]=TP_X[0];  
        buffer[1][count]=TP_Y[0];
        count++;  
    }	/*用户点击触摸屏时即TP_INT_IN信号为低 并且 count<10*/
    while(!INT_IN_2046&& count<10);
    
		/*如果触笔弹起*/
    if(INT_IN_2046)						
    {
				/*中断标志复位*/
        touch_flag = 0;					 
    }
		
		/*如果成功采样10个样本*/
    if(count ==10)		 					
    {
        max_x=min_x=buffer[0][0];
        max_y=min_y=buffer[1][0];
        for(i=1; i<10; i++)
        {
            if(buffer[0][i]<min_x)
            {
                min_x=buffer[0][i];
            }
            else
            if(buffer[0][i]>max_x)
            {
                max_x = buffer[0][i];
            }
        }
        
        for(i=1; i<10; i++)
        {
            if(buffer[1][i]<min_y)
            {
                min_y=buffer[1][i];
            }
            else
            if(buffer[1][i]>max_y)
            {
                max_y = buffer[1][i];
            }
        }

				/*去除最小值和最大值之后求平均值*/
        screen2.x=(buffer[0][0]+buffer[0][1]+buffer[0][2]+buffer[0][3]+buffer[0][4]+buffer[0][5]+buffer[0][6]+buffer[0][7]+buffer[0][8]+buffer[0][9]-min_x-max_x)>>3;
        screen2.y=(buffer[1][0]+buffer[1][1]+buffer[1][2]+buffer[1][3]+buffer[1][4]+buffer[1][5]+buffer[1][6]+buffer[1][7]+buffer[1][8]+buffer[1][9]-min_y-max_y)>>3; 
        
        return &screen2;
    }
    
    return 0;
    
} 






/******************************************************
* 函数名：Cal_touch_para
* 描述  ：计算出触摸屏到液晶屏坐标变换的转换函数的 K A B C D E F系数
* 输入  : 无
* 输出  ：返回1表示成功 0失败
* 举例  ：无
* 注意  ：只有在LCD和触摸屏间的误差角度非常小时,才能运用下面公式
*********************************************************/    
FunctionalState Cal_touch_para( Coordinate * displayPtr,
                                Coordinate * screenPtr,
                                Parameter * para)
{
    
    FunctionalState retTHRESHOLD = ENABLE ;

    /* K＝(X0－X2) (Y1－Y2)－(X1－X2) (Y0－Y2) */
    para->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
    
		if( para->Divider == 0 )
    {
        retTHRESHOLD = DISABLE;
    }
    else
    {
        /* A＝((XD0－XD2) (Y1－Y2)－(XD1－XD2) (Y0－Y2))／K	*/
        para->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                   ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y));
        
        /* B＝((X0－X2) (XD1－XD2)－(XD0－XD2) (X1－X2))／K	*/
        para->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) - 
                   ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x));
        
        /* C＝(Y0(X2XD1－X1XD2)+Y1(X0XD2－X2XD0)+Y2(X1XD0－X0XD1))／K */
        para->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                   (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                   (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;
        
        /* D＝((YD0－YD2) (Y1－Y2)－(YD1－YD2) (Y0－Y2))／K	*/
        para->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) - 
                   ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;
        
        /* E＝((X0－X2) (YD1－YD2)－(YD0－YD2) (X1－X2))／K	*/
        para->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) - 
                   ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;
        
        
        /* F＝(Y0(X2YD1－X1YD2)+Y1(X0YD2－X2YD0)+Y2(X1YD0－X0YD1))／K */
        para->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                   (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                   (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y;
        
    }
    return( retTHRESHOLD ) ;
}






/******************************************************
* 函数名：Get_touch_point
* 描述  ：通过 K A B C D E F 把通道X Y的值转换为液晶屏坐标
* 输入  : 无
* 输出  ：返回1表示成功 0失败
* 举例  ：无
* 注意  ：如果获取的触点信息有误，将返回DISABLE
*********************************************************/    
long double linear=0 ;
long double aa1=0,bb1=0,cc1=0,aa2=0,bb2=0,cc2=0;
FunctionalState Get_touch_point(Coordinate * displayPtr,
                                Coordinate * screenPtr,
                                Parameter * para )
{
    FunctionalState retTHRESHOLD =ENABLE ;
    
    if(screenPtr==0)
    {
        /*如果获取的触点信息有误，则返回DISABLE*/
				retTHRESHOLD = DISABLE;			
    }
    else
	        if( para->Divider != 0 )
	        {		 
	        
	        
	        displayPtr->x = ( (aa1 * screenPtr->x) + 
	                          (bb1 * screenPtr->y) + 
	                          cc1 
	                          ) ;
	        
	        displayPtr->y = ((aa2 * screenPtr->x) + 
	                         (bb2 * screenPtr->y) + 
	                         cc2 
	                         );
	    	}
		    else
		    {
		        retTHRESHOLD = DISABLE;
		    }
    return(retTHRESHOLD);
} 




/******************************************************
* 函数名：Init_Palette
* 描述  ：画板初始化
* 输入  : 无
* 输出  ：无
* 举例  ：无
* 注意  ：“触摸画板实例”专用
*********************************************************/    

void Init_Palette(void)
{
#ifdef HX8347    
    LCD_draw_rectangle(0,0,240,320,WHITE);	/*清白屏*/
#else
		LCD_draw_rectangle(0,0,240,320,WHITE);	/*清白屏*/
#endif

    LCD_DrawLine(0,38,30,38);
    LCD_DrawLine(29,0,29,38);
    LCD_ShowString(7, 10, 0,"CLR");
    LCD_draw_rectangle(30,0,30,40,GREEN);
    LCD_draw_rectangle(60,0,30,40,BLUE);
    LCD_draw_rectangle(90,0,30,40,BRED);
    LCD_draw_rectangle(120,0,30,40,GRED);
    LCD_draw_rectangle(150,0,30,40,GBLUE);
    LCD_draw_rectangle(180,0,30,40,BLACK);
    LCD_draw_rectangle(210,0,30,40,RED);
    delay_ms(500);
    
}
/***************************************************************/
void Init_Menu(void)										   //
{															   //
#ifdef HX8347    											   //
    LCD_draw_rectangle(0,0,240,320,WHITE);	/*清白屏*/	       //
#else														   //
		LCD_draw_rectangle(0,0,240,320,WHITE);	/*清白屏*/	   //
#endif													       //
    LCD_draw_rectangle(210,0,30,80,BLUE);					   //
	PutChinese_strings21(20,220,"菜单",BLACK,1);			   //
    LCD_draw_rectangle(90,110,50,80,BLUE);					   //
	PutChinese_strings21(121,110,"下传数据",RED,1);			   //
	PutChinese_strings21(120,110,"下传数据",RED,1);			   //
															   //
	PutChinese_strings21(10,10,"设定温度转速对应关系",RED,1);  //
	PutChinese_strings21(10,30,"设定电量价格",RED,1);		   //
	PutChinese_strings21(10,50,"阈值温度",RED,1);			   //
	LCD_Show_8x16_String(180,10,1,emission1); 			 	   //
	LCD_Show_8x16_String(180,33,1,emission2); 				   //
	LCD_Show_8x16_String(180,56,1,emission3); 				   //
    delay_ms(500);										       //
}															   //
/****************************************************************/
void Init_Menu1(void)										   //
{															   //
#ifdef HX8347    											   //
    LCD_draw_rectangle(0,0,240,320,WHITE);	/*清白屏*/	       //
#else														   //
		LCD_draw_rectangle(0,0,240,320,WHITE);	/*清白屏*/	   //
#endif													       //
    LCD_draw_rectangle(90,110,50,80,BLUE);					   //
	PutChinese_strings21(121,110,"设置完成",RED,1);			   //
	PutChinese_strings21(120,110,"设置完成",RED,1);			   //
	LCD_draw_rectangle(70,90,20,20,BLUE);					   //
	LCD_draw_rectangle(70,190,20,20,BLUE);					   //
	LCD_draw_rectangle(140,90,20,20,BLUE);					   //
	LCD_draw_rectangle(140,190,20,20,BLUE);					   //
    delay_ms(500);										       //														   //
}
/****************************************************************/
void Init_Smenu(void)										    //
{																//
#ifdef HX8347    												//
    LCD_draw_rectangle(0,0,240,320,WHITE);	/*清白屏*/			//
#else														    //
		LCD_draw_rectangle(0,0,240,320,WHITE);	/*清白屏*/		//
#endif															//
    LCD_draw_rectangle(210,0,30,140,BLUE);						//
	LCD_draw_rectangle(178,0,30,140,BLUE);					    //
	LCD_draw_rectangle(146,0,30,140,BLUE);						//
	LCD_draw_rectangle(114,0,30,140,BLUE);						//
	LCD_draw_rectangle(82,0,30,140,BLUE);						//
	PutChinese_strings21(10,220,"工作情况",BLACK,1);			//
	PutChinese_strings21(10,188,"总用电量",BLACK,1);			//
	PutChinese_strings21(10,156,"温度转速对应关系",BLACK,1);	//
	PutChinese_strings21(10,124,"设置电量的价格",BLACK,1);		//
	PutChinese_strings21(10,92,"设置阈值温度",BLACK,1);		    //
	LCD_draw_rectangle(210,240,30,80,BLUE);						//
	PutChinese_strings21(260,220,"返回",BLACK,1);				//
    delay_ms(500);												//
}																//
/****************************************************************/
/****************************************************************/////////////////////////工作情况
void page1(unsigned int a,unsigned int b,unsigned c)												//
{																//
	  PutChinese_strings21(100,10,"各个风扇的工作情况",0xffff,1);//
	  if(a==0)													 //
	  {															 //
	  	LCD_draw_rectangle(120,50,30,40,WHITE);					 //
      }															 //
	  if(a==1)													 //
	  {															 //
	  LCD_draw_rectangle(120,50,30,40,RED);						 //
	  }	  														 //
		    PutChinese_strings21(10,150,"当前转速级别",0xffff,1); //
		    LCD_Show_8x16_String(110,150,1,":"); 				  //
		    PutChinese_strings21(10,170,"此地实时温度",0xffff,1); //
			LCD_Show_8x16_String(110,170,1,":"); 				  //
	 if(b==0)													   //
	 {															   //
	  	LCD_draw_rectangle(60,240,30,40,WHITE);					   //
	 }															   //
	 if(b==1)													   //
	 {															   //
	  	LCD_draw_rectangle(60,240,30,40,RED);					   //
	 }				  												//
			PutChinese_strings21(120,100,"当前转速级别",0xffff,1); //
			LCD_Show_8x16_String(220,100,1,":"); 				   //
			PutChinese_strings21(120,120,"此地实时温度",0xffff,1); //
			LCD_Show_8x16_String(220,120,1,":"); 				   //
	 if(c==0)														//
	 {																//
	  	LCD_draw_rectangle(150,240,30,40,WHITE);					//
	 }																//
	 if(c==1)														//
	 {																//
	  	LCD_draw_rectangle(150,240,30,40,RED);						//
	 }			   													//
	  		PutChinese_strings21(120,190,"当前转速级别",0xffff,1); //
			LCD_Show_8x16_String(220,190,1,":"); 				   //
			PutChinese_strings21(120,210,"此地实时温度",0xffff,1); //
			LCD_Show_8x16_String(220,210,1,":"); 				   //
	LCD_draw_rectangle(190,260,40,58,RED);							//
	PutChinese_strings21(270,200,"返回",WHITE,1);					//
}																   //
/*******************************************************************/
/*******************************************************************////////////////////////总用电量
void page2(void)												   //
{																   //
	LCD_draw_rectangle(10,300,10,20,BLUE);							//
	LCD_draw_rectangle(22,300,10,20,BLUE);							//
	LCD_draw_rectangle(34,300,10,20,BLUE);
	PutChinese_strings21(100,10,"总用电量",0xffff,1);	           //
	PutChinese_strings21(50,50,"度",0xffff,1);					   //
	PutChinese_strings21(50,70,"元",0xffff,1);	
	LCD_draw_rectangle(190,260,40,58,RED);						   //
	PutChinese_strings21(270,200,"返回",WHITE,1);				   //
																   //
}																   //
/*******************************************************************/
/*******************************************************************/////////////////温度转速对应关系
void page3(void)												   //
{																   //
	PutChinese_strings21(100,10,"设置温度和转速的对应关系",0xffff,1);   //
	PutChinese_strings21(10,40,"转速级别变化所需要的温度差值",0xffff,1);//
	LCD_draw_rectangle(100,10,30,30,RED);							//
	LCD_draw_rectangle(100,50,30,30,RED);							//
	LCD_draw_rectangle(100,90,30,30,RED);							//
	LCD_draw_rectangle(100,130,30,30,RED);							//
	LCD_draw_rectangle(100,170,30,30,RED);							//
	LCD_draw_rectangle(150,10,30,30,RED);							//
	LCD_draw_rectangle(150,50,30,30,RED);							//
	LCD_draw_rectangle(150,90,30,30,RED);							//
	LCD_draw_rectangle(150,130,30,30,RED);							//
	LCD_draw_rectangle(150,170,30,30,RED);							//
	LCD_draw_rectangle(190,260,40,58,RED);							//
	PutChinese_strings21(270,200,"确认",WHITE,1);					//
																	//
	LCD_Show_8x16_String(13,105,0,"0.5"); 							//
	LCD_Show_8x16_String(53,105,0,"1.0"); 							//
	LCD_Show_8x16_String(93,105,0,"1.5"); 							//
	LCD_Show_8x16_String(133,105,0,"2.0"); 							//
	LCD_Show_8x16_String(173,105,0,"2.5"); 							//
																	//
	LCD_Show_8x16_String(13,155,0,"3.0"); 							//
	LCD_Show_8x16_String(53,155,0,"3.5"); 							//
	LCD_Show_8x16_String(93,155,0,"4.0"); 							//
	LCD_Show_8x16_String(133,155,0,"4.5"); 							//
	LCD_Show_8x16_String(173,155,0,"5.0"); 							//
	LCD_draw_rectangle(10,10,25,40,WHITE);							//
}																	//
/********************************************************************////////////////设置电量的价格
/*******************************************************************/
void page4(void)												   //
{																   //
	 	PutChinese_strings21(100,10,"设置电量的价格",0xffff,1);	   //  
	LCD_draw_rectangle(100,10,30,30,RED);							//
	LCD_draw_rectangle(100,50,30,30,RED);							//
	LCD_draw_rectangle(100,90,30,30,RED);							//
	LCD_draw_rectangle(100,130,30,30,RED);							//
	LCD_draw_rectangle(100,170,30,30,RED);							//
	LCD_draw_rectangle(150,10,30,30,RED);							//
	LCD_draw_rectangle(150,50,30,30,RED);							//
	LCD_draw_rectangle(150,90,30,30,RED);							//
	LCD_draw_rectangle(150,130,30,30,RED);							//
	LCD_draw_rectangle(150,170,30,30,RED);							//
	LCD_Show_8x16_String(11,105,0,"0.58"); 							//
	LCD_Show_8x16_String(51,105,0,"0.59"); 							//
	LCD_Show_8x16_String(91,105,0,"0.60"); 							//
	LCD_Show_8x16_String(131,105,0,"0.61"); 						//
	LCD_Show_8x16_String(171,105,0,"0.62"); 						//
																	//
	LCD_Show_8x16_String(11,155,0,"0.63"); 							//
	LCD_Show_8x16_String(51,155,0,"0.64"); 							//
	LCD_Show_8x16_String(91,155,0,"0.65"); 							//
	LCD_Show_8x16_String(131,155,0,"0.66"); 						//
	LCD_Show_8x16_String(171,155,0,"0.67"); 						//
																	//
	LCD_draw_rectangle(190,260,40,58,RED);							//
	PutChinese_strings21(270,200,"确认",WHITE,1);					//
	LCD_draw_rectangle(10,10,25,40,WHITE);							//
}																	//
/*******************************************************************/
/*******************************************************************////////////////////设置阈值温度
void page5(void)													//
{																	//
 	PutChinese_strings21(100,10,"设置开启风扇的阈值温度",0xffff,1);	//
	LCD_draw_rectangle(100,10,30,30,RED);							//
	LCD_draw_rectangle(100,50,30,30,RED);							//
	LCD_draw_rectangle(100,90,30,30,RED);							//
	LCD_draw_rectangle(100,130,30,30,RED);							//
	LCD_draw_rectangle(100,170,30,30,RED);							//
	LCD_draw_rectangle(150,10,30,30,RED);							//
	LCD_draw_rectangle(150,50,30,30,RED);							//
	LCD_draw_rectangle(150,90,30,30,RED);							//
	LCD_draw_rectangle(150,130,30,30,RED);							//
	LCD_draw_rectangle(150,170,30,30,RED);							//
	LCD_Show_8x16_String(15,105,0,"25"); 							//
	LCD_Show_8x16_String(55,105,0,"26"); 							//
	LCD_Show_8x16_String(95,105,0,"27"); 							//
	LCD_Show_8x16_String(135,105,0,"28"); 							//
	LCD_Show_8x16_String(175,105,0,"29"); 							//
																	//
	LCD_Show_8x16_String(15,155,0,"30"); 							//
	LCD_Show_8x16_String(55,155,0,"31"); 							//
	LCD_Show_8x16_String(95,155,0,"32"); 							//
	LCD_Show_8x16_String(135,155,0,"33"); 							//
	LCD_Show_8x16_String(175,155,0,"34"); 							//
																	//
	LCD_draw_rectangle(190,260,40,58,RED);							//
	PutChinese_strings21(270,200,"确认",WHITE,1);					//
//	LCD_draw_rectangle(150,260,30,58,RED);							//
//	PutChinese_strings21(270,155,"键盘",WHITE,1);					//
	LCD_draw_rectangle(10,10,25,40,WHITE);							//
}																	//
/*******************************************************************/
/********************************************************************///////////////////////键盘
void keyborad()													   //
{																   //					  
	LCD_draw_rectangle(40,20,40,70,RED);
	LCD_draw_rectangle(40,100,40,70,RED);
	LCD_draw_rectangle(40,180,40,70,RED);

	LCD_draw_rectangle(90,20,40,70,RED);
	LCD_draw_rectangle(90,100,40,70,RED);
	LCD_draw_rectangle(90,180,40,70,RED);

	LCD_draw_rectangle(140,20,40,70,RED);
	LCD_draw_rectangle(140,100,40,70,RED);
	LCD_draw_rectangle(140,180,40,70,RED);

	LCD_draw_rectangle(190,20,40,70,RED);
	LCD_draw_rectangle(190,100,40,70,RED);
	LCD_draw_rectangle(190,180,40,70,RED);

	LCD_draw_rectangle(190,260,40,58,RED);
	PutChinese_strings21(270,200,"返回",WHITE,1);

	LCD_Show_8x16_String(50,55,0,"1"); 
	LCD_Show_8x16_String(130,55,0,"2"); 
	LCD_Show_8x16_String(210,55,0,"3"); 

	LCD_Show_8x16_String(50,105,0,"4"); 
	LCD_Show_8x16_String(130,105,0,"5"); 
	LCD_Show_8x16_String(210,105,0,"6"); 

	LCD_Show_8x16_String(50,155,0,"7"); 
	LCD_Show_8x16_String(130,155,0,"8"); 
	LCD_Show_8x16_String(210,155,0,"9"); 

	LCD_Show_8x16_String(50,205,0,"."); 
	LCD_Show_8x16_String(130,205,0,"0"); 
   	PutChinese_strings21(200,200,"确认",WHITE,1);

	LCD_draw_rectangle(10,10,25,40,WHITE);
}
/********************************************************************/
/******************************************************
* 函数名：Get_touch_place
* 描述  ：获取触摸LCD位置
* 输入  : 无
* 输出  ：无
* 举例  ：无
* 注意  ：如果触摸笔没有按下,将直接返回1
*********************************************************/    
char  Get_touch_place(u16 *x, u16 *y)
{		
		/*如果触笔有按下*/
    if(touch_flag == 1)				
    {
        if(Get_touch_point(&display, Read_2046(), &touch_para ) !=DISABLE)
        {
            
            *x = display.x ;
            *y = display.y ;
            return 0;
        }
    }
    
    return 1;				
    
}





/******************************************************
* 函数名：Touchl_Calibrate
* 描述  ：触摸屏校正函数
* 输入  : 无
* 输出  ：0	---	校正成功
					1	---	校正失败
* 举例  ：无
* 注意  ：无
*********************************************************/    
int Touchl_Calibrate(void)
{
    uint8_t i;
    u16 test_x=0, test_y=0;
    u16 gap_x=0, gap_y=0;
    Coordinate * Ptr;
    delay_init();
    Set_direction(0);
    for(i=0;i<4;i++)
    {
        LCD_CLEAR(0,0,240,320);
 //       LCD_ShowString(10, 10, 0,"Touch Calibrate");	
 		 PutChinese_strings21(20,10,"欢迎使用该系统",WHITE,1);	
		 PutChinese_strings21(20,30,"请校准",WHITE,1);	
        LCD_ShowNum(25, 10, 0, i+1);
        
        delay_ms(500);
        DrawCross(DisplaySample[i].x,DisplaySample[i].y);
        do
        {
            Ptr=Read_2046();
        }
        while( Ptr == (void*)0 );
        ScreenSample[i].x= Ptr->x; 
        ScreenSample[i].y= Ptr->y;

    }

		/* 送入值得到参数 */
    Cal_touch_para( &DisplaySample[0],&ScreenSample[0],&touch_para ) ;  	   
    
		/*计算X值*/
    test_x = ( (touch_para.An * ScreenSample[3].x) + 
               (touch_para.Bn * ScreenSample[3].y) + 
               touch_para.Cn 
               ) / touch_para.Divider ;			 
    
		/*计算Y值*/
    test_y = ( (touch_para.Dn * ScreenSample[3].x) + 
               (touch_para.En * ScreenSample[3].y) + 
               touch_para.Fn 
               ) / touch_para.Divider ;			 
    
    gap_x = (test_x > DisplaySample[3].x)?(test_x - DisplaySample[3].x):(DisplaySample[3].x - test_x);
    gap_x = (test_y > DisplaySample[3].y)?(test_y - DisplaySample[3].y):(DisplaySample[3].y - test_y);
    
    
    LCD_CLEAR(0,0,240,320);
    
    if((gap_x>8)||(gap_y>8))
    {
//        LCD_ShowString(100, 100, 0,"Calibrate fail");
//        LCD_ShowString(100, 120, 0,"  try again   ");
	   PutChinese_strings21(100,120,"校准失败重新校准",WHITE,1);
        delay_ms(2000);
        return 1;
    }
    
    
    aa1 = (touch_para.An*1.0)/touch_para.Divider;
    bb1 = (touch_para.Bn*1.0)/touch_para.Divider;
    cc1 = (touch_para.Cn*1.0)/touch_para.Divider;
    
    aa2 = (touch_para.Dn*1.0)/touch_para.Divider;
    bb2 = (touch_para.En*1.0)/touch_para.Divider;
    cc2 = (touch_para.Fn*1.0)/touch_para.Divider;
    
   // LCD_ShowString(100, 100, 0,"Calibrate Success");
   	 PutChinese_strings21(120,170,"校准成功",WHITE,1);
    delay_ms(1000);
    
    return 0;    
}



