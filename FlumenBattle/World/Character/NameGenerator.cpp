#include "NameGenerator.h"

using namespace world::character;

container::Array <Word> particles = {
    "nab", "leh", "tok", "pur", "dom", "dal", "mil", "neb", "sep", "fam", "fon", 
    "lim", "fek", "shar", "gash", "zesh", "zam", "shib", "saf", "jol", "dur", "rod", "nor", "rand", "bar"
    };

container::Array <Word> startParticles = {
    "maeri", "rae", "gaeli", "ziu", "sha", "tra", "brai", "belo", "teo", "clio", "lama", "doro", "rana", "gama", "vala", "dava", "blaya", "fae", 
    "asa", "oro", "isa", "ula", "ara", "ama", "ela", "eno", "oni", "iba", "zara", "noli", "zefi", "kena", "awa", "aya"
    };

Word NameGenerator::GenerateName()
{
    auto firstParticle = [&] {
        auto totalSize = particles.GetSize() + startParticles.GetSize() * 2;
        if(utility::GetRandom(0, totalSize) < particles.GetSize())
        {
            return particles.GetRandom();
        }
        else
        {
            return startParticles.GetRandom();
        }
    } ();

    auto secondParticle = [&]
    {
        while(true)
        {
            auto particle = particles.GetRandom();
            if(particle == firstParticle)
                continue;

            return particle;
        }
    } ();

    Word name = *firstParticle;
    name << *secondParticle;
    name.Capitalize();

    return name;
}