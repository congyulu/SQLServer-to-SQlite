#include "stdafx.h"
#include "SQLServer.h"
#include "SQLServerDBUtil.h"
#include "SQLDBUtil.h"
SQLServer::SQLServer(LPCTSTR constr)
{
	s_db = new SQLServerDBUtil(constr);
}


SQLServer::~SQLServer()
{
	if (s_db)
	{
		delete s_db;
		s_db = NULL;
	}
}

vector<string> SQLServer::GetAllTable()
{
	char sql[] = "select Name from [dbo].[sysobjects] where xtype='u'";
	s_db->Connection();
	SQLCHAR sno[255];
	SQLINTEGER snoLen;
	vector<string> vec = vector<string>();
	SQLHSTMT stmt = s_db->ExecuteSQL(sql);
	SQLRETURN s_return;
	if (stmt != NULL)
	{
		while ((s_return = SQLFetch(stmt)) != SQL_NO_DATA)
		{
			if (s_return == SQL_ERROR)
			{
				printf("Error occurred during fetch data.");
				s_db->PrintError();
				scanf_s("");
				return vector<string>();
			}
			else
			{
				SQLGetData(stmt, 1, SQL_CHAR, sno, 255, &snoLen);
				vec.push_back((char*)sno);
			}
		}
	}
	else
	{
		s_db->PrintError();
	}
	s_db->Close();
	return vec;
}

vector<TableInfo>  SQLServer::GetTableFiled(LPCTSTR tableName)
{
	char sql[1024];
	sprintf_s(sql, "SELECT a.name as [Name], b.name as [Type], a.length as [Len],(case when a.isnullable=1 then 1 else 0 end) as [IsNull], \
		Convert(int,case when exists(SELECT 1 FROM sys.sysobjects where xtype = 'PK' and parent_obj = a.id and name in \
		(SELECT name FROM sys.sysindexes WHERE indid in(SELECT indid FROM sys.sysindexkeys WHERE id = a.id AND colid = a.colid))) then 1 else 0 end) as [Primary] \
		FROM sys.syscolumns a left join systypes b \
		on a.xusertype = b.xusertype \
		inner join sys.sysobjects d \
		on a.id = d.id  and d.xtype = 'U' and  d.name<>'dtproperties' \
	    where d.name = '%s' ", tableName);
	s_db->Connection();
	SQLCHAR name[20], type[20];
	SQLINTEGER len, isNULL, primary;
	SQLINTEGER nameLen, typeLen, lenLen, isNULLLen, primaryLen;
	vector<TableInfo> vec = vector<TableInfo>();
	SQLHSTMT stmt = s_db->ExecuteSQL(sql);
	SQLRETURN s_return;
	if (stmt != NULL)
	{
		while ((s_return = SQLFetch(stmt)) != SQL_NO_DATA)
		{
			if (s_return == SQL_ERROR)
			{
				printf("Error occurred during fetch data.");
				s_db->PrintError();
				scanf_s("");
			}
			else
			{
				TableInfo info = TableInfo();
				SQLGetData(stmt, 1, SQL_CHAR, name, 20, &nameLen);
				SQLGetData(stmt, 2, SQL_CHAR, type, 20, &typeLen);
				SQLGetData(stmt, 3, SQL_INTEGER, &len, sizeof(len), &lenLen);
				SQLGetData(stmt, 4, SQL_INTEGER, &isNULL, sizeof(isNULL), &isNULLLen);
				SQLGetData(stmt, 5, SQL_INTEGER, &primary, sizeof(primary), &primaryLen);
				info.FieldName = (char*)name;
				info.FieldType = (char*)type;
				info.FieldLen = len;
				info.IsNULL = isNULL;
				info.Primary = primary;
				vec.push_back(info);
			}
		}
	}
	else
	{
		s_db->PrintError();
	}
	s_db->Close();
	return vec;
}

map<string, vector<ParamInfo>> SQLServer::GetInsertSQLByName(LPCTSTR tableName, vector<TableInfo> vti)
{
	Operation oper;
	map<string, vector<ParamInfo>> sqls = map<string, vector<ParamInfo>>();
	string colStr;
	for (int i = 0; i < vti.size(); i++)
	{
		colStr += vti[i].FieldName + ",";
	}
	colStr = colStr.substr(0, colStr.length() - 1);
	char sql[1024];
	sprintf_s(sql, "select %s from [dbo].%s", colStr.data(), tableName);
	s_db->Connection();

	SQLHSTMT stmt = s_db->ExecuteSQL(sql);
	if (stmt != NULL)
	{
		SQLINTEGER rowLen;
		SQLRowCount(stmt, &rowLen);
		SQLRETURN s_return;
		vector<ParamInfo> info = vector<ParamInfo>();
		while ((s_return = SQLFetch(stmt)) != SQL_NO_DATA)
		{
			char sql[1024];
			SQLCHAR rowValue[512];
			SQLINTEGER len;
			string value;
			int index = 1;
			for (int i = 0; i < vti.size(); i++)
			{
				TableInfo t = vti[i];
				SQLGetData(stmt, i + 1, SQL_CHAR, rowValue, 512, &len);
				if (t.FieldType == "int" || t.FieldType == "bigint" || t.FieldType == "decimal"
					|| t.FieldType == "float" || t.FieldType == "double")
				{
					value += (char*)rowValue;
				}
				else
				{
					char *szSql = oper.Char_To_UTF8((char*)rowValue);
					value += "?";
					ParamInfo pInfo = ParamInfo();
					pInfo.Index = index;
					pInfo.Value = szSql;
					info.push_back(pInfo);
					index++;
				}
				value += ",";
			}
			value = value.substr(0, value.length() - 1);
			sprintf_s(sql, "Insert Into %s (%s) values(%s)", tableName, colStr.data(), value.data());
			sqls.insert(pair<string, vector<ParamInfo>>(sql, info));
		}
	}
	s_db->Close();
	return sqls;
}


