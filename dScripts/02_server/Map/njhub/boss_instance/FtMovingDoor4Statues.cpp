#include "FtMovingDoor4Statues.h"
#include "ProximityMonitorComponent.h"
#include "EntityManager.h"
#include "MovingPlatformComponent.h"
#include "EntityInfo.h"
#include "Entity.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"
#include "GameMessages.h"


void FtMovingDoor4Statues::OnStartup(Entity* self) {
	Progress = 0;
	
//	Additional safeguard for shared client script
	self->SetNetworkVar(u"bIsInUse", true);
	
	auto* proximityMonitorComponent = self->GetComponent<ProximityMonitorComponent>();	
	self->SetProximityRadius(360, "fetch_players");	
}	

void FtMovingDoor4Statues::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
	if (name == "fetch_players") {	
		if (entering->IsPlayer()) {
			
			if (!player1) {
				player1 = entering;
			} else if (!player2) {
				player2 = entering;
			} else if (!player3) {
				player3 = entering;
			} else if (!player4) {
				player4 = entering;
			}
		}	
    }	
	

}
	
void FtMovingDoor4Statues::OnFireEventServerSide(Entity* self, Entity* sender, std::string args, int32_t param1, int32_t param2,
	int32_t param3) {

	if (args == "BuildComplete") {
		if (Progress == 3) {
			//Move Plat
			GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);


			//Distribute cinematics & audio
			//Still give players the cine if they leave the room but don't fuck with audio
			//Their current musiccue state is unknown beyond the room << maybe there's a way to pull it from client...?
			const auto& cine = u"4StatuesComplete";	
			auto* proximityMonitorComponent = self->GetComponent<ProximityMonitorComponent>();						
			self->SetNetworkVar(u"flourishready", 1);

			
			
			if (player1 != nullptr) {
				GameMessages::SendPlayCinematic(player1->GetObjectID(), cine, player1->GetSystemAddress());			
				if (proximityMonitorComponent->IsInProximity("fetch_players", player1->GetObjectID())) {
					self->AddTimer("Player1Flourish", 2.5f);
				}
			}
			if (player2 != nullptr) {			
				GameMessages::SendPlayCinematic(player2->GetObjectID(), cine, player2->GetSystemAddress());	
				if (proximityMonitorComponent->IsInProximity("fetch_players", player2->GetObjectID())) {
					self->AddTimer("Player2Flourish", 2.5f);
				}
			}
			if (player3 != nullptr) {		
				GameMessages::SendPlayCinematic(player3->GetObjectID(), cine, player3->GetSystemAddress());	
				if (proximityMonitorComponent->IsInProximity("fetch_players", player3->GetObjectID())) {
					self->AddTimer("Player3Flourish", 2.5f);
				}	
			}
			if (player4 != nullptr) {				
				GameMessages::SendPlayCinematic(player4->GetObjectID(), cine, player4->GetSystemAddress());	
				if (proximityMonitorComponent->IsInProximity("fetch_players", player4->GetObjectID())) {
					self->AddTimer("Player4Flourish", 2.5f);
				}	
			}			
			//end
			
		} else {
			Progress++;
		}
	}	
}

void FtMovingDoor4Statues::OnTimerDone(Entity* self, std::string timerName) {
	
	if (timerName == "Player1Flourish") {	
		GameMessages::SendPlayNDAudioEmitter(player1, player1->GetSystemAddress(), "{a4f234db-5b30-47e5-b41c-4d71c4d2ae0f}");		
	}		
	if (timerName == "Player2Flourish") {	
		GameMessages::SendPlayNDAudioEmitter(player2, player2->GetSystemAddress(), "{a4f234db-5b30-47e5-b41c-4d71c4d2ae0f}");	
	}	
	if (timerName == "Player3Flourish") {	
		GameMessages::SendPlayNDAudioEmitter(player3, player3->GetSystemAddress(), "{a4f234db-5b30-47e5-b41c-4d71c4d2ae0f}");			
	}	
	if (timerName == "Player4Flourish") {	
		GameMessages::SendPlayNDAudioEmitter(player4, player4->GetSystemAddress(), "{a4f234db-5b30-47e5-b41c-4d71c4d2ae0f}");		
	}	
	
}

// Initialize defaults
Entity* FtMovingDoor4Statues::player1 = nullptr;
Entity* FtMovingDoor4Statues::player2 = nullptr;
Entity* FtMovingDoor4Statues::player3 = nullptr;
Entity* FtMovingDoor4Statues::player4 = nullptr;
