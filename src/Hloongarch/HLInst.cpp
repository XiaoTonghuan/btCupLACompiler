#include "../include/Hloongarch/HLInst.hpp"
#include "../include/Hloongarch/HLLoc.hpp"
#include "../include/Hloongarch/H2L.hpp"
#include "../include/Hloongarch/HLFunction.hpp"

#include "logging.hpp"

HLFunction *HLInst::get_hloongarch_func() { return parent_->get_parent(); }

std::string CalleeSaveRegsInst::get_loongarch_code() {
    std::string loongarch_code;

    for(auto save_reg_pair: callee_save_regs_) {
        auto regloc = save_reg_pair.first;
        auto regbase = save_reg_pair.second;
        if(regloc->is_float())
            loongarch_code += H2L::fsw(new Reg(regloc->get_reg_id(), true), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
        else
            loongarch_code += H2L::sd(new Reg(regloc->get_reg_id(), false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
    }

    return loongarch_code;
}

std::string CalleeSaveRegsInst::print() {
    std::string code;
    code += H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space;
    code += "[ ";
    int num = callee_save_regs_.size();
    int i = 0;
    for(auto save_reg_pair: callee_save_regs_) {
        auto regloc = save_reg_pair.first;
        auto regbase = save_reg_pair.second;
        code += regloc->print();
        code += " -> ";
        code += regbase->print();
        if(i < num - 1) {
            code += ", ";
        }
        i++;
    }
    code += " ]";
    code += H2L::newline;
    return code;
}

std::string CalleeStartStackFrameInst::get_loongarch_code() {
    std::string loongarch_code;
    loongarch_code += H2L::mv(new Reg(reg_fp, false), new Reg(reg_sp, false));
    loongarch_code += H2L::addi(new Reg(reg_sp, false), new Reg(reg_sp, false), -stack_size_);
    return loongarch_code;
}

std::string CalleeStartStackFrameInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + std::to_string(stack_size_) + H2L::newline;
}

std::string CalleeArgsMoveInst::get_loongarch_code() {
    std::string loongarch_code;
    for(auto iarg_pair: to_move_iargs_) {
        auto target_loc = iarg_pair.first;
        auto src_loc = iarg_pair.second;
        if(dynamic_cast<RegLoc*>(target_loc) && dynamic_cast<RegLoc*>(src_loc)) {
            loongarch_code += H2L::mv(new Reg(dynamic_cast<RegLoc*>(target_loc)->get_reg_id(), false), new Reg(dynamic_cast<RegLoc*>(src_loc)->get_reg_id(), false));
        } else if(dynamic_cast<RegBase*>(target_loc) && dynamic_cast<RegLoc*>(src_loc)) {
            auto regbase = dynamic_cast<RegBase*>(target_loc);
            auto offset = regbase->get_offset();
            if(offset > 2047 || offset < -2048) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase->get_reg_id(), false), offset);
                loongarch_code += H2L::sd(new Reg(dynamic_cast<RegLoc*>(src_loc)->get_reg_id(), false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::sd(new Reg(dynamic_cast<RegLoc*>(src_loc)->get_reg_id(), false), new Reg(regbase->get_reg_id(), false), offset);
            }
        } else if(dynamic_cast<RegLoc*>(target_loc) && dynamic_cast<RegBase*>(src_loc)) {
            auto regbase = dynamic_cast<RegBase*>(src_loc);
            auto offset = regbase->get_offset();
            if(offset > 2047 || offset < -2048) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(dynamic_cast<RegLoc*>(target_loc)->get_reg_id(), false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(dynamic_cast<RegLoc*>(target_loc)->get_reg_id(), false), new Reg(regbase->get_reg_id(), false), offset);
            }
        } else {
            LOG(ERROR) << "出现未预期情况";
        }
    }
    
    for(auto farg_pair: to_move_fargs_) {
        auto target_loc = farg_pair.first;
        auto src_loc = farg_pair.second;
        if(dynamic_cast<RegLoc*>(target_loc) && dynamic_cast<RegLoc*>(src_loc)) {
            loongarch_code += H2L::fmvs(new Reg(dynamic_cast<RegLoc*>(target_loc)->get_reg_id(), true), new Reg(dynamic_cast<RegLoc*>(src_loc)->get_reg_id(), true));
        } else if(dynamic_cast<RegBase*>(target_loc) && dynamic_cast<RegLoc*>(src_loc)) {
            auto regbase = dynamic_cast<RegBase*>(target_loc);
            auto offset = regbase->get_offset();
            if(offset > 2047 || offset < -2048) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase->get_reg_id(), false), offset);
                loongarch_code += H2L::fsw(new Reg(dynamic_cast<RegLoc*>(src_loc)->get_reg_id(), true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::fsw(new Reg(dynamic_cast<RegLoc*>(src_loc)->get_reg_id(), true), new Reg(regbase->get_reg_id(), false), offset);
            }
        } else if(dynamic_cast<RegLoc*>(target_loc) && dynamic_cast<RegBase*>(src_loc)) {
            auto regbase = dynamic_cast<RegBase*>(src_loc);
            auto offset = regbase->get_offset();
            if(offset > 2047 || offset < -2048) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(dynamic_cast<RegLoc*>(target_loc)->get_reg_id(), true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(dynamic_cast<RegLoc*>(target_loc)->get_reg_id(), true), new Reg(regbase->get_reg_id(), false), offset);
            }
        } else {
            LOG(ERROR) << "出现未预期情况";
        }
    }

    return loongarch_code;
}

std::string CalleeArgsMoveInst::print() {
    std::string code;
    code += H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space;
    int num, i;
    if(! to_move_iargs_.empty()) {
        code += "[ ";
        num = to_move_iargs_.size();
        i = 0;
        for(auto iarg_pair: to_move_iargs_) {
            auto target = iarg_pair.first;
            auto src = iarg_pair.second;
            code += src->print();
            code += " -> ";
            code += target->print();
            if(i < num - 1) {
                code += ", ";
            }
            i++;
        }
        code += " ]";
    }
    if(! to_move_iargs_.empty() && ! to_move_fargs_.empty()) {
        code += ", ";
    }
    if(! to_move_fargs_.empty()) {
        code += "[ ";
        num = to_move_fargs_.size();
        i = 0;
        for(auto farg_pair: to_move_fargs_) {
            auto target = farg_pair.first;
            auto src = farg_pair.second;
            code += src->print();
            code += " -> ";
            code += target->print();
            if(i < num - 1) {
                code += ", ";
            }
            i++;
        }
        code += "]";
    }
    code += H2L::newline;
    return code;
}

std::string CalleeRestoreRegsInst::get_loongarch_code() {
    std::string loongarch_code;
    for(auto load_reg_pair: callee_load_regs_) {
        auto reg = load_reg_pair.first;
        auto regbase = load_reg_pair.second;
        if(reg->is_float())
            loongarch_code += H2L::flw(new Reg(reg->get_reg_id(), true), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
        else
            loongarch_code += H2L::ld(new Reg(reg->get_reg_id(), false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
    }

    return loongarch_code;
}

std::string CalleeRestoreRegsInst::print() {
    std::string code;
    code += H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space;
    code += "[ ";
    int num = callee_load_regs_.size();
    int i = 0;
    for(auto load_reg_pair: callee_load_regs_) {
        auto regloc = load_reg_pair.first;
        auto regbase = load_reg_pair.second;
        code += regbase->print();
        code += " -> ";
        code += regloc->print();
        if(i < num - 1) {
            code += ", ";
        }
        i++;
    }
    code += " ]";
    code += H2L::newline;
    return code;
}

std::string CalleeEndStackFrameInst::get_loongarch_code() {
    std::string loongarch_code;
    loongarch_code += H2L::addi(new Reg(reg_sp, false), new Reg(reg_sp, false), stack_size_);
    return loongarch_code;
}

std::string CalleeEndStackFrameInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + std::to_string(stack_size_) + H2L::newline;
}

std::string CallerSaveRegsInst::get_loongarch_code() {
    std::string loongarch_code;

    for(auto save_reg_pair: to_store_regs_) {
        auto regloc = save_reg_pair.first;
        auto regbase = save_reg_pair.second;
        if(regloc->is_float())
            loongarch_code += H2L::fsw(new Reg(regloc->get_reg_id(), true), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
        else
            loongarch_code += H2L::sd(new Reg(regloc->get_reg_id(), false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
    }

    return loongarch_code;
}

std::string CallerSaveRegsInst::print() {
    std::string code;
    code += H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space;
    code += "[ ";
    int num = to_store_regs_.size();
    int i = 0;
    for(auto save_reg_pair: to_store_regs_) {
        auto regloc = save_reg_pair.first;
        auto regbase = save_reg_pair.second;
        code += regloc->print();
        code += " -> ";
        code += regbase->print();
        if(i < num - 1) {
            code += ", ";
        }
        i++;
    }
    code += " ]";
    code += H2L::newline;
    return code;
}

std::string CallerArgsMoveInst::get_loongarch_code() {
    std::string loongarch_code;
    for(auto move_farg_pair: to_move_fargs_) {
        auto target = move_farg_pair.first;
        auto src = move_farg_pair.second;
        auto const_src = dynamic_cast<ConstPool*>(src);
        auto reg_src = dynamic_cast<RegLoc*>(src);
        auto regbase_src = dynamic_cast<RegBase*>(src); 
        if(const_src) {
            auto reg_target = dynamic_cast<RegLoc*>(target);
            auto regbase_target = dynamic_cast<RegBase*>(target);
            if(reg_target) {
                loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_src->get_fval()));
                loongarch_code += H2L::fmvsx(new Reg(reg_target->get_reg_id(), true), new Reg(reg_x, false));
            } else if(dynamic_cast<RegBase*>(target)) {
                loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_src->get_fval()));
                loongarch_code += H2L::fmvsx(new Reg(reg_target->get_reg_id(), true), new Reg(reg_x, false));
                if(regbase_target->get_offset() > 2047 || regbase_target->get_offset() < -2048) {
                    loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                    loongarch_code += H2L::fsw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
                } else {
                    loongarch_code += H2L::fsw(new Reg(reg_fs1, true), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                }
            } 
        } else if(reg_src) {
            auto reg_target = dynamic_cast<RegLoc*>(target);
            auto regbase_target = dynamic_cast<RegBase*>(target);
            if(reg_target) {
                loongarch_code += H2L::fmvs(new Reg(reg_target->get_reg_id(), true), new Reg(reg_src->get_reg_id(), true));
            } else if(dynamic_cast<RegBase*>(target)) {
                if(regbase_target->get_offset() > 2047 || regbase_target->get_offset() < -2048) {
                    loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                    loongarch_code += H2L::fsw(new Reg(reg_src->get_reg_id(), true), new Reg(reg_ra, false), 0);
                } else {
                    loongarch_code += H2L::fsw(new Reg(reg_src->get_reg_id(), true), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                }
            } 
        } else {
            if(regbase_src->get_offset() > 2047 || regbase_src->get_offset() < -2048) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase_src->get_reg_id(), false), regbase_src->get_offset()); 
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(regbase_src->get_reg_id(), false), regbase_src->get_offset()); 
            }
            auto reg_target = dynamic_cast<RegLoc*>(target);
            auto regbase_target = dynamic_cast<RegBase*>(target);
            if(reg_target) {
                loongarch_code += H2L::fmvs(new Reg(reg_target->get_reg_id(), true), new Reg(reg_fs1, true));
            } else if(dynamic_cast<RegBase*>(target)) {
                if(regbase_target->get_offset() > 2047 || regbase_target->get_offset() < -2048) {
                    loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                    loongarch_code += H2L::fsw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
                } else {
                    loongarch_code += H2L::fsw(new Reg(reg_fs1, true), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                }
            } 
        }
    }

    for(auto move_iarg_pair: to_move_iargs_) {
        auto target = move_iarg_pair.first;
        auto src = move_iarg_pair.second;
        auto const_src = dynamic_cast<ConstPool*>(src);
        auto reg_src = dynamic_cast<RegLoc*>(src);
        auto regbase_src = dynamic_cast<RegBase*>(src); 
        if(const_src) {
            auto reg_target = dynamic_cast<RegLoc*>(target);
            auto regbase_target = dynamic_cast<RegBase*>(target);
            if(reg_target) {
                loongarch_code += H2L::addi(new Reg(reg_target->get_reg_id(), false), new Reg(reg_zero, false), const_src->get_ival());
            } else if(dynamic_cast<RegBase*>(target)) {
                if(regbase_target->get_offset() > 2047 || regbase_target->get_offset() < -2048) {
                    loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                    loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(reg_zero, false), const_src->get_ival());
                    loongarch_code += H2L::sd(new Reg(reg_x, false), new Reg(reg_ra, false), 0);
                } else {
                    loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(reg_zero, false), const_src->get_ival());
                    loongarch_code += H2L::sd(new Reg(reg_x, false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                }
            } 
        } else if(reg_src) {
            auto reg_target = dynamic_cast<RegLoc*>(target);
            auto regbase_target = dynamic_cast<RegBase*>(target);
            if(reg_target) {
                loongarch_code += H2L::mv(new Reg(reg_target->get_reg_id(), false), new Reg(reg_src->get_reg_id(), false));
            } else if(dynamic_cast<RegBase*>(target)) {
                if(regbase_target->get_offset() > 2047 || regbase_target->get_offset() < -2048) {
                    loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                    loongarch_code += H2L::sd(new Reg(reg_src->get_reg_id(), false), new Reg(reg_ra, false), 0);
                } else {
                    loongarch_code += H2L::sd(new Reg(reg_src->get_reg_id(), false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                }
            } 
        } else {
            if(regbase_src->get_offset() > 2047 || regbase_src->get_offset() < -2048) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase_src->get_reg_id(), false), regbase_src->get_offset()); 
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(regbase_src->get_reg_id(), false), regbase_src->get_offset()); 
            }
            auto reg_target = dynamic_cast<RegLoc*>(target);
            auto regbase_target = dynamic_cast<RegBase*>(target);
            if(reg_target) {
                loongarch_code += H2L::mv(new Reg(reg_target->get_reg_id(), false), new Reg(reg_ra, false));
            } else if(dynamic_cast<RegBase*>(target)) {
                if(regbase_target->get_offset() > 2047 || regbase_target->get_offset() < -2048) {
                    loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                    loongarch_code += H2L::sd(new Reg(reg_ra, false), new Reg(reg_x, false), 0);
                } else {
                    loongarch_code += H2L::sd(new Reg(reg_ra, false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                }
            } 
        }
    } 

    return loongarch_code;
}

