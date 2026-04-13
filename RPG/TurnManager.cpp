#include <vector>
#include <memory>
#include <random>
#include <iostream>
#include <algorithm>
#include <string>
#include <map>

#include "TurnManager.h"
#include "PlayerParty.h"
#include "Encounter.h"
#include "Character.h"
#include "InputManager.h"
#include "AsciiArt.h"
#include "SmokeBomb.h"
#include "QuestLog.h"
#include "Potion.h"

#include "Warrior.h"
#include "Mage.h"
#include "Ranger.h"

#include "Goblin.h"
#include "Orc.h"
#include "Skeleton.h"
#include "Troll.h"
#include "Dragon.h"
#include "Beggar.h"
#include "Merchant.h"
#include "EnemyAI.h"

TurnManager::TurnManager() {}
TurnManager::~TurnManager() {}

//Helpers

static std::shared_ptr<Character> ChooseRandomAlive(
    const std::vector<std::shared_ptr<Character>>& list)
{
    std::vector<std::shared_ptr<Character>> alive;
    for (auto& e : list) if (e->IsAlive()) alive.push_back(e);
    if (alive.empty()) return nullptr;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, (int)alive.size() - 1);
    return alive[dis(gen)];
}

static void PressEnter(InputManager& input) {
    std::cout << "  [ Press Enter to continue... ]";
    input.GetLine();
}

static void PrintEnemyStatus(const Encounter& enc) {
    AsciiArt::PrintThinDivider();
    std::cout << "  ENEMIES\n";
    int i = 0;
    for (auto& e : enc.GetEnemies()) {
        if (!e->IsAlive()) { ++i; continue; }
        std::cout << "[" << i++ << "] ";
        AsciiArt::PrintHpBar(e->GetName(), e->GetHp(), e->GetMaxHp());
        if (e->GetStatus().IsActive())
            std::cout << "     [" << e->GetStatus().Name()
                      << " " << e->GetStatus().duration << "t]\n";
    }
    AsciiArt::PrintThinDivider();
}

static void PrintEnemyArt(const std::shared_ptr<Character>& e) {
    if      (std::dynamic_pointer_cast<Dragon>(e))   AsciiArt::PrintDragon();
    else if (std::dynamic_pointer_cast<Troll>(e))    AsciiArt::PrintTroll();
    else if (std::dynamic_pointer_cast<Goblin>(e))   AsciiArt::PrintGoblin();
    else if (std::dynamic_pointer_cast<Orc>(e))      AsciiArt::PrintOrc();
    else if (std::dynamic_pointer_cast<Skeleton>(e)) AsciiArt::PrintSkeleton();
}

static void DoPlayerAttack(std::shared_ptr<Character>& actor,
                           std::shared_ptr<Character>& target)
{
    int  dmg  = actor->GetAttack();
    bool crit = actor->RollCrit();
    if (crit) dmg *= 2;
    std::cout << actor->GetName() << " attacks " << target->GetName();
    if (crit) std::cout << " [CRITICAL HIT!]";
    std::cout << " for " << dmg << " damage!\n";
    target->TakeDamage(dmg);
}

//Roll for enemy loot drop
static void RollEnemyDrop(PlayerParty& party, const std::string& enemyName) {
    static std::random_device rd; static std::mt19937 gen(rd());
    std::uniform_int_distribution<> roll(1, 100);
    int r = roll(gen);
    if (r <= 20) {
        std::cout << "  " << enemyName << " dropped a Potion!\n";
        party.GetInventory().AddItem(std::make_shared<Potion>(), 1);
    } else if (r <= 30) {
        std::cout << "  " << enemyName << " dropped a Gold coin!\n";
        party.AddGold(1);
    }
}

//Name enemies when duplicates
static void NameEnemies(Encounter& encounter) {
    auto& enemies = const_cast<std::vector<std::shared_ptr<Character>>&>(
        encounter.GetEnemies());
    //Count occurrences
    std::vector<std::string> seen;
    for (auto& e : enemies) {
        std::string base = e->GetName();
        int count = 0;
        for (auto& s : seen) if (s == base) count++;
        if (count > 0 || std::count_if(enemies.begin(), enemies.end(),
            [&](auto& x){ return x->GetName() == base; }) > 1) {
        }
        seen.push_back(base);
    }
    //Suffix name
    for (auto& e : enemies) {
        std::string base = e->GetName();
        int total = (int)std::count_if(enemies.begin(), enemies.end(),
            [&](auto& x){ return x->GetName().substr(0, base.size()) == base; });
        (void)total;
    }
    //Track per-name counter
    std::vector<std::string> nameCount;
    for (auto& e : enemies) {
        std::string base = e->GetName();
        //Strip suffix first
        int cnt = (int)std::count(nameCount.begin(), nameCount.end(), base);
        if (std::count_if(enemies.begin(), enemies.end(),
            [&](auto& x){ return x->GetName() == base; }) > 1) {
            e->SetName(base + " " + std::string(1, (char)('A' + cnt)));
        }
        nameCount.push_back(base);
    }
}

