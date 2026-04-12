#include <vector>
#include <memory>
#include <random>
#include <iostream>
#include <algorithm>
#include <string>

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

TurnManager::TurnManager() {}
TurnManager::~TurnManager() {}

// ── Helpers ───────────────────────────────────────────────────

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

// Crit-aware player attack
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

// Roll for enemy loot drop (20% Potion, 10% coin)
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

// Name enemies uniquely when duplicates exist (Goblin A, Goblin B…)
static void NameEnemies(Encounter& encounter) {
    auto& enemies = const_cast<std::vector<std::shared_ptr<Character>>&>(
        encounter.GetEnemies());
    // count occurrences of each base name
    std::vector<std::string> seen;
    for (auto& e : enemies) {
        std::string base = e->GetName();
        int count = 0;
        for (auto& s : seen) if (s == base) count++;
        if (count > 0 || std::count_if(enemies.begin(), enemies.end(),
            [&](auto& x){ return x->GetName() == base; }) > 1) {
            // assign suffix A, B, C...
        }
        seen.push_back(base);
    }
    // Second pass: if any name appears more than once, suffix all of them
    for (auto& e : enemies) {
        std::string base = e->GetName();
        int total = (int)std::count_if(enemies.begin(), enemies.end(),
            [&](auto& x){ return x->GetName().substr(0, base.size()) == base; });
        (void)total;
    }
    // Simple approach: track per-name counter
    std::vector<std::string> nameCount;
    for (auto& e : enemies) {
        std::string base = e->GetName();
        // strip any existing suffix first
        int cnt = (int)std::count(nameCount.begin(), nameCount.end(), base);
        if (std::count_if(enemies.begin(), enemies.end(),
            [&](auto& x){ return x->GetName() == base; }) > 1) {
            e->SetName(base + " " + std::string(1, (char)('A' + cnt)));
        }
        nameCount.push_back(base);
    }
}

// ── Skill submenu helper ─────────────────────────────────────
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

