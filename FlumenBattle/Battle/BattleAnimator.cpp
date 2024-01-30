#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Time.hpp"

#include "BattleAnimator.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Battle/Combatant.h"

using namespace battle;

struct FollowPathData
{
    Position2 StartPosition;

    Position2 EndPosition;

    Combatant *Combatant;
} followPathData;

BattleAnimator::BattleAnimator()
{
    BattleScene::Get()->OnUpdate += {this, &BattleAnimator::Update};
}

void BattleAnimator::FollowPathMovement(Event onFinished)
{
    isAnimating = true;

    time = 0.0f;

    followPathData.StartPosition = BattleController::Get()->GetSelectedCombatant()->GetTile()->Position;

    followPathData.EndPosition = BattleController::Get()->GetTargetedTile()->Position;

    followPathData.Combatant = BattleController::Get()->GetSelectedCombatant();

    OnFinished = onFinished;
}

void BattleAnimator::Update()
{
    if(isAnimating == false)
        return;

    time += Time::GetDelta();

    auto updatedPosition = followPathData.StartPosition * (1.0f - time) + followPathData.EndPosition * time;
    followPathData.Combatant->SetPosition(updatedPosition);

    if(time > 1.0f)
    {
        isAnimating = false;

        OnFinished.Invoke();
    }
}