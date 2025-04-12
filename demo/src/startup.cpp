#include <cstdint>

extern int main();
extern uint32_t _estack;

// Mark Reset_Handler as used to prevent removal by LTO
extern "C" __attribute__((naked, noreturn, used)) void Reset_Handler() {
    asm volatile("ldr sp, =_estack");
    main();
    while (1);
}

// Mark vector table as used to prevent removal by LTO
__attribute__((section(".vectors"), used)) const void* vectors[] = {
    reinterpret_cast<void*>(&_estack),
    reinterpret_cast<void*>(Reset_Handler)
};