// ─────────────────────────────────────────────────────────────
bool TurnManager::ExecuteEncounter(PlayerParty& party, Encounter& encounter,
                                   InputManager& input, QuestLog* quests)
{
    // ── Beggar ───────────────────────────────────────────────
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
        return true;
    }

    // ── Merchant ─────────────────────────────────────────────
    if (encounter.IsMerchantEncounter()) {
        AsciiArt::PrintMerchant();
        Merchant merchant;
        merchant.Interact(party, input);
        return true;
    }

    // ── Combat ───────────────────────────────────────────────
    bool isBoss  = encounter.IsBossEncounter();
    bool isFinal = encounter.IsFinalBoss();

    // Name duplicate enemies (Goblin A, Goblin B…)
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

    int roundNum = 0;
    using EPtr = std::shared_ptr<Character>;

    while (true) {
        roundNum++;
        AsciiArt::PrintDivider();
        std::cout << "  ROUND " << roundNum << "\n";
        AsciiArt::PrintThinDivider();

        // Build and sort turn order
        std::vector<EPtr> order;
        for (auto& p : party.GetMembers()) if (p->IsAlive()) order.push_back(p);
        for (auto& e : encounter.GetEnemies()) if (e->IsAlive()) order.push_back(e);
        std::sort(order.begin(), order.end(), [](const EPtr& a, const EPtr& b) {
            return a->GetSpeed() > b->GetSpeed();
        });

        for (auto& actor : order) {
            if (!actor->IsAlive()) continue;
            if (party.IsDefeated() || encounter.IsAllEnemiesDefeated()) break;

            // Save stun state BEFORE ticking (stun consumes the turn)
            bool wasStunned = actor->IsStunned();

            // Tick status at start of turn
            actor->TickStatus();
            if (!actor->IsAlive()) continue;
            if (wasStunned) continue; // stun consumed the turn

            actor->ResetDefend();

            bool isPlayer = false;
            for (auto& p : party.GetMembers())
                if (p == actor) { isPlayer = true; break; }

            if (isPlayer) {
                // Show HP + MP + enemy status
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
                            return true;
                        }

                        // Show all members including dead (for Revive Scroll)
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
                            std::cout << "You successfully fled!\n"; return true;
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
            // ── Enemy turn ───────────────────────────────────
            else {
                auto alivePlayers = party.GetAliveMembers();
                auto target = ChooseRandomAlive(alivePlayers);
                if (!target) continue;

                static std::random_device rd; static std::mt19937 gen(rd());
                std::uniform_int_distribution<> roll(0,100);
                int r = roll(gen);

                if (auto d = std::dynamic_pointer_cast<Dragon>(actor)) {
                    if      (r <= 25) { auto m = party.GetAliveMembers(); d->DragonBreath(m); }
                    else if (r <= 50) d->TailSwipe(*target);
                    else { std::cout << actor->GetName() << " snaps at " << target->GetName() << "!\n"; target->TakeDamage(actor->GetAttack()); }
                }
                else if (auto t = std::dynamic_pointer_cast<Troll>(actor)) {
                    if      (r <= 20) t->BoulderSmash(*target);
                    else if (r <= 40) t->Regenerate();
                    else { std::cout << actor->GetName() << " swings at " << target->GetName() << "!\n"; target->TakeDamage(actor->GetAttack()); }
                }
                else if (auto g = std::dynamic_pointer_cast<Goblin>(actor)) {
                    if (r <= 30) g->ClubSmash(*target);
                    else { std::cout << actor->GetName() << " attacks " << target->GetName() << "!\n"; target->TakeDamage(actor->GetAttack()); }
                }
                else if (auto o = std::dynamic_pointer_cast<Orc>(actor)) {
                    if (r <= 30) o->AxeSwing(*target);
                    else { std::cout << actor->GetName() << " attacks " << target->GetName() << "!\n"; target->TakeDamage(actor->GetAttack()); }
                }
                else if (auto s = std::dynamic_pointer_cast<Skeleton>(actor)) {
                    // Skeleton: 20% poison attack
                    if (r <= 20) {
                        std::cout << s->GetName() << " jabs a poisoned bone at " << target->GetName() << "!\n";
                        target->TakeDamage(s->GetAttack());
                        target->ApplyStatus(StatusType::Poison, 2, 3);
                    }
                    else if (r <= 50) s->BoneSlash(*target);
                    else { std::cout << actor->GetName() << " attacks " << target->GetName() << "!\n"; target->TakeDamage(actor->GetAttack()); }
                }
                else {
                    std::cout << actor->GetName() << " attacks " << target->GetName() << "!\n";
                    target->TakeDamage(actor->GetAttack());
                }
            }
        } // end actor loop

        // Round-end pause for readability
        if (!party.IsDefeated() && !encounter.IsAllEnemiesDefeated())
            PressEnter(input);

        // ── Defeat ──────────────────────────────────────────
        if (party.IsDefeated()) {
            AsciiArt::PrintGameOver();
            return false;
        }

        // ── Victory ─────────────────────────────────────────
        if (encounter.IsAllEnemiesDefeated()) {
            if (isFinal) AsciiArt::PrintWin();
            else         AsciiArt::PrintVictory();

            int totalXp = 0, totalGold = 0;
            for (auto& e : encounter.GetEnemies()) {
                if (quests) quests->OnKill(e->GetName().substr(0, e->GetName().find(' ')));
                totalXp   += e->GetXpReward();
                totalGold += e->GetGoldReward();
                // Drop roll
                RollEnemyDrop(party, e->GetName());
            }
            std::cout << "Enemies defeated! +" << totalXp << " XP, +" << totalGold << " Gold!\n";
            AsciiArt::PrintThinDivider();

            // Full XP to alive, 50% to dead members
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

            // Post-fight MP recovery
            for (auto& m : party.GetMembers()) if (m->IsAlive()) m->OnFightEnd();

            // Tick curses
            for (auto& m : party.GetMembers()) {
                if (m->IsCursed()) {
                    m->TickCurse();
                    if (m->IsAlive())
                        std::cout << m->GetName() << "'s curse tightens... "
                                  << m->GetCurseRemainingFights() << " fight(s) left.\n";
                }
            }
            return true;
        }
    }
    return true;
}
