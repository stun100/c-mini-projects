#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

enum class RequestMethod{ GET, POST, PUT, DELETE, UNKNOWN};

struct Url {
    std::string host;
    std::string path;
    int portno;
};

void Print(std::string text) {
    std::cout << text << std::endl;
}

RequestMethod hashString(const std::string& str) {
    if (str == "GET") return RequestMethod::GET;
    if (str == "POST")  return RequestMethod::POST;
    if (str == "PUT")  return RequestMethod::PUT;
    if (str == "DELETE")  return RequestMethod::DELETE;
    return RequestMethod::UNKNOWN;
}

std::string ParseResponse(std::string raw_response){
    size_t headerEnd = raw_response.find("\r\n\r\n");
    if (headerEnd != std::string::npos) {
        return raw_response.substr(headerEnd + 4); 
    }
    return raw_response;
}

Url ParseUrl(std::string url){
    std::string host, path;
    int port = 80;

    size_t start = url.find("://");
    if (start != std::string::npos) {
        // skip :// which has length 3
        url = url.substr(start + 3);
    }

    size_t pos = url.find("/");

    if (pos == std::string::npos) {
        size_t pos_2 = url.find(":");
        host = url;
        path = "";
    } else {
        host = url.substr(0, pos);
        path = url.substr(pos + 1);
    }

    size_t hostend = host.find(":");
    if (hostend != std::string::npos){
        port = std::stoi(host.substr(hostend + 1));
        host = host.substr(0, hostend);
    }

    return {host, path, port};
}

int main(int argc, char* argv[]) {
    int sockfd, opt;
    char buffer[4096];
    int bytesReceived;
    
    std::string data;
    std::string header;
    std::string request_method;

    std::string url;
    std::string request;
    std::string response;
    std::string body;
    
    while((opt = getopt(argc, argv, "X:d:H:")) != -1){
        switch (opt){
            case 'X':
                request_method = optarg;
                break;
            case 'd':
                data = optarg;
                break;
            case 'H':
                header = optarg;
                break;
            default:
                Print("httpal: Try 'httpal --help' for more information");
                return 1;
        }
    }

    if (optind < argc) {
        url = argv[optind];
    } else {
        Print("httpal: Missing url");
        return -1;
    }

    Url u = ParseUrl(url);

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(u.host.c_str(), std::to_string(u.portno).c_str(), &hints, &res) != 0) {
        Print("httpal: Could not resolve host: " + u.host);
        return -1;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0){
        Print("httpal: Socket connection failed");
        return -1;
    }
    freeaddrinfo(res);
    
    if (request_method.length() != 0){
        switch (hashString(request_method)){
            case RequestMethod::GET:
                request =  "GET /" + u.path + " HTTP/1.1\r\n" + "Host: " + u.host + "\r\n" + "Connection: close\r\n\r\n";
                break;
            case RequestMethod::POST:
                request =  "POST /" + u.path + " HTTP/1.1\r\n" + "Host: " + u.host + "\r\n" + header + "\r\n" + "Connection: close\r\n" + "Content-Length: " +std::to_string(data.length()) + "\r\n\r\n" +data;
                break;
            case RequestMethod::PUT:
                request =  "PUT /" + u.path + " HTTP/1.1\r\n" + "Host: " + u.host + "\r\n" + header + "\r\n" + "Connection: close\r\n" + "Content-Length: " +std::to_string(data.length()) + "\r\n\r\n" +data;
                break;
            case RequestMethod::DELETE:
                request =  "DELETE /" + u.path + " HTTP/1.1\r\n" + "Host: " + u.host + "\r\n" + header + "\r\n" + "Connection: close\r\n\r\n";
                break;  
            case RequestMethod::UNKNOWN:
                Print("httpal: Wrong request method.");
                return -1;  
        }
        
    } else {
        request = "GET /" + u.path + " HTTP/1.1\r\n" + "Host: " + u.host + "\r\n" + "Connection: close\r\n\r\n";
    }
    // Print(request);
    send(sockfd, request.c_str(), request.size(), 0);

    while ((bytesReceived = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) != 0){
        buffer[bytesReceived] = '\0';
        response += buffer;
    }

    close(sockfd);

    body = ParseResponse(response);

    Print(body);

    return 0;
}