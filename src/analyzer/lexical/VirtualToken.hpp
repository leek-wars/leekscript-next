#ifndef VIRTUAL_TOKEN_HPP
#define VIRTUAL_TOKEN_HPP

namespace ls {

class Function;
class Token;

class VirtualToken {
public:
    Function* function = nullptr;
    unsigned int index;

    Token* operator -> () const;
    operator bool() const;
};

}

#endif