std::string CallerArgsMoveInst::print() {
    std::string code;
    code += H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space;
    int num, i;
    if(! to_move_fargs_.empty()) {
        num = to_move_fargs_.size();
        i = 0;
        code += "[ ";
        for(auto farg_pair: to_move_fargs_) {
            auto target = farg_pair.first;
            auto src = farg_pair.second;
            code += src->print();
            code += " -> ";
            code += target->print();
            if(i < num - 1) {
                code += ", ";
            }
            i++;
        }
        code += " ]";
    }
    if(! to_move_fargs_.empty() && ! to_move_iargs_.empty()) {
        code += ", ";
    } 
    if(! to_move_iargs_.empty()) {
        code += "[ ";
        num = to_move_iargs_.size();
        i = 0;
        for(auto iarg_pair: to_move_iargs_) {
            auto target = iarg_pair.first;
            auto src = iarg_pair.second;
            code += src->print();
            code += " -> ";
            code += target->print();
            if(i < num - 1) {
                code += ", ";
            }
            i++;
        }
        code += "]";
    }
    code += H2L::newline;
    return code;
}

std::string CallerRestoreRegsInst::get_loongarch_code() {
    std::string loongarch_code;
    for(auto load_reg_pair: to_restore_regs_) {
        auto reg = load_reg_pair.first;
        auto regbase = load_reg_pair.second;
        if(reg->is_float())
            loongarch_code += H2L::flw(new Reg(reg->get_reg_id(), true), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
        else
            loongarch_code += H2L::ld(new Reg(reg->get_reg_id(), false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
    }

    return loongarch_code;
}

std::string CallerRestoreRegsInst::print() {
    std::string code;
    code += H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space;
    code += "[ ";
    int num = to_restore_regs_.size();
    int i = 0;
    for(auto load_reg_pair: to_restore_regs_) {
        auto regloc = load_reg_pair.first;
        auto regbase = load_reg_pair.second;
        code += regbase->print();
        code += " -> ";
        code += regloc->print();
        if(i < num - 1) {
            code += ", ";
        }
        i++;
    }
    code += " ]";
    code += H2L::newline;
    return code;
}

std::string LdTmpRegsInst::get_loongarch_code() {
    std::string loongarch_code;
    for(auto load_reg_pair: to_ld_tmp_regs_) {
        auto reg = load_reg_pair.first;
        auto regbase = load_reg_pair.second;
        if(reg->is_float()) {
            if(regbase->get_offset() < -2048 || regbase->get_offset() > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
                loongarch_code += H2L::flw(new Reg(reg->get_reg_id(), true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg->get_reg_id(), true), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
            }
        } else {
            if(regbase->get_offset() < -2048 || regbase->get_offset() > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
                loongarch_code += H2L::ld(new Reg(reg->get_reg_id(), false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg->get_reg_id(), false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
            }
        }
    }

    return loongarch_code;
}

std::string LdTmpRegsInst::print() {
    std::string code;
    code += H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space;
    code += "[ ";
    int num = to_ld_tmp_regs_.size();
    int i = 0;
    for(auto load_reg_pair: to_ld_tmp_regs_) {
        auto regloc = load_reg_pair.first;
        auto regbase = load_reg_pair.second;
        code += regbase->print();
        code += " -> ";
        code += regloc->print();
        if(i < num - 1) {
            code += ", ";
        }
        i++;
    }
    code += " ]";
    code += H2L::newline;
    return code;
}

std::string StoreTmpResultInst::get_loongarch_code() {
    std::string loongarch_code;
    for(auto store_reg_pair: to_store_regs_) {
        auto reg = store_reg_pair.first;
        auto regbase = store_reg_pair.second;
        if(reg->is_float()) {
            if(regbase->get_offset() < -2048 || regbase->get_offset() > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
                loongarch_code += H2L::fsw(new Reg(reg->get_reg_id(), true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::fsw(new Reg(reg->get_reg_id(), true), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
            }
        } else {
            if(regbase->get_offset() < -2048 || regbase->get_offset() > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
                loongarch_code += H2L::sd(new Reg(reg->get_reg_id(), false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::sd(new Reg(reg->get_reg_id(), false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
            }
        }
    }

    return loongarch_code;
}

std::string StoreTmpResultInst::print() {
    std::string code;
    code += H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space;
    code += "[ ";
    int num = to_store_regs_.size();
    int i = 0;
    for(auto save_reg_pair: to_store_regs_) {
        auto regloc = save_reg_pair.first;
        auto regbase = save_reg_pair.second;
        code += regloc->print();
        code += " -> ";
        code += regbase->print();
        if(i < num - 1) {
            code += ", ";
        }
        i++;
    }
    code += " ]";
    code += H2L::newline;
    return code;
}

std::string AllocTmpRegsWithSaveInitialOwnerInst::get_loongarch_code() {
    std::string loongarch_code;
    for(auto store_reg_pair: to_store_regs_) {
        auto reg = store_reg_pair.first;
        auto regbase = store_reg_pair.second;
        if(reg->is_float()) {
            if(regbase->get_offset() < -2048 || regbase->get_offset() > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
                loongarch_code += H2L::fsw(new Reg(reg->get_reg_id(), true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::fsw(new Reg(reg->get_reg_id(), true), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
            }
        } else {
            if(regbase->get_offset() < -2048 || regbase->get_offset() > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
                loongarch_code += H2L::sd(new Reg(reg->get_reg_id(), false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::sd(new Reg(reg->get_reg_id(), false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
            }
        }
    }

    for(auto load_reg_pair: to_ld_regs_) {
        auto reg = load_reg_pair.first;
        auto regbase = load_reg_pair.second;
        if(reg->is_float()) {
            if(regbase->get_offset() < -2048 || regbase->get_offset() > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
                loongarch_code += H2L::flw(new Reg(reg->get_reg_id(), true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg->get_reg_id(), true), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
            }
        } else {
            if(regbase->get_offset() < -2048 || regbase->get_offset() > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
                loongarch_code += H2L::ld(new Reg(reg->get_reg_id(), false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg->get_reg_id(), false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
            }
        }
    }

    return loongarch_code;
}

std::string AllocTmpRegsWithSaveInitialOwnerInst::print() {
    std::string code;
    code += H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space;
    int num, i;
    if(! to_store_regs_.empty()) {
        code += "[ ";
        int num = to_store_regs_.size();
        int i = 0;
        for(auto save_reg_pair: to_store_regs_) {
            auto regloc = save_reg_pair.first;
            auto regbase = save_reg_pair.second;
            code += regloc->print();
            code += " -> ";
            code += regbase->print();
            if(i < num - 1) {
                code += ", ";
            }
            i++;
        }
        code += " ]";
    }

    if(! to_store_regs_.empty() && ! to_ld_regs_.empty()) {
        code += ", ";
    }
    if(! to_ld_regs_.empty()) {
        code += "[ ";
        num = to_store_regs_.size();
        i = 0;
        for(auto load_reg_pair: to_ld_regs_) {
            auto regloc = load_reg_pair.first;
            auto regbase = load_reg_pair.second;
            code += regbase->print();
            code += " -> ";
            code += regloc->print();
            if(i < num - 1) {
                code += ", ";
            }
            i++;
        }
        code += "]";
    }
    
    code += H2L::newline;
    return code;
}

std::string RestoreAllTmpRegsInst::get_loongarch_code() {
    std::string loongarch_code;
    for(auto load_reg_pair: to_restore_regs_) {
        auto reg = load_reg_pair.first;
        auto regbase = load_reg_pair.second;
        if(reg->is_float()) {
            if(regbase->get_offset() < -2048 || regbase->get_offset() > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
                loongarch_code += H2L::flw(new Reg(reg->get_reg_id(), true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg->get_reg_id(), true), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
            }
        } else {
            if(regbase->get_offset() < -2048 || regbase->get_offset() > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
                loongarch_code += H2L::ld(new Reg(reg->get_reg_id(), false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg->get_reg_id(), false), new Reg(regbase->get_reg_id(), false), regbase->get_offset());
            }
        }
    }

    return loongarch_code;
}

std::string RestoreAllTmpRegsInst::print() {
    std::string code;
    code += H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space;
    code += "[ ";
    int num = to_restore_regs_.size();
    int i = 0;
    for(auto load_reg_pair: to_restore_regs_) {
        auto regloc = load_reg_pair.first;
        auto regbase = load_reg_pair.second;
        code += regbase->print();
        code += " -> ";
        code += regloc->print();
        if(i < num - 1) {
            code += ", ";
        }
        i++;
    }
    code += " ]";
    code += H2L::newline;
    return code;
}

std::string PhiDataMoveInst::get_loongarch_code() {
    std::string loongarch_code;
    for(auto move_floc_pair: to_move_flocs_) {
        auto target = move_floc_pair.first;
        auto src = move_floc_pair.second;
        auto const_src = dynamic_cast<ConstPool*>(src);
        auto reg_src = dynamic_cast<RegLoc*>(src);
        auto regbase_src = dynamic_cast<RegBase*>(src); 
        if(const_src) {
            auto reg_target = dynamic_cast<RegLoc*>(target);
            auto regbase_target = dynamic_cast<RegBase*>(target);
            if(reg_target) {
                loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_src->get_fval()));
                loongarch_code += H2L::fmvsx(new Reg(reg_target->get_reg_id(), true), new Reg(reg_x, false));
            } else if(dynamic_cast<RegBase*>(target)) {
                loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_src->get_fval()));
                loongarch_code += H2L::fmvsx(new Reg(reg_target->get_reg_id(), true), new Reg(reg_x, false));
                if(regbase_target->get_offset() > 2047 || regbase_target->get_offset() < -2048) {
                    loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                    loongarch_code += H2L::fsw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
                } else {
                    loongarch_code += H2L::fsw(new Reg(reg_fs1, true), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                }
            } 
        } else if(reg_src) {
            auto reg_target = dynamic_cast<RegLoc*>(target);
            auto regbase_target = dynamic_cast<RegBase*>(target);
            if(reg_target) {
                loongarch_code += H2L::fmvs(new Reg(reg_target->get_reg_id(), true), new Reg(reg_src->get_reg_id(), true));
            } else if(dynamic_cast<RegBase*>(target)) {
                if(regbase_target->get_offset() > 2047 || regbase_target->get_offset() < -2048) {
                    loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                    loongarch_code += H2L::fsw(new Reg(reg_src->get_reg_id(), true), new Reg(reg_ra, false), 0);
                } else {
                    loongarch_code += H2L::fsw(new Reg(reg_src->get_reg_id(), true), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                }
            } 
        } else {
            if(regbase_src->get_offset() > 2047 || regbase_src->get_offset() < -2048) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase_src->get_reg_id(), false), regbase_src->get_offset()); 
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(regbase_src->get_reg_id(), false), regbase_src->get_offset()); 
            }
            auto reg_target = dynamic_cast<RegLoc*>(target);
            auto regbase_target = dynamic_cast<RegBase*>(target);
            if(reg_target) {
                loongarch_code += H2L::fmvs(new Reg(reg_target->get_reg_id(), true), new Reg(reg_fs1, true));
            } else if(dynamic_cast<RegBase*>(target)) {
                if(regbase_target->get_offset() > 2047 || regbase_target->get_offset() < -2048) {
                    loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                    loongarch_code += H2L::fsw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
                } else {
                    loongarch_code += H2L::fsw(new Reg(reg_fs1, true), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                }
            } 
        }
    }

    for(auto move_iloc_pair: to_move_ilocs_) {
        auto target = move_iloc_pair.first;
        auto src = move_iloc_pair.second;
        auto const_src = dynamic_cast<ConstPool*>(src);
        auto reg_src = dynamic_cast<RegLoc*>(src);
        auto regbase_src = dynamic_cast<RegBase*>(src); 
        if(const_src) {
            auto reg_target = dynamic_cast<RegLoc*>(target);
            auto regbase_target = dynamic_cast<RegBase*>(target);
            if(reg_target) {
                loongarch_code += H2L::addi(new Reg(reg_target->get_reg_id(), false), new Reg(reg_zero, false), const_src->get_ival());
            } else if(dynamic_cast<RegBase*>(target)) {
                if(regbase_target->get_offset() > 2047 || regbase_target->get_offset() < -2048) {
                    loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                    loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(reg_zero, false), const_src->get_ival());
                    loongarch_code += H2L::sd(new Reg(reg_x, false), new Reg(reg_ra, false), 0);
                } else {
                    loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(reg_zero, false), const_src->get_ival());
                    loongarch_code += H2L::sd(new Reg(reg_x, false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                }
            } 
        } else if(reg_src) {
            auto reg_target = dynamic_cast<RegLoc*>(target);
            auto regbase_target = dynamic_cast<RegBase*>(target);
            if(reg_target) {
                loongarch_code += H2L::mv(new Reg(reg_target->get_reg_id(), false), new Reg(reg_src->get_reg_id(), false));
            } else if(dynamic_cast<RegBase*>(target)) {
                if(regbase_target->get_offset() > 2047 || regbase_target->get_offset() < -2048) {
                    loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                    loongarch_code += H2L::sd(new Reg(reg_src->get_reg_id(), false), new Reg(reg_ra, false), 0);
                } else {
                    loongarch_code += H2L::sd(new Reg(reg_src->get_reg_id(), false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                }
            } 
        } else {
            if(regbase_src->get_offset() > 2047 || regbase_src->get_offset() < -2048) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase_src->get_reg_id(), false), regbase_src->get_offset()); 
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(regbase_src->get_reg_id(), false), regbase_src->get_offset()); 
            }
            auto reg_target = dynamic_cast<RegLoc*>(target);
            auto regbase_target = dynamic_cast<RegBase*>(target);
            if(reg_target) {
                loongarch_code += H2L::mv(new Reg(reg_target->get_reg_id(), false), new Reg(reg_ra, false));
            } else if(dynamic_cast<RegBase*>(target)) {
                if(regbase_target->get_offset() > 2047 || regbase_target->get_offset() < -2048) {
                    loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                    loongarch_code += H2L::sd(new Reg(reg_ra, false), new Reg(reg_x, false), 0);
                } else {
                    loongarch_code += H2L::sd(new Reg(reg_ra, false), new Reg(regbase_target->get_reg_id(), false), regbase_target->get_offset());
                }
            } 
        }
    } 

    return loongarch_code;
}

std::string PhiDataMoveInst::print() {
    std::string code;
    code += H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space;
    int num, i;
    if(! to_move_flocs_.empty()) {
        code += "[ ";
        num = to_move_flocs_.size();
        i = 0;
        for(auto floc_pair: to_move_flocs_) {
            auto target = floc_pair.first;
            auto src = floc_pair.second;
            code += src->print();
            code += " -> ";
            code += target->print();
            if(i < num - 1) {
                code += ", ";
            }
            i++;
        }
        code += " ]";
    }
    if(! to_move_flocs_.empty() && ! to_move_ilocs_.empty()) {
        code += ", ";
    }

    if(! to_move_ilocs_.empty()) {
        code += "[ ";
        num = to_move_ilocs_.size();
        i = 0;
        for(auto iloc_pair: to_move_ilocs_) {
            auto target = iloc_pair.first;
            auto src = iloc_pair.second;
            code += src->print();
            code += " -> ";
            code += target->print();
            if(i < num - 1) {
                code += ", ";
            }
            i++;
        }
        code += "]";
    }
    
    code += H2L::newline;
    return code;
}

std::string CallerSaveCallResultInst::get_loongarch_code() {
    std::string loongarch_code;
    if(src_->is_float()) {
        auto regbase_dst = dynamic_cast<RegBase*>(dst_);
        auto regloc_dst = dynamic_cast<RegLoc*>(dst_);
        if(regbase_dst) {
            if(regbase_dst->get_offset() > 2047 || regbase_dst->get_offset() < -2048) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase_dst->get_reg_id(), false), regbase_dst->get_offset());
                loongarch_code += H2L::fsw(new Reg(reg_fa0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::fsw(new Reg(reg_fa0, true), new Reg(regbase_dst->get_reg_id(), false), regbase_dst->get_offset());
            }
        } else {
            loongarch_code += H2L::fmvs(new Reg(regloc_dst->get_reg_id(), true), new Reg(reg_fa0, true));
        }
    } else { 
        auto regbase_dst = dynamic_cast<RegBase*>(dst_);
        auto regloc_dst = dynamic_cast<RegLoc*>(dst_);
        if(regbase_dst) {
            if(regbase_dst->get_offset() > 2047 || regbase_dst->get_offset() < -2048) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(regbase_dst->get_reg_id(), false), regbase_dst->get_offset());
                loongarch_code += H2L::sd(new Reg(reg_a0, false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::sd(new Reg(reg_a0, false), new Reg(regbase_dst->get_reg_id(), false), regbase_dst->get_offset());
            }
        } else {
            loongarch_code += H2L::mv(new Reg(regloc_dst->get_reg_id(), false), new Reg(reg_a0, false));
        }
    }
    return loongarch_code;
}

std::string CallerSaveCallResultInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + src_->print() + H2L::newline;
}

std::string CalleeSaveCallResultInst::get_loongarch_code() {
    std::string loongarch_code;
    if(dst_->is_float()) {
        auto const_src = dynamic_cast<Const*>(src_);
        auto reg_src = dynamic_cast<Reg*>(src_);
        auto mem_src = dynamic_cast<Mem*>(src_);
        if(const_src) {
            loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_src->get_fval()));
            loongarch_code += H2L::fmvsx(new Reg(reg_fa0, true), new Reg(reg_x, false));
        } else if(mem_src) {
            if(mem_src->get_offset() > 2047 || mem_src->get_offset() < -2048) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_src->get_reg_id(), false), mem_src->get_offset());
                loongarch_code += H2L::flw(new Reg(reg_fa0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fa0, true), new Reg(mem_src->get_reg_id(), false), mem_src->get_offset());
            }
        } else {
            loongarch_code += H2L::fmvs(new Reg(reg_fa0, true), reg_src);
        }
    } else {
        auto const_src = dynamic_cast<Const*>(src_);
        auto reg_src = dynamic_cast<Reg*>(src_);
        auto mem_src = dynamic_cast<Mem*>(src_);
        if(const_src) {
            loongarch_code += H2L::addi(new Reg(reg_a0, false), new Reg(reg_zero, false), const_src->get_ival());
        } else if(mem_src) {
            if(mem_src->get_offset() > 2047 || mem_src->get_offset() < -2048) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_src->get_reg_id(), false), mem_src->get_offset());
                loongarch_code += H2L::ld(new Reg(reg_a0, false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_a0, false), new Reg(mem_src->get_reg_id(), false), mem_src->get_offset());
            }
        } else {
            loongarch_code += H2L::mv(new Reg(reg_a0, false), reg_src);
        }
    }
    return loongarch_code;
}

std::string CalleeSaveCallResultInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + src_->print() + H2L::newline;
}

std::string ExpandStackSpaceInst::get_loongarch_code() {
    std::string loongarch_code;
    loongarch_code += H2L::addi(new Reg(reg_sp, false), new Reg(reg_sp, false), expand_size_);
    return loongarch_code;
}

std::string ExpandStackSpaceInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + std::to_string(-expand_size_) + H2L::newline;
}

std::string ShrinkStackSpaceInst::get_loongarch_code() {
    std::string loongarch_code;
    loongarch_code += H2L::addi(new Reg(reg_sp, false), new Reg(reg_sp, false), shrink_size_);
    return loongarch_code;
}

std::string ShrinkStackSpaceInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + std::to_string(shrink_size_) + H2L::newline;
}

std::string HLAddInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs1 = dynamic_cast<Const*>(rs1_);
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs1 && const_rs2) {
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), const_rs1->get_ival() + const_rs2->get_ival());
    } else if(const_rs1) {
        loongarch_code += H2L::addi(dst_, dynamic_cast<Reg*>(rs2_), const_rs1->get_ival());
    } else if(const_rs2) {
        loongarch_code += H2L::addi(dst_, dynamic_cast<Reg*>(rs1_), const_rs2->get_ival());
    } else {
        loongarch_code += H2L::add(dst_, dynamic_cast<Reg*>(rs1_), dynamic_cast<Reg*>(rs2_));
    }
    return loongarch_code;
}

