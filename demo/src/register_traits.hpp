#pragma once
#include <cstdint>
#include <cstddef>
#include <concepts>
#include <type_traits>
#include <bit>

enum class AccessType {ReadOnly, WriteOnly, ReadWrite};

template<typename T>
concept Readable = requires(const volatile T* reg) {
    { *reg } -> std::convertible_to<T>;
};

template<typename T>
concept Writable = requires(volatile T* reg, T value) {
    { *reg = value };
};

template<typename T>
concept RegisterType = std::same_as<T, uint8_t> || std::same_as<T, uint16_t> || std::same_as<T, uint32_t> || std::same_as<T, uint64_t>;


template<AccessType access_type>
consteval void static_assert_readable() {
    if constexpr (access_type == AccessType::WriteOnly) {
        static_assert (false, "Cannot read from a write-only register!");
    }
}

template<AccessType access_type>
consteval void static_assert_writable() {
    if constexpr (access_type == AccessType::ReadOnly) {
        static_assert (false, "Cannot write to a read-only register!");
    }
}

template<AccessType access_type>
consteval void static_assert_modifiable() {
    if constexpr (access_type == AccessType::ReadOnly) {
        static_assert (false, "Cannot modify a read-only register!");
    } else if constexpr (access_type == AccessType::WriteOnly) {
        static_assert (false, "Cannot modify a write-only register!");
    }
}