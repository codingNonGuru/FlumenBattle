#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Conventions.hpp"

namespace pregame
{
    struct MemberData;

    class PartyLoader : public core::Singleton <PartyLoader>
    {
    public:
        const container::Array <MemberData> &LoadPreset(const char *);

        const container::Array <MemberData> &LoadDefaultPreset();
    };
}