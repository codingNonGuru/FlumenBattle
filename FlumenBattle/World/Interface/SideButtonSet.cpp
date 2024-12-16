#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/SimpleList.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "SideButtonSet.h"
#include "FlumenBattle/World/WorldScene.h"

using namespace world::interface;

static const auto FADED_LABEL_COLOR = Color::WHITE * 0.5f;

static const auto START_BASE_POSITION = Position2(0.0f, 0.0f);

static const auto END_BASE_POSITION = Position2(20.0f, 0.0f);

void SideButton::HandleConfigure()
{
    frame = ElementFactory::BuildElement <Element>
    (
        {drawOrder_ + 1, {this}, {"FrameRound", false}, Opacity(1.0f)}
    );
    frame->AdjustSizeToTexture();
    frame->Enable();

    label = ElementFactory::BuildText(
        {drawOrder_ + 2, {Position2(0.0f, 2.0f), frame}}, 
        {{"Small"}, Color::WHITE}
    );
    label->Enable();

    SetWidth(frame->GetWidth());
    SetHeight(frame->GetHeight());
}

void SideButton::HandleUpdate()
{
    if(menu == nullptr)
    {
        label->Disable();
        return;
    }

    label->Enable();

    if(menu->IsLocallyActive() == true)
    {
        offsetFactor += Time::GetDelta();
    }
    else
    {
        offsetFactor -= Time::GetDelta();
    }

    if(offsetFactor < 0.0f)
    {
        offsetFactor = 0.0f;
    }
    else if(offsetFactor > 0.1f)
    {
        offsetFactor = 0.1f;
    }

    float finalFactor = offsetFactor / 0.1f;

    frame->SetBasePosition(END_BASE_POSITION * finalFactor + START_BASE_POSITION * (1.0f - finalFactor));

    if(type == MajorMenus::POLITY)
    {
        if(WorldScene::Get()->GetPlayerPolity() != nullptr)
        {
            label->SetColor(Color::WHITE);
        }
        else
        {
            label->SetColor(FADED_LABEL_COLOR);
        }
    }
}

void SideButton::Setup(MajorMenus type, Element *menu)
{
    this->menu = menu;

    this->type = type;

    switch(type)
    {
    case MajorMenus::CAMP:
        label->Setup("C");
        break;
    case MajorMenus::INVENTORY:
        label->Setup("I");
        break;
    case MajorMenus::POLITY:
        label->Setup("P");
        break;
    case MajorMenus::QUEST:
        label->Setup("Q");
        break;
    case MajorMenus::REPUTATION:
        label->Setup("R");
        break;
    }
}

void SideButtonSet::HandleConfigure()
{
    buttonList = ElementFactory::BuildSimpleList
    (
        {drawOrder_, {Position2(-8.0f, 0.0f), ElementAnchors::MIDDLE_CENTER, ElementPivots::MIDDLE_LEFT, this}},
        (int)MajorMenus::MEMBER_COUNT,
        ListOrientations::VERTICAL,
        5.0f
    );
    buttonList->Enable();

    buttons.Initialize((int)MajorMenus::MEMBER_COUNT);

    for(int i = 0; i < buttons.GetCapacity(); ++i)
    {
        auto button = ElementFactory::BuildElement <SideButton>
        (
            {drawOrder_, {buttonList}}
        );

        *buttons.Add() = button;
    }
}

void SideButtonSet::Setup(const std::initializer_list <SideButtonData> datas)
{
    auto button = buttons.GetStart();

    for(auto &data : datas)
    {
        (*button)->Setup(data.Type, data.Menu);
        (*button)->Enable();

        button++;
    }
}

