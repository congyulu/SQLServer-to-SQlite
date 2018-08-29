#pragma once
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#pragma comment(lib,"odbc32.lib")
class SQLServerDBUtil
{
public:
	SQLServerDBUtil(LPCTSTR constr);
	~SQLServerDBUtil();
public:
	void Connection();
	void Close();
	SQLHSTMT ExecuteSQL(char* sql);
	void PrintError();
private:
	SQLHENV s_env;
	SQLHDBC s_dbc;
	SQLHSTMT s_stmt;
	SQLRETURN s_return;
	char* pszDSN;
	char* pszUName;
	char* pszUPassword;
	LPCTSTR connectionString;
};

