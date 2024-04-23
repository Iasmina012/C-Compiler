#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "utils.h"
#include "parser.h"
#include "ad.h"

int main()
{
    //Analizator Lexical
    /*char* inbuf1 = loadFile("testlex.txt");
    puts(inbuf1);
    Token* tokens1 = tokenize(inbuf1);
    printf("\n");
    showTokens(tokens1);*/

    //Analizator Sintactic
    char* inbuf2 = loadFile("testparser.txt");
    puts(inbuf2);
    Token* tokens2 = tokenize(inbuf2);
    printf("\n");
    parse(tokens2);

    //Analiza de domeniu
   /* char* inbuf3 = loadFile("testad.c");
    puts(inbuf3);
    pushDomain();
    showDomain(symTable, "global");
    dropDomain();*/

    return 0;
}