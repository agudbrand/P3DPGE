#pragma once

#include "internal/olcPixelGameEngine.h"
#include <any>
#include <stack>
#include <chrono>
#include <string>
#include <vector>
#include <cstdarg>
#include <optional>
#include <iostream>
#include <windows.h>

#define internal static
#define local_persist static 
#define global_variable static

//global debug macros
#define DEBUG if(GLOBAL_DEBUG)

//debug message
#define SUCCESS(...)   DEBUG Debug::ToString(2, __VA_ARGS__)
#define LOG(...)       DEBUG Debug::ToString(1, __VA_ARGS__)
#define ERROR(...)     DEBUG Debug::ToString(0, __VA_ARGS__)
//force debug message
#define SUCCESSF(...) Debug::ToString(2, __VA_ARGS__)
#define LOGF(...)     Debug::ToString(1, __VA_ARGS__)
#define ERRORF(...)   Debug::ToString(0, __VA_ARGS__)

#define TOSTRING(...) Debug::ToStringReturn(__VA_ARGS__)

//stringize certain macros
//this is all probably REALLY slow but will be as is unless I find a more elegent solution
#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)
#define LINE_NUM  STRINGIZE(__LINE__)

#define __FILENAME__ (std::strrchr(__FILE__, '\\') ? std::strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOGFUNC LOG(__FUNCTION__, " called")
#define LOGFUNCM(...) LOG(__FUNCTION__, " called ", __VA_ARGS__)

//from John McFarlane on stack exchange
//returns false if wherever this is called has been called before
//eg. use this in an if statement in a loop for it to only run once ever
#define FIRST_TIME_HERE ([] { \
    static bool is_first_time = true; \
    auto was_first_time = is_first_time; \
    is_first_time = false; \
    return was_first_time; } ())

//wrap code in this for it to only run once the entire program
//TODO(g, sushi) write a macro for running once in a loop
#define RUN_ONCE if(FIRST_TIME_HERE)

//makes a random number only once and then always returns that same number
//if called by the same object
#define PERM_RAND_INT ([]{ static int rint = rand() % 100000; return rint;}())

//#define BUFFER_IDENTIFIER ([]{ static int id_ini = buffer_size++; return id_ini; }())
//#define BUFFERLOG(...) g_cBuffer.add_to(new StringedBuffer(TOSTRING(__VA_ARGS__), BUFFER_IDENTIFIER))

#define BUFFERLOG(i, ...) DEBUG g_cBuffer.add_to_index(TOSTRING(__VA_ARGS__), i)

#define BUFFERLOGI(i, o, ...) DEBUG ([&]{static int iter = 0; if(iter == o){g_cBuffer.add_to_index(TOSTRING(__VA_ARGS__), i); iter = 0;} else iter++;}())

//this stores and input vector and returns the previously stored vector
//you must store a vector to get the previous one
#define V_STORE(v, t) ([&]()->Vector3{static Vector3 vect[1];\
Vector3 vr = vect[0];\
if(t){ vect[0] = v; return vr; } \
else return vr; }\
())

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

#ifndef NDEBUG
#   define ASSERTWARN(condition, message) \
    do { \
        if (! (condition)) { \
            std::string file = __FILENAME__; \
            LOG("Warning '" #condition "' failed in " + file + " line " + std::to_string(__LINE__) + ": \n" #message); \
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

extern bool GLOBAL_DEBUG;

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

	static std::vector<float> nums;

//// Console Output ////

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

	static void ToString(int mtype, const char* str, bool newline = true) {
		switch (mtype) {
		case 0: Print(ConsoleColor::RED, str, newline);    break;
		case 1: Print(ConsoleColor::YELLOW, str, newline); break;
		case 2: Print(ConsoleColor::GREEN, str, newline);  break;
		//TODO(g, sushi) implement buffer colors
		}
	}

	static void ToString(int mtype, const std::string& str, bool newline = true) { ToString(mtype, str.c_str(), newline); }

	template<class T, typename std::enable_if<!has_str_method<T>::value, bool>::type = true>
	static void ToString(int mtype, T t, bool newline = true) { ToString(mtype, std::to_string(t), newline); }

	template<class T, typename std::enable_if<has_str_method<T>::value, bool>::type = true>
	static void ToString(int mtype, T t, bool newline = true) { ToString(mtype, t.str(), newline); }

	template<class... T>
	static void ToString(int mtype, T... args) { (ToString(mtype, args, false), ...); ToString(0, "", true); }

	//returns the string for in engine printing
	static std::string ToStringReturn(const char* str) { return std::string(str); }

	static std::string ToStringReturn(const std::string& str) { return str; }

	template<class T, typename std::enable_if<!has_str_method<T>::value, bool>::type = true>
	static std::string ToStringReturn(T t) { return ToStringReturn(std::to_string(t)); }

	template<class T, typename std::enable_if<has_str_method<T>::value, bool>::type = true>
	static std::string ToStringReturn(T t) { return ToStringReturn(t.str()); }

	template<class... T>
	static std::string ToStringReturn(T... args) { return (ToStringReturn(args) + ...); }

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

template<class T>
class ContainerManager {
public:
	//std::vector<std::pair<std::optional<T>, int>> container;

	std::vector<std::optional<T>> container;
	std::vector<int> empties;

	ContainerManager() {}

	//T t& operator [](int i) { return container[i]; }
	//void operator = (ContainerManager<T> c) { this->copy(c); }

	void add_to(T t) {
		if (empties.size() == 0) {
			container.push_back(t);
			return container.size() - 1;
		}
		else {
			container[empties[0]] = t;
			int index = empties[0];
			empties.erase(empties.begin());
			return index;
		}
	}

	void add_to_index(T t, int index) {
		ASSERT(allocate_space(index), "Container was unable to allocate space at specified index");
		container[index] = t;
	}

	void set_index(T t, int index) {
		container[index] = t;
	}

	void remove_from(int index) {
		//ASSERT(container[index], "Container at index " + std::to_string(index) + " is already empty.");
		ASSERT(index <= container.size() - 1, "Trying to access container at an index that doesn't exist.");

		//if (index == container.size() - 1) {
		//	container.pop_back();
		//}
		//else {
			container[index].first.reset();
			//empties.push_back(index);
		//}
	}

	bool allocate_space(int index) {
		ASSERT(index >= 0, "Attempted to pass negative index.");
		//ASSERT(index >= container.size(), "Attempted to allocate space at an index larger than container size.");
		if (index < container.size()) {
			//space already exists, there is no check to actually see if
			//it's already used by something else yet though
			return true;
		}
		else {

			std::optional<T> o;
			for (int i = container.size(); i < index; i++) {
				container.push_back(o);
			}
			
			return true;
		}
		return false;
		
	}

	int size() {
		return container.size();
	}

	void copy(ContainerManager<T> cm) {
		container = cm.container;
		empties = cm.empties;
	}

	void empty() {
		//this probably isn't what I should do so fix this if it isn't
		empties.clear();
		for (int i = 0; i < container.size(); i++) {
			container[i].first.reset();
			empties.push_back(i);
		}
	}

	void clear() {
		container.clear();
	}

	void str() {
		std::string s = "";
		int index = 1;
		for (auto t : container) {
			s += t.value()->str + " " + std::to_string(t.value()->index) + " " + std::to_string(index) + "\n";
			index++;
		}
		LOG(s);
	}
};

class StringedBuffer {
public:

	int index;
	std::string str;

	StringedBuffer(std::string s, int index) {
		this->index = index;
		str = s;
	}



};

inline static std::vector<int> id_buffer;

//global tracing variable
extern Debug::CallTrace TRACE;
