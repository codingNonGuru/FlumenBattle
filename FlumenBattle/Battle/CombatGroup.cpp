#include "FlumenBattle/Battle/CombatGroup.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Config.h"

using namespace battle;

CombatGroup::CombatGroup() : group(nullptr)
{
    static const auto MAXIMUM_CHARACTERS_PER_GROUP = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_CHARACTERS_PER_GROUP).Integer;

    combatants.Initialize(MAXIMUM_CHARACTERS_PER_GROUP);
}

void CombatGroup::Initialize(world::group::GroupCore *_group, BattleTile *_tile)
{
    group = _group;
    tile = _tile;

    combatants.Reset();

    auto &characters = group->GetCharacters();
    for(auto &character : characters)
    {
        auto combatant = combatants.Allocate();

        auto combatantTile = tile->GetEmptyTileInRange(4);

        combatant->Initialize(this, &character, combatantTile);
    }
}

bool CombatGroup::IsAlive() const
{
    return group->IsAlive();
}