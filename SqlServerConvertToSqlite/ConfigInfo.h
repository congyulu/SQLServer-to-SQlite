#pragma once
#include "stdafx.h"
struct ConfigInfo
{
	vector<string> ConnectionStrings;
	string SQLiteName;
	ConfigInfo()
	{
		ConnectionStrings = vector<string>();
	}
};