std::string HLAddInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLSubwInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs1 = dynamic_cast<Const*>(rs1_);
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs1 && const_rs2) {
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), const_rs1->get_ival() - const_rs2->get_ival());
    } else if(const_rs1) {
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), const_rs1->get_ival());
        loongarch_code += H2L::subw(dst_, dst_, dynamic_cast<Reg*>(rs2_));
    } else if(const_rs2) {
        loongarch_code += H2L::addi(dst_, dynamic_cast<Reg*>(rs1_), - const_rs2->get_ival());
    } else {
        loongarch_code += H2L::subw(dst_, dynamic_cast<Reg*>(rs1_), dynamic_cast<Reg*>(rs2_));
    }
    return loongarch_code;
}

std::string HLSubwInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLMulwInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs1 = dynamic_cast<Const*>(rs1_);
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs1 && const_rs2) {
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), const_rs1->get_ival() * const_rs2->get_ival());
    } else if(const_rs1) {
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), const_rs1->get_ival());
        loongarch_code += H2L::mulw(dst_, dst_, dynamic_cast<Reg*>(rs2_));
    } else if(const_rs2) {
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), const_rs2->get_ival());
        loongarch_code += H2L::mulw(dst_, dynamic_cast<Reg*>(rs1_), dst_);
    } else {
        loongarch_code += H2L::mulw(dst_, dynamic_cast<Reg*>(rs1_), dynamic_cast<Reg*>(rs2_));
    }
    return loongarch_code;
}

