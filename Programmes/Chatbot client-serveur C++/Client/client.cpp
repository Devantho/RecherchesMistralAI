#include <iostream>
#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 55555
#define SERVER_IP "127.0.0.1"
#define CERT_FILE "cert.pem"

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

    // Création du contexte SSL
    const SSL_METHOD* method = TLS_client_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (!ctx) {
        std::cerr << "Erreur SSL_CTX_new" << std::endl;
        WSACleanup();
        return 1;
    }

    // Chargement du certificat du serveur pour vérification (optionnel)
    //if (!SSL_CTX_load_verify_locations(ctx, CERT_FILE, nullptr)) {
    //    std::cerr << "Attention: échec du chargement du certificat serveur pour vérification." << std::endl;
    //}

    // Création de la socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Erreur socket()" << std::endl;
        SSL_CTX_free(ctx);
        WSACleanup();
        return 1;
    }

    // Connexion au serveur
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Erreur connexion serveur" << std::endl;
        closesocket(sock);
        SSL_CTX_free(ctx);
        WSACleanup();
        return 1;
    }

    // Passage au mode SSL
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, (int)sock);
    if (SSL_connect(ssl) <= 0) {
        std::cerr << "Erreur SSL_connect" << std::endl;
        SSL_free(ssl);
        closesocket(sock);
        SSL_CTX_free(ctx);
        WSACleanup();
        return 1;
    }

    // Boucle de dialogue
    while (true) {
        std::cout << "Votre question (ou tapez 'exit' pour quitter) : ";
        std::string question;
        std::getline(std::cin, question);
        if (question.empty() || question == "exit" || question == "quit") {
            break;
        }

        // Envoi de la question
        if (SSL_write(ssl, question.c_str(), (int)question.size()) <= 0) {
            std::cerr << "Erreur d'envoi de la requête." << std::endl;
            break;
        }

        // Réception de la réponse
        char buffer[8192] = { 0 };
        int bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            std::cout << "Réponse : " << std::string(buffer, bytes) << std::endl;
        }
        else {
            std::cout << "Aucune réponse reçue ou connexion fermée par le serveur." << std::endl;
            break;
        }
    }

    // Fermeture propre de la connexion
    SSL_shutdown(ssl);
    SSL_free(ssl);
    closesocket(sock);
    SSL_CTX_free(ctx);
    EVP_cleanup();
    WSACleanup();

    std::cout << "Client fermé." << std::endl;
    return 0;
}
