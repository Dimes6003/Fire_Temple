#include "FtWavesManager.h"
#include "GameMessages.h"
#include "ProximityMonitorComponent.h"

void FtWavesManager::OnStartup(Entity* self) {
	
			
}

void FtWavesManager::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {
	
	

}	

void FtWavesManager::OnCollisionPhantom(Entity* self, Entity* target) {
	if (target->IsPlayer()) {
		
//		collect & store entities for cines	
	    if (target != engaged1 && target != engaged2 && target != engaged3 && target != engaged4) {	
			if (!engaged1) {
				engaged1 = target;
			} else if (!engaged2) {
				engaged2 = target;
			} else if (!engaged3) {
				engaged3 = target;
			} else if (!engaged4) {
				engaged4 = target;
			}
		}	
//		end		


	}	
}

void FtWavesManager::OnTimerDone(Entity* self, std::string timerName) {
	
}

// Initialize defaults
Entity* FtWavesManager::engaged1 = nullptr;
Entity* FtWavesManager::engaged2 = nullptr;
Entity* FtWavesManager::engaged3 = nullptr;
Entity* FtWavesManager::engaged4 = nullptr;