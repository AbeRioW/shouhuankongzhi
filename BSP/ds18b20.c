/**
    @文 件 名：DS18B20.C
    @功能描述：温度传感器DS18B20初始化驱动代码
    @日    期：5/2/2022
    @作    者：自由eπ
    @注    释：本段程序是基于STM32F03VET6单片机和STM32Cube MX工具开发的
*/

#include "main.h"
#include "ds18b20.h"

/**
    @Brief ：  void delay_us(uint8_t Time)
    @Param：   无
    @Function：us延时
    @Return：  无
    @Author：  自由eπ
  @Note：    无 
*/
void delay_us(uint32_t udelay)
{
  uint32_t startval,tickn,delays,wait;
 
  startval = SysTick->VAL;
  tickn = HAL_GetTick();
  //sysc = 72000;  //SystemCoreClock / (1000U / uwTickFreq);
  delays =udelay * 72; //sysc / 1000 * udelay;
  if(delays > startval)
    {
      while(HAL_GetTick() == tickn)
        {
 
        }
      wait = 72000 + startval - delays;
      while(wait < SysTick->VAL)
        {
 
        }
    }
  else
    {
      wait = startval - delays;
      while(wait < SysTick->VAL && HAL_GetTick() == tickn)
        {
 
        }
    }
}
/**
    @Brief ：  static void Set_DS_Pin_In(void)
    @Param：   无
    @Function：对温度传感器的数据引脚配置为输入模式
    @Return：  无
    @Author：  自由eπ
  @Note：    无 
*/
static void Set_DS_Pin_In(void){
  GPIO_InitTypeDef GPIO_InitStruct = {0};
    
        /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
    
  GPIO_InitStruct.Pin = DS_PORT_Pin;
    GPIO_InitStruct.Mode =GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
    
  HAL_GPIO_Init(DS_PORT_GPIO_Port, &GPIO_InitStruct);
}
/**
    @Brief ：  static void Set_DS_Pin_Out(void)
    @Param：   无
    @Function：对温度传感器的数据引脚配置为出模式
    @Return：  无
    @Author：  自由eπ
  @Note：    无 
*/
static void Set_DS_Pin_Out(void){
  GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
    
  GPIO_InitStruct.Pin = DS_PORT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    
  HAL_GPIO_Init(DS_PORT_GPIO_Port, &GPIO_InitStruct);
}
/**
    @Brief ：  void DS_Reset(void)
    @Param：   无
    @Function：对温度传感器进行复位
    @Return：  无
    @Author：  自由eπ
  @Note：    无 
*/
void DS_Reset(void){
    Set_DS_Pin_Out();
    HAL_GPIO_WritePin (DS_PORT_GPIO_Port,DS_PORT_Pin,GPIO_PIN_RESET);//将DS引脚拉低
    delay_us (750);
    HAL_GPIO_WritePin(DS_PORT_GPIO_Port,DS_PORT_Pin,GPIO_PIN_SET);//将DS引脚拉高
    delay_us (15);
}

