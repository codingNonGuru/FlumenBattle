#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupType.h"
#include "FlumenBattle/World/Group/GroupAllocator.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/WorldTile.h"

Array <Color> colors = {Color::RED, Color::GREEN, Color::CYAN, Color::RED * 0.5f, Color::GREEN * 0.5f, Color::GREEN, Color::BLUE};

int nameIndex = 0;

namespace world::group
{
    Color GetColor()
    {
        static Integer colorIndex = 0;
        auto color = colors[colorIndex];

        colorIndex++;
        if(colorIndex == colors.GetSize())
            colorIndex = 0;

        return color;
    }

    Group* GroupFactory::Create(GroupBuildData buildData)
    {
        auto group = new Group(); 

        auto type = GroupTypeFactory::BuildGroupType(buildData.Type);

        auto size = type->Size; //utility::GetRandom(4, 7);

        auto color = GetColor();

        if(buildData.Type == GroupClasses::MERCHANT)
        {
            group->muleCount = 2;
        }
        else
        {
            group->muleCount = 0;
        }

        group->Initialize(type, size, color, buildData.Race);

        group->SetHome(buildData.Home);

        if(buildData.Type == GroupClasses::BANDIT)
        {
            auto nearbyTiles = buildData.Home->GetLocation()->GetNearbyTiles(2);

            while(true)
            {
                auto randomTile = *nearbyTiles.Tiles.GetRandom();
                if(randomTile != buildData.Home->GetLocation())
                {
                    group->SetTile(randomTile);
                    break;
                }
            }
        }
        else if(buildData.Type == GroupClasses::PATROL)
        {
            auto nearbyTiles = buildData.Home->GetLocation()->GetNearbyTiles(2);

            auto randomTile = *nearbyTiles.Tiles.GetRandom();
            
            group->SetTile(randomTile);
        }
        else
        {
            group->SetTile(buildData.Home->GetLocation());
        }

        nameIndex++;

        return group;
    }
}