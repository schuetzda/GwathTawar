#pragma once
#include "Registry.h"

class TypeIDGenerator
{
    static size_t identifier() noexcept {
        static size_t value = 0;
        return value++;
    }

public:
    template<typename>
    static size_t type() noexcept {
        static const size_t value = identifier();
        return value;
    }
};
