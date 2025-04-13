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
        *raw_ptr = bit_mask;
    }

    constexpr operator reg_type() const requires(access_type != AccessType::WriteOnly){
        return *raw_ptr;
    }

    constexpr void operator |= (reg_type bit_mask) const requires(access_type == AccessType::ReadWrite){
        *raw_ptr |= bit_mask;
    }

    constexpr void operator &= (reg_type bit_mask) const requires(access_type == AccessType::ReadWrite){
        *raw_ptr &= bit_mask;
    }

    constexpr void operator ^= (reg_type bit_mask) const requires(access_type == AccessType::ReadWrite) {
        *raw_ptr ^= bit_mask;
    } 

    //! @brief proxy for individual bits
    template<size_t pos>
    class BitProxy {
        reg_type* target;
    public:
        constexpr BitProxy(volatile reg_type* t) : target(const_cast<reg_type*>(t)) {}
        constexpr void operator=(bool value) const requires(access_type == AccessType::ReadWrite) {
            constexpr reg_type mask = 1 << pos;
            if (value) { *target |= mask; }
            else { *target &= ~mask; }
        }
        
        constexpr operator bool() const requires(access_type != AccessType::WriteOnly) {
            return (*target >> pos) & 1;
        }

        constexpr void toggle() const requires(access_type == AccessType::ReadWrite) {
            *target ^= (1 << pos);
        }

        constexpr void set() const requires(access_type == AccessType::ReadWrite) {
            *target |= (1 << pos);
        }

        constexpr void clear() const requires(access_type == AccessType::ReadWrite) {
            *target &= ~(1 << pos);
        }
    };

    //! @brief proxy for multiple bits e.g bit<0,1> = 1 would set bit 0 and 1
    template<size_t... positions>
    class BitsGroupProxy {
        reg_type* target;
    public:
        constexpr BitsGroupProxy(volatile reg_type* t) : target(const_cast<reg_type*>(t)) {}
        constexpr void operator=(bool value) const requires(access_type != AccessType::ReadOnly) {
            constexpr reg_type mask = ((1 << positions) | ...);
            if(value) {
                *target |= mask;
            } else {
                *target &= ~mask;
            }
        }
    
        constexpr operator reg_type() const requires(access_type != AccessType::WriteOnly) {
            return (*raw_ptr & ((1 << positions) | ...));
        }
       
        constexpr void toggle() const requires(access_type == AccessType::ReadWrite) {
            *target ^= ((1 << positions) | ...);
        }

        constexpr void set() const requires(access_type == AccessType::ReadWrite) {
            *target |= ((1 << positions) | ...);
        }

        constexpr void clear() const requires(access_type == AccessType::ReadWrite) {
            *target &= ~((1 << positions) | ...);
        }
    };

    //! @brief proxy for a range of bits e.g. slice<0,2> = 3 would set the range [0,2] to 3
    template<size_t start, size_t end>
    class BitSliceProxy {
        reg_type* target;
    public:
        constexpr BitSliceProxy(volatile reg_type* t) : target(const_cast<reg_type*>(t)) {}
        static_assert(start <= end, "Start bit must be <= end bit");
        static_assert(end < sizeof(reg_type)*8, "End bit exceeds register width");
    
        constexpr void operator=(reg_type value) const requires(access_type != AccessType::ReadOnly) {
            constexpr reg_type mask = generate_mask();
            constexpr reg_type value_mask = (1 << (end - start + 1)) - 1;
            *target = (*target & ~mask) | ((value & value_mask) << start);
        }

        constexpr operator reg_type() const requires(access_type != AccessType::WriteOnly) {
            constexpr reg_type mask = generate_mask();
            return (*target & mask) >> start;
        }

    private:
        static constexpr reg_type generate_mask() {
            return ((1 << (end - start + 1)) - 1) << start;
        }
    };

    template<size_t pos>
    constexpr BitProxy<pos> bit() const {
        static_assert(pos < sizeof(reg_type)*8, "Bit position out of range");
        return {raw_ptr};
    }

    template<size_t start, size_t end>
    constexpr BitSliceProxy<start, end> slice() const {
        static_assert(start <= end, "Invalid bit range (start > end)");
        static_assert(end < sizeof(reg_type) * 8, "Bit range exceeds register width");
        return {raw_ptr};
    }

    template<size_t... positions>
    constexpr BitsGroupProxy<positions...> bits() const {
        static_assert(((positions < sizeof(reg_type)*8) && ...), "Bit position out of range");
        return {raw_ptr};
    }
private:
    static inline volatile reg_type* const raw_ptr = reinterpret_cast<volatile reg_type*>(Address);
};


