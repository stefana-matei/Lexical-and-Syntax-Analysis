#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define _CRT_SECURE_NO_WARNINGS
#define MAX_ATOMI 10000


typedef enum {
	ID, INT, REAL, STR, VAR, FUNCTION, IF, ELSE, WHILE, END, RETURN, TYPE_INT, TYPE_REAL,
	TYPE_STR, COLON, SEMICOLON, LPAR, RPAR, COMMA, OR, AND, NOT, EQUAL, NOTEQUAL, LESS, ASSIGN, ADD,
	SUB, MUL, DIV, FINISH
}tipAtom;


char numeAtomi[31][10] = { "ID", "INT", "REAL", "STR", "VAR", "FUNCTION", "IF", "ELSE", "WHILE", "END", "RETURN",
							"TYPE_INT", "TYPE_REAL", "TYPE_STR", "COLON", "SEMICOLON", "LPAR", "RPAR",
							"COMMA", "OR", "AND", "NOT", "EQUAL", "NOTEQUAL", "LESS", "ASSIGN", "ADD", "SUB",
							"MUL", "DIV", "FINISH" };


typedef struct {
	int cod;	// codul atomului
	int linieFisier;
	char image[1000];
	int valueInt;
	double valueDouble;
}Atom;


Atom atomi[MAX_ATOMI];
int nrAtomi = 0;	// numarul de atomi din vectorul atomi


char bufin[30001];
char* pch;	// cursor la caracterul curent din bufin


char buf[1000];
int n = 0;
int linie = 1;

char tipCuvinteCheie[10][10] = { "var", "funtion", "if", "else", "while", "end", "return", "int", "real", "str" };


// conversie in INT
int stringToInt(char a[])
{
	int c, sign = 1, offset = 0, n = 0;

	if (a[0] == '-') {  // Handle negative integers
		sign = -1;
	}

	if (sign == -1) {  // Set starting position to convert
		offset = 1;
	}
	else {
		offset = 0;
	}

	n = 0;

	for (c = offset; a[c] != '\0'; c++) {
		n = n * 10 + a[c] - '0';
	}

	if (sign == -1) {
		n = -n;
	}

	return n;
}

// conversie in REAL
double stringToDouble(char string[])
{
	char* endptr;

	return strtod(string, &endptr);
}



void addAtom(tipAtom tip)
{
	buf[n] = '\0';
	if (tip == ID)
	{
		for (int i = 4; i < 14; i++)
		{
			if (!strcmp(buf, tipCuvinteCheie[i - 4]))
			{
				tip = i;
				//printf("tip = %d, buf = %s\n", i,buf);
				break;
			}
		}
	}
	atomi[nrAtomi].cod = tip;
	//printf("tip = %d, buf = %s\n", tip, buf);
	strcpy_s(atomi[nrAtomi].image, 1000, buf);
	atomi[nrAtomi].linieFisier = linie;


	if (atomi[nrAtomi].cod == 1)
	{
		atomi[nrAtomi].valueInt = stringToInt(atomi[nrAtomi].image);
	}
	if (atomi[nrAtomi].cod == 2)
	{
		atomi[nrAtomi].valueDouble = stringToDouble(atomi[nrAtomi].image);
	}
	nrAtomi++;
}



