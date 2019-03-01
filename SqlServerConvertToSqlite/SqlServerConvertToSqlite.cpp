// 
// Author: Congyu Lu
// E-mail:759728758@qq.com
//

#include "stdafx.h"
#include "SQLServer.h"
#include "SQLite.h"
#include "Configs.h"
#include "ConfigInfo.h"
//"SERVER=.\\SQLEXPRESS;DATABASE=NewResource;UID=sa;PWD=123456"
//"local.db"
void Process(LPCTSTR str, LPCTSTR sqliteName, LPCTSTR sqlitePWD = "");
ConfigInfo GetConfig();
int main()
{
	ConfigInfo config = GetConfig();
	printf("Press any key to run.");
	getchar();
	printf("Running\n");
	for (int i = 0; i < config.ConnectionStrings.size(); i++)
	{
		Process(config.ConnectionStrings[i].data(), config.SQLiteName.data(),config.SQLitePWD.data());
	}
	printf("Finished");
	getchar();
	return 0;
}
void Process(LPCTSTR str, LPCTSTR sqliteName, LPCTSTR sqlitePWD)
{
	SQLServer* mssql = new SQLServer(str);
	SQLite* sqlite = new SQLite(sqliteName, sqlitePWD);
	vector<string> tableNames = mssql->GetAllTable();
	if (tableNames.size() > 0)
	{
		for (int a = 0; a < tableNames.size(); a++)
		{
			LPCTSTR tableName = tableNames[a].data();
			vector<TableInfo> vti = mssql->GetTableFiled(tableName);
			printf("%s\n", tableNames[a].data());
			for (int b = 0; b < vti.size(); b++)
			{
				TableInfo ti = vti[b];
				printf("Name=%s  FieldType=%s FieldLen=%d IsNULL=%d Primary=%d \n",
					ti.FieldName.data(), ti.FieldType.data(), ti.FieldLen, ti.IsNULL, ti.Primary);
			}
			if (sqlite->CreateTable(tableName, vti))
			{
				sqlite->InsertRow(mssql->GetInsertSQLByName(tableName, vti));
			}
		}
	}
	delete sqlite;
	delete mssql;
}
ConfigInfo GetConfig()
{
	ConfigInfo config = ConfigInfo();
	TCHAR szDocument[MAX_PATH] = { 0 };
	TCHAR exeFullPath[MAX_PATH] = { 0 };
	if (GetModuleFileName(NULL, szDocument, MAX_PATH) != 0)
	{
		TCHAR drive[_MAX_DRIVE];
		TCHAR dir[_MAX_DIR];
		_splitpath(szDocument, drive, dir, NULL, NULL);
		strcat(exeFullPath, drive);
		strcat(exeFullPath, dir);
		strcat(exeFullPath, _T("\\Config.conf"));
	}
	Config* info = new Config();
	if (info->FromFile(exeFullPath))
	{
		Json::Value cs = info->Get("Config.ConnectionString", "");
		if (!cs.empty())
		{
			printf("ConnectionString:");
			if (cs.isArray())
			{
				for (int i = 0; i < cs.size(); i++)
				{
					printf("%s \n", cs[i].asCString());
					config.ConnectionStrings.push_back(cs[i].asString());
				}
			}
			else
			{
				printf("%s \n", cs.asCString());
				config.ConnectionStrings.push_back(cs.asString());
			}
		}
		else
		{
			info->Set("Config.ConnectionString", "");
		}
		Json::Value name = info->Get("Config.SQLiteName", "");
		if (!name.empty())
		{
			printf("SQLite DB Name:%s \n", name.asCString());
			config.SQLiteName = name.asString();
		}
		else
		{
			info->Set("Config.SQLiteName", "");
		}
		Json::Value pwd = info->Get("Config.SQLitePWD", "");
		if (!pwd.empty())
		{
			printf("SQLite DB PWD:%s \n",pwd.asCString());
			config.SQLitePWD = pwd.asString();
		}
		else
		{
			info->Set("Config.SQLitePWD","");
		}
	}
	else
	{
		info->Set("Config.ConnectionString", "");
		info->Set("Config.SQLiteName", "");
		info->Set("Config.SQLitePWD", "");
	}
	info->ToFile(exeFullPath);
	return config;
}





