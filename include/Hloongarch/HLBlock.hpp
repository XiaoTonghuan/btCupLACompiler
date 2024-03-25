#pragma once


#include "HLModule.hpp"
#include "HLFunction.hpp"
#include "HLInst.hpp"
#include "HLLoc.hpp"

#include "BasicBlock.hpp"



class HLBlock {
public:
    explicit HLBlock(HLFunction *parent, BasicBlock *bb, Label *label): parent_(parent), bb_(bb), label_(label) {}
    
public:

    CalleeSaveRegsInst *create_callee_save_regs(std::vector<std::pair<RegLoc*, RegBase*>> to_save_regs);

    CalleeStartStackFrameInst *create_callee_start_stack_frame(int stack_size);

    CalleeArgsMoveInst *create_callee_args_move(
        std::vector<std::pair<HLLoc*, HLLoc*>> to_move_iargs, 
        std::vector<std::pair<HLLoc*, HLLoc*>> to_move_fargs
    );

    CalleeEndStackFrameInst *create_callee_end_stack_frame(int stack_size);

    CalleeRestoreRegsInst *create_callee_restore_regs(std::vector<std::pair<RegLoc*, RegBase*>> to_load_regs);

    LdTmpRegsInst *create_ld_tmp_regs(std::vector<std::pair<RegLoc*, RegBase*>> to_load_tmp_regs);

    AllocTmpRegsWithSaveInitialOwnerInst *create_alloc_tmp_regs_with_save_initial_owner(
        std::vector<std::pair<RegLoc*, RegBase*>> to_store_regs,
        std::vector<std::pair<RegLoc*, RegBase*>> to_ld_regs
    );

    StoreTmpResultInst *create_store_tmp_result(std::vector<std::pair<RegLoc*, RegBase*>> to_store_regs);

    RestoreAllTmpRegsInst *create_restore_all_tmp_regs(std::vector<std::pair<RegLoc*, RegBase*>> to_restore_regs);

    CallerSaveRegsInst *create_caller_save_regs(std::vector<std::pair<RegLoc*, RegBase*>> to_store_regs);

    CallerRestoreRegsInst *create_caller_restore_regs(std::vector<std::pair<RegLoc*, RegBase*>> to_restore_regs);

    CallerArgsMoveInst *create_caller_args_move(
        std::vector<std::pair<HLLoc*, HLLoc*>> to_move_fargs, 
        std::vector<std::pair<HLLoc*, HLLoc*>> to_move_iargs
    );

    ExpandStackSpaceInst *create_expand_stack_space(int expand_size);

    ShrinkStackSpaceInst *create_shrink_stack_space(int shrink_size);

    PhiDataMoveInst *create_phi_data_move(
        std::vector<std::pair<HLLoc*, HLLoc*>> to_move_ilocs, 
        std::vector<std::pair<HLLoc*, HLLoc*>> to_move_flocs
    );

    HLFBeqInst *create_fbeq(HLVal *lval, HLVal *rval, Label *label);
    HLFBgeInst *create_fbge(HLVal *lval, HLVal *rval, Label *label);
    HLFBgtInst *create_fbgt(HLVal *lval, HLVal *rval, Label *label);
    HLFBleInst *create_fble(HLVal *lval, HLVal *rval, Label *label);
    HLFBltInst *create_fblt(HLVal *lval, HLVal *rval, Label *label);
    HLFBneInst *create_fbne(HLVal *lval, HLVal *rval, Label *label);

    HLBeqInst *create_beq(HLVal *lval, HLVal *rval, Label *label);
    HLBgeInst *create_bge(HLVal *lval, HLVal *rval, Label *label);
    HLBltInst *create_blt(HLVal *lval, HLVal *rval, Label *label);
    HLBneInst *create_bne(HLVal *lval, HLVal *rval, Label *label);

    HLJInst *create_j(Label *label);

