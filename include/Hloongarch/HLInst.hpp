#pragma once

#include <string>

#include "HLLoc.hpp"
#include "HLValue.hpp"

class HLBlock;
class HLFunction;

class HLInst {
public:
    enum LOpID {
        callee_save_regs,
        callee_start_stack_frame,
        callee_args_move,
        callee_restore_regs,
        callee_end_stack_frame,
        caller_save_regs,
        caller_args_move,
        caller_restore_regs,
        ld_tmp_regs,
        store_tmp_result,
        alloc_tmp_regs_with_save_initial_owner,
        restore_all_tmp_regs,
        phi_data_move,
        caller_save_call_result,
        callee_save_call_result,
        expand_stack_space,
        shrink_stack_space,
        add,
        subw,
        mulw,
        mul64,
        divw,
        remw,
        sraw,
        sllw,
        srlw,
        sra,
        sll,
        srl,
        land,
        fadds,
        fsubs,
        fmuls,
        fdivs,
        fcvtws,
        fcvtsw,
        zext,
        snez,
        seqz,
        feqs,
        fles,
        flts,
        fges,
        fgts,
        fnes,
        lw,
        flw,
        sw,
        fsw,
        memset,
        call,
        la,
        mv,
        beq,
        bne,
        bge,
        blt,
        fbeq,
        fbge,
        fbgt,
        fble,
        fblt,
        fbne,
        j,
        ret
    };
public:
    LOpID get_hloongarchinst_op() { return op_id_; }
    static std::string get_instr_op_name(LOpID id) {
        switch(id) {
            case callee_save_regs: return "callee_save_regs"; break;
            case callee_start_stack_frame: return "callee_start_stack_frame"; break;
            case callee_args_move: return "callee_args_move"; break;
            case callee_restore_regs: return "callee_restore_regs"; break;
            case callee_end_stack_frame: return "callee_end_stack_frame"; break;
            case caller_save_regs: return "caller_save_regs"; break;
            case caller_args_move: return "caller_args_move"; break;
            case caller_restore_regs: return "caller_restore_regs"; break;
            case ld_tmp_regs: return "ld_tmp_regs"; break;
            case store_tmp_result: return "store_tmp_result"; break;
            case alloc_tmp_regs_with_save_initial_owner: return "alloc_tmp_regs_with_save_initial_owner"; break;
            case restore_all_tmp_regs: return "restore_all_tmp_regs"; break;
            case phi_data_move: return "phi_data_move"; break;
            case caller_save_call_result: return "caller_save_call_result"; break;
            case callee_save_call_result: return "callee_save_call_result"; break;
            case expand_stack_space: return "expand_stack_space"; break;
            case shrink_stack_space: return "shrink_stack_space"; break;
            case add: return "add"; break;
            case subw: return "subw"; break;
            case mulw: return "mulw"; break;
            case mul64: return "mul64"; break;
            case divw: return "divw"; break;
            case remw: return "remw"; break;
            case sraw: return "sraw"; break;
            case sllw: return "sllw"; break;
            case srlw: return "srlw"; break;
            case sra: return "sra"; break;
            case sll: return "sll"; break;
            case srl: return "srl"; break;
            case land: return "land"; break;
            case fadds: return "fadds"; break;
            case fsubs: return "fsubs"; break;
            case fmuls: return "fmuls"; break;
            case fdivs: return "fdivs"; break;
            case fcvtws: return "fcvtws"; break;
            case fcvtsw: return "fcvtsw"; break;
            case zext: return "zext"; break;
            case snez: return "snez"; break;
            case seqz: return "seqz"; break;
            case feqs: return "feqs"; break;
            case fles: return "fles"; break;
            case flts: return "flts"; break;
            case fges: return "fges"; break;
            case fgts: return "fgts"; break;
            case fnes: return "fnes"; break;
            case lw: return "lw"; break;
            case flw: return "flw"; break;
            case sw: return "sw"; break;
            case fsw: return "fsw"; break; 
            case memset: return "memset"; break;
            case call: return "call"; break;
            case la: return "la"; break;
            case mv: return "mv"; break;
            case beq: return "beq"; break;
            case bne: return "bne"; break;
            case bge: return "bge"; break;
            case blt: return "blt"; break;
            case fbeq: return "fbeq"; break;
            case fbge: return "fbge"; break;
            case fbgt: return "fbgt"; break;
            case fble: return "fble"; break;
            case fblt: return "fblt"; break;
            case fbne: return "fbne"; break;
            case j: return "j"; break;
            case ret: return "ret"; break;
            default: return "unknown"; break;
        }
    }

public:
    bool is_callee_save_regs() { return op_id_ == callee_save_regs; }
    bool is_callee_start_stack_frame() { return op_id_ == callee_start_stack_frame; }
    bool is_callee_args_move() { return op_id_ == callee_args_move; }
    bool is_callee_restore_regs() { return op_id_ == callee_restore_regs; }
    bool is_callee_end_stack_frame() { return op_id_ == callee_end_stack_frame; }
    bool is_caller_save_regs() { return op_id_ == caller_save_regs; }
    bool is_caller_args_move() { return op_id_ == caller_args_move; }
    bool is_caller_restore_regs() { return op_id_ == caller_restore_regs; }
    bool is_ld_tmp_regs() { return op_id_ == ld_tmp_regs; }
    bool is_store_tmp_result() { return op_id_ == store_tmp_result; }
    bool is_alloc_tmp_regs_with_save_initial_owner() { return op_id_ == alloc_tmp_regs_with_save_initial_owner; }
    bool is_restore_all_tmp_regs() { return op_id_ == restore_all_tmp_regs; }
    bool is_phi_data_move() { return op_id_ == phi_data_move; }
    bool is_caller_save_call_result() { return op_id_ == caller_save_call_result; }
    bool is_callee_save_call_result() { return op_id_ == callee_save_call_result; }
    bool is_expand_stack_space() { return op_id_ == expand_stack_space; }
    bool is_shrink_stack_space() { return op_id_ == shrink_stack_space; }
    bool is_add() { return op_id_ == add; }
    bool is_subw() { return op_id_ == subw; }
    bool is_mulw() { return op_id_ == mulw; }
    bool is_mul64() { return op_id_ == mul64; }
    bool is_divw() { return op_id_ == divw; }
    bool is_remw() { return op_id_ == remw; }
    bool is_sraw() { return op_id_ == sraw; }
    bool is_sllw() { return op_id_ == sllw; }
    bool is_srlw() { return op_id_ == srlw; }
    bool is_sra() { return op_id_ == sra; }
    bool is_sll() { return op_id_ == sll; }
    bool is_srl() { return op_id_ == srl; }
    bool is_land() { return op_id_ == land; }
    bool is_fadds() { return op_id_ == fadds; }
    bool is_fsubs() { return op_id_ == fsubs; }
    bool is_fmuls() { return op_id_ == fmuls; }
    bool is_fdivs() { return op_id_ == fdivs; }
    bool is_fcvtws() { return op_id_ == fcvtws; }
    bool is_fcvtsw() { return op_id_ == fcvtsw; }
    bool is_zext() { return op_id_ == zext; }
    bool is_snez() { return op_id_ == snez; }
    bool is_seqz() { return op_id_ == seqz; }
    bool is_feqs() { return op_id_ == feqs; }
    bool is_fles() { return op_id_ == fles; }
    bool is_flts() { return op_id_ == flts; }
    bool is_fges() { return op_id_ == fges; }
    bool is_fgts() { return op_id_ == fgts; }
    bool is_fnes() { return op_id_ == fnes; }
    bool is_lw() { return op_id_ == lw; }
    bool is_flw() { return op_id_ == flw; }
    bool is_sw() { return op_id_ == sw; }
    bool is_fsw() { return op_id_ == fsw; }
    bool is_memset() { return op_id_ == memset; }
    bool is_call() { return op_id_ == call; }
    bool is_la() { return op_id_ == la; }
    bool is_mv() { return op_id_ == mv; }
    bool is_beq() { return op_id_ == beq; }
    bool is_bne() { return op_id_ == bne; }
    bool is_bge() { return op_id_ == bge; }
    bool is_blt() { return op_id_ == blt; }
    bool is_fbeq() { return op_id_ == fbeq; }
    bool is_fbge() { return op_id_ == fbge; }
    bool is_fbgt() { return op_id_ == fbgt; }
    bool is_fble() { return op_id_ == fble; }
    bool is_fblt() { return op_id_ == fblt; }
    bool is_fbne() { return op_id_ == fbne; }
    bool is_j() { return op_id_ == j; }
    bool is_ret() { return op_id_ == ret; }

public:
    explicit HLInst(LOpID id, HLBlock *parent): op_id_(id), parent_(parent) {}    

