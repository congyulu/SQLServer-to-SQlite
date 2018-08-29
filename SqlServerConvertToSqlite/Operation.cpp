#include "stdafx.h"
#include "Operation.h"


Operation::Operation()
{
}


Operation::~Operation()
{
}

string Operation::String_To_UTF8(string str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	wchar_t * pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, pwBuf, nwLen);
	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char * pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);
	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
	string retStr(pBuf);
	delete[]pwBuf;
	delete[]pBuf;
	pwBuf = NULL;
	pBuf = NULL;
	return retStr;
}

char* Operation::Char_To_UTF8(const char* str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	wchar_t * pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, str, -1, pwBuf, nwLen);
	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char * pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);
	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
	delete[]pwBuf;
	pwBuf = NULL;
	return pBuf;
}
