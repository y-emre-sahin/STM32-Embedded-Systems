/**
 * @file main.c
 * @author Emre Sahin
 * @brief Bare-metal Traffic Light Controller for STM32F4
 * @details Implements a smart traffic light with pedestrian skip logic
 * using SysTick timer and direct register access.
 */

#include <stdint.h>

/* --- REGISTER DEFINITIONS --- */
#define RCC_AHB1ENR   (*((volatile uint32_t *)0x40023830))
#define GPIOA_MODER   (*((volatile uint32_t *)0x40020000))
#define GPIOA_ODR     (*((volatile uint32_t *)0x40020014))
#define GPIOA_IDR     (*((volatile uint32_t *)0x40020010))

/* --- SYSTICK DEFINITIONS --- */
#define STK_CTRL      (*((volatile uint32_t *)0xE000E010))
#define STK_LOAD      (*((volatile uint32_t *)0xE000E014))
#define STK_VAL       (*((volatile uint32_t *)0xE000E018))

/**
 * @brief Precise delay function that monitors PA10 button
 * @return 1 if button pressed, 0 if time expired
 */
int delay_ms(uint32_t ms) {
    STK_LOAD = 16000 - 1; // 1ms for 16MHz clock
    STK_VAL = 0;
    STK_CTRL = 0x5;
    for (uint32_t i = 0; i < ms; i++) {
        while (!(STK_CTRL & (1 << 16)));
        if (GPIOA_IDR & (1 << 10)) {
            while (GPIOA_IDR & (1 << 10)); // Wait for release (Debouncing)
            STK_CTRL = 0;
            return 1;
        }
    }
    STK_CTRL = 0;
    return 0;
}

int main(void) {
    RCC_AHB1ENR |= (1 << 0); // Enable GPIOA clock

    // Configure PA0, PA1, PA4 as Output; PA10 as Input
    GPIOA_MODER &= ~((3 << 0) | (3 << 2) | (3 << 8) | (3 << 20));
    GPIOA_MODER |= (1 << 0) | (1 << 2) | (1 << 8);

    uint8_t skip_yellow_phase = 0;

    while (1) {
        // --- PHASE 1: RED ---
        GPIOA_ODR |= (1 << 0);
        if(delay_ms(2000) == 1){
            GPIOA_ODR |= (1 << 1); // Red + Yellow (Get Ready)
            delay_ms(1000);
            skip_yellow_phase = 1;
        }
        GPIOA_ODR &= ~((1 << 0) | (1 << 1)); // Clear Red and Yellow

        // --- PHASE 2: YELLOW ---
        if (skip_yellow_phase == 0) {
            GPIOA_ODR |= (1 << 1);
            delay_ms(1000);
            GPIOA_ODR &= ~(1 << 1);
        }
        skip_yellow_phase = 0; // Reset flag

        // --- PHASE 3: GREEN ---
        GPIOA_ODR |= (1 << 4);
        if(delay_ms(2000) == 1){
            // Immediate transition to Warning (Yellow)
            GPIOA_ODR &= ~(1 << 4);
            GPIOA_ODR |= (1 << 1);
            delay_ms(1000);
        }
        // Cleanup all lights before next cycle
        GPIOA_ODR &= ~((1 << 0) | (1 << 1) | (1 << 4));
    }
}
