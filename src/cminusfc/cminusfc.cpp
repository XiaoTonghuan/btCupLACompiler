#include "include/codegen/CodeGen.hpp"
#include "DeadCode.hpp"
#include "Mem2Reg.hpp"
#include "ActiveVar.hpp"
#include "Check.hpp"
#include "CFGAnalyse.hpp"
#include "ConstProp.hpp"
#include "LIR.hpp"
#include "DeadCodeEliWithBr.hpp"
#include "StrengthReduction.hpp"
#include "LoopInvariant.hpp"
#include "AlgeSimplify.hpp"
#include "LocalComSubExprEli.hpp"
#include "FuncInline.hpp"
#include "LoopExpansion.hpp"
#include "DeadStoreEli.hpp"
#include "TailRecursionElim.hpp"
#include "InstructionScheduling.hpp"
#include "LoopSearch.hpp"
#include "LoopInfo.hpp"
#include "LoopStrengthReduction.hpp"
#include "RedundantInstsEli.hpp"
#include "AgressiveConstProp.hpp"
#include "VarLoopExpansion.hpp"
#include "AgressiveLocalComSubExprEli.hpp"
#include "RecSeq.hpp"
#include "Module.hpp"
#include "PassManager.hpp"
#include "cminusf_builder.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

using std::string;
using std::operator""s;

struct Config {
    string exe_name; // compiler exe name
    std::filesystem::path input_file;
    std::filesystem::path output_file;

    bool emitllvm{false};
    bool emitloongarch{false};
    bool mem2reg{false};

    Config(int argc, char **argv) : argc(argc), argv(argv) {
        parse_cmd_line();
        check();
    }

  private:
    int argc{-1};
    char **argv{nullptr};

    void parse_cmd_line();
    void check();
    // print helper infomation and exit
    void print_help() const;
    void print_err(const string &msg) const;
};

int main(int argc, char **argv) {
    Config config(argc, argv);
    
    std::unique_ptr<Module> m;
    {
        auto syntax_tree = parse(config.input_file.c_str());
        auto ast = AST(syntax_tree);
        CminusfBuilder builder;
        //ASTPrinter p;
        //ast.run_visitor(p);
        ast.run_visitor(builder);
        m = builder.getModule();
    }
    m->set_print_name();
    m->set_file_name(config.input_file.c_str());
    PassManager PM(m.get());
    if (config.mem2reg) {
        // PM.add_pass<DeadStoreEli>();
        // PM.add_pass<Mem2Reg>();
        // PM.add_pass<ConstProp>();
        // PM.add_pass<DeadCodeEliWithBr>();
        // PM.add_pass<RecSeq>();
        // PM.add_pass<FuncInline>();
        // PM.add_pass<DeadCodeEliWithBr>();
        // PM.add_pass<AgressiveLocalComSubExprEli>();
        // PM.add_pass<LocalComSubExprEli>();
        // PM.add_pass<AlgeSimplify>();
        // PM.add_pass<LoopInvariant>();
        // PM.add_pass<DeadCodeEliWithBr>();
        // PM.add_pass<LoopInvariant>();
        // PM.add_pass<DeadCodeEliWithBr>();
        // PM.add_pass<LoopExpansion>();
        // PM.add_pass<VarLoopExpansion>();
        // PM.add_pass<DeadCodeEliWithBr>();
        // // PM.add_pass<Check>();

        // //& 第二轮
        // PM.add_pass<ConstProp>();
        // PM.add_pass<AlgeSimplify>();
        // PM.add_pass<DeadCodeEliWithBr>();
        // PM.add_pass<LoopInvariant>();
        // PM.add_pass<DeadCodeEliWithBr>();
        // PM.add_pass<TailRecursionElim>();

        // //& 第三轮
        // PM.add_pass<LIR>();
        // // PM.add_pass<LoopStrengthReduction>();
        // PM.add_pass<LocalComSubExprEli>();
        // PM.add_pass<ConstProp>();
        // PM.add_pass<DeadStoreEli>();
        // PM.add_pass<DeadCodeEliWithBr>();
        // PM.add_pass<StrengthReduction>();
        // PM.add_pass<LocalComSubExprEli>();
        // PM.add_pass<DeadCodeEliWithBr>();
        // PM.add_pass<RedundantInstsEli>();
        // PM.add_pass<AgressiveConstProp>();
        // // PM.add_pass<InstructionScheduling>();
        // PM.add_pass<ActiveVar>();
        // PM.add_pass<CFGAnalyse>();
        
        PM.add_pass<Mem2Reg>();
        PM.add_pass<FuncInline>();
        PM.add_pass<LIR>();
        PM.add_pass<LocalComSubExprEli>();
        PM.add_pass<CFGAnalyse>();
        PM.add_pass<ActiveVar>();
    }
    PM.execute(0);
    std::ofstream output_stream(config.output_file);
    if (config.emitllvm) {
        auto abs_path = std::filesystem::canonical(config.input_file);
        output_stream << "; ModuleID = 'cminus'\n";
        output_stream << "source_filename = " << abs_path << "\n\n";
        output_stream << m->print();
    } else if (config.emitloongarch) {
        auto mptr = m.get();
        mptr->set_print_name();
        sysY_asbuilder asbuilder(mptr);
        asbuilder.module_gen();
        //std::cout<<asbuilder.get_module()->print();
        auto loongarch_code = asbuilder.get_module()->get_loongarch_code();
        output_stream << loongarch_code;
        // CodeGen codegen(m.get());
        // codegen.run();
        // output_stream << codegen.print();
        //std::cout<<asbuilder.get_module()->print();
    }

    return 0;
}

void Config::parse_cmd_line() {
    exe_name = argv[0];
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == "-h"s || argv[i] == "--help"s) {
            print_help();
        } else if (argv[i] == "-o"s) {
            if (output_file.empty() && i + 1 < argc) {
                output_file = argv[i + 1];
                i += 1;
            } else {
                print_err("bad output file");
            }
        } else if (argv[i] == "-emit-llvm"s) {
            emitllvm = true;
        } else if (argv[i] == "-S"s) {
            emitloongarch = true;
        } else if (argv[i] == "-mem2reg"s) {
            mem2reg = true;
        } else {
            if (input_file.empty()) {
                input_file = argv[i];
            } else {
                string err =
                    "unrecognized command-line option \'"s + argv[i] + "\'"s;
                print_err(err);
            }
        }
    }
}

void Config::check() {
    if (input_file.empty()) {
        print_err("no input file");
    }
    if (input_file.extension() != ".sy") {
        print_err("file format not recognized");
    }
    if (emitllvm and emitloongarch) {
        print_err("emit llvm and emit loongarch both set");
    }
    if (not emitllvm and not emitloongarch) {
        print_err("not supported: generate executable file directly");
    }
    if (output_file.empty()) {
        output_file = input_file.stem();
        if (emitllvm) {
            output_file.replace_extension(".ll");
        } else if (emitloongarch) {
            output_file.replace_extension(".s");
        }
    }
}

void Config::print_help() const {
    std::cout << "Usage: " << exe_name
              << " [-h|--help] [-o <target-file>] [-mem2reg] [-emit-llvm] [-S] "
                 "<input-file>"
              << std::endl;
    exit(0);
}

void Config::print_err(const string &msg) const {
    std::cout << exe_name << ": " << msg << std::endl;
    exit(-1);
}
