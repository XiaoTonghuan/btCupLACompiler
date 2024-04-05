#include "../include/Hloongarch/H2L.hpp"
#include "../include/Hloongarch/HLLoc.hpp"
#include "../include/Hloongarch/HLValue.hpp"

#define IMM_12_MAX 0x7FF
#define IMM_12_MIN -0x800

#define LOW_12_MASK 0x00000FFF
#define LOW_20_MASK 0x000FFFFF
#define LOW_32_MASK 0xFFFFFFFF
#define __L_TRIPLE_REG_OP__(op, rd, rs1, rs2)         \
        (H2L::space + (op) + H2L::space + (rd)->get_loongarch_code() + ", " + (rs1)->get_loongarch_code() + ", "+ (rs2)->get_loongarch_code() + H2L::newline) 

namespace H2L {

//& immediate instructions
//& 外部调用保证rs不可能为s1号寄存器
std::string addi(Reg *rd, Reg *rs, int imm) {
    std::string loongarch_code;
    Reg* tmp_s1 = new Reg(reg_x, false);
    if(imm <= 2047 && imm >= -2048) {
        loongarch_code += H2L::space + "addi.d" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    } else {
        loongarch_code += H2L::li(tmp_s1, imm);
        loongarch_code += __L_TRIPLE_REG_OP__("add.d", rd, rs, tmp_s1);
    }
    return loongarch_code;
}

std::string addiw(Reg *rd, Reg *rs, int imm) {
    std::string loongarch_code;
    Reg* tmp_s1 = new Reg(reg_x, false);
    if(imm <= 2047 && imm >= -2048) {
        loongarch_code += H2L::space + "addi.w" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    } else {
        loongarch_code += H2L::li(tmp_s1, imm);
        loongarch_code += __L_TRIPLE_REG_OP__("add.w", rd, rs, tmp_s1);
    }
    return loongarch_code;
}

std::string li(Reg *rd, int64_t imm) {
    std::string loongarch_code;
    auto low_32 = static_cast<int32_t>(imm & LOW_32_MASK);
    H2L::li(rd, low_32);
    auto high_32 = static_cast<int32_t>(imm >> 32);
    int32_t high_32_low_20 = (high_32 << 12) >> 12; // si20
    int32_t high_32_high_12 = high_32 >> 20;        // si12
    loongarch_code += H2L::space + "lu32i.d" + H2L::space + rd->get_loongarch_code() + ", " + std::to_string(high_32_low_20) + H2L::newline;
    loongarch_code += H2L::space + "lu52i.d" + H2L::space + rd->get_loongarch_code() + ", " +rd->get_loongarch_code() + ", "+ std::to_string(high_32_high_12) + H2L::newline;
    return loongarch_code;
}

std::string li(Reg *rd, int32_t fliteral) {
    std::string loongarch_code;
    int32_t high_20 = fliteral >> 12; // si20
    uint32_t low_12 = fliteral & LOW_12_MASK;
    loongarch_code += H2L::space + "lu12i.w" + H2L::space + rd->get_loongarch_code() + ", " + std::to_string(high_20) + H2L::newline;
    loongarch_code += H2L::space + "ori" + H2L::space + rd->get_loongarch_code() + ", " + rd->get_loongarch_code() + ", "+ std::to_string(low_12) + H2L::newline;
    return loongarch_code;
}


//& 3 regs alu instructions
std::string add(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("add.d", rd, rs1, rs2);
    return loongarch_code;
}

std::string addw(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("add.w", rd, rs1, rs2);
    return loongarch_code;
}

std::string subw(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("sub.w", rd, rs1, rs2);
    return loongarch_code;
}

std::string mulw(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("mul.w", rd, rs1, rs2);
    return loongarch_code;
}

std::string mul(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("mul.d", rd, rs1, rs2);
    return loongarch_code;
}

std::string divw(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("div.w", rd, rs1, rs2);
    return loongarch_code;
}


std::string remw(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("mod.w", rd, rs1, rs2);
    return loongarch_code;
}

std::string slliw(Reg *rd, Reg *rs, int imm) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "slli.w" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    return loongarch_code;
}


