#include "FtWavesManager.h"
#include "GameMessages.h"
#include "ProximityMonitorComponent.h"
#include "dZoneManager.h"
#include "EntityManager.h"
#include "RenderComponent.h"
#include "BuffComponent.h"
#include "BaseCombatAIComponent.h"

// Initialize defaults
Entity* FtWavesManager::engaged1 = nullptr;
Entity* FtWavesManager::engaged2 = nullptr;
Entity* FtWavesManager::engaged3 = nullptr;
Entity* FtWavesManager::engaged4 = nullptr;



void FtWavesManager::OnStartup(Entity* self) {
	
	auto BlacksmithSpinner = Game::entityManager->GetEntitiesInGroup("WavesSpinnerBlacksmith");
	

	
	Wave1Progress = 0;
	Wave2Progress = 0;
	Wave3Progress = 0;
	
	WaveNum = 1;	

	LargeGroup = 0;
	
	auto* proximityMonitorComponent = self->GetComponent<ProximityMonitorComponent>();	
	self->SetProximityRadius(450, "Area_Radius");	
	self->SetProximityRadius(150, "Room_Radius");		
}

void FtWavesManager::OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) {	

	if (name == "Area_Radius") {	
		if (entering->IsPlayer()) {
//		collect & store player entities for cines	
			if (entering != engaged1 && entering != engaged2 && entering != engaged3 && entering != engaged4) {	
				if (!engaged1) {
					engaged1 = entering;
				} else if (!engaged2) {
					engaged2 = entering;
				} else if (!engaged3) {
					engaged3 = entering;
				} else if (!engaged4) {
					engaged4 = entering;
				}
			}	
		}
	}	
//		end	
}	

void FtWavesManager::OnCollisionPhantom(Entity* self, Entity* target) {
	if (target->IsPlayer()) {
		
	

		if (bStarted == 1) {
			return;
		}	
		bStarted = 1;	
	
	
		ActivateWaveSpinners(self);	
	}	
}

void FtWavesManager::OnFireEventServerSide(Entity* self, Entity* sender, std::string args, int32_t param1, int32_t param2,
int32_t param3) {
	
	
}