std::string HLMulwInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLMul64Inst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs1 = dynamic_cast<Const*>(rs1_);
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs1 && const_rs2) {
        LOG(ERROR) << "无法处理";
    } else if(const_rs1) {
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), const_rs1->get_ival());
        loongarch_code += H2L::mul(dst_, dst_, dynamic_cast<Reg*>(rs2_));
    } else if(const_rs2) {
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), const_rs2->get_ival());
        loongarch_code += H2L::mul(dst_, dynamic_cast<Reg*>(rs1_), dst_);
    } else {
        loongarch_code += H2L::mul(dst_, dynamic_cast<Reg*>(rs1_), dynamic_cast<Reg*>(rs2_));
    }
    return loongarch_code;
}

std::string HLMul64Inst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLDivwInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs1 = dynamic_cast<Const*>(rs1_);
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs1 && const_rs2) {
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), const_rs1->get_ival() / const_rs2->get_ival());
    } else if(const_rs1) {
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), const_rs1->get_ival());
        loongarch_code += H2L::divw(dst_, dst_, dynamic_cast<Reg*>(rs2_));
    } else if(const_rs2) {
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), const_rs2->get_ival());
        loongarch_code += H2L::divw(dst_, dynamic_cast<Reg*>(rs1_), dst_);
    } else {
        loongarch_code += H2L::divw(dst_, dynamic_cast<Reg*>(rs1_), dynamic_cast<Reg*>(rs2_));
    }
    return loongarch_code;
}

std::string HLDivwInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLRemwInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs1 = dynamic_cast<Const*>(rs1_);
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs1 && const_rs2) {
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), const_rs1->get_ival() % const_rs2->get_ival());
    } else if(const_rs1) {
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), const_rs1->get_ival());
        loongarch_code += H2L::remw(dst_, dst_, dynamic_cast<Reg*>(rs2_));
    } else if(const_rs2) {
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), const_rs2->get_ival());
        loongarch_code += H2L::remw(dst_, dynamic_cast<Reg*>(rs1_), dst_);
    } else {
        loongarch_code += H2L::remw(dst_, dynamic_cast<Reg*>(rs1_), dynamic_cast<Reg*>(rs2_));
    }
    return loongarch_code;
}

std::string HLRemwInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLSrawInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs1 = dynamic_cast<Const*>(rs1_);
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs2 == nullptr)
        LOG(ERROR) << "出现未预期情况";
    if(const_rs1)
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), (const_rs1->get_ival()) >> (const_rs2->get_ival()));
    else 
        loongarch_code += H2L::sraiw(dst_, dynamic_cast<Reg*>(rs1_), const_rs2->get_ival());

    return loongarch_code;
}

std::string HLSrawInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLSllwInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs1 = dynamic_cast<Const*>(rs1_);
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs2 == nullptr)
        LOG(ERROR) << "出现未预期情况";
    if(const_rs1)
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), (const_rs1->get_ival()) << (const_rs2->get_ival()));
    else 
        loongarch_code += H2L::slliw(dst_, dynamic_cast<Reg*>(rs1_), const_rs2->get_ival());

    return loongarch_code;
}

std::string HLSllwInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLSrlwInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs1 = dynamic_cast<Const*>(rs1_);
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs2 == nullptr)
        LOG(ERROR) << "出现未预期情况";
    if(const_rs1)
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), (const_rs1->get_ival()) >> (const_rs2->get_ival()));
    else 
        loongarch_code += H2L::srliw(dst_, dynamic_cast<Reg*>(rs1_), const_rs2->get_ival());

    return loongarch_code;
}

std::string HLSrlwInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLSraInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs1 = dynamic_cast<Const*>(rs1_);
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs2 == nullptr)
        LOG(ERROR) << "出现未预期情况";
    if(const_rs1)
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), (const_rs1->get_ival()) >> (const_rs2->get_ival()));
    else 
        loongarch_code += H2L::srai(dst_, dynamic_cast<Reg*>(rs1_), const_rs2->get_ival());

    return loongarch_code;
}

std::string HLSraInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLSllInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs1 = dynamic_cast<Const*>(rs1_);
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs2 == nullptr)
        LOG(ERROR) << "出现未预期情况";
    if(const_rs1)
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), (const_rs1->get_ival()) << (const_rs2->get_ival()));
    else 
        loongarch_code += H2L::slli(dst_, dynamic_cast<Reg*>(rs1_), const_rs2->get_ival());

    return loongarch_code;
}

std::string HLSllInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLSrlInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs1 = dynamic_cast<Const*>(rs1_);
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs2 == nullptr)
        LOG(ERROR) << "出现未预期情况";
    if(const_rs1)
        loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), (const_rs1->get_ival()) >> (const_rs2->get_ival()));
    else 
        loongarch_code += H2L::srli(dst_, dynamic_cast<Reg*>(rs1_), const_rs2->get_ival());

    return loongarch_code;
}

std::string HLSrlInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLLandInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs2 == nullptr)
        LOG(ERROR) << "出现未预期情况";
    if(const_rs2->get_ival() > 2047 || const_rs2->get_ival() < -2048) {
        loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(reg_zero, false), const_rs2->get_ival());
        loongarch_code += H2L::land(dst_, dynamic_cast<Reg*>(rs1_), new Reg(reg_ra, false));
    } else {
        loongarch_code += H2L::andi(dst_, dynamic_cast<Reg*>(rs1_), const_rs2->get_ival());
    }   

    return loongarch_code;
}

std::string HLLandInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLFaddsInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs1 = dynamic_cast<Const*>(rs1_);
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs1 && const_rs2) {
        auto result = const_rs1->get_fval() + const_rs2->get_fval();
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)(&result));
        loongarch_code += H2L::fmvsx(dst_, new Reg(reg_x, false));
    } else if(const_rs1) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)(&(const_rs1->get_fval())));
        loongarch_code += H2L::fmvsx(dst_, new Reg(reg_x, false));
        loongarch_code += H2L::fadds(dst_, dst_, dynamic_cast<Reg*>(rs2_));
    } else if(const_rs2) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)(&(const_rs2->get_fval())));
        loongarch_code += H2L::fmvsx(dst_, new Reg(reg_x, false));
        loongarch_code += H2L::fadds(dst_, dynamic_cast<Reg*>(rs1_), dst_);
    } else {
        loongarch_code += H2L::fadds(dst_, dynamic_cast<Reg*>(rs1_), dynamic_cast<Reg*>(rs2_));
    }
    return loongarch_code;
}

std::string HLFaddsInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLFsubsInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs1 = dynamic_cast<Const*>(rs1_);
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs1 && const_rs2) {
        auto result = const_rs1->get_fval() - const_rs2->get_fval();
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)(&result));
        loongarch_code += H2L::fmvsx(dst_, new Reg(reg_x, false));
    } else if(const_rs1) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)(&(const_rs1->get_fval())));
        loongarch_code += H2L::fmvsx(dst_, new Reg(reg_x, false));
        loongarch_code += H2L::fsubs(dst_, dst_, dynamic_cast<Reg*>(rs2_));
    } else if(const_rs2) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)(&(const_rs2->get_fval())));
        loongarch_code += H2L::fmvsx(dst_, new Reg(reg_x, false));
        loongarch_code += H2L::fsubs(dst_, dynamic_cast<Reg*>(rs1_), dst_);
    } else {
        loongarch_code += H2L::fsubs(dst_, dynamic_cast<Reg*>(rs1_), dynamic_cast<Reg*>(rs2_));
    }
    return loongarch_code;
}