    HLBlock *get_parent() { return parent_; }

    HLFunction *get_hloongarch_func();

    virtual std::string get_loongarch_code() = 0;
    virtual std::string print() = 0;

private:
    LOpID op_id_;
    HLBlock *parent_;
};

class CalleeSaveRegsInst: public HLInst {
public:
    CalleeSaveRegsInst(std::vector<std::pair<RegLoc*, RegBase*>> to_save_regs, HLBlock *parent)
        : callee_save_regs_(to_save_regs), HLInst(LOpID::callee_save_regs, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    std::vector<std::pair<RegLoc*, RegBase*>> callee_save_regs_;
};

class CalleeStartStackFrameInst: public HLInst {
public:
    CalleeStartStackFrameInst(int stack_size, HLBlock *parent) 
        : stack_size_(stack_size), HLInst(LOpID::callee_start_stack_frame, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    int stack_size_;
};

class CalleeArgsMoveInst: public HLInst {
public:
    CalleeArgsMoveInst(std::vector<std::pair<HLLoc*, HLLoc*>> to_move_iargs, std::vector<std::pair<HLLoc*, HLLoc*>> to_move_fargs, HLBlock *parent) 
        : to_move_iargs_(to_move_iargs), to_move_fargs_(to_move_fargs), HLInst(LOpID::callee_args_move, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    std::vector<std::pair<HLLoc*, HLLoc*>> to_move_iargs_;
    std::vector<std::pair<HLLoc*, HLLoc*>> to_move_fargs_;
};

class CalleeRestoreRegsInst: public HLInst {
public:
    CalleeRestoreRegsInst(std::vector<std::pair<RegLoc*, RegBase*>> to_load_regs, HLBlock *parent)
        : callee_load_regs_(to_load_regs), HLInst(LOpID::callee_restore_regs, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    std::vector<std::pair<RegLoc*, RegBase*>> callee_load_regs_;   
};

class CalleeEndStackFrameInst: public HLInst {
public:
    CalleeEndStackFrameInst(int stack_size, HLBlock *parent) 
        : stack_size_(stack_size), HLInst(LOpID::callee_end_stack_frame, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    int stack_size_;
};

class CallerSaveRegsInst: public HLInst {
public:
    CallerSaveRegsInst(std::vector<std::pair<RegLoc*, RegBase*>> to_store_regs, HLBlock *parent)
        : to_store_regs_(to_store_regs), HLInst(LOpID::caller_save_regs, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    std::vector<std::pair<RegLoc*, RegBase*>> to_store_regs_;          
};

class CallerArgsMoveInst: public HLInst {
public:
    CallerArgsMoveInst(
        std::vector<std::pair<HLLoc*, HLLoc*>> to_move_fargs, 
        std::vector<std::pair<HLLoc*, HLLoc*>> to_move_iargs, 
        HLBlock *parent
    ) : to_move_fargs_(to_move_fargs), to_move_iargs_(to_move_iargs), HLInst(LOpID::caller_args_move, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    std::vector<std::pair<HLLoc*, HLLoc*>> to_move_fargs_;
    std::vector<std::pair<HLLoc*, HLLoc*>> to_move_iargs_;
};

class CallerRestoreRegsInst: public HLInst {
public:
    CallerRestoreRegsInst(std::vector<std::pair<RegLoc*, RegBase*>> to_restore_regs, HLBlock *parent)
        : to_restore_regs_(to_restore_regs), HLInst(LOpID::caller_restore_regs, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    std::vector<std::pair<RegLoc*, RegBase*>> to_restore_regs_; 
};

class LdTmpRegsInst: public HLInst {
public:
    LdTmpRegsInst(std::vector<std::pair<RegLoc*, RegBase*>> to_load_tmp_regs, HLBlock *parent)
        : to_ld_tmp_regs_(to_load_tmp_regs), HLInst(LOpID::ld_tmp_regs, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    std::vector<std::pair<RegLoc*, RegBase*>> to_ld_tmp_regs_;  
};

class StoreTmpResultInst: public HLInst {
public:
    StoreTmpResultInst(std::vector<std::pair<RegLoc*, RegBase*>> to_store_regs, HLBlock *parent)
        : to_store_regs_(to_store_regs), HLInst(LOpID::store_tmp_result, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    std::vector<std::pair<RegLoc*, RegBase*>> to_store_regs_;  
};

class AllocTmpRegsWithSaveInitialOwnerInst: public HLInst {
public:
    AllocTmpRegsWithSaveInitialOwnerInst(std::vector<std::pair<RegLoc*, RegBase*>> to_store_regs, std::vector<std::pair<RegLoc*, RegBase*>> to_ld_regs, HLBlock *parent)
        : to_store_regs_(to_store_regs), to_ld_regs_(to_ld_regs), HLInst(LOpID::alloc_tmp_regs_with_save_initial_owner, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    std::vector<std::pair<RegLoc*, RegBase*>> to_store_regs_;
    std::vector<std::pair<RegLoc*, RegBase*>> to_ld_regs_;  
};

class RestoreAllTmpRegsInst: public HLInst {
public:
    RestoreAllTmpRegsInst(std::vector<std::pair<RegLoc*, RegBase*>> to_restore_regs, HLBlock *parent)
        : to_restore_regs_(to_restore_regs), HLInst(LOpID::restore_all_tmp_regs, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    std::vector<std::pair<RegLoc*, RegBase*>> to_restore_regs_; 
};

class PhiDataMoveInst: public HLInst {
public:
    PhiDataMoveInst(
        std::vector<std::pair<HLLoc*, HLLoc*>> to_move_ilocs, 
        std::vector<std::pair<HLLoc*, HLLoc*>> to_move_flocs, 
        HLBlock *parent):
        to_move_ilocs_(to_move_ilocs), to_move_flocs_(to_move_flocs), HLInst(LOpID::phi_data_move, parent) {}

