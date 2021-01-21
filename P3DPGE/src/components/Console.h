#pragma once
#include "Component.h"

struct Console : public Component {
	char inputBuf[256];
	std::vector<std::string> buffer;
	std::vector<std::string> history;
	int historyPos;

	bool autoScroll;
	bool scrollToBottom;

	Console() {
		memset(inputBuf, 0, sizeof(inputBuf));
		historyPos = -1;
		autoScroll = true;
		scrollToBottom = false;
	}

};