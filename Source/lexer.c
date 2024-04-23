#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "lexer.h"
#include "utils.h"

Token* tokens;	// single linked list of tokens
Token* lastTk;		// the last token in list

int line = 1;		// the current line in the input file

// adds a token to the end of the tokens list and returns it
// sets its code and line


Token* addTk(int code) {

	Token* tk = safeAlloc(sizeof(Token));
	tk->code = code;
	tk->line = line;
	tk->next = NULL;
	if (lastTk) {
		lastTk->next = tk;
	}
	else {
		tokens = tk;
	}
	lastTk = tk;
	return tk;

}

char* extract(const char* begin, const char* end) {

	//err("extract needs to be implemented");

	size_t n = end - begin;
	char* buf = safeAlloc(n + 1);
	strncpy(buf, begin, n);
	buf[n] = '\0';
	//printf("aici este extract: %s\n",buf);
	return buf;

}

Token* tokenize(const char* pch) {

	const char* start;
	Token* tk;
	for (;;) {
		switch (*pch) {
		case ' ':
		case '\t':
			pch++;
			break;
		case '\r': //handles different kinds of newlines (Windows: \r\n, Linux: \n, MacOS, OS X: \r or \n)
			if (pch[1] == '\n')
				pch++;
			//fallthrough to \n
		case '\n':
			line++;
			pch++;
			break;
		case '\0':
			addTk(END);
			return tokens;
		case ',':
			addTk(COMMA);
			pch++;
			break;
		case ';':
			addTk(SEMICOLON);
			pch++;
			break;
		case '(':
			addTk(LPAR);
			pch++;
			break;
		case ')':
			addTk(RPAR);
			pch++;
			break;
		case '[':
			addTk(LBRACKET);
			pch++;
			break;
		case ']':
			addTk(RBRACKET);
			pch++;
			break;
		case '{':
			addTk(LACC);
			pch++;
			break;
		case '}':
			addTk(RACC);
			pch++;
			break;
		case '+':
			addTk(ADD);
			pch++;
			break;
		case '-':
			addTk(SUB);
			pch++;
			break;
		case '*':
			addTk(MUL);
			pch++;
			break;
		case '/':
			if (pch[1] == '/')
			{
				while (*pch != '\n')
				{
					pch++;
				}
			}
			else
			{
				addTk(DIV);
				pch++;
			}
			break;
		case '.':
			addTk(DOT);
			pch++;
			break;
		case '&':
			if (pch[1] == '&')
			{
				addTk(AND);
				pch = pch + 2;
			}
			else
				err("Error!");
			break;
		case '|':
			if (pch[1] == '|')
			{
				addTk(OR);
				pch = pch + 2;
			}
			else
				err("Error!");
			break;
		case '!':
			if (pch[1] == '=')
			{
				addTk(NOTEQ);
				pch = pch + 2;
			}
			else
			{
				addTk(NOT);
				pch++;
			}
			break;
		case '<':
			if (pch[1] == '=')
			{
				addTk(LESSEQ);
				pch = pch + 2;
			}
			else
			{
				addTk(LESS);
				pch++;
			}
			break;
		case '>':
			if (pch[1] == '=')
			{
				addTk(GREATEREQ);
				pch = pch + 2;
			}
			else
			{
				addTk(GREATER);
				pch++;
			}
			break;
		case '=':
			if (pch[1] == '=')
			{
				addTk(EQUAL);
				pch += 2;
			}
			else
			{
				addTk(ASSIGN);
				pch++;
			}
			break;
		default:
			if (isalpha(*pch) || *pch == '_') {
				for (start = pch++; isalnum(*pch) || *pch == '_' || *pch == 'e' || *pch == 'E' || *pch == '-' /* || *pch == '+'*/; pch++) {}
				char* text = extract(start, pch);
				if (strcmp(text, "char") == 0) addTk(TYPE_CHAR);
				else if (strcmp(text, "double") == 0) addTk(TYPE_DOUBLE);
				else if (strcmp(text, "else") == 0) addTk(ELSE);
				else if (strcmp(text, "if") == 0) addTk(IF);
				else if (strcmp(text, "int") == 0) addTk(TYPE_INT);
				else if (strcmp(text, "return") == 0) addTk(RETURN);
				else if (strcmp(text, "struct") == 0) addTk(STRUCT);
				else if (strcmp(text, "void") == 0) addTk(VOID);
				else if (strcmp(text, "while") == 0) addTk(WHILE);
				else {
					tk = addTk(ID);
					tk->text = text;
				}
			}
			else if (isdigit(*pch)) {

				int ok1 = 0; //daca are punct
				int ok2 = 0; //daca are numere dupa virgula

				for (start = pch++; isdigit(*pch) || *pch == '.'; pch++)
				{
					if (*pch == '.')
					{
						ok1 = 1; //daca am punct
					}
					else {
						if (ok1) {
							ok2 = 1; //daca are punct si are numere dupa virgula
						}
					}
				}

				if (ok1 && !ok2) //daca am punct si nu am numere dupa virgula
				{
					ok1 = 0;
					err("invalid number format!");
				}

				char* digit_text = extract(start, pch);
				tk = addTk(ok1 ? DOUBLE : INT);
				
				/*tk = addTk(DOUBLE);
				double number;
				number = atof(digit_text);
				printf("numarul %g\n", number);
				tk->d = number;*/

				if (ok1) {

					//printf("%s\n",digit_text);
					tk->d = atof(digit_text);
				}
				else {
					tk->i = atoi(digit_text);
				}

				free(digit_text);
			}

			else if (*pch == '\'') {
				pch++;
				char c = *pch++;
				if (*pch == '\'') {
					pch++;
					tk = addTk(CHAR);
					tk->c = c;
				}
				else {
					err("invalid character constant!"); //daca lipseste intre ''
				}
			}

			else if (*pch == '\"') {
				pch++;
				start = pch;
				while (*pch != '\"') {
					if (*pch == '\0' || *pch == '\n') {
						err("unterminated string constant!"); //daca lipseste o "
					}
					pch++;
				}
				char* str_text = extract(start, pch);
				pch++;
				tk = addTk(STRING);
				tk->text = str_text;
			}
			else err("invalid char: %c (%d)", *pch, *pch);
		}
	}

}

