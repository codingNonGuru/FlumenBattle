#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Time.hpp"

#include "BattleAnimator.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/CharacterAction.h"
#include "FlumenBattle/World/Character/Spell.h"
#include "FlumenBattle/Battle/Render/FireballEffect.h"
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

enum class BattleAnimations {MOVEMENT, FIREBALL};

static BattleAnimations currentAnimation;

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

static battle::render::FireballEffect *fireballEffect = nullptr;

const battle::render::FireballEffect &BattleAnimator::GetFireballEffect() const
{
    return *fireballEffect;
}

void BattleAnimator::SetupActionAnimation(Event onFinished)
{
    static const auto battleController = BattleController::Get();

    bool isActionSpell = battleController->GetSelectedCharacter()->GetSelectedAction()->Type == world::character::CharacterActions::CAST_SPELL;

    bool isSpellFireball = battleController->GetSelectedCharacter()->GetSelectedSpell()->Type == SpellTypes::FIRE_BALL;

    if(isActionSpell == true && isSpellFireball == true)
    {
        currentAnimation = BattleAnimations::FIREBALL;

        isAnimating = true;

        time = 0.0f;

        OnFinished = onFinished;

        fireballEffect = new render::FireballEffect(battleController->GetTargetedTile(), 2);    
    }
    else
    {
        onFinished.Invoke();
    }
}

void BattleAnimator::FollowPathMovement(Event onFinished)
{
    static const auto battleController = BattleController::Get();

    currentAnimation = BattleAnimations::MOVEMENT;

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

    switch(currentAnimation)
    {
        case BattleAnimations::MOVEMENT:
        {
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
            break;
        }
        case BattleAnimations::FIREBALL:
        {
            auto hasFinished = fireballEffect->Update();

            if(hasFinished == true)
            {
                isAnimating = false;

                delete fireballEffect;

                OnFinished.Invoke();
            }
            break;
        }
    }
}