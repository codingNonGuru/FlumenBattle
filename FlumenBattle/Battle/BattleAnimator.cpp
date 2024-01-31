#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Time.hpp"

#include "BattleAnimator.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/Utility/Pathfinder.h"

using namespace battle;

struct FollowPathData
{
    Position2 StartPosition;

    Position2 EndPosition;

    Combatant *Combatant;

    BattleTile *CurrentTile;

    BattleTile *NextTile;
} followPathData;

auto jumpsLeft = 0;

static constexpr auto JUMP_TIME_LENGTH = 0.5f;

BattleAnimator::BattleAnimator()
{
    BattleScene::Get()->OnUpdate += {this, &BattleAnimator::Update};
}

BattleTile *BattleAnimator::GetNextTile()
{
    static const auto battleController = BattleController::Get();

    auto &pathData = battleController->GetPathData();

    auto index = 0;
    for(auto &tile : pathData.Tiles)
    {
        if(tile == followPathData.CurrentTile)
        {
            if(index == pathData.Length - 1)
                return nullptr;
            else 
                return *pathData.Tiles[index + 1];
        }

        index++;
    }
}

void BattleAnimator::Advance()
{
    followPathData.CurrentTile = followPathData.NextTile;

    followPathData.NextTile = GetNextTile();

    followPathData.StartPosition = followPathData.CurrentTile->Position;

    followPathData.EndPosition = followPathData.NextTile->Position;
}

float BattleAnimator::GetAnimationLength() const
{
    return totalLength;
}

void BattleAnimator::FollowPathMovement(Event onFinished)
{
    static const auto battleController = BattleController::Get();

    isAnimating = true;

    time = 0.0f;

    followPathData.Combatant = battleController->GetSelectedCombatant();

    followPathData.CurrentTile = followPathData.Combatant->GetTile();

    followPathData.NextTile = GetNextTile();

    followPathData.StartPosition = followPathData.CurrentTile->Position;

    followPathData.EndPosition = followPathData.NextTile->Position;

    OnFinished = onFinished;

    auto &pathData = battleController->GetPathData();

    auto jumpCount = 0;
    for(auto &tile : pathData.Tiles)
    {
        if(tile == battleController->GetTargetedTile())
        {
            break;
        }

        jumpCount++;
    }

    jumpsLeft = jumpCount;

    totalLength = JUMP_TIME_LENGTH;

    jumpLength = JUMP_TIME_LENGTH / (float)jumpCount;
}

void BattleAnimator::Update()
{
    if(isAnimating == false)
        return;

    time += Time::GetDelta();

    auto timeFactor = time / jumpLength;

    auto oldPosition = followPathData.Combatant->GetPosition();

    auto newPosition = followPathData.StartPosition * (1.0f - timeFactor) + followPathData.EndPosition * timeFactor;
    followPathData.Combatant->SetPosition(newPosition);

    auto direction = newPosition - oldPosition;

    auto newRotation = atan2(direction.y, direction.x);
    followPathData.Combatant->SetRotation(newRotation);

    if(time > jumpLength)
    {
        jumpsLeft--;

        if(jumpsLeft == 0)
        {
            isAnimating = false;

            OnFinished.Invoke();
        }
        else
        {
            time = 0.0f;

            Advance();
        }
    }
}