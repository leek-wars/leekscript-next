#ifndef FOR_HPP
#define FOR_HPP

#include <vector>
#include "../value/Value.hpp"
#include "../value/Block.hpp"
#include "Instruction.hpp"

namespace ls {

class Block;
class SemanticVar;

class For : public Instruction {
public:

	std::shared_ptr<Token> token;
	std::vector<Instruction*> inits;
	Value* condition;
	std::vector<Instruction*> increments;
	Block* body = nullptr;

	For();
	virtual ~For();

	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	virtual void analyze(SemanticAnalyzer*, const Type& req_type) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Instruction* clone() const override;
};

}

#endif
