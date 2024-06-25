#include "NjFirePetServer.h"
#include "PetComponent.h"

void NjFirePetServer::OnStartup(Entity* self) {
	auto* petComponent = self->GetComponent<PetComponent>();
	if (petComponent == nullptr || petComponent->GetOwnerId() != LWOOBJID_EMPTY)
		return;

	// Removes the dog c:
	petComponent->SetPreconditions(const_cast<std::string&>(m_Precondition));
	PetFromObjectServer::OnStartup(self);
}
