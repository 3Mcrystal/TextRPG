#pragma once
#include <iostream>
#include <string>

//Ugly as f

namespace AsciiArt {

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

//Dividers
inline void PrintDivider() {
    std::cout << "============================================\n";
}
inline void PrintThinDivider() {
    std::cout << "--------------------------------------------\n";
}

//Party members
inline void PrintWarrior() {
    std::cout << R"(
      O
     /|\
    _/ \_
   [=====]   SLANKEC (WARRIOR)
)" << "\n";
}

inline void PrintMage() {
    std::cout << R"(
      *
    ( O )
     )|(
    /   \      NARCHIS (MAGE)
)" << "\n";
}

inline void PrintRanger() {
    std::cout << R"(
      O
    --|\
      |  -->   AETHYRA (RANGER)
     / \
)" << "\n";
}

//Enemies
//The only thing that looks like something
inline void PrintGoblin() {
    std::cout << R"(
    .  .
   (o  o)
    \  /
   _|__|_   GOBLIN
)" << "\n";
}

//why do you even read that ?
inline void PrintOrc() {
    std::cout << R"(
   ,--  --,
  ( O    O )
   \  ||  /
    |====|   ORC
)" << "\n";
}

// I swear, it's a skeleton
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

//Events
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
  +-+-+-+-+-+-+-+-+-+
  |G|A|M|E| |O|V|E|R|
  +-+-+-+-+-+-+-+-+-+
     ...Git gud...
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

//Cursya from Super Paper Mario or a dead pile of jelly 
inline void PrintSkull() {
    std::cout << R"(
    ___
   (x x)
   )   (    CURSED!
  (____)
)" << "\n";
}

//Your dad when you forget to take out the trash
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

//Dragon with 200 chromosomes
inline void PrintDragon() {
    std::cout << R"(
       ___
     /`   `\
    /  .-.  \
   '  (   )  '
   |  | | |  |
    \ \___/ /     
  ~~~`-----'~~~   IGNARAX THE DRAGON
)" << "\n";
}

inline void PrintWin() {
    std::cout << R"(
 *** *** *** *** *** *** *** ***
 *                             *
 *     THE DRAGON IS SLAIN!    *
 *       Ashveil is saved.     *
 *                             *
 *        ~ YOU WIN ~          *
 *                             *
 *** *** *** *** *** *** *** ***
)" << "\n";
}

//looks like a bomb
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

//HP bar
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

//MP bar
inline void PrintMpBar(const std::string& name, int mp, int maxMp, int width = 20) {
    int filled = (maxMp > 0) ? (mp * width / maxMp) : 0;
    if (filled < 0) filled = 0;
    if (filled > width) filled = width;
    std::string bar = "[";
    for (int i = 0; i < width; ++i)
        bar += (i < filled) ? "*" : " ";
    bar += "]";
    std::cout << name << " MP " << bar << " " << mp << "/" << maxMp << "\n";
}

//XP bar
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

}