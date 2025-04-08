#include <stm32f767.hpp>

int main() {
    
    //These must currently be in main, which increases the size of the binary
    Register<uint32_t, Read_write> AHB1ENR {0x40023800 + 0x30};
    Register<uint32_t, Read_write> gpiob_moder {0x40020400 + 0x00};
    Register<uint32_t, Read_write> gpiob_odr {0x40020400 + 0x14};

    AHB1ENR |= (1 << 1); // Enable GPIOB clock
    gpiob_moder &= ~(3 << 0); // Clear MODER bits for PB0
    gpiob_moder |= (1 << 0); // Set MODER bits for PB0 to output mode

    while(1) {
        gpiob_odr ^= (1 << 0); // Toggle PB0
        delay<1000000>();
    }
}



