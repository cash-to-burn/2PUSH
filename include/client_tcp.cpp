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


struct CTCPdata
{
    int sockFD;
    std::string ip;
    uint16_t port = 6767;
    sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    bool connect_condtion = false;
    
} C_TCP;

RC4_State client;



void client_tcp_init(std::string ip_addres, uint16_t port) {
    if ((C_TCP.sockFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "[E] ERROR in socker()\n";
        return;
    }

    C_TCP.serv_addr.sin_family = AF_INET;
    C_TCP.serv_addr.sin_port = htons(port);

    if ((inet_pton(AF_INET, ip_addres.c_str(), &C_TCP.serv_addr.sin_addr)) <= 0) {
        std::cerr << "[E] ERROR in inet_pton()\n";
        return;
    }

    if((connect(C_TCP.sockFD, (struct sockaddr*) &C_TCP.serv_addr, sizeof(C_TCP.serv_addr))) == -1) {
        // std::cerr << "[E] ERROR in connect()\n";
        
        return;
    }
    
    unsigned char key[] = "TvfilthyFrank";
    rc4_init(&client, key, 13);


    C_TCP.connect_condtion = true;
}


void client_tcp_send_file(std::string filePath, char *buffer, int buffer_size, bool LOGMODE) {
    std::ifstream Send_file(filePath, std::ios::binary);
    if (!Send_file.is_open()) {
        std::cerr << "[E] ERROR in ifstream\n";
        //return -1;
    }
    Send_file.seekg(0, std::ios::end);
    std::streamsize fileSize = Send_file.tellg();
    Send_file.seekg(0, std::ios::beg);

    std::filesystem::path path(filePath);
    std::filesystem::path relativePath = std::filesystem::relative(path, std::filesystem::current_path());
    
    std::string FileName = relativePath.string();

    uint64_t FileNamelength = FileName.length();

    uint32_t netNamelength = htonl(FileNamelength);
    send(C_TCP.sockFD, &netNamelength, sizeof(netNamelength), 0);

    send(C_TCP.sockFD, FileName.c_str(), FileNamelength, 0);

    uint64_t netFileSize = htobe64(fileSize);
    send(C_TCP.sockFD, &netFileSize, sizeof(netFileSize), 0);

    if (fileSize > 0) {
        while ((Send_file.read(buffer, buffer_size)) || (Send_file.gcount() > 0)) {
        ssize_t Readedbyts = Send_file.gcount();
        rc4_crypt(&client, buffer, Readedbyts);
        if(send(C_TCP.sockFD, buffer, Readedbyts, 0) == -1) {
            std::cerr << "[E] ERROR in send()\n";
            //return -1;
        }
        
        
        
    }   
    if (LOGMODE) {
        std::cout << "[+] Pushing New Files ~> " << FileName << " (" << covert_bytes(fileSize) << ")" << std::endl;
    }
        
    }

    Send_file.close();
}


void client_tcp_close_socket() {
    close(C_TCP.sockFD);
}


#ifdef ENABLE_MAIN
int main() 
{
    /*
    if ((C_TCP.sockFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "[E] ERROR in socker()\n";
        return -1;
    }

    C_TCP.serv_addr.sin_family = AF_INET;
    C_TCP.serv_addr.sin_port = htons(C_TCP.port);

    if ((inet_pton(AF_INET, C_TCP.ip.c_str(), &C_TCP.serv_addr.sin_addr)) <= 0) {
        std::cerr << "[E] ERROR in inet_pton()\n";
        return -1;
    }


    if((connect(C_TCP.sockFD, (struct sockaddr*) &C_TCP.serv_addr, sizeof(C_TCP.serv_addr))) == -1) {
        std::cerr << "[E] ERROR in connect()\n";
        return -1;
    }
    */

    client_tcp_init("192.168.122.1", C_TCP.port);



    get_changes(".");
    save_changes(file.NFiles, file.SFiles);
    /*
    std::ifstream file(".f_changes.txt", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[E] ERROR in ifstream\n";
        return -1;
    }
    
    
    
    while ((file.read(C_TCP.buffer, BUFFER_SIZE)) || (file.gcount() > 0)) {
        ssize_t Readedbyts = file.gcount();
        if(send(C_TCP.sockFD, C_TCP.buffer, Readedbyts, 0) == -1) {
            std::cerr << "[E] ERROR in send()\n";
            return -1;
        }
    }
    */

    client_tcp_send_file(".f_changes.txt", C_TCP.buffer, BUFFER_SIZE);
    client_tcp_close_socket();

    // file.close();
    

}
#endif