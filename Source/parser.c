#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "parser.h"

Token* iTk;		// the iterator in the tokens list
Token* consumedTk;		// the last consumed token

bool consume(int code);
bool typeBase();
bool unit();
bool structDef();
bool varDef();
bool arrayDecl();
bool fnDef();
bool fnParam();
bool stm();
bool stmCompound();
bool expr();
bool exprAssign();
bool exprOrPrim();
bool exprOr();
bool exprAnd();
bool exprAndPrim();
bool exprEq();
bool exprEqPrim();
bool exprRel();
bool exprRelPrim();
bool exprAdd();
bool exprAddPrim();
bool exprMul(); 
bool exprMulPrim();
bool exprCast();
bool exprUnary();
bool exprPostfixPrim();
bool exprPostfix();
bool exprPrimary();

void tkerr(const char* fmt, ...) {
	fprintf(stderr, "error in line %d: ", iTk->line);
	va_list va;
	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

bool consume(int code) {

	if (iTk->code == code) 
	{
		consumedTk = iTk;
		iTk = iTk->next;
		return true;
	}
	//else
	return false;
}

//typeBase: TYPE_INT | TYPE_DOUBLE | TYPE_CHAR | STRUCT ID
bool typeBase() {

	//int start = iTk;

	if (consume(TYPE_INT)) {
		return true;
	}
	 if (consume(TYPE_DOUBLE)) {
		return true;
	}
	 if (consume(TYPE_CHAR)) {
		return true;
	}
	 if (consume(STRUCT)) {
		if (consume(ID)) {
			return true;
		}
	}
	//iTk = start;
	return false;
}

//unit: ( structDef | fnDef | varDef )* END
bool unit() {

	for (;;) {
		if (structDef()) {}
		else if (fnDef()) {}
		else if (varDef()) {}
		else break;
	}
	if (consume(END)) {
		return true;
	}
	return false;
}

//structDef: STRUCT ID LACC varDef* RACC SEMICOLON
bool structDef() {

	Token* start = iTk;

	if (consume(STRUCT)) {
		if (consume(ID)) {
			if (consume(LACC)) {
				for (;;) {
					if (varDef()) {
					}
					else {
						break;
					}
				}
				if (consume(RACC)) {
					if (consume(SEMICOLON)) {
						return true;
					}
					else tkerr("missing ';' after the struct's definition");
				}
				else tkerr("missing '}' in the struct's definition");
			}
		}
		else tkerr("missing the struct's identifier");
	}

	iTk = start;
	return false;
}

//varDef: typeBase ID arrayDecl? SEMICOLON
bool varDef() {
	if (typeBase()) {
		if (consume(ID)) {
			arrayDecl(); 
			if (consume(SEMICOLON)) {
				return true;
			}
			else {
				tkerr("missing ';' after the variable's definition");
			}
		}
		else {
			tkerr("missing token after the data's type");
		}
	}
	if (consume(ID)) {
		if (consume(ASSIGN)) {
			if (typeBase()) {
				if (consume(SEMICOLON)) {
					return true;
				}
			}
			else if (consume(ID)) {
				if (consume(DIV)) {
					if (consume(INT)) {
						if (consume(SEMICOLON)) {
							return true;
						}
						else {
							tkerr("missing ';' after the variable's definition");
						}
					}
					else {
						tkerr("invalid expression after '/'");
					}
				}
				else if (consume(SEMICOLON)) {
					return true;
				}
				else {
					tkerr("missing ';' after the variable's definition");
				}
			}
			else if (consume(INT)) {
				if (consume(SEMICOLON)) {
					return true;
				}
				else {
					tkerr("missing ';' after the variable's definition");
				}
			}
			else {
				tkerr("missing token after equal");
			}
		}
		else if (consume(LPAR)) {
			if (consume(ID)) {
				if (consume(RPAR)) {
					if (consume(SEMICOLON)) {
						return true;
					}
				}
			}
		}
		/*else {
			tkerr("missing the type");
		}*/
	}
	return false;
}

//arrayDecl: LBRACKET INT? RBRACKET
bool arrayDecl() {

	Token* start = iTk;

	if (consume(LBRACKET)) {
		if (consume(INT)) {}
		if (consume(RBRACKET)) {
			return true;
		}
		else tkerr("missing ']' after the array's declaration");
	}

	iTk = start;
	return false;
}

/*fnDef: ( typeBase | VOID ) ID
		LPAR ( fnParam ( COMMA fnParam )* )? RPAR
		stmCompound*/
bool fnDef() {
	
	Token* start = iTk;

	if (typeBase()) {
		if (consume(ID)) {
			if (consume(LPAR)) {
				if (fnParam()) {
					while (consume(COMMA)) {
						if (fnParam()) {}
						else tkerr("missing the parameter after ',' in the header of the function");
					}
				}
				if (consume(RPAR)) {
					if (stmCompound()) {
						return true;
					}
					else tkerr("missing the body of the function");
				}
				else tkerr("missing ')' in the header of the function");
			}
		}
		else tkerr("missing the function's identifier");
	}

	if (consume(VOID)) {
		if (consume(ID)) {
			if (consume(LPAR)) {
				if (fnParam()) {
					while (consume(COMMA)) {
						if (fnParam()) {}
						else tkerr("missing the parameter after ',' in the header of the function");
					}
				}
				if (consume(RPAR)) {
					if (stmCompound()) {
						return true;
					}
					else tkerr("missing the body of the function");
				}
				else tkerr("missing ')' in the header of the function");
			}
		}
		else tkerr("missing the function's identifier");
	}

	iTk = start;
	return false;
}

//fnParam: typeBase ID arrayDecl?
bool fnParam() {

	Token* start = iTk;

	if (typeBase()) {
		if (consume(ID)) {
			if (arrayDecl()) {}
			return true;
		}
		else tkerr("missing the parameter's name");
	}

	iTk = start;
	return false;
}

/*stm:  stmCompound
		| IF LPAR expr RPAR stm ( ELSE stm )?
		| WHILE LPAR expr RPAR stm
		| RETURN expr? SEMICOLON
		| expr? SEMICOLON */
bool stm() {
	
	Token* start = iTk;

	if (stmCompound())
	{
		return true;
	}
	if (consume(IF)) {
		if (consume(LPAR)) {
			if (expr()) {
				if (consume(RPAR)) {
					if (stm()) {
						if (consume(ELSE)) {
							if (stm()) {}
							else tkerr("missing the statement after 'else'");
						}
						return true;
					}
					else tkerr("missing the statement after 'if'");
				}
				else tkerr("missing ')' after the expression");
			}
			else tkerr("missing the expression'");
		}
		else tkerr("missing '(' after 'if'");
	}
	if (consume(WHILE)) {
		if (consume(LPAR)) {
			if (expr()) {
				if (consume(RPAR)) {
					if (stm()) {
						return true;
					}
					else tkerr("missing the statement after 'while'");
				}
				else tkerr("missing ')' after the expression");
			}
		}
		else tkerr("missing the expression between '()'");
	}
	if (consume(RETURN)) {
		if (expr()) {}
		else {
			tkerr("missing statement after return");
		}
		if (consume(SEMICOLON)) {
			return true;
		}
		else tkerr("missing ';' after return");
	}

	if (consume(LPAR)) {
		tkerr("missing the function's call or operator");
	}

	if (expr()) {
		if (consume(SEMICOLON)) {
			return true;
		}
		else tkerr("missing ';' after the expression");
	}
	if (consume(SEMICOLON))
		return true;

	iTk = start;
	return false;
}

//stmCompound: LACC ( varDef | stm )* RACC
bool stmCompound() {

	Token* start = iTk;

	if (consume(LACC)) {
		for (;;) {
			if (varDef()) {
			}
			else if (stm()) {
			}
			else {
				break;
			}
		}
		

		else tkerr("missing '}' after the function's block");
	}

	iTk = start;
	return false;
}

//expr: exprAssign
bool expr() {

	Token* start = iTk;

	if (exprAssign()) {
		return true;
	}

	iTk = start;
	return false;
}

//exprAssign: exprUnary ASSIGN exprAssign | exprOr
bool exprAssign() {
	
	Token* start = iTk;

	if (exprUnary()) {
		if (consume(ASSIGN)) {
			if (exprAssign()) {
				return true;
			}
			else tkerr("missing the expression after '='");
		}
	}
	iTk = start;
	if (exprOr()) {
		return true;
	}

	iTk = start;
	return false;

}

//exprOr: exprOr OR exprAnd | exprAnd
bool exprOrPrim() {

	Token* start = iTk;

	if (consume(OR)) {
		if (exprAnd()) {
			if (exprOrPrim()) {
				return true;
			}
		}
		else tkerr("invalid expression after '||'");
	}

	iTk = start;
	return true;
}

bool exprOr() {

	Token* start = iTk;

	if (exprAnd()) {
		if (exprOrPrim()) {
			return true;
		}
	}

	iTk = start;
	return false;
}

//exprAnd: exprAnd AND exprEq | exprEq
bool exprAndPrim() {

	Token* start = iTk;

	if (consume(AND)) {
		if (exprEq()) {
			if (exprAndPrim()) {
				return true;
			}
		}
		else tkerr("invalid expression after '&&'");
	}

	iTk = start;
	return true;
}

bool exprAnd() {

	Token* start = iTk;

	if (exprEq()) {
		if (exprAndPrim()) {
			return true;
		}
	}

	iTk = start;
	return false;
}

//exprEq: exprEq ( EQUAL | NOTEQ ) exprRel | exprRel
bool exprEqPrim() {

	Token* start = iTk;

	if (consume(EQUAL)) {
		if (exprRel()) {
			if (exprEqPrim()) {
				return true;
			}
		}
		else tkerr("invalid expression after '=='");
	}
	if (consume(NOTEQ)) {
		if (exprRel()) {
			if (exprEqPrim()) {
				return true;
			}
		}
		else tkerr("invalid expression after '!='");
	}

	iTk = start;
	return true;
}

bool exprEq() {

	Token* start = iTk;

	if (exprRel()) {
		if (exprEqPrim()) {
			return true;
		}
	}

	iTk = start;
	return false;
}

//exprRel: exprRel ( LESS | LESSEQ | GREATER | GREATEREQ ) exprAdd | exprAdd
bool exprRelPrim() {

	Token* start = iTk;

	if (consume(LESS)) {
		if (exprAdd()) {
			if (exprRelPrim()) {
				return true;
			}
		}
		else tkerr("invalid expression after '<'");
	}
	if (consume(LESSEQ)) {
		if (exprAdd()) {
			if (exprRelPrim()) {
				return true;
			}
		}
		else tkerr("invalid expression after '<='");
	}
	if (consume(GREATER)) {
		if (exprAdd()) {
			if (exprRelPrim()) {
				return true;
			}
		}
		else tkerr("invalid expression after '>'");
	}
	if (consume(GREATEREQ)) {
		if (exprAdd()) {
			if (exprRelPrim()) {
				return true;
			}
		}
		else tkerr("invalid expression after '>='");
	}

	iTk = start;
	return true;

}

bool exprRel() {

	Token* start = iTk;

	if (exprAdd()) {
		if (exprRelPrim()) {
			return true;
		}
	}

	iTk = start;
	return false;
}

//exprAdd: exprAdd ( ADD | SUB ) exprMul | exprMul
bool exprAddPrim() {

	Token* start = iTk;

	if (consume(ADD)) {
		if (exprMul()) {
			if (exprAddPrim()) {
				return true;
			}
		}
		else tkerr("invalid expression after '+'");
	}
	if (consume(SUB)) {
		if (exprMul()) {
			if (exprAddPrim()) {
				return true;
			}
		}
		else tkerr("invalid expression after '-'");
	}

	iTk = start;
	return true;
}

bool exprAdd() {

	Token* start = iTk;

	if (exprMul()) {
		if (exprAddPrim()) {
			return true;
		}
	}

	iTk = start;
	return false;
}

//exprMul: exprMul ( MUL | DIV ) exprCast | exprCast
bool exprMulPrim() {

	Token* start = iTk;

	if (consume(MUL)) {
		if (exprCast()) {
			if (exprMulPrim()) {
				return true;
			}
		}
		else tkerr("invalid expression after '*'");
	}

	if (consume(DIV)) {
		if (exprCast()) {
			if (exprMulPrim()) {
				return true;
			}
		}
		else tkerr("invalid expression after '/'");
	}

	iTk = start;
	return true;
}

bool exprMul() {

	Token* start = iTk;

	if (exprCast()) {
		if (exprMulPrim()) {
			return true;
		}
	}

	iTk = start;
	return false;
}

// exprCast: LPAR typeBase arrayDecl? RPAR exprCast | exprUnary
bool exprCast() {

	Token* start = iTk;

	if (consume(LPAR)) {
		if (typeBase()) {
			if (arrayDecl()) {}
			if (consume(RPAR)) {
				if (exprCast()) {
					return true;
				}
			}
			else tkerr("missing ')' for casting");
		}
	}

	iTk = start;
	if (exprUnary()) {
		return true;
	}

	iTk = start;
	return false;
}

// exprUnary: ( SUB | NOT ) exprUnary | exprPostfix
bool exprUnary() {

	Token* start = iTk;

	if (consume(SUB)) {
		if (exprUnary()) {
			return true;
		}
	}
	if (consume(NOT)) {
		if (exprUnary()) {
			return true;
		}
	}

	iTk = start;
	if (exprPostfix()) {
		return true;
	}

	iTk = start;
	return false;
}

/*exprPostfix:  exprPostfix LBRACKET expr RBRACKET
				| exprPostfix DOT ID
				| exprPrimary */
bool exprPostfixPrim() {

	Token* start = iTk;

	if (consume(LBRACKET)) {
		if (expr()) {
			if (consume(RBRACKET)) {
				if (exprPostfixPrim()) {
					return true;
				}
			}
			else tkerr("missing ']' after the expression");
		}
		else tkerr("missing the expression after '['");
	}
	if (consume(DOT)) {
		if (consume(ID)) {
			if (exprPostfixPrim()) {
				return true;
			}
		}
		else tkerr("missing the identifier after '.'");
	}

	iTk = start;
	return true;
}

bool exprPostfix() {

	Token* start = iTk;

	if (exprPrimary()) {
		if (exprPostfixPrim()) {
			return true;
		}
	}

	iTk = start;
	return false;
}

/*exprPrimary: ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
			| INT | DOUBLE | CHAR | STRING | LPAR expr RPAR */
bool exprPrimary() {

	Token* start = iTk;

	if (consume(ID)) {
		if (consume(LPAR)) {
			if (expr()) {
				for (;;) {
					if (consume(COMMA)) {
						if (expr()) {}
						else tkerr("missing the expression after ',' in the function's call");
					}
					else {
						break;
					}
				}
			}
			if (consume(RPAR)) {
				return true;
			}
			else tkerr("missing ')' in the function's call");
		}
		return true;
	}
	if (consume(INT)) {
		return true;
	}
	if (consume(DOUBLE)) {
		return true;
	}
	if (consume(CHAR)) {
		return true;
	}
	if (consume(STRING)) {
		return true;
	}
	if (consume(LPAR)) {
		if (expr()) {
			if (consume(RPAR)) {
				return true;
			}
			else tkerr("missing ')' after the expression");
		}
	}

	iTk = start;
	return false;
}

void parse(Token* tokens) {

	iTk = tokens;
	if (!unit())
		tkerr("Syntax error!\n");
	else 
		printf("No syntax error!\n");

	return 0;
}
