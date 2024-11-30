#pragma once

namespace world::polity
{
    class Polity;

    class Mind
    {
    public:
        virtual void MakeDecision(Polity &) const = 0;

        virtual void UpdateWorkforce(Polity &) const = 0;

        virtual void DecideResearch(Polity &) const = 0;
    };
}