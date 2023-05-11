 /**
  ******************************************************************************
  * @file    bsp_cs5532_spi.c
  * @author  zp
  * @version V1.0
  * @date    2014-xx-xx
  * @brief   cs5532 底层应用函数bsp 
  ******************************************************************************
  * @attention
  *
  * 平台: 高铁建钢筋仪
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
  * @brief  CS5532 端口配置
  * @param  无  
  * @retval 无
  */
void CS5532_GPIO_Init(void)
{
	
    GPIO_InitTypeDef GPIO_InitStructure;              /*定义一个GPIO_InitTypeDef类型的结构体*/ 

    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | CS5532_GPIO_CLK, ENABLE); /*开启GPIOB的外设时钟，复用了JTAG口*/
    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);  //禁用JTAG，可以使用SW
		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //推挽输出   
    GPIO_InitStructure.GPIO_Pin = CS5532_CS_PIN | CS5532_SCLK_PIN | CS5532_SDI_PIN;
    GPIO_Init(CS5532_GPIO_PORT, &GPIO_InitStructure);	
	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     //上拉输入
    GPIO_InitStructure.GPIO_Pin = CS5532_SDO_PIN;
    GPIO_Init(CS5532_SDO_GPIO_PORT, &GPIO_InitStructure);

    CS5532_CS_HIGH();    //端口全部置高
    CS5532_SCLK_HIGH();
    CS5532_SDI_HIGH();
    CS5532_SDO_HIGH();  	
}
/**
  * @brief  CS5532 简单的延时函数
  * @param  __IO uint32_t nCount
  * @retval 无
  */
static void CS5532_Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/**
  * @brief  CS5532 发送单个字节
  * @param  unsigned char byte 
  * @retval 无
  */
void CS5532_SendByte(unsigned char byte)
{
    unsigned char i;
    CS5532_SCLK_LOW();
    CS5532_CS_LOW();	
    for(i = 0;i < 10;i ++)  //延时50ns
        __nop();
    for(i = 0;i < 8;i ++)
    {
        if((byte << i) & 0x80) 
            CS5532_SDI_HIGH();
        else             
            CS5532_SDI_LOW();
        __nop();__nop();       //延时50ns 
        __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//延时 > 250ns 
         __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//延时 > 250ns 
        CS5532_SCLK_HIGH();  
        __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//延时 > 250ns 
        CS5532_SCLK_LOW();
    }
    CS5532_SDI_HIGH();
    //CS5532_CS_HIGH(); 
}
/**
  * @brief  CS5532 发送配置命令及配置数据
  * @param  command 8bit命令， reg： 32bit配置信息 
  * @retval 无
  */
void CS5532_Send_REG(u8 command,u32 reg)
{
    unsigned char i,k;
    CS5532_SCLK_LOW(); 
    CS5532_CS_LOW();	
    for(k = 0;k < 3;k ++)  //延时50ns
        __nop();
    for(i = 0;i < 8;i ++)      //先发送命令
    {
        if((command << i) & 0x80) 
            CS5532_SDI_HIGH();
        else             
            CS5532_SDI_LOW();
            __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//延时 > 250ns 
             __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//延时 > 250ns 
        CS5532_SCLK_HIGH();  
        __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//延时 > 250ns 
        CS5532_SCLK_LOW();
    }
    for(i = 0;i < 32;i ++)    //发送32位寄存器
    {
        if((reg << i) & 0x80000000) 
            CS5532_SDI_HIGH();
        else             
            CS5532_SDI_LOW();
            __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//延时 > 250ns 
             __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//延时 > 250ns 
        CS5532_SCLK_HIGH();  
        __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//延时 > 250ns 
        CS5532_SCLK_LOW();
    }
    CS5532_SDI_HIGH();
    CS5532_CS_HIGH();
}

/**
  * @brief  CS5532 读出1字节数据
  * @param  无
  * @retval 读出的byt数据
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
  * @brief  CS5532 读取配置命令下的数据
  * @param  command 命令
  * @retval 32bit数据
  */
