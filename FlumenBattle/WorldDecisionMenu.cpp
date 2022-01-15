#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"

#include "FlumenBattle/WorldDecisionMenu.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupAction.h"

using namespace world;

void WorldDecisionMenu::HandleConfigure() 
{
    travelLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 30.0f), this}},
        {{"JSLAncient", "Medium"}, Color::RED * 0.5f, "Press [T] to travel to location."}
    );
    travelLabel->Enable();

    searchLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 0.0f), this}},
        {{"JSLAncient", "Medium"}, Color::RED * 0.5f, "Press [S] to search for enemies."}
    );
    searchLabel->Enable();

    restLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, -30.0f), this}},
        {{"JSLAncient", "Medium"}, Color::RED * 0.5f, "Press [R] to rest."}
    );
    restLabel->Enable();

    statusLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, -65.0f), this}},
        {{"JSLAncient", "Large"}, Color::RED * 0.5f, "Current action: None"}
    );
    statusLabel->Enable();
}

void WorldDecisionMenu::HandleUpdate() 
{
    Phrase text = "Current action: ";
    
    auto group = WorldScene::Get()->GetPlayerGroup();
    if(group->GetAction())
    {
        switch(group->GetAction()->Type)
        {
            case GroupActions::TRAVEL:
                text << "Travelling to " << (int)group->GetDestination() << ".";
                break;
            case GroupActions::TAKE_LONG_REST:
                text << "Taking a long rest.";
                break;
            case GroupActions::TAKE_SHORT_REST:
                text << "Taking a short rest.";
                break;
            case GroupActions::SEARCH:
                text << "Searching for enemies.";
                break;
            case GroupActions::FIGHT:
                text << "Fighting.";
                break;
        }

        int duration = group->GetRemainingActionDuration() / 6;
        
        if(group->GetRemainingActionDuration() % 6 > 2)
            duration += 1;

        if(duration < 0)
            duration = 0;

        text << " " << duration << " hours left.";
    }
    else
    {
        text << "None";
    }

    statusLabel->Setup(text);
}