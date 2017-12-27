#ifndef _LED_H
#define _LED_H
#include "stm32f0xx.h" //wei20160601


//PA0 GSM POWER 
#define  GSM_POWERON_PIN  GPIO_Pin_0
#define GSM_POWERON_PORT  GPIOA

//PA1 GPS POWER
#define  GPS_POWERON_PIN  GPIO_Pin_1
#define GPS_POWERON_PORT  GPIOA
//PA11 SHUT DOWN OIL
#define  OIL_BREAK_PIN  GPIO_Pin_11
#define OIL_BREAK_PORT  GPIOA
//PA12 ACC CHECK
#define  ACC_PIN  GPIO_Pin_12
#define ACC_PORT  GPIOA
//PA8  ARALM
#define  ALARM_PIN  GPIO_Pin_8
#define ALARM_PORT  GPIOA

// PF6  SCL LIS3DH
//PF7 SDA 

//PB5  LIS3DH INT1
#define  SHAKE_PIN  GPIO_Pin_5
#define  SHAKE_PORT  GPIOB
//PB6 LIS3DH INT2



//gsm uart1 PA9 PA10

//PB14 GSM POWER KEY
#define  GSM_POWERKEY_PIN  GPIO_Pin_14
#define GSM_POWERKEY_PORT  GPIOB
//PB15 GSM RING
#define GSM_RING_PIN GPIO_Pin_15
#define GSM_RING_PORT  GPIOB


//GPS UART2 PA2 PA3

//���߽������ߣ����ͻ���
#define DTR_PIN     GPIO_Pin_11
#define DTR_PORT   GPIOB

#define LEDn                   3
//pA8  LED1    SYSTEM POWER LED
#define LED1_PIN					GPIO_Pin_4	 
#define LED1_GPIO_PORT		GPIOA
#define LED1_GPIO_CLK			RCC_AHBPeriph_GPIOA
//PB12  LED2  GPS POWER LED
#define LED2_PIN					GPIO_Pin_12	
#define LED2_GPIO_PORT		GPIOB
#define LED2_GPIO_CLK			RCC_AHBPeriph_GPIOB
//PB13  LED3  GSM POWER LED
#define LED3_PIN					GPIO_Pin_13	
#define LED3_GPIO_PORT		GPIOB
#define LED3_GPIO_CLK			RCC_AHBPeriph_GPIOB

extern GPIO_TypeDef* GPIO_PORT[LEDn] ;
extern const uint16_t GPIO_PIN[LEDn] ;
extern const uint32_t GPIO_CLK[LEDn] ;





typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}
bool;

//led work struct
typedef struct _LED
{
	u16 freq;
	u16 counter;
	bool power;
	bool flash;
}LED_STRUCT;

extern LED_STRUCT LED_yellow;
extern LED_STRUCT LED_red;

/**********************************************************************
//˵��: �������е�IO��
//����1:��
//����:��
**********************************************************************/
void IO_Init(void);
/******wei,20160531****************************************************************
//˵��: ����DTR�������뻹�����
//����1:��
//����:��
**********************************************************************/
//wei,20160803for space,ֻ�г��͹���ģʽ��������ҪGMSģ�������ˣ�����ֱ�Ӷϵ� extern void DTR_PIN_MODE_Contrl(FunctionalState New_state);

extern void RING_EXTI0_Config(void);
/**********************************************************************
//˵��: ����GSM��Դ
//����1: TRUE ���ϵ磻FALSE:���ϵ�
//����:��
**********************************************************************/
//wei20160531 void GSM_Power(bool status);
extern u8 GSM_Power(bool status); //wei20160531
/**********************************************************************
//˵��: ����Զ�̹���
//����1: TRUE ���ϵ磻FALSE:���ϵ�
//����:��
**********************************************************************/
void set_REMOTE_CLOSE(u32 time);
/**********************************************************************
//˵��: �����������ͷ��Դ
//����1: TRUE ���ϵ磻FALSE:���ϵ�
//����:��
**********************************************************************/
void set_CAMERA_POWER(bool status);

/**********************************************************************
//˵��: ���ö����õĵ�Դ
//����1: TRUE ���ϵ磻FALSE:���ϵ�
//����:��
**********************************************************************/
void set_OIL_BREAK_POWER(bool status);
/**********************************************************************
//˵��: ����Զ�̿��ŵĵ�Դ
//����1: TRUE ���ϵ磻FALSE:���ϵ�
//����:��
**********************************************************************/
void set_REMOTE_OPEN(u32 time);
/**********************************************************************
//˵��: ����GSM�Ĺ����Դ
//����1: TRUE ���ϵ磻FALSE:���ϵ�
//����:��
**********************************************************************/
void set_GSM_POWERON(bool status);