//Skill submenu
static bool HandleSkill(std::shared_ptr<Character>& actor,
                        std::vector<std::shared_ptr<Character>>& ae,
                        InputManager& input)
{
    if (auto w = std::dynamic_pointer_cast<Warrior>(actor)) {
        std::cout << "Choose skill:\n";
        std::cout << " 1) Shield Bash  (8 MP) - damage + 50% stun\n";
        std::cout << " 2) Rally        (6 MP) - restore HP\n> ";
        std::string sc = input.GetLine();
        if (sc == "2") { w->Rally(); return true; }
        // default 1: needs target
        if (ae.empty()) { std::cout << "No targets!\n"; return false; }
        int idx = input.RequestTragetIndex((int)ae.size());
        w->ShieldBash(*ae[idx]);
        return true;
    }
    if (auto m = std::dynamic_pointer_cast<Mage>(actor)) {
        std::cout << "Choose skill:\n";
        std::cout << " 1) Fireball     (10 MP) - single target fire\n";
        std::cout << " 2) Arcane Blast (18 MP) - heavy magic hit\n> ";
        std::string sc = input.GetLine();
        if (ae.empty()) { std::cout << "No targets!\n"; return false; }
        int idx = input.RequestTragetIndex((int)ae.size());
        if (sc == "2") m->ArcaneBlast(*ae[idx]);
        else           m->Fireball(*ae[idx]);
        return true;
    }
    if (auto r = std::dynamic_pointer_cast<Ranger>(actor)) {
        std::cout << "Choose skill:\n";
        std::cout << " 1) Precision Shot (7 MP)  - guaranteed critical hit\n";
        std::cout << " 2) Poison Arrow   (10 MP) - damage + 3 turns poison\n> ";
        std::string sc = input.GetLine();
        if (ae.empty()) { std::cout << "No targets!\n"; return false; }
        int idx = input.RequestTragetIndex((int)ae.size());
        if (sc == "2") r->PoisonArrow(*ae[idx]);
        else           r->PrecisionShot(*ae[idx]);
        return true;
    }
    std::cout << actor->GetName() << " has no skills!\n";
    return false;
}