    std::string get_loongarch_code();
    std::string print();
private:
    std::vector<std::pair<HLLoc*, HLLoc*>> to_move_ilocs_;
    std::vector<std::pair<HLLoc*, HLLoc*>> to_move_flocs_; 
};

class CallerSaveCallResultInst: public HLInst {
public:
    CallerSaveCallResultInst(HLLoc *dst, Reg *src, HLBlock *parent):
        dst_(dst), src_(src), HLInst(LOpID::caller_save_call_result, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    HLLoc *dst_;
    Reg *src_;
};

class CalleeSaveCallResultInst: public HLInst {
public:
    CalleeSaveCallResultInst(RegLoc *dst, HLVal *src, HLBlock *parent):
        dst_(dst), src_(src), HLInst(LOpID::callee_save_call_result, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    RegLoc *dst_;
    HLVal *src_;
};

class ExpandStackSpaceInst: public HLInst {
public:
    ExpandStackSpaceInst(int expand_size, HLBlock *parent) : expand_size_(expand_size), HLInst(LOpID::expand_stack_space, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    int expand_size_;
};

class ShrinkStackSpaceInst: public HLInst {
public:
    ShrinkStackSpaceInst(int shrink_size, HLBlock *parent) : shrink_size_(shrink_size), HLInst(LOpID::shrink_stack_space, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    int shrink_size_;
};


class HLAddInst: public HLInst {
public:
    HLAddInst(Reg *dst, HLVal *rs1, HLVal *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::add, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *rs1_;
    HLVal *rs2_;
};


class HLSubwInst: public HLInst {
public:
    HLSubwInst(Reg *dst, HLVal *rs1, HLVal *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::subw, parent) {}

