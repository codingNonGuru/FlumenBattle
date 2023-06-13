#include "FlumenEngine/Core/Singleton.h"

namespace world
{
    class WorldScene;

    class WorldGenerator : public Singleton<WorldGenerator>
    {
        friend class WorldScene;

        void GenerateWorld(WorldScene &);
    };
}