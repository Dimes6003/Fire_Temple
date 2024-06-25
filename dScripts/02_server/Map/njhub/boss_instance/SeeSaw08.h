#pragma once
#include "CppScripts.h"

class SeeSaw08 : public CppScripts::Script
{
public:
	void OnStartup(Entity* self) override;

	void OnSkillEventFired(Entity* self, Entity* caster, const std::string& message) override;

	void TriggerDrill(Entity* self);

	void SpawnLegs(Entity* self, const std::string& loc);

	void OnChildLoaded(Entity* self, Entity* child);

	void NotifyDie(Entity* self, Entity* other, Entity* killer);

	void OnChildRemoved(Entity* self, Entity* child);

	void OnWaypointReached(Entity* self, uint32_t waypointIndex) override;

	void OnTimerDone(Entity* self, std::string timerName) override;

private:


};
