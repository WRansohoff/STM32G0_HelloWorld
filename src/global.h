#ifndef _VVC_GLOBAL_H
#define _VVC_GLOBAL_H

#include <stdint.h>

#ifdef VVC_F0
  #include "stm32f0xx.h"
#elif  VVC_G0
  #include "stm32g0xx.h"
#elif  VVC_F1
  #include "stm32f1xx.h"
#elif  VVC_L0
  #include "stm32l0xx.h"
#elif  VVC_L4
  #include "stm32l4xx.h"
#endif

// Global defines.
// (LED pin depends on your board)
#ifdef VVC_F1
  #define PoLED   (GPIOB)
  #define PiLED   (12)
#elif  VVC_G0
  //#define PoLED   (GPIOC)
  //#define PiLED   (6)
  #define PoLED   (GPIOA)
  #define PiLED   (8)
#else
  #define PoLED   (GPIOB)
  #define PiLED   (3)
#endif

// Global variables.
uint32_t core_clock_hz;

#endif
