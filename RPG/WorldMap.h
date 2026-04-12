#pragma once
#include <string>
#include <vector>

enum class LocationId {
	Town,
	Forest,
	Dungeon,
	Mountains
};

struct Location {
	LocationId  id;
	std::string name;
	std::string description;
	std::string flavorText;

	//Encounter weights
	int goblinWeight;
	int orcWeight;
	int skeletonWeight;
	bool hasMerchant;
	bool isSafe; //Town only
};

class WorldMap {
public:
	WorldMap();

	void              PrintMap()   const;
	const Location&   GetCurrent() const;
	bool              Travel(int choice);  //false if invalid
	void              PrintLocationArt() const;

	LocationId CurrentId() const { return m_currentId; }

private:
	std::vector<Location> m_locations;
	LocationId            m_currentId = LocationId::Town;

	const Location& Find(LocationId id) const;
};
