#include "VirtualToken.hpp"
#include "../value/Function.hpp"
#include "../resolver/File.hpp"

namespace ls {

Token* VirtualToken::operator -> () const {
    assert(function);
    if (function->token_offset + index >= function->file->tokens.size()) {
        std::cout << function->token_offset << " " << index << " " << function->file->tokens.size() << std::endl;
        assert(false);
    }
    auto t = &function->file->tokens.at(function->token_offset + index);
    // std::cout << "VT get " << function->token_offset
    //     << " " << index
    //     << " " << function->token_offset + index
    //     << " " << t->content << std::endl;
    return t;
}

VirtualToken::operator bool() const {
    return function != nullptr;
}

}