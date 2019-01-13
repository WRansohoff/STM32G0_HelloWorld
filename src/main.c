#include "main.h"

/**
 * Main program.
 */
int main(void) {
  // Initial clock setup.
  clock_setup();

  // Enable the GPIOA and GPIOB peripherals.
  #ifdef VVC_F0
    RCC->AHBENR   |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR   |= RCC_AHBENR_GPIOBEN;
  #elif  VVC_F1
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  #elif  VVC_L0
    RCC->IOPENR   |= RCC_IOPENR_IOPAEN;
    RCC->IOPENR   |= RCC_IOPENR_IOPBEN;
  #elif  VVC_G0
    RCC->IOPENR   |= RCC_IOPENR_GPIOAEN;
    RCC->IOPENR   |= RCC_IOPENR_GPIOBEN;
    RCC->IOPENR   |= RCC_IOPENR_GPIOCEN;
  #elif  VVC_L4
    RCC->AHB2ENR  |= RCC_AHB2ENR_GPIOAEN;
    RCC->AHB2ENR  |= RCC_AHB2ENR_GPIOBEN;
  #endif

  // Output type: Push-pull, high-speed.
  // TODO: low-speed?
  #if defined(VVC_F1)
    #if (PiLED < 8)
      PoLED->CRL    &= ~(0xF << (PiLED * 4));
      PoLED->CRL    |=  (0x3 << (PiLED * 4));
    #else
      PoLED->CRH    &= ~(0xF << ((PiLED - 8) * 4));
      PoLED->CRH    |=  (0x3 << ((PiLED - 8) * 4));
    #endif
  #else
    PoLED->MODER    &= ~(0x3 << (PiLED * 2));
    PoLED->MODER    |=  (0x1 << (PiLED * 2));
    PoLED->OTYPER   &= ~(0x1 << PiLED);
    PoLED->OSPEEDR  |=  (0x3 << (PiLED * 2));
    PoLED->ODR      |=  (1 << PiLED);
  #endif

  // Blink the LED.
  while (1) {
    delay_cycles(2000000);
    PoLED->ODR ^=  (1 << PiLED);
  }
}
