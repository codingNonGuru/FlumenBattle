#pragma once

#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Singleton.h"

class Element;

namespace world::character
{
    class Character;
}

namespace battle
{
    namespace interface
    {
        class BattleCounter;
        class CharacterHoverInfo;
        class ActionInfoPanel;
        class TargetCursor;
    }

    class BattleInfoPanel;
    class BattleScene;
    class BattleEndMessage;
    class CharacterInfo;
    class CharacterDetailPanel;
    class Combatant;

    class BattleInterface : public core::Singleton <BattleInterface>
    {
        friend class core::Singleton <BattleInterface>;

        BattleScene* battleScene;

        Element *canvas;

        BattleInfoPanel *battleInfoPanel;

        CharacterDetailPanel *characterDetailPanel;

        interface::ActionInfoPanel *actionInfoPanel;

        BattleEndMessage *battleEndMessage;

        Array <CharacterInfo*> characterInfos;

        interface::CharacterHoverInfo *hoverInfo;

        Pool <interface::BattleCounter *> damageCounters;

        interface::TargetCursor *targetCursor;

        BattleInterface();

        void HandleTargetInitiated();

        void HandleTargetAbandoned();

    public:
        void Initialize();

        void Enable();

        void Disable();

        interface::BattleCounter *GetDamageCounter(int, Combatant *);

        void RemoveDamageCounter(interface::BattleCounter *);

        void EnableHoverExtension(CharacterInfo *);

        Combatant *GetHoveredCombatant();
    };
}