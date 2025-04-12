#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
template<size_t N>
__attribute__((optimize("O0"))) void delay() {
    for (volatile uint32_t i = 0; i < N; ++i) {
        asm volatile("nop");
    }
}
#pragma GCC diagnostic pop