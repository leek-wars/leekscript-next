#ifndef VIRTUAL_TOKEN_HPP
#define VIRTUAL_TOKEN_HPP

#include <limits>

namespace ls {

class Function;
class Token;

class VirtualToken {
public:
    Function& function;
    unsigned int index = std::numeric_limits<unsigned int>::max();

    VirtualToken(Function& function);
    VirtualToken(Function& function, unsigned int index);
    VirtualToken& operator = (const VirtualToken&);

    Token* operator -> () const;
    operator bool() const;
};

}

#endif