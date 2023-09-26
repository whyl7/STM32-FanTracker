#ifndef _24L01__H_
#define _24L01__H_
#include "stm32f10x.h"
#define uchar unsigned char
#define uint unsigned int
/*******************************************************/
#define TX_ADDR_WITDH 5	//���͵�ַ��������Ϊ5���ֽ�
#define RX_ADDR_WITDH 5	//���յ�ַ��������Ϊ5���ֽ�
#define TX_DATA_WITDH 5//�������ݿ���5���ֽ�
#define RX_DATA_WITDH 5//�������ݿ���5���ֽ�
/*******************����Ĵ���***************************/
#define  R_REGISTER      0x00//��ȡ���üĴ���
#define  W_REGISTER      0x20//д���üĴ���
#define  R_RX_PAYLOAD 	 0x61//��ȡRX��Ч����
#define  W_TX_PAYLOAD	 0xa0//дTX��Ч����
#define  FLUSH_TX		 0xe1//���TXFIFO�Ĵ���
#define  FLUSH_RX		 0xe2//���RXFIFO�Ĵ���
#define  REUSE_TX_PL     0xe3//����ʹ����һ����Ч����
#define  NOP             0xff//�ղ���
/******************�Ĵ�����ַ****************************/
#define  CONFIG          0x00//���üĴ���
#define  EN_AA			 0x01//ʹ���Զ�Ӧ��
#define  EN_RXADDR       0x02//����ͨ��ʹ��0-5��ͨ��
#define  SETUP_AW        0x03//��������ͨ����ַ����3-5
#define  SETUP_RETR      0x04//�����Զ��ط�
#define  RF_CH           0x05//��Ƶͨ������
#define  RF_SETUP        0x06//��Ƶ�Ĵ���
#define  STATUS          0x07//״̬�Ĵ���
#define  OBSERVE_TX      0x08//���ͼ��Ĵ���
#define  CD              0x09//�ز�
#define  RX_ADDR_P0      0x0a//����ͨ��0���յ�ַ
#define  RX_ADDR_P1      0x0b//����ͨ��1���յ�ַ
#define  RX_ADDR_P2      0x0c//����ͨ��2���յ�ַ
#define  RX_ADDR_P3      0x0d//����ͨ��3���յ�ַ
#define  RX_ADDR_P4      0x0e//����ͨ��4���յ�ַ
#define  RX_ADDR_P5      0x0f//����ͨ��5���յ�ַ
#define  TX_ADDR         0x10//���͵�ַ
#define  RX_PW_P0        0x11//P0ͨ�����ݿ�������
#define  RX_PW_P1        0x12//P1ͨ�����ݿ�������
#define  RX_PW_P2        0x13//P2ͨ�����ݿ�������
#define  RX_PW_P3        0x14//P3ͨ�����ݿ�������
#define  RX_PW_P4        0x15//P4ͨ�����ݿ�������
#define  RX_PW_P5        0x16//P5ͨ�����ݿ�������
#define  FIFO_STATUS     0x17//FIFO״̬�Ĵ���
/*******************��غ�������**************************/
uchar NRFACK();
uchar NRFSPI(uchar date);
uchar NRFReadReg(uchar RegAddr);
uchar NRFWriteReg(uchar RegAddr,uchar date);
uchar NRFReadRxDate(uchar RegAddr,uchar *RxDate,uchar DateLen);
uchar NRFWriteTxDate(uchar RegAddr,uchar *TxDate,uchar DateLen);
void NRFSetTxMode0(uchar *TxDate);
void NRF24L01Int();
void NRFSetRXMode();
uchar CheckACK();
extern void GetDate();
/**********************************************************************/
uchar NRFACK1();
uchar NRFSPI1(uchar date);
uchar NRFReadReg1(uchar RegAddr);
uchar NRFWriteReg1(uchar RegAddr,uchar date);
uchar NRFReadRxDate1(uchar RegAddr,uchar *RxDate,uchar DateLen);
uchar NRFWriteTxDate1(uchar RegAddr,uchar *TxDate,uchar DateLen);
void NRFSetTxMode1(uchar *TxDate);
void NRF24L01Int1();
void NRFSetRXMode1();
uchar CheckACK1();
extern void GetDate1();
extern uchar sta;			  //bdata 
extern uchar RevTempDate0[5];//ͬ��0��������
extern uchar RevTempDate1[5];//ͬ��1��������
extern uchar RevTempDate2[5];//ͬ��2��������
/*********************************************************/
#endif