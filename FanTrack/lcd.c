/******************** ********************************************************
 * 文件名  ：main.c
 * 描述    ：LCD FSMC 应用函数库。
 *           实现的功能：清屏、画点、划线、显示数字、字符串、图片、汉字         
**********************************************************************************/
#include "lcd.h"
#include "ascii.h"		// 12*6
#include "asc_font.h"	//12*8
#include "sd_fs_app.h"


/* 选择BANK1-BORSRAM1 连接 TFT，地址范围为0X60000000~0X63FFFFFF
 * FSMC_A16 接LCD的DC(寄存器/数据选择)脚
 * 16 bit => FSMC[24:0]对应HADDR[25:1]
 * 寄存器基地址 = 0X60000000
 * RAM基地址 = 0X60020000 = 0X60000000+2^16*2 = 0X60000000 + 0X20000 = 0X60020000
 * 当选择不同的地址线时，地址要重新计算。
 */
#define Bank1_LCD_D    ((u32)0x60020000)    //Disp Data ADDR
#define Bank1_LCD_C    ((u32)0x60000000)	   //Disp Reg ADDR

#define RED	  0XF800
#define GREEN 0X07E0
#define BLUE  0X001F  
#define BRED  0XF81F
#define GRED  0XFFE0
#define GBLUE 0X07FF
#define BLACK 0X0000
#define WHITE 0XFFFF

u16 POINT_COLOR = BLACK ;							  // WHITE
volatile u8 display_direction = 0;
/*
 * 函数名：LCD_GPIO_Config
 * 描述  ：根据FSMC配置LCD的I/O
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用        
 */
static void LCD_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable the FSMC Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    
    /* config lcd gpio clock base on FSMC */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    /* config tft back_light gpio base on the PT4101 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;		
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    /* config tft rst gpio */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 	 
    GPIO_Init(GPIOE, &GPIO_InitStructure);  		   
    
    /* config tft data lines base on FSMC
	 * data lines,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10
	 */	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
    /* config tft control lines base on FSMC
	 * PD4-FSMC_NOE  :LCD-RD
   * PD5-FSMC_NWE  :LCD-WR
	 * PD7-FSMC_NE1  :LCD-CS
   * PD11-FSMC_A16 :LCD-DC
	 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    /* tft control gpio init */	 
    GPIO_SetBits(GPIOD, GPIO_Pin_13);		 // LIGHT	
    GPIO_ResetBits(GPIOE, GPIO_Pin_1);	 //	RST = 1   
    GPIO_SetBits(GPIOD, GPIO_Pin_4);		 // RD = 1  
    GPIO_SetBits(GPIOD, GPIO_Pin_5);		 // WR = 1 
    GPIO_SetBits(GPIOD, GPIO_Pin_7);		 //	CS = 1 
    
    GPIO_ResetBits(GPIOD, GPIO_Pin_3);
    GPIO_SetBits(GPIOD, GPIO_Pin_6); 
}

/*
 * 函数名：LCD_FSMC_Config
 * 描述  ：LCD  FSMC 模式配置
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用        
 */

static void LCD_FSMC_Config(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  p; 
    
    
    p.FSMC_AddressSetupTime = 0x02;	 //地址建立时间
    p.FSMC_AddressHoldTime = 0x00;	 //地址保持时间
    p.FSMC_DataSetupTime = 0x05;		 //数据建立时间
    p.FSMC_BusTurnAroundDuration = 0x00;
    p.FSMC_CLKDivision = 0x00;
    p.FSMC_DataLatency = 0x00;
    p.FSMC_AccessMode = FSMC_AccessMode_B;	 // 一般使用模式B来控制LCD
    
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p; 
    
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    
    /* Enable FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
}

static void Delay(__IO u32 nCount)
{
    for(; nCount != 0; nCount--);
}


/*
 * 函数名：LCD_Rst
 * 描述  ：LCD 软件复位
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用        
 */
static void LCD_Rst(void)
{			
    GPIO_ResetBits(GPIOE, GPIO_Pin_1);
    Delay(0xAFFFFf);					   
    GPIO_SetBits(GPIOE, GPIO_Pin_1 );		 	 
    Delay(0xAFFFFf);	
}




/*
 * 函数名：LCD_WR_REG
 * 描述  ：ILI9325 写寄存器函数
 * 输入  ：-index 寄存器
 * 调用  ：内部调用
 */
static __inline void LCD_WR_REG(u16 index)
{
    *(__IO u16 *) (Bank1_LCD_C) = index;
}



/*
 * 函数名：LCD_WR_REG
 * 描述  ：往ILI9325寄存器写数据
 * 输入  ：-index 寄存器
 *         -val   写入的数据
 */
static __inline void LCD_WR_CMD(u16 index, u16 val)
{	
    *(__IO u16 *) (Bank1_LCD_C) = index;	
    *(__IO u16 *) (Bank1_LCD_D) = val;
}



/*
 * 函数名：LCD_WR_Data
 * 描述  ：往ILI9325 GRAM 写入数据
 * 输入  ：-val 写入的数据,16bit        
 */
static __inline void LCD_WR_Data(unsigned int val)
{   
    *(__IO u16 *) (Bank1_LCD_D) = val; 	
}



/*
 * 函数名：LCD_WR_REG
 * 描述  ：读 ILI9325 RAM 数据
 * 输出  ：读取的数据,16bit *         
 */
static __inline u16 LCD_RD_data(void)
{
#ifdef HX8347
    u16 tmp_color = 0;
		u8 Green = 0,Red=0,Black=0;
		Green = *((__IO u8 *)(Bank1_LCD_D)); 	   //Dummy	
    Green = *((__IO u8 *)(Bank1_LCD_D));     //L
    Red =   *((__IO u8 *)(Bank1_LCD_D)); 
		Black = *((__IO u8 *)(Bank1_LCD_D)); 
		tmp_color = (u16)((((u16)Red>>3)<<11)|(((u16)Green>>2)<<5)|((u16)Black>>3));
    return(tmp_color);
#else
    u16 a = 0;
    a = *((__IO u16 *) (Bank1_LCD_D)); 	   //Dummy	
    a = *((__IO u16 *) (Bank1_LCD_D));     //L		    
    return(a);
#endif	
}



/*
 *  函数名：RGB
 *  功能：RGB颜色混合函数
 *  输入：R 0-31,G 0-63,B 0-31
 *  输出：混合后的颜色值
 */
static u16 RGB(u8 R,u8 G,u8 B)
{	
    return ( ( (u8)R<<11 ) | ( (u8)G<<5 & 0X07E0 ) | ( (u8)(B&0X1F) ) );
}




void Set_direction(u8 option)
{
#ifdef	HX8347
    switch(option)
    {
    case 0:{	 /*横屏*/
            LCD_WR_CMD(0x16,0X70); // 
            
            LCD_WR_CMD(0x06,0x00);
            LCD_WR_CMD(0x07,0x00); //Column Start
            
            LCD_WR_CMD(0x08,0x00);
            LCD_WR_CMD(0x09,0xEF); //Column End
            
                        
            LCD_WR_CMD(0x02,0x00);
            LCD_WR_CMD(0x03,0x00); //Row Start
            
            LCD_WR_CMD(0x04,0x01);
            LCD_WR_CMD(0x05,0x3F); //Row End}
            display_direction = 0;		
        }break;
    case 1:
        {		/*竖屏*/
        LCD_WR_CMD(0x16,0x50); //左上到右下   竖屏 				   
        LCD_WR_CMD(0x02,0x00);
        LCD_WR_CMD(0x03,0x00); //Column Start
        
        LCD_WR_CMD(0x04,0x00);
        LCD_WR_CMD(0x05,0xEF); //Column End
        
        LCD_WR_CMD(0x06,0x00);
        LCD_WR_CMD(0x07,0x00); //Row Start
        
        LCD_WR_CMD(0x08,0x01);
        LCD_WR_CMD(0x09,0x3F); //Row End
        display_direction = 1;
    }break;
        
    }
#else
    switch(option)
    {
        //以液晶屏的接口对准自己
    case 0:	  LCD_WR_CMD(0x0003,0X1038);break;		//垂直扫描
    case 1:		LCD_WR_CMD(0x0003, 0X1030);break;	 	//水平扫描 
    default:	LCD_WR_CMD(0x0003, 0X1018);break;		//垂直扫描 
    }
    display_direction = 0;
#endif
}


/*----------------------------------------以下为LCD应用函数----------------------------------------------*/


/*
 * 函数名：LCD_open_windows
 * 描述  ：开窗(以x,y为坐标起点，长为len,高为wid)
 * 输入  ：-x -y -len -wid -rgb565
 * 输出  ：无
 * 举例:
 *	   LCD_open_windows(0,0,120,160);	 	   //右上角  120*160
 *	   LCD_open_windows(0,160,120,160);	   //左上角	120*160
 *	   LCD_open_windows(120,0,120,160);	   //右下角	 120*160
 *	   LCD_open_windows(120,160,120,160);	   //左下角	120*160
 *
 */
void LCD_open_windows(u16 x,u16 y,u16 len,u16 wid)
{                    
#ifdef HX8347
    if(display_direction == 0)		//如果是横屏选项
    {
        LCD_WR_CMD(0x02, x>>8);		       //高八位
        LCD_WR_CMD(0x03, x-((x>>8)<<8)); //低八位
        
        LCD_WR_CMD(0x04, (x+len-1)>>8);
        LCD_WR_CMD(0x05, (x+len-1)-(((x+len-1)>>8)<<8));
        
        LCD_WR_CMD(0x06, y>>8);
        LCD_WR_CMD(0x07, y-((y>>8)<<8)); 
        
        LCD_WR_CMD(0x08, (y+wid-1)>>8);
        LCD_WR_CMD(0x09, (y+wid-1)-(((y+wid-1)>>8)<<8));
        
    }
    else
    {
        LCD_WR_CMD(0x06, x>>8);
        LCD_WR_CMD(0x07, x-((x>>8)<<8)); 
        
        LCD_WR_CMD(0x08, (x+len-1)>>8);
        LCD_WR_CMD(0x09, (x+len-1)-(((x+len-1)>>8)<<8));
        
        LCD_WR_CMD(0x02, y>>8);
        LCD_WR_CMD(0x03, y-((y>>8)<<8)); 
        
        LCD_WR_CMD(0x04, (y+wid-1)>>8);
        LCD_WR_CMD(0x05, (y+wid-1)-(((y+wid-1)>>8)<<8));
        
    }
#else
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, x+len-1); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+wid-1);
    LCD_WR_CMD(0X20, x); 	
    LCD_WR_CMD(0X21, y); 
