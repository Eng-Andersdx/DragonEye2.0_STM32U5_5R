/**
  ******************************************************************************
  * @file    bsp.h
  * @author  MCD Application Team
  * @brief   This file contains generic definitions for BSP framework and
  *          hardware resources.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */



/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_H
#define __BSP_H

#ifdef __cplusplus
 extern "C" {
#endif

 /* Includes ------------------------------------------------------------------*/
//#define st1x33i
//#define DEU5_2_1R
//#define DEU5_4S
#define DEU5_5R
#define PWM_LED
//#define st1x33i_480x480

typedef enum
{
  PB_SET = 0,
  PB_RESET = !PB_SET,
  PB_LONG_PRESS
} ButtonValue_TypeDef;


typedef enum
{
 BSP_OK    = 0,
 BSP_ERROR = 1

} BSP_Status_TypeDef;


#ifdef __cplusplus
}
#endif

#endif /* __BSP_H */
