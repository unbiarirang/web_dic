#include "crawler.h"
#include "charString.h"
#include "stack.h"
#include "curl\curl.h"
#include "avlTree.h"
#include "map.h"
#include "hash.h"
#include <pthread.h>
#include <thread>
#include <memory>
#include <vector>
#include <string>
#include <mutex>

AVLTree tree;	// AVL tree inverse document
HashDoc hashDoc;// hash table inverse document
Map map;		// word-wordID map
std::mutex m;
int g_key_index = 1; // word unique id index

// main algorithm
void analyze_html(int tid, std::vector<CharString>* urlStrs, std::ofstream &opStream, HashTable *dic, HashTable *dic_ex) {
	std::vector<CharString>* words = new std::vector<CharString>; // split words result
	std::vector<CharString> resultStr(4);
	std::wstring wideData = L"";

	for (int i = (URL_NUM / MAX_THREAD_NUM) * tid; i < (URL_NUM / MAX_THREAD_NUM) * (tid + 1); i++) {
		DocID docID = i + 1;
		resultStr.assign(4, "");
		CharString urlStr = (*urlStrs)[i];
		CharString dataStr = get_html_str(tid, urlStr);

		bool exceptional_flag = false; // exceptional situation. in case of ansi string main contents
		int index = 0;

		resultStr[0] = urlStr;

		// extract data trash1, trash2
		index = extract_data_from_html(dataStr, "div class=\"z\"", 'a', index, &resultStr, &exceptional_flag, 3);
		// no authority to visit this html page
		if (index == -1) {
			resultStr[0].concat("\n");
			std::cout << resultStr[0];
			opStream << ansi_to_utf8(&resultStr[0]);
			resultStr.clear();
			continue;
		}
		index = extract_data_from_html(dataStr, "a href=", 'a', index, &resultStr, &exceptional_flag, 3);
		// extract data 1~3 (article big category, article small category, title)
		index = extract_data_from_html(dataStr, "a href=", 'a', index, &resultStr, &exceptional_flag, 3);
		index = extract_data_from_html(dataStr, "a href=", 'a', index, &resultStr, &exceptional_flag, 3);
		index = extract_data_from_html(dataStr, "a href=", 'a', index, &resultStr, &exceptional_flag, 1);
		// extract data 7 (article type)
		index = extract_data_from_html(dataStr, "div class=\"ts z h1\"", 'a', index, &resultStr, &exceptional_flag, 3);
		// extract data 5 (writer)
		index = extract_data_from_html(dataStr, "div class=\"authi\"", 'a', index, &resultStr, &exceptional_flag, 3);
		// extract data 6 (date)
		index = extract_data_from_html(dataStr, "div class=\"authi\"", 'e', index, &resultStr, &exceptional_flag, 3);
		//extract data 4 (main contents)
		index = extract_data_from_html(dataStr, "div class=\"t_fsz\"", 'p', index, &resultStr, &exceptional_flag, 2);
		if (exceptional_flag == true)
			index = extract_data_from_html2(dataStr, "div class=\"t_fsz\"", 't', index, &resultStr);

		filter_result_str(&resultStr, &wideData);

		for (int k = 0; k < 3; k++) {
			// handle main contents
			if (k == 2) {
				bool decimal_flag = false;
				int j = 0;
				// in case of decimal code main contents
				while (true) {
					if (j >= resultStr[k].getLen()) break;
					if (resultStr[k].getStr()[j] == '&') {
						decimal_flag = true; break;
					}
					if (resultStr[k].getStr()[j] < '0' || resultStr[k].getStr()[j] > '9') {
						decimal_flag = false; break;
					}
					j++;
				}
				if (decimal_flag == true) {
					CharString str = ansi_encode(wideData);
					str.concat(",");
					opStream << ansi_to_utf8(&str);
					std::cout << str;
					split_words(dic, dic_ex, words, &str, docID);
					continue;
				}
				else {};
			}

			// print result string
			resultStr[k].concat(",");
			opStream << ansi_to_utf8(&resultStr[k]);
			std::cout << resultStr[k];

			// split words only from title and main contents
			if (k == 1 || k == 2)
				split_words(dic, dic_ex, words, &resultStr[k], docID);
		}
		// print split words result
		for (auto word : *words) {
			word.concat(" ");
			std::cout << word;
			opStream << ansi_to_utf8(&word);
		}

		std::cout << "\n";
		opStream << std::endl;
		words->clear();
		resultStr.clear();
	}
	std::cout << "tid: " << tid << " ended" << std::endl;
}