#endif    
    LCD_WR_REG(0X22);
    
}


/*
 * 函数名：LCD_draw_rectangle
 * 描述  ：以x,y为坐标起点，在长len,高wid的矩形区域内描上制定的rgb565颜色
 * 输入  ：-x -y -len -wid -rgb565
 * 输出  ：无 
 * 举例  :  
 *			LCD_draw_rectangle(0,0,120,160,0XF800);		右上角    	120*160	 	REG 
 *	   		LCD_draw_rectangle(0,160,120,160,0X07E0);	 左上角		120*160		GREEN
 *	   		LCD_draw_rectangle(120,0,120,160,0X001F);	 右下角		120*160	 	BLUE
 *	   		LCD_draw_rectangle(120,160,120,160,0XFF00);	  左下角	120*160	   	orange
 */
void LCD_draw_rectangle(u16 x,u16 y,u16 len,u16 wid,u16 rgb565)
{                    
    
		u32 n, temp;
		
		#ifdef HX8347
		Set_direction(0);
		LCD_open_windows(y,x,wid,len);
		#else
				
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, x+len-1); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+wid-1);
		LCD_WR_CMD(0X20, x); 	
    LCD_WR_CMD(0X21, y); 
    LCD_WR_REG(0X22);
    #endif
    temp = (u32)len*wid;    
    for(n=0; n<temp; n++)
        LCD_WR_Data( rgb565 );
				 
}



/*
 * 函数名：LCD_CLEAR
 * 描述  ：以x,y为坐标起点，在长len,高wid的范围内清屏
 * 输入  ：-x -y -len -wid
 * 输出  ：无 
 */
void LCD_CLEAR(u8 x,u16 y,u8 len,u16 wid)
{                    
    u32 n, temp;
    
    
#ifdef HX8347
    if(display_direction ==0) //如果横屏标志位为0(0表示横屏)
    {
        /*横屏*/
        LCD_WR_CMD(0x06, 0);
        LCD_WR_CMD(0x07, x); 
        
        LCD_WR_CMD(0x08, 0);
        LCD_WR_CMD(0x09, x+len-1);
        
        
        LCD_WR_CMD(0x02, y>>8);
        LCD_WR_CMD(0x03, y-((y>>8)<<8)); 
        
        LCD_WR_CMD(0x04, (y+wid-1)>>8);
        LCD_WR_CMD(0x05, (y+wid-1)-(((y+wid-1)>>8)<<8));
    }
    else		 //如果横屏标志位不为0(不为0表示竖屏)
    {	
        /*竖屏*/
        LCD_WR_CMD(0x02, 0);
        LCD_WR_CMD(0x03, x); 
        
        LCD_WR_CMD(0x04, 0);
        LCD_WR_CMD(0x05, x+len);
        
        
        LCD_WR_CMD(0x06, y>>8);
        LCD_WR_CMD(0x07, y-((y>>8)<<8)); 
        
        LCD_WR_CMD(0x08, (y+wid)>>8);
        LCD_WR_CMD(0x09, (y+wid)-(((y+wid)>>8)<<8));
    }
#else
    /*ili9325*/
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, x+len-1); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+wid-1); 
#endif
    
    LCD_WR_REG(0X22);    
    
    temp = (u32)len*wid;    
    for(n=0; n<temp; n++)
        LCD_WR_Data( RGB(20,20,20) );
				 
}



void LCD_WRITE_SATAT(void)
{
    LCD_WR_CMD(0x0050,0); 
    LCD_WR_CMD(0x0051, 239); 
    LCD_WR_CMD(0x0052, 0); 
    LCD_WR_CMD(0x0053, 319); 
    LCD_WR_REG(0X22);
    
}



void MY_LCD_WRITE(u16 rgb565)
{
    LCD_WR_Data(rgb565);
    
}



/*
 * 函数名：LCD_DrawPoint
 * 描述  ：在坐标(x,y)处画一个点
 * 输入  ：-x 0~239
           -y 0~319
 * 输出  ：无 
 */
void LCD_DrawPoint(u16 x,u16 y)
{
#ifdef HX8347
    LCD_WR_CMD(0x06, 0);
    LCD_WR_CMD(0x07, x); 		
    LCD_WR_CMD(0x08, 0);
    LCD_WR_CMD(0x09, x);
    
    LCD_WR_CMD(0x02, y>>8);
    LCD_WR_CMD(0x03, y-((y>>8)<<8)); 
    LCD_WR_CMD(0x04, (y)>>8);
    LCD_WR_CMD(0x05, (y)-(((y)>>8)<<8));
    
#else
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, 0x00EF);
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, 0x013F); 
    LCD_WR_CMD(0X20, x); 	
    LCD_WR_CMD(0X21, y); 
