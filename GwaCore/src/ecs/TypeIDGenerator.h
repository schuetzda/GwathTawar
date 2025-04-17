#pragma once
#include "Registry.h"

class TypeIDGenerator
{
    static uint32_t identifier() noexcept {
        static uint32_t value = 0;
        return value++;
    }


public:
    template<typename>
    static uint32_t type(bool registerNewValue) noexcept {
        static uint32_t value = std::numeric_limits<uint32_t>::max();
        if (registerNewValue && value== std::numeric_limits<uint32_t>::max())
        {
            value = identifier();
        }        
        return value;
    }

};
