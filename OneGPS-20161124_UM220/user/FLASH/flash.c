#include "flash.h" 
#include "spi.h"
#include "led.h"   
#include "stm32f10x.h"

//SPI CS PA4
#ifndef    MAIN_SPI_FLASH
#define MAIN_SPI_FLASH  		  1
		  
//SPI CS PB0
#define SUBORDINATION_SPI_FLASH   2
#endif


#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //��� 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����

#define	SPI_FLASH_CS PAout(4)  //ѡ��FLASH	

#define	SUBORDINATION_SPI_FLASH_CS PBout(0)  //ѡ��FLASH SUBORDINATION	

volatile u8 x25ReadFlag=0;
volatile u8 x25WriteFlag=0;

//KbytesΪһ��Sector
//16������Ϊ1��Block
//w25x32
//����Ϊ4M�ֽ�,����64��Block,512��Sector 

//��ʼ��SPI FLASH��IO��
void SPI_Flash_Init(FlashInfo_t *pInf)
{  		
	GPIO_InitTypeDef GPIO_InitStructure;
	if (MAIN_SPI_FLASH == pInf->channel)
	{
		RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );	

		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  //SPI CS
	 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�����������
	 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	 	SPI_FLASH_CS=1; 
	//	GPIO_ResetBits(GPIOA,GPIO_Pin_4);

	}
	else if (SUBORDINATION_SPI_FLASH == pInf->channel)
	{
		RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  //SPI CS
	 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�����������
	 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	 	SUBORDINATION_SPI_FLASH_CS = 1;
//		SUBORDINATION_SPI_FLASH_CS = 0;

//		GPIO_SetBits(GPIOB,GPIO_Pin_0);

//		GPIO_ResetBits(GPIOB,GPIO_Pin_0);
    }
	else
	  return;
	SPIx_Init();		   //��ʼ��SPI
}  

//��ȡSPI_FLASH��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
u8 SPI_Flash_ReadSR(FlashInfo_t *pInf)   
{  
	u8 byte=0;   
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=0;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 0;
	else return 0;



	                          //ʹ������   
	SPIx_ReadWriteByte(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPIx_ReadWriteByte(0Xff);             //��ȡһ���ֽ� 
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=1;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 1; 
//	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     
	return byte;   
} 
//дSPI_FLASH״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void SPI_FLASH_Write_SR(FlashInfo_t *pInf,u8 sr)   
{   
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=0;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 0;
	else return;                           //ʹ������   
	SPIx_ReadWriteByte(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������    
	SPIx_ReadWriteByte(sr);               //д��һ���ֽ�  
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=1;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 1;
}   
//SPI_FLASHдʹ��	
//��WEL��λ   
void SPI_FLASH_Write_Enable(FlashInfo_t *pInf)   
{
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=0;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 0;
	else return;
    SPIx_ReadWriteByte(W25X_WriteEnable);      //����дʹ��  
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=1;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 1;        	      
} 
//SPI_FLASHд��ֹ	
//��WEL����  
void SPI_FLASH_Write_Disable(FlashInfo_t *pInf)   
{  
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=0;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 0;
	else return;  
    SPIx_ReadWriteByte(W25X_WriteDisable);     //����д��ָֹ��    
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=1;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 1;      	      
} 			    
//��ȡоƬID W25X16��ID:0XEF14
u16 SPI_Flash_ReadID(FlashInfo_t *pInf)
{
	u16 Temp = 0;	  
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=0;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 0;
	else return 0; 			    
	SPIx_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x00); 	 			   
	Temp|=SPIx_ReadWriteByte(0xFF)<<8;  
	Temp|=SPIx_ReadWriteByte(0xFF);	 
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=1;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 1; 			    
	return Temp;
}   		    
//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void SPI_Flash_Read(FlashInfo_t *pInf,u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;    	
//	if (0x5a == x25ReadFlag)
//		return;
//	x25ReadFlag = 0x5a;
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=0;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 0;
	else return;  
    SPIx_ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����   
    SPIx_ReadWriteByte((u8)((ReadAddr)>>16));  //����24bit��ַ    
    SPIx_ReadWriteByte((u8)((ReadAddr)>>8));   
    SPIx_ReadWriteByte((u8)ReadAddr);   
#if 1	
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPIx_ReadWriteByte(0XFF);   //ѭ������  
    }
#else
    SPIx_ReadByteDMA(pBuffer,NumByteToRead);
#endif
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=1;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 1;   
//	x25ReadFlag = 0;
}  
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 

void SPI_Flash_Write_Page(FlashInfo_t *pInf,u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
//	u8 spibuf[4];
 
    SPI_FLASH_Write_Enable(pInf);                  //SET WEL 


	if(NumByteToWrite > 256 )
		NumByteToWrite = 256;


	//for (i=0; i<NumByteToWrite; i++)
	//	spibuf[4+i] = *pBuffer++;
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=0;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 0;
	else return;  
#if 1
    SPIx_ReadWriteByte(W25X_PageProgram);      //����дҳ����   
    SPIx_ReadWriteByte((u8)((WriteAddr)>>16)); //����24bit��ַ    
    SPIx_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPIx_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++) SPIx_ReadWriteByte(pBuffer[i]);//ѭ��д��  
    
	//for(i=0;i<260;i++)SPIx_ReadWriteByte(spibuf[i]);//ѭ��д��  
