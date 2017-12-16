#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdio.h>
#include <thread>
#include "stack.h"
#include "charString.h"
#include "csNode.h"
#include "crawler.h"
#include "curl/curl.h"
#include "util.h"
#include "hash.h"
#include "avlTree.h"
#include "map.h"

using namespace std;

extern AVLTree tree;	// AVL tree inverse document
extern HashDoc hashDoc;	// hash table inverse document
extern Map map;			// word-wordID map

int main(void)
{
	// read from input file url.csv
	vector<CharString>* urlStrs = new vector<CharString>;
	get_url_str(urlStrs, "input/url.csv");

	// open UTF8 encoding csv file
	ofstream opStream;
	opStream.open("output/result.csv");
	const char *bom = "\xef\xbb\xbf"; // UTF-8
	opStream << bom;
	init_file(opStream);

	// init dictionary
	HashTable *dic = new HashTable();	 // dictionary for words division
	HashTable *dic_ex = new HashTable(); // dictionary for exclusion
	dic->appendFrom("dic.txt");
	dic->appendFrom("dic_special.txt");
	dic_ex->appendFrom("dic_exclusion.txt");

	std::thread t[MAX_THREAD_NUM];
	curl_global_init(CURL_GLOBAL_ALL);

	//Launch a group of threads
	for (int i = 0; i < MAX_THREAD_NUM; i++) {
		// main algorithm
		t[i] = std::thread(analyze_html, i, std::ref(urlStrs), std::ref(opStream), std::ref(dic), std::ref(dic_ex));
		//t[i] = std::thread([&]() {_analyze_html(i, urlStrs, opStream, curl, dic, dic_ex); return 1; }); // for VC2012
		std::cout << "Launched from the main\n";
	}

	//Join the threads with the main thread
	for (int i = 0; i < MAX_THREAD_NUM; i++)
		t[i].join();

	opStream.close();




	vector<CharString>* searchStrs = new vector<CharString>;
	vector<CharString>* resultStr = new vector<CharString>;
	get_search_str(searchStrs, "query.txt");
	opStream.open("result.txt");

	WordNode* node;
	int result[URL_NUM + 1] = { 0 };
	for (int i = 0; i < searchStrs->size(); i++) {
		split_words(dic, dic_ex, resultStr, &(*searchStrs)[i], 0);
		
		for (int j = 0; j < resultStr->size(); j++) {
			node = tree.search(map[(*resultStr)[j].getStr()]);
			// print result
			if (node) {
				cout << node->occur_count << endl;
				DocNode* n = node->doc_list->getHead();
				cout << (*resultStr)[j].getStr() << ": ";
				while (n) {
					result[n->id] += n->occur_count;
					cout << "(" << n->id << " ," << n->occur_count << ") ";
					n = n->next;
				}
				cout << endl;
			}
		}
		cout << "=============real result===============" << endl;
		for (int j = 1; j < URL_NUM + 1; j++) {
			if (result[j] == 0) continue;
			cout << "(" << j << ", " << result[j] << ") ";
			opStream << "(" << j << ", " << result[j] << ") ";
		}
		cout << endl << "======================================" << endl;
		opStream << endl;
		// init results
		resultStr->clear();
		memset(result, 0, sizeof(int) * (URL_NUM + 1));
	}

	





	opStream.close();

	system("pause");
	return 0;
}

//int main() {
//	HashTable* dic = new HashTable;
//	HashTable* dic_ex = new HashTable;
//	dic->appendFrom("dic.txt");
//	dic->appendFrom("dic_special.txt");
//	dic_ex->appendFrom("dic_exclusion.txt");
//
//	ofstream opStream;
//	opStream.open("result.csv");
//	const char *bom = "\xef\xbb\xbf"; // UTF-8
//	opStream << bom;
//	init_file(opStream);
//	
//	CharString str = "求购精品小挖，纯进口，合资机优先！";
//	CharString str2 = "打算最近买台小挖，在农村做事。型号不能小于70，车况要好，有合适的吗？万一没有打算前往深圳二手机械市场！";
//	vector<CharString> resultStr;
//	vector<CharString> resultStr2;
//	str.concat(&str2);
//	split_words(dic, dic_ex, &resultStr, &str);
//	//split_words(dic, dic_ex, &resultStr2, &str2);
//
//	for (auto word : resultStr) {
//		word.concat(" ");
//		std::cout << word;
//		opStream << ansi_to_utf8(&word);
//		opStream << endl;
//	}
//	for (auto word : resultStr2) {
//		word.concat(" ");
//		std::cout << word;
//		opStream << ansi_to_utf8(&word);
//		opStream << endl;
//	}
//	system("pause");
//}


/***************************************************************************
*                                  _   _ ____  _
*  Project                     ___| | | |  _ \| |
*                             / __| | | | |_) | |
*                            | (__| |_| |  _ <| |___
*                             \___|\___/|_| \_\_____|
*
* Copyright (C) 1998 - 2017, Daniel Stenberg, <daniel@haxx.se>, et al.
*
* This software is licensed as described in the file COPYING, which
* you should have received as part of this distribution. The terms
* are also available at https://curl.haxx.se/docs/copyright.html.
*
* You may opt to use, copy, modify, merge, publish, distribute and/or sell
* copies of the Software, and permit persons to whom the Software is
* furnished to do so, under the terms of the COPYING file.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express or implied.
*
***************************************************************************/
/* <DESC>
* A multi-threaded example that uses pthreads to fetch several files at once
* </DESC>
*/

