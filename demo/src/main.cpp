#include <register.hpp>
#include "delay.hpp"

// Leds are on PB0, PB7 and PB14

constexpr Register<uint32_t, 0x40023830, AccessType::ReadWrite> AHB1ENR;

struct GPIOB_t {
    static constexpr uint32_t BASE = 0x40020400;
    static constexpr auto MODER = Register<uint32_t, BASE, AccessType::ReadWrite>();
    static constexpr auto ODR = Register<uint32_t, BASE+0x14, AccessType::ReadWrite>();
};

inline constexpr GPIOB_t GPIOB;

int main() {
    AHB1ENR.bit<1>() = 1;
    
    GPIOB.MODER.slice<0,1>() = 1; // set mode for PB0 to output
    GPIOB.MODER.slice<14,15>() = 1; // set mode for PB7 to output
    GPIOB.MODER.slice<28,29>() = 1; // Set mode for PB14 to output

    while(1) {
        GPIOB.ODR.bits<0,7,14>().toggle();
        delay<100000>();
    }
}