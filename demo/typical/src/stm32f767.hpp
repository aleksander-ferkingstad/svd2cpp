#pragma once

#include <cstdint>
#include <cstddef>

// Register addresses
constexpr uint32_t RCC_BASE = 0x40023800;
constexpr uint32_t GPIOB_BASE = 0x40020400;

// Register definitions
struct RCC_t {
    volatile uint32_t CR, PLLCFGR, CFGR, CIR, AHB1RSTR, AHB2RSTR, AHB3RSTR,
        RESERVED0, APB1RSTR, APB2RSTR, RESERVED1[2], AHB1ENR, AHB2ENR, AHB3ENR,
        RESERVED2, APB1ENR, APB2ENR, RESERVED3[2], AHB1LPENR, AHB2LPENR,
        AHB3LPENR, RESERVED4, APB1LPENR, APB2LPENR;
};

struct GPIO_t {
    volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
};

// Peripheral instances
#define RCC ((RCC_t*)RCC_BASE)
#define GPIOB ((GPIO_t*)GPIOB_BASE)

template<size_t N>
__attribute__((optimize("O0"))) void delay() {
    for (volatile uint32_t i = 0; i < N; ++i) {
        asm volatile("nop");
    }
}
