#pragma once
#include "CppScripts.h"

class Spinner22 : public CppScripts::Script
{
public:
	void OnStartup(Entity* self) override;

	void OnSkillEventFired(Entity* self, Entity* caster, const std::string& message) override;

	void TriggerDrill(Entity* self);

	void OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) override;

	void OnTimerDone(Entity* self, std::string timerName) override;

private:
	int m_Counter;

	std::vector<int32_t> m_MissionsToUpdate = { 3096, 3097, 3098 };

};
