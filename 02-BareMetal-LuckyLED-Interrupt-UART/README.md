STM32 Bare-Metal Lucky LED Game (Interrupt & UART Driven)
An interactive lucky-number game developed for the STM32F401RE (Nucleo) board using pure C and direct register manipulation. This project demonstrates advanced embedded concepts like interrupt handling and custom serial communication libraries without using any HAL or LL libraries.

Features
Bare-Metal Programming: Direct manipulation of RCC, GPIO, SysTick, EXTI, NVIC, and UART registers.

Interrupt-Driven Logic: Utilizes the onboard blue button (PC13) as an asynchronous trigger via EXTI13 and NVIC.

Custom UART Library: Implements manual uart_print and uart_print_int functions for real-time status updates to a serial terminal (9600 Baud).

Entropy-Based Randomness: Generates a seed for srand() by capturing a high-speed counter at the exact microsecond of the button press.

Knight Rider Animation: Features a smooth, slowing LED sequence before landing on the winning "Lucky LED".

Hardware Setup
Board: Nucleo-F401RE

LEDs: - LED 1-3: PA0, PA1, PA4

LED 4: PB0

LED 5-6: PC1, PC0

Button: Onboard Blue Button (PC13) configured with internal Pull-up.

Communication: UART2 (PA2 as TX) connected via ST-LINK USB.

What I Learned
As a first-year Software Engineering student at Ankara University, this project allowed me to master:

Interrupt Vector Table & NVIC: Configuring external interrupts and handling priorities.

Serial Protocols (UART): Calculating Baud Rates and managing Transmit Data Registers (TDR).

Data Serialization: Converting raw integers to ASCII characters for human-readable output (uart_print_int).

Clock Trees: Enabling peripheral clocks across different buses (AHB1, APB1, APB2).
