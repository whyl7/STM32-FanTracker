#ifndef __LCD_H
#define	__LCD_H

#include "stm32f10x.h"
/*
 * ���� Put_ascii_chinese_string ��������Ӣ�����ʾ�����������Ƚ����ã����õĻ�����
 * ���ùܣ�����Ȥ�Ļ�Ҳ���Կ��£�д�ñȽ���^_^������
 * ���þ��� Put_ascii_chinese_string(0, 0,����ӭʹ��Ұ��Stm32�����塱, 0, 1);
 * �ڵ��ú���Put_ascii_chinese_string();֮ǰһ��Ҫ���ú���void LCD_Init();
 * ����ʼ��Һ����  
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

void LCD_ShowString(u16 y, u8 x, u8 mode, const u8 *p);	 //6*12	ascii		 //����
void LCD_ShowString2(u8 x, u16 y, u8 mode, const u8 *p);	 //6*12	ascii	 //����

void LCD_ShowBmp(u8 x, u16 y, u8 lenth, u16 wide, const u8 *p);

/*-------------------------------------------------*/
void LCD_Init(void);
void LCD_WRITE_SATAT(void);
void MY_LCD_WRITE(u16 rgb565);
void LCD_SetCursor(uint16_t Xpos,uint16_t Ypos);
/*-------------------------------------------------*/

void LCD_Show_8x16_Char(u8 x, u16 y, u8 mode, u8 acsii);
void LCD_Show_8x16_String(u16 y, u8 x, u8 mode, u8 *str); //8*16 ASCII				//����

void LCD_Show_8x16_Char2(u8 x, u16 y, u8 mode, u8 acsii);
void LCD_Show_8x16_String2(u8 x, u16 y, u8 mode, u8 *str); //8*16 ASCII				//����

void PutChinese11(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor);				 //����
void PutChinese_strings11(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor); //����

void PutChinese12(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor);				 //����
void PutChinese_strings12(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,uint16_t bkColor); //����

void PutChinese21(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode);					 //����
void PutChinese_strings21(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode);	 //����

void PutChinese22(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode);					 //����
void PutChinese_strings22(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode);	 //����

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

#define HX8347		/* LCD������Ϊ HX8347-G(T) ---������ú궨��ע�͵�����ʹ��ili9325Һ��������*/


#endif /* __LCD_H */

