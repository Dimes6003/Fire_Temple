#include "NjLightningDragonPetServer.h"
#include "Entity.h"

void NjLightningDragonPetServer::SetVariables(Entity* self) {
	self->SetVar<LOT>(u"petLOT", 16740);
	self->SetVar<std::string>(u"petType", "litngpet");
	self->SetVar<uint32_t>(u"maxPets", 3);
	self->SetVar<std::u16string>(u"spawnAnim", u"spawn");
	self->SetVar<std::u16string>(u"spawnCinematic", u"LitngPetSpawn");
}
