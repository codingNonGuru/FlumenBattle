#pragma once

namespace world::polity
{
    class Polity;

    class Mind
    {
    public:
        virtual void MakeDecision(Polity &) const = 0;
    };
}