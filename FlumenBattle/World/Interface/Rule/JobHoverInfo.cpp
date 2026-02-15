#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/ProgressBar.h"

#include "JobHoverInfo.h"
#include "JobItem.h"
#include "FlumenBattle/World/Settlement/Job.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Polity/HumanMind.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto HIGHLIGHT_COLOR = Color::RED * 0.9f;

static const SpriteDescriptor BORDER_SPRITE = {"panel-border-007", true};

static const auto MOUSE_FOLLOW_OFFSET = Position2(0.0f, 10.0f);

static const auto BASE_SIZE = Size(100, 100);

static const auto OPACITY = Opacity(0.7f);

static const auto BORDER_OPACITY = Opacity(0.7f);

void JobHoverInfo::HandleConfigure()
{
    size_ = BASE_SIZE;

    border = ElementFactory::BuildElement <Element>(
        {size_, drawOrder_ + 1, {this}, BORDER_SPRITE, BORDER_OPACITY}
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    FollowMouse(MOUSE_FOLLOW_OFFSET);

    progressBar = ElementFactory::BuildProgressBar <ProgressBar>
    (
        {Size(60, 24), drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::MIDDLE_CENTER, ElementPivots::MIDDLE_CENTER, this}, {"BaseBar", true}},
        {"BaseFillerRed", {6.0f, 6.0f}}
    );
    progressBar->Enable();

    progressLabel = ElementFactory::BuildRichText(
        {drawOrder_ + 1, {Position2(0.0f, -20.0f), ElementAnchors::MIDDLE_CENTER, ElementPivots::MIDDLE_CENTER, this}}, 
        {{"Small"}, TEXT_COLOR, HIGHLIGHT_COLOR, "<2>W<1>ool"}
    );
    progressLabel->Enable();
}

void JobHoverInfo::HandleUpdate()
{
    if(hoveredItem == nullptr)
        return;

    if(hoveredItem->IsHovered() == false)
    {
        hoveredItem = nullptr;
        Disable();
        return;
    }

    auto job = polity::HumanMind::Get()->GetJobFromInstruction(this->hoveredJob);

    auto progress = (float)job->GetProgress() / (float)settlement::ResourceHandler::CYCLE_LENGTH;
    progressBar->SetProgress(progress);

    auto status = job->GetStatus();
    if(status == settlement::JobStatus::OBTAINING_MATERIALS)
    {
        progressLabel->Setup("<2>O<1>btaining");   
    }
    else if(status == settlement::JobStatus::PRODUCING)
    {
        progressLabel->Setup("<2>P<1>roducing");   
    }
    else if(status == settlement::JobStatus::DELIVERING_GOODS)
    {
        progressLabel->Setup("<2>D<1>elivering");   
    }
}

void JobHoverInfo::Setup(JobItem *item, polity::WorkInstruction *instruction)
{
    this->hoveredItem = item;

    this->hoveredJob = instruction;
}