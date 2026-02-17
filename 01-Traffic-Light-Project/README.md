# STM32 Bare-Metal Traffic Light Controller

A realistic traffic light simulation developed for the STM32F401RE (Nucleo) board using pure C and direct register manipulation (No HAL/LL libraries).

Features
- Bare-Metal Programming: Direct manipulation of RCC, GPIO, and SysTick registers.
- State-Aware Logic: Handles realistic transitions (e.g., Red+Yellow when button is pressed during Red phase).
- Non-blocking Polling: The `delay_ms` function monitors the pedestrian button every 1ms for instant feedback.
- Hardware-Software Integration: Implemented Pull-down resistor logic for stable input reading.

Hardware Setup
- Board: Nucleo-F401RE
- LEDs: Red (PA0), Yellow (PA1), Green (PA4)
- Button: Connected to PA10 (Pull-down resistor recommended)

What I Learned
Through this project at Ankara University, I mastered the fundamentals of embedded software engineering, including:
- Clock management (RCC).
- Pin multiplexing and mode configuration (MODER).
- Bitwise operations for register control.
- Timer-based delay implementation.