// get url strings from input file
void get_url_str(std::vector<CharString>* urlStrs, CharString fileName)
{
	char buffer[MAX_URL_LENGTH] = { 0 };
	std::ifstream readFile(fileName.getStr());
	try {
		if (readFile.is_open() == true) {
			std::cout << "read file: " << fileName << "\n";
			// delete the first line of the file
			readFile.getline(buffer, sizeof(char) * MAX_URL_LENGTH);

			while (readFile.getline(buffer, sizeof(char) * MAX_URL_LENGTH)) {
				CharString tempStr = buffer;
				char* temp = tempStr.substring(tempStr.indexOf("\"") + 1, tempStr.getLen() - 2);
				urlStrs->push_back(temp);
			}

			std::cout << "close file: " << fileName << "\n";
			readFile.close();
		}
	}
	catch (bool) {
		std::cout << "fail to read file: " << fileName << "\n";
	}
}

void get_search_str(std::vector<CharString>* searchStrs, CharString fileName)
{
	std::vector<CharString> search_strs;
	char buffer[1024] = { 0 };
	std::ifstream readFile(fileName.getStr());
	try {
		if (readFile.is_open() == true) {
			std::cout << "success read file " << fileName << std::endl;
			while (readFile.getline(buffer, sizeof(char) * 1024))
				searchStrs->push_back(buffer);
			readFile.close();
		}
	}
	catch (bool) {
		std::cout << "fail to read file" << std::endl;
	}
}

// curl write callback
std::string g_dataStr[MAX_THREAD_NUM];
size_t _write_callback(char* buf, size_t size, size_t nmemb, void* tid)
{
	for (int c = 0; c<size*nmemb; c++)
		g_dataStr[int(tid)].push_back(buf[c]);
	return size*nmemb;
}

CharString get_html_str(int tid, CharString urlStr)
{
	CURLcode res;
	g_dataStr[tid] = std::string();

	CURL* curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, urlStr.getStr());
		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &_write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)tid);

		/* Perform the request, res will get the return code */
		m.lock();
		res = curl_easy_perform(curl);
		m.unlock();

		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		curl_easy_cleanup(curl);
	}
	return CharString(g_dataStr[tid]);
}

// return the end index of match string
int extract_data_from_html(CharString dataStr, CharString matchStr, char tag, int index_prev, std::vector<CharString>* resultStr, bool* exceptional_flag, int res_index)
{
	int temp_index_prev = index_prev;
	int index = get_index_of_match_str(dataStr, matchStr, index_prev);
	if (index == -1) return -1;

	bool get_tag_flag = false;
	char c_before = 0;
	Stack s = Stack();
	Stack s2 = Stack();
	char data[1024] = { 0 };
	int data_index = 0;

	while (true) {
		char c = dataStr.getStr()[index];
		if (c_before == '<' && c == tag) get_tag_flag = true;
		if (get_tag_flag)
			s.push(c);
		// get data between <tag>...</tag>
		if (c_before == '/' && c == tag) {
			while (!s.empty()) {
				s2.push(s.top());
				s.pop();
			}
			bool get_content_flag = false;
			char c;
			while (!s2.empty()) {
				c = s2.top();
				if (c == '<' && get_content_flag == true) break;
				if (get_content_flag) {
					data[data_index] = s2.top();
					data_index++;
				}
				if (c == '>') get_content_flag = true;
				s2.pop();
			}

			// exceptional situation. for ansi string main contents
			if (data[0] == '\n') {
				*exceptional_flag = true;
				return temp_index_prev;
			}

			(*resultStr)[res_index] = data;
			return index;
		}

		c_before = c;
		index++;
	}
}

