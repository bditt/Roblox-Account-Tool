#if !JSON11_TEST_CUSTOM_CONFIG
#define JSON11_TEST_CPP_PREFIX_CODE
#define JSON11_TEST_CPP_SUFFIX_CODE
#define JSON11_TEST_STANDALONE_MAIN 1
#define JSON11_TEST_CASE(name) static void name()
#define JSON11_TEST_ASSERT(b) assert(b)
#ifdef NDEBUG
#undef NDEBUG
#endif
#endif

#ifndef JSON11_ENABLE_DR1467_CANARY
#define JSON11_ENABLE_DR1467_CANARY 0
#endif


#include <Windows.h>
#include <cassert>
#include <string>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include "JSON.h"
#include <list>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <type_traits>

#include "/curl/curl.h"

JSON11_TEST_CPP_PREFIX_CODE

using namespace json11;
using std::string;
using namespace std;

void askname();
void getrobuxamount(std::string base);
void options(string cookie);
void getfriendamount(std::string base);
void askid();
void getcookie();

string base = ".ROBLOSECURITY=";
string cookie = "";
string rname = "";
int id;

static size_t data_write(void* buf, size_t size, size_t nmemb, void* userp)
{
	if (userp)
	{
		std::ostream& os = *static_cast<std::ostream*>(userp);
		std::streamsize len = size * nmemb;
		if (os.write(static_cast<char*>(buf), len))
			return len;
	}

	return 0;
}

CURLcode curl_read(const std::string& url, std::ostream& os, std::string cookie, long timeout = 30)
{
	CURLcode code(CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init();

	if (curl)
	{
		if (CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write))
			&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_COOKIE, cookie))
			&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L))
			&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L))
			&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &os))
			&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout))
			&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str())))
		{
			code = curl_easy_perform(curl);
		}
		curl_easy_cleanup(curl);
	}
	return code;
}

// Check that Json has the properties we want.
#define CHECK_TRAIT(x) static_assert(std::x::value, #x)
CHECK_TRAIT(is_nothrow_constructible<Json>);
CHECK_TRAIT(is_nothrow_default_constructible<Json>);
CHECK_TRAIT(is_copy_constructible<Json>);
CHECK_TRAIT(is_nothrow_move_constructible<Json>);
CHECK_TRAIT(is_copy_assignable<Json>);
CHECK_TRAIT(is_nothrow_move_assignable<Json>);
CHECK_TRAIT(is_nothrow_destructible<Json>);

#if JSON11_TEST_STANDALONE_MAIN

static void parse_from_stdin() {
	string buf;
	string line;
	while (std::getline(std::cin, line)) {
		buf += line + "\n";
	}

	string err;
	auto json = Json::parse(buf, err);
	if (!err.empty()) {
		printf("Failed: %s\n", err.c_str());
	}
	else {
		printf("Result: %s\n", json.dump().c_str());
	}
}

void getcookie() {
	printf("Please enter a Roblox cookie: ");
	cin >> cookie;
	printf("\n");
	if (cookie == "") {
		printf("Please enter a cookie!");
	}
	base.append(cookie);
	base.append(";");
}

void getfriendamount(std::string base) {
	std::ostringstream result;
	curl_read("http://api.roblox.com/user/get-friendship-count", result, base);
	string err;
	const auto json = Json::parse(result.str(), err);
	std::cout << "Friend amount: " << json["count"].number_value() << "\n";
	printf("\n");
}

void getrobuxamount(std::string base) {
	std::ostringstream result;
	curl_read("http://api.roblox.com/currency/balance", result, base);
	string err;
	const auto json = Json::parse(result.str(), err);
	std::cout << "Robux amount: " << json["robux"].number_value() << "\n";
	printf("\n");
}

void listfriendsid(std::string base) {
	askname();
	std::ostringstream result;
	string url1 = "http://api.roblox.com/users/get-by-username?username=";
	url1.append(rname);
	curl_read(url1, result, base);
	string err;
	const auto json1 = Json::parse(result.str(), err);
	stringstream convert(json1["Id"].dump());
	convert >> id;
	std::ostringstream resultx;
	string url = "http://api.roblox.com/users/";
	string endurl = url + json1["Id"].dump() + "/friends?page=1";
	curl_read(endurl, resultx, base);
	string errz;
	const auto json = Json::parse(resultx.str(), errz);
	for (auto &k : json.array_items()) {
		const auto kjson = Json::parse(k.dump(), errz);
		std::cout << "ID: " << kjson["Id"].dump() << "\n";
	}
	printf("\n");
}

void listfriendsname(std::string base) {
	askname();
	std::ostringstream result;
	string url1 = "http://api.roblox.com/users/get-by-username?username=";
	url1.append(rname);
	curl_read(url1, result, base);
	string err;
	const auto json1 = Json::parse(result.str(), err);
	stringstream convert(json1["Id"].dump());
	convert >> id;
	std::ostringstream resultx;
	string url = "http://api.roblox.com/users/";
	string endurl = url + json1["Id"].dump() + "/friends?page=1";
	curl_read(endurl, resultx, base);
	string errz;
	const auto json = Json::parse(resultx.str(), errz);
	for (auto &k : json.array_items()) {
		const auto kjson = Json::parse(k.dump(), errz);
		std::cout << "Username: " << kjson["Username"].string_value() << "\n";
	}
	printf("\n");
}

void askname() {
	printf("Please enter the targets Roblox username: ");
	cin >> rname;
	printf("\n");
}

void askid() {
	printf("Please enter the targets Roblox username: ");
	cin >> id;
	printf("\n");
}

void options(string cookie) {
	int option;
	printf("1) Check friend amount.\n");
	printf("2) Check Robux amount.\n");
	printf("3) List all friend IDs.\n");
	printf("4) List all friend names.\n");
	printf("5) Send a friend request.\n");
	printf("6) Change cookie.\n");
	printf("Please enter an option: ");
	cin >> option;
	printf("\n");

	if (option == 1) {
		getfriendamount(cookie);
	}

	else if (option == 2) {
		getrobuxamount(cookie);
	}

	else if (option == 3) {
		listfriendsid(cookie);
	}

	else if (option == 4) {
		listfriendsname(cookie);
	}

	else if (option == 5) {
		getcookie();
	}

	else {
		printf("Please enter a valid option!");
		printf("\n");
	}
}

int main(void) {
	getcookie();
	do {
		options(base);
	} while (true);
}

#endif
JSON11_TEST_CPP_SUFFIX_CODE