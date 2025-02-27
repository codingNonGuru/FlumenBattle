#include "NameGenerator.h"

using namespace world::settlement;

static container::Array <Word> startParticles = 
{
    "frost", "bill", "iron", "rock", "fire", "frame", "sour", "brim", "stone", "forge", "hack", "safe", "wood", "barren", "bare", "root",
    "round", "even", "ever", "sweet", "bitter", "dour", "brisk", "well", "sun", "moon", "star", "sky", "flame", "wolf", "bear", "boar", "hare",
    "groan", "grass", "old", "new", "near", "far", "great", "bright", "dire", "wild", "fair", "mill", "scrub", "flow", "silt", "kindle", "brave",
    "dread", "hard", "tame", "dawn", "dusk", "mourn", "cheer", "bloom", "brine", "bold", "flood", "cliff", "home", "hair", "head", "gold",
    "ferry", "merry", "sorrow", "song", "yarn", "chill", "shield", "yard", "breast", "fleck", "ice", "lone", "sage", "heart", "keen", "bale", "grim",
    "shade", "wall", "blade", "spoke", "wise", "proud", "mouth", "neck", "oak", "ash", "pine", "car", "kin", "bryn", "shroud", "shale", "bow", "mist",
    "winter", "summer", "fall", "mimble", "cross", "sizzle", "key", "smite", "broke", "smoke", "still", "loud", "fare", "chaff", "snow", "gray", "waste",
    "hammer", "light", "dark", "dust", "peak", "rain", "foul", "seed", "dirt", "riddle", "lock", "wander", "knave", "shard", "mellow", "nut", "nail",
    "chisel", "chap", "chime", "bless", "quick", "fast", "bottom", "top", "toast", "corn", "husk", "fiddle", "needle", "thorn", "whole", "some", "frail",
    "rope", "chest", "whistle", "sear", "singe", "stir", "stick", "stale", "grow", "stave", "staff", "flock", "wool", "huckle", "pile", "pyke", "bird", 
    "fresh", "tear", "foot", "roof", "rough", "cedar", "maple", "birch", "reach", "rash", "wonder", "witch", "arrow", "barn", "thunder", "bluff", "noon",
    "mush", "bride", "raven", "wax", "white", "gray", "tail", "lost", "never", "hope", "glove", "coke", "faint", "faith", "grove", "grief", "gale",
    "pebble", "fist", "glow", "horn", "brick", "hide", "deer", "dear", "moth", "maw", "shrift", "sin", "brow", "fog", "gift", "give"
};

static container::Array <Word> endParticles = 
{
    "ton", "town", "dike", "dyke", "ford", "land", "borrow", "borough", "kirk", "bury", "wick", "by", "bie", "mont", "berry", "ville", "court", "burn",
    "ground", "haven", "wood", "mark", "ham", "mane", "hall", "rock", "hill", "dam", "lane", "creek", "stone", "lake", "dale", "vale", "weald",
    "hurst", "meadow", "brook", "root", "spring", "plane", "bite", "shaft", "pit", "well", "field", "way", "road", "crack", "camp", "watch", "fort",
    "bridge", "rim", "wind", "ridge", "pass", "rift", "pond", "lands", "water", "more", "cradle", "cliff", "head", "glade", "fen", "moor", "marsh",
    "bog", "tip", "song", "yard", "path", "heart", "deep", "mere", "mead", "wast", "cher", "gard", "gate", "combe", "fold", "keld", "keth", "stead", "garth", 
    "mine", "ward", "keep", "end", "peak", "husk", "rope", "quest", "reach", "bluff", "grove", "fist", "horn", "hold", "maw", "brow", "fog", "gift"
    //"reef", "sea", "isle", "harbor", "dock", "tide", "flood", "coast", "beach", "wave", "bay", "vic", "fish", "whale", "shore", "sail", "boat"
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

    auto lastCharacter = firstParticle->GetEnd() - 1;
    if(*lastCharacter == *secondParticle->Get() && *lastCharacter != 'w')
    {
        name << "-";
    }

    name << *secondParticle;
    name.Capitalize();

    return name;
}