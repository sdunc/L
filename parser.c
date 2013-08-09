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

#include "parser.h"
#include "listops.h"
#include "primitives.h"
#include "binmap.h"
#include <ctype.h>

T_SYMBOL hash(char *sym) {
    T_SYMBOL hval = 0xAAAAAAAA;
    for (int i = 0; sym[i]; i++) hval ^= ((T_SYMBOL)sym[i]) << ((4*i)%24);
    return hval;
}

static NODE *sym_map = NIL;
static NODE *literal_map = NIL;

void parser_init() {
    debug("Defining built-in symbols\n");
    sym_map = binmap(newSYMBOL(hash("NIL")),newSTRING("NIL"));
    debugVal(sym_map,"sym_map 1: ");
    literal_map = newNODE(newNODE(newSYMBOL(intern("NIL")),NIL),newNODE(NIL,NIL));
    //literal_map = binmap(newSYMBOL(intern("NIL")),NIL);
    debugVal(sym_map,"sym_map 2: ");
    binmap_put(newSYMBOL(intern("LAMBDA")),newPRIMFUNC(PRIM_LAMBDA),literal_map);
    binmap_put(newSYMBOL(intern("QUOTE")),newPRIMFUNC(PRIM_QUOTE),literal_map);
    binmap_put(newSYMBOL(intern("LIST")),newPRIMFUNC(PRIM_LIST),literal_map);
    binmap_put(newSYMBOL(intern("ADDR")),newPRIMFUNC(PRIM_ADDR),literal_map);
    binmap_put(newSYMBOL(intern("DATA")),newPRIMFUNC(PRIM_DATA),literal_map);
    binmap_put(newSYMBOL(intern("SETA")),newPRIMFUNC(PRIM_SETA),literal_map);
    binmap_put(newSYMBOL(intern("SETD")),newPRIMFUNC(PRIM_SETD),literal_map);
    binmap_put(newSYMBOL(intern("+")),newPRIMFUNC(PRIM_ADD),literal_map);
    binmap_put(newSYMBOL(intern("-")),newPRIMFUNC(PRIM_SUB),literal_map);
    binmap_put(newSYMBOL(intern("*")),newPRIMFUNC(PRIM_MUL),literal_map);
    binmap_put(newSYMBOL(intern("/")),newPRIMFUNC(PRIM_DIV),literal_map);
    binmap_put(newSYMBOL(intern("REF")),newPRIMFUNC(PRIM_REF),literal_map);
}

T_SYMBOL intern(char *c_str) {
    if (!sym_map) parser_init();
    c_str = strdup(c_str);
    for (int i = 0; c_str[i]; i++) c_str[i] = toupper(c_str[i]);
    debug("intern: %s %i\n",c_str,hash(c_str));
    SYMBOL *sym = newSYMBOL(hash(c_str));
    STRING *str = newSTRING(c_str);
    NODE *entry;
    while ((entry = binmap_find(sym,sym_map))) {
        debugVal(entry,"matching: ");
        if (cmpSTRING((STRING*)entry->addr,str)) {
            decRef(entry);
            sym->sym++;
        } else {
            break;
        }
    }
    if (!entry) {
        debug("adding symbol: %s\n",c_str);
        binmap_put(sym,str,sym_map);
        return sym->sym;
    } else {
        decRef(sym);
        decRef(str);
        decRef(entry);
        return ((SYMBOL*)entry->data)->sym;
    }
}

NODE* parse(char **exp) {
    debug("Parse List: %s\n",*exp);
    NODE *head = NIL;
    while (**exp) {
        switch (*((*exp)++)) {
            case '\'':
                list_push(newNODE(newPRIMFUNC(PRIM_QUOTE),parse(exp)),&head);
            case '(':
                list_push(parse(exp),&head);
                continue;
            case ')':
                debugVal(head,"ParseList: ");
                return list_reverse(head);
            case '\n':
            case '\r':
            case '\t':
            case ' ':
                continue;
            default: {
                char *sym = *exp-1;
                debug("Start of literal: %s\n",sym);
                while (**exp && **exp != ' ' && **exp != ')') (*exp)++;
                char old = **exp;
                **exp = 0;
                debug("Literal: %s\n",sym);
                switch (sym[0]) {
                    case '+':
                    case '-':
                        if (!isdigit(sym[1])) {
                            list_push(newSYMBOL(intern(sym)),&head);
                            break;
                        }  
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        {
                            bool real = false;
                            char *scn = sym+1;
                            while (*scn) {
                                if (*scn == '.') { 
                                    real = true;
                                } else if (!isdigit(*scn)) {
                                    error("Malformed number character %c",*scn);
                                }
                                scn++;
                            }
                            if (real) {
                                list_push(newREAL(atof(sym)),&head);
                            } else {
                                list_push(newINTEGER(atoi(sym)),&head);
                            }
                        }
                        break;
                    default:
                        list_push(newSYMBOL(intern(sym)),&head);
                        break;
                 }
                 **exp = old;
                 if (head->data->type == ID_SYMBOL) {
                    NODE *literal;
                    if ((literal = binmap_find(head->data,literal_map))) {
                        NODE *last = (NODE*)head->addr;
                        incRef(last);
                        decRef(head);
                        incRef(literal->addr);
                        head = last;
                        list_push(literal->addr,&head);
                    }
                 }
                 debugVal(head,"ParseLiteral: ");
             }
        }
    }
    return list_reverse(head);
}

NODE* parseForms(char *exp) {
    if (!sym_map) parser_init();
    char *dup = strdup(exp);
    char *org = dup;
    NODE *forms = parse(&dup);
    free(org);
    return forms;
}
