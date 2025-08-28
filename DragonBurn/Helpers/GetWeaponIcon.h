#pragma once
#include <string>
#include <map>

static const std::unordered_map<std::string, const char*> gunIcons = 
{
    {"ct_knife", "]"},
    {"t_knife", "["},
    {"deagle", "A"},
    {"elite", "B"},
    {"fiveseven", "C"},
    {"glock", "D"},
    {"revolver", "J"},
    {"p2000", "E"},
    {"p250", "F"},
    {"usp", "G"},
    {"tec9", "H"},
    {"cz75a", "I"},
    {"mac10", "K"},
    {"ump45", "L"},
    {"bizon", "M"},
    {"mp7", "N"},
    {"mp9", "P"},
    {"p90", "O"},
    {"galilar", "Q"},
    {"famas", "R"},
    {"m4a1", "T"},
    {"m4a4", "S"},
    {"aug", "U"},
    {"sg556", "V"},
    {"ak47", "W"},
    {"g3sg1", "X"},
    {"scar20", "Y"},
    {"awp", "Z"},
    {"ssg08", "a"},
    {"xm1014", "b"},
    {"sawedoff", "c"},
    {"mag7", "d"},
    {"nova", "e"},
    {"negev", "f"},
    {"m249", "g"},
    {"taser", "h"},
    {"flashbang", "i"},
    {"hegrenade", "j"},
    {"smokegrenade", "k"},
    {"molotov", "l"},
    {"decoy", "m"},
    {"incgrenade", "n"},
    {"c4", "o"}
};

// https://www.unknowncheats.me/forum/counter-strike-2-a/608799-weapon-icon-esp.html
const char* GunIcon(const std::string weapon)
{
	auto it = gunIcons.find(weapon);
	if (it != gunIcons.end()) {
		return it->second;
	}
	return "";
}