    std::string get_loongarch_code();
    std::string print();
private:
    Reg *dst_;
    HLVal *rs1_;
    HLVal *rs2_;
};

class HLMulwInst: public HLInst {
public:
    HLMulwInst(Reg *dst, HLVal *rs1, HLVal *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::mulw, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *rs1_;
    HLVal *rs2_;
};

class HLMul64Inst: public HLInst {
public:
    HLMul64Inst(Reg *dst, HLVal *rs1, HLVal *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::mul64, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *rs1_;
    HLVal *rs2_;
};

class HLDivwInst: public HLInst {
public:
    HLDivwInst(Reg *dst, HLVal *rs1, HLVal *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::divw, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *rs1_;
    HLVal *rs2_;
};

class HLRemwInst: public HLInst {
public:
    HLRemwInst(Reg *dst, HLVal *rs1, HLVal *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::remw, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *rs1_;
    HLVal *rs2_;
};

class HLSrawInst: public HLInst {
public:
    HLSrawInst(Reg *dst, HLVal *rs1, Const *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::sraw, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *rs1_;
    Const *rs2_;
};

class HLSllwInst: public HLInst {
public:
    HLSllwInst(Reg *dst, HLVal *rs1, Const *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::sllw, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *rs1_;
    Const *rs2_;
};

class HLSrlwInst: public HLInst {
public:
    HLSrlwInst(Reg *dst, HLVal *rs1, Const *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::srlw, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *rs1_;
    Const *rs2_;
};


class HLSraInst: public HLInst {
public:
    HLSraInst(Reg *dst, HLVal *rs1, Const *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::sra, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *rs1_;
    Const *rs2_;
};

class HLSllInst: public HLInst {
public:
    HLSllInst(Reg *dst, HLVal *rs1, Const *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::sll, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *rs1_;
    Const *rs2_;
};

class HLSrlInst: public HLInst {
public:
    HLSrlInst(Reg *dst, HLVal *rs1, Const *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::srl, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *rs1_;
    Const *rs2_;
};

class HLLandInst: public HLInst {
public:
    HLLandInst(Reg *dst, Reg *rs1, Const *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::land, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    Reg *rs1_;
    Const *rs2_;
};

class HLFaddsInst: public HLInst {
public:
    HLFaddsInst(Reg *dst, HLVal *rs1, HLVal *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::fadds, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *rs1_;
    HLVal *rs2_;
};

class HLFsubsInst: public HLInst {
public:
    HLFsubsInst(Reg *dst, HLVal *rs1, HLVal *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::fsubs, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *rs1_;
    HLVal *rs2_;
};

class HLFmulsInst: public HLInst {
public:
    HLFmulsInst(Reg *dst, HLVal *rs1, HLVal *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::fmuls, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *rs1_;
    HLVal *rs2_;
};

class HLFdivsInst: public HLInst {
public:
    HLFdivsInst(Reg *dst, HLVal *rs1, HLVal *rs2, HLBlock *parent) : 
        dst_(dst), rs1_(rs1), rs2_(rs2), HLInst(LOpID::fdivs, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *rs1_;
    HLVal *rs2_;
};

class HLFcvtwsInst: public HLInst {
public:
    HLFcvtwsInst(Reg *dst, HLVal *src, HLBlock *parent) : 
        dst_(dst), src_(src), HLInst(LOpID::fcvtws, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *src_;
};

class HLFcvtswInst: public HLInst {
public:
    HLFcvtswInst(Reg *dst, HLVal *src, HLBlock *parent) : 
        dst_(dst), src_(src), HLInst(LOpID::fcvtsw, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *src_;
};

class HLZextInst: public HLInst {
public:
    HLZextInst(Reg *dst, Reg *src, HLBlock *parent) : 
        dst_(dst), src_(src), HLInst(LOpID::zext, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    Reg *src_;
};

class HLSnezInst: public HLInst {
public:
    HLSnezInst(Reg *dst, HLVal *cond, HLBlock *parent) : 
        dst_(dst), cond_(cond), HLInst(LOpID::snez, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *cond_;
};

class HLSeqzInst: public HLInst {
public:
    HLSeqzInst(Reg *dst, HLVal *cond, HLBlock *parent) : 
        dst_(dst), cond_(cond), HLInst(LOpID::seqz, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *cond_;
};

class HLFeqsInst: public HLInst {
public:
    HLFeqsInst(Reg *dst, HLVal *cond1, HLVal *cond2, HLBlock *parent) : 
        dst_(dst), cond1_(cond1), cond2_(cond2), HLInst(LOpID::feqs, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *cond1_;
    HLVal *cond2_;
};

class HLFlesInst: public HLInst {
public:
    HLFlesInst(Reg *dst, HLVal *cond1, HLVal *cond2, HLBlock *parent) : 
        dst_(dst), cond1_(cond1), cond2_(cond2), HLInst(LOpID::fles, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *cond1_;
    HLVal *cond2_;
};

class HLFltsInst: public HLInst {
public:
    HLFltsInst(Reg *dst, HLVal *cond1, HLVal *cond2, HLBlock *parent) : 
        dst_(dst), cond1_(cond1), cond2_(cond2), HLInst(LOpID::flts, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *cond1_;
    HLVal *cond2_;
};

class HLFgesInst: public HLInst {
public:
    HLFgesInst(Reg *dst, HLVal *cond1, HLVal *cond2, HLBlock *parent) : 
        dst_(dst), cond1_(cond1), cond2_(cond2), HLInst(LOpID::fges, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *cond1_;
    HLVal *cond2_;
};

class HLFgtsInst: public HLInst {
public:
    HLFgtsInst(Reg *dst, HLVal *cond1, HLVal *cond2, HLBlock *parent) : 
        dst_(dst), cond1_(cond1), cond2_(cond2), HLInst(LOpID::fgts, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *cond1_;
    HLVal *cond2_;
};

class HLFnesInst: public HLInst {
public:
    HLFnesInst(Reg *dst, HLVal *cond1, HLVal *cond2, HLBlock *parent) : 
        dst_(dst), cond1_(cond1), cond2_(cond2), HLInst(LOpID::fnes, parent) {}

    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    HLVal *cond1_;
    HLVal *cond2_;
};

class HLLwInst: public HLInst {
public:
    HLLwInst(Reg *dst, Label *label, HLBlock *parent): 
        dst_(dst), label_(label), is_label_(true), HLInst(LOpID::lw, parent) {}

