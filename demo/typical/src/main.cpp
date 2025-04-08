#include <stm32f767.hpp>

int main() {
    // Enable GPIOB clock
    RCC->AHB1ENR |= (1 << 1);
    
    // Configure PB0 as output
    GPIOB->MODER &= ~(3 << 0);
    GPIOB->MODER |= (1 << 0);

    while(1) {
        GPIOB->ODR ^= (1 << 0);
        delay<1000000>();
    }
}



