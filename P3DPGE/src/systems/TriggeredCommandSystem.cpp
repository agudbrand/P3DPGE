#include "TriggeredCommandSystem.h"

#include "../utils/Command.h"

void TriggeredCommandSystem::Update() {
	//execute all triggered commands
	for(auto& c : admin->commands) {
		if(c.second->triggered) {
			c.second->Exec(admin);
			c.second->triggered = false;
		}
	}
}