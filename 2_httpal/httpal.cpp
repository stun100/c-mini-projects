#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <string>

#include <print>
#include <array>
#include <cstdint>

constexpr std::size_t BUFFER_SIZE{4096};

enum class RequestMethod
{ 
    GET, 
    POST, 
    PUT, 
    DELETE, 
    UNKNOWN
};

class URL 
{
private:
    std::string host;
    std::string path;
    int portno;
    friend class Request;
public:
    URL(std::string& url)
    {
        portno = 443;

        size_t start = url.find("://");
        if (start != std::string::npos)
            // skip :// which has length 3
            url = url.substr(start + 3);

        size_t pos = url.find("/");

        if (pos == std::string::npos) 
        {
            size_t pos_2 = url.find(":");
            host = url;
            path = "";
        } 
        else 
        {
            host = url.substr(0, pos);
            path = url.substr(pos + 1);
        }

        size_t hostend = host.find(":");
        if (hostend != std::string::npos)
        {
            portno = std::stoi(host.substr(hostend + 1));
            host = host.substr(0, hostend);
        }
    }
};

RequestMethod hashString(const std::string& str) 
{
    if (str == "GET") return RequestMethod::GET;
    if (str == "POST")  return RequestMethod::POST;
    if (str == "PUT")  return RequestMethod::PUT;
    if (str == "DELETE")  return RequestMethod::DELETE;
    return RequestMethod::UNKNOWN;
}

class Request
{
private:
    struct addrinfo hints;
    URL url;
    int sockfd;
    std::string request;
    std::string response;
public:
    Request(
        std::string& url, 
        std::string& request_method,
        std::string& data,
        std::string& header
    ) 
    : url{url}, hints{}, request{}, response{}
    {
        struct addrinfo* res;

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        std::println("Resolving host='{}' port='{}'", this->url.host, this->url.portno);

        // Resolve the url
        if (getaddrinfo(
            this->url.host.c_str(), 
            std::to_string(this->url.portno).c_str(), 
            &hints, 
            &res) != 0
        ) 
        {
            throw std::runtime_error("Could not resolve host: " + this->url.host);
        }

        std::println("[httpal::Request] url resolved");

        // Create the socket
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0)
            throw std::runtime_error("httpal: Socket connection failed");

        std::println("[httpal::Request] connection created");

        freeaddrinfo(res);

        // Setup the request
        if (request_method.length() != 0)
        {
            switch (hashString(request_method)){
                case RequestMethod::GET:
                    request =   "GET /" + this->url.path  + " HTTP/1.1\r\n" 
                                + "Host: " + this->url.host + "\r\n" 
                                + "Connection: close\r\n"
                                + "\r\n";
                    break;
                case RequestMethod::POST:
                    request =   "POST /" + this->url.path + " HTTP/1.1\r\n" 
                                + "Host: " + this->url.host + "\r\n" 
                                + header 
                                + "\r\n" 
                                + "Connection: close\r\n" 
                                + "Content-Length: " + std::to_string(data.length()) + "\r\n" 
                                + "\r\n" 
                                + data;
                    break;
                case RequestMethod::PUT:
                    request =   "PUT /" + this->url.path + " HTTP/1.1\r\n" 
                                + "Host: " + this->url.host + "\r\n" 
                                + header  
                                + "Connection: close\r\n" 
                                + "Content-Length: " + std::to_string(data.length()) + "\r\n" 
                                + "\r\n" 
                                + data;
                    break;
                case RequestMethod::DELETE:
                    request =   "DELETE /" + this->url.path + " HTTP/1.1\r\n" 
                                + "Host: " + this->url.host + "\r\n" 
                                + header 
                                + "\r\n" 
                                + "Connection: close\r\n";
                    break;  
                case RequestMethod::UNKNOWN:
                    throw std::runtime_error("httpal: Wrong request method.");
            }
        } 
        else 
        {
            request =   "GET /" + this->url.path + " HTTP/1.1\r\n" 
                        + "Host: " + this->url.host + "\r\n" 
                        + "Connection: close\r\n\r\n";
        }

        std::println("[httpal::Request] request created");
    }

    void send()
    {
        ::send(sockfd, request.c_str(), request.size(), 0);

        std::println("[httpal::Request::send] send done");

        std::array<char, BUFFER_SIZE> buffer;
        int bytesReceived;

        while ((bytesReceived = recv(sockfd, buffer.data(), buffer.size() - 1, 0)) != 0)
        {
            buffer.at(bytesReceived) = '\0';
            response.append(buffer.data(), bytesReceived);
        }

        std::println("[httpal::Request::send] response collected");
    }

    std::string parseResponse()
    {
        std::size_t headerEnd = response.find("\r\n\r\n");
        if (headerEnd != std::string::npos)
            return response.substr(headerEnd + 4); 

        return response;
    }

    ~Request()
    {
        close(sockfd);
    }
};

int main(int argc, char* argv[]) 
{
    int opt;
    
    std::string data;
    std::string header;
    std::string requestMethod;

    std::string url;
    std::string request;
    std::string response;
    
    while((opt = getopt(argc, argv, "X:d:H:")) != -1)
    {
        switch (opt)
        {
            case 'X':
                requestMethod = optarg;
                break;
            case 'd':
                data = optarg;
                break;
            case 'H':
                header = optarg;
                break;
            default:
                std::println("httpal: Try 'httpal --help' for more information");
                return 1;
        }
    }

    if (optind < argc) 
    {
        url = argv[optind];
    } 
    else 
    {
        std::println("httpal: Missing url");
        return -1;
    }

    Request req 
    {
        url, 
        requestMethod, 
        data, 
        header
    };
    
    // std::println(request);
    req.send();

    std::string body = req.parseResponse();

    std::println("{}", body);

    return 0;
}