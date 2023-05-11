 /**
  ******************************************************************************
  * @file    bsp_cs5532_spi.c
  * @author  zp
  * @version V1.0
  * @date    2014-xx-xx
  * @brief   cs5532 �ײ�Ӧ�ú���bsp 
  ******************************************************************************
  * @attention
  *
  * ƽ̨: �������ֽ���
  * 
  *
  ******************************************************************************
  */
  
#include "bsp_cs5532_IO.h"

/* Private typedef -----------------------------------------------------------*/

#define Dummy_Byte                0xFF

unsigned char  CS5532_State;
extern  u16  B10msTimer,B1msTimer;

void CS5532_Polling(void)
{
	

}	
 




/**
  * @brief  CS5532 �˿�����
  * @param  ��  
  * @retval ��
  */
void CS5532_GPIO_Init(void)
{
	
    GPIO_InitTypeDef GPIO_InitStructure;              /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/ 

    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | CS5532_GPIO_CLK, ENABLE); /*����GPIOB������ʱ�ӣ�������JTAG��*/
    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);  //����JTAG������ʹ��SW
		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //�������   
    GPIO_InitStructure.GPIO_Pin = CS5532_CS_PIN | CS5532_SCLK_PIN | CS5532_SDI_PIN;
    GPIO_Init(CS5532_GPIO_PORT, &GPIO_InitStructure);	
	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     //��������
    GPIO_InitStructure.GPIO_Pin = CS5532_SDO_PIN;
    GPIO_Init(CS5532_SDO_GPIO_PORT, &GPIO_InitStructure);

    CS5532_CS_HIGH();    //�˿�ȫ���ø�
    CS5532_SCLK_HIGH();
    CS5532_SDI_HIGH();
    CS5532_SDO_HIGH();  	
}
/**
  * @brief  CS5532 �򵥵���ʱ����
  * @param  __IO uint32_t nCount
  * @retval ��
  */
static void CS5532_Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/**
  * @brief  CS5532 ���͵����ֽ�
  * @param  unsigned char byte 
  * @retval ��
  */
void CS5532_SendByte(unsigned char byte)
{
    unsigned char i;
    CS5532_SCLK_LOW();
    CS5532_CS_LOW();	
    for(i = 0;i < 10;i ++)  //��ʱ50ns
        __nop();
    for(i = 0;i < 8;i ++)
    {
        if((byte << i) & 0x80) 
            CS5532_SDI_HIGH();
        else             
            CS5532_SDI_LOW();
        __nop();__nop();       //��ʱ50ns 
        __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//��ʱ > 250ns 
         __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//��ʱ > 250ns 
        CS5532_SCLK_HIGH();  
        __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//��ʱ > 250ns 
        CS5532_SCLK_LOW();
    }
    CS5532_SDI_HIGH();
    //CS5532_CS_HIGH(); 
}
/**
  * @brief  CS5532 �������������������
  * @param  command 8bit��� reg�� 32bit������Ϣ 
  * @retval ��
  */
void CS5532_Send_REG(u8 command,u32 reg)
{
    unsigned char i,k;
    CS5532_SCLK_LOW(); 
    CS5532_CS_LOW();	
    for(k = 0;k < 3;k ++)  //��ʱ50ns
        __nop();
    for(i = 0;i < 8;i ++)      //�ȷ�������
    {
        if((command << i) & 0x80) 
            CS5532_SDI_HIGH();
        else             
            CS5532_SDI_LOW();
            __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//��ʱ > 250ns 
             __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//��ʱ > 250ns 
        CS5532_SCLK_HIGH();  
        __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//��ʱ > 250ns 
        CS5532_SCLK_LOW();
    }
    for(i = 0;i < 32;i ++)    //����32λ�Ĵ���
    {
        if((reg << i) & 0x80000000) 
            CS5532_SDI_HIGH();
        else             
            CS5532_SDI_LOW();
            __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//��ʱ > 250ns 
             __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//��ʱ > 250ns 
        CS5532_SCLK_HIGH();  
        __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//��ʱ > 250ns 
        CS5532_SCLK_LOW();
    }
    CS5532_SDI_HIGH();
    CS5532_CS_HIGH();
}

/**
  * @brief  CS5532 ����1�ֽ�����
  * @param  ��
  * @retval ������byt����
  */
u8 CS5532_ReadByte(void)
{
    unsigned char i,rec_data;
    rec_data = 0;
    CS5532_SCLK_LOW(); 
    __nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();
    for(i = 8;i > 0;i --)
    {
        rec_data = rec_data << 1;
        CS5532_SCLK_HIGH();
        __nop();__nop();__nop();__nop();
        if(CS5532_SDO_READ())
            rec_data ++;
        __nop();__nop();
        CS5532_SCLK_LOW();
        __nop();__nop();__nop();__nop();
    }
    return(rec_data);
}
/**
  * @brief  CS5532 ��ȡ���������µ�����
  * @param  command ����
  * @retval 32bit����
  */