/**
    @Brief ：  uint8_t Check_DS(void)
    @Param：   无
    @Function：检查温度传感器DS18B20是否存在
    @Return：  0->存在；1->不存在
    @Author：  自由eπ
  @Note：    无 
*/
uint8_t Check_DS(void){
    uint8_t Pulse_Time=0;
    Set_DS_Pin_In();//Set DSPin Into Push Up Input
    //Waitting for the low pulse from DS18B20 and can keep 60-240us 
    while(HAL_GPIO_ReadPin (DS_PORT_GPIO_Port,DS_PORT_Pin)&&Pulse_Time<100){
        Pulse_Time++;
        delay_us (1);
    }
    if( Pulse_Time>=100)
        return 1;
    else
        Pulse_Time= 0;
    //The processing of out time
        while(!HAL_GPIO_ReadPin (DS_PORT_GPIO_Port,DS_PORT_Pin) && Pulse_Time<240){
        Pulse_Time++;
        delay_us (1);
    }
    if( Pulse_Time>=240)
        return 1;
    else
        return 0;
}
/**
    @Brief ：  static uint8_t DS_Read_Bit(void)
    @Param：   无
    @Function：从温度传感器中获取一个比特位的数据
    @Return：  0->   ；1->    
    @Author：  自由eπ
  @Note：    无 
*/
static  uint8_t DS_Read_Bit(void){
    uint8_t data;
    Set_DS_Pin_Out();
    HAL_GPIO_WritePin (DS_PORT_GPIO_Port ,DS_PORT_Pin ,GPIO_PIN_RESET);//Set DS pin into reset
    delay_us (10);
    
    Set_DS_Pin_In();
    if(HAL_GPIO_ReadPin (DS_PORT_GPIO_Port ,DS_PORT_Pin) == SET)
        data=1;
    else 
        data=0;
    delay_us(45);
    return data ;
}
/**
    @Brief ：  static uint8_t DS_Read_Byte(void)
    @Param：   无
    @Function：从温度传感器中获取一个字节的数据，低位先行
    @Return：  0->   ；1->    
    @Author：  自由eπ
  @Note：    无 
*/
static  uint8_t DS_Read_Byte(void){
    
    uint8_t i,j,data=0;
    for(i=0;i<8;i++){
        j=DS_Read_Bit();
        data=(data)|(j<<i);
    }
    
    return data ;
}
/**
    @Brief ：  static  void  Write_DS_Byte(uint8_t data)
    @Param：   uint8_t data
    @Function：向温度传感器中写入一个字节的数据，低位先行
    @Return：  无 
    @Author：  自由eπ
  @Note：    无 
*/
static  void  DS_Write_Byte(uint8_t data){
    uint8_t Test_Bit;
    
    Set_DS_Pin_Out();
    for(Test_Bit=0x01; Test_Bit !=0; Test_Bit<<=1){
        //Write 0
        if((data&Test_Bit)==RESET){
            HAL_GPIO_WritePin (DS_PORT_GPIO_Port,DS_PORT_Pin,GPIO_PIN_RESET);//Set DS Pin into reset
            delay_us(70);
            HAL_GPIO_WritePin (DS_PORT_GPIO_Port,DS_PORT_Pin,GPIO_PIN_SET);
            delay_us(2);
        }
        //Write 1
        else {
            HAL_GPIO_WritePin (DS_PORT_GPIO_Port,DS_PORT_Pin,GPIO_PIN_RESET);//Set DS Pin into reset
            delay_us(9);
            HAL_GPIO_WritePin (DS_PORT_GPIO_Port,DS_PORT_Pin,GPIO_PIN_SET);
            delay_us (55);
        }
    }
}
/**
    @Brief ：  uint8_t DS_Init(void)
    @Param：   无
    @Function：对温度传感器进行初始化
    @Return：  无
    @Author：  自由eπ
  @Note：    无
*/
uint8_t DS_Init(void){
    DS_Reset();
    return Check_DS() ;
}
/**
    @Brief ：  float Get_DS_Tempture(void)
    @Param：   无
    @Function：获取温度
    @Return：  从DS18B20上读取到的温度值
    @Author：  自由eπ
  @Note：    此获取方法为跳过 ROM 读取,适合于总线上只有一个设备
*/
float Get_DS_Temperature(void){
    
    uint8_t tpmsb,tplsb;
    short s_tem;
    float f_tem;
    
    DS_Reset ();
    Check_DS ();
    DS_Write_Byte (SKIP_ROM );       //Skip ROM
    DS_Write_Byte (CONVERT_TEMP );   //Let transition temperature
    delay_us (350);
    DS_Reset ();
    Check_DS ();
    DS_Write_Byte (SKIP_ROM );       //Skip ROM
    DS_Write_Byte (READ_SCRATCHPAD); //Get temperature from DS18B20
    
    tplsb=DS_Read_Byte();
    tpmsb=DS_Read_Byte();
    
    s_tem = tpmsb<<8;
    s_tem = s_tem | tplsb;
    
    if( s_tem < 0 )        /* 负温度 */
        f_tem = (~s_tem+1) * 0.0625;    
    else
        f_tem = s_tem * 0.0625;
    
    return f_tem; 
}