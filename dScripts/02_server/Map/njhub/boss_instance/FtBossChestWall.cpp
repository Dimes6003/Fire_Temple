#include "FtBossChestWall.h"
#include "DestroyableComponent.h"
#include "SkillComponent.h"
#include "GameMessages.h"
#include "EntityInfo.h"
#include "EntityManager.h"


void FtBossChestWall::OnStartup(Entity* self) {
	
}

void FtBossChestWall::OnNotifyObject(Entity* self, Entity* sender, const std::string& name, 
int32_t param1, int32_t param2) {
	
	if (name == "BossOver") {
		self->AddTimer("SmashSelf", 2);	
	}
}	

void FtBossChestWall::OnTimerDone(Entity* self, std::string timerName) {	
//		self->AddTimer("SmashSelf", 2);	

	if (timerName == "SmashSelf") {	
		self->Smash(self->GetObjectID(), eKillType::SILENT);
	}
}