std::string HLFsubsInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLFmulsInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs1 = dynamic_cast<Const*>(rs1_);
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs1 && const_rs2) {
        auto result = const_rs1->get_fval() * const_rs2->get_fval();
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)(&result));
        loongarch_code += H2L::fmvsx(dst_, new Reg(reg_x, false));
    } else if(const_rs1) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)(&(const_rs1->get_fval())));
        loongarch_code += H2L::fmvsx(dst_, new Reg(reg_x, false));
        loongarch_code += H2L::fmuls(dst_, dst_, dynamic_cast<Reg*>(rs2_));
    } else if(const_rs2) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)(&(const_rs2->get_fval())));
        loongarch_code += H2L::fmvsx(dst_, new Reg(reg_x, false));
        loongarch_code += H2L::fmuls(dst_, dynamic_cast<Reg*>(rs1_), dst_);
    } else {
        loongarch_code += H2L::fmuls(dst_, dynamic_cast<Reg*>(rs1_), dynamic_cast<Reg*>(rs2_));
    }
    return loongarch_code;
}

std::string HLFmulsInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLFdivsInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_rs1 = dynamic_cast<Const*>(rs1_);
    auto const_rs2 = dynamic_cast<Const*>(rs2_);
    if(const_rs1 && const_rs2) {
        auto result = const_rs1->get_fval() / const_rs2->get_fval();
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)(&result));
        loongarch_code += H2L::fmvsx(dst_, new Reg(reg_x, false));
    } else if(const_rs1) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)(&(const_rs1->get_fval())));
        loongarch_code += H2L::fmvsx(dst_, new Reg(reg_x, false));
        loongarch_code += H2L::fdivs(dst_, dst_, dynamic_cast<Reg*>(rs2_));
    } else if(const_rs2) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)(&(const_rs2->get_fval())));
        loongarch_code += H2L::fmvsx(dst_, new Reg(reg_x, false));
        loongarch_code += H2L::fdivs(dst_, dynamic_cast<Reg*>(rs1_), dst_);
    } else {
        loongarch_code += H2L::fdivs(dst_, dynamic_cast<Reg*>(rs1_), dynamic_cast<Reg*>(rs2_));
    }
    return loongarch_code;
}

std::string HLFdivsInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + rs1_->print() + ", " + rs2_->print()+ H2L::newline;
}

std::string HLFcvtwsInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_src = dynamic_cast<Const*>(src_);
    if(const_src) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)(&(const_src->get_fval())));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        loongarch_code += H2L::fcvtws(dst_, new Reg(reg_fs1, true));
    } else {
        loongarch_code += H2L::fcvtws(dst_, dynamic_cast<Reg*>(src_));
    }
    
    return loongarch_code;
}

std::string HLFcvtwsInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + src_->print() + H2L::newline;
}

std::string HLFcvtswInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_src = dynamic_cast<Const*>(src_);
    if(const_src) {
        loongarch_code += H2L::addiw(new Reg(reg_ra, false), new Reg(reg_zero, false), const_src->get_ival());
        loongarch_code += H2L::fcvtsw(dst_, new Reg(reg_ra, false));
    } else {
        loongarch_code += H2L::fcvtsw(dst_, dynamic_cast<Reg*>(src_));
    }
    
    return loongarch_code;
}

std::string HLFcvtswInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + src_->print() + H2L::newline;
}

std::string HLZextInst::get_loongarch_code() {
    std::string loongarch_code;
    loongarch_code += H2L::andi(dst_, src_, 1);
    return loongarch_code;
}

std::string HLZextInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + src_->print() + H2L::newline;
}

std::string HLSnezInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_cond = dynamic_cast<Const*>(cond_);
    if(const_cond) {
        if(const_cond->get_ival() != 0) {
            loongarch_code += H2L::seqz(dst_, new Reg(reg_zero, false));
        } else {
            loongarch_code += H2L::snez(dst_, new Reg(reg_zero, false));
        }   
    } else {
        loongarch_code += H2L::sextw(new Reg(reg_ra, false), dynamic_cast<Reg*>(cond_));
        loongarch_code += H2L::snez(dst_, new Reg(reg_ra, false));
    }   
    return loongarch_code;
}

std::string HLSnezInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + cond_->print() + H2L::newline;
}

std::string HLSeqzInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_cond = dynamic_cast<Const*>(cond_);
    if(const_cond) {
        if(const_cond->get_ival() == 0) {
            loongarch_code += H2L::seqz(dst_, new Reg(reg_zero, false));
        } else {
            loongarch_code += H2L::snez(dst_, new Reg(reg_zero, false));
        }   
    } else {
        loongarch_code += H2L::sextw(new Reg(reg_ra, false), dynamic_cast<Reg*>(cond_));
        loongarch_code += H2L::seqz(dst_, new Reg(reg_ra, false));
    }   
    return loongarch_code;
}

std::string HLSeqzInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + cond_->print() + H2L::newline;
}

std::string HLFeqsInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_cond1 = dynamic_cast<Const*>(cond1_);
    auto const_cond2 = dynamic_cast<Const*>(cond2_);
    if(const_cond1 && const_cond2) {
        if(const_cond1->get_fval() == const_cond2->get_fval()) {
            loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), 1);
        } else {    
            loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), 0);
        }
    } else if(const_cond1) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_cond1->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        loongarch_code += H2L::feqs(dst_, new Reg(reg_fs1, true), dynamic_cast<Reg*>(cond2_));
    } else if(const_cond2) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_cond2->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        loongarch_code += H2L::feqs(dst_, dynamic_cast<Reg*>(cond1_), new Reg(reg_fs1, true));
    } else {
        loongarch_code += H2L::feqs(dst_, dynamic_cast<Reg*>(cond1_), dynamic_cast<Reg*>(cond2_));
    }
    return loongarch_code; 
}

std::string HLFeqsInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + cond1_->print() + ", " + cond2_->print() + H2L::newline;
}

std::string HLFlesInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_cond1 = dynamic_cast<Const*>(cond1_);
    auto const_cond2 = dynamic_cast<Const*>(cond2_);
    if(const_cond1 && const_cond2) {
        if(const_cond1->get_fval() <= const_cond2->get_fval()) {
            loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), 1);
        } else {    
            loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), 0);
        }
    } else if(const_cond1) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_cond1->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        loongarch_code += H2L::fles(dst_, new Reg(reg_fs1, true), dynamic_cast<Reg*>(cond2_));
    } else if(const_cond2) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_cond2->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        loongarch_code += H2L::fles(dst_, dynamic_cast<Reg*>(cond1_), new Reg(reg_fs1, true));
    } else {
        loongarch_code += H2L::fles(dst_, dynamic_cast<Reg*>(cond1_), dynamic_cast<Reg*>(cond2_));
    }
    return loongarch_code;
}

std::string HLFlesInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + cond1_->print() + ", " + cond2_->print() + H2L::newline;
}

std::string HLFltsInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_cond1 = dynamic_cast<Const*>(cond1_);
    auto const_cond2 = dynamic_cast<Const*>(cond2_);
    if(const_cond1 && const_cond2) {
        if(const_cond1->get_fval() < const_cond2->get_fval()) {
            loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), 1);
        } else {    
            loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), 0);
        }
    } else if(const_cond1) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_cond1->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        loongarch_code += H2L::flts(dst_, new Reg(reg_fs1, true), dynamic_cast<Reg*>(cond2_));
    } else if(const_cond2) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_cond2->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        loongarch_code += H2L::flts(dst_, dynamic_cast<Reg*>(cond1_), new Reg(reg_fs1, true));
    } else {
        loongarch_code += H2L::flts(dst_, dynamic_cast<Reg*>(cond1_), dynamic_cast<Reg*>(cond2_));
    }
    return loongarch_code;
}

std::string HLFltsInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + cond1_->print() + ", " + cond2_->print() + H2L::newline;
}

std::string HLFgesInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_cond1 = dynamic_cast<Const*>(cond1_);
    auto const_cond2 = dynamic_cast<Const*>(cond2_);
    if(const_cond1 && const_cond2) {
        if(const_cond1->get_fval() >= const_cond2->get_fval()) {
            loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), 1);
        } else {    
            loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), 0);
        }
    } else if(const_cond1) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_cond1->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        loongarch_code += H2L::fles(dst_, dynamic_cast<Reg*>(cond2_), new Reg(reg_fs1, true));
    } else if(const_cond2) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_cond2->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        loongarch_code += H2L::fles(dst_, new Reg(reg_fs1, true), dynamic_cast<Reg*>(cond1_));
    } else {
        loongarch_code += H2L::fles(dst_, dynamic_cast<Reg*>(cond2_), dynamic_cast<Reg*>(cond1_));
    }
    return loongarch_code;
}

std::string HLFgesInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + cond1_->print() + ", " + cond2_->print() + H2L::newline;
}

std::string HLFgtsInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_cond1 = dynamic_cast<Const*>(cond1_);
    auto const_cond2 = dynamic_cast<Const*>(cond2_);
    if(const_cond1 && const_cond2) {
        if(const_cond1->get_fval() > const_cond2->get_fval()) {
            loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), 1);
        } else {    
            loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), 0);
        }
    } else if(const_cond1) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_cond1->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        loongarch_code += H2L::flts(dst_, dynamic_cast<Reg*>(cond2_), new Reg(reg_fs1, true));
    } else if(const_cond2) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_cond2->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        loongarch_code += H2L::flts(dst_, new Reg(reg_fs1, true), dynamic_cast<Reg*>(cond1_));
    } else {
        loongarch_code += H2L::flts(dst_, dynamic_cast<Reg*>(cond2_), dynamic_cast<Reg*>(cond1_));
    }
    return loongarch_code;
}

std::string HLFgtsInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + cond1_->print() + ", " + cond2_->print() + H2L::newline;
}

std::string HLFnesInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_cond1 = dynamic_cast<Const*>(cond1_);
    auto const_cond2 = dynamic_cast<Const*>(cond2_);
    if(const_cond1 && const_cond2) {
        if(const_cond1->get_fval() != const_cond2->get_fval()) {
            loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), 1);
        } else {    
            loongarch_code += H2L::addi(dst_, new Reg(reg_zero, false), 0);
        }
    } else if(const_cond1) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_cond1->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        loongarch_code += H2L::feqs(new Reg(reg_x, false), new Reg(reg_fs1, true), dynamic_cast<Reg*>(cond2_));
        loongarch_code += H2L::seqz(dst_, new Reg(reg_x, false));
    } else if(const_cond2) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_cond2->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        loongarch_code += H2L::feqs(new Reg(reg_x, false), dynamic_cast<Reg*>(cond1_), new Reg(reg_fs1, true));
        loongarch_code += H2L::seqz(dst_, new Reg(reg_x, false));
    } else {
        loongarch_code += H2L::feqs(new Reg(reg_x, false), dynamic_cast<Reg*>(cond1_), dynamic_cast<Reg*>(cond2_));
        loongarch_code += H2L::seqz(dst_, new Reg(reg_x, false));
    }
    return loongarch_code;
}

