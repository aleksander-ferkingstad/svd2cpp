#pragma once
#include "register.hpp"

struct {{ peripheral.name }} {
    static constexpr std::uint32_t BASE = 0x{{ peripheral.base_address }};
    {% for reg in peripheral.registers %}
    //! @brief {{ reg.name }} register
    struct {{ reg.name }} {
        static constexpr std::uint32_t ADDRESS = BASE + 0x{{ reg.address_offset }};
        {%- if reg.reset_value.is_some() %}
        static constexpr std::uint32_t RESET_VALUE = 0x{{ reg.reset_value.as_ref().unwrap() }};
        {%- endif %}
        using Reg = Register<std::uint32_t, ADDRESS, AccessType::ReadWrite>;
        {%- for field in reg.fields %}
        static constexpr auto {{ field.name }}() { return Reg{}.bit<{{ field.bit_offset }}, {{ field.bit_width }}>(); }
        {% if field.enumerated_values.is_some() %}
        enum class {{ field.name }}_t {
            {%- for ev in field.enumerated_values.as_ref().unwrap() %}
            {{ ev.name }} = {{ ev.value }}{% if ev.description.is_some() %}, // {{ ev.description.as_ref().unwrap() }}{% else %},{% endif %}
            {%- endfor %}
        };
        {%- endif %}
        {%- endfor %}
    };
    {%- endfor %}
};