// exceptional situation. for ansi string main contents
int extract_data_from_html2(CharString dataStr, CharString matchStr, char tag, int index_prev, std::vector<CharString>* resultStr)
{
	int temp_index_prev = index_prev;
	int index = get_index_of_match_str(dataStr, matchStr, index_prev);
	if (index == -1) return -1;

	bool get_tag_flag = false;
	char c_before = 0;
	Stack s = Stack();
	Stack s2 = Stack();
	char data[1024] = { 0 };
	int data_index = 0;
	int count = 0;

	while (true) {
		char c = dataStr.getStr()[index];
		if (count == 4) get_tag_flag = true;
		if (c == '>') count++;
		// replace "&nbsp;" with " "
		if (c == '&') { index += 6; s.push(' '); continue; }
		if (get_tag_flag && c != '\n' && c != ' ')
			s.push(c);
		// get data between <tag>...</tag>
		if (get_tag_flag && c == '<') {
			s.pop();
			while (!s.empty()) {
				s2.push(s.top());
				s.pop();
			}

			while (!s2.empty()) {
				if (s2.top() != ',' && s2.top() != '\n') {
					data[data_index] = s2.top();
					data_index++;
				}
				s2.pop();
			}

			(*resultStr)[2] = data;
			return index;
		}
		c_before = c;
		index++;
	}
}

const int MAX_EXTRACT_STRING_LENGTH = 1024;
char extractStr[MAX_EXTRACT_STRING_LENGTH];
int get_index_of_match_str(CharString dataStr, CharString matchStr, int prev_index)
{
	Stack s = Stack();
	Stack s2 = Stack();
	char c = 0;
	int index = prev_index;
	bool push_flag = false;

	while (index < dataStr.getLen()) {
		c = dataStr.getStr()[index];

		// get data between the bracket <...>
		if (c == '>' && push_flag == true) {
			push_flag = false;
			while (!s.empty()) {
				s2.push(s.top());
				s.pop();
			}
			int i = 0;
			while (!s2.empty()) {
				extractStr[i] = s2.top();
				s2.pop();
				i++;
			}
			extractStr[i] = '\0';
			CharString tempStr = extractStr;
			if (tempStr.indexOf(&matchStr) >= 0)
				return index - tempStr.getLen() - 1;
			
			for (int j = 0; j <= i; j++) {
				extractStr[j] = 0;
			}
		}

		if (push_flag)
			s.push(c);

		if (c == '<') push_flag = true;
		
		index++;
	}
	return -1;
}

