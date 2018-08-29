#pragma once
#include "SQLDBUtil.h"
#include "TableInfo.h"
class SQLite
{
public:
	SQLite(LPCTSTR sqliteName, LPCTSTR sqlitePWD);
	~SQLite();
public:
	bool CreateTable(LPCTSTR tableName, vector<TableInfo> ti);
	void InsertRow(vector<string> sqls);
private:
	SQLDBUtil* db;
};

