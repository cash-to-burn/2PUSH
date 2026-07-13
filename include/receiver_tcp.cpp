#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <filesystem>
#include "H_file.h"
#include "rc4.h"
#define BUFFER_SIZE 4096
// #define ENABLE_MAIN

struct RTCPdata
{
    int sockFD, newSock;
    uint16_t port = 6767;
    sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    int socklen = sizeof(serv_addr);
    int opt = 1;
} R_TCP;

RC4_State receiver;


std::filesystem::path StripThePath (const std::filesystem::path &path) {
    std::filesystem::path res;
    for (const auto i : path) {
        if (i != ".." && i != ".") {
            res /= i;
        }
    }
    return res;
}


void receiver_tcp_init(uint16_t port) {
    if ((R_TCP.sockFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "[E] ERROR in socker()\n";
        return;
        
    }
    
    R_TCP.serv_addr.sin_family = AF_INET;
    R_TCP.serv_addr.sin_port = htons(port);
    R_TCP.serv_addr.sin_addr.s_addr = INADDR_ANY;

    if ((setsockopt(R_TCP.sockFD, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR, &R_TCP.opt, sizeof(R_TCP.opt))) == -1) {
        std::cerr << "[E] ERROR in setsockopt()\n";
        return;
    }

    if ((bind(R_TCP.sockFD, (sockaddr*)&R_TCP.serv_addr, sizeof(R_TCP.serv_addr))) == -1) {
        std::cerr << "[E] ERROR in bind()\n";
        return;
    }


    if((listen(R_TCP.sockFD, SOMAXCONN)) < 0) {
        // std::cerr << "[E] ERROR in connect()\n";
        return;
    }
    
    if ((R_TCP.newSock = accept(R_TCP.sockFD, (sockaddr*)&R_TCP.serv_addr, (socklen_t*)&R_TCP.socklen)) == -1) {
        std::cerr << "[E] ERROR in accept()\n";
        return;
    }
    unsigned char key[] = "TvfilthyFrank";
    rc4_init(&receiver, key, 13);

    return;
}

void receiver_tcp_receive_file(char *buffer, int buffer_size, bool LOGMODE) {
    while (true)
    {
        uint32_t netNameLength = 0;

        ssize_t res = recv(R_TCP.newSock, &netNameLength, sizeof(netNameLength), 0);
        if (res <= 0) {
            break; 
        }
        
        uint32_t nameLength = ntohl(netNameLength);

        char fileNameBuffer[512] = {0};
        recv(R_TCP.newSock, fileNameBuffer, nameLength, 0);
        fileNameBuffer[nameLength] = '\0';
        std::string filename(fileNameBuffer);

        if (filename == ".f_changes.txt") LOGMODE = false;

        uint64_t netfilesize = 0;
        recv(R_TCP.newSock, &netfilesize, sizeof(netfilesize), 0);
        uint64_t filesize = be64toh(netfilesize);

        std::filesystem::path path(filename);
        if (path.is_absolute()) {
            path = path.relative_path();
        }

        std::filesystem::path finalpath = StripThePath(path);
        if(finalpath.has_parent_path()) {
            std::filesystem::create_directories(finalpath.parent_path());
        }
        

        std::ofstream file(finalpath, std::ios::binary);
        if(!file.is_open()) {
            std::cerr << "[E] ERROR in ofstream ->" << filename << "\n";
            return;
        }

        /* BytesReceived get the buffer and size it using the chunk size
           totalFileRecived add it over it BytesReceived to restart the while and get the new binary addres by filesize
        
        */

        // Cleaning the buffer before allocate it
        std::memset(buffer, 0, buffer_size);

        uint64_t totalFileRecived = 0;
        while(totalFileRecived < filesize) {
            uint64_t BytesLeft = filesize - totalFileRecived;
            uint64_t BytestoRead = (BytesLeft < BUFFER_SIZE) ? BytesLeft : BUFFER_SIZE; // for the most part its like BUFFER_SIZE aka {4096 bytes}

            ssize_t BytesReceived = recv(R_TCP.newSock, buffer, BytestoRead, 0);
            rc4_crypt(&receiver, buffer, BytesReceived);
            if (BytesReceived <= 0) {
                std::cerr << "[E] ERROR: No Connection... \n";
                std::cerr << "[!] Might Be Some DATA Received is Damged \n";
                file.close();
                return;
            }

            file.write(buffer, BytesReceived);
            totalFileRecived += BytesReceived;
        }

        file.close();
        if (LOGMODE) {
            std::cout << "[*] FILE Received ~> " << filename << " (" << covert_bytes(filesize) << ")" <<  std::endl;
        }
        

    }
    
}

void receiver_tcp_close_socket() {
    close(R_TCP.sockFD);
    close(R_TCP.newSock);
}

#ifdef ENABLE_MAIN
int main() 
{
    /*
    int sockFD, newSock;
    uint16_t port = 6767;
    sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    int socklen = sizeof(serv_addr);
    int opt = 1;
    */
    /*

    change this

    if ((R_TCP.sockFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "[E] ERROR in socker()\n";
        return -1;
    }

    R_TCP.serv_addr.sin_family = AF_INET;
    R_TCP.serv_addr.sin_port = htons(port);
    R_TCP.serv_addr.sin_addr.s_addr = INADDR_ANY;

    if ((setsockopt(R_TCP.sockFD, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR, &opt, sizeof(opt))) == -1) {
        std::cerr << "[E] ERROR in setsockopt()\n";
        return -1;
    }

    if ((bind(R_TCP.sockFD, (sockaddr*)&R_TCP.serv_addr, sizeof(R_TCP.serv_addr))) < 0) {
        std::cerr << "[E] ERROR in bind()\n";
        return -1;
    }


    if((listen(R_TCP.sockFD, SOMAXCONN)) < 0) {
        std::cerr << "[E] ERROR in connect()\n";
        return -1;
    }
    // this check
    if ((R_TCP.newSock = accept(R_TCP.sockFD, (sockaddr*)&R_TCP.serv_addr, (socklen_t*)&R_TCP.socklen)) < 0) {
        std::cerr << "[E] ERROR in accept()\n";
        return -1;
    }
    */

    receiver_tcp_init(6767);
    /*
    std::ofstream file(".f_changes.txt", std::ios::binary);
    if(!file.is_open()) {
        std::cerr << "[E] ERROR in ofstream\n";
        return -1;
    }

    ssize_t bytesReceived;
    while((bytesReceived = recv(R_TCP.newSock, buffer, BUFFER_SIZE, 0)) > 0) {
        file.write(buffer, bytesReceived);
        break;
    }

    file.close();
    */
    receiver_tcp_receive_file(R_TCP.buffer, BUFFER_SIZE);
    reciver_tcp_close_socket();
    /*
    close(R_TCP.sockFD);
    close(R_TCP.newSock);
    */
}
#endif