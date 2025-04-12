#pragma once
#include <cstdint>
#include <cstddef>
#include <concepts>
#include <type_traits>
#include <bit>
#include "register_traits.hpp"

template<RegisterType reg_type, AccessType access_type=AccessType::ReadWrite>
class Register {
public:
constexpr explicit Register(uintptr_t address) noexcept
        : raw_ptr(volatile_from_address(address)) {}

    void operator=(reg_type bit_mask) {
        static_assert_writable<access_type>();
        *raw_ptr = bit_mask;
    }

    operator reg_type() const {
        static_assert_readable<access_type>();
        return *raw_ptr;
    }

    void operator |= (reg_type bit_mask) const {
        static_assert_modifiable<access_type>();
        *raw_ptr |= bit_mask;
    }

    void operator &= (reg_type bit_mask) const {
        static_assert_modifiable<access_type>();
        *raw_ptr &= bit_mask;
    }

    void operator ^= (reg_type bit_mask) const {
        static_assert_modifiable<access_type>();
        *raw_ptr ^= bit_mask;
    }
private:
    volatile reg_type* raw_ptr;
    static constexpr volatile reg_type* volatile_from_address(uintptr_t addr) {
        return reinterpret_cast<volatile reg_type*>(addr);  // Allowed in C++20
    }
};


