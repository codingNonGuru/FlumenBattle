#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Conventions.hpp"

namespace world::settlement
{
    class NameGenerator : public core::Singleton <NameGenerator>
    {
    public:
        Word GenerateName();
    };
}