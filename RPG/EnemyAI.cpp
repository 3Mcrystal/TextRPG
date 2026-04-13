#include "EnemyAI.h"

#include <random>
#include <algorithm>
#include "Character.h"
#include "PlayerParty.h"
#include "StatusEffect.h"

//Helpers

static std::mt19937& RNG() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

static int Roll(int lo, int hi) {
    std::uniform_int_distribution<> d(lo, hi);
    return d(RNG());
}

//HP thresholds
static float HpFrac(const Character& c) {
    return static_cast<float>(c.GetHp()) / static_cast<float>(c.GetMaxHp());
}

//The lowest-HP player gets rekt
static std::shared_ptr<Character> LowestHpPlayer(PlayerParty& party) {
    auto alive = party.GetAliveMembers();
    if (alive.empty()) return nullptr;
    return *std::min_element(alive.begin(), alive.end(),
        [](const auto& a, const auto& b){ return a->GetHp() < b->GetHp(); });
}

//Random alive player
static std::shared_ptr<Character> RandomPlayer(PlayerParty& party) {
    auto alive = party.GetAliveMembers();
    if (alive.empty()) return nullptr;
    std::uniform_int_distribution<> d(0, (int)alive.size() - 1);
    return alive[d(RNG())];
}

// if (any player status effect) = true
static bool AnyPlayerHasStatus(PlayerParty& party) {
    for (auto& m : party.GetAliveMembers())
        if (m->GetStatus().IsActive()) return true;
    return false;
}

//Item roll
EnemyItem RollEnemyItem(const std::string& enemyType) {
    EnemyItem item;
    int r = Roll(1, 100);

    if (enemyType == "Goblin") {
        if (r <= 35) { item.type = EnemyItem::Type::HealPotion;    item.uses = 1; item.name = "Stolen Potion"; } //Heal Potion
    }
    else if (enemyType == "Orc") {
        if (r <= 40) { item.type = EnemyItem::Type::BerserkerRoot; item.uses = 1; item.name = "Berserker Root"; } //boosts next attack
    }
    else if (enemyType == "Skeleton") {
		if (r <= 25) { item.type = EnemyItem::Type::PoisonFlask;   item.uses = 1; item.name = "Poison Flask"; } //poisons on hit
    }
    else if (enemyType == "Troll") {
        if (r <= 60) { item.type = EnemyItem::Type::HealPotion;    item.uses = 2; item.name = "Troll Brew"; } //2 use, regen
    }
	//dragon don't need items, he burn, he tanks, he hits like a truck, it's a dragon (and with 200 chromosomes, he wouldn't be able to use them anyway)
    return item;
}

//More artificial than intelligent
//Goblin AI
AIDecision EnemyAI_Goblin(const AIContext& ctx) {
    AIDecision d;
    float selfHpFrac = HpFrac(*ctx.self);
    int   r          = Roll(1, 100);

    //Use potion if below 40% HP and has one
    if (ctx.item.IsAvailable() && ctx.item.type == EnemyItem::Type::HealPotion) {
        if (selfHpFrac < 0.40f) {
            d.action = AIAction::UseItem;
            d.target = ctx.self;
            return d;
        }
    }

    //Defend when last enemy and below 25% HP
    bool isLastEnemy = (ctx.aliveEnemies.size() == 1);
    if (isLastEnemy && selfHpFrac < 0.25f && r <= 45) {
        d.action = AIAction::Defend;
        return d;
    }

    //Focus lowest HP player
    d.target = LowestHpPlayer(ctx.party);
    if (!d.target) { d.action = AIAction::BasicAttack; return d; }

    //50% ClubSmash, 50% basic attack
    //70% skill chance if target is below 50%HP
    float targetHpFrac = HpFrac(*d.target);
    int skillChance = (targetHpFrac < 0.5f) ? 70 : 50;

    d.action = (r <= skillChance) ? AIAction::UseSkill : AIAction::BasicAttack;
    return d;
}

//Orc AI
AIDecision EnemyAI_Orc(const AIContext& ctx) {
    AIDecision d;
    float selfHpFrac = HpFrac(*ctx.self);
    int   r          = Roll(1, 100);

    //Use BerserkerRoot when above 50% HP
    if (ctx.item.IsAvailable() && ctx.item.type == EnemyItem::Type::BerserkerRoot) {
        if (selfHpFrac > 0.5f && ctx.roundNumber <= 3 && r <= 60) {
            d.action = AIAction::UseItem;
            d.target = ctx.self;
            return d;
        }
    }

    //Defend if below 30%
    if (selfHpFrac < 0.30f && r <= 35) {
        d.action = AIAction::Defend;
        return d;
    }

    //Target highest attack player
    auto alive = ctx.party.GetAliveMembers();
    if (alive.empty()) { d.action = AIAction::BasicAttack; return d; }
    d.target = *std::max_element(alive.begin(), alive.end(),
        [](const auto& a, const auto& b){ return a->GetAttack() < b->GetAttack(); });

    //55% AxeSwing, 45% basic attack
    d.action = (r <= 55) ? AIAction::UseSkill : AIAction::BasicAttack;
    return d;
}