#endif
    
    LCD_WR_REG(0X22);
    
    LCD_WR_Data(POINT_COLOR);	
} 


/*
* 函数名：LCD_DrawPoint
* 描述  ：在坐标(x,y)处画一个指定颜色的点
* 输入  ：-x 0~239
-y 0~319
* 输出  ：无 
*/
void LCD_Draw_ColorPoint(u16 x,u16 y, u16 rgb565)
{
#ifdef HX8347
    LCD_WR_CMD(0x06, 0);
    LCD_WR_CMD(0x07, x); 		
    LCD_WR_CMD(0x08, 0);
    LCD_WR_CMD(0x09, x);
    
    LCD_WR_CMD(0x02, y>>8);
    LCD_WR_CMD(0x03, y-((y>>8)<<8)); 
    LCD_WR_CMD(0x04, (y)>>8);
    LCD_WR_CMD(0x05, (y)-(((y)>>8)<<8));
    
#else
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, 0x00EF);
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, 0x013F); 
    LCD_WR_CMD(0X20, x); 	
    LCD_WR_CMD(0X21, y); 
#endif
    
    LCD_WR_REG(0X22);
    
    LCD_WR_Data(rgb565);	
} 


/*
 * 函数名：LCD_Show_ColorBar
 * 描述  ：显示16条彩带
 * 输入  ：无
 * 输出  ：无 
 */
void LCD_Show_ColorBar(void)
{
    
#ifdef HX8347
    //
#else
		u32 i, j, temp;
    for(i=0,temp=0; i<16; i++)	 		// 显示16条彩带
    {		
        for(j=0; j<4800; j++)			 // 4800 = 320*/16*240
        {
            LCD_WR_Data(temp);
        }
        temp += 4369;							 // 4369 = 65535/15
    }
#endif
}




void LCD_SetCursor(uint16_t Xpos,uint16_t Ypos)
{
    
#ifdef HX8347
    //等待加入
    
#else
    LCD_WR_CMD(0x004e,Xpos); 	/* Row */
    LCD_WR_CMD(0x004f,Ypos); 	/* Line */	
#endif 
    
    //   LCD_WriteReg(0x0020,Xpos); /* Row */
    //  LCD_WriteReg(0x0021,Ypos); /* Line */
    
}


/*
 * 函数名：abs
 * 描述  ：求绝对值
 * 输入  ：-res   32bit        
 * 输出  ：无 
 */
static u32 abs(s32 res)
{
    if(res < 0)
        return -res;
    else
        return res;
}



/*
 * 函数名：LCD_DrawLine
 * 描述  ：在液晶中以(x1,y1)为起点,(x2,y2)为终点画一直线
 * 输入  ：-x1 0~239
           -y1 0~319
					 -x2 0~239
					 -y2 0~319       
 * 输出  ：无 
 */  
void LCD_DrawLine(u8 x1, u16 y1, u8 x2, u16 y2)
{
    u16 x, y, t;
    if( (x1 == x2) && (y1 == y2) )
        LCD_DrawPoint(x1, y1);
    else if( abs(y2 - y1) > abs(x2 - x1) )			//斜率大于1 
    {
        if(y1 > y2) 
        {
            t = y1;
            y1 = y2;
            y2 = t; 
            t = x1;
            x1 = x2;
            x2 = t; 
        }
        for(y=y1; y<y2; y++)									//以y轴为基准 
        {
            x = (u32)(y-y1)*(x2-x1) / (y2-y1) + x1;
#ifdef HX8347
            /*调试等待等待加入*/
#else
            LCD_DrawPoint(x, y);
#endif  
        }
    }
    else     																	//斜率小于等于1 
    {
        if(x1 > x2)
        {
            t = y1;
            y1 = y2;
            y2 = t;
            t = x1;
            x1 = x2;
            x2 = t;
        }   
        for(x=x1; x<x2; x++)									//以x轴为基准 
        {
            y = (u32)(x-x1)*(y2-y1) / (x2-x1) + y1;
#ifdef HX8347
            /*等待加入*/
#else
            LCD_DrawPoint(x, y); 
#endif
        }
    } 
}




/*
 * 函数名  ：LCD_ShowChar
 * 描述    ：在液晶坐标(x,y)中,显示一个12*6大小的字符
 * 输入    ：-x 0~(239-6)
             -y 0~(319-12)
						 -mode 0 无叠加效果
						       1 有叠加效果
      		   -acsii 要写入的字符
 * 输出    ：无
 * 调用方法：TFT_ShowChar(10, 10, 'A');
 *           内部调用 
 */

/*新屏--zyq*/
static void LCD_ShowChar(u8 x, u16 y, u8 mode, u8 acsii)	//横屏
{       
#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 304 
    u8 temp, t, pos;  
    u16 R_dis_mem;    
    if(x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
        return;
#ifdef HX8347
    LCD_WR_CMD(0x06, 0);
    LCD_WR_CMD(0x07, x); 		
    LCD_WR_CMD(0x08, 0);
    LCD_WR_CMD(0x09, x+11);
    
    LCD_WR_CMD(0x02, y>>8);
    LCD_WR_CMD(0x03, y-((y>>8)<<8)); 
    LCD_WR_CMD(0x04, (y+5)>>8);
    LCD_WR_CMD(0x05, (y+5)-(((y+5)>>8)<<8));
#else
    Set_direction(0);	//扫描方向设置	 													
    LCD_WR_CMD(0x0051, x); 
    LCD_WR_CMD(0x0051, x+11); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+5);	 
    LCD_WR_CMD(0X20, x);
    LCD_WR_CMD(0X21, y);
#endif

    LCD_WR_REG(0X22);
    
    
    acsii = acsii - ' ';													//得到偏移后的值
    for(pos=0; pos<12; pos++)
    {
        temp = asc2_1206[acsii][pos];
        for(t=0; t<6; t++)											// 低位开始,丢弃高两位
        {                 
            if(temp & 0x01)
                LCD_WR_Data(POINT_COLOR);
            else 	
            {
                if ( mode == 0 )
                    LCD_WR_Data(0xffff);				//白色
                else if ( mode == 1 )
                {
#ifdef HX8347				
                    LCD_WR_Data(0xffff);				/*待修正*/

#else
                    /* 叠加效果 */
                    LCD_WR_CMD(0X20, x+pos);		//水平显示区地址
                    LCD_WR_CMD(0X21, y+t);			//垂直显示区地址
                    LCD_WR_REG(0X22); 
                    
                    R_dis_mem =LCD_RD_data();
                    LCD_WR_Data(R_dis_mem);
#endif
                }				
            }   
            temp >>= 1; 
        }
    }
    
    
}


static void LCD_ShowChar2(u8 x, u16 y, u8 mode, u8 acsii)	//竖屏
{       
#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 304 
    u8 temp, t, pos;  
    u16 R_dis_mem;    
    if(x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
        return;
    
#ifdef HX8347
    LCD_WR_CMD(0x02, 0);
    LCD_WR_CMD(0x03, x); 
    
    LCD_WR_CMD(0x04, 0);
    LCD_WR_CMD(0x05, x+5);
    
    LCD_WR_CMD(0x06, y>>8);
    LCD_WR_CMD(0x07, y-((y>>8)<<8)); 
    
    LCD_WR_CMD(0x08, (y+11)>>8);
    LCD_WR_CMD(0x09, (y+11)-(((y+11)>>8)<<8));
    
#else
    Set_direction(1);																	//扫描方向设置
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, x+5); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+11);	 
    LCD_WR_CMD(0X20, x);
    LCD_WR_CMD(0X21, y);
