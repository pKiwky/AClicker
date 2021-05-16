#pragma once

class Settings {

public:
	Settings(): m_keyToggle(90), m_totalClicks(0), m_isRunning(false) {}

public:
	int m_keyToggle;
	int m_totalClicks;
	bool m_isRunning;

};

