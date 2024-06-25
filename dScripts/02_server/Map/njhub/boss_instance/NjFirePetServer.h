#pragma once
#include "PetFromObjectServer.h"

class NjFirePetServer : public PetFromObjectServer {
	void OnStartup(Entity* self) override;
	const std::string m_Precondition = "359";
};