void FtWavesManager::ActivateWaveSpinners(Entity* self) {

	if (WaveNum == 4) {
		return;
	}
	
	if (WaveNum == 3) {
		if (LargeGroup == 0) {	
//			TODO: Play solo cine (final cine only)			
			self->AddTimer("GoldSpinnerMove", 0.3f);			
			
//			Solo player in cinematic so screw timings			
			self->AddTimer("SmallFinalSpinners", 2);
			self->AddTimer("SmallFinalEnemies", 2);			
		} else if (LargeGroup == 1) {	
		
		}
		return;
	}
	
	auto* proximityMonitorComponent = self->GetComponent<ProximityMonitorComponent>();
	
//	Cinematic stuff	
    Entity* engagedEntities[] = {engaged1, engaged2, engaged3, engaged4};

    for (Entity* engaged : engagedEntities) {
        if (engaged != nullptr && proximityMonitorComponent->IsInProximity("Room_Radius", engaged->GetObjectID())) {
            auto* buffComponent = engaged->GetComponent<BuffComponent>();
            if (buffComponent != nullptr) {
                buffComponent->RemoveBuff(60);
            }
            GameMessages::SendPlayCinematic(engaged->GetObjectID(), u"WavesSpawnerSpinners", engaged->GetSystemAddress());
			self->AddTimer("UnstunEnemies", 8.5f);	
        }
    }
//	End






	

//	Pull a fuckton info 
	auto blacksmithspawners = Game::zoneManager->GetSpawnersByName("Waves_Blacksmith_Spawner");
	auto handspawners = Game::zoneManager->GetSpawnersByName("Waves_Hand_Spawner");
	auto marksmanspawners = Game::zoneManager->GetSpawnersByName("Waves_Marksman_Spawner");
	auto scientistspawners = Game::zoneManager->GetSpawnersByName("Waves_Scientist_Spawner");
	auto wolfspawners = Game::zoneManager->GetSpawnersByName("Waves_Wolf_Spawner");
	auto beetlespawners = Game::zoneManager->GetSpawnersByName("Waves_Beetle_Spawner");
	auto bonezaispawners = Game::zoneManager->GetSpawnersByName("Waves_Bonezai_Spawner");	
	auto overseerspawners = Game::zoneManager->GetSpawnersByName("Waves_Overseer_Spawner");	
	
	auto* blacksmithspawner = blacksmithspawners.front();
	auto* handspawner = handspawners.front();
	auto* marksmanspawner = marksmanspawners.front();
	auto* scientistspawner = scientistspawners.front();
	auto* wolfspawner = wolfspawners.front();
	auto* beetlespawner = beetlespawners.front();
	auto* bonezaispawner = bonezaispawners.front();
	auto* overseerspawner = overseerspawners.front();
	
	blacksmithspawner->SetSpawnLot(32335);	
	handspawner->SetSpawnLot(14001);	
	marksmanspawner->SetSpawnLot(16848);
	scientistspawner->SetSpawnLot(16938);
	wolfspawner->SetSpawnLot(16850);
	beetlespawner->SetSpawnLot(32336);
	bonezaispawner->SetSpawnLot(16853);
	overseerspawner->SetSpawnLot(16846);
	

	auto scientistEntities = Game::entityManager->GetEntitiesByLOT(16938);	
	auto wolfEntities = Game::entityManager->GetEntitiesByLOT(16850);	
	auto beetleEntities = Game::entityManager->GetEntitiesByLOT(32336);	
	auto bonezaiEntities = Game::entityManager->GetEntitiesByLOT(16853);	
	auto overseerEntities = Game::entityManager->GetEntitiesByLOT(16846);	
//	End
	




//	Wave enemy spawning & spinner timers
	if (WaveNum == 1) {	


		HandleSpinner(self, "hand", "up");		
		HandleSpinner(self, "blacksmith", "up");	
		self->AddTimer("SpinnersDown", 10);		
		

		self->AddTimer("Wave1LeftSpinners", 3);
		self->AddTimer("Wave1RightEnemies", 3.3f);
		
	} else if (WaveNum == 2) {

//	Timings for spinners
		HandleSpinner(self, "hand", "up");		
		HandleSpinner(self, "scientist", "up");	
		self->AddTimer("Wave2LeftSpinners", 3);
		self->AddTimer("SpinnersDown", 10);			
		self->AddTimer("Wave2RightEnemies", 3.3f);	
		

	} else if (WaveNum == 3) {	
		if (LargeGroup == 1) {			
			for (auto i = 0; i < 3; i++) {
				handspawner->Spawn({ handspawner->m_Info.nodes.at(i % handspawner->m_Info.nodes.size()) }, true);								
			}
			for (auto i = 0; i < 2; i++) {
				wolfspawner->Spawn({ wolfspawner->m_Info.nodes.at(i % wolfspawner->m_Info.nodes.size()) }, true);			
				scientistspawner->Spawn({ scientistspawner->m_Info.nodes.at(i % scientistspawner->m_Info.nodes.size()) }, true);
				marksmanspawner->Spawn({ marksmanspawner->m_Info.nodes.at(i % marksmanspawner->m_Info.nodes.size()) }, true);				
				blacksmithspawner->Spawn({ blacksmithspawner->m_Info.nodes.at(i % blacksmithspawner->m_Info.nodes.size()) }, true);
				overseerspawner->Spawn({ overseerspawner->m_Info.nodes.at(i % overseerspawner->m_Info.nodes.size()) }, true);				
			}
			for (auto i = 0; i < 1; i++) {
				beetlespawner->Spawn({ beetlespawner->m_Info.nodes.at(i % beetlespawner->m_Info.nodes.size()) }, true);
				bonezaispawner->Spawn({ bonezaispawner->m_Info.nodes.at(i % bonezaispawner->m_Info.nodes.size()) }, true);			
			}
		} else if (LargeGroup == 0) {	
			for (auto i = 0; i < 1; i++) {
				wolfspawner->Spawn({ wolfspawner->m_Info.nodes.at(i % wolfspawner->m_Info.nodes.size()) }, true);
				beetlespawner->Spawn({ beetlespawner->m_Info.nodes.at(i % beetlespawner->m_Info.nodes.size()) }, true);
				handspawner->Spawn({ handspawner->m_Info.nodes.at(i % handspawner->m_Info.nodes.size()) }, true);				
				scientistspawner->Spawn({ scientistspawner->m_Info.nodes.at(i % scientistspawner->m_Info.nodes.size()) }, true);
				marksmanspawner->Spawn({ marksmanspawner->m_Info.nodes.at(i % marksmanspawner->m_Info.nodes.size()) }, true);				
				blacksmithspawner->Spawn({ blacksmithspawner->m_Info.nodes.at(i % blacksmithspawner->m_Info.nodes.size()) }, true);
				bonezaispawner->Spawn({ bonezaispawner->m_Info.nodes.at(i % bonezaispawner->m_Info.nodes.size()) }, true);
				overseerspawner->Spawn({ overseerspawner->m_Info.nodes.at(i % overseerspawner->m_Info.nodes.size()) }, true);				
			}
		}	
	}
	


//	Now catch each enemy 
//	auto handEntities = Game::entityManager->GetEntitiesByLOT(14001);
		

	
//	for (auto* hand : handEntities) {
//		const auto handID = hand->GetObjectID();	
		
//		hand->AddDieCallback([this, handID, hand]() {
//		hand->AddDieCallback([self, this]() {	
//			self->AddTimer("ShowDoor", 1);		
//		});	
//	}


}