u32 CS5532_Read_REG(u8 command)
{
    unsigned char i,k;
    unsigned long rec_data = 0;
    CS5532_SCLK_LOW(); 
    CS5532_CS_LOW();	
    for(k = 0;k < 3;k ++)  //延时50ns
        __nop();
    for(i = 0;i < 8;i ++)      //先发送命令
    {
        if((command << i) & 0x80) 
            CS5532_SDI_HIGH();
        else             
            CS5532_SDI_LOW();
        CS5532_SCLK_HIGH();  
        __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
        CS5532_SCLK_LOW();
    }
    for(i = 0;i < 32;i ++)    //发送32位寄存器
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
         __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//延时 > 250ns 
    }
    CS5532_SDI_HIGH();
    CS5532_CS_HIGH();
    return rec_data;
}
/**
  * @brief  AD 软件复位
  * @param  无 
  * @retval 无
  * CS5532 没有上电复位功能，所以需要软件复位
  * 至少15个0xff，再加1个0xfe
  */
void CS5532_Soft_Reset(void)    
{
    char i;
    for(i = 0;i < 15;i ++)
        CS5532_SendByte(0xff);
    CS5532_SendByte(0xfe);        
}
/**
  * @brief  CS5532 初始化
  * @param  无
  * @retval 无
  */
void CS5532_Init(void)
{
    u32 tmp;
    CS5532_GPIO_Init();       //端口初始化
    CS5532_CS_LOW();          //片选 
    do
    {                  
	      CS5532_Soft_Reset();      //软件复位
        CS5532_Delay(0x1ffff);    //约25ms
        CS5532_Send_REG(CS5532_ConfigWrite,0x20000000);		     
        CS5532_Delay(0xff);
        tmp = CS5532_Read_REG(0x0b);   //读配置寄存器
        //break;
    } 
    while(tmp != 0x10000000);  //当配置寄存器中的RV没有置位时，表示系统没有复位	
	
    CS5532_Delay(0xf);
    CS5532_Send_REG(CS5532_ConfigWrite,CS5532_ConfigData); 	//配置寄存器,电压基准设置为1V~2.5V， RVS(bit25) = 1
    CS5532_Send_REG(CS5532_SetupCH1,CS5532_Channel1_Data); //配置通道1
    CS5532_CS_LOW();          //片选     
    //CS5532_Delay(100);
    //CS5532_SendByte(0xc0);    //连续转换命令    
    //CS5532_SendByte(0x55);    //连续转换命令   
    CS5532_Read_ADData();
}
/**
  * @brief  CS5532 连续读取采集数据
  * @param  无
  * @retval 32bit采集数据
  */
long CS5532_Read_ADData(void)
{
    unsigned char i;
    long rec_data = 0;

    CS5532_SCLK_LOW(); 
    CS5532_SDI_LOW();        //保持SDI为低电平
    CS5532_CS_LOW();	
    CS5532_Delay(100);
    CS5532_SendByte(0x80);
    CS5532_Delay(100);  
     
    while(CS5532_SDO_READ())    //等待SDO拉低，完成转换并准备好数据
    {
    	
    	 
    	
    }	  
    __nop();__nop();__nop();	

	
    for(i = 0;i < 8;i ++)     //发送8个脉冲清SDO位 
    {
        CS5532_SCLK_HIGH();  
        __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
        CS5532_SCLK_LOW();
        __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
    }
    for(i = 0;i < 32;i ++)    //读数据
    {
        rec_data = rec_data << 1;
        CS5532_SCLK_HIGH();
        __nop();__nop();__nop();__nop();
        rec_data += CS5532_SDO_READ();
        __nop();__nop();
        CS5532_SCLK_LOW();
        __nop();__nop();__nop();__nop();
         __nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();//延时 > 250ns 
    }
    CS5532_SDI_HIGH();
    CS5532_CS_HIGH(); 
    //while(1);
    return rec_data >> 8;
}


