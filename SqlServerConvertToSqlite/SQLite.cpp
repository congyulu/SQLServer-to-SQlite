#include "stdafx.h"
#include "SQLite.h"
SQLite::SQLite(LPCTSTR sqliteName, LPCTSTR sqlitePWD)
{
	db = new SQLDBUtil(sqliteName, sqlitePWD);
	if (db->Init(SQLITE_LOCAL) && db->Open(SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE))
	{
		b_canProcess = true;
	}
	else
	{
		b_canProcess = false;
	}
}


SQLite::~SQLite()
{
	if (db)
	{
		delete db;
		db = NULL;
	}
}

bool SQLite::CreateTable(LPCTSTR tableName, vector<TableInfo> vti)
{
	if (b_canProcess)
	{
		char sql[1024];
		string t_sql;
		for (int i = 0; i < vti.size(); i++)
		{
			TableInfo ti = vti[i];
			t_sql += ti.FieldName + " ";
			if (ti.FieldType == "int")
			{
				t_sql += "INT";
				if (ti.Primary)
				{
					t_sql += " Primary key";
				}
			}
			else if (ti.FieldType == "decimal")
			{
				t_sql += "DECIMAL(8,2)";
			}
			else if (ti.FieldType == "smalldatetime" || ti.FieldType == "datetime")
			{
				t_sql += "DATETIME";
			}
			else if (ti.FieldType == "bigint")
			{
				t_sql += "BIGINT";
				if (ti.Primary)
				{
					t_sql += " Primary key";
				}
			}
			else if (ti.FieldType == "bit")
			{
				t_sql += "BOOL";
			}
			else
			{
				t_sql += "TEXT";
			}

			t_sql += ",";
		}
		if (t_sql.length() > 0)
		{
			t_sql = t_sql.substr(0, t_sql.length() - 1);
			sprintf(sql, "CREATE TABLE [%s] (%s)", tableName, t_sql.data());
			printf_s("%s\n", sql);
			assert(db != NULL);
			return db->ExcuteNonQuery(sql);
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void SQLite::InsertRow(vector<SQLInfo> sqls)
{
	if (b_canProcess)
	{
		for (int i = 0; i < sqls.size(); i++)
		{
			SQLInfo si = sqls[i];
			SQLiteCommand* cmd = new SQLiteCommand(db, si.SQL.data());
			printf("%s \n", si.SQL);
			vector<ParamInfo> vpi = si.Values;
			for (int i = 0; i < vpi.size(); i++)
			{
				ParamInfo pi = vpi[i];
				cmd->BindParam(pi.Index, (char*)pi.Value);
			}
			if (!db->ExcuteNonQuery(cmd))
			{
				printf(db->GetLastErrorMsg());
				getchar();
			}
			cmd->Clear();
			delete cmd;
		}
	}
}
