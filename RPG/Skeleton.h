#pragma once

#include "Character.h"

class Skeleton : public Character
{
public:
	Skeleton(const std::string& name = "Skeleton");
	virtual ~Skeleton();

	virtual void Update() override;

	void BoneSlash(Character& target);

	virtual int GetXpReward() const override;
	virtual int GetGoldReward() const override;

};

