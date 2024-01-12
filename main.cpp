#include <iostream>
#include <fstream>
#include <future>
#include <iterator>
#include <ostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "curl/curl.h"
#include "tomlplusplus/toml.hpp"
// ----------------------------------------- Globals
std::vector<std::string> white_list;
std::vector<std::string> full_list;
// ----------------------------------------- Globals

std::vector<std::string> combine_lists(std::vector<std::string> v, std::vector<std::string> v_append)
{
	v.reserve(v.size() + distance(v_append.begin(),v_append.end()));
	v.insert(v.end(), v_append.begin(),v_append.end());
	return v;
}

int write_vector_to_file(const std::vector<std::string>& data, const std::string& filename) {
    std::ofstream outputFile(filename);

    if (outputFile.is_open()) {
        for (const auto& line : data) {
            outputFile << line << std::endl;
        }

        outputFile.close();
        std::cout << "Vector contents successfully written to " << filename << std::endl;
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
	return 0;
}


static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string curl_test(std::string uri)
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

		//std::cout << readBuffer << std::endl;
	}
	return readBuffer;
}

std::string toml_value_to_string(toml::value<std::string> uri)
{
	std::string thing_formated = uri->c_str();
	return thing_formated;
}

std::string clean_up_string(std::string input)
{
	std::string check_array[2] = {"127.0.0.1", "0.0.0.0"};
	for (std::string unneeded_string : check_array) {
		if (input.compare(0, unneeded_string.length(), unneeded_string) == 0) {
			// Find the position of the first non-whitespace character after "unneeded_string"
			size_t startPos = input.find_first_not_of(" \t", unneeded_string.length());

			// Remove the "unneeded_string" part and any spaces if found
			if (startPos != std::string::npos) {
				input = input.substr(startPos);
			} else {
				input = input.substr(unneeded_string.length()); // If no non-whitespace character found after "unneeded_string", set the string to empty
			}
		} 
	}

	char illegalChar[2] = {'^', '|'};

	for (char bad_char: illegalChar) {
		input.erase(std::remove(input.begin(), input.end(), bad_char), input.end());
	}

	//std::cout << input << std::endl;
	return input;
}

bool check_white_list(std::string item)
{
	// The only problem here is that if the link is
	// f.eks www.google.com,
	// and the white_filter has google.com in it, 
	// it will return false
	//! This might be thing to add later
	if ( std::find(white_list.begin(), white_list.end(), item) != white_list.end() ) {
		// if the item is in the whitelist
		if ( std::find(full_list.begin(), full_list.end(), item) != full_list.end() ){
			return true;
		}
		return true;
	}
	// else return false
	return false;
}

std::vector<std::string> split_string_to_list(const std::string& str)
{
    auto result = std::vector<std::string>{};
    auto ss = std::stringstream{str};

    for (std::string line; std::getline(ss, line, '\n');){
		// Skip the string if it starts with "#"
		// As there is no need for comments
		if (line.rfind("#", 0) != 0) {

			if (line.rfind("!", 0) != 0) {
				std::string clean_uri = clean_up_string(line);
				// if the item is not in the list_filter
				if (!check_white_list(clean_uri)) {
					// add the item
					result.push_back(clean_uri);
				}

			}
		}
		//std::cout << line << std::endl;
	}
    return result;
}

 std::vector<std::string> iterate_array(const toml::array& arr, std::string category) {
	std::vector<std::string> v;
	for (const auto& item : arr)
	{
		// Assuming the array contains integers, change the type accordingly
		if (auto str_value = item.as_string(); str_value) {
			std::string real_string = toml_value_to_string(*str_value);
			std::string curl_result = curl_test(real_string);
			std::vector<std::string> v_append = split_string_to_list(curl_result);
		
			v = combine_lists(v, v_append);
		} else {
			std::cerr << "Error: Array item is not an integer!" << std::endl;
		}
	}
	return v;
}

std::vector<std::string> remove_duplicates(std::vector<std::string> v)
{
	//std::set<std::string> string_set;
	//unsigned size = v.size();
	//for( unsigned i = 0; i < size; ++i ) string_set.insert( v[i] );
	//v.assign( string_set.begin(), string_set.end() );
	//
	sort( v.begin(), v.end() );
	v.erase( unique( v.begin(), v.end() ), v.end() );

	return v;
}

