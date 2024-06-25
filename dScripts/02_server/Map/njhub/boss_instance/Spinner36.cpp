#include "Spinner36.h"
#include "EntityManager.h"
#include "MovingPlatformComponent.h"
#include "EntityInfo.h"
#include "GameMessages.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"

void Spinner36::OnStartup(Entity* self) {
//	self->AddTimer("MoveDown", 11.9f);	
}

void Spinner36::OnTimerDone(Entity* self, std::string timerName) {	
	if (timerName == "MoveUp") {	
		RenderComponent::PlayAnimation(self, u"up");
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 0, 1);
		self->AddTimer("MoveDown", 4.0f);	
	}
	
	if (timerName == "MoveDown") {	
		RenderComponent::PlayAnimation(self, u"down");
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);
		self->AddTimer("MoveUp", 4.0f);	
	}
}


