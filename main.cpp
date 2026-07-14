
/*
    TODO:
    - improve handel delete 
*/


#include <iostream>
#include "include/H_file.h"
#include "include/client_tcp.cpp"
#include "include/receiver_tcp.cpp"

void Function_sync(std::string ip, uint16_t port, bool sync_conditon, std::string path) {
    if ((file.New_changed_path.size() > 0) && (sync_conditon == true)) {
        for(int i =0; i < file.New_changed_path.size(); i++) {
            client_tcp_send_file(file.New_changed_path[i], C_TCP.buffer, BUFFER_SIZE, LOGMODE::ON);
        }
    } else return;
}

void Function_send_all(std::string ip, uint16_t port, bool send_all_conditon) {
    if(send_all_conditon == true) {
        for (int i = 0; i < file.NFiles.size(); i++){
            client_tcp_send_file(file.NFiles[i], C_TCP.buffer, BUFFER_SIZE, LOGMODE::ON);
        }
    } else return;
}

void Function_receive(uint16_t port, bool receive_condtion) {
    if (receive_condtion == true) {
        receiver_tcp_init(port);
        receiver_tcp_receive_file(R_TCP.buffer, BUFFER_SIZE, LOGMODE::ON);
    }
}

void print_help() {

    std::printf("\n 2PUSH help menu: \n \n -ip --> IP Target to Send Files \n -port --> PORT Target to Send Files \n -sync --> Sync Directories Files \n -send_all --> Send All Files in The Directories \n -receive --> Endable Receiver Mode \n \n Written by Unc Frank \n");


    return;
}


int main (int argc, char *argv[]) {
    
    if (argc < 2) {
        std::cerr << "[!] Using ~> " << argv[0] << " -param /path/to/fold \n[!] USE --help" << std::endl;
        return -1;
    }
    std::string path;
    std::string ip;
    uint16_t port;
    bool receive;
    bool send_all;
    bool sync;
    bool ip_condation = false;
    bool port_condation = false;
    for (int i = 0; i < argc; i++ ) {
        if (std::string(argv[i]) == "-sync" ) {
            if (i + 1 < argc) {
                path = argv[i+1];
                sync = true;
                i++;
            }
        }
        if (std::string(argv[i]) == "-send_all" ) {
            if (i + 1 < argc) {
                path = argv[i+1];
                send_all = true;
            }
        } 
        if(std::string(argv[i]) == "-ip") {
                ip = argv[i+1];
                ip_condation = true;
                }    

        if(std::string(argv[i]) == "-port") {
            port = static_cast<uint16_t>(std::stoi(argv[i+1]));
            port_condation = true;
            }
        if(std::string(argv[i]) == "--help") {
                        print_help();
        }
        if (std::string(argv[i]) == "-receive") {
            if (i + 1 < argc) {
                ip = argv[i+1];
                receive = true;
            }     
        }  
    }
    
    
    

    if ((receive == true) && (port_condation == true)) {
        std::cout << "[~] start receiving files ..." << std::endl;
        Function_receive(port, receive);
        return 0;
    } else if ((sync == true) || (send_all == true) && (ip_condation == true) && (port_condation == true)) {
        get_changes(path);
        check_def(Size_calc_buffer_from_file(".f_changes.txt"), file.SFiles, Path_calc_buffer_from_file(".f_changes.txt"), file.NFiles);
        client_tcp_init(ip, port);
        

        if (C_TCP.connect_condtion == true) {
            if (sync) {
                Function_sync(ip, port, sync, path);
            }
            if (send_all) {
                Function_send_all(ip, port, send_all);
            }
        }
        
    }
    

    /*
        check up for the file ".f_changes.txt" 
        TODO: make it in saprit function
            update:
            make it as function causes this -> Illegal instruction (core dumped)
    */

    if (sync || send_all) {
        std::ifstream file_check(".f_changes.txt"); 
        if (!file_check.is_open()) {
            save_changes(file.NFiles, file.SFiles);
            std::cout << "[#] First Time Initializing This Path !" << std::endl;
            client_tcp_close_socket();
    } else {
        if (!C_TCP.connect_condtion) {
            std::cout << "[#] NOT Initializing The Path Because There is No Connection ..." << std::endl;
        } else {
            save_changes(file.NFiles, file.SFiles);
            std::cout << "[#] Initializing The Path is Done ..." << std::endl;
            client_tcp_send_file(".f_changes.txt", C_TCP.buffer, BUFFER_SIZE, LOGMODE::OFF);
            if ((file.New_changed_path.size() == 0) && sync == true) {
                std::cout << "[#] NO CHANGES HAVE DONE BEFORE" << std::endl;
            }
        }
    }
    }
    
    
    if (receive) {
        receiver_tcp_close_socket();
    } else if (sync || send_all) {
        client_tcp_close_socket();
    }
    
    

    return 0;
}