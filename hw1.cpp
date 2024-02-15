#include "tokens.hpp"
#include <string.h>

#include <iostream>
#include <sstream>

const char* token_names[] = {"", "VOID", "INT", "BYTE", "B", "BOOL", "AND", "OR", "NOT", "TRUE", 
      "FALSE", "RETURN", "IF", "ELSE", "WHILE", "BREAK", "CONTINUE", "SC", "LPAREN", "RPAREN", "LBRACE", 
      "RBRACE", "ASSIGN", "RELOP", "BINOP", "COMMENT", "ID", "NUM", "STRING", "UNCLOSED", "INVALID_ESCAPE", 
      "INVALID_HEX", "ERROR"};

int findUndefinedEscape(char* input){
    int length = strlen(input);

    for (int i = 0; i < length - 1; ++i) {
        //check if starts with \ and then an illegal charecter
        if (input[i] == '\\' && input[i + 1] != 't' && input[i + 1] != 'r' && input[i + 1] != 'n' &&
            input[i + 1] != '\"' && input[i + 1] != '\\' && input[i + 1] != '0') { 
            
            //check if an illegal hexadecimal
            if (input[i + 1] == 'x') {
                if ((i + 3 < length) &&  //TODO: fix for cases in end of string
                !(('0' <= input[i + 2] && input[i + 2] <= '9') || ('a' <= input[i + 2] && input[i + 2] <= 'f') ||
                  ('A' <= input[i + 2] && input[i + 2] <= 'F')) ||
                !(('0' <= input[i + 3] && input[i + 3] <= '9') || ('a' <= input[i + 3] && input[i + 3] <= 'f') ||
                  ('A' <= input[i + 3] && input[i + 3] <= 'F'))){
                    return i;
                }
            }
            else {
                return i;
            }
        }
    }

    if (input[length-1] == '\\'){
        return length-1;
    }

    return -1;  // If no invalid substring is found
}

//write a function convert /x__ to char to the right value in every place in the string. for example: "a\x41b" -> "aAb"
//return the new string
char* convertHex(char* input){
	int length = strlen(input);
	char* output = (char*)malloc(length * sizeof(char));
	int j = 0;
	for (int i = 0; i < length; ++i) {
		if (input[i] == '\\' && input[i + 1] == 'x') {
			if ((i + 3 < length) &&  //TODO: fix for cases in end of string
				(('0' <= input[i + 2] && input[i + 2] <= '9') || ('a' <= input[i + 2] && input[i + 2] <= 'f') ||
				 ('A' <= input[i + 2] && input[i + 2] <= 'F')) &&
				(('0' <= input[i + 3] && input[i + 3] <= '9') || ('a' <= input[i + 3] && input[i + 3] <= 'f') ||
				 ('A' <= input[i + 3] && input[i + 3] <= 'F'))) {
				char hex[3] = {input[i + 2], input[i + 3], '\0'};
				int value;
				sscanf(hex, "%x", &value);
				output[j] = (char)value;
				j++;
				i += 3;
			}
		}
		else {
			output[j] = input[i];
			j++;
		}
	}
	output[j] = '\0';
	return output;
}

//can you write a function that recievs a string and when recognizing \n, \t, \", \r, \0, \\, \x__ it will convert it to the right char?"
//return the new string
char* convertEscape(char* input){
	int length = strlen(input);
	char* output = (char*)malloc(length * sizeof(char));
	int j = 0;
	for (int i = 0; i < length; ++i) {
		if (input[i] == '\\' && input[i + 1] == 'n') {
			output[j] = '\n';
			j++;
			i++;
		}
		else if (input[i] == '\\' && input[i + 1] == 't') {
			output[j] = '\t';
			j++;
			i++;
		}
		else if (input[i] == '\\' && input[i + 1] == '\"') {
			output[j] = '\"';
			j++;
			i++;
		}
		else if (input[i] == '\\' && input[i + 1] == 'r') {
			output[j] = '\r';
			j++;
			i++;
		}
		else if (input[i] == '\\' && input[i + 1] == '0') {
			output[j] = '\0';
			j++;
			i++;
		}
		else if (input[i] == '\\' && input[i + 1] == '\\') {
			output[j] = '\\';
			j++;
			i++;
		}
		else {
			output[j] = input[i];
			j++;
		}
	}
	output[j] = '\0';
	return output;
}

void showToken(tokentype type){
    if(type == COMMENT){
        printf("%d %s %s\n", yylineno, token_names[type], "//");
    }
    else if(type == STRING){
        yytext[strlen(yytext)-1] = '\0';
        yytext+=1;
		yytext = convertEscape(convertHex(yytext));

        
        printf("%d %s %s\n", yylineno, token_names[type], yytext);
    }
    else if(type != UNCLOSED && type != INVALID_ESCAPE && type != INVALID_HEX && type != ERROR){
        printf("%d %s %s\n", yylineno, token_names[type], yytext);
    }
    else if(type == UNCLOSED){
        printf("Error unclosed string\n");
        exit(0);
    }
    else if(type == INVALID_ESCAPE || type == INVALID_HEX){
        int i = findUndefinedEscape(yytext);
        if (yytext[i+1] == 'x'){
            if (i+1 < strlen(yytext)-2){
                printf("Error undefined escape sequence %c%c%c\n", *(yytext+i+1), *(yytext+i+2), *(yytext+i+3));
            }
            else if (i+1 < strlen(yytext)-1){
                printf("Error undefined escape sequence %c%c\n", *(yytext+i+1), *(yytext+i+2));
            }
            else{
                printf("Error undefined escape sequence %c\n", *(yytext+i+1));
            }
        }
        else{
            printf("Error undefined escape sequence %c\n", *(yytext+i+1));
        }      
        exit(0);
    }
    else if(type == ERROR){
        printf("%s %s\n", "Error", yytext);
    }
}

int main()
{
	int token;
	while(token = yylex()) {
		showToken((tokentype)token);
	}
	return 0;
}