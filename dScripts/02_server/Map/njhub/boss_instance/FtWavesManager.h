#pragma once
#include "CppScripts.h"

class FtWavesManager : public CppScripts::Script
{
public:
	void OnStartup(Entity* self) override;

	void OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) override;

	void OnCollisionPhantom(Entity* self, Entity* target) override;

	void OnTimerDone(Entity* self, std::string timerName) override;

private:

    static Entity* engaged1;
    static Entity* engaged2;
    static Entity* engaged3;
    static Entity* engaged4;


};
