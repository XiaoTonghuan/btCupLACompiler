#pragma once

#include "BasicBlock.hpp"
#include "Constant.hpp"
#include "Function.hpp"
#include "IRBuilder.hpp"
#include "Module.hpp"
#include "Type.hpp"
#include "ast.hpp"
#include "Value.hpp"
#include <iostream>
#include <map>
#include <memory>
#include <vector>

class Scope {
  public:
    void enter() { 
            lookup_var_table.push_back({});
            lookup_func_table.push_back({});
            lookup_array_size_table.push_back({});
            lookup_array_const_table.push_back({});    
        }
        void exit() { 
            lookup_var_table.pop_back(); 
            lookup_func_table.pop_back();
            lookup_array_size_table.pop_back();
            lookup_array_const_table.pop_back();    
        }

        bool in_global() { return lookup_var_table.size() == 1; }

        bool push(std::string name, Value *val) {
            auto result = lookup_var_table[lookup_var_table.size() - 1].insert({name, val});
            return result.second; 
        }

        Value* find(std::string name) {
            for(auto v = lookup_var_table.rbegin(); v != lookup_var_table.rend(); v++) {
                auto iter = v->find(name);
                if(iter != v->end()) return iter->second;
            }

            return nullptr;
        }

        bool push_func(std::string name, Value *val) {
            auto result = lookup_func_table[lookup_func_table.size()-1].insert({name, val});
            return result.second;
        }

        Value* find_func(std::string name) {
            for(auto v = lookup_func_table.rbegin(); v != lookup_func_table.rend(); v++) {
                auto iter = v->find(name);
                if(iter != v->end()) return iter->second;
            }

            return nullptr;
        }

        bool push_size(std::string name, std::vector<int> size) {
            auto result = lookup_array_size_table[lookup_array_size_table.size()-1].insert({name, size});
            return result.second;
        }

        std::vector<int> find_size(std::string name) {
            for(auto v = lookup_array_size_table.rbegin(); v != lookup_array_size_table.rend(); v++) {
                auto iter = v->find(name);
                if(iter != v->end()) return iter->second;
            }

            return {};
        }

        bool push_const(std::string name, ConstantArray* array) {
            auto result = lookup_array_const_table[lookup_array_const_table.size()-1].insert({name, array});
            return result.second;
        }

        ConstantArray* find_const(std::string name) {
            for(auto v = lookup_array_const_table.rbegin(); v != lookup_array_const_table.rend(); v++) {
                auto iter = v->find(name);
                if(iter != v->end()) return iter->second;
            }

            return nullptr;
        }

  private:
    std::vector<std::map<std::string, Value*>> lookup_var_table;
    std::vector<std::map<std::string, Value*>> lookup_func_table;
    std::vector<std::map<std::string, std::vector<int>>> lookup_array_size_table;
    std::vector<std::map<std::string, ConstantArray*>> lookup_array_const_table;
};

