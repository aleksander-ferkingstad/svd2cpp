#include <register.hpp>
#include "delay.hpp"

constexpr Register<uint32_t, 0x40023830, AccessType::ReadWrite> AHB1ENR;
constexpr Register<uint32_t, 0x40020400, AccessType::ReadWrite> gpiob_moder;
constexpr Register<uint32_t, 0x40020414, AccessType::ReadWrite> gpiob_odr;
/*
int main() {
    AHB1ENR.bit<1>() = 1; // Enable GPIOB clock
    gpiob_moder.slice<0,1>() = 1; // Set MODER bits for PB0 to output mode
    while(1) {
        //gpiob_odr.bit<0>() = !gpiob_odr.bit<0>(); // Toggle PB0
        gpiob_odr ^= 1; // Toggle PB0
        delay<100000>();
    }
}
*/

int main() {
    AHB1ENR |= 3; // Enable GPIOB clock
    gpiob_moder &= ~(3 << 0); // Clear MODER bits for PB0
    gpiob_moder |= (1 << 0); // Set MODER bits for PB0 to output mode
    while(1) {
        gpiob_odr ^= (1 << 0); // Toggle PB0
        delay<100000>();
    }
}