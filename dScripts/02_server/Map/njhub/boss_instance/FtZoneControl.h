#pragma once
#include "CppScripts.h"

class FtZoneControl : public CppScripts::Script
{
public:
	void OnStartup(Entity* self) override;

	void OnZoneLoadedInfo(Entity* self);

	void OnPlayerLoaded(Entity* self, Entity* player);

	void OnPlayerExit(Entity* self);

	void OnTimerDone(Entity* self, std::string timerName) override;

};
