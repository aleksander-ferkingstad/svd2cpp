#pragma once
#include <cstdint>
#include <cstddef>
#include <concepts>
#include <type_traits>
#include <bit>
#include "register_traits.hpp"

template<RegisterType reg_type, uintptr_t Address, AccessType access_type=AccessType::ReadWrite>
class Register {
public:
    constexpr Register() noexcept = default;
    void operator=(reg_type bit_mask) {
        static_assert_writable<access_type>();
        auto raw_ptr = reinterpret_cast<volatile reg_type*>(Address);
        *raw_ptr = bit_mask;
    }

    operator reg_type() const {
        static_assert_readable<access_type>();
        auto raw_ptr = reinterpret_cast<volatile reg_type*>(Address);
        return *raw_ptr;
    }

    void operator |= (reg_type bit_mask) const {
        static_assert_modifiable<access_type>();
        auto raw_ptr = reinterpret_cast<volatile reg_type*>(Address);
        *raw_ptr |= bit_mask;
    }

    void operator &= (reg_type bit_mask) const {
        static_assert_modifiable<access_type>();
        auto raw_ptr = reinterpret_cast<volatile reg_type*>(Address);
        *raw_ptr &= bit_mask;
    }

    void operator ^= (reg_type bit_mask) const {
        static_assert_modifiable<access_type>();
        auto raw_ptr = reinterpret_cast<volatile reg_type*>(Address);
        *raw_ptr ^= bit_mask;
    } 
private:
//    static constexpr volatile reg_type* raw_ptr = reinterpret_cast<volatile reg_type*>(Address);
};


