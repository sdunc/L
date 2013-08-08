/**
 *  Copyright 2013 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of L, a virtual machine for a lisp-like language.
 *
 *  L is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  L is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with L. If not, see <http://www.gnu.org/licenses/>.
 */

#include "primitives.h"

NODE* list(NODE *args, NODE *scope) {
    return args ? newNODE(evaluate(args->data,scope),list(asNODE(args->addr),scope)) : NIL;
}

VALUE* quote(NODE *args, NODE *scope) {
    if (args->addr) error("QUOTE takes exactly 1 argument");
    return deep_copy(args->data);
}

VALUE* data(NODE *args, NODE *scope) {
    if (args->addr) error("DATA takes exactly 1 argument");
    return asNODE(args->data)->data;
}

VALUE* addr(NODE *args, NODE *scope) {
    if (args->addr) error("ADDR takes exactly 1 argument");
    return asNODE(args->data)->addr;
}

VALUE* add(NODE *args, NODE *scope) {
    debugVal(args,"add: ");
    bool real = false;
    T_REAL accum_r = 0;
    T_INTEGER accum_i = 0;
    while (args) {
        failNIL(args->data,"NIL is not a number");
        if (real) {
            if (args->data->type == ID_REAL) {
                accum_r += ((REAL*)args->data)->val;
            } else {
                accum_r += asINTEGER(args->data)->val;
            }
        } else {
            if (args->data->type == ID_REAL) {
                real = true;
                accum_r = accum_i + ((REAL*)args->data)->val;
            } else {
                accum_i += asINTEGER(args->data)->val;
            }
        }
        args = asNODE(args->addr);
    }    
    return real ? asVALUE(newREAL(accum_r)) : asVALUE(newINTEGER(accum_i));
}

VALUE* mul(NODE *args, NODE *scope) {
    debugVal(args,"mul: ");
    bool real = false;
    T_REAL accum_r = 1.0;
    T_INTEGER accum_i = 1;
    while (args) {
        failNIL(args->data,"NIL is not a number");
        if (real) {
            if (args->data->type == ID_REAL) {
                accum_r *= ((REAL*)args->data)->val;
            } else {
                accum_r *= asINTEGER(args->data)->val;
            }
        } else {
            if (args->data->type == ID_REAL) {
                real = true;
                accum_r = accum_i * ((REAL*)args->data)->val;
            } else {
                accum_i *= asINTEGER(args->data)->val;
            }
        }
        args = asNODE(args->addr);
    }    
    return real ? asVALUE(newREAL(accum_r)) : asVALUE(newINTEGER(accum_i));
}

VALUE* sub(NODE *args, NODE *scope) {
    debugVal(args,"sub: ");
    bool real = false;
    T_REAL accum_r;
    T_INTEGER accum_i;
    failNIL(args->data,"NIL is not a number");
    if (args->data->type == ID_REAL) {
        accum_r = ((REAL*)args->data)->val;
        real = true;
    } else {
        accum_i = asINTEGER(args->data)->val;
    }
    args = asNODE(args->addr);
    while (args) {
        failNIL(args->data,"NIL is not a number");
        if (real) {
            if (args->data->type == ID_REAL) {
                accum_r -= ((REAL*)args->data)->val;
            } else {
                accum_r -= asINTEGER(args->data)->val;
            }
        } else {
            if (args->data->type == ID_REAL) {
                real = true;
                accum_r = accum_i - ((REAL*)args->data)->val;
            } else {
                accum_i -= asINTEGER(args->data)->val;
            }
        }
        args = asNODE(args->addr);
    }    
    return real ? asVALUE(newREAL(accum_r)) : asVALUE(newINTEGER(accum_i));
}

VALUE* div(NODE *args, NODE *scope) {
    debugVal(args,"div: ");
    bool real = false;
    T_REAL accum_r;
    T_INTEGER accum_i;
    failNIL(args->data,"NIL is not a number");
    if (args->data->type == ID_REAL) {
        accum_r = ((REAL*)args->data)->val;
        real = true;
    } else {
        accum_i = asINTEGER(args->data)->val;
    }
    args = asNODE(args->addr);
    while (args) {
        failNIL(args->data,"NIL is not a number");
        if (real) {
            if (args->data->type == ID_REAL) {
                accum_r /= ((REAL*)args->data)->val;
            } else {
                accum_r /= asINTEGER(args->data)->val;
            }
        } else {
            if (args->data->type == ID_REAL) {
                real = true;
                accum_r = accum_i / ((REAL*)args->data)->val;
            } else {
                accum_i /= asINTEGER(args->data)->val;
            }
        }
        args = asNODE(args->addr);
    }    
    return real ? asVALUE(newREAL(accum_r)) : asVALUE(newINTEGER(accum_i));
}
