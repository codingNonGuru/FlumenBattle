#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Conventions.hpp"

namespace world::character
{
    class NameGenerator : public core::Singleton <NameGenerator>
    {
        bool hasGeneratedPool {false};

    public:
        Word GenerateName();

        Word FetchName(int);

        void GenerateNamePool();
    };
}