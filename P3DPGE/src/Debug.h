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

//global debug macros
#define DEBUG if(Debug::GLOBAL_DEBUG)
//debug message
#define DEBUG_S DEBUG Debug::Success
#define DEBUG_M DEBUG Debug::Message
#define DEBUG_E DEBUG Debug::Error
//force debug message
#define DEBUG_SF Debug::Success
#define DEBUG_MF Debug::Message
#define DEBUG_EF Debug::Error

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

//template magic thanks to fux#2562
template<class T>
struct has_str_method {
	template<class U> static decltype(&U::str, std::true_type{}) test(int);
	template<class> static std::false_type test(...);
	static constexpr bool value = decltype(test<T>(0))::value;
};

namespace Debug {
	internal HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);;
	internal int color = 7;
	internal void ResetCmd() { //resets cmd font color to white
		color = 7;
		SetConsoleTextAttribute(hConsole, color);
	}

	static bool GLOBAL_DEBUG = 1;

	////Console Output////

	static void Print(int color, const char* str, int frame_skip = 0) {
		SetConsoleTextAttribute(hConsole, color);
		std::cout << str << std::endl;
		ResetCmd();
	}

	//template magic thanks to fux#2562

	//sends a green message to the console
	static void Success(const char* str) { Print(2, str); }

	static void Success(const std::string& str) { Print(2, str.c_str()); }

	template<class T, typename std::enable_if<!has_str_method<T>::value, bool>::type = true>
	static void Success(T t) { Success(std::to_string(t)); }

	template<class T, typename std::enable_if<has_str_method<T>::value, bool>::type = true>
	static void Success(T t) { Success(t.str()); }

	//sends a yellow message to the console
	static void Message(const char* str) { Print(2, str); }

	static void Message(const std::string& str) { Print(6, str.c_str()); }

	template<class T, typename std::enable_if<!has_str_method<T>::value, bool>::type = true>
	static void Message(T t) { Message(std::to_string(t)); }

	template<class T, typename std::enable_if<has_str_method<T>::value, bool>::type = true>
	static void Message(T t) { Message(t.str()); }

	//sends a red message to the console
	static void Error(const char* str) { Print(2, str); }

	static void Error(const std::string& str) { Print(4, str.c_str()); }
	
	template<class T, typename std::enable_if<!has_str_method<T>::value, bool>::type = true>
	static void Error(T t) { Error(std::to_string(t)); }

	template<class T, typename std::enable_if<has_str_method<T>::value, bool>::type = true>
	static void Error(T t) { Error(t.str()); }

	

};