const char* showAtoms(int code)
{

	switch (code) {

	case 0:
		return(" ID: ");
		break;

	case 1:
		return(" TYPE_CHAR");
		break;
	case 2:
		return(" TYPE_DOUBLE");
		break;
	case 3:
		return(" ELSE");
		break;
	case 4:
		return(" IF");
		break;
	case 5:
		return(" TYPE_INT");
		break;
	case 6:
		return(" RETURN");
		break;
	case 7:
		return(" STRUCT");
		break;
	case 8:
		return(" VOID");
		break;
	case 9:
		return(" WHILE");
		break;

	case 10:
		return(" INT: ");
		break;
	case 11:
		return(" DOUBLE: ");
		break;
	case 12:
		return(" CHAR: ");
		break;
	case 13:
		return(" STRING: ");
		break;

	case 14:
		return(" COMMA");
		break;
	case 15:
		return(" SEMICOLON");
		break;
	case 16:
		return(" LPAR");
		break;
	case 17:
		return(" RPAR");
		break;
	case 18:
		return(" LBRACKET");
		break;
	case 19:
		return(" RBRACKET");
		break;
	case 20:
		return(" LACC");
		break;
	case 21:
		return(" RACC");
		break;
	case 22:
		return(" END");
		break;

	case 23:
		return(" ADD");
		break;
	case 24:
		return(" SUB");
		break;
	case 25:
		return(" MUL");
		break;
	case 26:
		return(" DIV");
		break;
	case 27:
		return(" DOT");
		break;
	case 28:
		return(" AND");
		break;
	case 29:
		return(" OR");
		break;
	case 30:
		return(" NOT");
		break;
	case 31:
		return(" ASSIGN");
		break;
	case 32:
		return(" EQUAL");
		break;
	case 33:
		return(" NOTEQ");
		break;
	case 34:
		return(" LESS");
		break;
	case 35:
		return(" LESSEQ");
		break;
	case 36:
		return(" GREATER");
		break;
	case 37:
		return(" GREATEREQ");
		break;

	case 38:
		return(" SPACE");
		break;
	case 39:
		return(" LINECOMMENT");
		break;

	}

}

void toDOUBLE(double d) {

	int in = (int)d;
	double fr = d - in;

	if (fr == 0) 
		printf("%.1f", d);
	else 
		printf("%g", d);

}

void showTokens(const Token* tokens) {  //[ line:    TYPE_* | ID: atribut ]

	int index = 0; 
	for (const Token* tk = tokens; tk; tk = tk->next)
	{
		//printf("%d\n",tk->code);
		//translatare de la cod la nume

		switch (tk->code) {
		case ID:
			printf("%d%s%s\n", tk->line, showAtoms(tk->code), tk->text);
			break;
		case CHAR:
			printf("%d%s%c\n", tk->line, showAtoms(tk->code), tk->c);
			break;
		case DOUBLE:
			printf("%d%s", tk->line, showAtoms(tk->code)); 
			toDOUBLE(tk->d);
			printf("\n");
			break;
		case INT:
			printf("%d%s%d\n", tk->line, showAtoms(tk->code), tk->i);
			break;
		case STRING:
			printf("%d%s%s\n", tk->line, showAtoms(tk->code), tk->text);
			break;
		default:
			printf("%d%s\n", tk->line, showAtoms(tk->code));
			break;
		}
	}

}
