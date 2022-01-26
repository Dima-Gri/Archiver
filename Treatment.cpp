#include "Treatment.h"

#include <cstddef>
#include <iostream>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

std::string Treatment::GetArchiveName() {
    return archive_name_;
}
std::string Treatment::GetOutput() {
    return output_;
}
Treatment::Status Treatment::GetStatus() {
    return status_;
}
std::vector<std::string> Treatment::GetFiles() {
    return files_;
}

Treatment::Treatment(std::vector<std::string_view>& arguments) {
    if (arguments.empty()) {
        output_ = "Failed: You can use 'archiver -h' to see info";
        status_ = Status::Error;
        return;
    }
    
    if (arguments[0] == "-h") {
        status_ = Status::Info;
    } else if (arguments[0] == "-d") {
        if (arguments.size() == 2) {
            status_ = Status::DeArchive;
            archive_name_ = arguments[1];
        } else {
            output_ = "Failed: You can use 'archiver -h' to see info";
            status_ = Status::Error;
        }
    } else if (arguments[0] == "-c") {
        if (arguments.size() > 2) {
            status_ = Status::Archive;
            archive_name_ = arguments[1];
            for (size_t i = 2; i < arguments.size(); ++i) {
                files_.emplace_back(arguments[i]);
            }
            if (!CheckFiles(files_)) {
                return;
            }
        } else {
            output_ = "Failed: You can use 'archiver -h' to see info";
            status_ = Status::Error;
        }
    } else {
        status_ = Status::Error;
        output_ = "Failed: You can use 'archiver -h' to see info";
    }
}

bool Treatment::CheckFiles(const std::vector<std::string>& file) {
    for (const auto& it : file) {
        if (!CheckFile(it)) {
            return false;
        }
    }
    return true;
}

bool Treatment::CheckFile(const std::string& names) {
    if (!std::filesystem::exists(names)) {
        status_ = Status::Error;
        output_ = "Filed: File " + names + " not find.";
        return false;
    }
    return true;
}

void Treatment::PrintInfo() {
    std::cout << std::endl << "Use 'archiver -c archive_name file1 [file2 ...]' to create archive of file1, file2 ... with name 'archive_name." << std::endl << "Must contain as least 3 arguments'" << std::endl << std::endl << ""
                "Use 'archiver -d archive_name' to get archived files in the same directory." << std::endl << "Must contain 3 arguments." << std::endl << std::endl <<
                "Use again 'archiver -h to see more information" << std::endl << std::endl;
}
