#include <register.hpp>
#include "delay.hpp"

// Simples solution
constexpr uint32_t BASE = 0x40020400;
constexpr auto AHB1ENR = Register<uint32_t, 0x40023830, AccessType::ReadWrite>();
constexpr auto MODER = Register<uint32_t, BASE, AccessType::ReadWrite>();
constexpr auto ODR = Register<uint32_t, BASE+0x14, AccessType::ReadWrite>();

// Structured solution
//GPIOB::MODER::MODER1() = 1;
//GPIOB::MODER::MODER7() = 1;
//GPIOB::MODER::MODER14() = 1;
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

// Inverted logic
// Example: pin<B, 0>::MODER() = 1;
// Example: pin<B, 0>::ODR().toggle();
// Inverted logic is nice for GPIO, but how would it work for other peripherals?
struct A {};
struct B {};
template<typename Port>
struct GPIOBaseAddress;
template<> struct GPIOBaseAddress<A> { static constexpr uintptr_t value = 0x40020000; };
template<> struct GPIOBaseAddress<B> { static constexpr uintptr_t value = 0x40020400; };

template<typename Port, size_t Pin>
struct pin {
    static constexpr uintptr_t BASE = GPIOBaseAddress<Port>::value;

    static constexpr uintptr_t MODER_ADDR = BASE + 0x00;
    using MODER_Reg = Register<uint32_t, MODER_ADDR, AccessType::ReadWrite>;
    static constexpr auto MODER() { return MODER_Reg{}.template slice<Pin*2, Pin*2+1>(); }

    static constexpr uintptr_t ODR_ADDR = BASE + 0x14;
    using ODR_Reg = Register<uint32_t, ODR_ADDR, AccessType::ReadWrite>;
    static constexpr auto ODR() { return ODR_Reg{}.template bit<Pin>(); }
};


int main() {
    AHB1ENR.bit<1>().set();
    pin<B, 0>::MODER() = 1;
    GPIOB::MODER::MODER1() = 1;
    GPIOB::MODER::MODER7() = 1;
    GPIOB::MODER::MODER14() = 1;

    
    while(1) {
        GPIOB::ODR::ODR14().toggle();
        pin<B, 0>::ODR().toggle();  
        delay<1000000>();
    }
}