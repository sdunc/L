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

#include "scope.h"
#include "binmap.h"

NODE* pushScope(NODE *parent_scope) {
    incRef(parent_scope);
    return newNODE(NIL,parent_scope);
}

NODE* popScope(NODE *scope) { 
    NODE *parent_scope = ((NODE*)scope->addr);
    decRef(scope);
    return parent_scope;
}

NODE* resolve_ref(SYMBOL *sym, NODE *scope) {
    debug("Resolving: %i\n", sym->sym);
    while (scope) {
        NODE *entry = binmap_find(sym,(NODE*)scope->data);
        if (entry) return entry;
        scope = (NODE*)scope->addr;
    }
    return NIL;
}

VALUE* resolve(SYMBOL *sym, NODE *scope) {
    NODE *ref = resolve_ref(sym,scope);
    if (ref) {
        VALUE *val = ref->addr;
        incRef(val);
        decRef(ref);
        return val;
    }
    error("Unbound symbol");
}

void bind(SYMBOL *sym, VALUE *val, NODE *scope) {
    debug("Binding: %i\n", sym->sym);
    incRef(val);
    if (scope->data) {
        binmap_put(sym,val,(NODE*)scope->data);
    } else {
        scope->data = (VALUE*)binmap(sym,val);
    }
}

void bindMany(NODE *vars, NODE *vals, NODE *scope) {
    while (vars && vals) {
        bind(asSYMBOL(vars->data),vals->data,scope);
        vars = asNODE(vars->addr);
        vals = asNODE(vals->addr);
    }
}
