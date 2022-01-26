#include "Constants.h"

class Encoder {
public:
    Encoder(std::ofstream& out);
    ~Encoder();
    void Create(const std::string& file_name);
    void GetCanCode(std::unordered_map<std::bitset<SIZE>, size_t>& freque);
    void MakeCanCode(std::vector<std::pair<std::bitset<SIZE>, size_t>>& size_code);
    void Encode(const std::string& file_name);
    bool GetFailed() const;
    const std::string& GetErrorMessage() const;
private:
    Writer out_stream_;
    std::vector<std::bitset<SIZE>> new_symbols_;
    std::unordered_map<std::bitset<SIZE>, std::vector<bool>> can_codes_;
    std::string error_message_;
    bool flag_end_of_file_ = true;
    bool failed_ = false;
};
