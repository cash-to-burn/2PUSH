#pragma once
/*
 -- TODO:
        - [IMPORT] add a date vector
        - add a checker for .f_changes.txt aka {New_size[i]} for if it int or string
        - make Path_calc_buffer_from_file & Size_calc_buffer_from_file same function and store values in struct(data)
*/

#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <cmath>
#include <limits>

void skip_lines(std::ifstream &S_file, int numLine) {
    for (int i = 0; i < numLine; i++) {
        S_file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}





namespace LOGMODE
{
    const bool ON = true;
    const bool OFF = false;
}



struct data
{
    std::vector<std::string> NFiles;
    std::vector<uintmax_t> SFiles;
    std::vector<std::string> Check_Buffer;

    std::vector<std::string> New_changed_path;
    std::vector<std::string> Deleted_path;


} file;

char *covert_bytes(uint64_t bytes) {
    char *result = (char *) malloc(sizeof(char) * 20);
    char *sizeNames[] = { "B", "KB", "MB", "GB" };

    uint64_t i = (uint64_t) floor(log(bytes) / log(1024));
    double humanSize = bytes / pow(1024, i);
    snprintf(result, sizeof(char) * 20, "%g %s", humanSize, sizeNames[i]);

    return result;
}



int find_index(const std::vector<std::string> Existing_path, const std::string name) {
    for (size_t i = 0 ; i < Existing_path.size(); i++) {
        if(Existing_path[i] == name) return (int)i;
    }
    return -1;
}


 void check_def(std::vector<uintmax_t> Existing_size, std::vector<uintmax_t> New_size, std::vector<std::string> Existing_path, std::vector<std::string> New_path) {
    sleep(0.5);

    for (unsigned int i=0; i < New_path.size(); i++ ) {
        int index = find_index(Existing_path, New_path[i]);
        if (index == -1) {
            //std::cout << "[+] file added -> " << New_path[i] << " | " << New_size[i] << std::endl; 
            file.New_changed_path.push_back(New_path[i]);
        } else if (Existing_size[index] != New_size[i]) {
            //std::cout << "[~] file changed -> " << New_path[i] << " | " << New_size[i] << std::endl;
            file.New_changed_path.push_back(New_path[i]);
        }
    }


    for (unsigned int i = 0; i < Existing_path.size(); i++) {
        if (find_index(New_path, Existing_path[i]) == -1) {
            //std::cout << "[-] file deleted -> " << Existing_path[i] << " | " << Existing_path[i] << std::endl;
            file.Deleted_path.push_back(Existing_path[i]);
        }
    }

 }


std::vector<std::string> Path_calc_buffer_from_file(const std::string Npath){
    std::ifstream path(Npath);
    std::string line;
    
    if (!path.is_open()) {
        std::cerr << "Error in Path_calc_buffer_from_file()" << std::endl;
    }
    /*
    if (path.is_open()) {
        while (getline(path, line)) {
            file.Check_Buffer.push_back(line);
        }
    }
    */
    char open_bracket;
    char closed_bracket;
    uintmax_t size;
    std::string name;

    std::vector<std::string> Pbuffer;
    skip_lines(path, 8);
    while (path >> open_bracket >> size >> closed_bracket >> name) {
        if (open_bracket == '(' && closed_bracket == ')') {
            Pbuffer.push_back(name);
        }
    }
    
    path.close();
    return Pbuffer;
}

std::vector<std::uintmax_t> Size_calc_buffer_from_file(const std::string Npath){
    std::ifstream path(Npath);
    std::string line;
    
    if (!path.is_open()) {
        std::cerr << "Error in Size_calc_buffer_from_file()" << std::endl;
    }
    /*
    if (path.is_open()) {
        while (getline(path, line)) {
            file.Check_Buffer.push_back(line);
        }
    }
    */
    char open_bracket;
    char closed_bracket;
    uintmax_t t_size;
    std::string name;

    std::vector<std::uintmax_t> Sbuffer;
    skip_lines(path, 8);
    while (path >> open_bracket >> t_size >> closed_bracket >> name) {
        if (open_bracket == '(' && closed_bracket == ')') {
            //TODO: add a cheacker for int
            Sbuffer.push_back(t_size);
        }
    }
    
    path.close();
    return Sbuffer;
}


void save_changes(std::vector<std::string> files, std::vector<uintmax_t> size) {
    std::ofstream change_file(".f_changes.txt");
    
    if (!change_file.is_open()) {
        std::cerr << "[E] ERROR in save_changes()" << std::endl;
        exit(1);
    }

    const std::string warrning = R"([*][!][!][!][!][!][!][!][!][!][!][!][!][!][!][!][!][!][!][!][!][!][[*]
[!]                                                                [!]
[!]                                                                [!]
[!]                DO NOT CHANGE ANYTHING HERE !                   [!]
[!]                                                                [!]
[!]                                                                [!]
[*][!][!][!][!][!][!][!][!][!][!][!][!][!][!][!][!][!][!][!][!][!][[*])";

    change_file << warrning << "\n \n";

    for (int i = 0; i < files.size(); i++) {
        change_file << "("<< size[i] <<") "<< files[i] <<  "\n";
    }
    change_file.close();
}

void get_changes(std::string Fpath) {
    for (const auto i : std::filesystem::recursive_directory_iterator(Fpath)) {
        
        if (!std::filesystem::is_regular_file(i.status())) continue;
        
        if (i.path().string() == "./.f_changes.txt") continue;

        file.NFiles.push_back(std::filesystem::path(i.path().string())); 
        file.SFiles.push_back(std::filesystem::file_size(i.path()));
        }
}