/**********************************************************************
//˵��: �����ֱ���GPS�Ĺ����Դ
//����1: TRUE ���ϵ磻FALSE:���ϵ�
//����:��
**********************************************************************/
void set_HANDLE_GPS_POWER(bool status);
/**********************************************************************
//˵��: ���ö���
//����1: TRUE �����ͣ�FALSE:������
//����:��
**********************************************************************/
void set_OIL_BREAK(bool status);
/**********************************************************************
//˵��: ����GSM��power key
//����1: TRUE ���ϵ磻FALSE:���ϵ�
//����:��
**********************************************************************/
void set_GSM_POWERKEY(bool status);
/**********************************************************************
//˵��: �����ֱ����Ⱦ���
//����1: TRUE �����þ�����FALSE:������
//����:��
**********************************************************************/
void set_HANDLE_SPEAKER_MUTE(bool status);
/**********************************************************************
//˵��: ����������·��ĵ�Դ
//����1: TRUE ���ϵ磻FALSE:���ϵ�
//����:��
**********************************************************************/
void set_MAINPOWER(bool status);
/**********************************************************************
//˵��: �����������
//����1: TRUE �������FALSE:�������
//����:��
**********************************************************************/
void set_SPEAKEROUT(bool status);
/**********************************************************************
//˵��: ����������Ⱦ���
//����: ��
//����: TRUE ��������FALSE:��������
**********************************************************************/
void  set_EXTERNAL_SPEAKER_MUTE(bool status);


/**********************************************************************
//˵��: ��ȡGSM ring �� ״̬
//����: ��
//����: TRUE ����Ч���е绰����ź��룻FALSE:���޵绰�����
**********************************************************************/
bool  get_GSM_RING_status(void);

/**********************************************************************
//˵��: ��ȡGPS����״̬
//����: ��
//����: //#define   GPS_ANTENNA_OK            0
//#define   GPS_ANTENNA_N0_CONNECT    1
//#define   GPS_ANTENNA_SHORT         2
//#define   GPS_ANTENNA_NO_USE        3
**********************************************************************/
u8  get_GPS_ANTENNA(void);

/**********************************************************************
//˵��: ��ȡ�ʹ�����2��״̬
//����: ��
//����: TRUE ����Ч��FALSE:����Ч
**********************************************************************/
bool  get_LOWSENSOR2_status(void);
/**********************************************************************
//˵��: ��ȡ�񶯴�������״̬
//����: ��
//����: TRUE ����Ч��FALSE:����Ч
**********************************************************************/
bool  get_SHAKE_status(void);
/**********************************************************************
//˵��: ��ȡɲ����������״̬
//����: ��
//����: TRUE ����Ч��FALSE:����Ч
**********************************************************************/
bool  get_BRAKE_status(void);
/**********************************************************************
//˵��: ��ȡ�ű߼�⴫������״̬
//����: ��
//����: TRUE ����Ч��FALSE:����Ч
**********************************************************************/
bool  get_DOOR_status(void);

/**********************************************************************
//˵��: ��ȡACC������2��״̬
//����: ��
//����: TRUE ����Ч��FALSE:����Ч
**********************************************************************/
bool  get_ACC_CHECK_status(void);
/**********************************************************************
//˵��: ��ȡ����������״̬
//����: ��
//����: TRUE ����Ч��FALSE:����Ч
**********************************************************************/
bool  get_ALARM_status(void);

/**********************************************************************
//˵��: ��ȡ��ת��Ƶ�״̬
//����: ��
//����: TRUE ����Ч��FALSE:����Ч
**********************************************************************/
bool  get_LIGHT_TURNLEFT_status(void);
/**********************************************************************
//˵��: ��ȡ��ת��Ƶ�״̬
//����: ��
//����: TRUE ����Ч��FALSE:����Ч
**********************************************************************/
bool  get_LIGHT_TURNRIGHT_status(void);

/**********************************************************************
//˵��: ��ȷ��ʱָ��ʱ�䣬��λms
//����: ��ʱ��ʱ�䳣��
//����: ��
**********************************************************************/
void delayms(u32 num);

void LED_Key_Init(void);
void LED_On(uint8_t x);
void LED_Off(uint8_t x);
/**********************************************************************
//˵��: ����LED
//����1:LED���()
//����2:�Ƿ���Ҫ��˸(0:�أ�1:��)
//����3:��˸Ƶ��(1~1400hz)
//����4:����(0:�أ�1:��)
//����:�Ƿ�ɹ�(0:ʧ�ܣ�1:�ɹ�)
**********************************************************************/
bool setled(LED_STRUCT* led,bool flash,u16 freq,bool power);
/**********************************************************************
//˵��: GPS ��Դ����
//����: �����߹�
//����: ��
**********************************************************************/
void set_GPS_POWERON(bool status);
/**********************************************************************
//˵��: GSM �ϵ�
//����: ��
//����: ��
**********************************************************************/
void GSM_reset(void);
/******wei20160601****************************************************************
//˵��: ����15��AT���ж�MCU��û�к�GSMģ��ͨ�ųɹ���
//����: ��
//����: ����0���ɹ���  ����0��ʧ��
**********************************************************************/
extern u8 Check_AT(void);//wei20160601
/**********************************************************************
//˵��: GSM ����
//����: TRUE ���룬FALSE �Ƴ�
//����: ��
**********************************************************************/
void set_GSM_SLEEP(bool status);

bool  get_SHAKE_status(void);

#endif /*_LED_H*/