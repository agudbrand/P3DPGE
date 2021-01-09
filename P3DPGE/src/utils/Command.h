#pragma once
#include "Debug.h"

struct EntityAdmin;

typedef void (*CommandAction)(EntityAdmin* admin);
//typedef void (*CommandActionArgs)(EntityAdmin* admin, std::string args);

struct Command {
	CommandAction action;
	bool triggered;
	std::string name;
	std::string description;

	static bool CONSOLE_PRINT_EXEC;

	Command(CommandAction action, std::string name, std::string description = "") {
		this->triggered = false;
		this->action = action;
		this->name = name;
		this->description = description;
	}

	//execute command action function
	inline void Exec(EntityAdmin* admin) const {
		//DEBUG if(CONSOLE_PRINT_EXEC) LOG("Executing command: ", name);
		DEBUG if(CONSOLE_PRINT_EXEC) Debug::ToString(1, std::string("Executing command: ") + name, true);
		action(admin);
	}
};

//TODO(i,delle) maybe add delayed and repeating commands
//struct DelayedCommand : public Command { 
//	float delay;
//
//	DelayedCommand(CommandAction action, std::string name, float delay, std::string description = "") {
//		this->triggered = false;
//		this->action = action;
//		this->name = name;
//		this->description = description;
//	}
//
//	inline void operator () (EntityAdmin* admin) const {
//		action(admin);
//	}
//};