std::string HLFnesInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + cond1_->print() + ", " + cond2_->print() + H2L::newline;
}

std::string HLLwInst::get_loongarch_code() {
    std::string loongarch_code;
    if(is_label_) {
        loongarch_code += H2L::la(new Reg(reg_ra, false), label_);
        loongarch_code += H2L::lw(dst_, new Reg(reg_ra, false), 0);
    } else {
        auto const_offset = dynamic_cast<Const*>(offset_);
        if(const_offset) {
            int offset_val = const_offset->get_ival() * 4;
            if(offset_val > 2047 || offset_val < -2048) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), base_, offset_val);
                loongarch_code += H2L::lw(dst_, new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::lw(dst_, base_, offset_val);
            }
        } else {
            loongarch_code += H2L::slliw(new Reg(reg_ra, false), dynamic_cast<Reg*>(offset_), 2);
            loongarch_code += H2L::add(new Reg(reg_ra, false), base_, new Reg(reg_ra, false));
            loongarch_code += H2L::lw(dst_, new Reg(reg_ra, false), 0);
        }
    }
    return loongarch_code;
}

std::string HLLwInst::print() {
    if(is_label_) {
        return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + label_->print() + H2L::newline;
    } else {
        return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + "[" + base_->print() + ", " + offset_->print() + "]" + H2L::newline;
    }
}

std::string HLFlwInst::get_loongarch_code() {
    std::string loongarch_code;
    if(is_label_) {
        loongarch_code += H2L::la(new Reg(reg_ra, false), label_);
        loongarch_code += H2L::flw(dst_, new Reg(reg_ra, false), 0);
    } else {
        auto const_offset = dynamic_cast<Const*>(offset_);
        if(const_offset) {
            int offset_val = const_offset->get_ival() * 4;
            if(offset_val > 2047 || offset_val < -2048) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), base_, offset_val);
                loongarch_code += H2L::flw(dst_, new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(dst_, base_, offset_val);
            }
        } else {
            loongarch_code += H2L::slliw(new Reg(reg_ra, false), dynamic_cast<Reg*>(offset_), 2);
            loongarch_code += H2L::add(new Reg(reg_ra, false), base_, new Reg(reg_ra, false));
            loongarch_code += H2L::flw(dst_, new Reg(reg_ra, false), 0);
        }
    }
    return loongarch_code;
}

std::string HLFlwInst::print() {
    if(is_label_) {
        return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + label_->print() + H2L::newline;
    } else {
        return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + "[" + base_->print() + ", " + offset_->print() + "]" + H2L::newline;
    }
}

std::string HLSwInst::get_loongarch_code() {
    std::string loongarch_code;
    if(is_label_) {
        loongarch_code += H2L::la(new Reg(reg_ra, false), label_);
        if(dynamic_cast<Const*>(val_)) {
            loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(reg_zero, false), dynamic_cast<Const*>(val_)->get_ival());
            loongarch_code += H2L::sw(new Reg(reg_x, false), new Reg(reg_ra, false), 0);
        } else {
            loongarch_code += H2L::sw(dynamic_cast<Reg*>(val_), new Reg(reg_ra, false), 0);
        }
    } else {
        if(dynamic_cast<Const*>(val_)) {
            if(dynamic_cast<Const*>(offset_)) {
                int offset_value = dynamic_cast<Const*>(offset_)->get_ival() * 4;
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(reg_zero, false), dynamic_cast<Const*>(val_)->get_ival());
                if(offset_value > 2047 || offset_value < -2048) {
                    loongarch_code += H2L::addi(new Reg(reg_x, false), base_, offset_value);
                    loongarch_code += H2L::sw(new Reg(reg_ra, false), new Reg(reg_x, false), 0);
                } else {
                    loongarch_code += H2L::sw(new Reg(reg_ra, false), base_, offset_value);
                }
            } else {
                loongarch_code += H2L::slliw(new Reg(reg_ra, false), dynamic_cast<Reg*>(offset_), 2);
                loongarch_code += H2L::add(new Reg(reg_ra, false), new Reg(reg_ra, false), base_);
                loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(reg_zero, false), dynamic_cast<Const*>(val_)->get_ival());
                loongarch_code += H2L::sw(new Reg(reg_x, false), new Reg(reg_ra, false), 0);
            }
        } else {
            if(dynamic_cast<Const*>(offset_)) {
                int offset_value = dynamic_cast<Const*>(offset_)->get_ival() * 4;
                if(offset_value > 2047 || offset_value < -2048) {
                    loongarch_code += H2L::addi(new Reg(reg_ra, false), base_, offset_value);
                    loongarch_code += H2L::sw(dynamic_cast<Reg*>(val_), new Reg(reg_ra, false), 0);
                } else {
                    loongarch_code += H2L::sw(dynamic_cast<Reg*>(val_), base_, offset_value);
                }
            } else {
                loongarch_code += H2L::slliw(new Reg(reg_ra, false), dynamic_cast<Reg*>(offset_), 2);
                loongarch_code += H2L::add(new Reg(reg_ra, false), new Reg(reg_ra, false), base_);
                loongarch_code += H2L::sw(dynamic_cast<Reg*>(val_), new Reg(reg_ra, false), 0);
            }
        }
    }
    return loongarch_code;
}

std::string HLSwInst::print() {
    if(is_label_) {
        return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + val_->print() + ", " + label_->print() + H2L::newline;
    } else {
        return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + val_->print() + ", " + "[" + base_->print() + ", " + offset_->print() + "]" + H2L::newline;
    }
}

std::string HLFswInst::get_loongarch_code() {
    std::string loongarch_code;
    if(is_label_) {
        loongarch_code += H2L::la(new Reg(reg_ra, false), label_);
        if(dynamic_cast<Const*>(val_)) {
            loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(dynamic_cast<Const*>(val_)->get_fval()));
            loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
            loongarch_code += H2L::fsw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
        } else {
            loongarch_code += H2L::fsw(dynamic_cast<Reg*>(val_), new Reg(reg_ra, false), 0);
        }
    } else {
        if(dynamic_cast<Const*>(val_)) {
            loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(dynamic_cast<Const*>(val_)->get_fval()));
            loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
            if(dynamic_cast<Const*>(offset_)) {
                int offset_value = dynamic_cast<Const*>(offset_)->get_ival() * 4;
                if(offset_value > 2047 || offset_value < -2048) {
                    loongarch_code += H2L::addi(new Reg(reg_ra, false), base_, offset_value);
                    loongarch_code += H2L::fsw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
                } else {
                    loongarch_code += H2L::fsw(new Reg(reg_fs1, true), base_, offset_value);
                }
            } else {
                loongarch_code += H2L::slliw(new Reg(reg_ra, false), dynamic_cast<Reg*>(offset_), 2);
                loongarch_code += H2L::add(new Reg(reg_ra, false), new Reg(reg_ra, false), base_);
                loongarch_code += H2L::fsw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            }
        } else {
            if(dynamic_cast<Const*>(offset_)) {
                int offset_value = dynamic_cast<Const*>(offset_)->get_ival() * 4;
                if(offset_value > 2047 || offset_value < -2048) {
                    loongarch_code += H2L::addi(new Reg(reg_ra, false), base_, offset_value);
                    loongarch_code += H2L::fsw(dynamic_cast<Reg*>(val_), new Reg(reg_ra, false), 0);
                } else {
                    loongarch_code += H2L::fsw(dynamic_cast<Reg*>(val_), base_, offset_value);
                }
            } else {
                loongarch_code += H2L::slliw(new Reg(reg_ra, false), dynamic_cast<Reg*>(offset_), 2);
                loongarch_code += H2L::add(new Reg(reg_ra, false), new Reg(reg_ra, false), base_);
                loongarch_code += H2L::fsw(dynamic_cast<Reg*>(val_), new Reg(reg_ra, false), 0);
            }
        }
    }
    return loongarch_code;
}

std::string HLFswInst::print() {
    if(is_label_) {
        return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + val_->print() + ", " + label_->print() + H2L::newline;
    } else {
        return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + val_->print() + ", " + "[" + base_->print() + ", " + offset_->print() + "]" + H2L::newline;
    }
}

std::string HLMemsetInst::get_loongarch_code() {
    std::string loongarch_code;
    loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(base_->get_reg_id(), false), base_->get_offset());
    if(is_fp_) {
        int offset = 0;
        int past_size = 0;
        loongarch_code += H2L::fcvtsw(new Reg(reg_fs1, true), new Reg(reg_zero, false));
        while(past_size < total_size_) {
            if(offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(reg_ra, false), offset-step_size_);
                offset = step_size_;
            }
            loongarch_code += H2L::fsw(new Reg(reg_fs1, true), new Reg(reg_ra, false), offset);
            offset += step_size_;
            past_size += step_size_;
        }
    } else {
        int offset = 0;
        int past_size = 0;
        while(past_size < total_size_) {
            if(offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(reg_ra, false), offset-step_size_);
                offset = step_size_;
            }
            loongarch_code += H2L::sw(new Reg(reg_zero, false), new Reg(reg_ra, false), offset);
            offset += step_size_;
            past_size += step_size_;
        }
    }   
    return loongarch_code;
}

std::string HLMemsetInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + base_->print() + ", " +  std::to_string(total_size_) + H2L::newline;
}

std::string HLCallInst::get_loongarch_code() {
    std::string loongarch_code;
    loongarch_code += H2L::call(label_);
    return loongarch_code;
}

std::string HLCallInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + label_->print() + H2L::newline;
}

std::string HLLaInst::get_loongarch_code() {
    std::string loongarch_code;
    loongarch_code += H2L::la(dst_, label_);
    return loongarch_code;
}

std::string HLLaInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + label_->print() + H2L::newline;
}

std::string HLMvInst::get_loongarch_code() {
    std::string loongarch_code;
    loongarch_code += H2L::mv(dst_, src_);
    return loongarch_code;
}

std::string HLMvInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + dst_->print() + ", " + src_->print() + H2L::newline;
}

