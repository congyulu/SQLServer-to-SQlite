#include "stdafx.h"
#include "Configs.h"
#include <fstream>


Config::Config(void) :
	value_(Json::objectValue)
{

}


Config::~Config(void)
{

}

bool Config::FromFile(const std::string& sFile)
{
	if (sFile.empty())
		return false;
	char s[256];
	// 获取文件大小
	FILE* file = fopen(sFile.data(), "rt+");
	if (file == NULL) return false;
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	if (file_size <= 0)
		return false;
	fseek(file, 0, SEEK_SET);
	// 从文件读取数据
	char* buffer = new char[file_size];
	fread(buffer, file_size, 1, file);
	fclose(file);

	// 初始化
	bool b = FromString(buffer);
	delete buffer;
	buffer = NULL;

	return b;
}

bool Config::FromString(const std::string& sBuffer)
{
	Json::Reader reader;
	if (!reader.parse(sBuffer, value_)) {
		return false;
	}

	return true;
}

std::string Config::ToString()
{
	Json::StyledWriter writer;
	return writer.write(value_);
}

bool Config::ToFile(const std::string& sSaveFile)
{
	if (sSaveFile.empty())
		return false;

	std::string sOut = ToString();

	std::ofstream f(sSaveFile.c_str());
	if (!f.good()) {
		return false;
	}

	f.write(sOut.c_str(), sOut.size());
	f.close();

	return true;
}


#ifdef USING_TestCode
class TestCode_Config {
public:
	TestCode_Config() {
		Config config;
		config.FromString("{}");
		config.Set("test.base.savepath", "D:/test");

		Json::Value v = config.Get("base.savepath", "");
		std::string s = v.asString();
		OutputDebugStringA(s.c_str());
		std::string sJsonString;
		sJsonString = config.ToString();
		OutputDebugStringA(sJsonString.c_str());
	}

	~TestCode_Config() {

	}
} TestCode_Config_c;
#endif