#pragma once
#include "CppScripts.h"

class BaseInteractDropLootChest : public CppScripts::Script
{
public:
	virtual void OnUse(Entity* self, Entity* user) override;
	void BaseUse(Entity* self, Entity* user);
};