std::string HLBeqInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_lval = dynamic_cast<Const*>(lval_);
    auto const_rval = dynamic_cast<Const*>(rval_);
    if(const_lval && const_rval) {
        if(const_lval->get_ival() == const_rval->get_ival()) {
            loongarch_code += H2L::beq(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        } else {
            loongarch_code += H2L::bne(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        }
    } else if(const_lval) {
        loongarch_code += H2L::addiw(new Reg(reg_ra, false), new Reg(reg_zero, false), const_lval->get_ival());
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(reg_x, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::sextw(new Reg(reg_x, false), new Reg(reg_x, false));
            loongarch_code += H2L::beq(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        } else {
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(rval_));
            loongarch_code += H2L::beq(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        }
    } else if(const_rval) {
        loongarch_code += H2L::addiw(new Reg(reg_ra, false), new Reg(reg_zero, false), const_rval->get_ival());
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(reg_x, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::sextw(new Reg(reg_x, false), new Reg(reg_x, false));
            loongarch_code += H2L::beq(new Reg(reg_x, false), new Reg(reg_ra, false), label_);
        } else {
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(lval_));
            loongarch_code += H2L::beq(new Reg(reg_x, false), new Reg(reg_ra, false), label_);
        }
    } else {
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_lval && mem_rval) {
            int offset1 = mem_lval->get_offset();
            int offset2 = mem_rval->get_offset();
            if(offset1 < -2048 || offset1 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset1);
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset1);
            }
            if(offset2 < -2048 || offset2 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(mem_rval->get_reg_id(), false), offset2);
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(reg_x, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(mem_rval->get_reg_id(), false), offset2);
            }
            loongarch_code += H2L::sextw(new Reg(reg_ra, false), new Reg(reg_ra, false));
            loongarch_code += H2L::sextw(new Reg(reg_x, false), new Reg(reg_x, false));
            loongarch_code += H2L::beq(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        } else if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::sextw(new Reg(reg_ra, false), new Reg(reg_ra, false));
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(rval_));
            loongarch_code += H2L::beq(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        } else if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::sextw(new Reg(reg_ra, false), new Reg(reg_ra, false));
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(lval_));
            loongarch_code += H2L::beq(new Reg(reg_x, false), new Reg(reg_ra, false), label_);
        } else {
            loongarch_code += H2L::sextw(new Reg(reg_ra, false), dynamic_cast<Reg*>(lval_));
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(rval_));
            loongarch_code += H2L::beq(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        }
    }
    return loongarch_code;
}

std::string HLBeqInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + lval_->print() + ", " + rval_->print() + ", " + label_->print() + H2L::newline;
}

std::string HLBneInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_lval = dynamic_cast<Const*>(lval_);
    auto const_rval = dynamic_cast<Const*>(rval_);
    if(const_lval && const_rval) {
        if(const_lval->get_ival() != const_rval->get_ival()) {
            loongarch_code += H2L::beq(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        } else {
            loongarch_code += H2L::bne(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        }
    } else if(const_lval) {
        loongarch_code += H2L::addiw(new Reg(reg_ra, false), new Reg(reg_zero, false), const_lval->get_ival());
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(reg_x, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::sextw(new Reg(reg_x, false), new Reg(reg_x, false));
            loongarch_code += H2L::bne(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        } else {
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(rval_));
            loongarch_code += H2L::bne(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        }
    } else if(const_rval) {
        loongarch_code += H2L::addiw(new Reg(reg_ra, false), new Reg(reg_zero, false), const_rval->get_ival());
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(reg_x, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::sextw(new Reg(reg_x, false), new Reg(reg_x, false));
            loongarch_code += H2L::bne(new Reg(reg_x, false), new Reg(reg_ra, false), label_);
        } else {
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(lval_));
            loongarch_code += H2L::bne(new Reg(reg_x, false), new Reg(reg_ra, false), label_);
        }
    } else {
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_lval && mem_rval) {
            int offset1 = mem_lval->get_offset();
            int offset2 = mem_rval->get_offset();
            if(offset1 < -2048 || offset1 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset1);
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset1);
            }
            if(offset2 < -2048 || offset2 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(mem_rval->get_reg_id(), false), offset2);
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(reg_x, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(mem_rval->get_reg_id(), false), offset2);
            }
            loongarch_code += H2L::sextw(new Reg(reg_ra, false), new Reg(reg_ra, false));
            loongarch_code += H2L::sextw(new Reg(reg_x, false), new Reg(reg_x, false));
            loongarch_code += H2L::bne(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        } else if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::sextw(new Reg(reg_ra, false), new Reg(reg_ra, false));
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(rval_));
            loongarch_code += H2L::bne(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        } else if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::sextw(new Reg(reg_ra, false), new Reg(reg_ra, false));
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(lval_));
            loongarch_code += H2L::bne(new Reg(reg_x, false), new Reg(reg_ra, false), label_);
        } else {
            loongarch_code += H2L::sextw(new Reg(reg_ra, false), dynamic_cast<Reg*>(lval_));
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(rval_));
            loongarch_code += H2L::bne(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        }
    }
    return loongarch_code;
}

std::string HLBneInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + lval_->print() + ", " + rval_->print() + ", " + label_->print() + H2L::newline;
}

std::string HLBgeInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_lval = dynamic_cast<Const*>(lval_);
    auto const_rval = dynamic_cast<Const*>(rval_);
    if(const_lval && const_rval) {
        if(const_lval->get_ival() >= const_rval->get_ival()) {
            loongarch_code += H2L::beq(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        } else {
            loongarch_code += H2L::bne(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        }
    } else if(const_lval) {
        loongarch_code += H2L::addiw(new Reg(reg_ra, false), new Reg(reg_zero, false), const_lval->get_ival());
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(reg_x, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::sextw(new Reg(reg_x, false), new Reg(reg_x, false));
            loongarch_code += H2L::bge(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        } else {
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(rval_));
            loongarch_code += H2L::bge(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        }
    } else if(const_rval) {
        loongarch_code += H2L::addiw(new Reg(reg_ra, false), new Reg(reg_zero, false), const_rval->get_ival());
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(reg_x, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::sextw(new Reg(reg_x, false), new Reg(reg_x, false));
            loongarch_code += H2L::bge(new Reg(reg_x, false), new Reg(reg_ra, false), label_);
        } else {
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(lval_));
            loongarch_code += H2L::bge(new Reg(reg_x, false), new Reg(reg_ra, false), label_);
        }
    } else {
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_lval && mem_rval) {
            int offset1 = mem_lval->get_offset();
            int offset2 = mem_rval->get_offset();
            if(offset1 < -2048 || offset1 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset1);
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset1);
            }
            if(offset2 < -2048 || offset2 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(mem_rval->get_reg_id(), false), offset2);
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(reg_x, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(mem_rval->get_reg_id(), false), offset2);
            }
            loongarch_code += H2L::sextw(new Reg(reg_ra, false), new Reg(reg_ra, false));
            loongarch_code += H2L::sextw(new Reg(reg_x, false), new Reg(reg_x, false));
            loongarch_code += H2L::bge(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        } else if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::sextw(new Reg(reg_ra, false), new Reg(reg_ra, false));
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(rval_));
            loongarch_code += H2L::bge(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        } else if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::sextw(new Reg(reg_ra, false), new Reg(reg_ra, false));
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(lval_));
            loongarch_code += H2L::bge(new Reg(reg_x, false), new Reg(reg_ra, false), label_);
        } else {
            loongarch_code += H2L::sextw(new Reg(reg_ra, false), dynamic_cast<Reg*>(lval_));
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(rval_));
            loongarch_code += H2L::bge(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        }
    }
    return loongarch_code;
}

std::string HLBgeInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + lval_->print() + ", " + rval_->print() + ", " + label_->print() + H2L::newline;
}

std::string HLBltInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_lval = dynamic_cast<Const*>(lval_);
    auto const_rval = dynamic_cast<Const*>(rval_);
    if(const_lval && const_rval) {
        if(const_lval->get_ival() < const_rval->get_ival()) {
            loongarch_code += H2L::beq(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        } else {
            loongarch_code += H2L::bne(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        }
    } else if(const_lval) {
        loongarch_code += H2L::addiw(new Reg(reg_ra, false), new Reg(reg_zero, false), const_lval->get_ival());
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(reg_x, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::sextw(new Reg(reg_x, false), new Reg(reg_x, false));
            loongarch_code += H2L::blt(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        } else {
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(rval_));
            loongarch_code += H2L::blt(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        }
    } else if(const_rval) {
        loongarch_code += H2L::addiw(new Reg(reg_ra, false), new Reg(reg_zero, false), const_rval->get_ival());
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(reg_x, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::sextw(new Reg(reg_x, false), new Reg(reg_x, false));
            loongarch_code += H2L::blt(new Reg(reg_x, false), new Reg(reg_ra, false), label_);
        } else {
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(lval_));
            loongarch_code += H2L::blt(new Reg(reg_x, false), new Reg(reg_ra, false), label_);
        }
    } else {
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_lval && mem_rval) {
            int offset1 = mem_lval->get_offset();
            int offset2 = mem_rval->get_offset();
            if(offset1 < -2048 || offset1 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset1);
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset1);
            }
            if(offset2 < -2048 || offset2 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_x, false), new Reg(mem_rval->get_reg_id(), false), offset2);
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(reg_x, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_x, false), new Reg(mem_rval->get_reg_id(), false), offset2);
            }
            loongarch_code += H2L::sextw(new Reg(reg_ra, false), new Reg(reg_ra, false));
            loongarch_code += H2L::sextw(new Reg(reg_x, false), new Reg(reg_x, false));
            loongarch_code += H2L::blt(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        } else if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::sextw(new Reg(reg_ra, false), new Reg(reg_ra, false));
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(rval_));
            loongarch_code += H2L::blt(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        } else if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::ld(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::sextw(new Reg(reg_ra, false), new Reg(reg_ra, false));
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(lval_));
            loongarch_code += H2L::blt(new Reg(reg_x, false), new Reg(reg_ra, false), label_);
        } else {
            loongarch_code += H2L::sextw(new Reg(reg_ra, false), dynamic_cast<Reg*>(lval_));
            loongarch_code += H2L::sextw(new Reg(reg_x, false), dynamic_cast<Reg*>(rval_));
            loongarch_code += H2L::blt(new Reg(reg_ra, false), new Reg(reg_x, false), label_);
        }
    }
    return loongarch_code;
}

std::string HLBltInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + lval_->print() + ", " + rval_->print() + ", " + label_->print() + H2L::newline;
}

std::string HLFBeqInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_lval = dynamic_cast<Const*>(lval_);
    auto const_rval = dynamic_cast<Const*>(rval_);
    if(const_lval && const_rval) {
        if(const_lval->get_fval() == const_rval->get_fval()) {
            loongarch_code += H2L::beq(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        } else {
            loongarch_code += H2L::bne(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        }
    } else if(const_lval) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_lval->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::feqs(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), new Reg(reg_fs0, true));
        } else {
            loongarch_code += H2L::feqs(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), dynamic_cast<Reg*>(rval_));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    } else if(const_rval) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_rval->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::feqs(new Reg(reg_fcc0, false,true), new Reg(reg_fs0, true), new Reg(reg_fs1, true));
        } else {
            loongarch_code += H2L::feqs(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(lval_), new Reg(reg_fs1, true));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    } else {
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_lval && mem_rval) {
            int offset1 = mem_lval->get_offset();
            int offset2 = mem_rval->get_offset();
            if(offset1 < -2048 || offset1 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset1);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_lval->get_reg_id(), false), offset1);
            }
            if(offset2 < -2048 || offset2 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset2);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_rval->get_reg_id(), false), offset2);
            }
            loongarch_code += H2L::feqs(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), new Reg(reg_fs0, true));
        } else if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::feqs(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), dynamic_cast<Reg*>(rval_));
        } else if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::feqs(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(lval_), new Reg(reg_fs1, true));
        } else {
            loongarch_code += H2L::feqs(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(lval_), dynamic_cast<Reg*>(rval_));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    }

    return loongarch_code;
}

