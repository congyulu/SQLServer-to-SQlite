#include "stdafx.h"
#include "SQLServerDBUtil.h"

SQLServerDBUtil::SQLServerDBUtil(LPCTSTR constr) :s_stmt(NULL), s_env(NULL), s_dbc(NULL), s_return(NULL)
{
	connectionString = constr;
}


SQLServerDBUtil::~SQLServerDBUtil()
{
	Close();
}

void SQLServerDBUtil::Connection()
{
	Close();
	s_return = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &s_env);
	if ((s_return != SQL_SUCCESS) && (s_return != SQL_SUCCESS_WITH_INFO))
	{
		cout << "Erro AllocHandle" << s_return << endl;
		return;
	}
	s_return = SQLSetEnvAttr(s_env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	if ((s_return != SQL_SUCCESS) && (s_return != SQL_SUCCESS_WITH_INFO))
	{
		cout << "Erro AllocHandle" << s_return << endl;
		SQLFreeHandle(SQL_HANDLE_DBC, s_env);
		s_env = NULL;
		return;
	}
	s_return = SQLAllocHandle(SQL_HANDLE_DBC, s_env, &s_dbc);
	if ((s_return != SQL_SUCCESS) && (s_return != SQL_SUCCESS_WITH_INFO))
	{
		cout << "Erro AllocHandle" << s_return << endl;
		SQLFreeHandle(SQL_HANDLE_DBC, s_env);
		s_env = NULL;
		return;
	}
	s_return = SQLSetConnectAttr(s_dbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
	SQLCHAR outstr[1024];
	SQLSMALLINT outstrlen;
	char instr[1024];
	sprintf(instr, "DRIVER={SQL Server};%s", connectionString);
	s_return = SQLDriverConnect(s_dbc, NULL, (SQLCHAR*)instr, SQL_NTS, outstr, SQL_NTS, &outstrlen, SQL_DRIVER_NOPROMPT);
	if (s_return == SQL_ERROR)
	{
		PrintError();
		printf("Fail to connect db.\n");
		return;
	}
	s_return = SQLAllocHandle(SQL_HANDLE_STMT, s_dbc, &s_stmt);
	if (SQL_SUCCESS != s_return)
	{
		printf("Fail to create stmt.\n");
		return;
	}
	else
	{
		printf("Connect Database Successfully.\n");
	}
}
void SQLServerDBUtil::Close()
{
	if (s_stmt)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, s_stmt);
		s_stmt = NULL;
	}
	if (s_dbc)
	{
		SQLDisconnect(s_dbc);
		SQLFreeHandle(SQL_HANDLE_DBC, s_dbc);
		s_dbc = NULL;
	}
	if (s_env)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, s_env);
		s_env = NULL;
	}
}

SQLHSTMT SQLServerDBUtil::ExecuteSQL(char* sql)
{
	s_return = SQLExecDirect(s_stmt, (SQLCHAR*)sql, SQL_NTS);
	if (s_return == SQL_SUCCESS || s_return == SQL_SUCCESS_WITH_INFO)
	{
		return s_stmt;
	}
	else
	{
		printf("exec error.");
		PrintError();
		return NULL;
	}
}

void SQLServerDBUtil::PrintError()
{
	SQLCHAR state[255];
	SQLINTEGER type;
	SQLCHAR msg[1024];
	SQLSMALLINT msgLen;
	SQLError(s_env, s_dbc, s_stmt, (SQLCHAR*)&state, &type, (SQLCHAR*)&msg, 1024, &msgLen);
	cout << msg << "\n" << endl;
}

