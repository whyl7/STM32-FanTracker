#include "24l01.h"
/**********************��ʱ����**************************/

/*****************��һ��****nrf24l01�뵥Ƭ����*�˿�����*****************/
/*CE 		PC8														  */
/*CSN 		PC9														  */
/*SCLK 		PC10													  */
/*MOSI 		PC11													  */
/*MISO 		PC12													  */
/*IRQ  		PC13													  */
/**********************************************************************/
/****************�ڶ���****nrf24l01�뵥Ƭ����*�˿�����*****************/
/*CE 		PC1													  	  */
/*CSN 		PC2														  */
/*SCLK 		PC3													      */
/*MOSI 		PC4													      */
/*MISO 		PC5													      */
/*IRQ  		PC6													      */
/**********************************************************************/
uchar RevTempDate0[5];//ͬ��0��������
uchar RevTempDate1[5];//ͬ��0��������
uchar RevTempDate2[5];//ͬ��0��������

uchar  RxAddr0[]={0x34,0x43,0x10,0x10,0x01};//A���յ�ַ�����ַ�ͷ��ͷ���ַһ��!

uchar  TxAddr[]={0x34,0x43,0x10,0x10,0x01};//A���յ�ַ�����ַ�ͷ��ͷ���ַһ��!
//uchar  TxAddr0[]={0xc2,0xc2,0xc2,0xc2,0xc2};//B���յ�ַ�����ַ�ͷ��ͷ���ַһ��!
/*****************״̬��־*****************************************/
uchar   sta;   //״̬��־	
#define RX_DR sta&0x40
#define TX_DS sta&0x20
#define MAX_RT sta&0x10
void NRFDelay(uint t)
{
   uint x,y;
   for(x=t;x>0;x--)
    for(y=1100;y>0;y--);	 //1100��stc12le60s2
}
/*****************SPIʱ����******************************************/
uchar NRFSPI(uchar date)
{
    uchar i;
   	for(i=0;i<8;i++)          // ѭ��8��
   	{
	  if(date&0x80)
	  	GPIO_SetBits(GPIOC,GPIO_Pin_11);
//	    MOSI=1;
	  else
	  	GPIO_ResetBits(GPIOC,GPIO_Pin_11);
//	    MOSI=0;   // byte���λ�����MOSI
   	  date<<=1;             // ��һλ��λ�����λ
	  	GPIO_SetBits(GPIOC,GPIO_Pin_10);
//   	  SCLK=1; 
	  if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12))     //MISO //����SCK��nRF24L01��MOSI����1λ���ݣ�ͬʱ��MISO���1λ����
   	    date|=0x01;       	// ��MISO��byte���λ
		GPIO_ResetBits(GPIOC, GPIO_Pin_10);						  
//   	  SCLK=0;            	// SCK�õ�									  
   	}																	  
    return(date);           	// ���ض�����һ�ֽ�							         
}
/*****************SPIʱ����******************************************/
uchar NRFSPI1(uchar date)
{
    uchar i;
   	for(i=0;i<8;i++)          // ѭ��8��
   	{
	  if(date&0x80)
	  	GPIO_SetBits(GPIOC,GPIO_Pin_4);
//	    MOSI=1;
	  else
	  	GPIO_ResetBits(GPIOC,GPIO_Pin_4);
//	    MOSI=0;   // byte���λ�����MOSI
   	  date<<=1;             // ��һλ��λ�����λ
	  	GPIO_SetBits(GPIOC,GPIO_Pin_3);
//   	  SCLK=1; 
	  if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5))     //MISO //����SCK��nRF24L01��MOSI����1λ���ݣ�ͬʱ��MISO���1λ����
   	    date|=0x01;       	// ��MISO��byte���λ
		GPIO_ResetBits(GPIOC, GPIO_Pin_3);						  
