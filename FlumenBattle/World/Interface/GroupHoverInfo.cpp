#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "GroupHoverInfo.h"
#include "FlumenBattle/World/Group/GroupSpotting.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Character/Character.h"

using namespace world::interface;

static const SpriteDescriptor BORDER_SPRITE = {"panel-border-007", true};

static const auto BORDER_OPACITY = Opacity(0.8f);

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto CLASS_LABEL_OFFSET = Position2(0.0f, 10.0f);

static const auto BASE_SIZE = Size(130, 150);

void GroupHoverInfo::HandleConfigure()
{
    size_ = BASE_SIZE;

    border = ElementFactory::BuildElement <Element>(
        {size_, drawOrder_ + 1, {this}, BORDER_SPRITE, BORDER_OPACITY}
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    classLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {CLASS_LABEL_OFFSET, ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}, 
        {{"Small"}, TEXT_COLOR, ""}
    );
    classLabel->Enable();

    originLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, classLabel}}, 
        {{"VerySmall"}, TEXT_COLOR, ""}
    );
    originLabel->Enable();

    compositionLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 15.0f), ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, originLabel}}, 
        {{"Small"}, TEXT_COLOR, ""}
    );
    compositionLabel->Enable();    
}

void GroupHoverInfo::Setup(const group::GroupSpotting &spotting)
{
    FollowWorldPosition(&spotting.VisualPosition, Cameras::WORLD, Scale2(0.0f, 1.0f), Scale2(0.0f, 10.0f));

    auto name = Phrase() << utility::GetClassName(spotting.GroupClass) << "s";
    classLabel->Setup(name);

    originLabel->Setup(spotting.OriginName);

    auto text = Phrase();

    auto index = 0;
    for(auto &character : spotting.Characters)
    {
        auto className = utility::GetClassName(character);
        text << className[0];

        if(index == 5)
        {
            text << "\n";
            index = 0;
        }
        else
        {
            text << " ";
            index++;
        }
    }

    compositionLabel->Setup(text);
}