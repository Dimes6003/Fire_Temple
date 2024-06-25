#include "Spinner06.h"
#include "ProximityMonitorComponent.h"
#include "EntityManager.h"
#include "DestroyableComponent.h"
#include "MovingPlatformComponent.h"
#include "EntityInfo.h"
#include "GameMessages.h"
#include "MissionComponent.h"
#include "RenderComponent.h"
#include "eStateChangeType.h"

void Spinner06::OnStartup(Entity* self) {
	self->SetNetworkVar(u"bIsInUse", false);
	self->SetVar(u"bActive", true);
	self->AddTimer("IdleUpAnim", 3);

	self->AddTimer("BladeGUID", 10.7f);

	SpawnLegs(self, "Rear");	
}

void Spinner06::SpawnLegs(Entity* self, const std::string& loc) {
	auto pos = self->GetPosition();
	auto rot = self->GetRotation();
	pos.y += self->GetVarAs<float>(u"vert_offset");

	auto newRot = rot;
	auto offset = self->GetVarAs<float>(u"hort_offset");

	auto legLOT = self->GetVar<LOT>(u"legLOT");

	if (legLOT == 0) {
		return;
	}

	std::vector<LDFBaseData*> config = { new LDFData<std::string>(u"Leg", loc) };

	EntityInfo info{};
	info.lot = legLOT;
	info.spawnerID = self->GetObjectID();
	info.settings = config;
	info.rot = newRot;
	
//if spinner down to up: pos.y = Spinner starting pos (aka Waypoint 1) - 0.1
	if (loc == "Right") {
		const auto dir = rot.GetForwardVector();
		pos.x += dir.x * offset;
		pos.z += dir.z * offset;
		info.pos = pos;
	} else if (loc == "Rear") {
		const auto dir = rot.GetRightVector();
		pos.x = 101.04268;
		pos.y = 290.84376;
		pos.z = 85.66721;
		info.pos = pos;
	} else if (loc == "Left") {
		const auto dir = rot.GetForwardVector() * -1;
		pos.x += dir.x * offset;
		pos.z += dir.z * offset;
		info.pos = pos;
	}

	info.rot = NiQuaternion::LookAt(info.pos, self->GetPosition());

	auto* entity = Game::entityManager->CreateEntity(info);

	Game::entityManager->ConstructEntity(entity);

	OnChildLoaded(self, entity);
}

void Spinner06::OnChildLoaded(Entity* self, Entity* child) {
	
	auto legTable = self->GetVar<std::vector<LWOOBJID>>(u"legTable");

	legTable.push_back(child->GetObjectID());

	self->SetVar(u"legTable", legTable);

	const auto selfID = self->GetObjectID();

	child->AddDieCallback([this, selfID, child]() {
		auto* self = Game::entityManager->GetEntity(selfID);
		auto* destroyableComponent = child->GetComponent<DestroyableComponent>();

		if (destroyableComponent == nullptr || self == nullptr) {
			return;
		}

		NotifyDie(self, child, destroyableComponent->GetKiller());
		});
}

void Spinner06::NotifyDie(Entity* self, Entity* other, Entity* killer) {
	auto players = self->GetVar<std::vector<LWOOBJID>>(u"Players");

	const auto& iter = std::find(players.begin(), players.end(), killer->GetObjectID());

	if (iter == players.end()) {
		players.push_back(killer->GetObjectID());
	}

	self->SetVar(u"Players", players);

	OnChildRemoved(self, other);
	

}

void Spinner06::OnChildRemoved(Entity* self, Entity* child) {
	auto legTable = self->GetVar<std::vector<LWOOBJID>>(u"legTable");

	const auto& iter = std::find(legTable.begin(), legTable.end(), child->GetObjectID());

	if (iter != legTable.end()) {
		legTable.erase(iter);
	}

	self->SetVar(u"legTable", legTable);

	if (legTable.empty()) {
		self->CancelTimer("IdleUpAnim");		
		
//		Move to waypoint 
		GameMessages::SendPlatformResync(self, UNASSIGNED_SYSTEM_ADDRESS, true, 1, 0, 0, eMovementPlatformState::Moving);	

//		Descend sfx
		self->SetVar(u"bActive", false);
		GameMessages::SendStopNDAudioEmitter(self, self->GetSystemAddress(), "{ab21b048-5d1a-40b3-9203-88b376f92087}");	
		GameMessages::SendStopNDAudioEmitter(self, self->GetSystemAddress(), "{33a2e1c6-7911-4eca-a232-77f0c10afbd2}");	
		GameMessages::SendStopNDAudioEmitter(self, self->GetSystemAddress(), "{2bdb2891-b3ab-4eb1-a288-aed75784f6bd}");	
		GameMessages::SendStopNDAudioEmitter(self, self->GetSystemAddress(), "{19423553-7ac9-4dd7-a578-0d7c39337ed4}");			
		
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{40e86d71-084c-4149-884e-ab9b45b694dc}");	
		self->AddTimer("DescentGUID", 0.1f);	
		
//		End

		RenderComponent::PlayAnimation(self, u"down");
		self->AddTimer("DownAnim", 0.1);
	}

	auto deadLegs = self->GetVar<std::vector<std::string>>(u"DeadLegs");

	const auto& leg = child->GetVar<std::string>(u"Leg");

	const auto& legIter = std::find(deadLegs.begin(), deadLegs.end(), leg);

	if (legIter == deadLegs.end()) {
		deadLegs.push_back(leg);
	}

	self->SetVar(u"DeadLegs", deadLegs);

}

void Spinner06::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName == "DownAnim") {	
		RenderComponent::PlayAnimation(self, u"down");
	}	
	if (timerName == "IdleUpAnim") {	
		const auto AttachedPath = self->GetVar<std::u16string>(u"attached_path");
		if (AttachedPath != u"ZSpinner21") {
			RenderComponent::PlayAnimation(self, u"idle-up");
			self->AddTimer("IdleUpAnim", 1);
		}
	}	
	
//	Handle spinner sound orders
	if (timerName == "DescentGUID") {
		GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{97b60c03-51f2-45b6-80cc-ccbbef0d94cf}");	
	}
	if (timerName == "BladeGUID") {
		
//	Optional > include lines below to give each spinner their own sound 
//	But will cause the sfx to have a bad echo	
		
//		const auto AttachedPath = self->GetVar<std::u16string>(u"attached_path");		
//		if (AttachedPath == u"ZSpinner03") {
			GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{ab21b048-5d1a-40b3-9203-88b376f92087}");		
//		} else if (AttachedPath == u"ZSpinner04") {
//			GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{33a2e1c6-7911-4eca-a232-77f0c10afbd2}");			
//		} else if (AttachedPath == u"ZSpinner05") {
//			GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{2bdb2891-b3ab-4eb1-a288-aed75784f6bd}");
//		} else if (AttachedPath == u"ZSpinner06") {		
//			GameMessages::SendPlayNDAudioEmitter(self, self->GetSystemAddress(), "{19423553-7ac9-4dd7-a578-0d7c39337ed4}");
//		}
	}
}

