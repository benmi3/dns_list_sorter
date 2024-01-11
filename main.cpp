#include <iostream>
#include <fstream>
#include <future>
#include <map>
#include <ostream>
#include <vector>
#include <string>
#include "curl/curl.h"
#include "tomlplusplus/toml.hpp"
using namespace std::string_view_literals;

int square(int x)
{
	return x*x;
}

void write_file(const char* filename, std::vector<std::string>& lines)
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

toml::parse_result get_toml_data()
{
    return toml::parse_file("./raw_lists.toml");
}

std::string toml_value_to_string(toml::value<std::string> uri)
{
	std::string thing_formated = uri->c_str();
	return thing_formated;
}

void iterate_array(const toml::array& arr, std::string category, std::map<std::string, std::vector<std::string>> list_map) {
    for (const auto& item : arr) {
        // Assuming the array contains integers, change the type accordingly
		if (auto str_value = item.as_string(); str_value) {
			std::string real_string = toml_value_to_string(*str_value);
            std::cout << real_string << std::endl;
			//curl_test(real_string);
			list_map[category].push_back(real_string);
        } else {
            std::cerr << "Error: Array item is not an integer!" << std::endl;
        }
    }
}

int toml_read_and_sort(toml::table tbl, std::string category, std::map<std::string, std::vector<std::string>> list_map) 
{
    // get a toml::node_view of the element '' using operator[]
    auto numbers = tbl[category]["links"];

    const auto& yourArray = numbers.as_array();

    // Check if 'yourArray' is an array
    if (const auto array_ptr = yourArray; array_ptr) {
        iterate_array(*array_ptr, category, list_map);

    } else {
        std::cerr << "Error: 'your_array' is not an array!" << std::endl;
    }
	return 0;
}

int main (int argc, char *argv[])
{
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
	//Small test
	std::map<std::string, std::vector<std::string>> list_map;
	// =================
	toml::table tbl = get_toml_data();

	std::string arr[5] = {"Suspicious", "Advertising", "Tracking", "Malicious", "Other"};
	std::vector<std::future<int>> get_lists;

	std::cout << "Start loop---------" << std::endl;

	for (const auto& item : arr) {
		std::cout << item << std::endl;
		get_lists.push_back(std::async(&toml_read_and_sort, tbl, item, list_map));
    }

	std::cout << "Waiting" << std::endl;
    
	for (auto& task : get_lists) {
        task.wait();
    }
	
	std::cout << "I waited, now its finished" << std::endl;
	
	for (const std::string& item : arr)
	{
		std::cout << item << std::endl;
		//std::cout << list_map["Suspicious"] << std::endl;
		for (const auto& item: list_map[item])
		{
			std::cout << item << std::endl;
		}
    }
	std::cout << "---start" << std::endl;
	for(std::map<std::string, std::vector<std::string>> ::const_iterator it = list_map.begin();  it != list_map.end(); ++it)
	{
		std::cout << it->first << ": ";
		for (std::vector<std::string>::const_iterator b = it->second.begin(); b != it->second.end() ;++it)
		{
			std::cout << *b;
		}
		std::cout << std::endl;
	}
	std::cout << "----end" << std::endl;
	for(std::map<std::string, std::vector<std::string>> ::const_iterator it = list_map.begin();  it != list_map.end(); ++it)
	{
		std::cout << it->first << ": ";
		for (std::vector<std::string>::const_iterator b = it->second.begin(); b != it->second.end() ;++it)
		{
			std::cout << *b;
		}
		std::cout << std::endl;
	}
	return 0;
}

