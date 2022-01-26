#include "Treatment.h"
#include "Encoder.h"
#include "Decoder.h"
#include <cstddef>
#include <iostream>
#include <string_view>


int main(int argc, char* argv[]) {
    std::vector<std::string_view> str;
    for (int i = 1; i < argc; ++i) {
        str.push_back(std::string_view(argv[i]));
    }
    Treatment treatment(str);
    Treatment::Status status = treatment.GetStatus();
    if (status == Treatment::Status::Error) {
        std::cout << treatment.GetOutput() << std::endl;
        return 1;
    } else if (status == Treatment::Status::Info) {
        treatment.PrintInfo();
        return 0;
    }

    if (status == Treatment::Status::Archive) {
        std::ofstream out(treatment.GetArchiveName(), std::ios::binary);
        Encoder coder(out);

        for (size_t i = 0; i < treatment.GetFiles().size(); ++i) {
            auto file = treatment.GetFiles()[i];
            coder.Create(file);
            if (coder.GetFailed()) {
                std::cout << coder.GetErrorMessage() << std::endl;
                return 1;
            }
            std::cout << "File " << file << " archived" << std::endl;
        }

        std::cout << "Archive " << treatment.GetArchiveName() << " encoded successfully" << std::endl;
    } else {
        std::cout << "Decoding..." << std::endl;

        std::ifstream in(treatment.GetArchiveName(), std::ios::binary);
        Decoder decoder(in);
        decoder.Decode();
        if (decoder.GetFailed()) {
            std::cerr << decoder.GetErrorMessage() << std::endl;
            return 1;
        }

        std::cout << "Archive " + treatment.GetArchiveName() + " decoded successfully" << std::endl;
    }
    return 0;
}
