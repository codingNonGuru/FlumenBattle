#include "NameGenerator.h"

using namespace world::settlement;

static container::Array <Word> startParticles = 
{
    "frost", "bill", "iron", "rock", "fire", "frame", "sour", "brim", "stone", "forge", "hack", "safe", "wood", "barren", "bare", "root",
    "round", "even", "ever", "sweet", "bitter", "dour", "brisk", "well", "sun", "moon", "star", "sky", "flame", "wolf", "bear", "boar",
    "groan", "grass", "old", "new", "near", "great", "bright", "dire", "wild", "fair"
};

static container::Array <Word> endParticles = 
{
    "ton", "town", "dike", "dyke", "ford", "land", "borrow", "borough", "kirk", "bury", "wick", "by", "bie", "mont", "berry", "ville", "court", "burn",
    "ground", "haven", "wood", "mark", "ham", "mane", "hall", "rock", "hill", "dam", "lane", "creek", "stone", "lake", "dale", "vale", "weald",
    "hurst", "meadow", "brook", "root", "spring", "plane", "bite", "shaft", "pit", "well", "field", "way", "road", "crack", "camp", "watch", "fort",
    "bridge", "rim"
};

Word NameGenerator::GenerateName()
{
    auto firstParticle = [&] 
    {
        return startParticles.GetRandom();
    } ();

    auto secondParticle = [&]
    {
        while(true)
        {
            auto particle = endParticles.GetRandom();
            if(*particle == *firstParticle)
                continue;

            return particle;
        }
    } ();

    Word name = *firstParticle;
    name << *secondParticle;
    name.Capitalize();

    return name;
}