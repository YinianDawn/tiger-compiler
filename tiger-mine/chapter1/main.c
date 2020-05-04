/* This file is intentionally empty.  You should fill it in with your
   solution to the programming exercise. */
#include <stdio.h>
#include "util.h"
#include "slp.h"
#include "prog1.h"

int maxArgs(A_stm);
int getMaxLengthByStm(A_stm);
int getMaxLengthByExp(A_exp);
int getMaxLengthByExpList(A_expList);

typedef struct table* Table_;
struct table { string id; int value; Table_ tail; };
Table_ Table(string id, int value, struct table* tail) {
    Table_ t = checked_malloc(sizeof(*t));
    t->id = id;
    t->value = value;
    t->tail = tail;
    return t;
}

Table_ update(Table_ t1, string id, int value) {
    return Table(id, value, t1);
}

int lookup(Table_ t, string key) {
    if (t->tail == NULL) {
        return (int) NULL;
    }
    if (t->id == key) {
        return t->value;
    }
    return lookup(t->tail, key);
}
Table_ interpretStm(A_stm, Table_);
Table_ interpretExp(A_exp exp, Table_ t) {
    int o1;
    int o2;
    int r = 0;
    switch (exp->kind) {
        case A_eseqExp:
            interpretStm(exp->u.eseq.stm, t);
            return interpretExp(exp->u.eseq.exp, t);
        case A_idExp: return t;
        case A_numExp: return update(t, "op", exp->u.num);
        case A_opExp:
            t = interpretExp(exp->u.op.left, t);
            o1 = lookup(t, "op");
            t = interpretExp(exp->u.op.right, t);
            o2 = lookup(t, "op");
            switch (exp->u.op.oper) {
                case A_plus: r = o1 + o2; break;
                case A_minus: r = o1 - o2; break;
                case A_times: r = o1 * o2; break;
                case A_div: r = o1 / o2; break;
                default: break;
            }
            return update(t, "op", r);
        default: break;
    }
    printf("error");
    return t;
}
Table_ interpretExpList(A_expList exps, Table_ t) {
    int r;
    switch (exps->kind) {
        case A_lastExpList:
            t = interpretExp(exps->u.last, t);
            r = lookup(t, "op");
            printf("%d\n", r);
            return t;
        case A_pairExpList:
            t = interpretExp(exps->u.pair.head, t);
            r = lookup(t, "op");
            printf("%d ", r);
            t = interpretExpList(exps->u.pair.tail, t);
            return t;
        default: break;
    }
    printf("error");
    return t;
}
Table_ interpretStm(A_stm stm, Table_ t) {
    int r;
    switch (stm->kind) {
        case A_compoundStm:
            t = interpretStm(stm->u.compound.stm1, t);
            t = interpretStm(stm->u.compound.stm2, t);
            return t;
        case A_assignStm:
            t = interpretExp(stm->u.assign.exp, t);
            r = lookup(t, "op");
            return update(t, stm->u.assign.id, r);
        case A_printStm:
            return interpretExpList(stm->u.print.exps, t);
        default: break;
    }
    printf("error");
    return t;
}

void interp(A_stm s) {
    Table_ t = Table("null", -100, NULL);
    interpretStm(s, t);
}

int main() {
    A_stm stm = prog();
    printf("%d\n", maxArgs(stm));
    interp(stm);
    return 0;
}

int maxArgs(A_stm stm) {
    return getMaxLengthByStm(stm);
}

int getMaxLengthByStm(A_stm stm) {
    int length1;
    int length2;
    switch (stm->kind) {
        case A_compoundStm:
            length1 = maxArgs(stm->u.compound.stm1);
            length2 = maxArgs(stm->u.compound.stm2);
            return length1 < length2 ? length2 : length1;
        case A_assignStm:
            return getMaxLengthByExp(stm->u.assign.exp);
        case A_printStm:
            return getMaxLengthByExpList(stm->u.print.exps);
        default: break;
    }
    return 0;
}

int getMaxLengthByExp(A_exp exp) {
    int length1;
    int length2;
    switch (exp->kind) {
        case A_eseqExp:
            length1 = getMaxLengthByStm(exp->u.eseq.stm);
            length2 = getMaxLengthByExp(exp->u.eseq.exp);
            return length1 < length2 ? length2 : length1;
        case A_idExp:
        case A_numExp:
        case A_opExp:
            return 1;
        default: break;
    }
    return 0;
}

int getMaxLengthByExpList(A_expList exps) {
    int length;
    switch (exps->kind) {
        case A_lastExpList:
            length = getMaxLengthByExp(exps->u.last);
            return 1 < length ? length : 1;
        case A_pairExpList:
            return getMaxLengthByExp(exps->u.pair.head) + getMaxLengthByExpList(exps->u.pair.tail);
        default: break;
    }
    return 0;
}