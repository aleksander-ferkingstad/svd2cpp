#include "../generated/STM32F767_pac.hpp"
#include "delay.hpp"

// Simples solution (for reference)
constexpr uint32_t BASE = 0x40020400;
constexpr auto AHB1ENR = Register<uint32_t, 0x40023830, AccessType::ReadWrite>();

int main() {
    AHB1ENR.bit<1>().set();

    GPIOB::MODER::MODER1().set<1>();
    GPIOB::MODER::MODER7().set<1>();
    GPIOB::MODER::MODER14().set<1>();
    
    while(1) {
        GPIOB::ODR::ODR14().toggle();
        delay<100000>();
    }
}