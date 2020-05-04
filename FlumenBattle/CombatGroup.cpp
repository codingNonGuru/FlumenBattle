#include "FlumenBattle/CombatGroup.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/Combatant.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleTile.h"

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
    for(auto character = characters.GetStart(); character != characters.GetEnd(); ++character)
    {
        auto combatant = combatants.Allocate();

        auto combatantTile = tile->GetEmptyTileInRange(5);
        combatantTile->Combatant = combatant;

        combatant->Initialize(this, character, combatantTile);
    }
}

bool CombatGroup::IsAlive() const
{
    return group->IsAlive();
}