//   	  SCLK=0;            	// SCK�õ�									  
   	}																	  
    return(date);           	// ���ض�����һ�ֽ�							         
}
/**********************NRF24L01��ʼ������*******************************/		 
void NRF24L01Int()
{
	NRFDelay(2);//��ϵͳʲô������
		 GPIO_ResetBits(GPIOC, GPIO_Pin_8);
		 GPIO_SetBits(GPIOC, GPIO_Pin_9);
		 GPIO_ResetBits(GPIOC, GPIO_Pin_10);
		 GPIO_SetBits(GPIOC, GPIO_Pin_13);
//	CE=0;
//	CSN=1;  
//	SCLK=0;
//	IRQ=1; 
}
void NRF24L01Int1()
{
	NRFDelay(2);//��ϵͳʲô������
		 GPIO_ResetBits(GPIOC, GPIO_Pin_1);
		 GPIO_SetBits(GPIOC, GPIO_Pin_2);
		 GPIO_ResetBits(GPIOC, GPIO_Pin_3);
		 GPIO_SetBits(GPIOC, GPIO_Pin_6);
//	CE=0;
//	CSN=1;  
//	SCLK=0;
//	IRQ=1; 
}
/*****************SPI���Ĵ���һ�ֽں���*********************************/
uchar NRFReadReg(uchar RegAddr)
{
   uchar BackDate;
   GPIO_ResetBits(GPIOC, GPIO_Pin_9);
//   CSN=0;//����ʱ��
   NRFSPI(RegAddr);//д�Ĵ�����ַ
   BackDate=NRFSPI(0x00);//д����Ĵ���ָ��  
   GPIO_SetBits(GPIOC, GPIO_Pin_9);
//   CSN=1;
   return(BackDate); //����״̬
}
uchar NRFReadReg1(uchar RegAddr)
{
   uchar BackDate;
   GPIO_ResetBits(GPIOC, GPIO_Pin_2);
//   CSN=0;//����ʱ��
   NRFSPI1(RegAddr);//д�Ĵ�����ַ
   BackDate=NRFSPI1(0x00);//д����Ĵ���ָ��  
   GPIO_SetBits(GPIOC, GPIO_Pin_2);
//   CSN=1;
   return(BackDate); //����״̬
}
/*****************SPIд�Ĵ���һ�ֽں���*********************************/
uchar NRFWriteReg(uchar RegAddr,uchar date)
{
   uchar BackDate;
   GPIO_ResetBits(GPIOC, GPIO_Pin_9);
//   CSN=0;//����ʱ��
   BackDate=NRFSPI(RegAddr);//д���ַ
   NRFSPI(date);//д��ֵ
   GPIO_SetBits(GPIOC, GPIO_Pin_9);
//   CSN=1;  
   return(BackDate);
}
uchar NRFWriteReg1(uchar RegAddr,uchar date)
{
   uchar BackDate;
   GPIO_ResetBits(GPIOC, GPIO_Pin_2);
//   CSN=0;//����ʱ��
   BackDate=NRFSPI1(RegAddr);//д���ַ
   NRFSPI1(date);//д��ֵ
   GPIO_SetBits(GPIOC, GPIO_Pin_2);
//   CSN=1;  
   return(BackDate);
}
/*****************SPI��ȡRXFIFO�Ĵ�����ֵ********************************/
uchar NRFReadRxDate(uchar RegAddr,uchar *RxDate,uchar DateLen)
{  //�Ĵ�����ַ//��ȡ���ݴ�ű���//��ȡ���ݳ���//���ڽ���
    uchar BackDate,i;
   GPIO_ResetBits(GPIOC, GPIO_Pin_9);
//	CSN=0;//����ʱ��
	BackDate=NRFSPI(RegAddr);//д��Ҫ��ȡ�ļĴ�����ַ
	for(i=0;i<DateLen;i++) //��ȡ����
	  {
	     RxDate[i]=NRFSPI(0);
	  } 
   GPIO_SetBits(GPIOC, GPIO_Pin_9);
//    CSN=1;
   return(BackDate); 
}
uchar NRFReadRxDate1(uchar RegAddr,uchar *RxDate,uchar DateLen)
{  //�Ĵ�����ַ//��ȡ���ݴ�ű���//��ȡ���ݳ���//���ڽ���
    uchar BackDate,i;
   GPIO_ResetBits(GPIOC, GPIO_Pin_2);
//	CSN=0;//����ʱ��
	BackDate=NRFSPI1(RegAddr);//д��Ҫ��ȡ�ļĴ�����ַ
	for(i=0;i<DateLen;i++) //��ȡ����
	  {
	     RxDate[i]=NRFSPI1(0);
	  } 
   GPIO_SetBits(GPIOC, GPIO_Pin_2);
//    CSN=1;
   return(BackDate); 
}
/*****************SPIд��TXFIFO�Ĵ�����ֵ**********************************/
uchar NRFWriteTxDate(uchar RegAddr,uchar *TxDate,uchar DateLen)
{ //�Ĵ�����ַ//д�����ݴ�ű���//��ȡ���ݳ���//���ڷ���
   uchar BackDate,i;
   GPIO_ResetBits(GPIOC, GPIO_Pin_9);
//   CSN=0;
   BackDate=NRFSPI(RegAddr);//д��Ҫд��Ĵ����ĵ�ַ
   for(i=0;i<DateLen;i++)//д������
     {
	    NRFSPI(*TxDate++);
	 }
   GPIO_SetBits(GPIOC, GPIO_Pin_9);   
//   CSN=1;
   return(BackDate);
}
uchar NRFWriteTxDate1(uchar RegAddr,uchar *TxDate,uchar DateLen)
{ //�Ĵ�����ַ//д�����ݴ�ű���//��ȡ���ݳ���//���ڷ���
   uchar BackDate,i;
   GPIO_ResetBits(GPIOC, GPIO_Pin_2);
//   CSN=0;
   BackDate=NRFSPI1(RegAddr);//д��Ҫд��Ĵ����ĵ�ַ
   for(i=0;i<DateLen;i++)//д������
     {
	    NRFSPI1(*TxDate++);
	 }
   GPIO_SetBits(GPIOC, GPIO_Pin_2);   
//   CSN=1;
   return(BackDate);
}
/*****************NRF����Ϊ����ģʽ����������******************************/
//void NRFSetTxMode(uchar *TxDate)
//{  //����ģʽ
//	GPIO_ResetBits(GPIOC, GPIO_Pin_8); 
////    CE=0;   
//  	NRFWriteTxDate(W_REGISTER+TX_ADDR,RxAddr0,TX_ADDR_WITDH);//д�Ĵ���ָ��+P0��ַʹ��ָ��+���͵�ַ+��ַ���
////  	NRFWriteTxDate(W_REGISTER+TX_ADDR,RxAddr1,TX_ADDR_WITDH);//д�Ĵ���ָ��+P0��ַʹ��ָ��+���͵�ַ+��ַ���
////  	NRFWriteTxDate(W_REGISTER+TX_ADDR,RxAddr2,1);//д�Ĵ���ָ��+P0��ַʹ��ָ��+���͵�ַ+��ַ���
//
//										  // TX_ADDR_WITDH
//	NRFWriteTxDate(W_TX_PAYLOAD,TxDate,TX_DATA_WITDH);//д������ 
//	//******�����йؼĴ�������**************
//  	NRFWriteReg(W_REGISTER+EN_AA,0x3f);       // ʹ�ܽ���ͨ��0�Զ�Ӧ��0x01	0x3f	  
//  	NRFWriteReg(W_REGISTER+EN_RXADDR,0x3f);   // ʹ�ܽ���ͨ��0	0x01  0x3f
//				  
//  	NRFWriteReg(W_REGISTER+SETUP_RETR,0x0a);  // �Զ��ط���ʱ�ȴ�250us+86us���Զ��ط�10��
//  	NRFWriteReg(W_REGISTER+RF_CH,0x40);         // ѡ����Ƶͨ��0x40
//  	NRFWriteReg(W_REGISTER+RF_SETUP,0x07);    // ���ݴ�����1Mbps�����书��0dBm���������Ŵ�������
//  	NRFWriteReg(W_REGISTER+CONFIG,0x0e);      // CRCʹ�ܣ�16λCRCУ�飬�ϵ�	
//	GPIO_SetBits(GPIOC, GPIO_Pin_8); 
////	CE=1;	
//	NRFDelay(5);//����10us������
//
//}  	
//void NRFSetTxMode1(uchar *TxDate)
//{  //����ģʽ
//	GPIO_ResetBits(GPIOC, GPIO_Pin_8); 
////    CE=0;   
//  	NRFWriteTxDate1(W_REGISTER+TX_ADDR,RxAddr0,TX_ADDR_WITDH);//д�Ĵ���ָ��+P0��ַʹ��ָ��+���͵�ַ+��ַ���
////  	NRFWriteTxDate(W_REGISTER+TX_ADDR,RxAddr1,TX_ADDR_WITDH);//д�Ĵ���ָ��+P0��ַʹ��ָ��+���͵�ַ+��ַ���
////  	NRFWriteTxDate(W_REGISTER+TX_ADDR,RxAddr2,1);//д�Ĵ���ָ��+P0��ַʹ��ָ��+���͵�ַ+��ַ���
//
//										  // TX_ADDR_WITDH
//	NRFWriteTxDate1(W_TX_PAYLOAD,TxDate,TX_DATA_WITDH);//д������ 
//	//******�����йؼĴ�������**************
//  	NRFWriteReg1(W_REGISTER+EN_AA,0x3f);       // ʹ�ܽ���ͨ��0�Զ�Ӧ��0x01	0x3f	  
//  	NRFWriteReg1(W_REGISTER+EN_RXADDR,0x3f);   // ʹ�ܽ���ͨ��0	0x01  0x3f
//				  
//  	NRFWriteReg1(W_REGISTER+SETUP_RETR,0x0a);  // �Զ��ط���ʱ�ȴ�250us+86us���Զ��ط�10��
//  	NRFWriteReg1(W_REGISTER+RF_CH,0x40);         // ѡ����Ƶͨ��0x40
//  	NRFWriteReg1(W_REGISTER+RF_SETUP,0x07);    // ���ݴ�����1Mbps�����书��0dBm���������Ŵ�������
//  	NRFWriteReg1(W_REGISTER+CONFIG,0x0e);      // CRCʹ�ܣ�16λCRCУ�飬�ϵ�	
//	GPIO_SetBits(GPIOC, GPIO_Pin_8); 
////	CE=1;	
//	NRFDelay(5);//����10us������
//
//}  	    
//void NRFSetTxMode1(uchar *TxDate)
//{  //����ģʽ
//	GPIO_ResetBits(GPIOC, GPIO_Pin_8);  
//   	NRFWriteTxDate(W_REGISTER+TX_ADDR,TxAddr1,TX_ADDR_WITDH);//д�Ĵ���ָ��+���յ�ַʹ��ָ��+���յ�ַ+��ַ���
//	NRFWriteTxDate(W_REGISTER+RX_ADDR_P1,TxAddr1,TX_ADDR_WITDH);//Ϊ��Ӧ������豸������ͨ��0��ַ�ͷ��͵�ַ��ͬ
//	NRFWriteTxDate(W_TX_PAYLOAD,TxDate,TX_DATA_WITDH);//д������ 
//	/******�����йؼĴ�������**************/
//  	NRFWriteReg(W_REGISTER+EN_AA,0x3f);       // ʹ�ܽ���ͨ��0�Զ�Ӧ��
//  	NRFWriteReg(W_REGISTER+EN_RXADDR,0x3f);   // ʹ�ܽ���ͨ��0
//  	NRFWriteReg(W_REGISTER+SETUP_RETR,0x0a);  // �Զ��ط���ʱ�ȴ�250us+86us���Զ��ط�10��
//  	NRFWriteReg(W_REGISTER+RF_CH,0x40);         // ѡ����Ƶͨ��0x40
//  	NRFWriteReg(W_REGISTER+RF_SETUP,0x07);    // ���ݴ�����1Mbps�����书��0dBm���������Ŵ�������
//	NRFWriteReg(W_REGISTER+CONFIG,0x0e);      // CRCʹ�ܣ�16λCRCУ�飬�ϵ�  
//	//CE=1;
//	GPIO_SetBits(GPIOC, GPIO_Pin_8); 
//	NRFDelay(5);//����10us������
//}  
//void NRFSetTxMode2(uchar *TxDate)
//{  
//	GPIO_ResetBits(GPIOC, GPIO_Pin_8);  
//   	NRFWriteTxDate(W_REGISTER+TX_ADDR,TxAddr2,TX_ADDR_WITDH);//д�Ĵ���ָ��+���յ�ַʹ��ָ��+���յ�ַ+��ַ���
//	NRFWriteTxDate(W_REGISTER+RX_ADDR_P2,TxAddr2,TX_ADDR_WITDH);//Ϊ��Ӧ������豸������ͨ��0��ַ�ͷ��͵�ַ��ͬ
//	NRFWriteTxDate(W_TX_PAYLOAD,TxDate,TX_DATA_WITDH);//д������ 
//	/******�����йؼĴ�������**************/
//  	NRFWriteReg(W_REGISTER+EN_AA,0x3f);       // ʹ�ܽ���ͨ��0�Զ�Ӧ��
//  	NRFWriteReg(W_REGISTER+EN_RXADDR,0x3f);   // ʹ�ܽ���ͨ��0
//  	NRFWriteReg(W_REGISTER+SETUP_RETR,0x0a);  // �Զ��ط���ʱ�ȴ�250us+86us���Զ��ط�10��
//  	NRFWriteReg(W_REGISTER+RF_CH,0x40);         // ѡ����Ƶͨ��0x40
//  	NRFWriteReg(W_REGISTER+RF_SETUP,0x07);    // ���ݴ�����1Mbps�����书��0dBm���������Ŵ�������
//	NRFWriteReg(W_REGISTER+CONFIG,0x0e);      // CRCʹ�ܣ�16λCRCУ�飬�ϵ�  
//	//CE=1;
//	GPIO_SetBits(GPIOC, GPIO_Pin_8); 
//	NRFDelay(5);//����10us������
//}  
/*****************NRF����Ϊ����ģʽ����������******************************/
//����ģʽ
void NRFSetRXMode()
{
    GPIO_ResetBits(GPIOC, GPIO_Pin_8);
//    CE=0;
  	NRFWriteTxDate(W_REGISTER+RX_ADDR_P0,RxAddr0,TX_ADDR_WITDH);  // �����豸����ͨ��0ʹ�úͷ����豸��ͬ�ķ��͵�ַ
//	NRFWriteTxDate(W_REGISTER+RX_ADDR_P1,RxAddr1,TX_ADDR_WITDH);  // �����豸����ͨ��1ʹ�úͷ����豸��ͬ�ķ��͵�ַ
//	NRFWriteTxDate(W_REGISTER+RX_ADDR_P2,RxAddr2,1);			  //ע�⣡����	TX_ADDR_WITDH	
  	
	NRFWriteReg(W_REGISTER+EN_AA,0x3f);               // ʹ������ͨ��0��1�Զ�Ӧ��	
  	NRFWriteReg(W_REGISTER+EN_RXADDR,0x3f);           // ʹ�ܽ���ͨ��0��1
             
  	NRFWriteReg(W_REGISTER+RX_PW_P0,TX_DATA_WITDH);  // ����ͨ��0ѡ��ͷ���ͨ����ͬ��Ч���ݿ��
//	NRFWriteReg(W_REGISTER+RX_PW_P1,TX_DATA_WITDH);  // ����ͨ��1ѡ��ͷ���ͨ����ͬ��Ч���ݿ��
// 	NRFWriteReg(W_REGISTER+RX_PW_P2,TX_DATA_WITDH);	  // ����ͨ��2ѡ��ͷ���ͨ����ͬ��Ч���ݿ��

	NRFWriteReg(W_REGISTER+RF_CH,0x40);// ѡ����Ƶͨ��0x40 
	NRFWriteReg(W_REGISTER+RF_SETUP,0x07);            // ���ݴ�����1Mbps�����书��0dBm���������Ŵ�������
	NRFWriteReg(W_REGISTER+CONFIG,0x0f);             // CRCʹ�ܣ�16λCRCУ�飬�ϵ磬����ģʽ 
    GPIO_SetBits(GPIOC, GPIO_Pin_8);    
//  	CE = 1; 
	NRFDelay(5);    
}	 
void NRFSetRXMode1()
{
    GPIO_ResetBits(GPIOC, GPIO_Pin_1);
//    CE=0;
  	NRFWriteTxDate1(W_REGISTER+RX_ADDR_P0,RxAddr0,TX_ADDR_WITDH);  // �����豸����ͨ��0ʹ�úͷ����豸��ͬ�ķ��͵�ַ
//	NRFWriteTxDate(W_REGISTER+RX_ADDR_P1,RxAddr1,TX_ADDR_WITDH);  // �����豸����ͨ��1ʹ�úͷ����豸��ͬ�ķ��͵�ַ
//	NRFWriteTxDate(W_REGISTER+RX_ADDR_P2,RxAddr2,1);			  //ע�⣡����	TX_ADDR_WITDH	
  	
	NRFWriteReg1(W_REGISTER+EN_AA,0x3f);               // ʹ������ͨ��0��1�Զ�Ӧ��	
  	NRFWriteReg1(W_REGISTER+EN_RXADDR,0x3f);           // ʹ�ܽ���ͨ��0��1
             
  	NRFWriteReg1(W_REGISTER+RX_PW_P0,TX_DATA_WITDH);  // ����ͨ��0ѡ��ͷ���ͨ����ͬ��Ч���ݿ��
//	NRFWriteReg(W_REGISTER+RX_PW_P1,TX_DATA_WITDH);  // ����ͨ��1ѡ��ͷ���ͨ����ͬ��Ч���ݿ��
// 	NRFWriteReg(W_REGISTER+RX_PW_P2,TX_DATA_WITDH);	  // ����ͨ��2ѡ��ͷ���ͨ����ͬ��Ч���ݿ��

	NRFWriteReg1(W_REGISTER+RF_CH,0x40);// ѡ����Ƶͨ��0x40 
	NRFWriteReg1(W_REGISTER+RF_SETUP,0x07);            // ���ݴ�����1Mbps�����书��0dBm���������Ŵ�������
	NRFWriteReg1(W_REGISTER+CONFIG,0x0f);             // CRCʹ�ܣ�16λCRCУ�飬�ϵ磬����ģʽ 
    GPIO_SetBits(GPIOC, GPIO_Pin_1);    
//  	CE = 1; 
	NRFDelay(5);    
}	   
/****************************����Ƿ��н��յ�����******************************/
/*void CheckACK()
{  //���ڷ���ģʽ����Ӧ���ź�
	sta=NRFReadReg(R_REGISTER+STATUS);                    // ����״̬�Ĵ���
	if(TX_DS)
	   NRFWriteReg(W_REGISTER+STATUS,0xff);  // ���TX_DS��MAX_RT�жϱ�־
}*/
/****************************���Ӧ���ź�******************************/
/*uchar CheckACK()
{  //���ڷ���
	sta=NRFReadReg(R_REGISTER+STATUS);                    // ����״̬�Ĵ���
	if(TX_DS||MAX_RT) //��������ж�
	{  GPIO_SetBits(GPIOC,GPIO_Pin_0);NRFDelay(1000);
	   NRFWriteReg(W_REGISTER+STATUS,0xff);  // ���TX_DS��MAX_RT�жϱ�־
	  GPIO_ResetBits(GPIOC, GPIO_Pin_9); 
//	   CSN=0;
	   NRFSPI(FLUSH_TX);//�������FIFO �����ؼ�������Ȼ��������벻���ĺ����������Ҽ�ס����
	   GPIO_SetBits(GPIOC, GPIO_Pin_9);   
//       CSN=1; 
	   return(0);
	}
	else
	   return(1);
}	 */
void NRFSetTxMode(uchar *TxDate)
{//����ģʽ
  //  CE=0; 
  GPIO_ResetBits(GPIOC, GPIO_Pin_8);
   	NRFWriteTxDate(W_REGISTER+TX_ADDR,TxAddr,TX_ADDR_WITDH);//д�Ĵ���ָ��+���յ�ַʹ��ָ��+���յ�ַ+��ַ���
	NRFWriteTxDate(W_REGISTER+RX_ADDR_P0,TxAddr,TX_ADDR_WITDH);//Ϊ��Ӧ������豸������ͨ��0��ַ�ͷ��͵�ַ��ͬ
	NRFWriteTxDate(W_TX_PAYLOAD,TxDate,TX_DATA_WITDH);//д������ 
	/******�����йؼĴ�������**************/
  	NRFWriteReg(W_REGISTER+EN_AA,0x3f);       // ʹ�ܽ���ͨ��0�Զ�Ӧ��
  	NRFWriteReg(W_REGISTER+EN_RXADDR,0x3f);   // ʹ�ܽ���ͨ��0
  	NRFWriteReg(W_REGISTER+SETUP_RETR,0x0a);  // �Զ��ط���ʱ�ȴ�250us+86us���Զ��ط�10��
  	NRFWriteReg(W_REGISTER+RF_CH,0x40);         // ѡ����Ƶͨ��0x40
  	NRFWriteReg(W_REGISTER+RF_SETUP,0x07);    // ���ݴ�����1Mbps�����书��0dBm���������Ŵ�������
	NRFWriteReg(W_REGISTER+CONFIG,0x0e);      // CRCʹ�ܣ�16λCRCУ�飬�ϵ�  
	//CE=1;
	GPIO_SetBits(GPIOC, GPIO_Pin_8);
	NRFDelay(5);//����10us������
}
void NRFSetTxMode1(uchar *TxDate)
{//����ģʽ
  //  CE=0; 
  GPIO_ResetBits(GPIOC, GPIO_Pin_1);
   	NRFWriteTxDate1(W_REGISTER+TX_ADDR,TxAddr,TX_ADDR_WITDH);//д�Ĵ���ָ��+���յ�ַʹ��ָ��+���յ�ַ+��ַ���
	NRFWriteTxDate1(W_REGISTER+RX_ADDR_P0,TxAddr,TX_ADDR_WITDH);//Ϊ��Ӧ������豸������ͨ��0��ַ�ͷ��͵�ַ��ͬ
	NRFWriteTxDate1(W_TX_PAYLOAD,TxDate,TX_DATA_WITDH);//д������ 
	/******�����йؼĴ�������**************/
  	NRFWriteReg1(W_REGISTER+EN_AA,0x3f);       // ʹ�ܽ���ͨ��0�Զ�Ӧ��
  	NRFWriteReg1(W_REGISTER+EN_RXADDR,0x3f);   // ʹ�ܽ���ͨ��0
  	NRFWriteReg1(W_REGISTER+SETUP_RETR,0x0a);  // �Զ��ط���ʱ�ȴ�250us+86us���Զ��ط�10��
  	NRFWriteReg1(W_REGISTER+RF_CH,0x40);         // ѡ����Ƶͨ��0x40
  	NRFWriteReg1(W_REGISTER+RF_SETUP,0x07);    // ���ݴ�����1Mbps�����书��0dBm���������Ŵ�������
	NRFWriteReg1(W_REGISTER+CONFIG,0x0e);      // CRCʹ�ܣ�16λCRCУ�飬�ϵ�  
	//CE=1;
	GPIO_SetBits(GPIOC, GPIO_Pin_1);
	NRFDelay(5);//����10us������
}
/****************************���Ӧ���ź�******************************/
uchar CheckACK()
{  //���ڷ���
	sta=NRFReadReg(R_REGISTER+STATUS);                    // ����״̬�Ĵ���
	if(TX_DS||MAX_RT) //��������ж�
	{
	   NRFWriteReg(W_REGISTER+STATUS,0xff);  // ���TX_DS��MAX_RT�жϱ�־
	    GPIO_ResetBits(GPIOC, GPIO_Pin_9); 
	//   CSN=0;
	   NRFSPI(FLUSH_TX);//�������FIFO �����ؼ�������Ȼ��������벻���ĺ����������Ҽ�ס����  
 GPIO_SetBits(GPIOC, GPIO_Pin_9); 
      // CSN=1; 
	   return(0);
	}
	else
	   return(1);
}
uchar CheckACK1()
{  //���ڷ���
	sta=NRFReadReg1(R_REGISTER+STATUS);                    // ����״̬�Ĵ���
	if(TX_DS||MAX_RT) //��������ж�
	{
	   NRFWriteReg1(W_REGISTER+STATUS,0xff);  // ���TX_DS��MAX_RT�жϱ�־
	    GPIO_ResetBits(GPIOC, GPIO_Pin_2); 
	//   CSN=0;
	   NRFSPI1(FLUSH_TX);//�������FIFO �����ؼ�������Ȼ��������벻���ĺ����������Ҽ�ס����  
 GPIO_SetBits(GPIOC, GPIO_Pin_2); 
      // CSN=1; 
	   return(0);
	}
	else
	   return(1);
}
/*************************��������*********************************************/
void GetDate()				 
{
    uchar RX_P_NO;//����ͨ����
	sta=NRFReadReg(R_REGISTER+STATUS);//�������ݺ��ȡ״̬�Ĵ���	  
    if(RX_DR)				 // �ж��Ƿ���յ�����
     {	 
	   RX_P_NO=sta&0x0e;    //��ȡͨ����				0000 1110
    	GPIO_ResetBits(GPIOC, GPIO_Pin_8);	
				                     
//	   CE=0;                //����	  
		
	   switch(RX_P_NO)
	    {																		  // 
		   case 0x00:NRFReadRxDate(R_RX_PAYLOAD,RevTempDate0,RX_DATA_WITDH);break;// ��RXFIFO��ȡ����ͨ��0
		   case 0x02:NRFReadRxDate(R_RX_PAYLOAD,RevTempDate0,RX_DATA_WITDH);break;// ��RXFIFO��ȡ����ͨ��1
		   case 0x04:NRFReadRxDate(R_RX_PAYLOAD,RevTempDate0,RX_DATA_WITDH); break;// ��RXFIFO��ȡ����ͨ��2 
		   default:break;		  						//
		}	 					  					          
	   NRFWriteReg(W_REGISTER+STATUS,0xff); //���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�
    	GPIO_ResetBits(GPIOC, GPIO_Pin_9);	   
//	   CSN=0;				
	   NRFSPI(FLUSH_RX);//�������FIFO �����ؼ�������Ȼ��������벻���ĺ����������Ҽ�ס���� 
	   GPIO_SetBits(GPIOC, GPIO_Pin_9);
//	   CSN=1;		 	 	
     }						                                                
       //NRFWriteReg(W_REGISTER+STATUS,0xff); //���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�	  
} 
void GetDate1()				 
{
    uchar RX_P_NO;//����ͨ����
	sta=NRFReadReg1(R_REGISTER+STATUS);//�������ݺ��ȡ״̬�Ĵ���	  
    if(RX_DR)				 // �ж��Ƿ���յ�����
     {	 
	   RX_P_NO=sta&0x0e;    //��ȡͨ����				0000 1110
    	GPIO_ResetBits(GPIOC, GPIO_Pin_1);	
				                     
//	   CE=0;                //����	  
		
	   switch(RX_P_NO)
	    {																		  // 
		   case 0x00:NRFReadRxDate1(R_RX_PAYLOAD,RevTempDate0,RX_DATA_WITDH);break;// ��RXFIFO��ȡ����ͨ��0
		   case 0x02:NRFReadRxDate1(R_RX_PAYLOAD,RevTempDate0,RX_DATA_WITDH);break;// ��RXFIFO��ȡ����ͨ��1
		   case 0x04:NRFReadRxDate1(R_RX_PAYLOAD,RevTempDate0,RX_DATA_WITDH); break;// ��RXFIFO��ȡ����ͨ��2 
		   default:break;		  						//
		}	 					  					          
	   NRFWriteReg1(W_REGISTER+STATUS,0xff); //���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�
    	GPIO_ResetBits(GPIOC, GPIO_Pin_2);	   
//	   CSN=0;				
	   NRFSPI1(FLUSH_RX);//�������FIFO �����ؼ�������Ȼ��������벻���ĺ����������Ҽ�ס���� 
	   GPIO_SetBits(GPIOC, GPIO_Pin_2);
//	   CSN=1;		 	 	
     }						                                                
       //NRFWriteReg(W_REGISTER+STATUS,0xff); //���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�	  
} 
