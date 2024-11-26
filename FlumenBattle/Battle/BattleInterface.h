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
        class ConditionPopup;
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

        container::Pool <interface::ConditionPopup *> conditionPopups;

        BattleInterface();

        void HandleTargetInitiated();

        void HandleTargetAbandoned();

        void HandleConditionApplied();

    public:
        void Initialize();

        void Enable();

        void Disable();

        void Update();

        interface::BattleCounter *GetDamageCounter(int, Combatant *);

        void RemoveDamageCounter(interface::BattleCounter *);

        void EnableHoverExtension(CharacterInfo *);

        Combatant *GetHoveredCombatant();

        void RemoveConditionPopup(interface::ConditionPopup *);
    };
}