//Skeleton AI
AIDecision EnemyAI_Skeleton(const AIContext& ctx) {
    AIDecision d;
    int r = Roll(1, 100);

    //Use Poison Flask first turn
    if (ctx.item.IsAvailable() && ctx.item.type == EnemyItem::Type::PoisonFlask) {
        if (ctx.roundNumber == 1 || r <= 30) {
            d.action = AIAction::UseItem;
            d.target = ctx.self; //applies to self
            return d;
        }
    }

    //Target lowest-defense player
    auto alive = ctx.party.GetAliveMembers();
    if (alive.empty()) { d.action = AIAction::BasicAttack; return d; }
    d.target = *std::min_element(alive.begin(), alive.end(),
        [](const auto& a, const auto& b){ return a->GetDefense() < b->GetDefense(); });

    //40% BoneSlash, 20% poisoned jab, 40% basic attack
    //one roll, if skill, second roll
    if      (r <= 60) d.action = AIAction::UseSkill;
    else              d.action = AIAction::BasicAttack;

    return d;
}

//Troll AI
AIDecision EnemyAI_Troll(const AIContext& ctx) {
    AIDecision d;
    float selfHpFrac = HpFrac(*ctx.self);
    int   r          = Roll(1, 100);
    int   hpMissing  = ctx.self->GetMaxHp() - ctx.self->GetHp();

    //Use Troll Brew when below 25% HP
    if (ctx.item.IsAvailable() && ctx.item.type == EnemyItem::Type::HealPotion) {
        if (selfHpFrac < 0.25f) {
            d.action = AIAction::UseItem;
            d.target = ctx.self;
            return d;
        }
    }

    if (hpMissing >= 15 && r <= 35) {
        d.action = AIAction::UseSkill;  //Regenerate
        d.target = ctx.self;
        return d;
    }

    //Defend at very low HP
    if (selfHpFrac < 0.20f && r <= 40) {
        d.action = AIAction::Defend;
        return d;
    }

    //Alternate target each round
    auto alive = ctx.party.GetAliveMembers();
    if (alive.empty()) { d.action = AIAction::BasicAttack; return d; }
    int targetIdx = (ctx.roundNumber - 1) % (int)alive.size();
    d.target = alive[targetIdx];

    //40% BoulderSmash, 60% basic attack
    d.action = (r <= 40) ? AIAction::UseSkill : AIAction::BasicAttack;
    return d;
}

//Dragon AI
AIDecision EnemyAI_Dragon(const AIContext& ctx) {
    AIDecision d;
    float selfHpFrac     = HpFrac(*ctx.self);
    int   r              = Roll(1, 100);
    int   aliveCount     = (int)ctx.party.GetAliveMembers().size();
    bool  mostHaveBurn   = false;

    //Check how many alive players burning
    int burnCount = 0;
    for (auto& m : ctx.party.GetAliveMembers())
        if (m->IsBurning()) burnCount++;
    if (aliveCount > 0 && burnCount >= aliveCount - 1) mostHaveBurn = true;

    //When below 40% HP
    if (selfHpFrac < 0.40f) {
        d.target = LowestHpPlayer(ctx.party);
        d.action = (r <= 60) ? AIAction::UseSkill : AIAction::BasicAttack; //TailSwipe
        return d;
    }

    //Always DragonBreath Turn 1
    if (ctx.roundNumber == 1) {
        d.action = AIAction::UseSkill;  //target=nullptr = AOE
        d.target = nullptr;
        return d;
    }

	//If nobody is burning, DragonBreath
    if (!mostHaveBurn && r <= 35) {
        d.action = AIAction::UseSkill;
        d.target = nullptr;
        return d;
    }

    //TailSwipe on lowest HP target
    d.target = LowestHpPlayer(ctx.party);
    if (r <= 50) {
        d.action = AIAction::UseSkill;
    } else {
        d.action = AIAction::BasicAttack;
        d.target = RandomPlayer(ctx.party);
    }
    return d;
}
