#pragma once
#include "CppScripts.h"

class FtLegTimed : public CppScripts::Script
{
public:
	void OnStartup(Entity* self) override;

	void OnSkillEventFired(Entity* self, Entity* caster, const std::string& message) override;

	void TriggerDrill(Entity* self);

	void OnTimerDone(Entity* self, std::string timerName) override;


};