class CminusfBuilder : public ASTVisitor {
  public:
    CminusfBuilder() {
        module = std::unique_ptr<Module>(new Module("sysY code"));
        builder = std::make_unique<IRBuilder>(nullptr, module.get());
        auto *VOID_T = module->get_void_type();
        auto *TyInt32 = module->get_int32_type();
        auto INT1_T = module->get_int1_type();
        auto *TyFloat = module->get_float_type();
        auto *INT32PTR_T = module->get_int32_ptr_type();
        auto *FLOATPTR_T = module->get_float_ptr_type();

        auto *input_type = FunctionType::get(TyInt32, {});
         auto get_int =
                Function::create(
                        input_type,
                        "getint",
                        module.get());

            input_type = FunctionType::get(TyFloat, {});
            auto get_float =
                Function::create(
                        input_type,
                        "getfloat",
                        module.get());

            input_type = FunctionType::get(TyInt32, {});
            auto get_char =
                Function::create(
                        input_type,
                        "getch",
                        module.get());

            std::vector<Type *> input_params;
            std::vector<Type *>().swap(input_params);
            input_params.push_back(INT32PTR_T);
            input_type = FunctionType::get(TyInt32, input_params);
            auto get_array =
                Function::create(
                        input_type,
                        "getarray",
                        module.get());

            std::vector<Type *>().swap(input_params);
            input_params.push_back(FLOATPTR_T);
            input_type = FunctionType::get(TyInt32, input_params);
            auto get_farray =
                Function::create(
                        input_type,
                        "getfarray",
                        module.get());

            std::vector<Type *> output_params;
            std::vector<Type *>().swap(output_params);
            output_params.push_back(TyInt32);
            auto output_type = FunctionType::get(VOID_T, output_params);
            auto put_int =
                Function::create(
                        output_type,
                        "putint",
                        module.get());

            std::vector<Type *>().swap(output_params);
            output_params.push_back(TyFloat);
            output_type = FunctionType::get(VOID_T, output_params);
            auto put_float =
                Function::create(
                        output_type,
                        "putfloat",
                        module.get());

            std::vector<Type *>().swap(output_params);
            output_params.push_back(TyInt32);
            output_type = FunctionType::get(VOID_T, output_params);
            auto put_char =
                Function::create(
                        output_type,
                        "putch",
                        module.get());

            std::vector<Type *>().swap(output_params);
            output_params.push_back(TyInt32);
            output_params.push_back(INT32PTR_T);
            output_type = FunctionType::get(VOID_T, output_params);
            auto put_array =
                Function::create(
                        output_type,
                        "putarray",
                        module.get());

            std::vector<Type *>().swap(output_params);
            output_params.push_back(TyInt32);
            output_params.push_back(FLOATPTR_T);
            output_type = FunctionType::get(VOID_T, output_params);
            auto put_farray =
                Function::create(
                        output_type,
                        "putfarray",
                        module.get());

            std::vector<Type *>().swap(input_params);
            input_params.push_back(TyInt32);
            auto time_type = FunctionType::get(VOID_T, input_params);
            auto start_time =
                Function::create(
                        time_type,
                        "_sysy_starttime",
                        module.get());

            std::vector<Type *>().swap(input_params);
            input_params.push_back(TyInt32);
            time_type = FunctionType::get(VOID_T, input_params);
            auto stop_time =
                Function::create(
                        time_type,
                        "_sysy_stoptime",
                        module.get());

            scope.enter();
            scope.push_func("getint", get_int);
            scope.push_func("getfloat", get_float);
            scope.push_func("getch", get_char);
            scope.push_func("getarray", get_array);
            scope.push_func("getfarray", get_farray);
            scope.push_func("putint", put_int);
            scope.push_func("putfloat", put_float);
            scope.push_func("putch", put_char);
            scope.push_func("putarray", put_array);
            scope.push_func("putfarray", put_farray);
            scope.push_func("starttime", start_time);
            scope.push_func("stoptime", stop_time);
    }

    std::unique_ptr<Module> getModule() { return std::move(module); }

  private:
    virtual void visit(ASTCompUnit &) override final;
        virtual void visit(ASTConstDef &) override final;
        virtual void visit(ASTConstDeclaration &) override final;
        virtual void visit(ASTConstInitVal &) override final;
        virtual void visit(ASTVarDeclaration &) override final;
        virtual void visit(ASTVarDef &) override final;
        virtual void visit(ASTInitVal &) override final;
        virtual void visit(ASTFuncDef &) override final;
        virtual void visit(ASTFuncFParam &) override final;
        virtual void visit(ASTBlock &) override final;
        virtual void visit(ASTAssignStmt &) override final;
        virtual void visit(ASTExpStmt &) override final;
        virtual void visit(ASTBlockStmt &) override final;
        virtual void visit(ASTSelectStmt &) override final;
        virtual void visit(ASTIterationStmt &) override final;
        virtual void visit(ASTBreakStmt &) override final;
        virtual void visit(ASTContinueStmt &) override final;
        virtual void visit(ASTReturnStmt &) override final;
        virtual void visit(ASTLVal &) override final;
        virtual void visit(ASTNumber &) override final;
        virtual void visit(ASTUnaryExp &) override final;
        virtual void visit(ASTCallee &) override final;
        virtual void visit(ASTMulExp &) override final;
        virtual void visit(ASTAddExp &) override final;
        virtual void visit(ASTRelExp &) override final;
        virtual void visit(ASTEqExp &) override final;
        virtual void visit(ASTLAndExp &) override final;
        virtual void visit(ASTLOrExp &) override final;
        bool is_all_zero(ASTInitVal &);

    InitZeroJudger zero_judger;
    std::unique_ptr<IRBuilder> builder;
    Scope scope;
    std::unique_ptr<Module> module;

    struct {
        // function that is being built
        Function *func = nullptr;
        // TODO: you should add more fields to store state
    } context;
};
