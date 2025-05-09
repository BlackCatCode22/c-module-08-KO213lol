// Enhanced Chatbot - Final Assignment
// Implements conversation history, error handling, retries, and timing

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <utility>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* out) {
    size_t totalSize = size * nmemb;
    out->append((char*)contents, totalSize);
    return totalSize;
}

string getTimeInItaly() {
    string responseString;
    CURL* curl = curl_easy_init();

    if (curl) {
        string url = "https://worldtimeapi.org/api/timezone/Europe/Rome";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
        curl_easy_setopt(curl, CURLOPT_CAINFO, "c:\\Users\\ko165\\Desktop\\Cpp class Projects\\Module-8\\chatBot01\\cacert.pem");

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) cerr << "cURL error: " << curl_easy_strerror(res) << endl;
        curl_easy_cleanup(curl);
    }

    if (responseString.empty()) return "Error: Could not fetch time.";

    try {
        json jsonResponse = json::parse(responseString);
        return jsonResponse.value("datetime", "Unknown time format");
    } catch (...) {
        return "Error: Failed to parse time response.";
    }
}

string sendMessageToChatbotRecursive(const string& userMessage, const string& apiKey, int retries = 3) {
    if (retries <= 0) return "Error: Failed after multiple retries.";

    string responseString;
    CURL* curl = curl_easy_init();

    if (curl) {
        string url = "https://api.openai.com/v1/chat/completions";
        string payload = R"({"model": "gpt-3.5-turbo", "messages": [{"role": "user", "content": ")" + userMessage + R"("}]})";

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
        curl_easy_setopt(curl, CURLOPT_CAINFO, "c:\\Users\\ko165\\Desktop\\Cpp class Projects\\Module-8\\chatBot01\\cacert.pem");

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK || responseString.empty()) {
            cerr << "API call failed. Retrying...\n";

            
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return sendMessageToChatbotRecursive(userMessage, apiKey, retries - 1);
        }
    }

    return responseString;
}

int main() {
    const char* apiKey = getenv("OPENAI_API_KEY");
    if (!apiKey) {
        cerr << "Missing API key. Set the OPENAI_API_KEY environment variable." << endl;
        return 1;
    }

    string userMessage;
    string chatbotName = "Assistant", userName = "User";
    int exchangeCount = 0;
    double totalResponseTime = 0.0;
    vector<pair<string, string>> conversationHistory;

    cout << "Chatbot (type 'exit' to quit):\n";

    while (true) {
        cout << "> ";
        getline(cin, userMessage);

        // Input validation
        if (userMessage.empty()) {
            cout << "Input cannot be empty. Try again.\n";
            continue;
        }
        if (userMessage.length() > 200) {
            cout << "Input too long. Max 200 characters.\n";
            continue;
        }

        if (userMessage == "exit") break;

        if (userMessage.find("Your name is now") != string::npos) {
            chatbotName = userMessage.substr(17);
            chatbotName.erase(chatbotName.find_last_not_of(" \n\r\t") + 1);
            cout << "Bot: Okay, I will now call myself " << chatbotName << ".\n";
            continue;
        }
        if (userMessage.find("my name is") != string::npos) {
            userName = userMessage.substr(10);
            userName.erase(userName.find_last_not_of(" \n\r\t") + 1);
            cout << chatbotName << ": Nice to meet you, " << userName << "!\n";
            continue;
        }
        if (userMessage.find("time in Italy") != string::npos) {
            cout << chatbotName << ": The current time in Italy is: " << getTimeInItaly() << "\n";
            continue;
        }

        auto start = chrono::high_resolution_clock::now();
        string response = sendMessageToChatbotRecursive(userMessage, apiKey);
        auto end = chrono::high_resolution_clock::now();

        double durationMs = chrono::duration<double, milli>(end - start).count();
        totalResponseTime += durationMs;
        exchangeCount++;

        try {
            json jsonResponse = json::parse(response);
            string botReply = jsonResponse["choices"][0]["message"]["content"];
            cout << chatbotName << ": " << botReply << "\n";

            conversationHistory.emplace_back(userMessage, botReply);
            for (const auto& [user, bot] : conversationHistory) {
                cout << userName << ": " << user << "\n";
                cout << chatbotName << ": " << bot << "\n";
            }
        } catch (...) {
            cout << chatbotName << ": Sorry, I couldn't understand the server response.\n";
        }

        exchangeCount++;
        cout << "Total exchanges: " << exchangeCount << endl;
        cout << "Average response time: " << (totalResponseTime / exchangeCount) << " ms\n";
    }

    return 0;
}
