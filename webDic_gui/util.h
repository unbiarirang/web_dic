#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <codecvt>
#include <locale>
#include "charString.h"

void init_file(std::ofstream &opStream);

// ansi - utf8 convertion
std::string utf8_encode(const std::wstring &wstr);
std::string ansi_encode(const std::wstring &wstr);
std::string ansi_to_utf8(CharString* str);
std::string utf8_to_ansi(CharString* str);