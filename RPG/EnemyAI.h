#pragma once

#include <memory>
#include <vector>
#include <string>

class Character;
class PlayerParty;

//Item enemy can hold and use
struct EnemyItem {
    enum class Type { None, HealPotion, PoisonFlask, BerserkerRoot };
    Type type = Type::None;
    int uses = 1;
    std::string name = "";

    bool IsAvailable() const { return type != Type::None && uses > 0; }
    void Consume()           { if (uses > 0) --uses; }
};

//Roll if enemy has item, or gets fucked
EnemyItem RollEnemyItem(const std::string& enemyType);

//AI decision context
struct AIContext {
    std::shared_ptr<Character> self;
    std::vector<std::shared_ptr<Character>> aliveEnemies;   //All living enemies (self too)
    PlayerParty& party;
    int roundNumber;
    EnemyItem& item;  //Enemy's held item (if any)
};

// Result
enum class AIAction {
    BasicAttack,
    UseSkill,
    Defend,
    UseItem,
};

struct AIDecision {
    AIAction action  = AIAction::BasicAttack;
    std::shared_ptr<Character> target;  // who to hit / heal
};

//One func / enemy type
AIDecision EnemyAI_Goblin (const AIContext& ctx);
AIDecision EnemyAI_Orc (const AIContext& ctx);
AIDecision EnemyAI_Skeleton (const AIContext& ctx);
AIDecision EnemyAI_Troll (const AIContext& ctx);
AIDecision EnemyAI_Dragon (const AIContext& ctx);
