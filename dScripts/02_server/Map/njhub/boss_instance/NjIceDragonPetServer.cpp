#include "NjIceDragonPetServer.h"
#include "Entity.h"

void NjIceDragonPetServer::SetVariables(Entity* self) {
	self->SetVar<LOT>(u"petLOT", 16479);
	self->SetVar<std::string>(u"petType", "zzicepet");
	self->SetVar<uint32_t>(u"maxPets", 3);
	self->SetVar<std::u16string>(u"spawnAnim", u"spawn");
	self->SetVar<std::u16string>(u"spawnCinematic", u"ZZIcePetSpawn");
}
