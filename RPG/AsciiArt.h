#pragma once
#include <iostream>
#include <string>

// ============================================================
//  AsciiArt  –  all terminal visuals for the RPG
// ============================================================
namespace AsciiArt {

// ── Title screen ─────────────────────────────────────────────
inline void PrintTitle() {
    std::cout << R"(
  ____  ____   ____
 |  _ \|  _ \ / ___|
 | |_) | |_) | |  _
 |  _ <|  __/| |_| |
 |_| \_\_|    \____|

  A Terminal Adventure
)" << "\n";
}

// ── Dividers ─────────────────────────────────────────────────
inline void PrintDivider() {
    std::cout << "============================================\n";
}
inline void PrintThinDivider() {
    std::cout << "--------------------------------------------\n";
}

// ── Party members ────────────────────────────────────────────
inline void PrintWarrior() {
    std::cout << R"(
      O
     /|\
    _/ \_
   [=====]   WARRIOR
)" << "\n";
}

inline void PrintMage() {
    std::cout << R"(
      *
    ( O )
     )|(
    /   \      MAGE
)" << "\n";
}

inline void PrintRanger() {
    std::cout << R"(
      O
    --|\
      |  -->   RANGER
     / \
)" << "\n";
}

// ── Enemies ──────────────────────────────────────────────────
inline void PrintGoblin() {
    std::cout << R"(
    .  .
   (o  o)
    \  /
   __|__|__   GOBLIN
)" << "\n";
}

inline void PrintOrc() {
    std::cout << R"(
   ,--  --,
  ( O    O )
   \  ||  /
  __|====|__  ORC
)" << "\n";
}

inline void PrintSkeleton() {
    std::cout << R"(
    _  _
   (o)(o)
    )  (
   /|  |\   SKELETON
)" << "\n";
}

inline void PrintBeggar() {
    std::cout << R"(
     o
    /|\
    / \     BEGGAR
  ~please~
)" << "\n";
}

inline void PrintMerchant() {
    std::cout << R"(
      O
    --|--
    _/ \_
   [$$$$$]   MERCHANT
)" << "\n";
}

// ── Events ───────────────────────────────────────────────────
inline void PrintVictory() {
    std::cout << R"(
  *   *   *
    \O/
     |      VICTORY!
    / \
  *   *   *
)" << "\n";
}

inline void PrintGameOver() {
    std::cout << R"(
  +-+-+-+-+-+-+-+-+
  |G|A|M|E| |O|V|E|R|
  +-+-+-+-+-+-+-+-+
     ...you died...
)" << "\n";
}

inline void PrintLevelUp() {
    std::cout << R"(
    *  *  *
   * LEVEL  *
  *   UP!    *
   *        *
    *  *  *
)" << "\n";
}

inline void PrintCombat() {
    std::cout << R"(
  >== BATTLE START ==<
  swords clash...
)" << "\n";
}

inline void PrintSkull() {
    std::cout << R"(
    ___
   (x x)
   )   (    CURSED!
  (_(_)_)
)" << "\n";
}

inline void PrintTroll() {
    std::cout << R"(
   ,########,
  ( O      O )
  |  ######  |
  |  ------  |
 /|__________|\
  |  ||  ||  |   TROLL  [BOSS]
)" << "\n";
}

inline void PrintBossWarning() {
    std::cout << R"(
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  !!      BOSS ENCOUNTER      !!
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)" << "\n";
}

inline void PrintCampfire() {
    std::cout << R"(
      ) )
     ( (
    .-'-.       ~ rest ~
   /|6 6|\
  | | = | |   The party camps
   \|---|/     for the night.
    '---'
)" << "\n";
}

// ── HP bar ───────────────────────────────────────────────────
inline void PrintHpBar(const std::string& name, int hp, int maxHp, int width = 20) {
    int filled = (maxHp > 0) ? (hp * width / maxHp) : 0;
    if (filled < 0) filled = 0;
    if (filled > width) filled = width;

    std::string bar = "[";
    for (int i = 0; i < width; ++i)
        bar += (i < filled) ? "#" : " ";
    bar += "]";

    std::cout << name << " HP " << bar << " " << hp << "/" << maxHp << "\n";
}

// ── XP bar ───────────────────────────────────────────────────
inline void PrintXpBar(int xp, int xpNeeded, int level, int width = 20) {
    int filled = (xpNeeded > 0) ? (xp * width / xpNeeded) : width;
    if (filled < 0) filled = 0;
    if (filled > width) filled = width;

    std::string bar = "[";
    for (int i = 0; i < width; ++i)
        bar += (i < filled) ? "=" : ".";
    bar += "]";

    std::cout << "LVL " << level << " XP " << bar << " " << xp << "/" << xpNeeded << "\n";
}

} // namespace AsciiArt
