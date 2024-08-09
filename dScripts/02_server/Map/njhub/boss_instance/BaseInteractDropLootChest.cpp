#include "BaseInteractDropLootChest.h"
#include "Loot.h"
#include "GameMessages.h"

void BaseInteractDropLootChest::OnUse(Entity* self, Entity* user) {
	if (!self->GetNetworkVar<bool>(u"bInUse")) {
		BaseUse(self, user);
	}
}

void BaseInteractDropLootChest::BaseUse(Entity* self, Entity* user) {
	uint32_t lootMatrix = self->GetVar<int32_t>(u"UseLootMatrix");
	if (lootMatrix == 0) lootMatrix = self->GetVar<int32_t>(u"smashable_loot_matrix");
	if (lootMatrix == 0) lootMatrix = 919;

	auto useSound = self->GetVar<std::string>(u"sound1");
	if (!useSound.empty()) {
		GameMessages::SendPlayNDAudioEmitter(self, user->GetSystemAddress(), useSound);
	}

	self->SetNetworkVar<bool>(u"bInUse", true);

	Loot::DropLoot(user, self, lootMatrix, 0, 0);
}
