#include "NameGenerator.h"

using namespace world::settlement;

static container::Array <Word> startParticles = 
{
    "frost", "bill", "iron", "rock", "fire", "frame", "sour", "brim", "stone", "forge", "hack", "safe", "wood", "barren", "bare", "root",
    "round", "even", "ever", "sweet", "bitter", "dour", "brisk", "well", "sun", "moon", "star", "sky", "flame", "wolf", "bear", "boar", "hare",
    "groan", "grass", "old", "new", "near", "far", "great", "bright", "dire", "wild", "fair", "mill", "scrub", "flow", "silt", "kindle", "brave",
    "dread", "hard", "tame", "dawn", "dusk", "mourn", "cheer", "bloom", "brine", "bold", "flood", "cliff", "home", "hair", "head", "gold",
    "ferry", "merry", "sorrow", "song", "yarn", "chill", "shield", "yard", "breast", "fleck", "ice", "lone", "sage", "heart", "keen", "bale", "grim",
    "shade", "wall", "blade", "spoke", "wise", "proud"
};

static container::Array <Word> endParticles = 
{
    "ton", "town", "dike", "dyke", "ford", "land", "borrow", "borough", "kirk", "bury", "wick", "by", "bie", "mont", "berry", "ville", "court", "burn",
    "ground", "haven", "wood", "mark", "ham", "mane", "hall", "rock", "hill", "dam", "lane", "creek", "stone", "lake", "dale", "vale", "weald",
    "hurst", "meadow", "brook", "root", "spring", "plane", "bite", "shaft", "pit", "well", "field", "way", "road", "crack", "camp", "watch", "fort",
    "bridge", "rim", "wind", "ridge", "pass", "rift", "pond", "lands", "water", "more", "cradle", "cliff", "head", "glade", "fen", "moor", "marsh",
    "bog", "tip", "song", "yard", "path", "heart", "deep"
    //"reef", "sea", "isle", "harbor", "dock", "tide", "flood", "coast", "beach", "wave"
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