#pragma once
#include <cstdint>
#include <cstddef>
#include <concepts>
#include <type_traits>
#include <bit>

enum class AccessType {ReadOnly, WriteOnly, ReadWrite};

template<typename T>
concept RegisterType = std::same_as<T, uint8_t> || std::same_as<T, uint16_t> || std::same_as<T, uint32_t> || std::same_as<T, uint64_t>;

template<RegisterType reg_type, uintptr_t Address, AccessType access_type=AccessType::ReadWrite>
class Register {
public:
    constexpr Register() noexcept = default;
      
    
    constexpr void operator=(reg_type bit_mask) const requires(access_type != AccessType::ReadOnly) {
        *raw_ptr() = bit_mask;
    }

    constexpr operator reg_type() const requires(access_type != AccessType::WriteOnly){
        return *raw_ptr();
    }

    constexpr void operator |= (reg_type bit_mask) const requires(access_type == AccessType::ReadWrite){
        *raw_ptr() |= bit_mask;
    }

    constexpr void operator &= (reg_type bit_mask) const requires(access_type == AccessType::ReadWrite){
        *raw_ptr() &= bit_mask;
    }

    constexpr void operator ^= (reg_type bit_mask) const requires(access_type == AccessType::ReadWrite) {
        *raw_ptr() ^= bit_mask;
    } 

    //! @brief proxy for individual bits
    template<size_t pos>
    class BitProxy {
    public:
        constexpr void operator=(bool value) const requires(access_type == AccessType::ReadWrite) {
            constexpr reg_type mask = 1 << pos;
            if (value) { *raw_ptr() |= mask; }
            else { *raw_ptr() &= ~mask; }
        }
        
        constexpr operator bool() const requires(access_type != AccessType::WriteOnly) {
            return (*raw_ptr() >> pos) & 1;
        }

        constexpr void toggle() const requires(access_type == AccessType::ReadWrite) {
            *raw_ptr() ^= (1 << pos);
        }
    };

    //! @brief proxy for multiple bits e.g bit<0,1> = 1 would set bit 0 and 1
    template<size_t... positions>
    class BitsGroupProxy {
    public:
        constexpr void operator=(bool value) const requires(access_type != AccessType::ReadOnly) {
            constexpr reg_type mask = ((1 << positions) | ...);
            if(value) {
                *raw_ptr() |= mask;
            } else {
                *raw_ptr() &= ~mask;
            }
        }
    
        constexpr operator reg_type() const requires(access_type != AccessType::WriteOnly) {
            return (*raw_ptr() & ((1 << positions) | ...));
        }
    };

    //! @brief proxy for a range of bits e.g. slice<0,2> = 3 would set the range [0,2] to 3
    template<size_t start, size_t end>
    class BitSliceProxy {
    public:
        static_assert(start <= end, "Start bit must be <= end bit");
        static_assert(end < sizeof(reg_type)*8, "End bit exceeds register width");
    
        constexpr void operator=(reg_type value) const requires(access_type != AccessType::ReadOnly) {
            constexpr reg_type mask = generate_mask();
            constexpr reg_type value_mask = (1 << (end - start + 1)) - 1;
            *raw_ptr() = (*raw_ptr() & ~mask) | ((value & value_mask) << start);
        }
    
        constexpr operator reg_type() const requires(access_type != AccessType::WriteOnly) {
            constexpr reg_type mask = generate_mask();
            return (*raw_ptr() & mask) >> start;
        }
    
    private:
        static constexpr reg_type generate_mask() {
            return ((1 << (end - start + 1)) - 1) << start;
        }
    };

    template<size_t pos>
    constexpr BitProxy<pos> bit() const {
        static_assert(pos < sizeof(reg_type)*8, "Bit position out of range");
        return {};
    }

    template<size_t start, size_t end>
    constexpr BitSliceProxy<start, end> slice() const {
        static_assert(start <= end, "Invalid bit range (start > end)");
        static_assert(end < sizeof(reg_type)*8, "Bit range exceeds register width");
        return {};
    }

    template<size_t... positions>
    constexpr BitsGroupProxy<positions...> bits() const {
        static_assert(((positions < sizeof(reg_type)*8) && ...), "Bit position out of range");
        return {};
    }
private:
    static constexpr volatile reg_type* raw_ptr() {
        return reinterpret_cast<volatile reg_type*>(Address);
    }
};


