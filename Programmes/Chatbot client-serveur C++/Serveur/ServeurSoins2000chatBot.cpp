#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include "chatbot_utils.hpp"

#define PORT 55555
#define CERT_FILE "cert.pem"
#define KEY_FILE  "key.pem"


void handle_client(SSL* ssl, const std::string& apiKey, const std::string& filePath) {
    // Log client connection
    SOCKET client_fd = SSL_get_fd(ssl);
    sockaddr_in peer_addr;
    int peer_len = sizeof(peer_addr);
    char ip_str[INET_ADDRSTRLEN] = { 0 };
    if (getpeername(client_fd, (sockaddr*)&peer_addr, &peer_len) == 0) {
        inet_ntop(AF_INET, &peer_addr.sin_addr, ip_str, sizeof(ip_str));
        std::cout << "Client connecté: " << ip_str << ":" << ntohs(peer_addr.sin_port) << std::endl;
    }

    // Loop to handle multiple requests on the same connection
    while (true) {
        char buffer[4096] = { 0 };
        int bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
        if (bytes <= 0) {
            std::cout << "Client déconnecté: " << ip_str << ":" << ntohs(peer_addr.sin_port) << std::endl;
            break;
        }
        std::string userInput(buffer, bytes);

        // -- Préparation du contexte RAG (inchangé) --
        std::string fileContent = readFileToString(filePath);
        std::string cleanedFileContent = cleanUTF8(fileContent);
        auto chunks = splitTextIntoChunks(cleanedFileContent, 500);
        std::vector<std::vector<float>> chunkEmbeddings;
        for (auto& chunk : chunks) {
            chunkEmbeddings.push_back(getEmbedding(apiKey, chunk));
        }
        auto questionEmbedding = getEmbedding(apiKey, userInput);
        auto relevantChunks = findMostRelevantChunks(chunks, chunkEmbeddings, questionEmbedding, 3);
        std::string context;
        for (auto& c : relevantChunks) context += c + "\n";

        std::string systemPrompt =
            "Répond simplement à la demande de l'utilisateur en utilisant le contexte, "
            "fait une réponse concise sans informations superflues; "
            "dans le cas ou tu ne peut pas répondre alors retourne le message suivant:\n"
            "Je ne peux pas répondre a votre question, merci de contacter l'assistance Soins2000.\n"
            "Adresse e-mail: maintenance@soins2000.com Numéro de téléphone: 04 68 42 78 90";
        std::string augmentedPrompt = userInput + " (Prompt Système: " + systemPrompt +
            " (Contexte: " + context + "))";

        auto modelResponse = chatWithModel(apiKey, augmentedPrompt);

        // Extraction du texte de la réponse et logging des tokens
        std::string answer;
        try {
            auto jsonResponse = nlohmann::json::parse(modelResponse);
            answer = jsonResponse["choices"][0]["message"]["content"];
            int p = jsonResponse["usage"]["prompt_tokens"];
            int c = jsonResponse["usage"]["completion_tokens"];
            int t = jsonResponse["usage"]["total_tokens"];
            std::cout << "Requête Mistral: prompt tokens=" << p
                << ", completion tokens=" << c
                << ", total tokens=" << t << std::endl;
        }
        catch (...) {
            answer = "Erreur lors de la génération de la réponse.";
        }

        // Envoi de la réponse
        SSL_write(ssl, answer.c_str(), (int)answer.size());
    }

    // Une seule fermeture SSL après toutes les requêtes
    SSL_shutdown(ssl);
    SSL_free(ssl);
}


int main() {
    // Passe la sortie console en UTF-8
    SetConsoleOutputCP(CP_UTF8);
    // (Optionnel) Passe aussi l’entrée console en UTF-8 si vous lisez des accents
    SetConsoleCP(CP_UTF8);
    // Initialisation Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Erreur WSAStartup" << std::endl;
        return 1;
    }
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    const SSL_METHOD* method = TLS_server_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (!ctx) {
        std::cerr << "Erreur SSL_CTX_new" << std::endl;
        WSACleanup();
        return 1;
    }
    if (SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) <= 0) {
        std::cerr << "Erreur chargement certificat/clé" << std::endl;
        SSL_CTX_free(ctx);
        WSACleanup();
        return 1;
    }
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Erreur socket()" << std::endl;
        SSL_CTX_free(ctx);
        WSACleanup();
        return 1;
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "Erreur bind()" << std::endl;
        closesocket(server_fd);
        SSL_CTX_free(ctx);
        WSACleanup();
        return 1;
    }
    if (listen(server_fd, 1) == SOCKET_ERROR) {
        std::cerr << "Erreur listen()" << std::endl;
        closesocket(server_fd);
        SSL_CTX_free(ctx);
        WSACleanup();
        return 1;
    }
    std::cout << "Serveur prêt sur le port " << PORT << std::endl;
    std::string apiKey = "fIjUMmTHGCIA0aagctLAMQWB7lj7hvZH";
    std::string filePath = "C:/Users/leona/source/repos/ChatBotMistralSoins2000/ChatBotMistralSoins2000/documentation.txt";
    while (true) {
        sockaddr_in client_addr;
        int len = sizeof(client_addr);
        SOCKET client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &len);
        if (client_fd == INVALID_SOCKET) continue;
        SSL* ssl = SSL_new(ctx);
        SSL_set_fd(ssl, (int)client_fd);
        if (SSL_accept(ssl) <= 0) {
            SSL_free(ssl);
            closesocket(client_fd);
            continue;
        }
        // Logging is handled in handle_client
        std::thread(handle_client, ssl, apiKey, filePath).detach();
    }
    closesocket(server_fd);
    SSL_CTX_free(ctx);
    EVP_cleanup();
    WSACleanup();
    return 0;
}
