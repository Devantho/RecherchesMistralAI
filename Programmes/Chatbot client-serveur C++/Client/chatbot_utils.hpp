#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <curl/curl.h>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
std::string readFileToString(const std::string& filePath);
std::string cleanUTF8(const std::string& str);
std::vector<std::string> splitTextIntoChunks(const std::string& text, size_t chunkSize = 500);
std::vector<float> getEmbedding(const std::string& apiKey, const std::string& text);
float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b);
std::vector<std::string> findMostRelevantChunks(
    const std::vector<std::string>& chunks,
    const std::vector<std::vector<float>>& chunkEmbeddings,
    const std::vector<float>& questionEmbedding,
    size_t topK = 3
);
std::string chatWithModel(const std::string& apiKey, const std::string& prompt);
void printModelResponse(const std::string& response);