#endif
    
    LCD_WR_REG(0X22);
    
    acsii = acsii - ' ';															//得到偏移后的值
    for(pos=0; pos<12; pos++)
    {
#ifdef HX8347
        if(display_direction==0)
            temp = asc2_1206[acsii][pos];
        else
            temp = asc2_1206[acsii][12-pos];
#else
        temp = asc2_1206[acsii][12-pos];
#endif
        for(t=0; t<6; t++)														// 低位开始,丢弃高两位
        {                 
            if(temp & 0x01)
                LCD_WR_Data(POINT_COLOR);
            else 	
            {
                if ( mode == 0 )
                    LCD_WR_Data(0xffff);							//白色
                else if ( mode == 1 )
                {
#ifdef HX8347
                    LCD_WR_Data(0xffff);					 // HX8347调试阶段
#else
                    /* 叠加效果 */				
                    LCD_WR_CMD(0X20, x+t);					//水平显示区地址
                    LCD_WR_CMD(0X21, y+pos);				//垂直显示区地址
                    LCD_WR_REG(0X22); 
                    
                    R_dis_mem =LCD_RD_data();
                    LCD_WR_Data(R_dis_mem);
#endif
                }				
            }   
            temp >>= 1; 
        }
    }
    
    
}


/*
 * 函数名  ：LCD_Show_8x16_Char
 * 描述    ：在液晶坐标(x,y)中,显示一个12*6大小的字符
 * 输入    ：-x 0~(239-6)
             -y 0~(319-12)
						 -mode 0 无叠加效果
						       1 有叠加效果
      		   -acsii 要写入的字符
 * 输出    ：无
 * 调用方法：TFT_ShowChar(10, 10, 'A');
 *           内部调用 
 */ 

void LCD_Show_8x16_Char(u8 x, u16 y, u8 mode, u8 acsii)//横屏
{       
#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 304 
    u8 temp, t, pos;  
    u16 R_dis_mem;     
    if(x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
        return;
    
    
#ifdef HX8347
    LCD_WR_CMD(0x06, 0);
    LCD_WR_CMD(0x07, x); 
    
    LCD_WR_CMD(0x08, 0);
    LCD_WR_CMD(0x09, x+15);
    
    LCD_WR_CMD(0x02, y>>8);
    LCD_WR_CMD(0x03, y-((y>>8)<<8)); 
    
    LCD_WR_CMD(0x04, (y+7)>>8);
    LCD_WR_CMD(0x05, (y+7)-(((y+7)>>8)<<8));
    
#else
    Set_direction(0);														//扫描方向设置
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, x+15); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+7);	 
    LCD_WR_CMD(0X20, x);
    LCD_WR_CMD(0X21, y);
    LCD_WR_REG(0X22);  
#endif
    
    LCD_WR_REG(0X22);  
    
    for (pos=0;pos<16;pos++)
    {
        temp=ascii_8x16[((acsii-0x20)*16)+pos];
        
        for(t=0; t<8; t++)
        {
            
            if(temp & 0x80)
                LCD_WR_Data(POINT_COLOR);
            else 	
            {
                if ( mode == 0 )
                    LCD_WR_Data(0xffff);					//白色
                
                else if ( mode == 1 )				 /*读数据尚有问题*/
                {
#ifdef	HX8347 
                    LCD_WR_Data(0xffff);	 		//HX8347尚在调试阶段
#else
                    LCD_WR_CMD(0X20, x+pos);		//水平显示区地址
                    LCD_WR_CMD(0X21, y+t);			//垂直显示区地址
                    LCD_WR_REG(0X22);                    
                    R_dis_mem =LCD_RD_data();
                    LCD_WR_Data(R_dis_mem);
#endif
                }				
            } 
            
            temp <<= 1;	
            
        }
        
        
    }
    
}





void LCD_Show_8x16_Char2(u8 x, u16 y, u8 mode, u8 acsii) //竖屏
{       
#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 304 
    u8 temp, t, pos;  
    u16 R_dis_mem;     
    if(x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
        return;
    
#ifdef HX8347
    LCD_WR_CMD(0x02, 0);
    LCD_WR_CMD(0x03, x); 		
    LCD_WR_CMD(0x04, 0);
    LCD_WR_CMD(0x05, x+7);
    
    LCD_WR_CMD(0x06, y>>8);
    LCD_WR_CMD(0x07, y-((y>>8)<<8)); 
    LCD_WR_CMD(0x08, (y+15)>>8);
    LCD_WR_CMD(0x09, (y+15)-(((y+15)>>8)<<8));
#else
    Set_direction(1);		 													//扫描方向设置
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, x+7); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+15);	 
    LCD_WR_CMD(0X20, x);
    LCD_WR_CMD(0X21, y);
#endif
    
    LCD_WR_REG(0X22);    
    
    
    for (pos=0;pos<16;pos++)
    {
        temp=ascii_8x16[((acsii-0x20)*16)+16-pos];
        
        for(t=0; t<8; t++)
        {
            
            if(temp & 0x80)
                LCD_WR_Data(POINT_COLOR);
            else 	
            {
                if ( mode == 0 )
                    LCD_WR_Data(0xffff);					//白色
                else if ( mode == 1 )
                {
#ifdef HX8347
                    LCD_WR_Data(0xffff);					//	HX8347 调试阶段
#else
                    LCD_WR_CMD(0X20, x+t);				//水平显示区地址 
                    LCD_WR_CMD(0X21, y+pos);			//垂直显示区地址
                    
                    LCD_WR_REG(0X22); 
                    R_dis_mem = LCD_RD_data();
                    LCD_WR_Data(R_dis_mem);
#endif
                }				
            } 
            
            temp <<= 1;	
            
        }
        
        
    }
    
}



/*
 * 函数名  ：LCD_Show_8x16_String
 * 描述    ：在液晶坐标(x,y)中,显示一个16*8大小的字符串
 * 输入    ：-x 0~(239-6)
             -y 0~(319-12)
						 -mode 0 无叠加效果
						       1 有叠加效果
      		   -acsii 要写入的字符
 * 输出    ：无
 * 调用方法：LCD_Show_8x16_String(130, 130, 1, "Runing");
 *          外部调用
 */ 
void LCD_Show_8x16_String(u16 y, u8 x, u8 mode, u8 *str)	 //横屏
{
    u8* tmp_str = str;
    u8 Tmp_y=y;
		Set_direction(0);
    while(*tmp_str != '\0')
    {
        LCD_Show_8x16_Char(x,Tmp_y,mode,*tmp_str);		
        tmp_str ++ ;
        Tmp_y += 8 ;
    }
    
}



/*
 * 函数名  ：LCD_Show_8x16_String
 * 描述    ：在液晶坐标(x,y)中,显示一个16*8大小的字符串
 * 输入    ：-x 0~(239-8)
             -y 0~(319-16)
						 -mode 0 无叠加效果
						       1 有叠加效果
      		   -acsii 要写入的字符
 * 输出    ：无
 * 调用方法：LCD_Show_8x16_String2(130, 130, 1, "Runing");
 *          外部调用
 */
void LCD_Show_8x16_String2(u8 x, u16 y, u8 mode, u8 *str)	 //竖屏
{
    u8* tmp_str = str;
    u8 Tmp_y=y;
		Set_direction(1);
    while(*tmp_str != '\0')
    {
        LCD_Show_8x16_Char2(x,Tmp_y,mode,*tmp_str);		
        tmp_str ++ ;
        x += 8 ;
    }
    
}



//m^n函数
static u32 mypow(u8 m, u8 n)
{
    u32 result = 1;	 
    while( n -- )
        result *= m;    
    return result;
}



