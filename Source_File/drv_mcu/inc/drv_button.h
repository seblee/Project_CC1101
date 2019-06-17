/**
  ******************************************************************************
  * @author  泽耀科技 ASHINING
  * @version V3.0
  * @date    2016-10-08
  * @brief   BUTTON配置H文件
  ******************************************************************************
  * @attention
  *
  * 官网	:	http://www.ashining.com
  * 淘宝	:	https://shop105912646.taobao.com
  * 阿里巴巴:	https://cdzeyao.1688.com
  ******************************************************************************
  */


#ifndef __DRV_BUTTON_H__
#define __DRV_BUTTON_H__


#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

//按键硬件定义 KEY0
#define BUTOTN_GPIO_PORT			GPIOA									
#define BUTTON_GPIO_CLK				RCC_APB2Periph_GPIOA
#define BUTTON_GPIO_PIN				GPIO_Pin_0


/** 按键状态定义 */
enum
{
	BUTOTN_UP = 0,		//按键未按下
	BUTOTN_PRESS_DOWN	//按键按下
};



void drv_button_init( void );
uint8_t drv_button_check( void );

#endif