void FtWavesManager::HandleSpinner(Entity* self, std::string spinner, std::string direction) {	
//	Get spinner entity
	auto BlacksmithSpinner = Game::entityManager->GetEntitiesInGroup("WavesSpinnerBlacksmith");
	auto HandSpinner = Game::entityManager->GetEntitiesInGroup("WavesSpinnerHand");	
	auto OverseerSpinner = Game::entityManager->GetEntitiesInGroup("WavesSpinnerOverseer");	
	auto MarksmanSpinner = Game::entityManager->GetEntitiesInGroup("WavesSpinnerMarksman");	
	auto WolfSpinner = Game::entityManager->GetEntitiesInGroup("WavesSpinnerWolf");	
	auto BeetleSpinner = Game::entityManager->GetEntitiesInGroup("WavesSpinnerBeetle");	
	auto ScientistSpinner = Game::entityManager->GetEntitiesInGroup("WavesSpinnerScientist");	
	auto BonezaiSpinner = Game::entityManager->GetEntitiesInGroup("WavesSpinnerBonezai");	
	auto GoldSpinner = Game::entityManager->GetEntitiesInGroup("WavesElevatorSpinner");		
	
    auto ProcessSpinnerUp = [](const std::vector<Entity*>& spinnerGroup) {
        for (auto* spinner : spinnerGroup) {
            GameMessages::SendPlatformResync(spinner, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);
            RenderComponent::PlayAnimation(spinner, u"up");
            GameMessages::SendPlayNDAudioEmitter(spinner, spinner->GetSystemAddress(), "{7f770ade-b84c-46ad-b3ae-bdbace5985d4}");
            GameMessages::SendPlayFXEffect(spinner->GetObjectID(), 10102, u"create", "create");
        }
    };
    auto ProcessSpinnerDown = [](const std::vector<Entity*>& spinnerGroup) {
        for (auto* spinner : spinnerGroup) {
            GameMessages::SendPlatformResync(spinner, UNASSIGNED_SYSTEM_ADDRESS, true, 0, 1, 1, eMovementPlatformState::Moving);
            RenderComponent::PlayAnimation(spinner, u"down");
            GameMessages::SendPlayNDAudioEmitter(spinner, spinner->GetSystemAddress(), "{97b60c03-51f2-45b6-80cc-ccbbef0d94cf}");
//            GameMessages::SendPlayFXEffect(spinner->GetObjectID(), 10102, u"create", "create");
        }
    };

    if (direction == "up") {
		
        if (spinner == "hand") {
			HandSpinnerUp = 1;
            ProcessSpinnerUp(HandSpinner);
        } else if (spinner == "blacksmith") {
			BlacksmithSpinnerUp = 1;		
            ProcessSpinnerUp(BlacksmithSpinner);
        } else if (spinner == "overseer") {
			OverseerSpinnerUp = 1;			
            ProcessSpinnerUp(OverseerSpinner);
        } else if (spinner == "marksman") {
			MarksmanSpinnerUp = 1;			
            ProcessSpinnerUp(MarksmanSpinner);
        } else if (spinner == "wolf") {
			WolfSpinnerUp = 1;		
            ProcessSpinnerUp(WolfSpinner);
        } else if (spinner == "beetle") {
			BeetleSpinnerUp = 1;		
            ProcessSpinnerUp(BeetleSpinner);
        } else if (spinner == "scientist") {
			ScientistSpinnerUp = 1;		
            ProcessSpinnerUp(ScientistSpinner);
        } else if (spinner == "bonezai") {
			BonezaiSpinnerUp = 1;			
            ProcessSpinnerUp(BonezaiSpinner);
        }
		
	} else if (direction == "down") {	
	
        if (spinner == "hand") {
			HandSpinnerUp = 0;
            ProcessSpinnerDown(HandSpinner);
        } else if (spinner == "blacksmith") {
			BlacksmithSpinnerUp = 0;			
            ProcessSpinnerDown(BlacksmithSpinner);
        } else if (spinner == "overseer") {
			OverseerSpinnerUp = 0;			
            ProcessSpinnerDown(OverseerSpinner);
        } else if (spinner == "marksman") {
			MarksmanSpinnerUp = 0;			
            ProcessSpinnerDown(MarksmanSpinner);
        } else if (spinner == "wolf") {
			WolfSpinnerUp = 0;		
            ProcessSpinnerDown(WolfSpinner);
        } else if (spinner == "beetle") {
			BeetleSpinnerUp = 0;	
            ProcessSpinnerDown(BeetleSpinner);
        } else if (spinner == "scientist") {
			ScientistSpinnerUp = 0;		
            ProcessSpinnerDown(ScientistSpinner);
        } else if (spinner == "bonezai") {
			BonezaiSpinnerUp = 0;			
            ProcessSpinnerDown(BonezaiSpinner);
        } else if (spinner == "gold") {
            ProcessSpinnerDown(GoldSpinner);			
		}	
	
	}

}