std::string slli(Reg *rd, Reg *rs, int imm) {
    std::string code;
    code += H2L::space + "slli.d" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    return code;
}

std::string sraiw(Reg *rd, Reg *rs, int imm) {
     std::string loongarch_code;
    loongarch_code += H2L::space + "srai.w" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    return loongarch_code;
}

std::string srai(Reg *rd, Reg *rs, int imm) {
     std::string code;
    code += H2L::space + "srai.d" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    return code;
}

std::string srliw(Reg *rd, Reg *rs, int imm) {
     std::string loongarch_code;
    loongarch_code += H2L::space + "srli.w" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    return loongarch_code;
}

std::string srli(Reg *rd, Reg *rs, int imm) {
     std::string code;
    code += H2L::space + "srli.d" + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm) + H2L::newline;
    return code;
}

std::string fadds(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("fadd.s", rd, rs1, rs2);
    return loongarch_code;
}

std::string fsubs(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("fsub.s", rd, rs1, rs2);
    return loongarch_code;
}

std::string fmuls(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("fmul.s", rd, rs1, rs2);
    return loongarch_code;
}

std::string fdivs(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("fdiv.s", rd, rs1, rs2);
    return loongarch_code;
}

std::string fnegs(Reg *rd) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "fneg.s	" + H2L::space + rd->get_loongarch_code() + ", " + rd->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

//& logical ops
std::string andi(Reg *rd, Reg *rs, int imm) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "andi " + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(imm)+ H2L::newline;
    return loongarch_code;
}

std::string land(Reg *rd, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("and", rd, rs1, rs2);
    return loongarch_code;
}

//& cmp ops
std::string seqz(Reg *rd, Reg *rs) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "sltui " + H2L::space + rd->get_loongarch_code() + ", " + rs->get_loongarch_code() + ", " + std::to_string(1)+ H2L::newline;
    return loongarch_code;
}

std::string snez(Reg *rd, Reg *rs) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("sltu", rd,new Reg(reg_zero,false), rs);
    return loongarch_code;
}

std::string feqs(Reg *rd1, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("fcmp.seq.s", new Reg(reg_fcc0,false,true), rs1, rs2);
    return loongarch_code;
}

std::string fles(Reg *rd1, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("fcmp.sle.s", rd1, rs1, rs2);
    return loongarch_code;
}

std::string flts(Reg *rd1, Reg *rs1, Reg *rs2) {
    std::string loongarch_code;
    loongarch_code += __L_TRIPLE_REG_OP__("fcmp.slt.s", rd1, rs1, rs2);
    return loongarch_code;
}

//& cond branches

std::string beq(Reg *rs1, Reg *rs2, Label *label) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "beq" + H2L::space + rs1->get_loongarch_code() + ", " + rs2->get_loongarch_code() + ", " + label->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

std::string bne(Reg *rs1, Reg *rs2, Label *label) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "bne" + H2L::space + rs1->get_loongarch_code() + ", " + rs2->get_loongarch_code() + ", " + label->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

std::string bge(Reg *rs1, Reg *rs2, Label *label) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "bge" + H2L::space + rs1->get_loongarch_code() + ", " + rs2->get_loongarch_code() + ", " + label->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

std::string blt(Reg *rs1, Reg *rs2, Label *label) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "blt" + H2L::space + rs1->get_loongarch_code() + ", " + rs2->get_loongarch_code() + ", " + label->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

std::string bceqz(Reg *rs1, Label *label) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "bceqz" + H2L::space + rs1->get_loongarch_code() + ", "  + label->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}
std::string bcnez(Reg *rs1, Label *label) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "bcnez" + H2L::space + rs1->get_loongarch_code() + ", "  + label->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}


