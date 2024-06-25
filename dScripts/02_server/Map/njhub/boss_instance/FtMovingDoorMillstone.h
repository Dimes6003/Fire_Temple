#pragma once
#include "CppScripts.h"

class FtMovingDoorMillstone : public CppScripts::Script
{
public:
	void OnStartup(Entity* self) override;

	void SpawnLegs(Entity* self, const std::string& loc);

	void OnChildLoaded(Entity* self, Entity* child);

	void NotifyDie(Entity* self, Entity* other, Entity* killer);

	void OnChildRemoved(Entity* self, Entity* child);
};
