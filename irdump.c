#include "9cc.h"

static int regno(Reg *r) {
  if (!r)
    return 0;
  if (r->rn != -1)
    return r->rn;
  return r->vn;
}

static char *tostr_call(IR *ir) {
  StringBuilder *sb = new_sb();
  sb_append(sb, format("r%d = %s(", regno(ir->r0), ir->name));
  for (int i = 0; i < ir->nargs; i++) {
    if (i != 0)
      sb_append(sb, ", ");
    sb_append(sb, format("r%d", regno(ir->args[i])));
  }
  sb_append(sb, ")");
  return sb_get(sb);
}

static char *tostr(IR *ir) {
  int r0 = regno(ir->r0);
  int r1 = regno(ir->r1);
  int r2 = regno(ir->r2);

  switch (ir->op) {
  case IR_ADD:
    return format("r%d = r%d + r%d", r0, r1, r2);
  case IR_CALL:
    return tostr_call(ir);
  case IR_DIV:
    return format("r%d = r%d / r%d", r0, r1, r2);
  case IR_IMM:
    return format("r%d = %d", r0, ir->imm);
  case IR_JMP:
    if (ir->bbarg)
      return format("JMP .L%d (r%d)", ir->bb1->label, regno(ir->bbarg));
    return format("JMP .L%d", ir->bb1->label);
  case IR_LABEL_ADDR:
    return format("r%d = .L%d", r0, ir->label);
  case IR_EQ:
    return format("r%d = r%d == r%d", r0, r1, r2);
  case IR_NE:
    return format("r%d = r%d != r%d", r0, r1, r2);
  case IR_LE:
    return format("r%d = r%d <= r%d", r0, r1, r2);
  case IR_LT:
    return format("r%d = r%d < r%d", r0, r1, r2);
  case IR_AND:
    return format("r%d = r%d & r%d", r0, r1, r2);
  case IR_OR:
    return format("r%d = r%d | r%d", r0, r1, r2);
  case IR_XOR:
    return format("r%d = r%d ^ r%d", r0, r1, r2);
  case IR_SHL:
    return format("r%d = r%d << r%d", r0, r1, r2);
  case IR_SHR:
    return format("r%d = r%d >> r%d", r0, r1, r2);
  case IR_LOAD:
    return format("LOAD%d r%d, r%d", ir->size, r0, r2);
  case IR_MOD:
    return format("r%d = r%d %% r%d", r0, r1, r2);
  case IR_MOV:
    return format("r%d = r%d", r0, r2);
  case IR_MUL:
    return format("r%d = r%d * r%d", r0, r1, r2);
  case IR_NOP:
    return "NOP";
  case IR_RETURN:
    return format("RET r%d", r0);
  case IR_STORE:
    return format("STORE%d r%d, r%d", ir->size, r1, r2);
  case IR_STORE_ARG:
    return format("STORE_ARG%d %d, %d", ir->size, ir->imm, ir->imm2);
  case IR_SUB:
    return format("r%d = r%d - r%d", r0, r1, r2);
  case IR_BPREL:
    return format("BPREL r%d %d", r0, ir->imm);
  case IR_BR:
    return format("BR r%d .L%d .L%d", r2, ir->bb1->label, ir->bb2->label);
  default:
    assert(0 && "unknown op");
  }
}

void dump_ir(Vector *irv) {
  for (int i = 0; i < irv->len; i++) {
    Function *fn = irv->data[i];
    fprintf(stderr, "%s:\n", fn->name);

    for (int i = 0; i < fn->bbs->len; i++) {
      BB *bb = fn->bbs->data[i];

      if (bb->param)
	fprintf(stderr, ".L%d(r%d):\n", bb->label, regno(bb->param));
      else
	fprintf(stderr, ".L%d:\n", bb->label);

      for (int i = 0; i < bb->ir->len; i++) {
        IR *ir = bb->ir->data[i];
        fprintf(stderr, "\t%s\n", tostr(ir));
      }
    }
  }
}
