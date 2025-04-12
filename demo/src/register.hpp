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
    
    void operator=(reg_type bit_mask) const requires(access_type != AccessType::ReadOnly) {
        *raw_ptr = bit_mask;
    }

    operator reg_type() const requires(access_type != AccessType::WriteOnly){
        return *raw_ptr;
    }

    void operator |= (reg_type bit_mask) const requires(access_type == AccessType::ReadWrite){
        *raw_ptr |= bit_mask;
    }

    void operator &= (reg_type bit_mask) const requires(access_type == AccessType::ReadWrite){
        *raw_ptr &= bit_mask;
    }

    void operator ^= (reg_type bit_mask) const requires(access_type == AccessType::ReadWrite) {
        *raw_ptr ^= bit_mask;
    } 
private:
    static inline volatile reg_type* const raw_ptr = reinterpret_cast<volatile reg_type*>(Address);
};


