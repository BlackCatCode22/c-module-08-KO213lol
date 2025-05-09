//
// Created by dm014 on 11/20/2024.
// testBot01.cpp
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>

int test_main() {
    // Test libcurl
    curl_version_info_data* curl_info = curl_version_info(CURLVERSION_NOW);
    std::cout << "libcurl version: " << curl_info->version << std::endl;

    // Test nlohmann/json
    nlohmann::json json_obj = {{"key", "value"}, {"number", 42}};
    std::cout << "JSON: " << json_obj.dump() << std::endl;

    return 0;
}
