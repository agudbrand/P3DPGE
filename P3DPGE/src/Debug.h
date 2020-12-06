#pragma once

#include "olcPixelGameEngine.h"
#include <string>
#include <iostream>
#include <cstdarg>
#include <windows.h>
#include <chrono>

#define internal static
#define local_persist static 
#define global_variable static

#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif

typedef signed char		int8;
typedef signed short	int16;
typedef signed int		int32;
typedef signed long		int64;
typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;
typedef unsigned long	uint64;

using namespace std::chrono;

namespace Math { //forward declare average
	template<class T>
	static double average(const T& container, int size);
}

namespace Debug {
	internal HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);;
	internal int color = 7;
	internal void ResetCmd() { //resets cmd font color to white
		color = 7;
		SetConsoleTextAttribute(hConsole, color);
	}

	////Console Output////

	/*static void Print(std::string& str, ...) {
		const char* fmt = str.c_str();
		va_list args;
		va_start(args, fmt);

		while (*fmt != '\0') {
			if (*fmt == '%') {
				++fmt;
				if(*fmt == 's'){
					std::string s = va_arg(args, std::string);
					std::cout << s;
				} else if (*fmt == 'i' || *fmt == 'd') {
					int i = va_arg(args, int);
					std::cout << i;
				} else if (*fmt == 'f' || *fmt == '.') {
					double f = va_arg(args, double);
					if (*fmt == '.') {
						int decimals = std::atoi(++fmt);
					}
				}
			}
			++fmt;
		}

		va_end(args);
	}*/

	/*static void Print(std::string& str, ...) {
		va_list args;
		std::vprintf(str.c_str(), args);
	}

	static void Print(const char* str, ...) {
		va_list args;
		std::vprintf(str, args);
	}*/

	//Sends a red string to the console
	static void Error(const std::string& str) {
		color = 4;
		SetConsoleTextAttribute(hConsole, color);
		std::cout << str << std::endl;
		ResetCmd();
	}

	//Sends a red string literal to the console
	static void Error(const char* str) {
		color = 4;
		SetConsoleTextAttribute(hConsole, color);
		std::cout << str << std::endl;
		ResetCmd();
	}

	//Sends a yellow message to console
	static void Message(const std::string& str) {
		color = 6;
		SetConsoleTextAttribute(hConsole, color);
		std::cout << str << std::endl;
		ResetCmd();
	}

	//Sends a yellow string literal to console
	static void Message(const char* str) {
		color = 6;
		SetConsoleTextAttribute(hConsole, color);
		std::cout << str << std::endl;
		ResetCmd();
	}

	//Sends a green message to console
	static void Success(const std::string& str) {
		color = 2;
		SetConsoleTextAttribute(hConsole, color);
		std::cout << str << std::endl;
		ResetCmd();
	}

	//Sends a green string literal to console
	static void Success(const char* str) {
		color = 2;
		SetConsoleTextAttribute(hConsole, color);
		std::cout << str << std::endl;
		ResetCmd();
	}

	//// Timer ////

	static steady_clock::time_point start;
	static steady_clock::time_point end;
	static std::list<double> times;
	static int elapsedFrames = 0;

	static void StartTimer() { start = steady_clock::now(); }

	static void EndTimer(std::string message = "") {
		end = steady_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(end - start);
		Message(message + std::to_string(time_span.count()));
	}

	static void EndTimer(double goodTime, std::string message = "") {
		end = steady_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(end - start);
		double t = time_span.count();
		if (t > goodTime) { Error(message + std::to_string(t)); }
		else if (t < goodTime) { Success(message + std::to_string(t)); }
		else { Message(message + std::to_string(t)); }
	}

	//prints an average time every nFrames
	//dunno if this would be useful, also probably not the most efficient thing ever
	//could make a conditional version but idk yet
	static void EndTimerAverage(int num, std::string message, int nFrames = 1) {
		end = steady_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(end - start);
		double t = time_span.count();

		if (times.size() < num) { times.push_back(t); }
		else { times.pop_front(); times.push_back(t); }

		if (elapsedFrames >= nFrames) { Message(message + std::to_string(Math::average(times, times.size()))); elapsedFrames = 0; }
		else { elapsedFrames++; }
	}

	//for timing things
	//this can potentially become innaccurate in certain situations
	//like with frame rate issues and such
	static double GetEndTimer(bool restart) {
		end = steady_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(end - start);
		if (restart) { start = steady_clock::now(); }
		return time_span.count();
	}

};