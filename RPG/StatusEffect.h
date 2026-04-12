#pragma once
#include <string>

enum class StatusType {
    None,
    Poison,//-HP each turn
    Stun, //skip next turn
    Burn //-HP each turn but in red
    //Curse  //Best to keep outside because no combat effects
};

struct StatusEffect {
    StatusType  type = StatusType::None;
    int duration = 0;//turns remaining
    int power = 0;//damage

    bool IsActive() const { return type != StatusType::None && duration > 0; }
    std::string Name() const {
        switch (type) {
            case StatusType::Poison: return "Poison";
            case StatusType::Stun: return "Stun";
            case StatusType::Burn: return "Burn";
            default: return "";
        }
    }
};
