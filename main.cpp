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
	return 0;
}


toml::parse_result get_toml_data()
{
    return toml::parse_file("raw_lists.toml");
}

int toml_items()
{
    static constexpr auto source = R"(
        str = "hello world"

        numbers = [ 1, 2, 3, "four", 5.0 ]
        vegetables = [ "tomato", "onion", "mushroom", "lettuce" ]
        minerals = [ "quartz", "iron", "copper", "diamond" ]

        [animals]
        cats = [ "tiger", "lion", "puma" ]
        birds = [ "macaw", "pigeon", "canary" ]
        fish = [ "salmon", "trout", "carp" ]
    )"sv;
    toml::table tbl = toml::parse(source);

    toml::table tbl = get_toml_data();

    // different ways of directly querying data
    std::optional<std::string_view> str1 = tbl["str"].value<std::string_view>();
    std::optional<std::string>      str2 = tbl["str"].value<std::string>();
    std::string_view                str3 = tbl["str"].value_or(""sv);
    std::string&                    str4 = tbl["str"].ref<std::string>(); // ~~dangerous~~

    std::cout << *str1 << "\n";
    std::cout << *str2 << "\n";
    std::cout << str3 << "\n";
    std::cout << str4 << "\n";

    // get a toml::node_view of the element 'numbers' using operator[]
    auto numbers = tbl["numbers"];
    std::cout << "table has 'numbers': " << !!numbers << "\n";
    std::cout << "numbers is an: " << numbers.type() << "\n";
    std::cout << "numbers: " << numbers << "\n";

    // get the underlying array object to do some more advanced stuff
    if (toml::array* arr = numbers.as_array())
    {
        // visitation with for_each() helps deal with heterogeneous data
        arr->for_each([](auto&& el)
        {
            if constexpr (toml::is_number<decltype(el)>)
                (*el)++;
            else if constexpr (toml::is_string<decltype(el)>)
                el = "five"sv;
        });

        // arrays are very similar to std::vector
        arr->push_back(7);
        arr->emplace_back<toml::array>(8, 9);
        std::cout << "numbers: " << numbers << "\n";
    }

    // node-views can be chained to quickly query deeper
    std::cout << "cats: " << tbl["animals"]["cats"] << "\n";
    std::cout << "fish[1]: " << tbl["animals"]["fish"][1] << "\n";

    // can also be retrieved via absolute path
    std::cout << "cats: " << tbl.at_path("animals.cats") << "\n";
    std::cout << "fish[1]: " << tbl.at_path("animals.fish[1]") << "\n";

    // ...even if the element doesn't exist
    std::cout << "dinosaurs: " << tbl["animals"]["dinosaurs"] << "\n"; //no dinosaurs :(

    return 0;

}
