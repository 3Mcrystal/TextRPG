#include <iostream>
#include "WorldMap.h"
#include "AsciiArt.h"

WorldMap::WorldMap() {
	m_locations = {
		{
			LocationId::Town,
			"Ashveil (Town)",
			"A battered trading post clinging to the edge of civilisation.",
			"The smell of smoke and bread fills the air. Townsfolk eye you warily.",
			0, 0, 0,  //no combat weights = safe zone
			true, true
		},
		{
			LocationId::Forest,
			"Darkwood Forest",
			"Ancient trees block out the sky. Goblins nest here in the hundreds.",
			"Twigs snap in the darkness. Something is watching you.",
			60, 10, 30,
			false, false
		},
		{
			LocationId::Dungeon,
			"The Sunken Dungeon",
			"Ruins of a fortress swallowed by the earth. Orcs have claimed it.",
			"The air reeks of rot. Torches flicker on mossy stone walls.",
			10, 60, 30,
			false, false
		},
		{
			LocationId::Mountains,
			"The Shattered Peaks",
			"Jagged crags where the undead roam. The Dragon lurks here.",
			"The wind howls like the dead. Every shadow could be your last.",
			10, 20, 70,
			false, false
		}
	};
}

const Location& WorldMap::Find(LocationId id) const {
	for (auto& l : m_locations)
		if (l.id == id) return l;
	return m_locations[0];
}

const Location& WorldMap::GetCurrent() const {
	return Find(m_currentId);
}

void WorldMap::PrintMap() const {
	AsciiArt::PrintDivider();
	std::cout << "  WORLD MAP\n";
	AsciiArt::PrintThinDivider();
	int i = 0;
	for (auto& l : m_locations) {
		std::string marker = (l.id == m_currentId) ? " << YOU ARE HERE" : "";
		std::cout << "[" << i++ << "] " << l.name << marker << "\n";
		std::cout << "    " << l.description << "\n";
	}
	AsciiArt::PrintDivider();
}

bool WorldMap::Travel(int choice) {
	if (choice < 0 || choice >= (int)m_locations.size()) return false;
	m_currentId = m_locations[choice].id;
	const auto& loc = GetCurrent();
	AsciiArt::PrintThinDivider();
	std::cout << "  >> Travelling to " << loc.name << "...\n";
	std::cout << "  " << loc.flavorText << "\n";
	PrintLocationArt();
	return true;
}
//more ugly ascii
void WorldMap::PrintLocationArt() const {
	switch (m_currentId) {
		case LocationId::Town:
			std::cout << R"(
   ________
  |  TOWN  |
  |________|
  | [_][_] |  Ashveil
  | [_][_] |
  |________|
)" << "\n"; break;
		case LocationId::Forest:
			std::cout << R"(
  ^  ^  ^
 /|\/|\/|\
/  \  /  \  Darkwood
 || || ||
)" << "\n"; break;
		case LocationId::Dungeon:
			std::cout << R"(
  _________
 |  _   _  |
 | | | | | |
 |_|_|_|_|_|  Sunken Dungeon
)" << "\n"; break;
		case LocationId::Mountains:
			std::cout << R"(
    /\  /\
   /  \/  \
  / /\  /\ \  Shattered Peaks
 /_/  \/  \_\
)" << "\n"; break;
	}
}
