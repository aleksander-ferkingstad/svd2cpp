
#include <cstdint>
extern int main();

// Stack pointer (defined in the linker script)
extern uint32_t _estack;

extern "C" __attribute__((naked, noreturn)) void Reset_Handler() {
    asm volatile("ldr sp, =_estack");
    main();
    while (1);
}

__attribute__((section(".vectors"))) const void* vectors[] = {
    reinterpret_cast<void*>(&_estack),
    reinterpret_cast<void*>(Reset_Handler)
};
