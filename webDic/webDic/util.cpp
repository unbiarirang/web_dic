#include "util.h"
#include "charString.h"
#include <windows.h>
#include <string>

// init output file (result.csv)
void init_file(std::ofstream &opStream) {
	std::string strArr[10] = { "序号,", "网址,", "发帖大类,", "发帖小类,", "发帖标题,",
		"发帖内容,", "发帖人,", "发帖日期,", "发帖类型,", "分词结果\n" };
	
	for (int i = 0; i < 10; i++)
		opStream << ansi_to_utf8(&CharString(strArr[i]));

	return;
}

// wide string to utf8 string
std::string utf8_encode(const std::wstring &wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

// wide string to ansi string
std::string ansi_encode(const std::wstring &wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

// ansi string to utf8 string
std::string ansi_to_utf8(CharString* str) {
	int size = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str->getStr(), str->getLen(), nullptr, 0);
	std::wstring utf16_str(size, '\0');
	MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str->getStr(), str->getLen(), &utf16_str[0], size);

	int utf8_size = WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(),
		utf16_str.length(), nullptr, 0, nullptr, nullptr);
	std::string utf8_str(utf8_size, '\0');
	WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(),
		utf16_str.length(), &utf8_str[0], utf8_size,nullptr, nullptr);
	return utf8_str;
}

// utf8 string to ansi string
std::string utf8_to_ansi(CharString* str) {
	int len = 0;
	WCHAR uni[1000];

	// utf8 to unicode
	len = MultiByteToWideChar(CP_UTF8, 0, str->getStr(), -1, NULL, 0);
	MultiByteToWideChar(CP_UTF8, 0, str->getStr(), -1, uni, len);

	char buff[1000];
	// unicode to ansi
	len = WideCharToMultiByte(CP_ACP, 0, uni, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, uni, -1, buff, len, NULL, NULL);

	return buff;
}

