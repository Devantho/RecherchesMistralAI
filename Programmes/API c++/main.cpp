#include <crow.h>// Crow micro-framework
#include <nlohmann/json.hpp>          // JSON parsing
#include <faiss/IndexFlat.h>          // FAISS index (L2 distance)
#include <curl/curl.h>                // libcurl pour HTTP
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>  // getenv

using json = nlohmann::json;
using Embedding = std::vector<float>;

// Callback pour libcurl
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Structure pour capturer code HTTP et corps de réponse
struct HttpResult {
    long status;
    std::string body;
};

// Fonction POST JSON retournant HttpResult
HttpResult postJsonWithStatus(const std::string& url,
    const json& body,
    const std::string& apiKey = "") {
    CURL* curl = curl_easy_init();
    if (!curl) throw std::runtime_error("Impossible d'initialiser CURL");
    HttpResult result{ 0, "" };

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    if (!apiKey.empty()) {
        headers = curl_slist_append(
            headers, ("Authorization: Bearer " + apiKey).c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    std::string payload = body.dump();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result.body);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        throw std::runtime_error(
            std::string("libcurl error: ") + curl_easy_strerror(res));
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &result.status);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return result;
}

// Ancienne fonction POST JSON simple (pour l'appel chat)
std::string postJson(const std::string& url, const json& body, const std::string& apiKey = "") {
    CURL* curl = curl_easy_init();
    std::string response;
    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        if (!apiKey.empty()) {
            headers = curl_slist_append(headers,
                ("Authorization: Bearer " + apiKey).c_str());
        }
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        std::string payload = body.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            throw std::runtime_error("Erreur libcurl: " + std::string(curl_easy_strerror(res)));
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    return response;
}

// Fonction pour obtenir l'embedding d'un texte
Embedding embedText(const std::string& text) {
    json req = {
        {"model", "mistral-embed"},
        {"input", text}
    };
    const std::string EMB_URL = "https://api.mistral.ai/v1/embeddings";
    const std::string EMB_KEY = "";

    // Appel HTTP avec code
    HttpResult http = postJsonWithStatus(EMB_URL, req, EMB_KEY);
    if (http.status != 200) {
        throw std::runtime_error(
            "embedText HTTP " + std::to_string(http.status)
            + ", body='" + http.body + "'");
    }

    // Parse JSON
    json j;
    try {
        j = json::parse(http.body);
    }
    catch (const json::parse_error& e) {
        throw std::runtime_error(
            std::string("embedText JSON parse error: ") + e.what()
            + ", body='" + http.body + "'");
    }

    // Vérification de data
    if (!j.contains("data") || !j["data"].is_array() || j["data"].empty()) {
        throw std::runtime_error(
            std::string("embedText réponse inattendue, pas de 'data', body='")
            + http.body + "'");
    }

    // Extraction
    try {
        return j["data"][0]["embedding"].get<std::vector<float>>();
    }
    catch (const json::type_error& e) {
        throw std::runtime_error(
            std::string("embedText JSON type error: ") + e.what()
            + ", body='" + http.body + "'");
    }
}

// Charge les chunks depuis un fichier
std::vector<std::string> loadChunks(const std::string& path) {
    std::vector<std::string> chunks;
    std::ifstream infile(path);
    if (!infile) {
        throw std::runtime_error("Impossible d'ouvrir " + path);
    }
    std::string line;
    while (std::getline(infile, line)) {
        if (!line.empty()) chunks.push_back(line);
    }
    return chunks;
}

// Middleware pour gérer le CORS
struct CORS {
    struct context {};
    void before_handle(crow::request& req, crow::response& res, context&) {
        res.add_header("Access-Control-Allow-Origin", "*");
        if (req.method == crow::HTTPMethod::Options) {
            res.add_header("Access-Control-Allow-Methods", "POST, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type");
            res.code = 204;
            res.end();
        }
    }
    void after_handle(crow::request&, crow::response&, context&) {}
};

int main() {
    // Chargement des morceaux de texte
    std::vector<std::string> chunks;
    try {
        chunks = loadChunks("chunks.txt");
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    // Création de l'index FAISS
    const int dim = 768;
    faiss::IndexFlatL2 index(dim);

    // Embedding initial des chunks
    try {
        for (auto& chunk : chunks) {
            Embedding vec = embedText(chunk);
            index.add(1, vec.data());
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Erreur d'initialisation (embedding des chunks) : "
            << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Démarrage du serveur HTTP
    crow::App<CORS> app;
    

    CROW_ROUTE(app, "/ask").methods(crow::HTTPMethod::Post)([&](const crow::request& req) {
        crow::response res;
        res.set_header("Content-Type", "application/json");

        // Vérification syntaxe JSON
        if (!json::accept(req.body)) {
            res.code = 400;
            res.write(R"({"error":"Corps non-JSON ou syntaxe invalide"})");
            return res;
        }

        // Parsing JSON
        json jreq = json::parse(req.body);

        // Récupération sécurisée de la question
        std::string question = jreq.value("question", "");
        if (question.empty()) {
            res.code = 400;
            res.write(R"({"error":"Le champ 'question' est manquant ou vide"})");
            return res;
        }

        // Traitement principal
        try {
            // Embed de la question
            Embedding qvec;
            qvec = embedText(question);

            // Recherche des k plus proches
            int k = 3;
            std::vector<faiss::idx_t> indices(k);
            std::vector<float> distances(k);
            index.search(1, qvec.data(), k, distances.data(), indices.data());

            // Construction du contexte
            std::ostringstream ctx;
            for (int i = 0; i < k; ++i) {
                ctx << "- " << chunks[indices[i]] << "\n";
            }
            std::string context = ctx.str();

            std::string systemPrompt = "Vous êtes un assistant intelligent qui répond aux questions en utilisant le contexte fourni. N'utilisez pas vos connaisances préalables. Donnez des réponses claires et concises. Vous êtes un assistant pour le logiciel Soins2000\n"
                "Contexte :\n" + context + "\n"
				"Répondez de manière concise et informative.";

            // Préparation du prompt chat
            json chatReq = {
                {"model", "ministral-8b-latest"},
                {"messages", {
                    {{"role", "system"}, {"content",
                        systemPrompt }},
                    {{"role", "user"},   {"content", question}}
                }}
            };

            // Appel chat completions
            const std::string CHAT_URL = "https://api.mistral.ai/v1/chat/completions";
            const std::string CHAT_KEY = "";
            std::string chatJson = postJson(CHAT_URL, chatReq, CHAT_KEY);
 
            std::string answer;
            try {
                auto jresp = json::parse(chatJson);
                answer = jresp["choices"][0]["message"]["content"].get<std::string>();
            }
            catch (...) {
                throw std::runtime_error("Impossible d'extraire le contenu de la réponse");
            }

            // On renvoie uniquement la réponse du modèle
            res.code = 200;
            res.write(
                json{ {"answer", answer} }.dump()
            );
            return res;

        }
        catch (const std::exception& e) {
            res.code = 500;
            res.write(
                std::string(R"({"error":"Erreur interne : )")
                + e.what() + R"("})"
            );
            return res;
        }
        });

    app.port(18080).multithreaded().run();
    return EXIT_SUCCESS;
}
