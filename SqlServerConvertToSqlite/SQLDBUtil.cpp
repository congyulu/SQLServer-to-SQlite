#include "stdafx.h"
#include "SQLDBUtil.h"
#pragma comment(lib, "shell32.lib")
#include <ShlObj.h>
SQLDBUtil::SQLDBUtil(LPCTSTR sqliteName, LPCTSTR sqlitePWD)
{
	s_name = sqliteName;
	s_pwd = sqlitePWD;
}

SQLDBUtil::~SQLDBUtil(void)
{

}

BOOL SQLDBUtil::Init(SQLITE_DB type)
{
	ZeroMemory(exeFullPath, MAX_PATH);
	switch (type)
	{
	case SQLITE_LOCAL: {
		TCHAR szDocument[MAX_PATH] = { 0 };
		if (GetModuleFileName(NULL, szDocument, MAX_PATH) != 0)
		{
			TCHAR drive[_MAX_DRIVE];
			TCHAR dir[_MAX_DIR];
			_splitpath(szDocument, drive, dir, NULL, NULL);
			strcat(exeFullPath, drive);
			strcat(exeFullPath, dir);
			strcat(exeFullPath, s_name);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	default:return FALSE;
	}
}

BOOL SQLDBUtil::Open(int sqliteType)
{
	if (exeFullPath == NULL)
	{
		return FALSE;
	}
	sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
	if (sqlite3_open_v2(exeFullPath, &w_db, sqliteType, NULL) != SQLITE_OK)
	{
		MessageBox(NULL, GetLastErrorMsg(), "错误", 0);
		return FALSE;
	}
	if (s_pwd&&strlen(s_pwd) > 0)
	{
		sqlite3_key(w_db, s_pwd, strlen(s_pwd));
		sqlite3_rekey(w_db, s_pwd, strlen(s_pwd));
	}
	return TRUE;
}

void SQLDBUtil::Close()
{
	if (w_db)
	{
		sqlite3_close_v2(w_db);
		w_db = NULL;
	}
}

BOOL SQLDBUtil::ExcuteNonQuery(LPCTSTR lpSql)
{
	if (lpSql == NULL&&w_db)
	{
		return FALSE;
	}
	char* error;
	BOOL result = FALSE;
#ifdef  UNICODE  
	char *szSql = oper.wcharTochar(lpSql);
	if (sqlite3_exec(w_db, szSql, 0, 0, &error) == SQLITE_OK)
#else   
	char *szSql = oper.Char_To_UTF8(lpSql);
	if (sqlite3_exec(w_db, szSql, 0, 0, &error) == SQLITE_OK)
#endif   
	{
		result = TRUE;
	}
	delete szSql;
	return result;
}

BOOL SQLDBUtil::ExcuteNonQuery(SQLiteCommand* pCmd)
{
	if (pCmd == NULL)
	{
		return FALSE;
	}
	return pCmd->Excute();
}

BOOL SQLDBUtil::ExcuteQuery(LPCTSTR lpSql, QueryCallback pCallBack)
{
	if (lpSql == NULL || pCallBack == NULL)
	{
		return FALSE;
	}
	char *errmsg = NULL;
#ifdef  UNICODE    
	char *szSql = oper.wcharTochar(lpSql);
	if (sqlite3_exec(w_db, szSql, pCallBack, NULL, &errmsg) != SQLITE_OK)
	{
		delete[] szSql;
		return FALSE;
	}
	delete[] szSql;
#else   
	char *szSql = oper.Char_To_UTF8(lpSql);
	if (sqlite3_exec(w_db, szSql, pCallBack, NULL, &errmsg) != SQLITE_OK)
	{
		return FALSE;
	}
#endif   
	return TRUE;
}

SQLiteDataReader* SQLDBUtil::ExcuteQuery(LPCTSTR lpSql)
{
	if (lpSql == NULL)
	{
		return FALSE;
	}
	sqlite3_stmt* stmt;
#ifdef  UNICODE    
	if (sqlite3_prepare16_v2(w_db, lpSql, -1, &stmt, NULL) != SQLITE_OK)
#else   
	if (sqlite3_prepare_v2(w_db, lpSql, -1, &stmt, NULL) != SQLITE_OK)
#endif   
	{
		return FALSE;
	}
	return new SQLiteDataReader(stmt);
}

// 开始事务   
BOOL SQLDBUtil::BeginTransaction()
{
	char * errmsg = NULL;
	if (sqlite3_exec(w_db, "BEGIN TRANSACTION;", NULL, NULL, &errmsg) != SQLITE_OK)
	{
		return FALSE;
	}
	sqlite3_free(errmsg);
	return TRUE;
}

BOOL SQLDBUtil::CommitTransaction()
{
	char * errmsg = NULL;
	if (sqlite3_exec(w_db, "COMMIT TRANSACTION;;", NULL, NULL, &errmsg) != SQLITE_OK)
	{
		sqlite3_free(errmsg);
		return FALSE;
	}

	return TRUE;
}

BOOL SQLDBUtil::RollbackTransaction()
{
	char * errmsg = NULL;
	if (sqlite3_exec(w_db, "ROLLBACK  TRANSACTION;", NULL, NULL, &errmsg) != SQLITE_OK)
	{
		sqlite3_free(errmsg);
		return FALSE;
	}
	return TRUE;
}

LPCTSTR SQLDBUtil::GetLastErrorMsg()
{
#ifdef UNICODE    
	return (LPCTSTR)sqlite3_errmsg16(w_db);
#else   
	return sqlite3_errmsg(w_db);
#endif   
}





SQLiteDataReader::SQLiteDataReader(sqlite3_stmt *pStmt) :
	m_pStmt(pStmt)
{

}

SQLiteDataReader::~SQLiteDataReader()
{
	Close();
}

BOOL SQLiteDataReader::Read()
{
	if (m_pStmt == NULL)
	{
		return FALSE;
	}
	if (sqlite3_step(m_pStmt) != SQLITE_ROW)
	{
		return FALSE;
	}
	return TRUE;
}

void SQLiteDataReader::Close()
{
	if (m_pStmt)
	{
		sqlite3_finalize(m_pStmt);
		m_pStmt = NULL;
	}
}

int SQLiteDataReader::ColumnCount(void)
{
	return sqlite3_column_count(m_pStmt);
}

LPCTSTR SQLiteDataReader::GetName(int nCol)
{
#ifdef  UNICODE    
	return (LPCTSTR)sqlite3_column_name16(m_pStmt, nCol);
#else   
	return (LPCTSTR)sqlite3_column_name(m_pStmt, nCol);
#endif   
}

SQLITE_DATATYPE SQLiteDataReader::GetDataType(int nCol)
{
	return (SQLITE_DATATYPE)sqlite3_column_type(m_pStmt, nCol);
}

#ifdef  UNICODE    
wchar_t* SQLiteDataReader::GetStringValue(int nCol)
#else 
char* SQLiteDataReader::GetStringValue(int nCol)
#endif
{
#ifdef  UNICODE    
	char* value = (char*)sqlite3_column_text(m_pStmt, nCol);
	wchar_t *wvalue = oper.charTowchar(value);
	return wvalue;
#else   
	return (char*)sqlite3_column_text(m_pStmt, nCol);
#endif
}

int SQLiteDataReader::GetIntValue(int nCol)
{
	return sqlite3_column_int(m_pStmt, nCol);
}

long SQLiteDataReader::GetInt64Value(int nCol)
{
	return (long)sqlite3_column_int64(m_pStmt, nCol);
}

double SQLiteDataReader::GetFloatValue(int nCol)
{
	return sqlite3_column_double(m_pStmt, nCol);
}

const BYTE* SQLiteDataReader::GetBlobValue(int nCol, int &nLen)
{
	nLen = sqlite3_column_bytes(m_pStmt, nCol);
	return (const BYTE*)sqlite3_column_blob(m_pStmt, nCol);
}

SQLiteCommand::SQLiteCommand(SQLDBUtil* pSqlite) :
	m_pSqlite(pSqlite),
	m_pStmt(NULL)
{
}

SQLiteCommand::SQLiteCommand(SQLDBUtil* pSqlite, LPCTSTR lpSql) :
	m_pSqlite(pSqlite),
	m_pStmt(NULL)
{
	SetCommandText(lpSql);
}

SQLiteCommand::~SQLiteCommand()
{

}

BOOL SQLiteCommand::SetCommandText(LPCTSTR lpSql)
{
#ifdef  UNICODE    
	if (sqlite3_prepare16_v2(m_pSqlite->w_db, lpSql, -1, &m_pStmt, NULL) != SQLITE_OK)
#else   
	if (sqlite3_prepare_v2(m_pSqlite->w_db, lpSql, -1, &m_pStmt, NULL) != SQLITE_OK)
#endif   
	{
		return FALSE;
	}
	return TRUE;
}

BOOL SQLiteCommand::BindParam(int index, LPCTSTR szValue)
{
#ifdef  UNICODE    
	if (sqlite3_bind_text16(m_pStmt, index, szValue, -1, SQLITE_TRANSIENT) != SQLITE_OK)
#else   
	if (sqlite3_bind_text(m_pStmt, index, szValue, -1, SQLITE_TRANSIENT) != SQLITE_OK)
#endif   
	{
		return FALSE;
	}
	return TRUE;
}

BOOL SQLiteCommand::BindParam(int index, const int nValue)
{
	if (sqlite3_bind_int(m_pStmt, index, nValue) != SQLITE_OK)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL SQLiteCommand::BindParam(int index, const double dValue)
{
	if (sqlite3_bind_double(m_pStmt, index, dValue) != SQLITE_OK)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL SQLiteCommand::BindParam(int index, const unsigned char* blobBuf, int nLen)
{
	if (sqlite3_bind_blob(m_pStmt, index, blobBuf, nLen, NULL) != SQLITE_OK)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL SQLiteCommand::Excute()
{
	sqlite3_step(m_pStmt);

	return (sqlite3_reset(m_pStmt) == SQLITE_OK) ? TRUE : FALSE;
}

void SQLiteCommand::Clear()
{
	if (m_pStmt)
	{
		sqlite3_finalize(m_pStmt);
		m_pStmt = NULL;
	}
}