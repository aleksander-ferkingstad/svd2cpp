// startup.cpp
#include <cstdint>

// External declaration of main
extern int main();

// Stack pointer (defined in the linker script)
extern uint32_t _estack;

// Reset handler
extern "C" __attribute__((naked, noreturn)) void Reset_Handler() {
    // Initialize stack pointer
    asm volatile("ldr sp, =_estack");

    // Call main()
    main();

    // Infinite loop in case main() returns
    while (1);
}

// Vector table
__attribute__((section(".vectors"))) const void* vectors[] = {
    reinterpret_cast<void*>(&_estack),  // Initial stack pointer
    reinterpret_cast<void*>(Reset_Handler)
};