#pragma once
#include "CppScripts.h"

class FtBossChestWall : public CppScripts::Script
{
public:
	void OnStartup(Entity* self) override;

	void OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1 = 0, int32_t param2 = 0) override;

	void OnTimerDone(Entity* self, std::string timerName) override;

private:

	int SharedInt;

};