// use dic to split the source string
const int WORD_MAX_LEN = 8;
void split_words(HashTable* dic, HashTable* dic_ex, std::vector<CharString>* resultStr, CharString* srcStr, DocID docID)
{
	CharString str = "";
	int str_len = WORD_MAX_LEN;
	int str_start_index = 0;
	int str_end_index = WORD_MAX_LEN;
	int index = 0;

	while (true) {
		str_len = WORD_MAX_LEN;
		while (true) {
			str_end_index = str_start_index + str_len;
			if (str_end_index > srcStr->getLen()) str_end_index = srcStr->getLen();

			index = str_start_index;
			while (true) {
				if (index == str_end_index) break;
				str.concat(srcStr->getStr()[index]);
				index++;
			}

			// get a series of number
			if (str.getStr()[0] >= 48 && str.getStr()[0] <= 57) {
				int num_count = 0;
				int num_index = str_start_index;
				while (true) {
					if (srcStr->getStr()[num_index] < 48 || srcStr->getStr()[num_index] > 57) break;
					num_count++;
					num_index++;
				}
				int i = 8;
				while (i < num_count) {
					str.concat(&(srcStr->getStr()[str_start_index + i]));
					i++;
				}
				str.getStr()[num_count] = '\0';
				resultStr->push_back(str.substring(0, num_count-1));
				str_start_index = str_start_index + num_count;
				str = "";
				break;
			}

			// remove all ascii characters except numbers
			if ((str.getStr()[0] >= 32 && str.getStr()[0] < 48) || (str.getStr()[0] > 57 && str.getStr()[0] <= 148)) {
				str_start_index = str_start_index + 1;
				str = "";
				break;
			}

			// filter exclusion words
			if (dic_ex->isExist(str.getStr())) {
				str_start_index = str_start_index + str_len;
				str = "";
				break;
			}

			// str remains a character
			if (str_len == 2) {
				resultStr->push_back(str);	// save word
				str_start_index = str_start_index + str_len;
				str = "";
				break;
			}

			// str is a word
			if (dic->isExist(str)) {
				resultStr->push_back(CharString(str));	// save word

				// add a word to inverse document and map
				if (docID) {
					m.lock();
					bool is_success = map.insert(str, g_key_index);
					if (is_success) { // new word
						WordNode* node = new WordNode(g_key_index, str);
						node->add(docID);
						tree.insert(g_key_index, node);
						hashDoc.insert(g_key_index, node);
						g_key_index++;
					}
					else { // the word already exist in inverse document and map
						int wordID = map[str];
						//WordNode* node = tree.search(wordID);
						WordNode* node = hashDoc.search(wordID);
						node->add(docID);
					}
					m.unlock();
				}

				str_start_index = str_start_index + str_len;
				str = "";
				break;
			}
			str_len--;
			str = "";
		}

		if (str_start_index >= srcStr->getLen()) break;
	}
}

void filter_result_str(std::vector<CharString>* resultStr, std::wstring* wideData)
{
	bool is_decimal = false;
	int j = 0;
	// check if the main contents is decimal code
	while (true) {
		if ((*resultStr)[2].getStr()[j] == '&') {
			is_decimal = true; break;
		}
		if ((*resultStr)[2].getStr()[j] < '0' || (*resultStr)[2].getStr()[j] > '9') {
			is_decimal = false; break;
		}
		j++;
	}

	// convert decimal code to wide string
	if (is_decimal == true) {
		CharString contentStr = (*resultStr)[2];
		std::wstring str = L"";
		Stack s;
		Stack s2;
		bool end_flag = false;
		bool push_flag = false;
		bool non_decimal_flag = false;
		long long code = 0;
		for (int i = 0; i < contentStr.getLen(); i++) {
			char c = contentStr.getStr()[i];
			if (c == ' ' || c == ',') continue;
			if (i == 0 && c != '&') end_flag = true;
			if (c == ';') {
				end_flag = true;

				if (!s.empty()) {
					while (!s.empty()) {
						s2.push(s.top());
						s.pop();
					}
					while (!s2.empty()) {
						code = code * 10 + s2.top() - 48;
						s2.pop();
					}
				}

				if (code) {
					wchar_t temp = wchar_t(code);
					str += temp;
				}
				else // &nbsp;
					str += L" ";

				code = 0;
				push_flag = false;
				non_decimal_flag = false;
				continue;
			}
			// non decimal code
			if (end_flag == true && c != '&') {
				non_decimal_flag = true;
				if (c >= '0' && c <= '9')
					str += std::to_wstring(c - 48);
				else if (c == '\0') {}
				else
					str += c;

				continue;
			}
			else
				end_flag = false;

			if (push_flag == true)
				s.push(c);
			if (c == '#')
				push_flag = true;
		}
		*wideData = str;
	}
}