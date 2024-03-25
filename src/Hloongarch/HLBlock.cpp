#include "../include/Hloongarch/HLBlock.hpp"
#include "../include/Hloongarch/HLValue.hpp"
#include "../include/Hloongarch/H2L.hpp"

#include "logging.hpp"

CalleeSaveRegsInst *HLBlock::create_callee_save_regs(std::vector<std::pair<RegLoc*, RegBase*>> to_save_regs) {
    auto new_inst = new CalleeSaveRegsInst(to_save_regs, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

CalleeStartStackFrameInst *HLBlock::create_callee_start_stack_frame(int stack_size) {
    auto new_inst = new CalleeStartStackFrameInst(stack_size, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

CalleeArgsMoveInst *HLBlock::create_callee_args_move(std::vector<std::pair<HLLoc*, HLLoc*>> to_move_iargs, std::vector<std::pair<HLLoc*, HLLoc*>> to_move_fargs) {
    auto new_inst = new CalleeArgsMoveInst(to_move_iargs, to_move_fargs, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

CalleeEndStackFrameInst *HLBlock::create_callee_end_stack_frame(int stack_size) {
    auto new_inst = new CalleeEndStackFrameInst(stack_size, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

CalleeRestoreRegsInst *HLBlock::create_callee_restore_regs(std::vector<std::pair<RegLoc*, RegBase*>> to_load_regs) {
    auto new_inst = new CalleeRestoreRegsInst(to_load_regs, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

LdTmpRegsInst *HLBlock::create_ld_tmp_regs(std::vector<std::pair<RegLoc*, RegBase*>> to_load_tmp_regs) {
    auto new_inst = new LdTmpRegsInst(to_load_tmp_regs, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

AllocTmpRegsWithSaveInitialOwnerInst *HLBlock::create_alloc_tmp_regs_with_save_initial_owner(
    std::vector<std::pair<RegLoc*, RegBase*>> to_store_regs,
    std::vector<std::pair<RegLoc*, RegBase*>> to_ld_regs
) {
    auto new_inst = new AllocTmpRegsWithSaveInitialOwnerInst(to_store_regs, to_ld_regs, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

StoreTmpResultInst *HLBlock::create_store_tmp_result(std::vector<std::pair<RegLoc*, RegBase*>> to_store_regs) {
    auto new_inst = new StoreTmpResultInst(to_store_regs, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

RestoreAllTmpRegsInst *HLBlock::create_restore_all_tmp_regs(std::vector<std::pair<RegLoc*, RegBase*>> to_restore_regs) {
    auto new_inst = new RestoreAllTmpRegsInst(to_restore_regs, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

CallerSaveRegsInst *HLBlock::create_caller_save_regs(std::vector<std::pair<RegLoc*, RegBase*>> to_store_regs) {
    auto new_inst = new CallerSaveRegsInst(to_store_regs, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

CallerRestoreRegsInst *HLBlock::create_caller_restore_regs(std::vector<std::pair<RegLoc*, RegBase*>> to_restore_regs) {
    auto new_inst = new CallerRestoreRegsInst(to_restore_regs, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

CallerArgsMoveInst *HLBlock::create_caller_args_move(
    std::vector<std::pair<HLLoc*, HLLoc*>> to_move_fargs, 
    std::vector<std::pair<HLLoc*, HLLoc*>> to_move_iargs
) {
    auto new_inst = new CallerArgsMoveInst(to_move_fargs, to_move_iargs, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

ExpandStackSpaceInst *HLBlock::create_expand_stack_space(int expand_size) {
    auto new_inst = new ExpandStackSpaceInst(expand_size, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

ShrinkStackSpaceInst *HLBlock::create_shrink_stack_space(int shrink_size) {
    auto new_inst = new ShrinkStackSpaceInst(shrink_size, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

PhiDataMoveInst *HLBlock::create_phi_data_move(
    std::vector<std::pair<HLLoc*, HLLoc*>> to_move_ilocs, 
    std::vector<std::pair<HLLoc*, HLLoc*>> to_move_flocs
) {
    auto new_inst = new PhiDataMoveInst(to_move_ilocs, to_move_flocs, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

HLFBeqInst *HLBlock::create_fbeq(HLVal *lval, HLVal *rval, Label *label) {
    auto new_inst = new HLFBeqInst(lval, rval, label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLFBgeInst *HLBlock::create_fbge(HLVal *lval, HLVal *rval, Label *label) {
    auto new_inst = new HLFBgeInst(lval, rval, label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLFBgtInst *HLBlock::create_fbgt(HLVal *lval, HLVal *rval, Label *label) {
    auto new_inst = new HLFBgtInst(lval, rval, label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLFBleInst *HLBlock::create_fble(HLVal *lval, HLVal *rval, Label *label) {
    auto new_inst = new HLFBleInst(lval, rval, label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLFBltInst *HLBlock::create_fblt(HLVal *lval, HLVal *rval, Label *label) {
    auto new_inst = new HLFBltInst(lval, rval, label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLFBneInst *HLBlock::create_fbne(HLVal *lval, HLVal *rval, Label *label) {
    auto new_inst = new HLFBneInst(lval, rval, label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

HLBeqInst *HLBlock::create_beq(HLVal *lval, HLVal *rval, Label *label) {
    auto new_inst = new HLBeqInst(lval, rval, label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLBgeInst *HLBlock::create_bge(HLVal *lval, HLVal *rval, Label *label) {
    auto new_inst = new HLBgeInst(lval, rval, label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLBltInst *HLBlock::create_blt(HLVal *lval, HLVal *rval, Label *label) {
    auto new_inst = new HLBltInst(lval, rval, label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLBneInst *HLBlock::create_bne(HLVal *lval, HLVal *rval, Label *label) {
    auto new_inst = new HLBneInst(lval, rval, label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

HLJInst *HLBlock::create_j(Label *label) {
    auto new_inst = new HLJInst(label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

HLAddInst *HLBlock::create_add(Reg *dst, HLVal *rs1, HLVal *rs2) {
    auto new_inst = new HLAddInst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLSubwInst *HLBlock::create_subw(Reg *dst, HLVal *rs1, HLVal *rs2) {
    auto new_inst = new HLSubwInst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLMulwInst *HLBlock::create_mulw(Reg *dst, HLVal *rs1, HLVal *rs2) {
    auto new_inst = new HLMulwInst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLMul64Inst *HLBlock::create_mul64(Reg *dst, HLVal *rs1, HLVal *rs2) {
    auto new_inst = new HLMul64Inst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLDivwInst *HLBlock::create_divw(Reg *dst, HLVal *rs1, HLVal *rs2) {
    auto new_inst = new HLDivwInst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLRemwInst *HLBlock::create_remw(Reg *dst, HLVal *rs1, HLVal *rs2) {
    auto new_inst = new HLRemwInst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

HLSrawInst *HLBlock::create_sraw(Reg *dst, HLVal *rs1, Const *rs2) {
    auto new_inst = new HLSrawInst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLSllwInst *HLBlock::create_sllw(Reg *dst, HLVal *rs1, Const *rs2) {
    auto new_inst = new HLSllwInst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLSrlwInst *HLBlock::create_srlw(Reg *dst, HLVal *rs1, Const *rs2) {
    auto new_inst = new HLSrlwInst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

HLSraInst *HLBlock::create_sra(Reg *dst, HLVal *rs1, Const *rs2) {
    auto new_inst = new HLSraInst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLSllInst *HLBlock::create_sll(Reg *dst, HLVal *rs1, Const *rs2) {
    auto new_inst = new HLSllInst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLSrlInst *HLBlock::create_srl(Reg *dst, HLVal *rs1, Const *rs2) {
    auto new_inst = new HLSrlInst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

HLLandInst *HLBlock::create_land(Reg *dst, Reg *rs1, Const *rs2) {
    auto new_inst = new HLLandInst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

HLFaddsInst *HLBlock::create_fadds(Reg *dst, HLVal *rs1, HLVal *rs2) {
    auto new_inst = new HLFaddsInst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLFsubsInst *HLBlock::create_fsubs(Reg *dst, HLVal *rs1, HLVal *rs2) {
    auto new_inst = new HLFsubsInst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLFmulsInst *HLBlock::create_fmuls(Reg *dst, HLVal *rs1, HLVal *rs2) {
    auto new_inst = new HLFmulsInst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLFdivsInst *HLBlock::create_fdivs(Reg *dst, HLVal *rs1, HLVal *rs2) {
    auto new_inst = new HLFdivsInst(dst, rs1, rs2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

HLFcvtwsInst *HLBlock::create_fcvtws(Reg *dst, HLVal *src) {
    auto new_inst = new HLFcvtwsInst(dst, src, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLFcvtswInst *HLBlock::create_fcvtsw(Reg *dst, HLVal *src) {
    auto new_inst = new HLFcvtswInst(dst, src, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

HLZextInst *HLBlock::create_zext(Reg *dst, Reg *src) {
    auto new_inst = new HLZextInst(dst, src, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

HLSeqzInst *HLBlock::create_seqz(Reg *dst, HLVal *cond) {
    auto new_inst = new HLSeqzInst(dst, cond, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLSnezInst *HLBlock::create_snez(Reg *dst, HLVal *cond) {
    auto new_inst = new HLSnezInst(dst, cond, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

HLFeqsInst *HLBlock::create_feqs(Reg *dst, HLVal *cond1, HLVal *cond2) {
    auto new_inst = new HLFeqsInst(dst, cond1, cond2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLFgesInst *HLBlock::create_fges(Reg *dst, HLVal *cond1, HLVal *cond2) {
    auto new_inst = new HLFgesInst(dst, cond1, cond2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLFgtsInst *HLBlock::create_fgts(Reg *dst, HLVal *cond1, HLVal *cond2) {
    auto new_inst = new HLFgtsInst(dst, cond1, cond2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLFlesInst *HLBlock::create_fles(Reg *dst, HLVal *cond1, HLVal *cond2) {
    auto new_inst = new HLFlesInst(dst, cond1, cond2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLFltsInst *HLBlock::create_flts(Reg *dst, HLVal *cond1, HLVal *cond2) {
    auto new_inst = new HLFltsInst(dst, cond1, cond2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLFnesInst *HLBlock::create_fnes(Reg *dst, HLVal *cond1, HLVal *cond2) {
    auto new_inst = new HLFnesInst(dst, cond1, cond2, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

HLFlwInst *HLBlock::create_flw(Reg *dst, Label *label) {
    auto new_inst = new HLFlwInst(dst, label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLFlwInst *HLBlock::create_flw(Reg *dst, Reg *base, HLVal *offset) {
    auto new_inst = new HLFlwInst(dst, base, offset, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLLwInst *HLBlock::create_lw(Reg *dst, Label *label) {
    auto new_inst = new HLLwInst(dst, label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLLwInst *HLBlock::create_lw(Reg *dst, Reg *base, HLVal *offset) {
    auto new_inst = new HLLwInst(dst, base, offset, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

HLFswInst *HLBlock::create_fsw(HLVal *val, Label *label) {
    auto new_inst = new HLFswInst(val, label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLFswInst *HLBlock::create_fsw(HLVal *val, Reg *base, HLVal *offset) {
    auto new_inst = new HLFswInst(val, base, offset, this);
    insts_list_.push_back(new_inst);
    return new_inst;
} 
HLSwInst *HLBlock::create_sw(HLVal *val, Label *label) {
    auto new_inst = new HLSwInst(val, label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLSwInst *HLBlock::create_sw(HLVal *val, Reg *base, HLVal *offset) {
    auto new_inst = new HLSwInst(val, base, offset, this);
    insts_list_.push_back(new_inst);
    return new_inst;
} 

HLCallInst *HLBlock::create_call(Label *label) {
    auto new_inst = new HLCallInst(label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

CallerSaveCallResultInst *HLBlock::create_caller_save_call_result(HLLoc *dst, Reg* src) {
    auto new_inst = new CallerSaveCallResultInst(dst, src, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
CalleeSaveCallResultInst *HLBlock::create_callee_save_call_result(RegLoc* dst, HLVal *src) {
    auto new_inst = new CalleeSaveCallResultInst(dst, src, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}
HLMemsetInst *HLBlock::create_memset(RegBase *base, int total_size, int step_size, bool is_fp) {
    auto new_inst = new HLMemsetInst(base, total_size, step_size, is_fp, this);
    insts_list_.push_back(new_inst);
    return new_inst;
} 

HLLaInst *HLBlock::create_la(Reg *dst, Label *label) {
    auto new_inst = new HLLaInst(dst, label, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

HLMvInst *HLBlock::create_mv(Reg *dst, Reg *src) {
    auto new_inst = new HLMvInst(dst, src, this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

HLRetInst *HLBlock::create_ret() {
    auto new_inst = new HLRetInst(this);
    insts_list_.push_back(new_inst);
    return new_inst;
}

std::string HLBlock::get_loongarch_code() {
    std::string loongarch_code;
    if(label_->get_label_name() != "") {
        loongarch_code += label_->get_label_name() + ":" + H2L::newline;
    } 
    for(auto inst: insts_list_) {
        loongarch_code += inst->get_loongarch_code();
    }

    return loongarch_code;
}


std::string HLBlock::print() {
    std::string hloongarch_code;
    if(label_->get_label_name() != "") {
        hloongarch_code += label_->get_label_name() + ":" + H2L::newline;
    } 
    for(auto inst: insts_list_) {
        hloongarch_code += inst->print();
    }

    return hloongarch_code;
}