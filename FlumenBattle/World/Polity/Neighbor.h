#pragma once

namespace world::polity
{
    class Polity;

    struct Neighbor
    {
        Polity *Polity;

        int LinkCount;

        bool operator== (class Polity *other) {return other == Polity;}
    };
}