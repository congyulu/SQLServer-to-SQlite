#pragma once

class Operation
{
public:
	Operation();
	~Operation();
public:
	std::string String_To_UTF8(std::string str);
	char * Char_To_UTF8(const char * str);
};

