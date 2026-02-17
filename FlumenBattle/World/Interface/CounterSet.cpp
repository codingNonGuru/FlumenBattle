#include "FlumenEngine/Interface/ElementFactory.h"

#include "CounterSet.h"
#include "Counter.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Settlement/PopHandler.h"
#include "FlumenBattle/RaceFactory.h"

using namespace world::interface;

#define MAXIMUM_SIZE 8

void CounterSet::HandleConfigure()
{
    items.Initialize(MAXIMUM_SIZE);

    list = ElementFactory::BuildSimpleList
    (
        { 
            drawOrder_, 
            {this}, 
            {false},
            Opacity(0.0f)
        },
        items.GetCapacity(),
        ListOrientations::HORIZONTAL,
        -3.0f
    );
    list->Enable();

    for(int i = 0; i < items.GetCapacity(); ++i)
    {
        auto item = ElementFactory::BuildElement <SetItem>
        (
            { 
                drawOrder_ + 1, 
                {Position2(), list}, 
                {"Radish", false}
            }
        );

        *items.Add() = item;
    }
}

void CounterSet::Setup(const container::Array <ResourceData> &setData)
{
    for(auto &item : items)
    {
        item->Disable();
    }

    auto item = items.GetStart();
    for(auto &data : setData)
    {
        auto textureName = settlement::ResourceFactory::Get()->CreateType(data.Type)->TextureName;

        (*item)->Setup(textureName, data.Amount);
        (*item)->Enable();

        item++;
    }

    list->UpdateSize();

    size_ = list->GetSize();

    UpdatePosition();
}

void CounterSet::Setup(const container::Array <settlement::RaceGroup> &setData)
{
    for(auto &item : items)
    {
        item->Disable();
    }

    auto item = items.GetStart();
    for(auto &data : setData)
    {
        auto textureName = RaceFactory::BuildRace(data.Race)->TextureName;

        (*item)->Setup(textureName, data.Size);
        (*item)->Enable();

        item++;
    }

    list->UpdateSize();

    size_ = list->GetSize();

    UpdatePosition();
}

void SetItem::HandleConfigure()
{
    counter = ElementFactory::BuildElement <Counter>
    (
        {drawOrder_ + 1, {Position2(15.0f, 20.0f), ElementAnchors::MIDDLE_CENTER, ElementPivots::MIDDLE_CENTER, this}, {"WhiteDotBackdrop", false}}
    );
    counter->Setup(&value, Scale2(0.85f), "Medium");
    counter->Enable();
}

void SetItem::Setup(Word textureName, int amount)
{
    value = amount;

    SetTexture(textureName);
}
