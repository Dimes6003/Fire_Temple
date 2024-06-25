#include "NjFireDragonPetServer.h"
#include "Entity.h"

void NjFireDragonPetServer::SetVariables(Entity* self) {
	self->SetVar<LOT>(u"petLOT", 16741);
	self->SetVar<std::string>(u"petType", "zfirepet");
	self->SetVar<uint32_t>(u"maxPets", 3);
	self->SetVar<std::u16string>(u"spawnAnim", u"spawn");
	self->SetVar<std::u16string>(u"spawnCinematic", u"ZFirePetSpawn");
}
