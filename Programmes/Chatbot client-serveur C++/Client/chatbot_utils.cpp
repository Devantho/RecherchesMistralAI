#define NOMINMAX
#include "chatbot_utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#undef min

using json = nlohmann::json;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

std::string readFileToString(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string cleanUTF8(const std::string& str) {
    std::string cleanedStr;
    for (size_t i = 0; i < str.size(); ++i) {
        unsigned char c = str[i];
        if ((c & 0b10000000) == 0) {
            cleanedStr += c;
        }
        else {
            if ((c & 0b11000000) == 0b10000000) {
                continue;
            }
            size_t length = 0;
            if ((c & 0b11100000) == 0b11000000) length = 2;
            else if ((c & 0b11110000) == 0b11100000) length = 3;
            else if ((c & 0b11111000) == 0b11110000) length = 4;
            else continue;

            if (i + length > str.size()) continue;

            bool valid = true;
            for (size_t j = 1; j < length; ++j) {
                if ((str[i + j] & 0b11000000) != 0b10000000) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                for (size_t j = 0; j < length; ++j) {
                    cleanedStr += str[i + j];
                }
                i += length - 1;
            }
        }
    }
    return cleanedStr;
}

std::vector<std::string> splitTextIntoChunks(const std::string& text, size_t chunkSize) {
    std::vector<std::string> chunks;
    size_t pos = 0;
    while (pos < text.size()) {
        size_t len = (std::min)(chunkSize, text.size() - pos);
        chunks.push_back(text.substr(pos, len));
        pos += len;
    }
    return chunks;
}

std::vector<float> getEmbedding(const std::string& apiKey, const std::string& text) {
    CURL* curl;
    CURLcode res;
    std::string response;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    std::vector<float> embedding;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.mistral.ai/v1/embeddings");
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        json postData = {
            {"model", "mistral-embed"},
            {"input", text}
        };
        std::string post_data = postData.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            try {
                auto jsonResponse = json::parse(response);
                embedding = jsonResponse["data"][0]["embedding"].get<std::vector<float>>();
            } catch (...) {}
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return embedding;
}

float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
    float dot = 0.0, normA = 0.0, normB = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    return dot / (sqrt(normA) * sqrt(normB) + 1e-8);
}

std::vector<std::string> findMostRelevantChunks(
    const std::vector<std::string>& chunks,
    const std::vector<std::vector<float>>& chunkEmbeddings,
    const std::vector<float>& questionEmbedding,
    size_t topK
) {
    std::vector<std::pair<float, size_t>> scores;
    for (size_t i = 0; i < chunkEmbeddings.size(); ++i) {
        float sim = cosineSimilarity(chunkEmbeddings[i], questionEmbedding);
        scores.push_back({sim, i});
    }
    std::sort(scores.rbegin(), scores.rend());
    std::vector<std::string> topChunks;
    for (size_t i = 0; i < (std::min)(topK, scores.size()); ++i) {
        topChunks.push_back(chunks[scores[i].second]);
    }
    return topChunks;
}

std::string chatWithModel(const std::string& apiKey, const std::string& prompt) {
    CURL* curl;
    CURLcode res;
    std::string response;

    std::string cleanedPrompt = cleanUTF8(prompt);
    if (cleanedPrompt.empty()) {
        return "";
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.mistral.ai/v1/chat/completions");

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        json postData = {
            {"model", "ministral-8b-latest"},
            {"messages", json::array({{
                {"role", "user"},
                {"content", cleanedPrompt}
            }})}
        };

        std::string post_data = postData.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        res = curl_easy_perform(curl);

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return response;
}

void printModelResponse(const std::string& response) {
    try {
        auto jsonResponse = json::parse(response);
        std::string content = jsonResponse["choices"][0]["message"]["content"];
        std::cout << "Model: " << content << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }
}
