#include "ByteParser.h"
#include <cstddef>
#include <iostream>
#include <vector>


Reader::Reader(std::ifstream& in) : in_(in) {
}
bool Reader::GetBit() {
    if (buffer_.empty()) {
        if (in_.peek() == EOF) {
            error_message_ = "Failed: file is invalid";
            flag_ = true;
            return false;
        }
        char symbol = in_.get();
        for (size_t i = 0; i < SUBSEQUENCE_SIZE; ++i) {
            buffer_.push_back(symbol & (1 << i));
        }
    }
    char bit = *(std::prev(buffer_.end(), 1));
    buffer_.pop_back();
    return bit;
}

std::bitset<SIZE> Reader::GetBits(size_t size) {
    std::bitset<SIZE> res;
    size_t index = 0;
    while (index < size) {
        res[index] = GetBit();
        if (flag_) {
            return {};
        }
        ++index;
    }
    return res;
}

std::bitset<SIZE> Reader::GetSymbol() {
    return GetBits(SIZE);
}

size_t Reader::GetSize() {
    auto symbol = GetSymbol();
    if (flag_) {
        return 0;
    }
    return symbol.to_ullong();
}
Reader::~Reader() {
    in_.close();
}
Writer::Writer(std::ofstream& out) : out_(out) {
}

void Writer::Write(const std::bitset<SIZE>& bitset) {
    Write(bitset.to_ullong(), bitset.size());
}

void Writer::Write(uint64_t val, size_t len) {
    std::vector<bool> temp_vec_bool;
    for (size_t i = 0; i < len; ++i) {
        temp_vec_bool.push_back(val % BINARY);
        val = int(val / 2);
    }
    Write(temp_vec_bool);
}

void Writer::Write(const std::vector<bool>& vec_bool) {
    for (const auto& bit : vec_bool) {
        buffer_.push_back(bit);
        if (buffer_.size() == SUBSEQUENCE_SIZE) {
            char buff_char = 0;
            for (size_t i = 0; i < SUBSEQUENCE_SIZE; ++i) {
                buff_char |= (buffer_[i] << (SUBSEQUENCE_SIZE - i - 1));
            }
            out_.put(buff_char);
            buffer_.clear();
        }
    }
}

Writer::~Writer() {
    if (buffer_.empty()) {
        return;
    }
    while (buffer_.size() < SUBSEQUENCE_SIZE) {
        buffer_.push_back(ADDITTIONAL_BIT);
    }
    char buff_char = 0;
    for (size_t i = 0; i < SUBSEQUENCE_SIZE; ++i) {
        buff_char |= (buffer_[i] << (SUBSEQUENCE_SIZE - i - 1));
    }
    out_.put(buff_char);
    buffer_.clear();
    out_.close();
}
