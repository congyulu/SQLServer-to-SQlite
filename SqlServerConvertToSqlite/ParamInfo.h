#pragma once
#include "stdafx.h"
struct ParamInfo
{
	int Index;
	void* Value;
	ParamInfo()
	{

	}
};

struct SQLInfo
{
	string SQL;
	vector<ParamInfo> Values;
	SQLInfo(string sql, vector<ParamInfo> values)
	{
		SQL = sql;
		Values = values;
	}
};

