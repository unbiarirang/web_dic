#pragma once
#include "charString.h"
#include "curl\curl.h"
#include "util.h"
#include "hash.h"
#include "docNode.h"
#include <vector>
#include <string>
#include <fstream>
#include <ostream>

void analyze_html(int tid, std::vector<CharString>* urlStrs, std::ofstream &opStream, HashTable *dic, HashTable *dic_ex);
void get_url_str(std::vector<CharString>* urlStrs, CharString fileName);
CharString get_html_str(int tid, CharString urlStr);

int get_index_of_match_str(CharString dataStr, CharString matchStr, int prev_index);
int extract_data_from_html(CharString dataStr, CharString matchStr, char tag, int index_prev, std::vector<CharString>* resultStr, bool* exceptional_flag, int res_index);
int extract_data_from_html2(CharString dataStr, CharString matchStr, char tag, int index_prev, std::vector<CharString>* resultStr);

void split_words(HashTable* dic, HashTable* dic_ex, std::vector<CharString>* resultStr, CharString* srcStr, DocID docID);
void filter_result_str(std::vector<CharString>* resultStr, std::wstring* wideData);
void get_search_str(std::vector<CharString>* searchStrs, CharString fileName);

const int MAX_THREAD_NUM = 10;
const int URL_NUM = 100;
const int MAX_URL_LENGTH = 256;