std::vector<std::string> toml_read_and_sort(toml::table tbl, std::string category) 
{
	std::vector<std::string> v; // full vector
	// ---------------------------------------------------------------------
    // get a toml::node_view of the element 'category' using operator[]
    auto category_links = tbl[category]["links"];

    const auto& category_links_array = category_links.as_array();

    // Check if 'yourArray' is an array
    if (const auto array_item = category_links_array; array_item) {
        std::vector<std::string> v_append = iterate_array(*array_item, category);
		v = combine_lists(v, v_append);
    } else {
        std::cerr << "Error: 'your_array' is not an array!" << std::endl;
    }
	std::vector<std::string> sorted_v = remove_duplicates(v);
	return sorted_v;
}

toml::parse_result get_toml_data()
{
    return toml::parse_file("raw_lists.toml");
}

std::vector<std::string> update_white_list(std::vector<std::string> v_append)
{
	white_list = combine_lists(white_list, v_append);
	return white_list;
}

int main (int argc, char *argv[])
{
	
	// Start with geting the raw data from toml
	toml::table tbl = get_toml_data();

	std::future<std::vector<std::string>> get_whitelist = std::async(&toml_read_and_sort, tbl, "Whitelist");
	std::vector<std::string> result_white = get_whitelist.get();
	white_list = update_white_list(result_white);
	std::cout << "Start gathering---------" << std::endl;

	// Go through each main cateory
	// collecting the data from the links
	std::future<std::vector<std::string>> get_sus = std::async(&toml_read_and_sort, tbl, "Suspicious");
	std::future<std::vector<std::string>> get_ads = std::async(&toml_read_and_sort, tbl, "Advertising");
	std::future<std::vector<std::string>> get_trc = std::async(&toml_read_and_sort, tbl, "Tracking");
	std::future<std::vector<std::string>> get_mal = std::async(&toml_read_and_sort, tbl, "Malicious");
	std::future<std::vector<std::string>> get_etc = std::async(&toml_read_and_sort, tbl, "Other");

	// Wait for the result
	std::cout << "Waiting" << std::endl;
	std::vector<std::string> result_sus = get_sus.get();
	std::vector<std::string> result_ads = get_ads.get();
	std::vector<std::string> result_trc = get_trc.get();
	std::vector<std::string> result_mal = get_mal.get();
	std::vector<std::string> result_etc = get_etc.get();

	std::cout << "I waited, now its finished" << std::endl;

	full_list = combine_lists(full_list, result_sus);
	full_list = combine_lists(full_list, result_ads);
	full_list = combine_lists(full_list, result_trc);
	full_list = combine_lists(full_list, result_mal);
	full_list = combine_lists(full_list, result_etc);

	full_list = remove_duplicates(full_list);

	/*
	for (auto i = full_list.begin(); i != full_list.end(); ++i) {
		std::cout << "Iteration: " << *i << std::endl;
	}
	*/

	// Print it to .txt files
	//writeVectorToFile(myVector, "output.txt");
	//std::async(&write_vector_to_file, tbl, "Whitelist.txt");
	std::future<int> wr_wht = std::async(&write_vector_to_file, white_list, "whitelist.txt");
	std::future<int> wr_sus = std::async(&write_vector_to_file, result_sus, "suspicious.txt");
	std::future<int> wr_ads = std::async(&write_vector_to_file, result_ads, "advertising.txt");
	std::future<int> wr_trc = std::async(&write_vector_to_file, result_trc, "tracking.txt");
	std::future<int> wr_mal = std::async(&write_vector_to_file, result_mal, "malicious.txt");
	std::future<int> wr_etc = std::async(&write_vector_to_file, result_etc, "other.txt");
	std::future<int> wr_full = std::async(&write_vector_to_file, full_list, "fulllist.txt");

	int res_w = wr_wht.get();
	int res_s = wr_sus.get();
	int res_a = wr_ads.get();
	int res_t = wr_trc.get();
	int res_m = wr_mal.get();
	int res_e = wr_etc.get();
	int res_f = wr_full.get();

	std::cout << "I finished writing all the files!" << std::endl;
	
	return 0;
}