#else
	spibuf[0] = W25X_PageProgram;
	spibuf[1] = (u8)((WriteAddr)>>16);
	spibuf[2] = (u8)((WriteAddr)>>8);
	spibuf[3] = (u8)((WriteAddr));
    SPIx_WriteByteDMA(spibuf,4);
    SPIx_WriteByteDMA(pBuffer,NumByteToWrite);
#endif
    //SPIx_WriteByteINTRRUPT(spibuf,NumByteToWrite+4);
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=1;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 1; 
	SPI_Flash_Wait_Busy(pInf);					   //�ȴ�д�����
} 
//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void SPI_Flash_Write_NoCheck(FlashInfo_t *pInf,u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		SPI_Flash_Write_Page(pInf,pBuffer,WriteAddr,pageremain);
		//OSTimeDly(1);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	}	    
} 
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)  		   
//u8 SPI_FLASH_BUF[4096];
void SPI_Flash_Write(FlashInfo_t *pInf,u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
// 	register u16 i;    
//	if (0x5a == x25WriteFlag)
//		return ;
//	x25WriteFlag = 0x5a;
	

//	for(i=0;i<5000;i++)
//	{
//		i ++;
//		i --;
//	}
//	delayms(1);
	secpos = WriteAddr/4096;//������ַ 0~511 for w25x32
	secoff = WriteAddr%4096;//�������ڵ�ƫ��
	secremain = 4096-secoff;//����ʣ��ռ��С   

	if(NumByteToWrite <= secremain) 
		secremain = NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
//		SPI_Flash_Read (pInf,SPI_FLASH_BUF,secpos*4096,4096);//������������������
//		for(i=0; i<secremain; i++)//У������
//		{
//			if(SPI_FLASH_BUF[secoff+i] != 0XFF) 
//				break;//��Ҫ����  	  
//		}
//		if(i<secremain)//��Ҫ����
//		{
//			SPI_Flash_Erase_Sector (pInf,secpos*4096);//�����������
//			for( i=0; i<secremain; i++)	   //����
//			{
//				SPI_FLASH_BUF[i+secoff] = pBuffer[i];	  
//			}
//			SPI_Flash_Write_NoCheck(pInf,SPI_FLASH_BUF,secpos*4096,4096);//д����������  
//
//		}
//		else
			SPI_Flash_Write_NoCheck(pInf,pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite == secremain)
			break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};	 
	SPI_Flash_Wait_Busy(pInf);
//	x25WriteFlag = 0;
}
//��������оƬ
//��Ƭ����ʱ��:
//W25X16:25s 
//w25x32:40s 
//W25X64:40s 
//�ȴ�ʱ�䳬��...
void SPI_Flash_Erase_Chip(FlashInfo_t *pInf)   
{                                             
    SPI_FLASH_Write_Enable(pInf);                  //SET WEL 
    SPI_Flash_Wait_Busy(pInf);   
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=0;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 0;
	else return; 
    SPIx_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������  
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=1;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 1;    	      
	SPI_Flash_Wait_Busy(pInf);   				   //�ȴ�оƬ��������
}   
//����һ������
//Dst_Addr:������ַ 0~511 for w25x16 20121126:�ĳɰ��վ��Ե�ַ�ٲ�
//����һ��ɽ��������ʱ��:150ms
void SPI_Flash_Erase_Sector(FlashInfo_t *pInf,u32 Dst_Addr)   
{   
//	Dst_Addr*=4096;
	  if (0 != Dst_Addr %4096)
	  {	
	  	Dst_Addr = Dst_Addr/4096;
	  	Dst_Addr *= 4096;
	  }	
    SPI_FLASH_Write_Enable(pInf);                  //SET WEL 	 
    SPI_Flash_Wait_Busy(pInf);   
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=0;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 0;
	else return;  
    SPIx_ReadWriteByte(W25X_SectorErase);      //������������ָ�� 
    SPIx_ReadWriteByte((u8)((Dst_Addr)>>16));  //����24bit��ַ    
    SPIx_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPIx_ReadWriteByte((u8)Dst_Addr);  
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=1;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 1;     	      
    SPI_Flash_Wait_Busy(pInf);   				   //�ȴ��������
}  
//�ȴ�����
void SPI_Flash_Wait_Busy(FlashInfo_t *pInf)   
{   
	while ((SPI_Flash_ReadSR(pInf)&0x01)==0x01) ;  // �ȴ�BUSYλ���
		//OSTimeDly(1);
}  
//�������ģʽ
void SPI_Flash_PowerDown(FlashInfo_t *pInf)   
{ 
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=0;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 0;
	else return;  
    SPIx_ReadWriteByte(W25X_PowerDown);        //���͵�������  
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=1;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 1; 	      
    OSTimeDly(1);                               //�ȴ�TPD  
}   
//����
void SPI_Flash_WAKEUP(FlashInfo_t *pInf)   
{  
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=0;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 0;
	else return;  
    SPIx_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	if (MAIN_SPI_FLASH == pInf->channel)
		SPI_FLASH_CS=1;  
	else if(SUBORDINATION_SPI_FLASH == pInf->channel)
		SUBORDINATION_SPI_FLASH_CS = 1; 	      
    OSTimeDly(1);                              //�ȴ�TRES1
}   



















