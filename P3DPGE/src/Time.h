#pragma once
#include "Debug.h"
#include <time.h>
//#include <exception>

//more debug macros
//this may not be necessary but is qol so

//these names are confusing and should be redone
//precise timers
#define PTIMER_S	Time::StartTimerPrecise()
#define PTIMER_E	Time::EndTimerPrecise //must pass no arguments or some
#define PTIMER_EA	Time::EndTimerAveragePrecise //must pass number to average by
#define PTIMER_EG	Time::GetEndTimerPrecise()
#define PTIMER_GE	Time::GetElapsedTimerPrecise()

//normal timers
#define TIMER_S		Time::StartTimer()
#define TIMER_E		Time::EndTimer //must pass no arguments or some
#define TIMER_EA	Time::EndTimerAverage //must pass number to average by
#define TIMER_EG	Time::GetEndTimer()
#define TIMER_GE	Time::GetElapsedTimer()

//qol (maybe)?
//this is most definitely evil!!
#define PTIMER_COND(a, b) if(PTIMER_GE > a) {b; PTIMER_E();}

namespace Time {
	global_variable float deltaTime;
	global_variable float totalTime;
	global_variable int64_t updateCount;

	global_variable float fixedTimeStep;
	global_variable float fixedDeltaTime;

	internal float fixedCounter;

	static float GetTotalTime() { return totalTime; }

	static void Init() {
		deltaTime = 0;
		totalTime = 0;
		updateCount = 0;

		fixedTimeStep = 50.f;
		fixedDeltaTime = 1.f / fixedTimeStep;
		fixedCounter = 0;
	}

	static void FixedUpdate() {
		fixedCounter = 0;
		//if (deltaTime > fixedDeltaTime) throw deltaTimeException;

		//physics
		//Physics::Update(Time::fixedDeltaTime);

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

		srand(time(NULL));
	}

	static void Cleanup() {}

//// Timer ////

	//NOTE sushi: we may want this to be its own class because that way we can easily create
	//			  multiple timer objects if we wish to. otherwise I think we'd have to store
	//			  them inside this namespace in some vector but that would proabably cause
	//			  data manipulation mix ups.

	static int elapsedFrames = 0;

	static steady_clock::time_point start_precise;
	static steady_clock::time_point end_precise;
	static std::list<double> times_precise;

	static float start;
	static float end;
	static std::list<float> times;

	static bool timer_running = false;
	static bool precise_timer_running = false;

//// Normal Timers ////

	static void StartTimer() { 
		if (!timer_running) { 
			start = Time::totalTime; 
			timer_running = true; 
		} 
	}

	static void EndTimer(std::string message = "") {
		end = totalTime;
		DEBUG_M(std::to_string(end - start));
		timer_running = false;
	}

	static void EndTimer(float goodTime, std::string message = "") {
		end = totalTime;
		float t = end - start;
		if (t > goodTime) { DEBUG_E(message + std::to_string(t)); }
		else if (t < goodTime) { DEBUG_S(message + std::to_string(t)); }
		else { DEBUG_M(message + std::to_string(t)); }
		timer_running = false;
	}

	static void EndTimerAverage(int num, std::string message = "", int nFrames = 1) {
		end = totalTime;
		float t = end - start;

		if (times.size() < num) { times_precise.push_back(t); }
		else { times.pop_front(); times_precise.push_back(t); }

		if (elapsedFrames >= nFrames) { DEBUG_M(message + std::to_string(Math::average(times, times.size()))); elapsedFrames = 0; }
		else { elapsedFrames++; }
		timer_running = false;
	}

	static float GetEndTimer() {
		end = totalTime;
		return end - start;
		timer_running = false;
	}

	static float GetElapsedTimer() {
		end = GetTotalTime();
		DEBUG_M(end);
		return end - start;
	}

//// Precise Timers ////

	//this is primarily for debug purposes so it shouldn't be used in any actual
	//engine/game implementation.
	static void StartTimerPrecise() {
		if (!precise_timer_running) {
			start_precise = steady_clock::now();
			precise_timer_running = true;
		}
	}

	static void EndTimerPrecise(std::string message = "") {
		end_precise = steady_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(end_precise - start_precise);
		DEBUG_M(message + std::to_string(time_span.count()));
		precise_timer_running = false;
	}

	static void EndTimerPrecise(double goodTime, std::string message = "") {
		end_precise = steady_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(end_precise - start_precise);
		double t = time_span.count();
		if (t > goodTime) { DEBUG_E(message + std::to_string(t)); }
		else if (t < goodTime) { DEBUG_S(message + std::to_string(t)); }
		else { DEBUG_M(message + std::to_string(t)); }
		precise_timer_running = false;
	}

	//prints an average time every nFrames
	//dunno if this would be useful, also probably not the most efficient thing ever
	//could make a conditional version but idk yet
	static void EndTimerAveragePrecise(int num, std::string message = "", int nFrames = 1) {
		end_precise = steady_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(end_precise - start_precise);
		double t = time_span.count();

		if (times_precise.size() < num) { times_precise.push_back(t); }
		else { times_precise.pop_front(); times_precise.push_back(t); }

		if (elapsedFrames >= nFrames) { DEBUG_M(message + std::to_string(Math::average(times, times.size()))); elapsedFrames = 0; }
		else { elapsedFrames++; }
	}

	//for timing things
	//this can potentially become innaccurate in certain situations
	//like with frame rate issues and such
	//also I don't know if we'd want to use this for actual timing
	//actually I shoud probably change this to use time!
	static double GetEndTimerPrecise() {
		end_precise = steady_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(end_precise - start_precise);
		return time_span.count();
		precise_timer_running = false;
	}
	
	static double GetElapsedTimerPrecise() {
		end_precise = steady_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(end_precise - start_precise);
		return time_span.count();
	}
};