// la fiecare apel returneaza codul unui atom
int getNextTk()			// get next token (atom lexical)
{
	int stare = 0;		// starea curenta
	n = 0;


	for (int i = 0; i < 1000; i++)
	{
		buf[i] = '\0';
	}


	for (;;) {

		char ch = *pch;	// caracterul curent
		

		// cate un case pentru fiecare stare din diagrama
		switch (stare) {

		case 0:
			// se testeaza toate tranzitiile posibile din acea stare
			if (isalpha(ch) || ch == '_')
			{
				stare = 1;
				pch++; // consuma caracterul curent
				buf[n++] = ch; // caracterul consumat va fi salvat in buffer pentru a putea fi introdus in structura Atom
			}
			else if (isdigit(ch)) // daca este cifra
			{
				stare = 3;
				pch++;
				buf[n++] = ch;
			}
			else if (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t')
			{ 
				pch++;

				if (ch == '\n')
				{
					linie++; // incrementare linie fisier
				}
			}
			else if (ch == '\"')
			{ 
				stare = 8;
				pch++;
				//imageBuf[nrImageChars++] = ch;
			}
			else if (ch == '#')
			{ 
				stare = 10;
				pch++;
			}
			else if (ch == ':')
			{
				stare = 11;
				pch++;
				buf[n++] = ch;
			}
			else if (ch == ';')
			{
				stare = 12;
				pch++;
				buf[n++] = ch;
			}
			else if (ch == '(')
			{ 
				stare = 13;
				pch++;
				buf[n++] = ch;
			}
			else if (ch == ')')
			{ 
				stare = 14;
				pch++;
				buf[n++] = ch;
			}
			else if (ch == ',')
			{
				stare = 15;
				pch++;
				buf[n++] = ch;
			}
			else if (ch == '|')
			{
				stare = 16;
				pch++;
				buf[n++] = ch;
			}
			else if (ch == '&')
			{
				stare = 18;
				pch++;
				buf[n++] = ch;
			}
			else if (ch == '<')
			{
				stare = 20;
				pch++;
				buf[n++] = ch;
			}
			else if (ch == '+')
			{
				stare = 21;
				pch++;
				buf[n++] = ch;
			}
			else if (ch == '-')
			{
				stare = 22;
				pch++;
				buf[n++] = ch;
			}
			else if (ch == '*') 
			{
				stare = 23;
				pch++;
				buf[n++] = ch;
			}
			else if (ch == '/')
			{
				stare = 24;
				pch++;
				buf[n++] = ch;
			}
			else if (ch == '!')
			{
			stare = 26;
			pch++;
			buf[n++] = ch;
			}
			else if (ch == '=')
			{
				stare = 27;
				pch++;
				buf[n++] = ch;
			
			}
			else if (ch == '\0')
			{
				stare = 25;
				pch++;
			}
			break;

		case 1:
			if (isalnum(ch) || ch == '_')
			{
				pch++;
				buf[n++] = ch;
			}
			else
			{
				stare = 2;
			}
			break;

		case 2:
			// adauga atomul gasit in lista de atomi
			addAtom(ID);
			return ID;

		case 3:
			if (isdigit(ch))
			{
				pch++;
				buf[n++] = ch;
			}
			else if (ch == '.')
			{
				stare = 5;
				pch++;
				buf[n++] = ch;
			}
			else
			{
				stare = 4;
			}
			break;

		case 4:
			addAtom(INT);
			return INT;

		case 5:
			if (isdigit(ch))
			{
				stare = 6;
				pch++;
				buf[n++] = ch;
			}
			break;

		case 6:
			if (isdigit(ch))
			{
				pch++;
				buf[n++] = ch;
			}
			else
			{
				stare = 7;
			}
			break;

		case 7:
			addAtom(REAL);
			return REAL;

		case 8:
			if (ch != '\"')
			{
				pch++;
				buf[n++] = ch;
			}
			else
			{
				stare = 9;
				pch++;
				//imageBuf[nrImageChars++] = ch;
			}
			break;

		case 9:
			addAtom(STR);
			return STR;

		case 10:
			if (ch != '\r' && ch != '\n' && ch != '\0')
			{
				pch++;
			}
			else
			{
				stare = 0;
			}
			break;

		case 11:
			addAtom(COLON);
			return COLON;

		case 12:
			addAtom(SEMICOLON);
			return SEMICOLON;

		case 13:
			addAtom(LPAR);
			return LPAR;

		case 14:
			addAtom(RPAR);
			return RPAR;

		case 15:
			addAtom(COMMA);
			return COMMA;

		case 16:
			if (ch == '|')
			{ 
				stare = 17;
				pch++;
				buf[n++] = ch;
			}
			break;

		case 17:
			addAtom(OR);
			return OR;

		case 18:
			if (ch == '&')
			{
				stare = 19;
				pch++;
				buf[n++] = ch;
			}
			break;

		case 19:
			addAtom(AND);
			return AND;

		case 20:
			addAtom(LESS);
			return LESS;

		case 21:
			addAtom(ADD);
			return ADD;

		case 22:
			addAtom(SUB);
			return SUB;

		case 23:
			addAtom(MUL);
			return MUL;

		case 24:
			addAtom(DIV);
			return DIV;

		case 25:
			addAtom(FINISH);
			return FINISH;

		case 26:
			if (ch == '=')
			{
				stare = 29;
				pch++;
				buf[n++] = ch;
			}
			else
			{
				stare = 28;
			}
			break;

		case 27:
			if (ch == '=')
			{
				stare = 30;
				pch++;
				buf[n++] = ch;
			}
			else 
			{ 
				stare = 31;
			}
			break;

		case 28:
			addAtom(NOT);
			return NOT;

		case 29:
			addAtom(NOTEQUAL);
			return NOTEQUAL;

		case 30:
			addAtom(EQUAL);
			return EQUAL;

		case 31:
			addAtom(ASSIGN);
			return ASSIGN;

		default:
			printf("Linia %d ->  stare invalida %d\n", linie, stare);
		}
	}
}



void afisareAtomiLexicali()
{
	for (int i = 0; i < nrAtomi; i++)
	{
		// daca atomul este ID
		if (atomi[i].cod == 0)
		{
			// in interiorul parantezelor patrate este codul atomului corespunzator, indexul codului din enumeratia tipAtom
			printf("Linia %d ->  %s : %s\n", atomi[i].linieFisier, numeAtomi[atomi[i].cod], atomi[i].image);
		}
		// daca atomul este INT
		else if (atomi[i].cod == 1)
		{
			atomi[i].valueInt = stringToInt(atomi[i].image);
			printf("Linia %d ->  %s : %d\n", atomi[i].linieFisier, numeAtomi[atomi[i].cod], atomi[i].valueInt);
		}
		// daca atomul este REAL
		else if (atomi[i].cod == 2)
		{
			atomi[i].valueDouble = stringToDouble(atomi[i].image);
			printf("Linia %d ->  %s : %lf\n", atomi[i].linieFisier, numeAtomi[atomi[i].cod], atomi[i].valueDouble);
		}
		// daca atomul este STR
		else if (atomi[i].cod == 3)
		{
			printf("Linia %d ->  %s : %s\n", atomi[i].linieFisier, numeAtomi[atomi[i].cod], atomi[i].image);
		}
		else
		{
			printf("Linia %d ->  %s\n", atomi[i].linieFisier, numeAtomi[atomi[i].cod]);
			//printf("Linia %d ->  %s : %s\n", atomi[i].linieFisier, tipAtomTablou[atomi[i].cod], atomi[i].image);
		}
	}
}


// metoda ce va realiza analiza lexicala 
void analizatorLexical()
{
	FILE* fisier;
	errno_t error;
	error = fopen_s(&fisier, "file.q", "rb");

	if (error != 0) {
		printf("Fisierul nu s-a putut deschide");
		return -1;
	}

	int n = fread(bufin, 1, 30000, fisier);	// returneaza nr de elemente citite integral
	bufin[n] = '\0'; // se adauga terminatorul de sir la finalul buffer-ului
	fclose(fisier);
	pch = bufin;	// initializare pch pe prima pozitie din bufin
	
	// extragere atomi
	while (getNextTk() != FINISH) {
	}
	
	afisareAtomiLexicali();
	printf("\n\n");
}


int main()
{
	analizatorLexical();
}
