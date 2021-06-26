#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "Settings.h"

using boost::property_tree::ptree;

Settings *g_Settings = nullptr;

void Settings::Save(int min, int sec, int ms) {
	ptree settingsTree;

	settingsTree.put("minutes", min);
	settingsTree.put("seconds", sec);
	settingsTree.put("milliseconds", ms);

	write_json("ASettings.json", settingsTree);
}

void Settings::Load(int &min, int &sec, int &ms) {
	ptree settingsTree;

	read_json("ASettings.json", settingsTree);

	min = settingsTree.get<int>("minutes", 0);
	sec = settingsTree.get<int>("seconds", 0);
	ms = settingsTree.get<int>("milliseconds", 100);
}
