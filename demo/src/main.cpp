#include <register.hpp>
#include "delay.hpp"

constinit Register<uint32_t, 0x40023830, AccessType::ReadWrite> AHB1ENR;
constinit Register<uint32_t, 0x40020400, AccessType::ReadWrite> gpiob_moder;
constinit Register<uint32_t, 0x40020414, AccessType::ReadWrite> gpiob_odr;

int main() {
    AHB1ENR |= (1 << 1); // Enable GPIOB clock
    gpiob_moder &= ~(3 << 0); // Clear MODER bits for PB0
    gpiob_moder |= (1 << 0); // Set MODER bits for PB0 to output mode
    while(1) {
        gpiob_odr ^= (1 << 0); // Toggle PB0
        delay<100000>();
    }
}
