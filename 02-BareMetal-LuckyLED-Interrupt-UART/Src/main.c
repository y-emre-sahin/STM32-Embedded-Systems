#include <stdint.h>
#include <stdlib.h>

#define RCC_AHB1ENR   (*((volatile uint32_t *)0x40023830))
#define GPIOA_MODER   (*((volatile uint32_t *)0x40020000))
#define GPIOA_ODR     (*((volatile uint32_t *)0x40020014))
#define GPIOB_MODER   (*((volatile uint32_t *)0x40020400))
#define GPIOB_ODR     (*((volatile uint32_t *)0x40020414))
#define GPIOC_MODER   (*((volatile uint32_t *)0x40020800))
#define GPIOC_ODR     (*((volatile uint32_t *)0x40020814))

#define STK_CTRL      (*((volatile uint32_t *)0xE000E010))
#define STK_LOAD      (*((volatile uint32_t *)0xE000E014))
#define STK_VAL       (*((volatile uint32_t *)0xE000E018))

#define GPIOC_IDR     (*((volatile uint32_t *)0x40020810))
#define GPIOC_PUPDR   (*((volatile uint32_t *)0x4002080C))

// SYSCFG Saatini açmak için
#define RCC_APB2ENR       (*((volatile uint32_t *)0x40023844))

// SYSCFG: Hattı Port C'ye bağlamak için
#define SYSCFG_EXTICR4    (*((volatile uint32_t *)0x40013814))

// EXTI: Maskeleme ve Tetikleme ayarları
#define EXTI_IMR          (*((volatile uint32_t *)0x40013C00)) // Interrupt Mask
#define EXTI_FTSR         (*((volatile uint32_t *)0x40013C0C)) // Falling Trigger
#define EXTI_PR           (*((volatile uint32_t *)0x40013C14)) // Pending Register (Bayrak)

// NVIC:(Set Enable Register)
#define NVIC_ISER1        (*((volatile uint32_t *)0xE000E104))

//uart2 saati
#define RCC_APB1ENR       (*((volatile uint32_t *)0x40023840))

// USART2 Birimi Başlangıç Adresi: 0x40004400
#define USART2_SR         (*((volatile uint32_t *)0x40004400)) // Durum (Status)
#define USART2_DR         (*((volatile uint32_t *)0x40004404)) // Veri (Data)
#define USART2_BRR        (*((volatile uint32_t *)0x40004408)) // Hız (Baud Rate)
#define USART2_CR1        (*((volatile uint32_t *)0x4000440C)) // Kontrol 1

#define GPIOA_AFRL        (*((volatile uint32_t *)0x40020020))


void delay_ms(uint32_t ms){
    STK_LOAD = 16000 - 1;
    STK_VAL = 0;
    STK_CTRL = 0x5;
    for (uint32_t i = 0; i < ms; i++){
        while(!(STK_CTRL & (1 << 16)));
    }
    STK_CTRL = 0;
}

typedef struct {
    volatile uint32_t *port_odr;
    uint8_t pin;
} LED;

LED leds[] = {
    {&GPIOA_ODR, 0}, {&GPIOA_ODR, 1}, {&GPIOA_ODR, 4},
    {&GPIOB_ODR, 0}, {&GPIOC_ODR, 1}, {&GPIOC_ODR, 0}
};

void activate_led(int index){
    GPIOA_ODR &= ~((1 << 0) | (1 << 1) | (1 << 4));
    GPIOB_ODR &= ~(1 << 0);
    GPIOC_ODR &= ~((1 << 1) | (1 << 0));
    *(leds[index].port_odr) |= (1 << leds[index].pin);
}

volatile uint8_t buton_basildi = 0;

void EXTI15_10_IRQHandler(void) {
    if (EXTI_PR & (1 << 13)) {

        buton_basildi = 1;
        EXTI_PR |= (1 << 13);
    }
}
void uart_write(int ch) {
    while (!(USART2_SR & (1 << 7)));

    // 2. Harfi veri kutusuna (DR) koy ve gönder!
    USART2_DR = (ch & 0xFF);
}

// Kelime göndermek için (String)
void uart_print(char *str) {
    while (*str) {
        uart_write(*str++);
    }
}
void uart_print_int(int n) {
    if (n == 0) { uart_write('0'); return; } // Sayı 0 ise direkt '0' bas geç.

    char buffer[10]; // Sayının basamaklarını geçici tutacağımız kap.
    int i = 0;

    while (n > 0) {
        // En sağdaki rakamı al, karaktere çevir ('0' ekleyerek) ve kaba koy.
        buffer[i++] = (n % 10) + '0';
        n /= 10; // En sağdaki rakamı sayıdan at.
    }

    // Kapta sayılar ters duruyor (Örn: 3, 2, 1).
    // Şimdi onları tersten (yani 1, 2, 3 diye) ekrana fırlatıyoruz.
    while (i > 0) {
        uart_write(buffer[--i]);
    }
}

int main(void){
    int rastgeleSayi;
    uint32_t sayac = 0;

    RCC_AHB1ENR |= (1 << 0) | (1 << 1) | (1 << 2);
    GPIOA_MODER |= (1 << 0) | (1 << 2) | (2 << 4) | (1 << 8);
    GPIOB_MODER |= (1 << 0);
    GPIOC_MODER |= (1 << 0) | (1 << 2);
    RCC_APB1ENR |= (1 << 17);
    GPIOA_AFRL  |= (7 << 8);
    USART2_CR1  |= (1 << 13);
    USART2_CR1  |= (1 << 3);

    GPIOC_MODER &= ~(3 << 26);
    GPIOC_PUPDR &= ~(3 << 26);
    GPIOC_PUPDR |= (1 << 26);

    USART2_BRR = 0x683;

    //Kesme hatları
    RCC_APB2ENR |= (1 << 14);
    SYSCFG_EXTICR4 &= ~(0xF << 4);//4biti temizledik
    SYSCFG_EXTICR4 |= (2 << 4);//0010 port c
    EXTI_IMR |= (1 << 13);//13. hattan gelen sinyalleri bana ilet
    EXTI_FTSR |= (1 << 13);//voltaj 0a düşünce kesmeyi başlat
    NVIC_ISER1 |= (1 << (40 - 32));

    // BUTON BEKLEME
    while(buton_basildi == 0){
            sayac++;
        }
    uart_print("Butona basildi, sansli LED seciliyor...\r\n");
    srand(sayac);
    rastgeleSayi = rand() % 6;
    buton_basildi = 0;

    int x = 50;

    while(x <= 300){
        for(int i = 0; i < 6; i++){ activate_led(i); delay_ms(x); }
        for(int i = 4; i > 0; i--){ activate_led(i); delay_ms(x); }
        x += 50;
    }


    for (int i = 0; i <= rastgeleSayi; i++){
        activate_led(i);
        delay_ms(300);
    }

    uart_print("Kazanan: ");
    uart_print_int(rastgeleSayi + 1);
    uart_print(". LED!\r\n");

    while(1){
        *(leds[rastgeleSayi].port_odr) &= ~(1 << leds[rastgeleSayi].pin);
        delay_ms(200);
        *(leds[rastgeleSayi].port_odr) |= (1 << leds[rastgeleSayi].pin);
        delay_ms(200);
    }
}
