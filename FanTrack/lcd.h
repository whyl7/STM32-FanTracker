#ifndef __LCD_H
#define	__LCD_H

#include "stm32f10x.h"
/*
 * 函数 Put_ascii_chinese_string 可用于中英混合显示，其他函数比较少用，不用的话可以
 * 不用管，有兴趣的话也可以看下，写得比较乱^_^……。
 * 调用举例 Put_ascii_chinese_string(0, 0,“欢迎使用野火Stm32开发板”, 0, 1);
 * 在调用函数Put_ascii_chinese_string();之前一定要调用函数void LCD_Init();
 * 来初始化液晶。  
 */
void Put_ascii_chinese_string(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode);



/*---------------------------------------------------------------------------------------------*/
void LCD_open_windows(u16 x,u16 y,u16 len,u16 wid);
void Set_direction(u8 option); 
void LCD_CLEAR(u8 x,u16 y,u8 len,u16 wid);
void LCD_draw_rectangle(u16 x,u16 y,u16 len,u16 wid,u16 rgb565);
void LCD_DrawPoint(u16 x,u16 y);
void LCD_Draw_ColorPoint(u16 x,u16 y, u16 rgb565);
void LCD_Show_ColorBar(void);
void LCD_DrawLine(u8 x1, u16 y1, u8 x2, u16 y2);

void LCD_ShowNum(u8 x,u16 y, u8 mode, u32 num);	   //6*12 ascii

void LCD_ShowString(u16 y, u8 x, u8 mode, const u8 *p);	 //6*12	ascii		 //横屏
void LCD_ShowString2(u8 x, u16 y, u8 mode, const u8 *p);	 //6*12	ascii	 //竖屏

void LCD_ShowBmp(u8 x, u16 y, u8 lenth, u16 wide, const u8 *p);

/*-------------------------------------------------*/
void LCD_Init(void);
void LCD_WRITE_SATAT(void);
void MY_LCD_WRITE(u16 rgb565);
void LCD_SetCursor(uint16_t Xpos,uint16_t Ypos);
/*-------------------------------------------------*/

void LCD_Show_8x16_Char(u8 x, u16 y, u8 mode, u8 acsii);
void LCD_Show_8x16_String(u16 y, u8 x, u8 mode, u8 *str); //8*16 ASCII				//横屏

void LCD_Show_8x16_Char2(u8 x, u16 y, u8 mode, u8 acsii);
void LCD_Show_8x16_String2(u8 x, u16 y, u8 mode, u8 *str); //8*16 ASCII				//竖屏

void PutChinese11(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor);				 //横屏
void PutChinese_strings11(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor); //横屏

void PutChinese12(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor);				 //竖屏
void PutChinese_strings12(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor); //竖屏

void PutChinese21(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode);					 //横屏
void PutChinese_strings21(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode);	 //横屏

void PutChinese22(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode);					 //竖屏
void PutChinese_strings22(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode);	 //竖屏

void Put_ascii_chinese_string(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode);

void bmp(u16 x, u16 y, u16 width, u16 height);
void bmp3(void);
u16 bmp4(u16 x, u16 y);
void MY_LCD_WR_Data(unsigned int val);

#define RED	  0XF800
#define GREEN 0X07E0
#define BLUE  0X001F  
#define BRED  0XF81F
#define GRED  0XFFE0
#define GBLUE 0X07FF
#define BLACK 0X0000
#define WHITE 0XFFFF

#define HX8347		/* LCD控制器为 HX8347-G(T) ---如果将该宏定义注释掉，则使用ili9325液晶控制器*/


#endif /* __LCD_H */

