#pragma once
#include <cstdint>
#include <cstddef>
#include <concepts>
#include <type_traits>
#include <bit>

class Read_only {};
class Write_only {};
class Read_write : public Read_only, public Write_only {};

template<typename T>
concept RegisterType = std::same_as<T, uint8_t> || std::same_as<T, uint16_t> || std::same_as<T, uint32_t> || std::same_as<T, uint64_t>;

template<RegisterType Reg_type, typename Access_type = Read_write>
class Register {
public:
constexpr Register(uint32_t address) : raw_ptr(std::bit_cast<volatile Reg_type*>(address)) {}

    void operator=(Reg_type bit_mask) {
        *raw_ptr = bit_mask;
    }

    operator Reg_type() const {
        return *raw_ptr;
    }

    void operator |= (Reg_type bit_mask) const {
        *raw_ptr |= bit_mask;
    }

    void operator &= (Reg_type bit_mask) const {
        *raw_ptr &= bit_mask;
    }

    void operator ^= (Reg_type bit_mask) const {
        *raw_ptr ^= bit_mask;
    }
private:
    volatile Reg_type* raw_ptr;
};

template<size_t N>
__attribute__((optimize("O0"))) void delay() {
    for (volatile uint32_t i = 0; i < N; ++i) {
        asm volatile("nop");
    }
}
