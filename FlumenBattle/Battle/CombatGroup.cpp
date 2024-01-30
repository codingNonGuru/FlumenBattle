#include "FlumenBattle/Battle/CombatGroup.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/BattleTile.h"

using namespace battle;

CombatGroup::CombatGroup() : group(nullptr)
{
    combatants.Initialize(16);
}

void CombatGroup::Initialize(world::group::Group *_group, BattleTile *_tile)
{
    group = _group;
    tile = _tile;

    combatants.Reset();

    auto &characters = group->GetCharacters();
    for(auto &character : characters)
    {
        auto combatant = combatants.Allocate();

        auto combatantTile = tile->GetEmptyTileInRange(5);

        combatant->Initialize(this, &character, combatantTile);
    }
}

bool CombatGroup::IsAlive() const
{
    return group->IsAlive();
}