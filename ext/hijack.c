/* -*- mode: c; coding: utf-8; c-basic-offset: 2; indent-tabs-mode: nil; -*- */

#include <ruby.h>
#include <env.h>
#include <node.h>

static void (*blk_mark)(struct BLOCK*);
static void (*blk_free)(struct BLOCK*);

/* from */
struct BLOCK {
    NODE *var;
    NODE *body;
    VALUE self;
    struct FRAME frame;
    struct SCOPE *scope;
    VALUE klass;
    NODE *cref;
    int iter;
    int vmode;
    int flags;
    int uniq;
    struct RVarmap *dyna_vars;
    VALUE orig_thread;
    VALUE wrapper;
    VALUE block_obj;
    struct BLOCK *outer;
    struct BLOCK *prev;
};

static void
print_node_name(enum node_type nt)
{
  switch (nt) {
#define CASE_NODE(type)    \
  case NODE_ ## type:      \
    printf("[NODE_" #type "]\n"); \
    break
    CASE_NODE(METHOD);
    CASE_NODE(FBODY);
    CASE_NODE(CFUNC);
    CASE_NODE(SCOPE);
    CASE_NODE(BLOCK);
    CASE_NODE(IF);
    CASE_NODE(CASE);
    CASE_NODE(WHEN);
    CASE_NODE(OPT_N);
    CASE_NODE(WHILE);
    CASE_NODE(UNTIL);
    CASE_NODE(ITER);
    CASE_NODE(FOR);
    CASE_NODE(BREAK);
    CASE_NODE(NEXT);
    CASE_NODE(REDO);
    CASE_NODE(RETRY);
    CASE_NODE(BEGIN);
    CASE_NODE(RESCUE);
    CASE_NODE(RESBODY);
    CASE_NODE(ENSURE);
    CASE_NODE(AND);
    CASE_NODE(OR);
    CASE_NODE(NOT);
    CASE_NODE(MASGN);
    CASE_NODE(LASGN);
    CASE_NODE(DASGN);
    CASE_NODE(DASGN_CURR);
    CASE_NODE(GASGN);
    CASE_NODE(IASGN);
    CASE_NODE(CDECL);
    CASE_NODE(CVASGN);
    CASE_NODE(CVDECL);
    CASE_NODE(OP_ASGN1);
    CASE_NODE(OP_ASGN2);
    CASE_NODE(OP_ASGN_AND);
    CASE_NODE(OP_ASGN_OR);
    CASE_NODE(CALL);
    CASE_NODE(FCALL);
    CASE_NODE(VCALL);
    CASE_NODE(SUPER);
    CASE_NODE(ZSUPER);
    CASE_NODE(ARRAY);
    CASE_NODE(ZARRAY);
    CASE_NODE(HASH);
    CASE_NODE(RETURN);
    CASE_NODE(YIELD);
    CASE_NODE(LVAR);
    CASE_NODE(DVAR);
    CASE_NODE(GVAR);
    CASE_NODE(IVAR);
    CASE_NODE(CONST);
    CASE_NODE(CVAR);
    CASE_NODE(NTH_REF);
    CASE_NODE(BACK_REF);
    CASE_NODE(MATCH);
    CASE_NODE(MATCH2);
    CASE_NODE(MATCH3);
    CASE_NODE(LIT);
    CASE_NODE(STR);
    CASE_NODE(DSTR);
    CASE_NODE(XSTR);
    CASE_NODE(DXSTR);
    CASE_NODE(EVSTR);
    CASE_NODE(DREGX);
    CASE_NODE(DREGX_ONCE);
    CASE_NODE(ARGS);
    CASE_NODE(ARGSCAT);
    CASE_NODE(ARGSPUSH);
    CASE_NODE(SPLAT);
    CASE_NODE(TO_ARY);
    CASE_NODE(SVALUE);
    CASE_NODE(BLOCK_ARG);
    CASE_NODE(BLOCK_PASS);
    CASE_NODE(DEFN);
    CASE_NODE(DEFS);
    CASE_NODE(ALIAS);
    CASE_NODE(VALIAS);
    CASE_NODE(UNDEF);
    CASE_NODE(CLASS);
    CASE_NODE(MODULE);
    CASE_NODE(SCLASS);
    CASE_NODE(COLON2);
    CASE_NODE(COLON3);
    CASE_NODE(CREF);
    CASE_NODE(DOT2);
    CASE_NODE(DOT3);
    CASE_NODE(FLIP2);
    CASE_NODE(FLIP3);
    CASE_NODE(ATTRSET);
    CASE_NODE(SELF);
    CASE_NODE(NIL);
    CASE_NODE(TRUE);
    CASE_NODE(FALSE);
    CASE_NODE(DEFINED);
    CASE_NODE(NEWLINE);
    CASE_NODE(POSTEXE);
    CASE_NODE(ALLOCA);
    CASE_NODE(DMETHOD);
    CASE_NODE(BMETHOD);
    CASE_NODE(MEMO);
    CASE_NODE(IFUNC);
    CASE_NODE(DSYM);
    CASE_NODE(ATTRASGN);
  }
}

static struct RNode*
find_def_node(struct RNode* nd)
{
  while (nd) {
    switch (nd_type(nd)) {
    case NODE_DEFN: case NODE_DEFS:
      return nd;
    case NODE_BEGIN:
      nd = nd->nd_body;
      break;
    case NODE_NEWLINE:
      nd = nd->nd_next;
      break;
    default:
      print_node_name(nd_type(nd));
      return NULL;
    }
  }
  return NULL;
}

static VALUE
f_hijack(VALUE self)
{
  VALUE proc, ary;
  struct BLOCK* blk;
  NODE* nd;
  NODE* block;
  NODE* args;
  NODE* body;
  int argc;
  ID mid;
  if (!rb_block_given_p()) return Qnil;
  proc = rb_block_proc();
  Data_Get_Struct(proc, struct BLOCK, blk);
  nd = find_def_node(blk->body);
  if (!nd->nd_defn) return Qnil;
  if (nd_type(nd->nd_defn) != NODE_SCOPE) return Qnil;
  /*
   * nd_mid: an id of the name of the method
   * nd_defn: scope node for the method
   */
  mid = nd->nd_mid;
  block = nd->nd_defn->nd_next;
  proc = Data_Wrap_Struct(rb_cProc, blk_mark, blk_free, block);
  return proc;
}

void
Init_hijack(void)
{
  VALUE proc;
  rb_define_global_function("hijack", f_hijack, 0);

  proc = rb_eval_string("lambda {}");
  blk_mark = (void (*)(struct BLOCK*)) RDATA(proc)->dmark;
  blk_free = (void (*)(struct BLOCK*)) RDATA(proc)->dfree;
}