void FtWavesManager::CheckWaveProgress(Entity* self) {

//	Wave 1	
	if (LargeGroup == 1 && Wave1Progress == 7) {
		Wave1Progress = 9;			
		self->AddTimer("ShowDoor", 1);		
	}
	if (LargeGroup == 0 && Wave1Progress == 4) {
		Wave1Progress = 9;			
		self->AddTimer("ShowDoor", 1);	
	}	

//	Wave 2
	if (LargeGroup == 1 && Wave2Progress == 10) {
		Wave2Progress = 12;			
		self->AddTimer("ShowDoor", 1);		
	}
	if (LargeGroup == 0 && Wave2Progress == 6) {
		Wave2Progress = 12;			
		self->AddTimer("ShowDoor", 1);	
	}		
	
//	Wave 3	
}	

void FtWavesManager::OnTimerDone(Entity* self, std::string timerName) {
	
	if (timerName == "ShowDoor") {
		
//		More cinematic stuff
		auto* proximityMonitorComponent = self->GetComponent<ProximityMonitorComponent>();
		Entity* engagedEntities[] = {engaged1, engaged2, engaged3, engaged4};

		for (Entity* engaged : engagedEntities) {
			if (engaged != nullptr && proximityMonitorComponent->IsInProximity("Room_Radius", engaged->GetObjectID())) {
				auto* buffComponent = engaged->GetComponent<BuffComponent>();
				if (buffComponent != nullptr) {
					buffComponent->RemoveBuff(60);
				}
	
				if (WaveNum == 1) {
					GameMessages::SendPlayCinematic(engaged->GetObjectID(), u"WavesDoorCam_Torch01", engaged->GetSystemAddress());
				} else if (WaveNum == 2) {
					GameMessages::SendPlayCinematic(engaged->GetObjectID(), u"WavesDoorCam_Torch02", engaged->GetSystemAddress());
				} else if (WaveNum == 3) {
					GameMessages::SendPlayCinematic(engaged->GetObjectID(), u"WavesDoorCam_Torch03", engaged->GetSystemAddress());
				}
			}
		}
//		End



		self->AddTimer("TurnOnIndicator", 1);

		self->AddTimer("SpawnNextWave", 14);
	
	} else if (timerName == "TurnOnIndicator") {
//	Get proper door indicator		
		if (WaveNum == 1) {	



//Start
//			const auto DeactivatedEvent = GeneralUtils::UTF16ToWTF8(self->GetVar<std::u16string>
//			(u"name_deactivated_event"));
//			const auto SpinnersGroup = GeneralUtils::UTF16ToWTF8(self->GetVar<std::u16string>(u"groupID"));
			auto groupObjs = Game::entityManager->GetEntitiesByLOT(16894);	
			
//			if (DeactivatedEvent == "spinnerdown1") {		
			for (auto* obj : groupObjs) {
				if (obj) {
//					const auto IndicatorId = GeneralUtils::UTF16ToWTF8(obj->GetVar<std::u16string>
//					(u"groupID"));
					const auto IndicatorId = obj->GetVar<std::u16string>(u"groupID");
					
					if (IndicatorId == u"WavesDoorIndicator1") {		
						obj->SetNetworkVar<bool>(u"FlameOn", true);	
					}
				}
			}
//			}	
//		End			

		
		
			WaveNum = 2;			
		} else if (WaveNum == 2) {	
		
		
			WaveNum = 3;			
		} else if (WaveNum == 3) {			


			WaveNum = 4;		
		}
	} else if (timerName == "SpawnNextWave") {
		ActivateWaveSpinners(self);			
	
	} else if (timerName == "SpinnersDown") {
		if (HandSpinnerUp == 1) {	
			HandleSpinner(self, "hand", "down");			
		} if (BlacksmithSpinnerUp == 1) {
			HandleSpinner(self, "blacksmith", "down");			
		} if (OverseerSpinnerUp == 1) {
			HandleSpinner(self, "overseer", "down");				
		} if (MarksmanSpinnerUp == 1) {
			HandleSpinner(self, "marksman", "down");	
		} if (WolfSpinnerUp == 1) {
			HandleSpinner(self, "wolf", "down");	
		} if (BeetleSpinnerUp == 1) {
			HandleSpinner(self, "beetle", "down");	
		} if (ScientistSpinnerUp == 1) {
			HandleSpinner(self, "scientist", "down");	
		} if (BonezaiSpinnerUp == 1) {
			HandleSpinner(self, "bonezai", "down");				
		}	

	} else if (timerName == "Wave1LeftSpinners") {
		self->AddTimer("Wave1LeftEnemies", 3.3f);
		HandleSpinner(self, "marksman", "up");

	} else if (timerName == "Wave2LeftSpinners") {
		self->AddTimer("Wave2LeftEnemies", 3.3f);
		HandleSpinner(self, "wolf", "up");
		HandleSpinner(self, "beetle", "up");
	
	
	} else if (timerName == "Wave1RightEnemies") {
		
		auto blacksmithspawners = Game::zoneManager->GetSpawnersByName("Waves_Blacksmith_Spawner");
		auto handspawners = Game::zoneManager->GetSpawnersByName("Waves_Hand_Spawner");		
		auto* blacksmithspawner = blacksmithspawners.front();
		auto* handspawner = handspawners.front();		
		blacksmithspawner->SetSpawnLot(32335);	
		handspawner->SetSpawnLot(14001);		
		
		if (LargeGroup == 1) {	
			for (auto i = 0; i < 3; i++) {
				blacksmithspawner->Spawn({ blacksmithspawner->m_Info.nodes.at(i % blacksmithspawner->m_Info.nodes.size()) }, true);
			}
			for (auto i = 0; i < 2; i++) {
				handspawner->Spawn({ handspawner->m_Info.nodes.at(i % handspawner->m_Info.nodes.size()) }, true);
			}
		} else if (LargeGroup == 0) {	
			for (auto i = 0; i < 2; i++) {
				blacksmithspawner->Spawn({ blacksmithspawner->m_Info.nodes.at(i % blacksmithspawner->m_Info.nodes.size()) }, true);
			}
			for (auto i = 0; i < 1; i++) {
				handspawner->Spawn({ handspawner->m_Info.nodes.at(i % handspawner->m_Info.nodes.size()) }, true);
			}
		}

		
		auto blacksmithEntities = Game::entityManager->GetEntitiesByLOT(32335);	
		auto handEntities = Game::entityManager->GetEntitiesByLOT(14001);		
		
		for (auto* hand : handEntities) {
			auto* CombatAI = hand->GetComponent<BaseCombatAIComponent>();
			if (CombatAI != nullptr) {
				CombatAI->SetDisabled(true);
			}			
			hand->AddDieCallback([self, this]() {	
				Wave1Progress++;	
				CheckWaveProgress(self);					
			});	
		}
		for (auto* blacksmith : blacksmithEntities) {
			auto* CombatAI = blacksmith->GetComponent<BaseCombatAIComponent>();
			if (CombatAI != nullptr) {
				CombatAI->SetDisabled(true);
			}	
			blacksmith->AddDieCallback([self, this]() {	
				Wave1Progress++;
				CheckWaveProgress(self);	
			});	
		}			
	} else if (timerName == "Wave2RightEnemies") {
		
		auto handspawners = Game::zoneManager->GetSpawnersByName("Waves_Hand_Spawner");	
		auto scientistspawners = Game::zoneManager->GetSpawnersByName("Waves_Scientist_Spawner");	
		auto* handspawner = handspawners.front();
		auto* scientistspawner = scientistspawners.front();	
		handspawner->SetSpawnLot(14001);
		scientistspawner->SetSpawnLot(16938);		
		
		if (LargeGroup == 1) {			
			for (auto i = 0; i < 4; i++) {
				scientistspawner->Spawn({ scientistspawner->m_Info.nodes.at(i % scientistspawner->m_Info.nodes.size()) }, true);
			}
			for (auto i = 0; i < 2; i++) {
				handspawner->Spawn({ handspawner->m_Info.nodes.at(i % handspawner->m_Info.nodes.size()) }, true);
			}
		} else if (LargeGroup == 0) {	
			for (auto i = 0; i < 2; i++) {
				handspawner->Spawn({ handspawner->m_Info.nodes.at(i % handspawner->m_Info.nodes.size()) }, true);				
				scientistspawner->Spawn({ scientistspawner->m_Info.nodes.at(i % scientistspawner->m_Info.nodes.size()) }, true);
			}
		}

		auto handEntities = Game::entityManager->GetEntitiesByLOT(14001);
		auto scientistEntities = Game::entityManager->GetEntitiesByLOT(16938);				
		
		for (auto* hand : handEntities) {
			auto* CombatAI = hand->GetComponent<BaseCombatAIComponent>();
			if (CombatAI != nullptr) {
				CombatAI->SetDisabled(true);
			}	
			hand->AddDieCallback([self, this]() {	
				Wave2Progress++;	
				CheckWaveProgress(self);					
			});	
		}
		for (auto* scientist : scientistEntities) {
			auto* CombatAI = scientist->GetComponent<BaseCombatAIComponent>();
			if (CombatAI != nullptr) {
				CombatAI->SetDisabled(true);
			}	
			scientist->AddDieCallback([self, this]() {	
				Wave2Progress++;
				CheckWaveProgress(self);	
			});	
		}					
	
	} else if (timerName == "Wave1LeftEnemies") {
		
		auto marksmanspawners = Game::zoneManager->GetSpawnersByName("Waves_Marksman_Spawner");
		auto* marksmanspawner = marksmanspawners.front();	
		marksmanspawner->SetSpawnLot(16848);
		
		if (LargeGroup == 1) {	
			for (auto i = 0; i < 2; i++) {
				marksmanspawner->Spawn({ marksmanspawner->m_Info.nodes.at(i % marksmanspawner->m_Info.nodes.size()) }, true);
			}
		} else if (LargeGroup == 0) {	
			for (auto i = 0; i < 1; i++) {
				marksmanspawner->Spawn({ marksmanspawner->m_Info.nodes.at(i % marksmanspawner->m_Info.nodes.size()) }, true);
			}
		}

		auto marksmanEntities = Game::entityManager->GetEntitiesByLOT(16848);
		
		for (auto* marksman : marksmanEntities) {
			auto* CombatAI = marksman->GetComponent<BaseCombatAIComponent>();
			if (CombatAI != nullptr) {
				CombatAI->SetDisabled(true);
			}	
			marksman->AddDieCallback([self, this]() {	
				Wave1Progress++;
				CheckWaveProgress(self);				
			});	
		}
		
	} else if (timerName == "Wave2LeftEnemies") {
		
		auto wolfspawners = Game::zoneManager->GetSpawnersByName("Waves_Wolf_Spawner");
		auto beetlespawners = Game::zoneManager->GetSpawnersByName("Waves_Beetle_Spawner");		
		auto* wolfspawner = wolfspawners.front();
		auto* beetlespawner = beetlespawners.front();
		
		wolfspawner->SetSpawnLot(16850);
		beetlespawner->SetSpawnLot(32336);		
		
		if (LargeGroup == 1) {			
			for (auto i = 0; i < 2; i++) {
				wolfspawner->Spawn({ wolfspawner->m_Info.nodes.at(i % wolfspawner->m_Info.nodes.size()) }, true);
				beetlespawner->Spawn({ beetlespawner->m_Info.nodes.at(i % beetlespawner->m_Info.nodes.size()) }, true);
			}
		} else if (LargeGroup == 0) {	
			for (auto i = 0; i < 1; i++) {
				wolfspawner->Spawn({ wolfspawner->m_Info.nodes.at(i % wolfspawner->m_Info.nodes.size()) }, true);
				beetlespawner->Spawn({ beetlespawner->m_Info.nodes.at(i % beetlespawner->m_Info.nodes.size()) }, true);
			}
		}

		auto beetleEntities = Game::entityManager->GetEntitiesByLOT(32336);		
		auto wolfEntities = Game::entityManager->GetEntitiesByLOT(16850);			
		

		for (auto* beetle : beetleEntities) {
			auto* CombatAI = beetle->GetComponent<BaseCombatAIComponent>();
			if (CombatAI != nullptr) {
				CombatAI->SetDisabled(true);
			}	
			beetle->AddDieCallback([self, this]() {	
				Wave2Progress++;
				CheckWaveProgress(self);				
			});	
		}	
		for (auto* wolf : wolfEntities) {
			auto* CombatAI = wolf->GetComponent<BaseCombatAIComponent>();
			if (CombatAI != nullptr) {
				CombatAI->SetDisabled(true);
			}	
			wolf->AddDieCallback([self, this]() {	
				Wave2Progress++;
				CheckWaveProgress(self);	
			});	
		}	
		
	} else if (timerName == "UnstunEnemies") {
		struct {
			uint32_t LOT;
			std::vector<Entity*> entities;
		} entityGroups[] = {
			{ 16938, Game::entityManager->GetEntitiesByLOT(16938) },   // scientistEntities
			{ 16850, Game::entityManager->GetEntitiesByLOT(16850) },   // wolfEntities
			{ 32336, Game::entityManager->GetEntitiesByLOT(32336) },   // beetleEntities
			{ 16853, Game::entityManager->GetEntitiesByLOT(16853) },   // bonezaiEntities
			{ 16846, Game::entityManager->GetEntitiesByLOT(16846) },   // overseerEntities
			{ 16848, Game::entityManager->GetEntitiesByLOT(16848) },   // marksmanEntities
			{ 14001, Game::entityManager->GetEntitiesByLOT(14001) },   // handEntities
			{ 32335, Game::entityManager->GetEntitiesByLOT(32335) }    // blacksmithEntities
		};

		for (auto& group : entityGroups) {
			for (auto* entity : group.entities) {
				auto* CombatAI = entity->GetComponent<BaseCombatAIComponent>();
				if (CombatAI != nullptr) {
					CombatAI->SetDisabled(false);
				}
			}
		}
	} else if (timerName == "GoldSpinnerMove") {
		HandleSpinner(self, "gold", "down");			
	} else if (timerName == "SmallFinalSpinnersUp") {		
		HandleSpinner(self, "hand", "up");
		HandleSpinner(self, "wolf", "up");	
		HandleSpinner(self, "beetle", "up");
		HandleSpinner(self, "bonezai", "up");			
		HandleSpinner(self, "marksman", "up");	
		HandleSpinner(self, "overseer", "up");	
		HandleSpinner(self, "scientist", "up");			
		HandleSpinner(self, "blacksmith", "up");				
	} else if (timerName == "SmallFinalEnemies") {	
//		Pull a fuckton info 
		auto blacksmithspawners = Game::zoneManager->GetSpawnersByName("Waves_Blacksmith_Spawner");
		auto handspawners = Game::zoneManager->GetSpawnersByName("Waves_Hand_Spawner");
		auto marksmanspawners = Game::zoneManager->GetSpawnersByName("Waves_Marksman_Spawner");
		auto scientistspawners = Game::zoneManager->GetSpawnersByName("Waves_Scientist_Spawner");
		auto wolfspawners = Game::zoneManager->GetSpawnersByName("Waves_Wolf_Spawner");
		auto beetlespawners = Game::zoneManager->GetSpawnersByName("Waves_Beetle_Spawner");
		auto bonezaispawners = Game::zoneManager->GetSpawnersByName("Waves_Bonezai_Spawner");	
		auto overseerspawners = Game::zoneManager->GetSpawnersByName("Waves_Overseer_Spawner");	
		
		auto* blacksmithspawner = blacksmithspawners.front();
		auto* handspawner = handspawners.front();
		auto* marksmanspawner = marksmanspawners.front();
		auto* scientistspawner = scientistspawners.front();
		auto* wolfspawner = wolfspawners.front();
		auto* beetlespawner = beetlespawners.front();
		auto* bonezaispawner = bonezaispawners.front();
		auto* overseerspawner = overseerspawners.front();
		
		blacksmithspawner->SetSpawnLot(32335);	
		handspawner->SetSpawnLot(14001);	
		marksmanspawner->SetSpawnLot(16848);
		scientistspawner->SetSpawnLot(16938);
		wolfspawner->SetSpawnLot(16850);
		beetlespawner->SetSpawnLot(32336);
		bonezaispawner->SetSpawnLot(16853);
		overseerspawner->SetSpawnLot(16846);	
		
		
		if (LargeGroup == 1) {			
			for (auto i = 0; i < 3; i++) {
				handspawner->Spawn({ handspawner->m_Info.nodes.at(i % handspawner->m_Info.nodes.size()) }, true);								
			}
			for (auto i = 0; i < 2; i++) {
				wolfspawner->Spawn({ wolfspawner->m_Info.nodes.at(i % wolfspawner->m_Info.nodes.size()) }, true);			
				scientistspawner->Spawn({ scientistspawner->m_Info.nodes.at(i % scientistspawner->m_Info.nodes.size()) }, true);
				marksmanspawner->Spawn({ marksmanspawner->m_Info.nodes.at(i % marksmanspawner->m_Info.nodes.size()) }, true);				
				blacksmithspawner->Spawn({ blacksmithspawner->m_Info.nodes.at(i % blacksmithspawner->m_Info.nodes.size()) }, true);
				overseerspawner->Spawn({ overseerspawner->m_Info.nodes.at(i % overseerspawner->m_Info.nodes.size()) }, true);				
			}
			for (auto i = 0; i < 1; i++) {
				beetlespawner->Spawn({ beetlespawner->m_Info.nodes.at(i % beetlespawner->m_Info.nodes.size()) }, true);
				bonezaispawner->Spawn({ bonezaispawner->m_Info.nodes.at(i % bonezaispawner->m_Info.nodes.size()) }, true);			
			}
		} else if (LargeGroup == 0) {	
			for (auto i = 0; i < 1; i++) {
				wolfspawner->Spawn({ wolfspawner->m_Info.nodes.at(i % wolfspawner->m_Info.nodes.size()) }, true);
				beetlespawner->Spawn({ beetlespawner->m_Info.nodes.at(i % beetlespawner->m_Info.nodes.size()) }, true);
				handspawner->Spawn({ handspawner->m_Info.nodes.at(i % handspawner->m_Info.nodes.size()) }, true);				
				scientistspawner->Spawn({ scientistspawner->m_Info.nodes.at(i % scientistspawner->m_Info.nodes.size()) }, true);
				marksmanspawner->Spawn({ marksmanspawner->m_Info.nodes.at(i % marksmanspawner->m_Info.nodes.size()) }, true);				
				blacksmithspawner->Spawn({ blacksmithspawner->m_Info.nodes.at(i % blacksmithspawner->m_Info.nodes.size()) }, true);
				bonezaispawner->Spawn({ bonezaispawner->m_Info.nodes.at(i % bonezaispawner->m_Info.nodes.size()) }, true);
				overseerspawner->Spawn({ overseerspawner->m_Info.nodes.at(i % overseerspawner->m_Info.nodes.size()) }, true);				
			}
		}	

		std::vector<std::vector<Entity*>> entityGroups = {
			Game::entityManager->GetEntitiesByLOT(16850), // wolfEntities
			Game::entityManager->GetEntitiesByLOT(16938), // scientistEntities
			Game::entityManager->GetEntitiesByLOT(32336), // beetleEntities
			Game::entityManager->GetEntitiesByLOT(16853), // bonezaiEntities
			Game::entityManager->GetEntitiesByLOT(16846), // overseerEntities
			Game::entityManager->GetEntitiesByLOT(14001), // handEntities
			Game::entityManager->GetEntitiesByLOT(16848), // marksmanEntities
			Game::entityManager->GetEntitiesByLOT(32335)  // blacksmithEntities
		};

		for (auto& entityGroup : entityGroups) {
			for (auto* entity : entityGroup) {
				auto* CombatAI = entity->GetComponent<BaseCombatAIComponent>();
				if (CombatAI != nullptr) {
					CombatAI->SetDisabled(true);
				}    
				entity->AddDieCallback([self, this]() {    
					Wave3Progress++;
					CheckWaveProgress(self);    
				});    
			}
		}
		
	}
}