EncounterResult TurnManager::ExecuteEncounter(PlayerParty& party, Encounter& encounter,
                                   InputManager& input, QuestLog* quests)
{
    //Beggar
    if (encounter.IsBeggarEncounter()) {
        AsciiArt::PrintBeggar();
        std::cout << "A beggar approaches you, asking for some gold.\n";
        static std::random_device rd; static std::mt19937 gen(rd());

        bool done = false;
        while (!done) {
            std::cout << "\nWhat do you do?\n 1) Attack\n 2) Give (one Gold)\n 3) Ignore\n> ";
            std::string choice = input.GetLine();

            if (choice == "1" || choice == "attack" || choice == "Attack") {
                std::cout << "You attack the beggar!\n";
                std::uniform_int_distribution<> loot(1,100);
                if (loot(gen) <= 50) { std::cout << "Found a coin.\n"; party.AddGold(1); }
                else std::cout << "Found nothing.\n";
                std::uniform_int_distribution<> cons(1,100); int c = cons(gen);
                if (c <= 49) { std::cout << "The Beggar collapses.\n"; }
                else if (c <= 99) {
                    auto a = party.GetAliveMembers();
                    if (!a.empty()) {
                        std::uniform_int_distribution<> idx(0,(int)a.size()-1);
                        auto t = a[idx(gen)];
                        std::cout << "The Beggar bites " << t->GetName() << " for 5 damage!\n";
                        t->TakeDamage(5);
                    }
                    std::cout << "Shame on you!\n";
                } else {
                    auto a = party.GetAliveMembers();
                    if (!a.empty()) {
                        std::uniform_int_distribution<> idx(0,(int)a.size()-1);
                        auto t = a[idx(gen)];
                        AsciiArt::PrintSkull();
                        std::cout << t->GetName() << " has been cursed!\n";
                        t->ApplyCurse(3);
                    }
                }
                done = true;
            }
            else if (choice == "2" || choice == "give" || choice == "Give") {
                if (party.GetGold() < 1) { std::cout << "Not enough gold!\n"; continue; }
                std::cout << "\"Thank you, kind stranger...\"\n";
                party.AddGold(-1); done = true;
            }
            else if (choice == "3" || choice == "ignore" || choice == "Ignore") {
                std::cout << "You walk on...\n"; done = true;
            }
            else std::cout << "Unknown command.\n";
        }
        return EncounterResult::Fled;
    }

    //Merchant
    if (encounter.IsMerchantEncounter()) {
        AsciiArt::PrintMerchant();
        Merchant merchant;
        merchant.Interact(party, input);
        return EncounterResult::Fled;
    }

    //Combat
    bool isBoss  = encounter.IsBossEncounter();
    bool isFinal = encounter.IsFinalBoss();

    //Name duplicate enemies (Goblin A, Goblin B…)
    NameEnemies(encounter);

    if (isFinal) {
        AsciiArt::PrintDragon();
        std::cout << "\n\"So... mortals dare challenge me?\"\n";
        std::cout << "\"I have burned cities. I have eaten kings.\"\n";
        std::cout << "\"You will be ASH.\"\n\n";
        PressEnter(input);
    } else if (isBoss) {
        AsciiArt::PrintBossWarning();
        for (auto& e : encounter.GetEnemies()) PrintEnemyArt(e);
        PressEnter(input);
    } else {
        AsciiArt::PrintCombat();
        for (auto& e : encounter.GetEnemies()) PrintEnemyArt(e);
    }

    //Roll items for each enemy
    std::map<Character*, EnemyItem> enemyItems;
    for (auto& e : encounter.GetEnemies()) {
        std::string baseName = e->GetName();
        auto space = baseName.find(' ');
        if (space != std::string::npos) baseName = baseName.substr(0, space);
        EnemyItem item = RollEnemyItem(baseName);
        if (item.IsAvailable())
            std::cout << "  [" << e->GetName() << " is carrying a " << item.name << "]\n";
        enemyItems[e.get()] = item;
    }

    int roundNum = 0;
    using EPtr = std::shared_ptr<Character>;

    while (true) {
        roundNum++;
        AsciiArt::PrintDivider();
        std::cout << "  ROUND " << roundNum << "\n";
        AsciiArt::PrintThinDivider();

        //Build and sort turn order
        std::vector<EPtr> order;
        for (auto& p : party.GetMembers()) if (p->IsAlive()) order.push_back(p);
        for (auto& e : encounter.GetEnemies()) if (e->IsAlive()) order.push_back(e);
        std::sort(order.begin(), order.end(), [](const EPtr& a, const EPtr& b) {
            return a->GetSpeed() > b->GetSpeed();
        });

        for (auto& actor : order) {
            if (!actor->IsAlive()) continue;
            if (party.IsDefeated() || encounter.IsAllEnemiesDefeated()) break;

            //Save stun state BEFORE ticking
            bool wasStunned = actor->IsStunned();

            //Tick status at start of turn
            actor->TickStatus();
            if (!actor->IsAlive()) continue;
            if (wasStunned) continue; //stun = void turn

            actor->ResetDefend();

            bool isPlayer = false;
            for (auto& p : party.GetMembers())
                if (p == actor) { isPlayer = true; break; }

            if (isPlayer) {
                //Show HP, MP, enemy status
                AsciiArt::PrintThinDivider();
                AsciiArt::PrintHpBar(actor->GetName(), actor->GetHp(), actor->GetMaxHp());
                if (actor->GetMaxMp() > 0)
                    AsciiArt::PrintMpBar(actor->GetName(), actor->GetMp(), actor->GetMaxMp());
                PrintEnemyStatus(encounter);

                bool turnDone = false;
                while (!turnDone) {
                    std::string cmd = input.RequestActionFor(actor->GetName());

                    if (cmd == "attack") {
                        std::vector<EPtr> ae;
                        for (auto& e : encounter.GetEnemies()) if (e->IsAlive()) ae.push_back(e);
                        if (ae.empty()) continue;
                        int idx = input.RequestTragetIndex((int)ae.size());
                        DoPlayerAttack(actor, ae[idx]);
                        turnDone = true;
                    }
                    else if (cmd == "defend") {
                        actor->Defend();
                        std::cout << actor->GetName() << " takes a defensive stance! (halved damage)\n";
                        turnDone = true;
                    }
                    else if (cmd == "skill") {
                        std::vector<EPtr> ae;
                        for (auto& e : encounter.GetEnemies()) if (e->IsAlive()) ae.push_back(e);
                        if (HandleSkill(actor, ae, input)) turnDone = true;
                    }
                    else if (cmd == "item") {
                        auto& inv = party.GetInventory();
                        const auto& stacks = inv.GetItemStacks();
                        if (stacks.empty()) { std::cout << "Inventory empty!\n"; continue; }

                        std::cout << "Inventory:\n";
                        for (size_t i = 0; i < stacks.size(); ++i)
                            std::cout << "[" << i << "] " << stacks[i].GetName()
                                      << " x" << stacks[i].GetQuantity() << "\n";
                        int itemIdx = input.RequestTragetIndex((int)stacks.size());
                        if (itemIdx < 0 || itemIdx >= (int)stacks.size()) itemIdx = 0;
                        auto choseName = stacks[itemIdx].GetName();

                        if (choseName == "Smoke Bomb") {
                            if (isBoss || isFinal) {
                                std::cout << "You can't flee a boss fight!\n"; continue;
                            }
                            inv.UseItem(choseName, *party.GetMembers()[0]);
                            return EncounterResult::Fled;
                        }

                        //Show all members
                        auto& members = party.GetMembers();
                        for (size_t i = 0; i < members.size(); ++i)
                            std::cout << "[" << i << "] " << members[i]->GetName()
                                      << " HP: " << members[i]->GetHp()
                                      << "/" << members[i]->GetMaxHp()
                                      << (members[i]->IsAlive() ? "" : " [DEAD]") << "\n";
                        int tIdx = input.RequestTragetIndex((int)members.size());
                        if (tIdx < 0 || tIdx >= (int)members.size()) tIdx = 0;
                        inv.UseItem(choseName, *members[tIdx]);
                        turnDone = true;
                    }
                    else if (cmd == "run") {
                        if (isBoss || isFinal) {
                            std::cout << "You can't run from a boss fight!\n"; continue;
                        }
                        static std::random_device rd2; static std::mt19937 gen2(rd2());
                        std::uniform_int_distribution<> dis(0,1);
                        if (dis(gen2) == 1) {
                            std::cout << "You successfully fled!\n"; return EncounterResult::Fled;
                        } else {
                            std::cout << "Failed to escape! You take 5 damage!\n";
                            actor->TakeDamage(5);
                        }
                        turnDone = true;
                    }
                    else if (cmd == "status") { party.PrintStatus(); }
                    else { std::cout << "Invalid command. (attack/defend/skill/item/run/status)\n"; }
                }
            }
            //Enemy turn
            else {
                EnemyItem& item = enemyItems[actor.get()];

                //Context
                std::vector<EPtr> aliveEnemies;
                for (auto& e : encounter.GetEnemies()) if (e->IsAlive()) aliveEnemies.push_back(e);

                AIContext ctx { actor, aliveEnemies, party, roundNum, item };

                //Get decision
                AIDecision decision;
                if      (std::dynamic_pointer_cast<Dragon>(actor)) decision = EnemyAI_Dragon (ctx);
                else if (std::dynamic_pointer_cast<Troll>(actor)) decision = EnemyAI_Troll (ctx);
                else if (std::dynamic_pointer_cast<Goblin>(actor)) decision = EnemyAI_Goblin (ctx);
                else if (std::dynamic_pointer_cast<Orc>(actor)) decision = EnemyAI_Orc (ctx);
                else if (std::dynamic_pointer_cast<Skeleton>(actor)) decision = EnemyAI_Skeleton (ctx);
                else {
                    //Fallback
                    decision.action = AIAction::BasicAttack;
                    decision.target = ChooseRandomAlive(party.GetAliveMembers());
                }

                //Ensure target is valid
                if (!decision.target && decision.action != AIAction::Defend
                    && !(decision.action == AIAction::UseSkill && std::dynamic_pointer_cast<Dragon>(actor))) {
                    decision.target = ChooseRandomAlive(party.GetAliveMembers());
                }
                if (!decision.target && decision.action == AIAction::BasicAttack) continue;

                //Execute
                switch (decision.action) {

                case AIAction::UseItem: {
                    //Use enemy item
                    if (item.IsAvailable()) {
                        std::cout << actor->GetName() << " uses their " << item.name << "!\n";
                        if (item.type == EnemyItem::Type::HealPotion) {
                            int heal = 25;
                            int before = actor->GetHp();
                            actor->SetHp(std::min(actor->GetHp() + heal, actor->GetMaxHp()));
                            std::cout << actor->GetName() << " recovers " << (actor->GetHp() - before) << " HP!\n";
                        }
                        else if (item.type == EnemyItem::Type::BerserkerRoot) {
                            //ATK + 5 for fight
                            actor->SetAttack(actor->GetAttack() + 5);
                            std::cout << actor->GetName() << "'s eyes go bloodshot — ATK +5!\n";
                        }
                        else if (item.type == EnemyItem::Type::PoisonFlask) {
                            //Next attack poison
                            actor->ApplyStatus(StatusType::Poison, 2, 0); //2 turn
                            std::cout << actor->GetName() << " coats their weapon with poison!\n";
                        }
                        item.Consume();
                    }
                    break;
                }

                case AIAction::Defend: {
                    actor->Defend();
                    std::cout << actor->GetName() << " braces for impact!\n";
                    break;
                }

                case AIAction::UseSkill: {
                    if (auto d = std::dynamic_pointer_cast<Dragon>(actor)) {
                        if (!decision.target) {
                            auto m = party.GetAliveMembers();
                            d->DragonBreath(m);
                        } else {
                            d->TailSwipe(*decision.target);
                        }
                    }
                    else if (auto t = std::dynamic_pointer_cast<Troll>(actor)) {
                        if (decision.target == actor) {
                            t->Regenerate();
                        } else {
                            t->BoulderSmash(*decision.target);
                        }
                    }
                    else if (auto g = std::dynamic_pointer_cast<Goblin>(actor)) {
                        g->ClubSmash(*decision.target);
                    }
                    else if (auto o = std::dynamic_pointer_cast<Orc>(actor)) {
                        o->AxeSwing(*decision.target);
                    }
                    else if (auto s = std::dynamic_pointer_cast<Skeleton>(actor)) {
                        //Skill roll
                        static std::mt19937 skGen(std::random_device{}());
                        std::uniform_int_distribution<> skRoll(1, 100);
                        int sv = skRoll(skGen);
                        if (sv <= 67) {
                            s->BoneSlash(*decision.target);
                        } else {
                            std::cout << s->GetName() << " jabs a poisoned bone at "
                                      << decision.target->GetName() << "!\n";
                            decision.target->TakeDamage(s->GetAttack());
                            decision.target->ApplyStatus(StatusType::Poison, 2, 3);
                        }
                    }
                    break;
                }

                case AIAction::BasicAttack:
                default: {
                    std::cout << actor->GetName() << " attacks " << decision.target->GetName() << "!\n";
                    decision.target->TakeDamage(actor->GetAttack());
                    break;
                }

                }
            }
        }

        //Round-end pause, easy read
        if (!party.IsDefeated() && !encounter.IsAllEnemiesDefeated())
            PressEnter(input);

        //Loser
        if (party.IsDefeated()) {
            AsciiArt::PrintGameOver();
            return EncounterResult::Defeat;
        }

        //Victory
        if (encounter.IsAllEnemiesDefeated()) {
            if (isFinal) AsciiArt::PrintWin();
            else         AsciiArt::PrintVictory();

            int totalXp = 0, totalGold = 0;
            for (auto& e : encounter.GetEnemies()) {
                if (quests) quests->OnKill(e->GetName().substr(0, e->GetName().find(' ')));
                totalXp   += e->GetXpReward();
                totalGold += e->GetGoldReward();
                //Drop roll
                RollEnemyDrop(party, e->GetName());
            }
            std::cout << "Enemies defeated! +" << totalXp << " XP, +" << totalGold << " Gold!\n";
            AsciiArt::PrintThinDivider();

            //Full XP to alive, 50% to dead members
            for (auto& m : party.GetMembers()) {
                int xp = m->IsAlive() ? totalXp : totalXp / 2;
                if (xp > 0) {
                    std::cout << m->GetName() << " gains " << xp << " XP"
                              << (m->IsAlive() ? "" : " (dead)") << "!\n";
                    m->GainXp(xp);
                }
            }
            party.AddXp(totalXp);
            party.AddGold(totalGold);
            if (quests) quests->OnGoldEarned(totalGold);

            //Post-fight MP recovery
            for (auto& m : party.GetMembers()) if (m->IsAlive()) m->OnFightEnd();

            //Tick curses
            for (auto& m : party.GetMembers()) {
                if (m->IsCursed()) {
                    m->TickCurse();
                    if (m->IsAlive())
                        std::cout << m->GetName() << "'s curse tightens... "
                                  << m->GetCurseRemainingFights() << " fight(s) left.\n";
                }
            }
            return EncounterResult::Victory;
        }
    }
    return EncounterResult::Fled;
}