#define NUMT 4

/*
List of URLs to fetch.

If you intend to use a SSL-based protocol here you might need to setup TLS
library mutex callbacks as described here:

https://curl.haxx.se/libcurl/c/threadsafe.html

*/
const char * const urls[8] = {
	"http://bbs.cehome.com/thread-614565-1-1.html",
	"http://bbs.cehome.com/thread-614565-1-1.html",
	"http://bbs.cehome.com/thread-614565-1-1.html",
	"www.haxx.se",
	"www.haxx.se",
	"www.haxx.se",
	"www.haxx.se",
	"www.haxx.se"
};

static void *pull_one_url(void *url)
{
	CURL *curl;

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_perform(curl); /* ignores error */
	curl_easy_cleanup(curl);

	return NULL;
}


static void *pull_two_url(void *i)
{
	CURL *curl;

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, urls[int(i)]);
	curl_easy_perform(curl); /* ignores error */
	curl_easy_setopt(curl, CURLOPT_URL, urls[int(i) + 1]);
	curl_easy_perform(curl); /* ignores error */
	curl_easy_cleanup(curl);

	return NULL;
}


/*
int pthread_create(pthread_t *new_thread_ID,
const pthread_attr_t *attr,
void * (*start_func)(void *), void *arg);
*/
//
//int main(int argc, char **argv)
//{
//	pthread_t tid[NUMT];
//	int i;
//	int error;
//
//	std::vector<const char*> url_vec;
//	url_vec.push_back("https://curl.haxx.se/");
//	url_vec.push_back("https://curl.haxx.se/");
//	url_vec.push_back("https://curl.haxx.se/");
//	url_vec.push_back("https://curl.haxx.se/");
//	url_vec.push_back("https://curl.haxx.se/");
//	url_vec.push_back("https://curl.haxx.se/");
//	url_vec.push_back("https://curl.haxx.se/");
//	url_vec.push_back("https://curl.haxx.se/");
//
//
//	/* Must initialize libcurl before any threads are started */
//	curl_global_init(CURL_GLOBAL_ALL);
//
//	for (i = 0; i< NUMT; i++) {
//		error = pthread_create(&tid[i],
//			NULL, /* default attributes please */
//			pull_two_url,
//			(void *)(i * 2));
//		if (0 != error)
//			fprintf(stderr, "Couldn't run thread number %d, errno %d\n", i, error);
//		else
//			fprintf(stderr, "Thread %d, gets %s\n", i, urls[i]);
//	}
//
//	/* now wait for all threads to terminate */
//	for (i = 0; i< NUMT; i++) {
//		error = pthread_join(tid[i], NULL);
//		fprintf(stderr, "Thread %d terminated\n", i);
//	}
//
//	system("pause");
//	return 0;
//}
void call_from_thread(int tid) {
	std::cout << "Launched by thread " << tid << std::endl;
}

void pull_one_url2(const char *url, int *i)
{
	CURL *curl;

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_perform(curl); /* ignores error */
	curl_easy_cleanup(curl);

	return;
}

void pull_two_url2(int i)
{
	CURL *curl;

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, urls[i]);
	curl_easy_perform(curl); /* ignores error */
	curl_easy_setopt(curl, CURLOPT_URL, urls[i]);
	curl_easy_perform(curl); /* ignores error */
	curl_easy_cleanup(curl);

	return ;
}

void get_html_str2(CharString urlStr)
{
	CURLcode res;

	CURL* curl2 = curl_easy_init();
	if (curl2) {
		curl_easy_setopt(curl2, CURLOPT_URL, urlStr.getStr());
		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curl2, CURLOPT_FOLLOWLOCATION, 1L);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl2);

		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		curl_easy_cleanup(curl2);
	}
}

static const int num_threads = 4;
//int main() {
//	std::thread t[num_threads];
//	int x = 1;
//	
//	//Launch a group of threads
//	for (int i = 0; i < num_threads; i++) {
//		t[i] = std::thread(pull_one_url2, "www.haxx.se", std::ref(x));
//		std::cout << "Launched from the main\n";
//	}
//
//
//	////Join the threads with the main thread
//	for (int i = 0; i < num_threads; ++i) {
//	    t[i].join();
//	
//	}
//	system("pause");
//	return 0;
//}



//
//#include "avlTree.h"
//#include "hashMap.h"
//int main(void)
//{
//	AVLTree t;
//	HashMap map;
//
//	string str;
//	cin >> str;
//	CharString input_str = str;
//	WordNode* node = new WordNode(1);
//	node->word = str;
//	t.insert(1, node);
//	map.insert(str, 1);
//
//	cin >> str;
//	input_str = str;
//	node = new WordNode(2);
//	node->word = str;
//	t.insert(2, node);
//	map.insert(str, 2);
//
//	cout << t.search(1)->id << t.search(1)->word << endl;
//	cout << t.search(2)->id << t.search(2)->word << endl;
//	system("pause");
//}