//& mem ops
std::string sd(Reg *src, Reg *base, int offset) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "st.d" + H2L::space + src->get_loongarch_code() + ", "  + base->get_loongarch_code() + "," +std::to_string(offset) + H2L::newline;
    return loongarch_code;
}

std::string ld(Reg *dst, Reg *base, int offset) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "ld.d" + H2L::space + dst->get_loongarch_code() + ", " + base->get_loongarch_code() +"," + std::to_string(offset) + H2L::newline;
    return loongarch_code;
}

std::string sw(Reg *src, Reg *base, int offset) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "st.w" + H2L::space + src->get_loongarch_code() + ", "  + base->get_loongarch_code() + "," +std::to_string(offset) + H2L::newline;
    return loongarch_code;
}

std::string lw(Reg *dst, Reg *base, int offset) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "ld.w" + H2L::space + dst->get_loongarch_code() + ", " + base->get_loongarch_code() +"," + std::to_string(offset) + H2L::newline;
    return loongarch_code;
}

std::string fsw(Reg *src, Reg *base, int offset) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "fst.s" + H2L::space + src->get_loongarch_code() + ", " + base->get_loongarch_code() +"," + std::to_string(offset) + H2L::newline;
    return loongarch_code;
}

std::string flw(Reg *dst, Reg *base, int offset) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "fld.s" + H2L::space + dst->get_loongarch_code() + ", " + base->get_loongarch_code() +"," + std::to_string(offset) + H2L::newline;
    return loongarch_code;
}

//& mv ops
std::string mv(Reg *dst, Reg *src) {
    std::string loongarch_code;
    if(dst->get_id() == src->get_id())
        return loongarch_code;
    loongarch_code+= H2L::addi(dst,src,0);
    return loongarch_code;
} 

std::string fmvs(Reg *dst, Reg *src) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "fmov.s" + H2L::space + dst->get_loongarch_code() + ", " + src->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

//& cast move
std::string fmvsx(Reg *dst, Reg *src) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "movgr2fr.w" + H2L::space + dst->get_loongarch_code() + ", " + src->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

//& unconditional branch
std::string j(Label *label) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "b" + H2L::space + label->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}


//& cast ops
//借用fs1,外部确保不会使用fs1
std::string fcvtsw(Reg *dst, Reg *src) {
    std::string loongarch_code;
    Reg* tmp_s1 = new Reg(reg_fs1, true);
    loongarch_code += H2L::space + "movgr2fr.w" + H2L::space + tmp_s1->get_loongarch_code() + ", " + src->get_loongarch_code() + H2L::newline;
    loongarch_code += H2L::space + "ffint.s.w" + H2L::space + dst->get_loongarch_code() + ", " + tmp_s1->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}
//借用fs1
std::string fcvtws(Reg *dst, Reg *src) {
    std::string loongarch_code;
    Reg* tmp_s1 = new Reg(reg_fs1, true);
    loongarch_code += H2L::space + "ftintrz.w.s" + H2L::space + tmp_s1->get_loongarch_code() + ", " + src->get_loongarch_code() + H2L::newline;
    loongarch_code += H2L::space + "movfr2gr.s" + H2L::space + dst->get_loongarch_code() + ", " + tmp_s1->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}


//& load address
std::string la(Reg *rd, Label *label) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "la.local" + H2L::space + rd->get_loongarch_code() + ", " + label->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

//& sext
std::string sextw(Reg *rd, Reg *rs) {
    std::string loongarch_code;
    loongarch_code+=H2L::addiw(rd,rs,0);
    return loongarch_code;
}

//& ret
std::string ret() {
    std::string loongarch_code;
    loongarch_code += H2L::space + "jr" + H2L::space+(new Reg(reg_ra,false))->get_loongarch_code()+H2L::newline;
    return loongarch_code;
}

//& call
std::string call(Label *label) {
    std::string loongarch_code;
    loongarch_code += H2L::space + "bl" + H2L::space + label->get_loongarch_code() + H2L::newline;
    return loongarch_code;
}

}