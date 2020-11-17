#pragma once
//#include <exception>

//TODO(u,delle,11/17/20) look into this exception and whether it is necessary
//class DeltaTimeException : public std::exception {
//	virtual const char* print() const throw() {
//		return "The delta time of the program went less than the fixed delta time, which should never happen.";
//	}
//} static deltaTimeException;

namespace Time {
	static float deltaTime;
	static float totalTime;
	static int64_t updateCount;

	static float fixedTimeStep;
	static float fixedDeltaTime;
	
	namespace { //private variables
		static float fixedCounter;
	}

	static void Init() {
		deltaTime = 0;
		totalTime = 0;
		updateCount = 0;

		fixedTimeStep = 50.f;
		fixedDeltaTime = 1.f/fixedTimeStep;
		fixedCounter = 0;
	}

	static void FixedUpdate() {
		fixedCounter = 0;
		//if (deltaTime > fixedDeltaTime) throw deltaTimeException;

		//physics
		Physics::Update(Time::fixedDeltaTime);

		//std::cout << "Fixed Update :)" << std::endl;
	}

	static void Update(float& fElapsedTime) {
		deltaTime = fElapsedTime;
		totalTime += deltaTime;
		updateCount += 1;

		fixedCounter += deltaTime;
		if (fixedCounter > fixedDeltaTime) {
			FixedUpdate();
		}
		//std::cout << deltaTime << std::endl;
	}

	static void Cleanup() {}
	
};