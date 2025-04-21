#pragma once
#include "register.hpp"

struct GPIOB {
    static constexpr std::uint32_t BASE = 0x1073873920;
    
    //! @brief MODER register
    struct MODER {
        static constexpr std::uint32_t ADDRESS = BASE + 0x0;
        
        static constexpr std::uint32_t RESET_VALUE = 0x640;
        
        using Reg = Register<std::uint32_t, ADDRESS, AccessType::ReadWrite>;
        
        static constexpr auto MODER0() { return Reg{}.bit<0, 2>(); }
        
        enum class MODER0_t {
            
            Input = 0, // Input mode (reset state)
            
            Output = 1, // General purpose output mode
            
            Alternate = 2, // Alternate function mode
            
            Analog = 3, // Analog mode
            
        };
        
        
    };
    
};