    HLLwInst(Reg *dst, Reg *base, HLVal *offset, HLBlock *parent): 
        dst_(dst), base_(base), offset_(offset), is_label_(false), HLInst(LOpID::lw, parent) {}
    bool is_label() { return is_label_; }
    std::string get_loongarch_code();
    std::string print();

private:
    Reg *dst_;
    Reg *base_;
    HLVal *offset_;
    Label *label_;
    bool is_label_;

};

class HLFlwInst: public HLInst {
public:
    HLFlwInst(Reg *dst, Label *label, HLBlock *parent): 
        dst_(dst), label_(label), is_label_(true), HLInst(LOpID::flw, parent) {}

    HLFlwInst(Reg *dst, Reg *base, HLVal *offset, HLBlock *parent): 
        dst_(dst), base_(base), offset_(offset), is_label_(false), HLInst(LOpID::flw, parent) {}
    bool is_label() { return is_label_; }
    std::string get_loongarch_code();
    std::string print();
    
private:
    Reg *dst_;
    Reg *base_;
    HLVal *offset_;
    Label *label_;
    bool is_label_;
};

class HLSwInst: public HLInst {
public:
    HLSwInst(HLVal *val, Label *label, HLBlock *parent): 
        val_(val), label_(label), is_label_(true), HLInst(LOpID::sw, parent) {}

