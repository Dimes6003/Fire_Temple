#pragma once
#include "CppScripts.h"

class DoorIndicatorServer : public CppScripts::Script
{
public:
	void OnFireEventServerSide(Entity* self, Entity* sender, std::string args, int32_t param1, int32_t param2,
		int32_t param3) override;



};

