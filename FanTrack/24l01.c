#include "24l01.h"
/**********************延时函数**************************/

/*****************第一个****nrf24l01与单片机的*端口连接*****************/
/*CE 		PC8														  */
/*CSN 		PC9														  */
/*SCLK 		PC10													  */
/*MOSI 		PC11													  */
/*MISO 		PC12													  */
/*IRQ  		PC13													  */
/**********************************************************************/
/****************第二个****nrf24l01与单片机的*端口连接*****************/
/*CE 		PC1													  	  */
/*CSN 		PC2														  */
/*SCLK 		PC3													      */
/*MOSI 		PC4													      */
/*MISO 		PC5													      */
/*IRQ  		PC6													      */
/**********************************************************************/
uchar RevTempDate0[5];//同道0接收数据
uchar RevTempDate1[5];//同道0接收数据
uchar RevTempDate2[5];//同道0接收数据

uchar  RxAddr0[]={0x34,0x43,0x10,0x10,0x01};//A接收地址这个地址和发送方地址一样!

uchar  TxAddr[]={0x34,0x43,0x10,0x10,0x01};//A接收地址这个地址和发送方地址一样!
//uchar  TxAddr0[]={0xc2,0xc2,0xc2,0xc2,0xc2};//B接收地址这个地址和发送方地址一样!
/*****************状态标志*****************************************/
uchar   sta;   //状态标志	
#define RX_DR sta&0x40
#define TX_DS sta&0x20
#define MAX_RT sta&0x10
void NRFDelay(uint t)
{
   uint x,y;
   for(x=t;x>0;x--)
    for(y=1100;y>0;y--);	 //1100是stc12le60s2
}
/*****************SPI时序函数******************************************/
uchar NRFSPI(uchar date)
{
    uchar i;
   	for(i=0;i<8;i++)          // 循环8次
   	{
	  if(date&0x80)
	  	GPIO_SetBits(GPIOC,GPIO_Pin_11);
//	    MOSI=1;
	  else
	  	GPIO_ResetBits(GPIOC,GPIO_Pin_11);
//	    MOSI=0;   // byte最高位输出到MOSI
   	  date<<=1;             // 低一位移位到最高位
	  	GPIO_SetBits(GPIOC,GPIO_Pin_10);
//   	  SCLK=1; 
	  if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12))     //MISO //拉高SCK，nRF24L01从MOSI读入1位数据，同时从MISO输出1位数据
   	    date|=0x01;       	// 读MISO到byte最低位
		GPIO_ResetBits(GPIOC, GPIO_Pin_10);						  
//   	  SCLK=0;            	// SCK置低									  
   	}																	  
    return(date);           	// 返回读出的一字节							         
}
/*****************SPI时序函数******************************************/
uchar NRFSPI1(uchar date)
{
    uchar i;
   	for(i=0;i<8;i++)          // 循环8次
   	{
	  if(date&0x80)
	  	GPIO_SetBits(GPIOC,GPIO_Pin_4);
//	    MOSI=1;
	  else
	  	GPIO_ResetBits(GPIOC,GPIO_Pin_4);
//	    MOSI=0;   // byte最高位输出到MOSI
   	  date<<=1;             // 低一位移位到最高位
	  	GPIO_SetBits(GPIOC,GPIO_Pin_3);
//   	  SCLK=1; 
	  if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5))     //MISO //拉高SCK，nRF24L01从MOSI读入1位数据，同时从MISO输出1位数据
   	    date|=0x01;       	// 读MISO到byte最低位
		GPIO_ResetBits(GPIOC, GPIO_Pin_3);						  
