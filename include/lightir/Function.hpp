#pragma once

#include <cassert>
#include <iterator>
#include <list>
#include <vector>

#include "User.hpp"
#include "Module.hpp"
#include "BasicBlock.hpp"
#include "Type.hpp"

class Module;
class Argument;
class BasicBlock;
class Type;
class FunctionType;

class Function : public Value {
public:
    Function(FunctionType *ty, const std::string &name, Module *parent);
    ~Function();

    static Function *create(FunctionType *ty, const std::string &name, Module *parent);

    FunctionType *get_function_type() const { return static_cast<FunctionType *>(get_type()); }
    Type *get_return_type() const { return get_function_type()->get_return_type(); }

    Module *get_parent() const { return parent_; }

    unsigned get_num_of_args() const { return get_function_type()->get_num_of_args(); }
    std::list<Argument *>::iterator arg_begin() { return arguments_.begin(); }
    std::list<Argument *>::iterator arg_end() { return arguments_.end(); }
    std::list<Argument *> &get_args() { return arguments_; }

    std::vector<Argument*> &get_iargs() { return i_args_; }
    std::vector<Argument*> &get_fargs() { return f_args_; }

    void add_basic_block(BasicBlock *bb);
    unsigned get_num_basic_blocks() const { return basic_blocks_.size(); }
    BasicBlock *get_entry_block() const { return *basic_blocks_.begin(); }
    std::list<BasicBlock *>&get_basic_blocks() { return basic_blocks_; }
    void remove_basic_block(BasicBlock *bb);

    bool is_declaration() { return basic_blocks_.empty(); }

    void set_instr_name();

    std::string print();
  
private:
    void build_args();

private:
    std::list<BasicBlock *> basic_blocks_;  //& basic blocks
    std::list<Argument *> arguments_;       //& arguments  
    std::vector<Argument*> i_args_;
    std::vector<Argument*> f_args_;
    Module *parent_;  
    unsigned seq_cnt_;
};

//// Argument of Function, does not contain actual value
class Argument : public Value {
public:
    //// Argument constructor.
    explicit Argument(Type *ty, const std::string &name="", Function *f=nullptr, 
                    unsigned arg_no = 0)
        : Value(ty, name), parent_(f), arg_no_(arg_no) {}
    virtual ~Argument() {}

    inline const Function *get_parent() const { return parent_; }
    inline Function *get_parent() { return parent_; }

    //// For example in "void foo(int a, float b)" a is 0 and b is 1.
    unsigned get_arg_no() const { 
      assert(parent_ && "can't get number of unparented arg");
      return arg_no_;
    }

    virtual std::string print() override;

private:
    Function *parent_;
    unsigned arg_no_; //& argument No. 
};

/*#pragma once

#include "BasicBlock.hpp"
#include "Type.hpp"
#include "User.hpp"

#include <cassert>
#include <cstddef>
#include <iterator>
#include <list>
#include <llvm/ADT/ilist.h>
#include <llvm/ADT/ilist_node.h>
#include <map>
#include <memory>

class Module;
class Argument;
class Type;
class FunctionType;

class Function : public Value, public llvm::ilist_node<Function> {
  public:
    Function(const Function &) = delete;
    Function(FunctionType *ty, const std::string &name, Module *parent);
    ~Function() = default;
    static Function *create(FunctionType *ty, const std::string &name,
                            Module *parent);

    FunctionType *get_function_type() const;
    Type *get_return_type() const;

    void add_basic_block(BasicBlock *bb);

    unsigned get_num_of_args() const;
    std::list<Argument *>::iterator arg_begin() { return arguments_.begin(); }
    std::list<Argument *>::iterator arg_end() { return arguments_.end(); }
    unsigned get_num_basic_blocks() const;

    Module *get_parent() const;

    void remove(BasicBlock *bb);
    BasicBlock *get_entry_block() { return &*basic_blocks_.begin(); }

    llvm::ilist<BasicBlock> &get_basic_blocks() { return basic_blocks_; }
    std::vector<Argument*> &get_iargs() { return i_args_; }
    std::vector<Argument*> &get_fargs() { return f_args_; }

    bool is_declaration() { return basic_blocks_.empty(); }

    void set_instr_name();
    std::string print();

  private:
    llvm::ilist<BasicBlock> basic_blocks_;
    std::list<Argument*> arguments_;
    std::vector<Argument*> i_args_;
    std::vector<Argument*> f_args_;
    Module *parent_;
    unsigned seq_cnt_; // print use
};

// Argument of Function, does not contain actual value
class Argument : public Value {
  public:
    Argument(const Argument &) = delete;
    explicit Argument(Type *ty, const std::string &name = "",
                      Function *f = nullptr, unsigned arg_no = 0)
        : Value(ty, name), parent_(f), arg_no_(arg_no) {}
    virtual ~Argument() {}

    inline const Function *get_parent() const { return parent_; }
    inline Function *get_parent() { return parent_; }

    /// For example in "void foo(int a, float b)" a is 0 and b is 1.
    unsigned get_arg_no() const {
        assert(parent_ && "can't get number of unparented arg");
        return arg_no_;
    }

    virtual std::string print() override;

  private:
    Function *parent_;
    unsigned arg_no_; // argument No.
};*/
