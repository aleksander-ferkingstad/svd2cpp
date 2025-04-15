#include <register.hpp>
#include "delay.hpp"

// Leds are on PB0, PB7 and PB14
constexpr uint32_t BASE = 0x40020400;
constexpr auto AHB1ENR = Register<uint32_t, 0x40023830, AccessType::ReadWrite>();
constexpr auto MODER = Register<uint32_t, BASE, AccessType::ReadWrite>();
constexpr auto ODR = Register<uint32_t, BASE+0x14, AccessType::ReadWrite>();

int main() {
    AHB1ENR.bits<1,2,3>().set();
    MODER.slice<0,1>() = 1;
    MODER.slice<14,15>() = 1;
    MODER.slice<28,29>() = 1;
    while(1) {
        ODR.bits<14>().toggle();
        delay<1000000>();
    }
}