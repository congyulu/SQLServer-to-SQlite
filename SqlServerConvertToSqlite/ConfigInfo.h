#pragma once
#include "stdafx.h"
struct ConfigInfo
{
	vector<string> ConnectionStrings;
	string SQLiteName;
	string SQLitePWD;
	ConfigInfo()
	{
		ConnectionStrings = vector<string>();
	}
};