/*
 * 函数名  ：LCD_ShowNum
 * 描述    ：在液晶坐标(x,y)中,显示一个12*6大小的数
 * 输入    ：-x 0~(239-6)
 *           -y 0~(319-12)
 *					 -mode 0 无叠加效果
 *						     1 有叠加效果
 *     		   -num 0~65535
 * 输出    ：无
 * 调用方法：LCD_ShowNum(10, 10, 0, 65535); 
 */  
void LCD_ShowNum(u8 x,u16 y, u8 mode, u32 num)
{      
    u32 res;   	   
    u8 t = 0, t1 = 0;   
    res = num;
    if( !num )
        LCD_ShowChar(x, y, mode, '0');
    while( res )  //得到数字长度
    {
        res/=10;
        t++;
    }
    t1 = t;
    while(t)	//显示数字
    {
        res = mypow(10, t-1); 	 
        LCD_ShowChar(x, y+(t1-t)*6, mode, (num/res)%10+'0');
        t -- ;
    }				     
} 



/*
 * 函数名  ：LCD_ShowString
 * 描述    ：在液晶中从坐标(x,y)开始显示字符串
 * 输入    ： -y	0~(319-12)
 *						-x 	0~(239-6)
 *           -mode 0 无叠加效果
 *                 1 有叠加效果
 *     		   -p 指向要写入液晶的字符串
 * 输出    ：无
 * 调用方法：LCD_ShowString(10, 10, 0, “I LOVE STM32”); 
 */
void LCD_ShowString(u16 y,u8 x, u8 mode, const u8 *p)	 //横屏
{    
		Set_direction(0);     
    while(*p != '\0')
    {       
        if(x > MAX_CHAR_POSX) 
        {	// 换行
            x = 0;
            y += 12;
        }
        if(y > MAX_CHAR_POSY) 
        {	// 一屏
            y = x = 0;
            LCD_CLEAR(0,0,240,320);
        }
        
        LCD_ShowChar(x ,y, mode, *p);
        y += 6;
        p ++ ;
    }  
}

/*
 * 函数名  ：LCD_ShowString
 * 描述    ：在液晶中从坐标(x,y)开始显示字符串
 * 输入    ：-x 0~(239-6)
 *           -y 0~(319-12)
 *           -mode 0 无叠加效果
 *                 1 有叠加效果
 *     		   -p 指向要写入液晶的字符串
 * 输出    ：无
 * 调用方法：LCD_ShowString(10, 10, 0, “I LOVE STM32”); 
 */
void LCD_ShowString2(u8 x, u16 y, u8 mode, const u8 *p)	//竖屏
{   Set_direction(1);      
    while(*p != '\0')
    {       
        if(x > MAX_CHAR_POSX) 
        {	// 换行
            x = 0;
            y += 12;
        }
        if(y > MAX_CHAR_POSY) 
        {	// 一屏
            y = x = 0;
            LCD_CLEAR(0,0,240,320);
        }
        
        LCD_ShowChar2(x ,y, mode, *p);
        x +=6;
        p ++ ;
    }  
}



/******************************************************************************
* Function Name  : PutChinese1
* Description    : 将Lcd屏上任意位置显示一个中文字
* Input          : - Xpos: 水平坐标 
*                  - Ypos: 垂直坐标  
*				   - str: 显示的中文字
*				   - Color: 字符颜色   
*				   - bkColor: 背景颜色 
* Output         : None
* Return         : None
* example		 : PutChinese1(200,100,"云",0,0xffff);
* Attention		 : 内部调用
*******************************************************************************/
void PutChinese11(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor)	//横屏
{
    
#ifndef NO_CHNISEST_DISPLAY   //如果汉字显示功能没有关闭
    
    uint8_t i,j;
    uint8_t buffer[32];
    uint16_t tmp_char=0;
    
    
   GetGBKCode_from_sd(buffer,str);  /* 取字模数据 */
    
    for (i=0;i<16;i++)
    {
        tmp_char=buffer[i*2];
        tmp_char=(tmp_char<<8);
        tmp_char|=buffer[2*i+1];
        for (j=0;j<16;j++)
        {
            if ( (tmp_char >> 15-j) & 0x01 == 0x01)
            {
                LCD_Draw_ColorPoint(Ypos+i,Xpos+j,Color);
            }
            else
            {
                LCD_Draw_ColorPoint(Ypos+i,Xpos+j,bkColor);
            }
        }
    }
    
#endif
}

/******************************************************************************
* Function Name  : PutChinese1
* Description    : 将Lcd屏上任意位置显示一个中文字
* Input          : - Xpos: 水平坐标 
*                  - Ypos: 垂直坐标  
*				   - str: 显示的中文字
*				   - Color: 字符颜色   
*				   - bkColor: 背景颜色 
* Output         : None
* Return         : None
* example		 : PutChinese1(200,100,"云",0,0xffff);
* Attention		 : 内部调用
*******************************************************************************/
void PutChinese12(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor)//竖屏
{
    
#ifndef NO_CHNISEST_DISPLAY   //如果汉字显示功能没有关闭
    
    uint8_t i,j;
    uint8_t buffer[32];
    uint16_t tmp_char=0;
    
    Set_direction(1);
   	GetGBKCode_from_sd(buffer,str);	 /* 取字模数据 */
#ifdef HX8347
    for (i=0;i<16;i++)
    {
        tmp_char=buffer[i*2];
        tmp_char=(tmp_char<<8);
        tmp_char|=buffer[2*i+1];
        for (j=0;j<16;j++)
        {
            if ( (tmp_char >> 15-j) & 0x01 == 0x01)
            {
                LCD_Draw_ColorPoint(Ypos+16-i,Xpos+j,Color);
            }
            else
            {
                LCD_Draw_ColorPoint(Ypos+16-i,Xpos+j,bkColor);
            }
        }
    }	 
#else
    for (i=0;i<16;i++)
    {
        tmp_char=buffer[i*2];
        tmp_char=(tmp_char<<8);
        tmp_char|=buffer[2*i+1];
        for (j=0;j<16;j++)
        {
            if ( (tmp_char >> 15-j) & 0x01 == 0x01)
            {
                LCD_Draw_ColorPoint(Xpos+j,Ypos+16-i,Color);
            }
            else
            {
                LCD_Draw_ColorPoint(Xpos+j,Ypos+16-i,bkColor);
            }
        }
    }
#endif	 
#endif
}




/******************************************************
 * 函数名：PutChinese21
 * 描述  ：显示单个汉字字符串
 * 输入  : pos: 0~(319-16)
 *         Ypos: 0~(239-16)
 *				 str: 中文字符串首址
 *				 Color: 字符颜色   
 *				 mode: 0--文字背景色为白色   
 *						   1--文字悬浮 
 * 输出  ：无
 * 举例  ：PutChinese21(200,100,"好",0,0);
 * 注意	 ：如果输入大于1的汉字字符串，显示将会截断，只显示最前面一个汉字
 *********************************************************/    
void PutChinese21(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode) //横屏
{
    uint8_t i,j;
    uint8_t buffer[32];
    uint16_t tmp_char=0;
    Set_direction(0);   
    GetGBKCode_from_sd(buffer,str); /* 取字模数据 */
    
    for (i=0;i<16;i++)
    {
        tmp_char=buffer[i*2];
				tmp_char=(tmp_char<<8);
				tmp_char|=buffer[2*i+1];
        for (j=0;j<16;j++)
        {            
            if ( (tmp_char >> 15-j) & 0x01 == 0x01)
            {
                LCD_Draw_ColorPoint(Ypos+i,Xpos+j,Color);
            }
            else
            {
                if ( mode == 0 )
                    LCD_Draw_ColorPoint(Ypos+i,Xpos+j,0xffff);	//背景色显示白色
                else if ( mode == 1 )
                {
                    //不写入
                }               
                
            }
        }
    }   
}




