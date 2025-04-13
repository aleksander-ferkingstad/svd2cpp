#include <cstdint>
#include <cstddef>
#include "delay.hpp"


constexpr uint32_t AHB1ENR_BASE = 0x40023830;
constexpr uint32_t GPIOB_MODER_BASE = 0x40020400;
constexpr uint32_t GPIOB_ODR_BASE = 0x40020414;

volatile uint32_t* AHB1ENR = reinterpret_cast<volatile uint32_t*>(AHB1ENR_BASE);
volatile uint32_t* GPIOB_MODER = reinterpret_cast<volatile uint32_t*>(GPIOB_MODER_BASE);
volatile uint32_t* GPIOB_ODR = reinterpret_cast<volatile uint32_t*>(GPIOB_ODR_BASE);

int main() {
    *AHB1ENR |= 3;
    *GPIOB_MODER &= ~(3 << 0);
    *GPIOB_MODER |= (1 << 0);
    while(1) {
        *GPIOB_ODR ^= (1 << 0);
        delay<1000000>();
    }
}



