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

	Command(CommandAction action, std::string name, std::string description = "") {
		this->triggered = false;
		this->action = action;
		this->name = name;
		this->description = description;
	}

	//execute command action function
	inline void Exec(EntityAdmin* admin) const {
		LOG("Executing command: ", name);
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