u32 CS5532_Read_REG(u8 command)
{
    unsigned char i,k;
    unsigned long rec_data = 0;
    CS5532_SCLK_LOW(); 
    CS5532_CS_LOW();	
    for(k = 0;k < 3;k ++)  //��ʱ50ns
        __nop();
    for(i = 0;i < 8;i ++)      //�ȷ�������
    {
        if((command << i) & 0x80) 
            CS5532_SDI_HIGH();
        else             
            CS5532_SDI_LOW();
        CS5532_SCLK_HIGH();  
        __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
        CS5532_SCLK_LOW();
    }
    for(i = 0;i < 32;i ++)    //����32λ�Ĵ���
    {
        rec_data = rec_data << 1;
        CS5532_SCLK_HIGH();
        __nop();__nop();__nop();__nop();
//         if(CS5532_SDO_READ())
//             rec_data ++;
        rec_data += CS5532_SDO_READ();
        __nop();__nop();__nop();__nop();
        CS5532_SCLK_LOW();
        __nop();__nop();__nop();__nop();
         __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//��ʱ > 250ns 
    }
    CS5532_SDI_HIGH();
    CS5532_CS_HIGH();
    return rec_data;
}
/**
  * @brief  AD �����λ
  * @param  �� 
  * @retval ��
  * CS5532 û���ϵ縴λ���ܣ�������Ҫ�����λ
  * ����15��0xff���ټ�1��0xfe
  */
void CS5532_Soft_Reset(void)    
{
    char i;
    for(i = 0;i < 15;i ++)
        CS5532_SendByte(0xff);
    CS5532_SendByte(0xfe);        
}
/**
  * @brief  CS5532 ��ʼ��
  * @param  ��
  * @retval ��
  */
void CS5532_Init(void)
{
    u32 tmp;
    CS5532_GPIO_Init();       //�˿ڳ�ʼ��
    CS5532_CS_LOW();          //Ƭѡ 
    do
    {                  
	      CS5532_Soft_Reset();      //�����λ
        CS5532_Delay(0x1ffff);    //Լ25ms
        CS5532_Send_REG(CS5532_ConfigWrite,0x20000000);		     
        CS5532_Delay(0xff);
        tmp = CS5532_Read_REG(0x0b);   //�����üĴ���
        //break;
    } 
    while(tmp != 0x10000000);  //�����üĴ����е�RVû����λʱ����ʾϵͳû�и�λ	
	
    CS5532_Delay(0xf);
    CS5532_Send_REG(CS5532_ConfigWrite,CS5532_ConfigData); 	//���üĴ���,��ѹ��׼����Ϊ1V~2.5V�� RVS(bit25) = 1
    CS5532_Send_REG(CS5532_SetupCH1,CS5532_Channel1_Data); //����ͨ��1
    CS5532_CS_LOW();          //Ƭѡ     
    //CS5532_Delay(100);
    //CS5532_SendByte(0xc0);    //����ת������    
    //CS5532_SendByte(0x55);    //����ת������   
    CS5532_Read_ADData();
}
/**
  * @brief  CS5532 ������ȡ�ɼ�����
  * @param  ��
  * @retval 32bit�ɼ�����
  */
long CS5532_Read_ADData(void)
{
    unsigned char i;
    long rec_data = 0;

    CS5532_SCLK_LOW(); 
    CS5532_SDI_LOW();        //����SDIΪ�͵�ƽ
    CS5532_CS_LOW();	
    CS5532_Delay(100);
    CS5532_SendByte(0x80);
    CS5532_Delay(100);  
     
    while(CS5532_SDO_READ())    //�ȴ�SDO���ͣ����ת����׼��������
    {
    	
    	 
    	
    }	  
    __nop();__nop();__nop();	

	
    for(i = 0;i < 8;i ++)     //����8��������SDOλ 
    {
        CS5532_SCLK_HIGH();  
        __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
        CS5532_SCLK_LOW();
        __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
    }
    for(i = 0;i < 32;i ++)    //������
    {
        rec_data = rec_data << 1;
        CS5532_SCLK_HIGH();
        __nop();__nop();__nop();__nop();
        rec_data += CS5532_SDO_READ();
        __nop();__nop();
        CS5532_SCLK_LOW();
        __nop();__nop();__nop();__nop();
         __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//��ʱ > 250ns 
    }
    CS5532_SDI_HIGH();
    CS5532_CS_HIGH(); 
    //while(1);
    return rec_data >> 8;
}


