#include "Encoder.h"
bool Encoder::GetFailed() const {
    return failed_;
}
const std::string& Encoder::GetErrorMessage() const {
    return error_message_;
}
void Encoder::Create(const std::string& name) {
    if (!flag_end_of_file_) {
        if (!can_codes_.contains(ONE_MORE_FILE)) {
            failed_ = true;
            error_message_ = "Failed: ONE_MORE_FILE";
            return;
        }
        out_stream_.Write(can_codes_[ONE_MORE_FILE]);
    }
    can_codes_.clear();
    new_symbols_.clear();
    flag_end_of_file_ = false;
    std::unordered_map<std::bitset<SIZE>, size_t> map_freq;

    for (auto c : name) {
        map_freq[std::bitset<SIZE>(c)]++;
    }
    map_freq[FILENAME_END]++;

    std::ifstream in(name, std::ios::binary);
    while (in.peek() != EOF) {
        char ch = in.get();
        map_freq[std::bitset<SIZE>(ch)]++;
    }
    in.close();
    map_freq[ONE_MORE_FILE]++;
    map_freq[ARCHIVE_END]++;
    GetCanCode(map_freq);
    if (failed_) {
        return;  
    }
    Encode(name);
}
void Encoder::GetCanCode(std::unordered_map<std::bitset<SIZE>, size_t>& freque) {
    auto cmp = [](const std::pair<size_t, HuffmanTree*>& a, const std::pair<size_t, HuffmanTree*>& b) {
        return a.first > b.first;
    };
    std::priority_queue<std::pair<size_t, HuffmanTree*>, std::vector<std::pair<size_t, HuffmanTree*>>, decltype(cmp)> priority_queue;
    for (const auto& [frequence, count] : freque) {
        HuffmanTree* new_tree = new HuffmanTree(frequence);
        std::pair<size_t, HuffmanTree*> huffman_tree = std::make_pair(count, new_tree);
        priority_queue.push(huffman_tree);
    }
    std::vector<HuffmanTree*> future_deleted_pointer;
    while (priority_queue.size() > 1) {
        std::pair<size_t, HuffmanTree*> top_1 = priority_queue.top();
        priority_queue.pop();
        std::pair<size_t, HuffmanTree*> top_2 = priority_queue.top();
        priority_queue.pop();
        HuffmanTree* new_tree = HuffmanTree::Merge(top_1.second, top_2.second);
        priority_queue.emplace(top_1.first + top_2.first, new_tree);
        future_deleted_pointer.push_back(top_1.second);
        future_deleted_pointer.push_back(top_2.second);
    }
    VectorSubsequencesCount size_code = priority_queue.top().second->GetCodesForSymbols();
    if (size_code.empty()) {
        failed_ = true;
        error_message_ = "Failed: impossible to get canonical codes to symbols";
        return;
    }
    
    for (const auto& point : future_deleted_pointer) {
        delete point->GetRoot();
        delete point;
    }
    delete priority_queue.top().second->GetRoot();
    delete priority_queue.top().second;    
    MakeCanCode(size_code);
}

void Encoder::MakeCanCode(std::vector<std::pair<std::bitset<SIZE>, size_t>>& size_code) {
    std::sort(size_code.begin(), size_code.end(),
              [](const auto& a, const auto& b) {
                auto a_symbol = a.first.to_ullong();
                auto b_symbol = b.first.to_ullong();
                return std::tie(a.second, a_symbol) < std::tie(b.second, b_symbol);
              }
    );
    uint64_t new_digit_code = 0;
    for (size_t i = 0; i < size_code.size(); ++i) {
        size_t len = size_code[i].second;
        std::bitset<SIZE> symbol = size_code[i].first;

        new_symbols_.push_back(symbol);
        uint64_t temperary_code = new_digit_code;
        for (size_t bit = 0; bit < len; ++bit) {
            can_codes_[symbol].push_back((temperary_code & 1) == 1);
            temperary_code >>= 1;
        }
        std::reverse(can_codes_[symbol].begin(), can_codes_[symbol].end());
        if (i + 1 < size_code.size()) {
            size_t next_len = size_code[i + 1].second;
            new_digit_code = (new_digit_code + 1) << (next_len - len);
        }
    }
}

void Encoder::Encode(const std::string& name) {
    out_stream_.Write(new_symbols_.size(), SIZE);
    for (const auto& symbol : new_symbols_) {
        out_stream_.Write(symbol);
    }
    std::vector<size_t> vec_couting_lengths;
    for (const auto& [symbol, code] : can_codes_) {
        size_t len = code.size();
        while (vec_couting_lengths.size() < len) {
            vec_couting_lengths.emplace_back(0);
        }
        vec_couting_lengths[len - 1]++;
    }
    for (size_t i = 1; i <= vec_couting_lengths.size(); ++i) {
        out_stream_.Write(vec_couting_lengths[i - 1], SIZE);
    }
    for (char ch : name) {
        out_stream_.Write(can_codes_[std::bitset<SIZE>(ch)]);
    }
    out_stream_.Write(can_codes_[FILENAME_END]);
    std::ifstream in(name, std::ios::binary);
    while (in.peek() != EOF) {
        char ch = in.get();
        out_stream_.Write(can_codes_[std::bitset<SIZE>(ch)]);
    }
    in.close();
}

Encoder::Encoder(std::ofstream& out) : out_stream_(out) {
}
Encoder::~Encoder() {
    if (!failed_) {
        out_stream_.Write(can_codes_[ARCHIVE_END]);
    }
}