//   	  SCLK=0;            	// SCK置低									  
   	}																	  
    return(date);           	// 返回读出的一字节							         
}
/**********************NRF24L01初始化函数*******************************/		 
void NRF24L01Int()
{
	NRFDelay(2);//让系统什么都不干
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
	NRFDelay(2);//让系统什么都不干
		 GPIO_ResetBits(GPIOC, GPIO_Pin_1);
		 GPIO_SetBits(GPIOC, GPIO_Pin_2);
		 GPIO_ResetBits(GPIOC, GPIO_Pin_3);
		 GPIO_SetBits(GPIOC, GPIO_Pin_6);
//	CE=0;
//	CSN=1;  
//	SCLK=0;
//	IRQ=1; 
}
/*****************SPI读寄存器一字节函数*********************************/
uchar NRFReadReg(uchar RegAddr)
{
   uchar BackDate;
   GPIO_ResetBits(GPIOC, GPIO_Pin_9);
//   CSN=0;//启动时序
   NRFSPI(RegAddr);//写寄存器地址
   BackDate=NRFSPI(0x00);//写入读寄存器指令  
   GPIO_SetBits(GPIOC, GPIO_Pin_9);
//   CSN=1;
   return(BackDate); //返回状态
}
uchar NRFReadReg1(uchar RegAddr)
{
   uchar BackDate;
   GPIO_ResetBits(GPIOC, GPIO_Pin_2);
//   CSN=0;//启动时序
   NRFSPI1(RegAddr);//写寄存器地址
   BackDate=NRFSPI1(0x00);//写入读寄存器指令  
   GPIO_SetBits(GPIOC, GPIO_Pin_2);
//   CSN=1;
   return(BackDate); //返回状态
}
/*****************SPI写寄存器一字节函数*********************************/
uchar NRFWriteReg(uchar RegAddr,uchar date)
{
   uchar BackDate;
   GPIO_ResetBits(GPIOC, GPIO_Pin_9);
//   CSN=0;//启动时序
   BackDate=NRFSPI(RegAddr);//写入地址
   NRFSPI(date);//写入值
   GPIO_SetBits(GPIOC, GPIO_Pin_9);
//   CSN=1;  
   return(BackDate);
}
uchar NRFWriteReg1(uchar RegAddr,uchar date)
{
   uchar BackDate;
   GPIO_ResetBits(GPIOC, GPIO_Pin_2);
//   CSN=0;//启动时序
   BackDate=NRFSPI1(RegAddr);//写入地址
   NRFSPI1(date);//写入值
   GPIO_SetBits(GPIOC, GPIO_Pin_2);
//   CSN=1;  
   return(BackDate);
}
/*****************SPI读取RXFIFO寄存器的值********************************/
uchar NRFReadRxDate(uchar RegAddr,uchar *RxDate,uchar DateLen)
{  //寄存器地址//读取数据存放变量//读取数据长度//用于接收
    uchar BackDate,i;
   GPIO_ResetBits(GPIOC, GPIO_Pin_9);
//	CSN=0;//启动时序
	BackDate=NRFSPI(RegAddr);//写入要读取的寄存器地址
	for(i=0;i<DateLen;i++) //读取数据
	  {
	     RxDate[i]=NRFSPI(0);
	  } 
   GPIO_SetBits(GPIOC, GPIO_Pin_9);
//    CSN=1;
   return(BackDate); 
}
uchar NRFReadRxDate1(uchar RegAddr,uchar *RxDate,uchar DateLen)
{  //寄存器地址//读取数据存放变量//读取数据长度//用于接收
    uchar BackDate,i;
   GPIO_ResetBits(GPIOC, GPIO_Pin_2);
//	CSN=0;//启动时序
	BackDate=NRFSPI1(RegAddr);//写入要读取的寄存器地址
	for(i=0;i<DateLen;i++) //读取数据
	  {
	     RxDate[i]=NRFSPI1(0);
	  } 
   GPIO_SetBits(GPIOC, GPIO_Pin_2);
//    CSN=1;
   return(BackDate); 
}
/*****************SPI写入TXFIFO寄存器的值**********************************/
uchar NRFWriteTxDate(uchar RegAddr,uchar *TxDate,uchar DateLen)
{ //寄存器地址//写入数据存放变量//读取数据长度//用于发送
   uchar BackDate,i;
   GPIO_ResetBits(GPIOC, GPIO_Pin_9);
//   CSN=0;
   BackDate=NRFSPI(RegAddr);//写入要写入寄存器的地址
   for(i=0;i<DateLen;i++)//写入数据
     {
	    NRFSPI(*TxDate++);
	 }
   GPIO_SetBits(GPIOC, GPIO_Pin_9);   
//   CSN=1;
   return(BackDate);
}
uchar NRFWriteTxDate1(uchar RegAddr,uchar *TxDate,uchar DateLen)
{ //寄存器地址//写入数据存放变量//读取数据长度//用于发送
   uchar BackDate,i;
   GPIO_ResetBits(GPIOC, GPIO_Pin_2);
//   CSN=0;
   BackDate=NRFSPI1(RegAddr);//写入要写入寄存器的地址
   for(i=0;i<DateLen;i++)//写入数据
     {
	    NRFSPI1(*TxDate++);
	 }
   GPIO_SetBits(GPIOC, GPIO_Pin_2);   
//   CSN=1;
   return(BackDate);
}
/*****************NRF设置为发送模式并发送数据******************************/
//void NRFSetTxMode(uchar *TxDate)
//{  //发送模式
//	GPIO_ResetBits(GPIOC, GPIO_Pin_8); 
////    CE=0;   
//  	NRFWriteTxDate(W_REGISTER+TX_ADDR,RxAddr0,TX_ADDR_WITDH);//写寄存器指令+P0地址使能指令+发送地址+地址宽度
////  	NRFWriteTxDate(W_REGISTER+TX_ADDR,RxAddr1,TX_ADDR_WITDH);//写寄存器指令+P0地址使能指令+发送地址+地址宽度
////  	NRFWriteTxDate(W_REGISTER+TX_ADDR,RxAddr2,1);//写寄存器指令+P0地址使能指令+发送地址+地址宽度
//
//										  // TX_ADDR_WITDH
//	NRFWriteTxDate(W_TX_PAYLOAD,TxDate,TX_DATA_WITDH);//写入数据 
//	//******下面有关寄存器配置**************
//  	NRFWriteReg(W_REGISTER+EN_AA,0x3f);       // 使能接收通道0自动应答0x01	0x3f	  
//  	NRFWriteReg(W_REGISTER+EN_RXADDR,0x3f);   // 使能接收通道0	0x01  0x3f
//				  
//  	NRFWriteReg(W_REGISTER+SETUP_RETR,0x0a);  // 自动重发延时等待250us+86us，自动重发10次
//  	NRFWriteReg(W_REGISTER+RF_CH,0x40);         // 选择射频通道0x40
//  	NRFWriteReg(W_REGISTER+RF_SETUP,0x07);    // 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益
//  	NRFWriteReg(W_REGISTER+CONFIG,0x0e);      // CRC使能，16位CRC校验，上电	
//	GPIO_SetBits(GPIOC, GPIO_Pin_8); 
////	CE=1;	
//	NRFDelay(5);//保持10us秒以上
//
//}  	
//void NRFSetTxMode1(uchar *TxDate)
//{  //发送模式
//	GPIO_ResetBits(GPIOC, GPIO_Pin_8); 
////    CE=0;   
//  	NRFWriteTxDate1(W_REGISTER+TX_ADDR,RxAddr0,TX_ADDR_WITDH);//写寄存器指令+P0地址使能指令+发送地址+地址宽度
////  	NRFWriteTxDate(W_REGISTER+TX_ADDR,RxAddr1,TX_ADDR_WITDH);//写寄存器指令+P0地址使能指令+发送地址+地址宽度
////  	NRFWriteTxDate(W_REGISTER+TX_ADDR,RxAddr2,1);//写寄存器指令+P0地址使能指令+发送地址+地址宽度
//
//										  // TX_ADDR_WITDH
//	NRFWriteTxDate1(W_TX_PAYLOAD,TxDate,TX_DATA_WITDH);//写入数据 
//	//******下面有关寄存器配置**************
//  	NRFWriteReg1(W_REGISTER+EN_AA,0x3f);       // 使能接收通道0自动应答0x01	0x3f	  
//  	NRFWriteReg1(W_REGISTER+EN_RXADDR,0x3f);   // 使能接收通道0	0x01  0x3f
//				  
//  	NRFWriteReg1(W_REGISTER+SETUP_RETR,0x0a);  // 自动重发延时等待250us+86us，自动重发10次
//  	NRFWriteReg1(W_REGISTER+RF_CH,0x40);         // 选择射频通道0x40
//  	NRFWriteReg1(W_REGISTER+RF_SETUP,0x07);    // 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益
//  	NRFWriteReg1(W_REGISTER+CONFIG,0x0e);      // CRC使能，16位CRC校验，上电	
//	GPIO_SetBits(GPIOC, GPIO_Pin_8); 
////	CE=1;	
//	NRFDelay(5);//保持10us秒以上
//
//}  	    
//void NRFSetTxMode1(uchar *TxDate)
//{  //发送模式
//	GPIO_ResetBits(GPIOC, GPIO_Pin_8);  
//   	NRFWriteTxDate(W_REGISTER+TX_ADDR,TxAddr1,TX_ADDR_WITDH);//写寄存器指令+接收地址使能指令+接收地址+地址宽度
//	NRFWriteTxDate(W_REGISTER+RX_ADDR_P1,TxAddr1,TX_ADDR_WITDH);//为了应答接收设备，接收通道0地址和发送地址相同
//	NRFWriteTxDate(W_TX_PAYLOAD,TxDate,TX_DATA_WITDH);//写入数据 
//	/******下面有关寄存器配置**************/
//  	NRFWriteReg(W_REGISTER+EN_AA,0x3f);       // 使能接收通道0自动应答
//  	NRFWriteReg(W_REGISTER+EN_RXADDR,0x3f);   // 使能接收通道0
//  	NRFWriteReg(W_REGISTER+SETUP_RETR,0x0a);  // 自动重发延时等待250us+86us，自动重发10次
//  	NRFWriteReg(W_REGISTER+RF_CH,0x40);         // 选择射频通道0x40
//  	NRFWriteReg(W_REGISTER+RF_SETUP,0x07);    // 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益
//	NRFWriteReg(W_REGISTER+CONFIG,0x0e);      // CRC使能，16位CRC校验，上电  
//	//CE=1;
//	GPIO_SetBits(GPIOC, GPIO_Pin_8); 
//	NRFDelay(5);//保持10us秒以上
//}  
//void NRFSetTxMode2(uchar *TxDate)
//{  
//	GPIO_ResetBits(GPIOC, GPIO_Pin_8);  
//   	NRFWriteTxDate(W_REGISTER+TX_ADDR,TxAddr2,TX_ADDR_WITDH);//写寄存器指令+接收地址使能指令+接收地址+地址宽度
//	NRFWriteTxDate(W_REGISTER+RX_ADDR_P2,TxAddr2,TX_ADDR_WITDH);//为了应答接收设备，接收通道0地址和发送地址相同
//	NRFWriteTxDate(W_TX_PAYLOAD,TxDate,TX_DATA_WITDH);//写入数据 
//	/******下面有关寄存器配置**************/
//  	NRFWriteReg(W_REGISTER+EN_AA,0x3f);       // 使能接收通道0自动应答
//  	NRFWriteReg(W_REGISTER+EN_RXADDR,0x3f);   // 使能接收通道0
//  	NRFWriteReg(W_REGISTER+SETUP_RETR,0x0a);  // 自动重发延时等待250us+86us，自动重发10次
//  	NRFWriteReg(W_REGISTER+RF_CH,0x40);         // 选择射频通道0x40
//  	NRFWriteReg(W_REGISTER+RF_SETUP,0x07);    // 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益
//	NRFWriteReg(W_REGISTER+CONFIG,0x0e);      // CRC使能，16位CRC校验，上电  
//	//CE=1;
//	GPIO_SetBits(GPIOC, GPIO_Pin_8); 
//	NRFDelay(5);//保持10us秒以上
//}  
/*****************NRF设置为接收模式并接收数据******************************/
//接收模式
void NRFSetRXMode()
{
    GPIO_ResetBits(GPIOC, GPIO_Pin_8);
//    CE=0;
  	NRFWriteTxDate(W_REGISTER+RX_ADDR_P0,RxAddr0,TX_ADDR_WITDH);  // 接收设备接收通道0使用和发送设备相同的发送地址
//	NRFWriteTxDate(W_REGISTER+RX_ADDR_P1,RxAddr1,TX_ADDR_WITDH);  // 接收设备接收通道1使用和发送设备相同的发送地址
//	NRFWriteTxDate(W_REGISTER+RX_ADDR_P2,RxAddr2,1);			  //注意！！！	TX_ADDR_WITDH	
  	
	NRFWriteReg(W_REGISTER+EN_AA,0x3f);               // 使能数据通道0和1自动应答	
  	NRFWriteReg(W_REGISTER+EN_RXADDR,0x3f);           // 使能接收通道0和1
             
  	NRFWriteReg(W_REGISTER+RX_PW_P0,TX_DATA_WITDH);  // 接收通道0选择和发送通道相同有效数据宽度
//	NRFWriteReg(W_REGISTER+RX_PW_P1,TX_DATA_WITDH);  // 接收通道1选择和发送通道相同有效数据宽度
// 	NRFWriteReg(W_REGISTER+RX_PW_P2,TX_DATA_WITDH);	  // 接收通道2选择和发送通道相同有效数据宽度

	NRFWriteReg(W_REGISTER+RF_CH,0x40);// 选择射频通道0x40 
	NRFWriteReg(W_REGISTER+RF_SETUP,0x07);            // 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益
	NRFWriteReg(W_REGISTER+CONFIG,0x0f);             // CRC使能，16位CRC校验，上电，接收模式 
    GPIO_SetBits(GPIOC, GPIO_Pin_8);    
//  	CE = 1; 
	NRFDelay(5);    
}	 
void NRFSetRXMode1()
{
    GPIO_ResetBits(GPIOC, GPIO_Pin_1);
//    CE=0;
  	NRFWriteTxDate1(W_REGISTER+RX_ADDR_P0,RxAddr0,TX_ADDR_WITDH);  // 接收设备接收通道0使用和发送设备相同的发送地址
//	NRFWriteTxDate(W_REGISTER+RX_ADDR_P1,RxAddr1,TX_ADDR_WITDH);  // 接收设备接收通道1使用和发送设备相同的发送地址
//	NRFWriteTxDate(W_REGISTER+RX_ADDR_P2,RxAddr2,1);			  //注意！！！	TX_ADDR_WITDH	
  	
	NRFWriteReg1(W_REGISTER+EN_AA,0x3f);               // 使能数据通道0和1自动应答	
  	NRFWriteReg1(W_REGISTER+EN_RXADDR,0x3f);           // 使能接收通道0和1
             
  	NRFWriteReg1(W_REGISTER+RX_PW_P0,TX_DATA_WITDH);  // 接收通道0选择和发送通道相同有效数据宽度
//	NRFWriteReg(W_REGISTER+RX_PW_P1,TX_DATA_WITDH);  // 接收通道1选择和发送通道相同有效数据宽度
// 	NRFWriteReg(W_REGISTER+RX_PW_P2,TX_DATA_WITDH);	  // 接收通道2选择和发送通道相同有效数据宽度

	NRFWriteReg1(W_REGISTER+RF_CH,0x40);// 选择射频通道0x40 
	NRFWriteReg1(W_REGISTER+RF_SETUP,0x07);            // 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益
	NRFWriteReg1(W_REGISTER+CONFIG,0x0f);             // CRC使能，16位CRC校验，上电，接收模式 
    GPIO_SetBits(GPIOC, GPIO_Pin_1);    
//  	CE = 1; 
	NRFDelay(5);    
}	   
/****************************检测是否有接收到数据******************************/
/*void CheckACK()
{  //用于发射模式接收应答信号
	sta=NRFReadReg(R_REGISTER+STATUS);                    // 返回状态寄存器
	if(TX_DS)
	   NRFWriteReg(W_REGISTER+STATUS,0xff);  // 清除TX_DS或MAX_RT中断标志
}*/
/****************************检测应答信号******************************/
/*uchar CheckACK()
{  //用于发射
	sta=NRFReadReg(R_REGISTER+STATUS);                    // 返回状态寄存器
	if(TX_DS||MAX_RT) //发送完毕中断
	{  GPIO_SetBits(GPIOC,GPIO_Pin_0);NRFDelay(1000);
	   NRFWriteReg(W_REGISTER+STATUS,0xff);  // 清除TX_DS或MAX_RT中断标志
	  GPIO_ResetBits(GPIOC, GPIO_Pin_9); 
//	   CSN=0;
	   NRFSPI(FLUSH_TX);//用于清空FIFO ！！关键！！不然会出现意想不到的后果！！！大家记住！！
	   GPIO_SetBits(GPIOC, GPIO_Pin_9);   
//       CSN=1; 
	   return(0);
	}
	else
	   return(1);
}	 */
void NRFSetTxMode(uchar *TxDate)
{//发送模式
  //  CE=0; 
  GPIO_ResetBits(GPIOC, GPIO_Pin_8);
   	NRFWriteTxDate(W_REGISTER+TX_ADDR,TxAddr,TX_ADDR_WITDH);//写寄存器指令+接收地址使能指令+接收地址+地址宽度
	NRFWriteTxDate(W_REGISTER+RX_ADDR_P0,TxAddr,TX_ADDR_WITDH);//为了应答接收设备，接收通道0地址和发送地址相同
	NRFWriteTxDate(W_TX_PAYLOAD,TxDate,TX_DATA_WITDH);//写入数据 
	/******下面有关寄存器配置**************/
  	NRFWriteReg(W_REGISTER+EN_AA,0x3f);       // 使能接收通道0自动应答
  	NRFWriteReg(W_REGISTER+EN_RXADDR,0x3f);   // 使能接收通道0
  	NRFWriteReg(W_REGISTER+SETUP_RETR,0x0a);  // 自动重发延时等待250us+86us，自动重发10次
  	NRFWriteReg(W_REGISTER+RF_CH,0x40);         // 选择射频通道0x40
  	NRFWriteReg(W_REGISTER+RF_SETUP,0x07);    // 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益
	NRFWriteReg(W_REGISTER+CONFIG,0x0e);      // CRC使能，16位CRC校验，上电  
	//CE=1;
	GPIO_SetBits(GPIOC, GPIO_Pin_8);
	NRFDelay(5);//保持10us秒以上
}
void NRFSetTxMode1(uchar *TxDate)
{//发送模式
  //  CE=0; 
  GPIO_ResetBits(GPIOC, GPIO_Pin_1);
   	NRFWriteTxDate1(W_REGISTER+TX_ADDR,TxAddr,TX_ADDR_WITDH);//写寄存器指令+接收地址使能指令+接收地址+地址宽度
	NRFWriteTxDate1(W_REGISTER+RX_ADDR_P0,TxAddr,TX_ADDR_WITDH);//为了应答接收设备，接收通道0地址和发送地址相同
	NRFWriteTxDate1(W_TX_PAYLOAD,TxDate,TX_DATA_WITDH);//写入数据 
	/******下面有关寄存器配置**************/
  	NRFWriteReg1(W_REGISTER+EN_AA,0x3f);       // 使能接收通道0自动应答
  	NRFWriteReg1(W_REGISTER+EN_RXADDR,0x3f);   // 使能接收通道0
  	NRFWriteReg1(W_REGISTER+SETUP_RETR,0x0a);  // 自动重发延时等待250us+86us，自动重发10次
  	NRFWriteReg1(W_REGISTER+RF_CH,0x40);         // 选择射频通道0x40
  	NRFWriteReg1(W_REGISTER+RF_SETUP,0x07);    // 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益
	NRFWriteReg1(W_REGISTER+CONFIG,0x0e);      // CRC使能，16位CRC校验，上电  
	//CE=1;
	GPIO_SetBits(GPIOC, GPIO_Pin_1);
	NRFDelay(5);//保持10us秒以上
}
/****************************检测应答信号******************************/
uchar CheckACK()
{  //用于发射
	sta=NRFReadReg(R_REGISTER+STATUS);                    // 返回状态寄存器
	if(TX_DS||MAX_RT) //发送完毕中断
	{
	   NRFWriteReg(W_REGISTER+STATUS,0xff);  // 清除TX_DS或MAX_RT中断标志
	    GPIO_ResetBits(GPIOC, GPIO_Pin_9); 
	//   CSN=0;
	   NRFSPI(FLUSH_TX);//用于清空FIFO ！！关键！！不然会出现意想不到的后果！！！大家记住！！  
 GPIO_SetBits(GPIOC, GPIO_Pin_9); 
      // CSN=1; 
	   return(0);
	}
	else
	   return(1);
}
uchar CheckACK1()
{  //用于发射
	sta=NRFReadReg1(R_REGISTER+STATUS);                    // 返回状态寄存器
	if(TX_DS||MAX_RT) //发送完毕中断
	{
	   NRFWriteReg1(W_REGISTER+STATUS,0xff);  // 清除TX_DS或MAX_RT中断标志
	    GPIO_ResetBits(GPIOC, GPIO_Pin_2); 
	//   CSN=0;
	   NRFSPI1(FLUSH_TX);//用于清空FIFO ！！关键！！不然会出现意想不到的后果！！！大家记住！！  
 GPIO_SetBits(GPIOC, GPIO_Pin_2); 
      // CSN=1; 
	   return(0);
	}
	else
	   return(1);
}
/*************************接收数据*********************************************/
void GetDate()				 
{
    uchar RX_P_NO;//接收通道号
	sta=NRFReadReg(R_REGISTER+STATUS);//发送数据后读取状态寄存器	  
    if(RX_DR)				 // 判断是否接收到数据
     {	 
	   RX_P_NO=sta&0x0e;    //获取通道号				0000 1110
    	GPIO_ResetBits(GPIOC, GPIO_Pin_8);	
				                     
//	   CE=0;                //待机	  
		
	   switch(RX_P_NO)
	    {																		  // 
		   case 0x00:NRFReadRxDate(R_RX_PAYLOAD,RevTempDate0,RX_DATA_WITDH);break;// 从RXFIFO读取数据通道0
		   case 0x02:NRFReadRxDate(R_RX_PAYLOAD,RevTempDate0,RX_DATA_WITDH);break;// 从RXFIFO读取数据通道1
		   case 0x04:NRFReadRxDate(R_RX_PAYLOAD,RevTempDate0,RX_DATA_WITDH); break;// 从RXFIFO读取数据通道2 
		   default:break;		  						//
		}	 					  					          
	   NRFWriteReg(W_REGISTER+STATUS,0xff); //接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清楚中断标
    	GPIO_ResetBits(GPIOC, GPIO_Pin_9);	   
//	   CSN=0;				
	   NRFSPI(FLUSH_RX);//用于清空FIFO ！！关键！！不然会出现意想不到的后果！！！大家记住！！ 
	   GPIO_SetBits(GPIOC, GPIO_Pin_9);
//	   CSN=1;		 	 	
     }						                                                
       //NRFWriteReg(W_REGISTER+STATUS,0xff); //接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清楚中断标	  
} 
void GetDate1()				 
{
    uchar RX_P_NO;//接收通道号
	sta=NRFReadReg1(R_REGISTER+STATUS);//发送数据后读取状态寄存器	  
    if(RX_DR)				 // 判断是否接收到数据
     {	 
	   RX_P_NO=sta&0x0e;    //获取通道号				0000 1110
    	GPIO_ResetBits(GPIOC, GPIO_Pin_1);	
				                     
//	   CE=0;                //待机	  
		
	   switch(RX_P_NO)
	    {																		  // 
		   case 0x00:NRFReadRxDate1(R_RX_PAYLOAD,RevTempDate0,RX_DATA_WITDH);break;// 从RXFIFO读取数据通道0
		   case 0x02:NRFReadRxDate1(R_RX_PAYLOAD,RevTempDate0,RX_DATA_WITDH);break;// 从RXFIFO读取数据通道1
		   case 0x04:NRFReadRxDate1(R_RX_PAYLOAD,RevTempDate0,RX_DATA_WITDH); break;// 从RXFIFO读取数据通道2 
		   default:break;		  						//
		}	 					  					          
	   NRFWriteReg1(W_REGISTER+STATUS,0xff); //接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清楚中断标
    	GPIO_ResetBits(GPIOC, GPIO_Pin_2);	   
//	   CSN=0;				
	   NRFSPI1(FLUSH_RX);//用于清空FIFO ！！关键！！不然会出现意想不到的后果！！！大家记住！！ 
	   GPIO_SetBits(GPIOC, GPIO_Pin_2);
//	   CSN=1;		 	 	
     }						                                                
       //NRFWriteReg(W_REGISTER+STATUS,0xff); //接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清楚中断标	  
} 
