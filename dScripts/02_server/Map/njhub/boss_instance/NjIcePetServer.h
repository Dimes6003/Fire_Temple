#pragma once
#include "PetFromObjectServer.h"

class NjIcePetServer : public PetFromObjectServer {
	void OnStartup(Entity* self) override;
	const std::string m_Precondition = "358";
};
