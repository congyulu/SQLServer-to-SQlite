#pragma once
#include "SQLDBUtil.h"
#include "TableInfo.h"
#include <map>
#include "ParamInfo.h"
class SQLite
{
public:
	SQLite(LPCTSTR sqliteName, LPCTSTR sqlitePWD);
	~SQLite();
public:
	bool CreateTable(LPCTSTR tableName, vector<TableInfo> ti);
	void InsertRow(map<string, vector<ParamInfo>> sqls);
private:
	SQLDBUtil* db;
};