    HLAddInst *create_add(Reg *dst, HLVal *rs1, HLVal *rs2);
    HLSubwInst *create_subw(Reg *dst, HLVal *rs1, HLVal *rs2);
    HLMulwInst *create_mulw(Reg *dst, HLVal *rs1, HLVal *rs2);
    HLMul64Inst *create_mul64(Reg *dst, HLVal *rs1, HLVal *rs2);
    HLDivwInst *create_divw(Reg *dst, HLVal *rs1, HLVal *rs2);
    HLRemwInst *create_remw(Reg *dst, HLVal *rs1, HLVal *rs2);

    HLSrawInst *create_sraw(Reg *dst, HLVal *rs1, Const *rs2);
    HLSllwInst *create_sllw(Reg *dst, HLVal *rs1, Const *rs2);
    HLSrlwInst *create_srlw(Reg *dst, HLVal *rs1, Const *rs2);

    HLSraInst *create_sra(Reg *dst, HLVal *rs1, Const *rs2);
    HLSllInst *create_sll(Reg *dst, HLVal *rs1, Const *rs2);
    HLSrlInst *create_srl(Reg *dst, HLVal *rs1, Const *rs2);

    HLLandInst *create_land(Reg *dst, Reg *rs1, Const *rs2);

    HLFaddsInst *create_fadds(Reg *dst, HLVal *rs1, HLVal *rs2);
    HLFsubsInst *create_fsubs(Reg *dst, HLVal *rs1, HLVal *rs2);
    HLFmulsInst *create_fmuls(Reg *dst, HLVal *rs1, HLVal *rs2);
    HLFdivsInst *create_fdivs(Reg *dst, HLVal *rs1, HLVal *rs2);

    HLFcvtwsInst *create_fcvtws(Reg *dst, HLVal *src);
    HLFcvtswInst *create_fcvtsw(Reg *dst, HLVal *src);

    HLZextInst *create_zext(Reg *dst, Reg *src);

    HLSeqzInst *create_seqz(Reg *dst, HLVal *cond);
    HLSnezInst *create_snez(Reg *dst, HLVal *cond);

    HLFeqsInst *create_feqs(Reg *dst, HLVal *cond1, HLVal *cond2);
    HLFgesInst *create_fges(Reg *dst, HLVal *cond1, HLVal *cond2);
    HLFgtsInst *create_fgts(Reg *dst, HLVal *cond1, HLVal *cond2);
    HLFlesInst *create_fles(Reg *dst, HLVal *cond1, HLVal *cond2);
    HLFltsInst *create_flts(Reg *dst, HLVal *cond1, HLVal *cond2);
    HLFnesInst *create_fnes(Reg *dst, HLVal *cond1, HLVal *cond2);

    HLFlwInst *create_flw(Reg *dst, Label *label);
    HLFlwInst *create_flw(Reg *dst, Reg *base, HLVal *offset);
    HLLwInst *create_lw(Reg *dst, Label *label);
    HLLwInst *create_lw(Reg *dst, Reg *base, HLVal *offset);

    HLFswInst *create_fsw(HLVal *val, Label *label);
    HLFswInst *create_fsw(HLVal *val, Reg *base, HLVal *offset); 
    HLSwInst *create_sw(HLVal *val, Label *label);
    HLSwInst *create_sw(HLVal *val, Reg *base, HLVal *offset); 

    HLCallInst *create_call(Label *label);

    CallerSaveCallResultInst *create_caller_save_call_result(HLLoc *dst, Reg* src);
    CalleeSaveCallResultInst *create_callee_save_call_result(RegLoc* dst, HLVal *src);
    HLMemsetInst *create_memset(RegBase *base, int total_size, int step_size, bool is_fp); 

    HLLaInst *create_la(Reg *dst, Label *label);

    HLMvInst *create_mv(Reg *dst, Reg *src);

    HLRetInst *create_ret();

    void pop_inst() {
        insts_list_.pop_back();
    }

    void push_inst(HLInst *inst) {
        insts_list_.push_back(inst);
    }

    BasicBlock *get_bb() { return bb_; }

    HLFunction *get_parent() { return parent_; }

    std::string get_loongarch_code();
    std::string print();

private:
    std::list<HLInst*> insts_list_;
    HLFunction *parent_;
    BasicBlock *bb_;
    Label *label_;
};