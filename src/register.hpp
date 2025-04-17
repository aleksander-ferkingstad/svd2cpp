#pragma once
#include <cstdint>
#include <cstddef>
#include <concepts>
#include <type_traits>
#include <bit>

enum class AccessType : uint8_t {ReadOnly, WriteOnly, ReadWrite};

template<typename T>
concept RegisterType = std::unsigned_integral<T>;

template<RegisterType reg_type, uintptr_t Address, AccessType access_type=AccessType::ReadWrite>
class Register {
public:
    constexpr Register() noexcept = default;
    
    constexpr void operator=(reg_type bit_mask) const requires(access_type != AccessType::ReadOnly) {
        *reinterpret_cast<volatile reg_type*>(Address) = bit_mask;
    }

    constexpr operator reg_type() const requires(access_type != AccessType::WriteOnly){
        return *reinterpret_cast<volatile reg_type*>(Address);
    }

    constexpr void operator |= (reg_type bit_mask) const requires(access_type == AccessType::ReadWrite){
        *reinterpret_cast<volatile reg_type*>(Address) |= bit_mask;
    }

    constexpr void operator &= (reg_type bit_mask) const requires(access_type == AccessType::ReadWrite){
        *reinterpret_cast<volatile reg_type*>(Address) &= bit_mask;
    }

    constexpr void operator ^= (reg_type bit_mask) const requires(access_type == AccessType::ReadWrite) {
        *reinterpret_cast<volatile reg_type*>(Address) ^= bit_mask;
    } 

    template<size_t pos, size_t width = 1>
    class BitFieldProxy {
        reg_type* target;
    public:
        constexpr BitFieldProxy(volatile reg_type* t) : target(const_cast<reg_type*>(t)) {
            static_assert(pos + width <= sizeof(reg_type)*8, "Bit field out of range");
        }

        template<reg_type Value>
        constexpr void set() const {
            static_assert(Value <= ((reg_type(1) << width) - 1), 
                "Value exceeds bit field width");
            constexpr reg_type mask = ((reg_type(1) << width) - 1) << pos;
            *target = (*target & ~mask) | ((Value << pos) & mask);
        }

        constexpr operator reg_type() const requires(access_type != AccessType::WriteOnly) {
            constexpr reg_type mask = ((reg_type(1) << width) - 1) << pos;
            return (*target & mask) >> pos;
        }
        // For single-bit fields, allow bool assignment and conversion
        
        constexpr void operator=(bool value) const requires(access_type == AccessType::ReadWrite && width == 1) {
            constexpr reg_type mask = reg_type(1) << pos;
            if (value) *target |= mask;
            else *target &= ~mask;
        }
        constexpr operator bool() const requires(access_type != AccessType::WriteOnly && width == 1) {
            return ((*target >> pos) & 1) != 0;
        }
        constexpr void toggle() const requires(access_type == AccessType::ReadWrite && width == 1) {
            *target ^= (reg_type(1) << pos);
        }
        constexpr void set() const requires(access_type == AccessType::ReadWrite && width == 1) {
            *target |= (reg_type(1) << pos);
        }
        constexpr void clear() const requires(access_type == AccessType::ReadWrite && width == 1) {
            *target &= ~(reg_type(1) << pos);
        }
    };

    // Non-contiguous bits proxy (bits<...>)
    template<size_t... positions>
    class BitsGroupProxy {
        reg_type* target;
    public:
        constexpr BitsGroupProxy(volatile reg_type* t) : target(const_cast<reg_type*>(t)) {}
        constexpr void operator=(bool value) const requires(access_type != AccessType::ReadOnly) {
            constexpr reg_type mask = ((reg_type(1) << positions) | ...);
            if (value) *target |= mask;
            else *target &= ~mask;
        }
        constexpr operator reg_type() const requires(access_type != AccessType::WriteOnly) {
            return (*target & ((reg_type(1) << positions) | ...));
        }
        constexpr void toggle() const requires(access_type == AccessType::ReadWrite) {
            *target ^= ((reg_type(1) << positions) | ...);
        }
        constexpr void set() const requires(access_type == AccessType::ReadWrite) {
            *target |= ((reg_type(1) << positions) | ...);
        }
        constexpr void clear() const requires(access_type == AccessType::ReadWrite) {
            *target &= ~((reg_type(1) << positions) | ...);
        }
    };

    // Unified interface
    template<size_t pos, size_t width = 1>
    constexpr BitFieldProxy<pos, width> bit() const {
        static_assert(pos + width <= sizeof(reg_type)*8, "Bit field out of range");
        return {reinterpret_cast<volatile reg_type*>(Address)};
    }
    template<size_t... positions>
    constexpr BitsGroupProxy<positions...> bits() const {
        static_assert(((positions < sizeof(reg_type)*8) && ...), "Bit position out of range");
        return {reinterpret_cast<volatile reg_type*>(Address)};
    }
};


