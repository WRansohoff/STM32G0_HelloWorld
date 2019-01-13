#include "util.h"

// Setup the core system clock speed. Currently just one speed;
// 48MHz for F0 chips, 32MHz for L0, 72MHz for F1 and L4.
void clock_setup(void) {
  #ifdef VVC_F0
    // Reset the Flash 'Access Control Register', and
    // then set 1 wait-state and enable the prefetch buffer.
    // (The device header files only show 1 bit for the F0
    //  line, but the reference manual shows 3...)
    FLASH->ACR &= ~(0x00000017);
    FLASH->ACR |=  (FLASH_ACR_LATENCY |
                    FLASH_ACR_PRFTBE);
    // Configure the PLL to (HSI / 2) * 12 = 48MHz.
    // Use a PLLMUL of 0xA for *12, and keep PLLSRC at 0
    // to use (HSI / PREDIV) as the core source. HSI = 8MHz.
    RCC->CFGR  &= ~(RCC_CFGR_PLLMUL |
                    RCC_CFGR_PLLSRC);
    RCC->CFGR  |=  (RCC_CFGR_PLLSRC_HSI_DIV2 |
                    RCC_CFGR_PLLMUL12);
    // Turn the PLL on and wait for it to be ready.
    RCC->CR    |=  (RCC_CR_PLLON);
    while (!(RCC->CR & RCC_CR_PLLRDY)) {};
    // Select the PLL as the system clock source.
    RCC->CFGR  &= ~(RCC_CFGR_SW);
    RCC->CFGR  |=  (RCC_CFGR_SW_PLL);
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) {};
    // The system clock is now 48MHz.
    core_clock_hz = 48000000;
  #elif VVC_F1
    // Set 2 wait states in flash and enable the prefetch buffer.
    FLASH->ACR &= ~(FLASH_ACR_LATENCY);
    FLASH->ACR |=  (FLASH_ACR_LATENCY_2 |
                    FLASH_ACR_PRFTBE);
    // Enable the HSE oscillator.
    // (This will be an infinite loop if your board doesn't have
    //  an HSE oscillator, but most cheap F103C8 boards seem to have one.)
    RCC->CR    |=  (RCC_CR_HSEON);
    while (!(RCC->CR & RCC_CR_HSERDY)) {};
    // Set the HSE oscillator as the system clock source.
    RCC->CFGR  &= ~(RCC_CFGR_SW);
    RCC->CFGR  |=  (RCC_CFGR_SW_HSE);
    // Set the PLL multiplication factor to 9, for 8*9=72MHz.
    RCC->CFGR  &= ~(RCC_CFGR_PLLMULL);
    RCC->CFGR  |=  (RCC_CFGR_PLLMULL9);
    // Enable the PLL.
    RCC->CR    |=  (RCC_CR_PLLON);
    while (!(RCC->CR & RCC_CR_PLLRDY)) {};
    // Set the PLL as the system clock source.
    RCC->CFGR  &= ~(RCC_CFGR_SW);
    RCC->CFGR  |=  (RCC_CFGR_SW_PLL);
    // The core clock is now 72MHz.
    core_clock_hz = 72000000;
  #elif VVC_L0
    // Set the Flash ACR to use 1 wait-state
    // and enable the prefetch buffer and pre-read.
    FLASH->ACR |=  (FLASH_ACR_LATENCY |
                    FLASH_ACR_PRFTEN |
                    FLASH_ACR_PRE_READ);
    // Enable the HSI oscillator, since the L0 series boots
    // to the MSI one.
    RCC->CR    |=  (RCC_CR_HSION);
    while (!(RCC->CR & RCC_CR_HSIRDY)) {};
    // Configure the PLL to use HSI16 with a PLLDIV of
    // 2 and PLLMUL of 4.
    RCC->CFGR  &= ~(RCC_CFGR_PLLDIV |
                    RCC_CFGR_PLLMUL |
                    RCC_CFGR_PLLSRC);
    RCC->CFGR  |=  (RCC_CFGR_PLLDIV2 |
                    RCC_CFGR_PLLMUL4 |
                    RCC_CFGR_PLLSRC_HSI);
    // Enable the PLL and wait for it to stabilize.
    RCC->CR    |=  (RCC_CR_PLLON);
    while (!(RCC->CR & RCC_CR_PLLRDY)) {};
    // Select the PLL as the system clock source.
    RCC->CFGR  &= ~(RCC_CFGR_SW);
    RCC->CFGR  |=  (RCC_CFGR_SW_PLL);
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) {};
    // Set the global clock speed variable.
    core_clock_hz = 32000000;
  #elif VVC_L4
    // Technically the L4 can do 80MHz, but 72MHz is rounder.
    // Set the Flash ACR to use 3 wait-states.
    // and enable the prefetch buffer and pre-read.
    FLASH->ACR    |=  (FLASH_ACR_LATENCY_3WS |
                       FLASH_ACR_PRFTEN      |
                       FLASH_ACR_ICEN        |
                       FLASH_ACR_DCEN);
    // Enable the HSI oscillator, since the L0 series boots
    // to the MSI one.
    RCC->CR       |=  (RCC_CR_HSION);
    while (!(RCC->CR & RCC_CR_HSIRDY)) {};
    // Configure the PLL to use HSI16 with a PLLDIV of
    // 4 (PLLR of 0 == /2) and PLLMUL of 18.
    RCC->PLLCFGR  &= ~(RCC_PLLCFGR_PLLM |
                       RCC_PLLCFGR_PLLN |
                       RCC_PLLCFGR_PLLR |
                       RCC_PLLCFGR_PLLSRC);

    // Without the 'PLLREN' bit, the main PLL clock
    // output is disabled and 'SWS_PLL' is never set.
    RCC->PLLCFGR  |=  (RCC_PLLCFGR_PLLM_0 |
                      (18 << RCC_PLLCFGR_PLLN_Pos) |
                       RCC_PLLCFGR_PLLREN |
                       RCC_PLLCFGR_PLLSRC_HSI);
    // Enable the PLL and wait for it to stabilize.
    RCC->CR       |=  (RCC_CR_PLLON);
    while (!(RCC->CR & RCC_CR_PLLRDY)) {};
    // Select the PLL as the system clock source.
    RCC->CFGR     &= ~(RCC_CFGR_SW);
    RCC->CFGR     |=  (RCC_CFGR_SW_PLL);
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) {};
    // Set the global clock speed variable.
    core_clock_hz  = 72000000;
  #endif
}

// Simple delay method, with instructions not to optimize.
// It doesn't accurately delay a precise # of cycles,
// it's just a rough scale.
void __attribute__((optimize("O0"))) delay_cycles(uint32_t cyc) {
  uint32_t d_i;
  for (d_i = 0; d_i < cyc; ++d_i) {
    asm("NOP");
  }
}
