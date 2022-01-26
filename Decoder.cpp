#include "Decoder.h"
bool Decoder::GetFailed() const{
    return failed_;
}
const std::string& Decoder::GetErrorMessage() const{
    return error_message_;
}
Decoder::Decoder(std::ifstream& in) : in_stream_(in) {
}
void Decoder::Decode() {
    bool files_ended = false;
    while (!files_ended) {
        can_codes_.clear();
        symbols_count_ = 0;
        vec_bits_frequences_.clear();
        symbols_count_ = in_stream_.GetSize();
        for (size_t i = 0; i < symbols_count_; ++i) {
            vec_bits_frequences_.push_back(in_stream_.GetSymbol());
        }
        size_t index = 0;
        size_t value = 0;
        for (size_t i = 1; index < symbols_count_; ++i) {
            size_t cnt = in_stream_.GetSize();
            for (size_t j = 0; j < cnt; ++j) {
                size_t new_value = value;
                std::vector<bool> code;
                for (size_t bit = 0; bit < i; ++bit) {
                    code.push_back(new_value % 2);
                    new_value = int(new_value / 2);
                }
                std::reverse(code.begin(), code.end());
                if (index == symbols_count_) {
                    failed_ = true;
                    error_message_ = "Failed: file can not be decoded";
                    return;
                }
                can_codes_[code] = vec_bits_frequences_[index++];
                value += 1;
            }
            value *= 2;
        }
        HuffmanTree huffman_tree(MASK);
        for (const auto& [symbol, frequence] : can_codes_) {
            huffman_tree.AddNewPoint(frequence, symbol);
            if (huffman_tree.GetFailed()) {
                failed_ = true;
                error_message_ = huffman_tree.GetError();
                return;
            }
        }
        files_ended = DecodeFile(huffman_tree);
        huffman_tree.Del(huffman_tree.GetRoot()->left);
        huffman_tree.Del(huffman_tree.GetRoot()->right);
        delete huffman_tree.GetRoot();        
    }
}
bool Decoder::DecodeFile(HuffmanTree& huffman_tree) {
    std::bitset<SIZE> symbol = GetSymbol(huffman_tree);
    if (failed_) {
        return true;
    }
    std::string name;
    while (symbol != FILENAME_END) {
        char c = static_cast<char>(symbol.to_ullong());
        name += c;
        symbol = GetSymbol(huffman_tree);
        if (failed_) {
            return true;
        }
    }
    std::ofstream out(name, std::ios::binary);
    symbol = GetSymbol(huffman_tree);
    if (failed_) {
        return true;
    }
    while (symbol != ONE_MORE_FILE && symbol != ARCHIVE_END) {
        char c = static_cast<char>(symbol.to_ullong());
        out.put(c);
        symbol = GetSymbol(huffman_tree);
        if (failed_) {
            return true;
        }
    }
    out.close();
    std::cout << "Decoded file " << name << std::endl;
    return symbol == ARCHIVE_END;
}
std::bitset<SIZE> Decoder::GetSymbol(HuffmanTree& huffman_tree) {
    std::bitset<SIZE> symbol = huffman_tree.GetFromTree(in_stream_);
    if (huffman_tree.GetFailed()) {
        failed_ = true;
        error_message_ = huffman_tree.GetError();
        return MASK;
    }
    return symbol;
}