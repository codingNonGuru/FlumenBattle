#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;

namespace pregame
{
    struct MemberData;

    class PartySetupPanel;

    class PartySetupMenu : public Element
    {
        friend class PreGameState;

        Text *titleLabel;

        Text *startGameLabel;

        Text *loadPresetLabel;

        Text *savePresetLabel;

        Text *backLabel;

        Element *border;

        PartySetupPanel *setupPanel;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void OnStartGamePressed();

        void OnBackPressed();
    };

    class PartyMemberItem : public Element
    {
        Element *border;

        Text *classLabel;

        Text *raceLabel;

        void HandleConfigure() override;

    public:
        void Setup(MemberData);
    };

    class PartySetupPanel : public Element
    {
        Element *border;

        LayoutGroup *itemLayout;

        container::Array <PartyMemberItem *> items;

        void HandleConfigure() override;

    public:
        void Setup(const container::Array <MemberData> &);
    };
}