    HLSwInst(HLVal *val, Reg *base, HLVal *offset, HLBlock *parent): 
        val_(val), base_(base), offset_(offset), is_label_(false), HLInst(LOpID::sw, parent) {}
    bool is_label() { return is_label_; }
    std::string get_loongarch_code();
    std::string print();

private:
    HLVal *val_;
    Reg *base_;
    HLVal *offset_;
    Label *label_;
    bool is_label_;
};

class HLFswInst: public HLInst {
public:
    HLFswInst(HLVal *val, Label *label, HLBlock *parent): 
        val_(val), label_(label), is_label_(true), HLInst(LOpID::fsw, parent) {}

    HLFswInst(HLVal *val, Reg *base, HLVal *offset, HLBlock *parent): 
        val_(val), base_(base), offset_(offset), is_label_(false), HLInst(LOpID::fsw, parent) {}
    bool is_label() { return is_label_; }
    std::string get_loongarch_code();
    std::string print();

private:
    HLVal *val_;
    Reg *base_;
    HLVal *offset_;
    Label *label_;
    bool is_label_;
};

class HLMemsetInst: public HLInst {
public:
    HLMemsetInst(RegBase *base, int total_size, int step_size, bool is_fp, HLBlock *parent):
        base_(base), total_size_(total_size), step_size_(step_size), is_fp_(is_fp), HLInst(LOpID::memset, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    RegBase *base_;
    int total_size_;
    int step_size_;
    bool is_fp_;
};

class HLCallInst: public HLInst {
public:
    HLCallInst(Label *label, HLBlock *parent):
        label_(label), HLInst(LOpID::call, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    Label *label_;
};

class HLLaInst: public HLInst {
public:
    HLLaInst(Reg *dst, Label *label, HLBlock *parent):
        dst_(dst), label_(label), HLInst(LOpID::la, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    Reg *dst_; 
    Label *label_;    
};

class HLMvInst: public HLInst {
public:
    HLMvInst(Reg *dst, Reg *src, HLBlock *parent):
        dst_(dst), src_(src), HLInst(LOpID::mv, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    Reg *dst_; 
    Reg *src_;  
};

class HLBeqInst: public HLInst {
public:
    HLBeqInst(HLVal *lval, HLVal *rval, Label *label, HLBlock *parent):
        lval_(lval), rval_(rval), label_(label), HLInst(LOpID::beq, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    HLVal *lval_;
    HLVal *rval_;
    Label *label_;
};

class HLBneInst: public HLInst {
public:
    HLBneInst(HLVal *lval, HLVal *rval, Label *label, HLBlock *parent):
        lval_(lval), rval_(rval), label_(label), HLInst(LOpID::bne, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    HLVal *lval_;
    HLVal *rval_;
    Label *label_;
};

class HLBgeInst: public HLInst {
public:
    HLBgeInst(HLVal *lval, HLVal *rval, Label *label, HLBlock *parent):
        lval_(lval), rval_(rval), label_(label), HLInst(LOpID::bge, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    HLVal *lval_;
    HLVal *rval_;
    Label *label_;
};

class HLBltInst: public HLInst {
public:
    HLBltInst(HLVal *lval, HLVal *rval, Label *label, HLBlock *parent):
        lval_(lval), rval_(rval), label_(label), HLInst(LOpID::blt, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    HLVal *lval_;
    HLVal *rval_;
    Label *label_;
};

class HLFBeqInst: public HLInst {
public:
    HLFBeqInst(HLVal *lval, HLVal *rval, Label *label, HLBlock *parent):
        lval_(lval), rval_(rval), label_(label), HLInst(LOpID::fbeq, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    HLVal *lval_;
    HLVal *rval_;
    Label *label_;
};

class HLFBgeInst: public HLInst {
public:
    HLFBgeInst(HLVal *lval, HLVal *rval, Label *label, HLBlock *parent):
        lval_(lval), rval_(rval), label_(label), HLInst(LOpID::fbge, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    HLVal *lval_;
    HLVal *rval_;
    Label *label_;
};

class HLFBgtInst: public HLInst {
public:
    HLFBgtInst(HLVal *lval, HLVal *rval, Label *label, HLBlock *parent):
        lval_(lval), rval_(rval), label_(label), HLInst(LOpID::fbgt, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    HLVal *lval_;
    HLVal *rval_;
    Label *label_;
};

class HLFBleInst: public HLInst {
public:
    HLFBleInst(HLVal *lval, HLVal *rval, Label *label, HLBlock *parent):
        lval_(lval), rval_(rval), label_(label), HLInst(LOpID::fble, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    HLVal *lval_;
    HLVal *rval_;
    Label *label_;
};

class HLFBltInst: public HLInst {
public:
    HLFBltInst(HLVal *lval, HLVal *rval, Label *label, HLBlock *parent):
        lval_(lval), rval_(rval), label_(label), HLInst(LOpID::fblt, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    HLVal *lval_;
    HLVal *rval_;
    Label *label_;
};

class HLFBneInst: public HLInst {
public:
    HLFBneInst(HLVal *lval, HLVal *rval, Label *label, HLBlock *parent):
        lval_(lval), rval_(rval), label_(label), HLInst(LOpID::fbne, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    HLVal *lval_;
    HLVal *rval_;
    Label *label_;
};

class HLJInst: public HLInst {
public:
    HLJInst(Label *label, HLBlock *parent):
        label_(label), HLInst(LOpID::j, parent) {}
    std::string get_loongarch_code();
    std::string print();
private:
    Label *label_;
};

class HLRetInst: public HLInst {
public:
    HLRetInst(HLBlock *parent): HLInst(LOpID::ret, parent) {}
    std::string get_loongarch_code();
    std::string print();
};