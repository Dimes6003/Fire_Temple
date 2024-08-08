#include "FtZoneControl.h"
#include "EntityManager.h"
#include "Character.h"
#include "Entity.h"
#include "GameMessages.h"
#include "dZoneManager.h"
#include "RenderComponent.h"


int PlayerCount = 0;
int BossPlayerCount = 0;

Entity* FtZoneControl::player1 = nullptr;
Entity* FtZoneControl::player2 = nullptr;
Entity* FtZoneControl::player3 = nullptr;
Entity* FtZoneControl::player4 = nullptr;

void FtZoneControl::OnStartup(Entity* self) {
	

	
}

void FtZoneControl::OnZoneLoadedInfo(Entity* self) {


	
}

void FtZoneControl::OnPlayerLoaded(Entity* self, Entity* player) {
	
	
    if (player != player1 && player != player2 && player != player3 && player != player4) {
        if (!player1) {
            player1 = player;
			PlayerCount++;
			BossPlayerCount++;
        } else if (!player2) {
            player2 = player;
			PlayerCount++;
			BossPlayerCount++;
        } else if (!player3) {
            player3 = player;	
			PlayerCount++;
			BossPlayerCount++;
        } else if (!player4) {
            player4 = player;
			PlayerCount++;
			BossPlayerCount++;
        }
		
//		Send team size		
//		const auto WavesManager = Game::entityManager->GetEntitiesInGroup("BossManager");

		const auto WavesManager = Game::entityManager->GetEntitiesInGroup("WavesManager");
		const auto BossManager = Game::entityManager->GetEntitiesInGroup("BossManager");
		for (auto* wavesobj : WavesManager) {
			wavesobj->SetVar(u"playercount", PlayerCount);	
		}
		for (auto* bossobj : BossManager) {
			bossobj->SetVar<int>(u"playercount", BossPlayerCount);	
		}		
    }	


}

void FtZoneControl::OnPlayerExit(Entity* self, Entity* player) {
	PlayerCount--;
	BossPlayerCount--;
	
	const auto WavesManager = Game::entityManager->GetEntitiesInGroup("WavesManager");
	const auto BossManager = Game::entityManager->GetEntitiesInGroup("BossManager");
	for (auto* wavesobj : WavesManager) {
		wavesobj->SetVar(u"playercount", PlayerCount);	
	}
	for (auto* bossobj : BossManager) {
		bossobj->SetVar<int>(u"playercount", BossPlayerCount);	
	}		
}

void FtZoneControl::OnTimerDone(Entity* self, std::string timerName) {
		
}
