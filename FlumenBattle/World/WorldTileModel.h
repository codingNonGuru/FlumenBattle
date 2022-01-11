#pragma once

#include "FlumenEngine/Render/Model.hpp"

#include "FlumenEngine/Core/Singleton.h"

class Shader;

namespace world 
{
    class WorldScene;

    class WorldTileModel : public Model, public Singleton <WorldTileModel>
    {
        friend class Singleton;

        Shader *shader;

        Shader *groupShader;

        WorldScene *worldScene;

        WorldTileModel();

    public:
        void Initialize() override;

        void Render();

        void CreateCamera();

        void HandleCharacterSelected();
    };
}