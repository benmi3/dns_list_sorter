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

void write_file(const char* filename, std::list<std::string>& lines)
{
	lines.clear();
	std::ifstream file(filename);
	std::string s;
	while (getline(file, s))
	{
		lines.push_back(s);
	}
}

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void curl_test(std::string uri)
{
	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		std::cout << readBuffer << std::endl;
	}
}

void curl_test(toml::value<std::string> uri)
{
	CURL *curl;
	CURLcode res;
	std::string readBuffer;
	std::string uri_formated = uri->c_str();
	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, uri_formated.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		std::cout << readBuffer << std::endl;
	}
}

toml::parse_result get_toml_data()
{
    return toml::parse_file("./raw_lists.toml");
}

void iterate_array(const toml::array& arr) {
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
        iterate_array(*array_ptr);
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
	// toml paring and looping
    //toml_items();
	// ------------------- Restructure
	
	return 0;
}

