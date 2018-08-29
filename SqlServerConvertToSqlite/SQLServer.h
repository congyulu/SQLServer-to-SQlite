#pragma once
#include "SQLServerDBUtil.h"
#include "TableInfo.h"
class SQLServer
{
public:
	SQLServer(LPCTSTR constr);
	~SQLServer();
public:
	vector<string> GetAllTable();
	vector<TableInfo>  GetTableFiled(LPCTSTR tableName);
	vector<string> GetInsertSQLByName(LPCTSTR tableName, vector<TableInfo> ti);
private:
	SQLServerDBUtil* s_db = NULL;
};

