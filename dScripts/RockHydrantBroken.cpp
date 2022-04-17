#include "RockHydrantBroken.h"
#include "EntityManager.h"
#include "GameMessages.h"

void RockHydrantBroken::OnStartup(Entity* self)
{
	self->AddTimer("playEffect", 1);

	const auto hydrant = "hydrant0" + self->GetVar<std::string>(u"hydrant");

	const auto bouncers = EntityManager::Instance()->GetEntitiesInGroup(hydrant);

	Game::logger->Log("RockHydrantBroken", "Broken Rock Hydrant spawned (%s)\n", hydrant.c_str());

	for (auto* bouncer : bouncers)
	{
		self->SetVar<LWOOBJID>(u"bouncer", bouncer->GetObjectID());
		Game::logger->Log("RockHydrantBroken", "Activate Bouncer (%s)\n", bouncer.c_str());

		GameMessages::SendBouncerActiveStatus(bouncer->GetObjectID(), true, UNASSIGNED_SYSTEM_ADDRESS);

		GameMessages::SendNotifyObject(bouncer->GetObjectID(), self->GetObjectID(), u"enableCollision", UNASSIGNED_SYSTEM_ADDRESS);
	}

	self->AddTimer("KillBroken", 10);
}

void RockHydrantBroken::OnTimerDone(Entity* self, std::string timerName)
{
	if (timerName == "KillBroken")
	{
		auto* bouncer = EntityManager::Instance()->GetEntity(self->GetVar<LWOOBJID>(u"bouncer"));

		if (bouncer != nullptr)
		{
			GameMessages::SendBouncerActiveStatus(bouncer->GetObjectID(), false, UNASSIGNED_SYSTEM_ADDRESS);

			GameMessages::SendNotifyObject(bouncer->GetObjectID(), self->GetObjectID(), u"disableCollision", UNASSIGNED_SYSTEM_ADDRESS);
		}

		self->Kill();
	}
	else if (timerName == "playEffect")
	{
		GameMessages::SendPlayFXEffect(self->GetObjectID(), 384, u"water", "water", LWOOBJID_EMPTY, 1, 1, true);
	}
}
