#include <register.hpp>
#include "delay.hpp"

constexpr Register<uint32_t, 0x40023830, AccessType::ReadWrite> AHB1ENR;

struct GPIOB_t {
    static constexpr uint32_t BASE = 0x40020400;
    static constexpr auto MODER = Register<uint32_t, BASE, AccessType::ReadWrite>();
    static constexpr auto ODR = Register<uint32_t, BASE+0x14, AccessType::ReadWrite>();
};

inline constexpr GPIOB_t GPIOB;

int main() {
    AHB1ENR.bit<1>() = 1;
    GPIOB.MODER.bits<0,1>() = 0;
    GPIOB.MODER.bit<0>() = 1;
    while(1) {
        GPIOB.ODR ^= 1;
        delay<100000>();
    }
}