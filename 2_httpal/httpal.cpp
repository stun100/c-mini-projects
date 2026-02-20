#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

void Print(std::string text) {
    std::cout << text << std::endl;
}

int main(int argc, char* argv[]) {
    int sockfd, portno, n;
    portno = 80;

    if (argc != 2) {
        Print("httpal: try 'httpal --help' for more information");
        return 0;
    }

    std::string url = argv[1];
    std::string host, path;

    size_t start = url.find("://");
    if (start != std::string::npos) {
        // skip :// which has length 3
        url = url.substr(start + 3);
    }

    size_t pos = url.find("/");

    if (pos == std::string::npos) {
        host = url;
        path = "";
    } else {
        host = url.substr(0, pos);
        path = url.substr(pos + 1);
    }

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host.c_str(), std::to_string(portno).c_str(), &hints, &res) != 0) {
        Print("httpal: Could not resolve host: " + host);
        return -1;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0){
        Print("httpal: Socket connection failed");
        return -1;
    }
    freeaddrinfo(res);

    std::string request = "GET /" + path + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";

    send(sockfd, request.c_str(), request.size(), 0);

    std::string response;
    std::string body;
    char buffer[4096];
    int bytesReceived;

    while ((bytesReceived = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) != 0){
        buffer[bytesReceived] = '\0';
        response += buffer;
    }

    close(sockfd);

    size_t headerEnd = response.find("\r\n\r\n");
    if (headerEnd != std::string::npos) {
        body = response.substr(headerEnd + 4); 
    }

    Print(body);

    return 0;
}