/******************************************************************************
* Function Name  : PutChinese22
* Description    : 将Lcd屏上任意位置显示一个中文字
* Input          : - Xpos: 水平坐标 
*                  - Ypos: 垂直坐标  
*				   - str: 显示的中文字
*				   - Color: 字符颜色   
*				   - mode: 模式选择		0--背景色为白色   1--文字悬浮
* Output         : None
* Return         : None
* example		 : PutChinese2(200,100,"云",0,1);
* Attention		 : 内部调用
*******************************************************************************/
void PutChinese22(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode) //竖屏
{
    
#ifndef NO_CHNISEST_DISPLAY   //如果汉字显示功能没有关闭
    
    uint8_t i,j;
    uint8_t buffer[32];
    uint16_t tmp_char=0;
    Set_direction(1);
    GetGBKCode_from_sd(buffer,str);  																		/* 取字模数据 */
   
    if(display_direction ==0)
    {
        for (i=0;i<16;i++)
        {
            tmp_char=buffer[i*2];
            tmp_char=(tmp_char<<8);
            tmp_char|=buffer[2*i+1];
            for (j=0;j<16;j++)
            {
                
                if ( (tmp_char >> 15-j) & 0x01 == 0x01)
                {
                    LCD_Draw_ColorPoint(Xpos+j,Ypos+16-i,Color);
                }
                else
                {
                    if ( mode == 0 )
                        LCD_Draw_ColorPoint(Ypos+j,Xpos+i,0xffff);	//背景色显示白色
                    else if ( mode == 1 )
                    {
                        //不写入
                    }	
                    
                    
                }
            }
        }
    }
    
    else
    {
        for (i=0;i<16;i++)
        {
            tmp_char=buffer[i*2];
            tmp_char=(tmp_char<<8);
            tmp_char|=buffer[2*i+1];
            for (j=0;j<16;j++)
            {
                
                if ( (tmp_char >> 15-j) & 0x01 == 0x01)
                {
                    LCD_Draw_ColorPoint(Ypos+16-i,Xpos+j,Color);
                }
                else
                {
                    if ( mode == 0 )
                        LCD_Draw_ColorPoint(Xpos+i,Ypos+j,0xffff);	//背景色显示白色
                    else if ( mode == 1 )
                    {
                        //不写入
                    }	
                    
                    
                }
            }
        }
        
        
    }
    
#endif
}



/******************************************************************************
* Function Name  : PutChinese_strings1
* Description    : 将Lcd屏上任意位置显示一个中文字
* Input          : - Xpos: 0-(319-16) 	 
*                  - Ypos: 0-(239-16)  
*				   - str: 显示的中文字符串
*				   - Color: 字符颜色   
*				   - bkColor: 背景颜色 
* Output         : None
* Return         : None
* example		 : PutChinese_strings11(200,100,"好人",0,0xffff);
				   PutChinese_strings11(200,150,"坏人",0xff,0xffff);
* Attention		 : 外部调用
*******************************************************************************/
void PutChinese_strings11(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor)	//横屏
{
    
    uint16_t Tmp_x, Tmp_y;
    uint8_t *tmp_str=str;
    
    Tmp_x = Xpos;
    Tmp_y = Ypos;
    
    Set_direction(0);
    while(*tmp_str != '\0')
    {
        PutChinese11(Tmp_x,Tmp_y,tmp_str,Color,bkColor);
        
        tmp_str += 2 ;
        Tmp_x += 16 ;	
    }
    
    
    
}



/******************************************************************************
* Function Name  : PutChinese_strings12
* Description    : 将Lcd屏上任意位置显示一个中文字
* Input          : - Xpos: 0-(239-16)
*                  - Ypos: 0-(319-16)
*				   - str: 显示的中文字符串
*				   - Color: 字符颜色   
*				   - bkColor: 背景颜色 
* Output         : None
* Return         : None
* example		 : PutChinese_strings1(200,100,"好人",0,0xffff);
				   PutChinese_strings1(200,150,"坏人",0xff,0xffff);
* Attention		 : 外部调用
*******************************************************************************/
void PutChinese_strings12(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor)	//竖屏
{
    
    uint16_t Tmp_x, Tmp_y;
    uint8_t *tmp_str=str;
    
    Tmp_x = Xpos;
    Tmp_y = Ypos;
    
    Set_direction(1);
    while(*tmp_str != '\0')
    {
        PutChinese12(Tmp_x,Tmp_y,tmp_str,Color,bkColor);
        
        tmp_str += 2 ;
        Tmp_x += 16 ;	
    }
    
}



/******************************************************
 * 函数名：PutChinese_strings21
 * 描述  ：显示汉字字符串
 * 输入  : pos: 0~(319-16)
 *         Ypos: 0~(239-16)
 *				 str: 中文字符串首址
 *				 Color: 字符颜色   
 *				 mode: 0--文字背景色为白色   
 *						   1--文字悬浮 
 * 输出  ：无
 * 举例  ：PutChinese_strings2(200,100,"好人",0,0);
 * 注意	 ：无
 *********************************************************/    
void PutChinese_strings21(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode)	//横屏
{
    
    uint16_t Tmp_x, Tmp_y;
    uint8_t *tmp_str=str;
    Tmp_x = Xpos;
    Tmp_y = Ypos;
    Set_direction(0);
    while(*tmp_str != '\0')
    {
        PutChinese21(Tmp_x,Tmp_y,tmp_str,Color,mode);
        
        tmp_str += 2 ;
        Tmp_x += 16 ;	
    }       
}



/******************************************************************************
* Function Name  : PutChinese_strings22
* Description    : 将Lcd屏上任意位置显示一个中文字	 
* Input          : - Xpos: 0~(239-16)
*                  - Ypos: 0~(319-16)
*				   - str: 显示的中文字符串
*				   - Color: 字符颜色   
*				   - mode: 背景模式选择  0--文字背景色为白色   1--文字悬浮 
* Output         : None
* Return         : None
* example		 : PutChinese_strings2(200,100,"好人",0,0);
				   		 PutChinese_strings2(200,150,"坏人",0,1);
* Attention		 : 外部调用
*/
void PutChinese_strings22(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode) //竖屏
{    
    uint16_t Tmp_x, Tmp_y;
    uint8_t *tmp_str=str;
    Tmp_x = Xpos;
    Tmp_y = Ypos;
    Set_direction(1);
    while(*tmp_str != '\0')
    {
        PutChinese22(Tmp_x,Tmp_y,tmp_str,Color,mode);
        
        tmp_str += 2 ;
        Tmp_x += 16 ;	
    }    
}

/* 
 * 函数名：Put_ascii_chinese_string
 * 描述  ：中英文混合显示，英文大小为16*8
 * 输入  ：-Xpos  x 坐标
 *         -Ypos  y 坐标
 *         -str   文字信息指针
 *         -Color 文字颜色
 *         -mode  显示模式：0 叠加，1 悬浮
 */
void Put_ascii_chinese_string(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode)
{
    uint16_t Tmp_x, Tmp_y;
    uint8_t *tmp_str=str;
    Tmp_x = Xpos;
    Tmp_y = Ypos;
    Set_direction(1);
    while(*tmp_str != '\0')
    {
        if(*tmp_str<125)
				{
					LCD_Show_8x16_Char2(Tmp_x,Tmp_y,mode, *tmp_str);
					tmp_str++ ;
					Tmp_x	+= 8 ;
				}
				else
				{
					if(*tmp_str==163)																//163-172 是中文逗号	 163-187是中文分号
					{
						LCD_Show_8x16_Char2(Tmp_x,Tmp_y,mode, ' ');		//改为显示1个空格
						tmp_str += 2 ;
						Tmp_x	+= 8 ;

					}
					PutChinese22(Tmp_x,Tmp_y,tmp_str,Color,mode);
          tmp_str += 2 ;
        	Tmp_x += 16 ;						
        }
    }
}

