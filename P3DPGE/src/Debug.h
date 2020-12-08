#pragma once

#include "internal/olcPixelGameEngine.h"
#include <any>
#include <stack>
#include <chrono>
#include <string>
#include <cstdarg>
#include <iostream>
#include <windows.h>

#define internal static
#define local_persist static 
#define global_variable static

#define ENDLOGC nullptr

//global debug macros
#define DEBUG if(Debug::GLOBAL_DEBUG)
//debug message
#define SUCCESS(...) DEBUG Debug::ToChar(2, __VA_ARGS__)
#define LOG(...)     DEBUG Debug::ToChar(1, __VA_ARGS__)
#define ERROR(...)   DEBUG Debug::ToChar(0, __VA_ARGS__)
//force debug message
#define SUCCESSF(...) Debug::ToChar(2, __VA_ARGS__)
#define LOGF(...)     Debug::ToChar(1, __VA_ARGS__)
#define ERRORF(...)   Debug::ToChar(0, __VA_ARGS__)

//stringize certain macros
//this is all probably REALLY slow but will be as is unless I find a more elegent solution
#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)
#define LINE_NUM  STRINGIZE(__LINE__)

#define __FILENAME__ (std::strrchr(__FILE__, '\\') ? std::strrchr(__FILE__, '\\') + 1 : __FILE__)

//call tracing
#define _TRACE TRACE.push_stack()



#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::string file = __FILENAME__; \
            ERROR("Assertion '" #condition "' failed in " + file + " line " + std::to_string(__LINE__) + ": \n" #message); \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) condition;
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



//template magic thanks to fux#2562
template<class T>
struct has_str_method {
	template<class U> static decltype(&U::str, std::true_type{}) test(int);
	template<class> static std::false_type test(...);
	static constexpr bool value = decltype(test<T>(0))::value;
};

enum ConsoleColor {
	BLUE = 1,
	GREEN = 2,
	CYAN = 3,
	RED = 4,
	PURPLE = 5,
	YELLOW = 6,
	WHITE = 7,
	GREY = 8,
	BRTBLUE = 9,
	BRTGREEN = 10,
	BRTCYAN = 11,
	BRTRED = 12,
	PINK = 13,
	BRTYELLOW = 14,
	BRTWHITE = 15
};

struct Camera;

namespace Debug{

	global_variable bool GLOBAL_DEBUG = false;

//// Console Output////

	internal HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	internal int color = 7;
	internal void ResetCmd() { //resets cmd font color to white
		color = 7;
		SetConsoleTextAttribute(hConsole, color);
	}

	static void Print(ConsoleColor color, const char* str, bool newline = true) {
		SetConsoleTextAttribute(hConsole, color);
		if(newline){ std::cout << str << std::endl; }
		else { std::cout << str; }
		ResetCmd();
	}

	static void ToChar(int mtype, const char* str, bool newline = true) {
		switch (mtype) {
		case 0: Print(ConsoleColor::RED, str, newline);    break;
		case 1: Print(ConsoleColor::YELLOW, str, newline); break;
		case 2: Print(ConsoleColor::GREEN, str, newline);  break;
		}
	}

	static void ToChar(int mtype, const std::string& str, bool newline = true) { ToChar(mtype, str.c_str(), newline); }

	template<class T, typename std::enable_if<!has_str_method<T>::value, bool>::type = true>
	static void ToChar(int mtype, T t, bool newline = true) { ToChar(mtype, std::to_string(t), newline); }

	template<class T, typename std::enable_if<has_str_method<T>::value, bool>::type = true>
	static void ToChar(int mtype, T t, bool newline = true) { ToChar(mtype, t.str(), newline); }

	template<class... T>
	static void ToChar(int mtype, T... args) { (ToChar(mtype, args, false), ...); ToChar(0, "", true); }

//// Call Tracing ////

	//figure this out later i guess
	class CallTrace {
	public:
		CallTrace(){}

		std::stack<std::pair<std::string, std::string>> call_stack;

		void push_stack(std::pair<std::string, std::string> c) { call_stack.push(c); }
		void pop_stack() { call_stack.pop(); }

		std::string str() {
			std::stack<std::pair<std::string, std::string>> cs_cpy = call_stack;
			std::string call_stack_str = "";
			while (cs_cpy.size() != 0) {
				call_stack_str.insert(0, cs_cpy.top().first + " in " + cs_cpy.top().second);
				cs_cpy.pop();
			}
			return call_stack_str;
		}
	};

	

};

//global tracing variable
extern Debug::CallTrace TRACE;
