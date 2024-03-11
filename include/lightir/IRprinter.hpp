#include "Value.hpp"
#include "Module.hpp"
#include "Function.hpp"
#include "GlobalVariable.hpp"
#include "Constant.hpp"
#include "BasicBlock.hpp"
#include "Instruction.hpp"
#include "User.hpp"
#include "Type.hpp"

std::string print_as_op(Value *v, bool print_ty);
std::string print_cmp_type(CmpOp op);
std::string print_fcmp_type(CmpOp op);
/*#pragma once

#include "BasicBlock.hpp"
#include "Constant.hpp"
#include "Function.hpp"
#include "GlobalVariable.hpp"
#include "Instruction.hpp"
#include "Module.hpp"
#include "Type.hpp"
#include "User.hpp"
#include "Value.hpp"

std::string print_as_op(Value *v, bool print_ty);
std::string print_instr_op_name(Instruction::OpID);*/
