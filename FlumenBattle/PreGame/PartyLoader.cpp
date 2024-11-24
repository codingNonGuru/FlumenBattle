#include <nlohmann/json.hpp>

#include "FlumenEngine/Core/File.hpp"
#include "FlumenEngine/Core/FileManager.hpp"

#include "PartyLoader.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/PreGame/Types.h"
#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/Types.hpp"

using namespace pregame;

using json = nlohmann::json;

static const auto DEFAULT_PRESET_NAME = "PartyPreset.json";

#define CONFIG_NOT_FOUND_MESSAGE "Fatal error: Engine can not find party preset file!\n"

static nlohmann::json_abi_v3_11_3::basic_json<> configObject;

container::Array <MemberData> memberDatas = [] 
{
    static const auto size = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_CHARACTERS_PER_GROUP).Integer;

    return container::Array <MemberData> (size);
} ();

const container::Array <MemberData> *PartyLoader::LoadPreset(const char *fileName)
{
    auto file = engine::FileManager::Get()->GetFile(fileName);

    assert((file != nullptr) && (CONFIG_NOT_FOUND_MESSAGE));

    auto fileContent = file->ReadContent();

    configObject = json::parse(fileContent);

    auto &memberArray = configObject.at("PartyMembers");

    if(memberArray.is_array() == false)
        return;

    std::string classString, raceString;

    memberDatas.Reset();
    for(auto member : memberArray)
    {
        member.at("Class").get_to(classString);

        member.at("Race").get_to(raceString);

        auto memberData = memberDatas.Allocate();

        if(CompareStrings(classString.c_str(), "wizard") == 0)
        {
            memberData->Class = world::character::CharacterClasses::WIZARD;
        }
        else if(CompareStrings(classString.c_str(), "fighter") == 0)
        {
            memberData->Class = world::character::CharacterClasses::FIGHTER;
        }
        else if(CompareStrings(classString.c_str(), "cleric") == 0)
        {
            memberData->Class = world::character::CharacterClasses::CLERIC;
        }
        else if(CompareStrings(classString.c_str(), "ranger") == 0)
        {
            memberData->Class = world::character::CharacterClasses::RANGER;
        }

        if(CompareStrings(raceString.c_str(), "human") == 0)
        {
            memberData->Race = RaceTypes::HUMAN;
        }
        else if(CompareStrings(raceString.c_str(), "dwarf") == 0)
        {
            memberData->Race = RaceTypes::DWARF;
        }
        else if(CompareStrings(raceString.c_str(), "gnome") == 0)
        {
            memberData->Race = RaceTypes::GNOME;
        }
        else if(CompareStrings(raceString.c_str(), "halfling") == 0)
        {
            memberData->Race = RaceTypes::HALFLING;
        }
        else if(CompareStrings(raceString.c_str(), "goblin") == 0)
        {
            memberData->Race = RaceTypes::GOBLIN;
        }
        else if(CompareStrings(raceString.c_str(), "orc") == 0)
        {
            memberData->Race = RaceTypes::ORC;
        }
        else if(CompareStrings(raceString.c_str(), "elf") == 0)
        {
            memberData->Race = RaceTypes::ELF;
        }
    }

    return &memberDatas;
}

const container::Array <MemberData> *PartyLoader::LoadDefaultPreset()
{
    return LoadPreset(DEFAULT_PRESET_NAME);
}