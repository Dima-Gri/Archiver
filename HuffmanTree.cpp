#include <iostream>
#include <string>
#include "HuffmanTree.h"

std::string HuffmanTree::GetError() {
    return error_message_;
}
bool HuffmanTree::GetFailed() {
    return failed_;
}
HuffmanTree::Point* HuffmanTree::GetRoot() {
    return root_;
}
HuffmanTree::~HuffmanTree() {
}
std::bitset<SIZE> HuffmanTree::GetFromTree(Point* point, Reader& out_huffman) {
    if (!point) {
        return MASK;
    }
    if (point->symbol != MASK) {
        return point->symbol;
    }
    auto byte = out_huffman.GetBit();
    if (byte == 0) {
        return GetFromTree(point->left, out_huffman);
    }
    return GetFromTree(point->right, out_huffman);
}
HuffmanTree* HuffmanTree::Merge(HuffmanTree* left, HuffmanTree* right) {
    if (!left->root_ || !right->root_) {
        return nullptr;
    }
    HuffmanTree* new_tree = new HuffmanTree(MASK);
    new_tree->root_->left = left->root_;
    new_tree->root_->right = right->root_;
    return new_tree;
}

VectorSubsequencesCount HuffmanTree::GetCodesForSymbols() const {
    std::vector<std::pair<std::bitset<SIZE>, size_t>> codes_for_symbols;
    Run(root_, codes_for_symbols, 0);
    return codes_for_symbols;
}

void HuffmanTree::Run(Point* point, VectorSubsequencesCount& code_lengths, size_t depth) {
    if (!point) {
        return;
    }
    if (!point->left && !point->right) {
        code_lengths.emplace_back(point->symbol, depth);
    }
    Run(point->left, code_lengths, depth + 1);
    Run(point->right, code_lengths, depth + 1);
}

void HuffmanTree::AddNewPoint(std::bitset<SIZE> symbol, std::vector<bool> code) {
    std::reverse(code.begin(), code.end());
    AddNewPoint(root_, symbol, code, failed_);
    if (failed_) {
        error_message_ = "Failed: Huffman tree not build";
    }
}

void HuffmanTree::AddNewPoint(Point* point, std::bitset<SIZE> symbol, std::vector<bool>& code, bool& failed) {
    if (code.empty()) {
        if (point->symbol != MASK) {
            failed = true;
            return;
        }
        point->symbol = symbol;
        return;
    }
    bool bit = code.back();
    code.pop_back();
    if (bit == 0) {
        if (!point->left) {
            point->left = new Point(MASK);
        }
        AddNewPoint(point->left, symbol, code, failed);
    } else {
        if (!point->right) {
            point->right = new Point(MASK);
        }
        AddNewPoint(point->right, symbol, code, failed);
    }
}

std::bitset<SIZE> HuffmanTree::GetFromTree(Reader& bin_in) {
    std::bitset<SIZE> huffman_bitset = HuffmanTree::GetFromTree(root_, bin_in);
    if (huffman_bitset == MASK) {
        failed_ = true;
        error_message_ = "Failed: file is invalid";
        return MASK;
    }
    return huffman_bitset;
}
HuffmanTree::Point::Point(std::bitset<SIZE> symbol)
    : symbol(symbol) {
}

HuffmanTree::HuffmanTree(std::bitset<SIZE> symbol) {
    root_ = new Point(symbol);
}
void HuffmanTree::Del(Point *point){
    if (point != nullptr) {
        Del(point->left);
        Del(point->right);
        delete point;
    } else{
        return;
    }
}
