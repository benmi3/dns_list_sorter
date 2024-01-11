#include <algorithm>
#include <iostream>
#include <fstream>
#include <future>
#include <list>
#include <string>
#include "curl/curl.h"
#include "tomlplusplus/toml.hpp"
using namespace std::string_view_literals;

int square(int x)
{
	return x*x;
}

static size_t my_write(void* buffer, size_t size, size_t nmemb, void* param)
{
	std::string& text = *static_cast<std::string*>(param);
	size_t totalsize = size * nmemb;
	text.append(static_cast<char*>(buffer), totalsize);
	return totalsize;
}

void readFile(const char* filename, std::list<std::string>& lines)
{
	lines.clear();
	std::ifstream file(filename);
	std::string s;
	while (getline(file, s))
	{
		lines.push_back(s);
	}
}

void curl_test(std::string uri)
{
	std::string result;
	CURL* curl;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (CURLE_OK != res) {
		std::cerr << "CURL error: " << res << '\n';
		}
	}
	curl_global_cleanup();
	std::cout << result << "\n\n";

}

void curl_test(toml::value<std::string> uri)
{
	std::string result;
	std::string uri_formated = uri->c_str();
	CURL* curl;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, uri_formated.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (CURLE_OK != res) {
		std::cerr << "CURL error: " << res << '\n';
		}
	}
	curl_global_cleanup();
	std::cout << result << "\n\n";

}
toml::parse_result get_toml_data()
{
    return toml::parse_file("./raw_lists.toml");
}

void iterateArray(const toml::array& arr) {
    for (const auto& item : arr) {
        // Assuming the array contains integers, change the type accordingly
        if (auto str_value = item.as_string(); str_value) {
			curl_test(*str_value);
            std::cout << *str_value << std::endl;
        } else {
            std::cerr << "Error: Array item is not an integer!" << std::endl;
        }
    }
}

int toml_items()
{
    toml::table tbl = get_toml_data();

    // get a toml::node_view of the element 'numbers' using operator[]
    auto numbers = tbl["Advertising"]["links"];

    const auto& yourArray = numbers.as_array();

    // Check if 'yourArray' is an array
    if (const auto array_ptr = yourArray; array_ptr) {
        iterateArray(*array_ptr);
    } else {
        std::cerr << "Error: 'your_array' is not an array!" << std::endl;
    }

    return 0;

}

int main (int argc, char *argv[]) {
	// Example on simple async function and waiting	
	std::future<int> asyncFunction = std::async(&square,12);
	for(int i=0; i< 10; i++){
		std::cout << square(i) << std::endl;
	}

	// We are blocked at the 'get()' operation, until our
	// result has computed
	int result_1 = asyncFunction.get();

	std::cout << "result is: " << result_1 << std::endl;

	// from where will be curl

	std::string test_url = "https://www.benmi.me/test.txt";
	curl_test(test_url);
    toml_items();
	return 0;
}

