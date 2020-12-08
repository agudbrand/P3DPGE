#pragma once

#include "internal/olcPixelGameEngine.h"
#include <stack>
#include <chrono>
#include <string>
#include <cstdarg>
#include <iostream>
#include <windows.h>

#define internal static
#define local_persist static 
#define global_variable static

//global debug macros
#define DEBUG if(Debug::GLOBAL_DEBUG)
//debug message
#define SUCCESS DEBUG Debug::Success
#define LOG     DEBUG Debug::Message
#define ERROR   DEBUG Debug::Error
#define LOGC	DEBUG Debug::LogCustom
//force debug message
#define SUCCESSF Debug::Success
#define LOGF     Debug::Message
#define ERRORF   Debug::Error
#define LOGCF	 Debug::LogCustom

//call tracing
#define _TRACE  

//stringize certain macros
//this is all probably REALLY slow but will be as is unless I find a more elegent solution
#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)
#define LINE_NUM  STRINGIZE(__LINE__)

#define __FILENAME__ (std::strrchr(__FILE__, '\\') ? std::strrchr(__FILE__, '\\') + 1 : __FILE__)

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
#   define ASSERT(condition, message) do { } while (false)
#endif



//std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
//<< " line " << __LINE__ << ": " << message << std::endl; \

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

namespace Debug {
	

	static bool GLOBAL_DEBUG = 1;

//// Call Tracing ////

	namespace Tracer {
		static std::stack<std::string> call_stack;
	}

	class call_tracer {
		std::stack<std::string> call_stack;

		call_tracer(std::string function_name) { Tracer::call_stack.push(function_name); }

		~call_tracer() { Tracer::call_stack.pop(); }
		void stack_place(std::string func_name) {
			call_stack.push(func_name);
		}
	};

	

//// Console Output////

internal HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);;
	internal int color = 7;
	internal void ResetCmd() { //resets cmd font color to white
		color = 7;
		SetConsoleTextAttribute(hConsole, color);
	}

	static void Print(ConsoleColor color, const char* str, int frame_skip = 0) {
		SetConsoleTextAttribute(hConsole, color);
		std::cout << str << std::endl;
		ResetCmd();
	}

	//template magic thanks to fux#2562

	//sends a green message to the console
	static void Success(const char* str) { Print(ConsoleColor::GREEN, str); }

	static void Success(const std::string& str) { Success(str.c_str()); }

	template<class T, typename std::enable_if<!has_str_method<T>::value, bool>::type = true>
	static void Success(T t) { Success(std::to_string(t)); }

	template<class T, typename std::enable_if<has_str_method<T>::value, bool>::type = true>
	static void Success(T t) { Success(t.str()); }

	//sends a yellow message to the console
	static void Message(const char* str) { Print(ConsoleColor::YELLOW, str); }

	static void Message(const std::string& str) { Message(str.c_str()); }

	template<class T, typename std::enable_if<!has_str_method<T>::value, bool>::type = true>
	static void Message(T t) { Message(std::to_string(t)); }

	template<class T, typename std::enable_if<has_str_method<T>::value, bool>::type = true>
	static void Message(T t) { Message(t.str()); }

	//sends a red message to the console

	static void Error(const char* str) { Print(ConsoleColor::RED, str); }

	static void Error(const std::string& str) { Error(str.c_str()); }
	
	template<class T, typename std::enable_if<!has_str_method<T>::value, bool>::type = true>
	static void Error(T t) { Error(std::to_string(t)); }

	template<class T, typename std::enable_if<has_str_method<T>::value, bool>::type = true>
	static void Error(T t) { Error(t.str()); }

	//sends a message with a custom color to the console
	static void LogCustom(const char* str, ConsoleColor color) { Print(color, str); }

	static void LogCustom(const std::string& str, ConsoleColor color) { LogCustom(str.c_str(), color); }

	template<class T, typename std::enable_if<!has_str_method<T>::value, bool>::type = true>
	static void LogCustom(T t, ConsoleColor color) { LogCustom(std::to_string(t), color); }

	template<class T, typename std::enable_if<has_str_method<T>::value, bool>::type = true>
	static void LogCustom(T t, ConsoleColor color) { LogCustom(t.str(), color); }

	

};