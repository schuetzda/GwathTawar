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
    static uint32_t type() noexcept {
        static const uint32_t value = identifier();
        return value;
    }

};
