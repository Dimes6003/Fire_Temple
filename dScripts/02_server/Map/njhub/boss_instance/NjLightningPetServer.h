#pragma once
#include "PetFromObjectServer.h"

class NjLightningPetServer : public PetFromObjectServer {
	void OnStartup(Entity* self) override;
	const std::string m_Precondition = "357";
};