/*----------------------------------BMP应用函数 start-------------------------------*/
/*
 * 函数名  ：LCD_ShowBmp
 * 描述    ：显示lenth*wide的16位真彩图片
 * 输入    ：-x 
 *           -y 
 *     		   -p 图片首地址
 * 输出    ：无
 * 调用    ：LCD_ShowBmp(0, 0, 240, 320, (u8 *)p); 
 *           一般是240*320的16bit真彩图。
 *					 外部调用	   
 */
void LCD_ShowBmp(u8 x, u16 y, u8 lenth, u16 wide, const u8 *p)
{      
    u32 n=0 , size=0, temp=0;
    
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, (u16)x+lenth-1); 
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, y+wide-1);
    LCD_WR_CMD(0x0020, x);			 
    LCD_WR_CMD(0x0021, y); 
    LCD_WR_REG(0X22);
    
    size = (u32) lenth * wide * 2; 	 // 16bit真彩,一个像素两个字节
    
    while(n < size) 
    {		
        temp = (uint16_t)(p[n]<<8) + p[n+1];		
        LCD_WR_Data( temp );
        n = n + 2; 
    }
}

void MY_LCD_WR_Data(unsigned int val)
{   
    *(__IO u16 *) (Bank1_LCD_D) = val; 	
}

void bmp(u16 x, u16 y, u16 width, u16 height)
{    
#ifdef HX8347	 
	  LCD_WR_CMD(0x16,0X20); 	 //横屏  左下到右上角方向
    LCD_WR_CMD(0x06, 0);
    LCD_WR_CMD(0x07, x); 
    
    LCD_WR_CMD(0x08, 0);
    LCD_WR_CMD(0x09, x+height-1);
    
    LCD_WR_CMD(0x02, y>>8);
    LCD_WR_CMD(0x03, y-((y>>8)<<8)); 
    
    LCD_WR_CMD(0x04, (y+width-1)>>8);
    LCD_WR_CMD(0x05, (y+width-1)-(((y+width-1)>>8)<<8));
#else
		Set_direction(0);
    LCD_WR_CMD(0X20, x);			//水平显示区地址
    LCD_WR_CMD(0X21, y);			//垂直显示区地址
    
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0052, y); 
    
    LCD_WR_CMD(0x0051, height-1+x);//240	
    LCD_WR_CMD(0x0053, width-1+y); //320
    
    LCD_WR_CMD(0x0003, 0X1028);	
#endif

		LCD_WR_REG(0X22);      
}

void bmp2()
{
    
    Set_direction(0);
    LCD_WR_REG(0X22);
    
}

void bmp3(void)
{
    Set_direction(0);
}


u16 bmp4(u16 x, u16 y)
{
    
#ifdef HX8347
				u16 Read_data=0;
			 	LCD_open_windows(x,y,1,1);
				LCD_WR_REG(0X22);
				Read_data = LCD_RD_data();
				return Read_data;
#else
		LCD_WR_CMD(0X20, x);	
    LCD_WR_CMD(0X21, y);
 		LCD_WR_REG(0X22);
		return  LCD_RD_data();
#endif				 
	 //	return (LCD_BGR2RGB(LCD_RD_data())) ;
       
}
/*----------------------------------BMP应用函数 end-------------------------------*/

/*
 * 函数名：LCD_Init
 * 描述  ：LCD 控制 I/O 初始化
 *         LCD FSMC 初始化
 *         LCD 控制器 ILI9325C 初始化         
 * 输入  ：NONE
 * 输出  ：NONE
 * 调用  ：外部调用
 */