std::string HLFBeqInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + lval_->print() + ", " + rval_->print() + ", " + label_->print() + H2L::newline;
}


std::string HLFBgeInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_lval = dynamic_cast<Const*>(lval_);
    auto const_rval = dynamic_cast<Const*>(rval_);
    if(const_lval && const_rval) {
        if(const_lval->get_fval() >= const_rval->get_fval()) {
            loongarch_code += H2L::beq(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        } else {
            loongarch_code += H2L::bne(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        }
    } else if(const_lval) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_lval->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::fles(new Reg(reg_fcc0, false,true), new Reg(reg_fs0, true), new Reg(reg_fs1, true));
        } else {
            loongarch_code += H2L::fles(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(rval_), new Reg(reg_fs1, true));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    } else if(const_rval) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_rval->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::fles(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), new Reg(reg_fs0, true));
        } else {
            loongarch_code += H2L::fles(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), dynamic_cast<Reg*>(lval_));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    } else {
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_lval && mem_rval) {
            int offset1 = mem_lval->get_offset();
            int offset2 = mem_rval->get_offset();
            if(offset1 < -2048 || offset1 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset1);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_lval->get_reg_id(), false), offset1);
            }
            if(offset2 < -2048 || offset2 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset2);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_rval->get_reg_id(), false), offset2);
            }
            loongarch_code += H2L::fles(new Reg(reg_fcc0, false,true), new Reg(reg_fs0, true), new Reg(reg_fs1, true));
        } else if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::fles(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(rval_), new Reg(reg_fs1, true));
        } else if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::fles(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), dynamic_cast<Reg*>(lval_));
        } else {
            loongarch_code += H2L::fles(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(rval_), dynamic_cast<Reg*>(lval_));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    }

    return loongarch_code;
}

std::string HLFBgeInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + lval_->print() + ", " + rval_->print() + ", " + label_->print() + H2L::newline;
}

std::string HLFBgtInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_lval = dynamic_cast<Const*>(lval_);
    auto const_rval = dynamic_cast<Const*>(rval_);
    if(const_lval && const_rval) {
        if(const_lval->get_fval() > const_rval->get_fval()) {
            loongarch_code += H2L::beq(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        } else {
            loongarch_code += H2L::bne(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        }
    } else if(const_lval) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_lval->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::flts(new Reg(reg_fcc0, false,true), new Reg(reg_fs0, true), new Reg(reg_fs1, true));
        } else {
            loongarch_code += H2L::flts(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(rval_), new Reg(reg_fs1, true));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    } else if(const_rval) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_rval->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::flts(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), new Reg(reg_fs0, true));
        } else {
            loongarch_code += H2L::flts(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), dynamic_cast<Reg*>(lval_));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    } else {
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_lval && mem_rval) {
            int offset1 = mem_lval->get_offset();
            int offset2 = mem_rval->get_offset();
            if(offset1 < -2048 || offset1 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset1);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_lval->get_reg_id(), false), offset1);
            }
            if(offset2 < -2048 || offset2 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset2);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_rval->get_reg_id(), false), offset2);
            }
            loongarch_code += H2L::flts(new Reg(reg_fcc0, false,true), new Reg(reg_fs0, true), new Reg(reg_fs1, true));
        } else if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::flts(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(rval_), new Reg(reg_fs1, true));
        } else if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::flts(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), dynamic_cast<Reg*>(lval_));
        } else {
            loongarch_code += H2L::flts(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(rval_), dynamic_cast<Reg*>(lval_));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    }

    return loongarch_code;
}

std::string HLFBgtInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + lval_->print() + ", " + rval_->print() + ", " + label_->print() + H2L::newline;
}

std::string HLFBleInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_lval = dynamic_cast<Const*>(lval_);
    auto const_rval = dynamic_cast<Const*>(rval_);
    if(const_lval && const_rval) {
        if(const_lval->get_fval() <= const_rval->get_fval()) {
            loongarch_code += H2L::beq(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        } else {
            loongarch_code += H2L::bne(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        }
    } else if(const_lval) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_lval->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::fles(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), new Reg(reg_fs0, true));
        } else {
            loongarch_code += H2L::fles(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), dynamic_cast<Reg*>(rval_));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    } else if(const_rval) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_rval->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::fles(new Reg(reg_fcc0, false,true), new Reg(reg_fs0, true), new Reg(reg_fs1, true));
        } else {
            loongarch_code += H2L::fles(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(lval_), new Reg(reg_fs1, true));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    } else {
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_lval && mem_rval) {
            int offset1 = mem_lval->get_offset();
            int offset2 = mem_rval->get_offset();
            if(offset1 < -2048 || offset1 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset1);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_lval->get_reg_id(), false), offset1);
            }
            if(offset2 < -2048 || offset2 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset2);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_rval->get_reg_id(), false), offset2);
            }
            loongarch_code += H2L::fles(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), new Reg(reg_fs0, true));
        } else if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::fles(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), dynamic_cast<Reg*>(rval_));
        } else if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::fles(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(lval_), new Reg(reg_fs1, true));
        } else {
            loongarch_code += H2L::fles(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(lval_), dynamic_cast<Reg*>(rval_));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    }

    return loongarch_code;
}

std::string HLFBleInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + lval_->print() + ", " + rval_->print() + ", " + label_->print() + H2L::newline;
}

std::string HLFBltInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_lval = dynamic_cast<Const*>(lval_);
    auto const_rval = dynamic_cast<Const*>(rval_);
    if(const_lval && const_rval) {
        if(const_lval->get_fval() < const_rval->get_fval()) {
            loongarch_code += H2L::beq(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        } else {
            loongarch_code += H2L::bne(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        }
    } else if(const_lval) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_lval->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::flts(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), new Reg(reg_fs0, true));
        } else {
            loongarch_code += H2L::flts(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), dynamic_cast<Reg*>(rval_));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    } else if(const_rval) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_rval->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::flts(new Reg(reg_fcc0, false,true), new Reg(reg_fs0, true), new Reg(reg_fs1, true));
        } else {
            loongarch_code += H2L::flts(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(lval_), new Reg(reg_fs1, true));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true) , label_);
    } else {
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_lval && mem_rval) {
            int offset1 = mem_lval->get_offset();
            int offset2 = mem_rval->get_offset();
            if(offset1 < -2048 || offset1 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset1);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_lval->get_reg_id(), false), offset1);
            }
            if(offset2 < -2048 || offset2 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset2);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_rval->get_reg_id(), false), offset2);
            }
            loongarch_code += H2L::flts(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), new Reg(reg_fs0, true));
        } else if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::flts(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), dynamic_cast<Reg*>(rval_));
        } else if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::flts(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(lval_), new Reg(reg_fs1, true));
        } else {
            loongarch_code += H2L::flts(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(lval_), dynamic_cast<Reg*>(rval_));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    }

    return loongarch_code;
}
std::string HLFBltInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + lval_->print() + ", " + rval_->print() + ", " + label_->print() + H2L::newline;
}


std::string HLFBneInst::get_loongarch_code() {
    std::string loongarch_code;
    auto const_lval = dynamic_cast<Const*>(lval_);
    auto const_rval = dynamic_cast<Const*>(rval_);
    if(const_lval && const_rval) {
        if(const_lval->get_fval() != const_rval->get_fval()) {
            loongarch_code += H2L::beq(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        } else {
            loongarch_code += H2L::bne(new Reg(reg_zero, false), new Reg(reg_zero, false), label_);
        }
    } else if(const_lval) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_lval->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::feqs(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), new Reg(reg_fs0, true));
        } else {
            loongarch_code += H2L::feqs(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), dynamic_cast<Reg*>(rval_));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    } else if(const_rval) {
        loongarch_code += H2L::li(new Reg(reg_x, false), *(uint32_t*)&(const_rval->get_fval()));
        loongarch_code += H2L::fmvsx(new Reg(reg_fs1, true), new Reg(reg_x, false));
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::feqs(new Reg(reg_fcc0, false,true), new Reg(reg_fs0, true), new Reg(reg_fs1, true));
        } else {
            loongarch_code += H2L::feqs(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(lval_), new Reg(reg_fs1, true));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    } else {
        auto mem_lval = dynamic_cast<Mem*>(lval_);
        auto mem_rval = dynamic_cast<Mem*>(rval_);
        if(mem_lval && mem_rval) {
            int offset1 = mem_lval->get_offset();
            int offset2 = mem_rval->get_offset();
            if(offset1 < -2048 || offset1 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset1);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_lval->get_reg_id(), false), offset1);
            }
            if(offset2 < -2048 || offset2 > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset2);
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs0, true), new Reg(mem_rval->get_reg_id(), false), offset2);
            }
            loongarch_code += H2L::feqs(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), new Reg(reg_fs0, true));
        } else if(mem_lval) {
            int offset = mem_lval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_lval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_lval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::feqs(new Reg(reg_fcc0, false,true), new Reg(reg_fs1, true), dynamic_cast<Reg*>(rval_));
        } else if(mem_rval) {
            int offset = mem_rval->get_offset();
            if(offset < -2048 || offset > 2047) {
                loongarch_code += H2L::addi(new Reg(reg_ra, false), new Reg(mem_rval->get_reg_id(), false), offset);
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(reg_ra, false), 0);
            } else {
                loongarch_code += H2L::flw(new Reg(reg_fs1, true), new Reg(mem_rval->get_reg_id(), false), offset);
            }
            loongarch_code += H2L::feqs(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(lval_), new Reg(reg_fs1, true));
        } else {
            loongarch_code += H2L::feqs(new Reg(reg_fcc0, false,true), dynamic_cast<Reg*>(lval_), dynamic_cast<Reg*>(rval_));
        }
        loongarch_code += H2L::bceqz(new Reg(reg_fcc0, false,true), label_);
    }

    return loongarch_code;
}

std::string HLFBneInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + lval_->print() + ", " + rval_->print() + ", " + label_->print() + H2L::newline;
}

std::string HLJInst::get_loongarch_code() {
    std::string loongarch_code;
    loongarch_code += H2L::j(label_);
    return loongarch_code;
}

std::string HLJInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::space + label_->print() + H2L::newline;
}

std::string HLRetInst::get_loongarch_code() {
    std::string loongarch_code;
    loongarch_code += H2L::ret();
    return loongarch_code;
}

std::string HLRetInst::print() {
    return H2L::space + get_instr_op_name(get_hloongarchinst_op()) + H2L::newline;
}