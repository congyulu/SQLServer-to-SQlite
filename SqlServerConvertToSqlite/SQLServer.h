#pragma once
#include "SQLServerDBUtil.h"
#include "TableInfo.h"
#include "ParamInfo.h"
#include <map>
class SQLServer
{
public:
	SQLServer(LPCTSTR constr);
	~SQLServer();
public:
	vector<string> GetAllTable();
	vector<TableInfo>  GetTableFiled(LPCTSTR tableName);
	map<string, vector<ParamInfo>> GetInsertSQLByName(LPCTSTR tableName, vector<TableInfo> ti);
private:
	SQLServerDBUtil* s_db = NULL;
};