void LCD_Init(void)
{
    unsigned long i;
    
    LCD_GPIO_Config();
    LCD_FSMC_Config();		
    LCD_Rst();
#ifdef HX8347			/*HX8347*/
    //Driving ability Setting
    LCD_WR_CMD(0xEA,0x00); //PTBA[15:8]
    LCD_WR_CMD(0xEB,0x20); //PTBA[7:0]
    LCD_WR_CMD(0xEC,0x3C); //STBA[15:8]
    LCD_WR_CMD(0xED,0xC4); //STBA[7:0]
    LCD_WR_CMD(0xE8,0x48); //OPON[7:0]
    LCD_WR_CMD(0xE9,0x38); //OPON1[7:0]
    LCD_WR_CMD(0xF1,0x01); //OTPS1B
    LCD_WR_CMD(0xF2,0x08); //GEN
    //Gamma 2.2 Setting
    LCD_WR_CMD(0x40,0x01); //
    LCD_WR_CMD(0x41,0x07); //
    LCD_WR_CMD(0x42,0x09); //
    LCD_WR_CMD(0x43,0x19); //
    LCD_WR_CMD(0x44,0x17); //
    LCD_WR_CMD(0x45,0x20); //
    LCD_WR_CMD(0x46,0x18); //
    LCD_WR_CMD(0x47,0x61); //
    LCD_WR_CMD(0x48,0x00); //
    LCD_WR_CMD(0x49,0x10); //
    LCD_WR_CMD(0x4A,0x17); //
    LCD_WR_CMD(0x4B,0x19); //
    LCD_WR_CMD(0x4C,0x14); //
    LCD_WR_CMD(0x50,0x1F); //
    LCD_WR_CMD(0x51,0x28); //
    LCD_WR_CMD(0x52,0x26); //
    LCD_WR_CMD(0x53,0x36); //
    LCD_WR_CMD(0x54,0x38); //
    LCD_WR_CMD(0x55,0x3E); //
    LCD_WR_CMD(0x56,0x1E); //
    LCD_WR_CMD(0x57,0x67); //
    LCD_WR_CMD(0x58,0x0B); //
    LCD_WR_CMD(0x59,0x06); //
    LCD_WR_CMD(0x5A,0x08); //
    LCD_WR_CMD(0x5B,0x0F); //
    LCD_WR_CMD(0x5C,0x1F); //
    LCD_WR_CMD(0x5D,0xCC); //
    
    //Power Voltage Setting
    LCD_WR_CMD(0x1B,0x1B); //VRH=4.65V
    LCD_WR_CMD(0x1A,0x01); //BT (VGH~15V,VGL~-10V,DDVDH~5V)
    LCD_WR_CMD(0x24,0x70); //VMH(VCOM High voltage ~4.2V)
    LCD_WR_CMD(0x25,0x58); //VML(VCOM Low voltage -1.2V)
    
    //****VCOM offset**///
    LCD_WR_CMD(0x23,0x6E); //for Flicker adjust //can reload from OTP
    //Power on Setting
    LCD_WR_CMD(0x18,0x36); //I/P_RADJ,N/P_RADJ, Normal mode 70Hz
    LCD_WR_CMD(0x19,0x01); //OSC_EN='1', start Osc
    LCD_WR_CMD(0x01,0x00); //DP_STB='0', out deep sleep       
    LCD_WR_CMD(0x1F,0xD0);// GAS=1, VOMG=10, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
    LCD_WR_CMD(0x17,0x05); //default 0x06 262k color // 0x05 65k color
    //SET PANEL
    LCD_WR_CMD(0x36,0x09); //SS_P, GS_P,REV_P,BGR_P       
    LCD_WR_CMD(0x28,0x3F); //GON=1, DTE=1, D=1100        
    LCD_WR_CMD(0x16,0x50); //左上到右下   竖屏 
    //LCD_WR_CMD(0x16,0X70); //    
    LCD_WR_CMD(0x02,0x00);
    LCD_WR_CMD(0x03,0x00); //Column Start    
    LCD_WR_CMD(0x04,0x00);
    LCD_WR_CMD(0x05,0xEF); //Column End
		   
    LCD_WR_CMD(0x06,0x00);
    LCD_WR_CMD(0x07,0x00); //Row Start    
    LCD_WR_CMD(0x08,0x01);
    LCD_WR_CMD(0x09,0x3F); //Row End
    
#else    /*ili9325*/
    //    LCD_WR_CMD(0x00E3, 0x3008); // Set internal timing
    LCD_WR_CMD(0x00E7, 0x0012); // Set internal timing
    LCD_WR_CMD(0x00EF, 0x1231); // Set internal timing
    LCD_WR_CMD(0x0000, 0x0001); // Start Oscillation
    
    //LCD_WR_CMD(0x0001, 0x0100); // set SS and SM bit s720->s1(左上角为坐标原点)
    //LCD_WR_CMD(0x0060, 0xA700);	// R01中的SS和R60中的GS一起决定坐标原点
    
    //	LCD_WR_CMD(0x0001, 0x0100);   // set SS and SM bit s720->s1
    LCD_WR_CMD(0x0001, 0x0000);   // set SS and SM bit s1->s720		(右下角为坐标原点)
    //LCD_WR_CMD(0x0060, 0x2700); // R01中的SS和R60中的GS一起决定坐标原点
    
    LCD_WR_CMD(0x0002, 0x0700); // set 1 line inversion
    /*设置扫描方向*/
    //	LCD_WR_CMD(0x0003, 0X1018); // set GRAM write direction and BGR=0,262K colors,1 transfers/pixel.
    LCD_WR_CMD(0x0003, 0X1030); 
    
    LCD_WR_CMD(0x0004, 0x0000); // Resize register
    LCD_WR_CMD(0x0008, 0x0202); // set the back porch and front porch
    LCD_WR_CMD(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
    LCD_WR_CMD(0x000A, 0x0000); // FMARK function
    LCD_WR_CMD(0x000C, 0x0000); // RGB interface setting
    LCD_WR_CMD(0x000D, 0x0000); // Frame marker Position
    LCD_WR_CMD(0x000F, 0x0000); // RGB interface polarity
    //Power On sequence 
    LCD_WR_CMD(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
    LCD_WR_CMD(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
    LCD_WR_CMD(0x0012, 0x0000); // VREG1OUT voltage
    LCD_WR_CMD(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
    Delay(200); // Dis-charge capacitor power voltage
    LCD_WR_CMD(0x0010, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
    LCD_WR_CMD(0x0011, 0x0227); // R11h=0x0221 at VCI=3.3V, DC1[2:0], DC0[2:0], VC[2:0]
    Delay(50); // Delay 50ms
    LCD_WR_CMD(0x0012, 0x001C); // External reference voltage= Vci;
    Delay(50); // Delay 50ms
    LCD_WR_CMD(0x0013, 0x1800); // R13=1200 when R12=009D;VDV[4:0] for VCOM amplitude
    LCD_WR_CMD(0x0029, 0x001C); // R29=000C when R12=009D;VCM[5:0] for VCOMH
    LCD_WR_CMD(0x002B, 0x000D); // Frame Rate = 91Hz
    Delay(50); // Delay 50ms
    LCD_WR_CMD(0x0020, 0x0000); // GRAM horizontal Address
    LCD_WR_CMD(0x0021, 0x0000); // GRAM Vertical Address
    //Adjust the Gamma Curve
    LCD_WR_CMD(0x0030, 0x0007);
    LCD_WR_CMD(0x0031, 0x0302);
    LCD_WR_CMD(0x0032, 0x0105);
    LCD_WR_CMD(0x0035, 0x0206);
    LCD_WR_CMD(0x0036, 0x0808);
    LCD_WR_CMD(0x0037, 0x0206);
    LCD_WR_CMD(0x0038, 0x0504);
    LCD_WR_CMD(0x0039, 0x0007);
    LCD_WR_CMD(0x003C, 0x0105);
    LCD_WR_CMD(0x003D, 0x0808);
    //Set GRAM area
    LCD_WR_CMD(0x0050, 0x0000); // Horizontal GRAM Start Address
    LCD_WR_CMD(0x0051, 0x00EF); // Horizontal GRAM End Address
    LCD_WR_CMD(0x0052, 0x0000); // Vertical GRAM Start Address
    LCD_WR_CMD(0x0053, 0x013F); // Vertical GRAM Start Address
    
    LCD_WR_CMD(0x0060, 0xA700); // Gate Scan Line
    //	LCD_WR_CMD(0x0060, 0x2700);		// 与R01中的SS一起决定坐标的原点
    
    LCD_WR_CMD(0x0061, 0x0001); // NDL,VLE, REV
    LCD_WR_CMD(0x006A, 0x0000); // set scrolling line
    //Partial Display Control
    LCD_WR_CMD(0x0080, 0x0000);
    LCD_WR_CMD(0x0081, 0x0000);
    LCD_WR_CMD(0x0082, 0x0000);
    LCD_WR_CMD(0x0083, 0x0000);
    LCD_WR_CMD(0x0084, 0x0000);
    LCD_WR_CMD(0x0085, 0x0000);
    //Panel Control
    LCD_WR_CMD(0x0090, 0x0010);
    LCD_WR_CMD(0x0092, 0x0000);
    LCD_WR_CMD(0x0093, 0x0003);
    LCD_WR_CMD(0x0095, 0x0110);
    LCD_WR_CMD(0x0097, 0x0000);
    LCD_WR_CMD(0x0098, 0x0000);
    
    // important
    LCD_WR_CMD(0x0007, 0x0133); // 262K color and display ON
    
    // clear the panel	
    LCD_WR_CMD(0x0050, 0);       // Horizontal GRAM Start Address
    LCD_WR_CMD(0x0051, 239);     // Horizontal GRAM End Address
    LCD_WR_CMD(0x0052, 0);       // Vertical GRAM Start Address
    LCD_WR_CMD(0x0053, 319);     // Vertical GRAM Start Address			 
    LCD_WR_CMD(0x0020, 0);			 // set the horizontal initial value of the address counter
    LCD_WR_CMD(0x0021, 0);			 // set the vertical initial value of the address counter  
    Set_direction(0);
    
#endif
    
    LCD_WR_REG(0x22);	
    
    for(i=0; i<(320*240); i++)		 		
    {
        LCD_WR_Data(RED);    //0xffff00  					    
    }
    
}

/*
 * R01H中的SS和R60H中的GS决定了坐标的原点。
 * R03H决定了是水平扫描还是垂直扫描，水平扫描跟垂直扫描又
 * 分为四种方式。水平取模的图像用液晶水平扫描扫描的方式来
 * 显示的话会有上下颠倒或者左右颠倒的现象，但显示的图片是
 * 正确的，若用液晶垂直扫描的方式的话则会出现乱码。
 
 * 注      ：图像水平取模时在液晶上有四种显示方式 
             垂直取模时也有四种显示方式。
 * 水平扫描：0X1000 右下->左上  
 *         : 0X1010	左下->右上
 *				 : 0X1020	右上->左下
 *				 : 0X1030	左上->右下
 * 垂直扫描：0X1008	右下->左上
 *         : 0X1018	左下->右上
 *         : 0X1028	右上->左下
 *         : 0X1038	左上->右下
 */

/******************** (C) MODIFIED  2011 野火嵌入式开发工作室 *****END OF FILE***/

