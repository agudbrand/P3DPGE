#pragma once
#include "olcPixelGameEngine.h"
#include <string>
#include <iostream>
#include <cstdarg>
#include <windows.h>

namespace Debug {

	namespace {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);;
		int color = 7;
	}

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
	static void Error(olc::PixelGameEngine* p, std::string& str) {
		color = 4;
		SetConsoleTextAttribute(hConsole, color);
		std::cout << str << std::endl;
		color = 7;
		SetConsoleTextAttribute(hConsole, color);
	}

	//Sends a red string literal to the console
	static void Error(olc::PixelGameEngine* p, const char* str) {
		color = 4;
		SetConsoleTextAttribute(hConsole, color);
		std::cout << str << std::endl;
		color = 7;
		SetConsoleTextAttribute(hConsole, color);
	}

	//Sends a yellow message to console
	static void Message(olc::PixelGameEngine* p, std::string& str) {
		color = 6;
		SetConsoleTextAttribute(hConsole, color);
		std::cout << str << std::endl;
		color = 7;
		SetConsoleTextAttribute(hConsole, color);
	}

	//Sends a yellow string literal to console
	static void Message(olc::PixelGameEngine* p, const char* str) {
		color = 6;
		SetConsoleTextAttribute(hConsole, color);
		std::cout << str << std::endl;
		color = 7;
		SetConsoleTextAttribute(hConsole, color);
	}

	//Sends a green message to console
	static void Success(olc::PixelGameEngine* p, std::string& str) {
		color = 2;
		SetConsoleTextAttribute(hConsole, color);
		std::cout << str << std::endl;
		color = 7;
		SetConsoleTextAttribute(hConsole, color);
	}

	//Sends a green string literal to console
	static void Success(olc::PixelGameEngine* p, const char* str) {
		color = 2;
		SetConsoleTextAttribute(hConsole, color);
		std::cout << str << std::endl;
		color = 7;
		SetConsoleTextAttribute(hConsole, color);
	}

};