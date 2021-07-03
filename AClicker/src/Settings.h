#pragma once
#include "AClicker.h"

class AClicker;

class Settings {

public:
	Settings();

public:
	void Save();
	void Load();
	void Apply(Ui::AClicker *clicker);

public:
	// User settings saved.
	int m_KeyStart;
	int m_KeyStop;
	int m_Minutes;
	int m_Seconds;
	int m_Milliseconds;
	int m_ClickCount;
	int m_ClickButton;
	int m_ClickType;

	// Application depentend variables.
	bool m_IsRunning;
};