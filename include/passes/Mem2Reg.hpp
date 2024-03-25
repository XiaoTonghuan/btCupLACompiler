#pragma once

#include "BasicBlock.hpp"
#include "Function.hpp"
#include "IRBuilder.hpp"
#include "Instruction.hpp"
#include "Module.hpp"
#include "PassManager.hpp"

class Mem2Reg : public Pass{
public:
	explicit Mem2Reg(Module *m, bool print_ir=false) : Pass(m, print_ir) {}
	~Mem2Reg(){};
	void execute() final;
	void genPhi();
	void insideBlockForwarding();
	void valueDefineCounting();
	void valueForwarding(BasicBlock *bb);
	void removeAlloc();
	void phiStatistic();
    const std::string get_name() const override {return name;}

	bool isLocalVarOp(Instruction *inst){
		if (inst->get_instr_type() == Instruction::OpID::store){
			StoreInst *sinst = static_cast<StoreInst *>(inst);
			auto lvalue = sinst->get_lval();
			auto glob = dynamic_cast<GlobalVariable *>(lvalue);
			auto array_element_ptr = dynamic_cast<GetElementPtrInst *>(lvalue);
			return !glob && !array_element_ptr;
		}
		else if (inst->get_instr_type() == Instruction::OpID::load){
			LoadInst *linst = static_cast<LoadInst *>(inst);
			auto lvalue = linst->get_lval();
			auto glob = dynamic_cast<GlobalVariable *>(lvalue);
			auto array_element_ptr = dynamic_cast<GetElementPtrInst *>(lvalue);
			return !glob && !array_element_ptr;
		}
		else
			return false;
	}
	//// void DeleteLS();
private:
	Function *func_;
	IRBuilder *builder;
	std::map<BasicBlock *, std::vector<Value *>> define_var;
    const std::string name = "Mem2Reg";
	std::map<Value*, Value*> lvalue_connection;
	std::set<Value*> no_union_set;

};

