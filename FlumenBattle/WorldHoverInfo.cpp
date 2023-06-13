#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "WorldHoverInfo.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Polity.h"

using namespace world;

void WorldHoverInfo::HandleConfigure()
{
    auto color = Color::RED * 0.5f;
    //auto height = -float(size_.y) / 2.0f + 20.0f;
    strengthLabel = ElementFactory::BuildText(
        {Size(size_.x - 10, 150), drawOrder_ + 1, {Position2(0.0f, 0.0f), this}},
        {{"JSLAncient", "Large"}, color, "Pop 1"}
    );
    strengthLabel->SetAlignment(Text::Alignments::LEFT);
    strengthLabel->Enable();

    diseaseLabel = ElementFactory::BuildText(
        {Size(size_.x - 10, 150), drawOrder_ + 1, {Position2(0.0f, 35.0f), this}},
        {{"JSLAncient", "Large"}, color, "Deaths 0"}
    );
    diseaseLabel->SetAlignment(Text::Alignments::LEFT);
    diseaseLabel->Enable();

    industryLabel = ElementFactory::BuildText(
        {Size(size_.x - 10, 150), drawOrder_ + 1, {Position2(0.0f, 70.0f), this}},
        {{"JSLAncient", "Large"}, color, "Industry 0"}
    );
    industryLabel->SetAlignment(Text::Alignments::LEFT);
    industryLabel->Enable();
}

void WorldHoverInfo::HandleUpdate()
{
    auto tile = WorldController::Get()->GetHoveredTile();
    if(tile == nullptr)
        return;

    if(tile->GetSettlement() == nullptr)
        return;

    Phrase text = "Pop "; 
    text << tile->GetSettlement()->GetPolity()->GetPopulation();
    strengthLabel->Setup(text);

    text = "Deaths "; 
    text << tile->GetSettlement()->GetPolity()->GetDeathCount();
    diseaseLabel->Setup(text);

    text = "Industry "; 
    text << tile->GetSettlement()->GetPolity()->GetIndustrialPower();
    industryLabel->Setup(text);
}