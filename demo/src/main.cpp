#include <register.hpp>
#include "delay.hpp"

// Leds are on PB0, PB7 and PB14
constexpr uint32_t BASE = 0x40020400;
constexpr auto AHB1ENR = Register<uint32_t, 0x40023830, AccessType::ReadWrite>();
constexpr auto MODER = Register<uint32_t, BASE, AccessType::ReadWrite>();
constexpr auto ODR = Register<uint32_t, BASE+0x14, AccessType::ReadWrite>();

struct GPIOB {
    static constexpr uintptr_t BASE = 0x40020400;
    
    struct MODE {
        static uint32_t constexpr OUTPTUT_PP = 0b01;
    };

    struct MODER {
        static constexpr uintptr_t ADDRESS = BASE + 0x00;
        using Reg = Register<uint32_t, ADDRESS, AccessType::ReadWrite>;
        static constexpr auto MODER1() { return Reg{}.slice<0,1>(); }
        static constexpr auto MODER7() { return Reg{}.slice<14,15>(); }
        static constexpr auto MODER14() { return Reg{}.slice<28,29>(); }


        
    };

    struct ODR {
        static constexpr uintptr_t ADDRESS = BASE + 0x14;
        using Reg = Register<uint32_t, ADDRESS, AccessType::ReadWrite>;
        static constexpr auto ODR0() { return Reg{}.bit<0>(); }
        static constexpr auto ODR7() { return Reg{}.bit<7>(); }
        static constexpr auto ODR14() { return Reg{}.bit<14>(); }
    };
};

int main() {
    AHB1ENR.bits<1,2,3>().set();
    GPIOB::MODER::MODER1() = 1;
    GPIOB::MODER::MODER7() = 1;
    GPIOB::MODER::MODER14() = 1;
    while(1) {
        GPIOB::ODR::ODR14().toggle();
        delay<1000000>();
    }
}