#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define MAX_ATOMI 10000


typedef enum {
	ID, INT, REAL, STR, VAR, FUNCTION, IF, ELSE, WHILE, END, RETURN, TYPE_INT, TYPE_REAL,
	TYPE_STR, COLON, SEMICOLON, LPAR, RPAR, COMMA, OR, AND, NOT, EQUAL, NOTEQUAL, LESS, ASSIGN, ADD,
	SUB, MUL, DIV, FINISH
}tipAtom;


char numeAtomi[31][10] = { "ID", "INT", "REAL", "STR", "VAR", "FUNCTION", "IF", "ELSE", "WHILE", "END", "RETURN",
							"TYPE_INT", "TYPE_REAL", "TYPE_STR", "COLON", "SEMICOLON", "LPAR", "RPAR",
							"COMMA", "OR", "AND", "NOT", "EQUAL", "NOTEQUAL", "LESS", "ASSIGN", "ADD", "SUB",
							"MUL", "DIV", "FINISH" 
};


char tipCuvinteCheie[10][10] = { "var", "function", "if", "else", "while", "end", "return", "int", "real", "str" };


union lexicalInformation
{
	char stringValue[1000];
	int valueInt;
	double valueDouble;
};


typedef struct {
	int cod;	// codul atomului
	int linie;
	union lexicalInformation lexicalInfo;
}Atom;


Atom atomi[MAX_ATOMI];
int nrAtomi = 0;	// numarul de atomi din vectorul atomi
int idxCrtAtom = 0;	// indexul atomului curent din atomi - pentru analiza sintactica

char bufin[30001];
char* pch;	// cursor la caracterul curent din bufin


char buf[1000];
int n = 0;

int linie = 1;	// linia curenta; adaugata automat la atom de addAtom


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
	buf[n] = 0;

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
	//strcpy_s(atomi[nrAtomi].stringValue, 1000, buf);
	atomi[nrAtomi].linie = linie;


	if (atomi[nrAtomi].cod == 1)
	{
		atomi[nrAtomi].lexicalInfo.valueInt = stringToInt(buf);
	}
	else if (atomi[nrAtomi].cod == 2)
	{
		atomi[nrAtomi].lexicalInfo.valueDouble = stringToDouble(buf);
	}
	else
	{
		strcpy_s(atomi[nrAtomi].lexicalInfo.stringValue, 1000, buf);
	}
	nrAtomi++;
}


// afisarea atomilor lexicali
void afisareAtomiLexicali()
{
	int linieAfisare = 0;

	if (linieAfisare < atomi[0].linie)
	{
		linieAfisare = atomi[0].linie;
		printf("Linia %d: ", linieAfisare);

		for (int i = 0; i < nrAtomi; i++)
		{

			if (linieAfisare < atomi[i].linie)
			{
				linieAfisare = atomi[i].linie;
				printf("\nLinia %d: ", linieAfisare);
			}
			// daca atomul este ID
			if (atomi[i].cod == 0)
			{
				// in interiorul parantezelor patrate este codul atomului corespunzator, indexul codului din enumeratia tipAtom
				printf(" %s : %s, ", numeAtomi[atomi[i].cod], atomi[i].lexicalInfo.stringValue);
			}
			// daca atomul este INT
			else if (atomi[i].cod == 1)
			{
				printf(" %s : %d, ", numeAtomi[atomi[i].cod], atomi[i].lexicalInfo.valueInt);
			}
			// daca atomul este REAL
			else if (atomi[i].cod == 2)
			{
				printf(" %s : %lf, ", numeAtomi[atomi[i].cod], atomi[i].lexicalInfo.valueDouble);
			}
			// daca atomul este STR
			else if (atomi[i].cod == 3)
			{
				printf(" %s : %s, ", numeAtomi[atomi[i].cod], atomi[i].lexicalInfo.stringValue);
			}
			else
			{
				printf("%s, ", numeAtomi[atomi[i].cod]);
				//printf("Linia %d ->  %s : %s", atomi[i].linie, numeAtomi[atomi[i].cod], atomi[i].lexicalInfo.stringValue);
			}
		}
	}
}


// la fiecare apel returneaza codul unui atom
int getNextTk()			// get next token (atom lexical)
{
	int stare = 0;		// starea curenta
	n = 0;


	for (int i = 0; i < 1000; i++)
	{
		buf[i] = 0;
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
					linie++; // incrementare linieAfisare fisier
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
	while (getNextTk() != FINISH)
	{

	}

	afisareAtomiLexicali();
	printf("\n\n");
}




//--------------------------------------------------------------------------------
//	analiza sintatica


// functia consume - se foloseste pentru a consuma atomi lexicali
// daca la pozitia curenta a analizei lexicale avem un atom cu codul "cod",
// atunci se trece mai departe (se consuma atomul respectiv) si se returneaza true
// altfel, sa ramana la atomul curent si se returneaza false
int consume(int cod) 
{
	if (atomi[idxCrtAtom].cod == cod) {
		idxCrtAtom++;
		return 1;
	}

	return 0;
}


// afiseaza locatia atomului curent (idxCrtAtom)
// afiseaza mesajul de eroare
// iese din program
void afisareEroare(const char* mesaj)
{
	printf("Eroare in linia %d: %s", atomi[idxCrtAtom].linie, mesaj);
	exit(1);
}


/*
- fiecare regula sintactica devine o functie fara parametri care consuma atomii corespunzatori corpului ei
ex: functiile program, defVar, baseType, ...
- fiecare functie returneaza true daca a reusit sa consume tot corpul ei, altfel returneaza false
- daca o functie returneaza false, nu trebuie sa modifice pozitia curenta la care am ajuns cu analiza sintactica
- atomii se consuma folosind o functie speciala, numita consume
- regulile se consuma, apelandu-se in mod direct
*/

int program();
int defVar();
int baseType();
int defFunc();
int block();
int funcParams();
int funcParam();
int instr();
int expr();
int exprLogic();
int exprAssign();
int exprComp();
int exprAdd();
int exprMul();
int exprPrefix();
int factor();



// factor ::= INT | REAL | STR | LPAR expr RPAR | ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
int factor()
{
	int startIdx = idxCrtAtom;
	if (consume(INT) || consume(REAL) || consume(STR))
	{
		return 1;
	}

	// LPAR expr RPAR
	if (consume(LPAR))
	{
		if (expr())
		{
			if (consume(RPAR))
			{
				return 1;
			}
			else 
				afisareEroare("Lipseste ) dupa expresie.\n");
		}
		else
			afisareEroare("Expresie gresita dupa (\n");
	}
	// ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
	if (consume(ID))
	{
		if (consume(LPAR))
		{
			if (expr())
			{
				for (;;)
				{
					if (consume(COMMA))
					{
						if (expr())
						{
							;
						}
						else
						{
							afisareEroare("Lipseste expresie dupa ,\n");
							idxCrtAtom = startIdx;
							return 0;
						}
					}
					else
						break;
				}
			}
			if (consume(RPAR))
			{
				return 1;
			}
			else
			{
				afisareEroare("Lipseste )\n");
				idxCrtAtom = startIdx;
				return 0;
			}
		}
		return 1;
	}


	idxCrtAtom = startIdx;
	return 0;
}


// baseType ::= TYPE_INT | TYPE_REAL | TYPE_STR
int baseType()
{
	int startIdx = idxCrtAtom;

	if (consume(TYPE_INT) || consume(TYPE_REAL) || consume(TYPE_STR)) 
	{
		return 1;
	}

	idxCrtAtom = startIdx;
	return 0;
}


// defFunc :: = FUNCTION ID LPAR funcParams RPAR COLON baseType defVar * block END
int defFunc()
{
	int startIdx = idxCrtAtom;

	if (consume(FUNCTION)) 
	{
		if (consume(ID))
		{
			if (consume(LPAR))
			{
				if (funcParams())
				{
					if (consume(RPAR))
					{
						if (consume(COLON))
						{
							if (baseType())
							{
								for (;;)
								{
									if (defVar())
									{
										;
									}
									else
										break;
								}
								if (block())
								{
									if (consume(END))
									{
										return 1;
									}
									else
										afisareEroare("Lipseste END dupa block.\n");
								}
								else
									afisareEroare("Lipseste BLOCK.\n");
							}
							else
								afisareEroare("Lipseste tipul returnat de functie SAU tipul nu este valid.\n");
						}
						else
							afisareEroare("Lipseste : dupa ).\n");
					}
					else
						afisareEroare("Lipseste ) dupa declararea variabilelor.\n");
				}

			}
			else
				afisareEroare("Lipseste ( dupa numele functiei.\n");
		}
		else
			afisareEroare("Lipseste numele functiei.\n");
	}

	idxCrtAtom = startIdx;
	return 0;
}

// expr ::= exprLogic
int expr()
{
	int startIdx = idxCrtAtom;

	if (exprLogic())
	{
		return 1;
	}

	idxCrtAtom = startIdx;
	return 0;
}


// exprAssign ::= ( ID ASSIGN )? exprComp
int exprAssign()
{
	int startIdx = idxCrtAtom;

	if (consume(ID)) 
	{
		if(consume(ASSIGN))
		{
			if (exprComp())
			{
				return 1;
			}
			else
				afisareEroare("Lipseste expresia ce urmeaza dupa = .\n");
		}
		else
		{
			idxCrtAtom = startIdx;
		}
	}

	if (exprComp)
	{
		return 1;
	}

	idxCrtAtom = startIdx;
	return 0;
}


// exprComp ::= exprAdd ( ( LESS | EQUAL ) exprAdd )?
int exprComp()
{
	int startIdx = idxCrtAtom;
	if (exprAdd())
	{
		if (consume(LESS) || consume(EQUAL))
		{
			if (exprAdd())
			{
				return 1;
			}
			else
				afisareEroare("Lipseste expresia ce urmeaza dupa < sau dupa =\n");

			idxCrtAtom = startIdx;
			return 0;
		}
		return 1;
	}

	idxCrtAtom = startIdx;
	return 0;
}


// exprLogic ::= exprAssign ( ( AND | OR ) exprAssign )*
int exprLogic()
{
	int startIdx = idxCrtAtom;

	if (exprAssign())
	{
		for (;;)
		{
			if (consume(AND) || consume(OR))
			{
				if (exprAssign())
				{
					;
				}
				else
				{
					afisareEroare("Lipseste expresia ce urmeaza dupa & sau dupa |.\n");
					idxCrtAtom = startIdx;
					return 0;
				}
			}
			else
				break;
		}
		return 1;
	}

	idxCrtAtom = startIdx;
	return 0;
}

// exprAdd :: = exprMul((ADD | SUB) exprMul) *
int exprAdd()
{
	int startIdx = idxCrtAtom;

	if (exprMul())
	{
		for (;;)
		{
			if (consume(ADD) || consume(SUB))
			{
				if (exprMul())
				{
					;
				}
				else
				{
					afisareEroare("Lipseste expresia ce urmeaza dupa + sau -.\n");
					idxCrtAtom = startIdx;
					return 0;
				}
			}
			else
				break;
		}
		return 1;
	}

	idxCrtAtom = startIdx;
	return 0;
}


// exprMul ::= exprPrefix ( ( MUL | DIV ) exprPrefix )*
int exprMul()
{
	int startIdx = idxCrtAtom;

	if (exprPrefix())
	{
		for (;;)
		{
			if (consume(MUL) || consume(DIV))
			{
				if (exprPrefix())
				{
					;
				}
				else
				{
					afisareEroare("Lipseste expresia ce urmeaza dupa * sau dupa /.\n");
					idxCrtAtom = startIdx;
					return 0;
				}
			}
			else
				break;
		}
		return 1;
	}

	idxCrtAtom = startIdx;
	return 0;
}


// exprPrefix :: = (SUB | NOT) ? factor
int exprPrefix()
{
	int startIdx = idxCrtAtom;

	if (consume(SUB) || consume(NOT))
	{
		if (factor())
		{
			return 1;
		}
		else
			afisareEroare("Lipseste factor dupa - sau dupa ! .\n");
	}
	if (factor())
	{
		return 1;
	}

	idxCrtAtom = startIdx;
	return 0;
}


//instr :: = expr ? SEMICOLON
//          | IF LPAR expr RPAR block(ELSE block) ? END
//			| RETURN expr SEMICOLON
//			| WHILE LPAR expr RPAR block END
int instr()
{
	int startIdx = idxCrtAtom;

	if (expr())
	{
		if (consume(SEMICOLON))
		{
			return 1;
		}
		else
			afisareEroare("Lipseste ; dupa expresie.\n");
	}
	else if (consume(SEMICOLON))
	{
		return 1;
	}
	//	| IF LPAR expr RPAR block ( ELSE block )? END
	else if (consume(IF))
	{
		if (consume(LPAR))
		{
			if (expr())
			{
				if (consume(RPAR))
				{
					if (block())
					{
						if (consume(ELSE))
						{
							if (block())
							{
								;
							}
							else
							{
								afisareEroare("Lipseste block dupa else.\n");
								idxCrtAtom = startIdx;
								return 0;
							}
						}
						if (consume(END))
						{
							return 1;
						}
						else
							afisareEroare("Lipseste end dupa block.\n");
					}
					else
						afisareEroare("Lipseste block dupa ).\n");
				}
				else
					afisareEroare("Lipseste ) dupa expresie.\n");
			}
			else
				afisareEroare("Lipseste expresie dupa (.\n");
		}
		else
			afisareEroare("lipseste ( dupa if.\n");
	}
	//	| RETURN expr SEMICOLON
	else if (consume(RETURN))
	{
		if (expr())
		{
			if (consume(SEMICOLON))
				return 1;
			else
				afisareEroare("Lipseste ; dupa expresie.\n");
		}
		else
			afisareEroare("Lipseste expresia dupa RETURN.\n");
	}
	//	| WHILE LPAR expr RPAR block END
	else if (consume(WHILE))
	{
		if (consume(LPAR))
		{
			if (expr())
			{
				if (consume(RPAR))
				{
					if (block())
					{
						if (consume(END))
							return 1;
						else
							afisareEroare("Lipseste end dupa block.\n");
					}
					else
						afisareEroare("Lipseste block dupa ).\n");
				}
				else
					afisareEroare("Lipseste ) dupa expresie.\n");
			}
			else
				afisareEroare("Lipseste expresie dupa (.\n");
		}
		else
			afisareEroare("Lipseste ( dupa while.\n");
	}

	idxCrtAtom = startIdx;
	return 0;
}


 // block ::= instr+
int block()
{
	int startIdx = idxCrtAtom;

	if (instr())
	{
		for (;;)
		{
			if (instr())
			{
				;
			}
			else
				break;
		}
		return 1;
	}

	idxCrtAtom = startIdx;
	return 0;
}


//	funcParam ::= ID COLON baseType
int funcParam()
{
	int startIdx = idxCrtAtom;

	if (consume(ID))
	{
		if (consume(COLON))
		{
			if (baseType())
			{
				return 1;
			}
				
			else
				afisareEroare("Lipseste tipul variabilei sau tipul variabilei este invalid.\n");
		}
		else
			afisareEroare("Lipseste : dupa numele parametrului.\n");
	}

	idxCrtAtom = startIdx;
	return 0;
}


//	funcParams ::= ( funcParam ( COMMA funcParam )* )?
int funcParams()
{
	int startIdx = idxCrtAtom;

	if (funcParam())
	{
		for (;;)
		{
			if (consume(COMMA))
			{
				if (funcParam())
				{
					;
				}
				else
				{
					afisareEroare("Lipseste parametru dupa virgula sau este virgula in plus.\n");
					idxCrtAtom = startIdx;
					return 0;
				}
			}
			else
				break;
		}
		return 1;
	}

	return 1;
}


//	VAR ID COLON baseType SEMICOLON
int defVar()
{
	int startIdx = idxCrtAtom;

	if (consume(VAR))
	{
		if (consume(ID))
		{
			if (consume(COLON))
			{
				if (baseType())
				{
					if (consume(SEMICOLON))
					{
						return 1;
					}
					else
						afisareEroare("Lipseste ; la finalul declaratiei de variabila.\n");
				}
				else
					afisareEroare("Lipseste tipul variabilei sau tipul este invalid.\n");
			}
			else
				afisareEroare("Lipseste : dupa numele variabilei.\n");
		}
		else
			afisareEroare("Lipseste numele variabilei.\n");
	}

	idxCrtAtom = startIdx;
	return 0;
}


// program ::= ( defVar | defFunc | block )* FINISH
int program()
{
	int startIdx = idxCrtAtom;

	for (;;)
	{
		if (defVar()) {}
		else if (defFunc()) {}
		else if (block()) {}
		else
			break;
	}
	if (consume(FINISH))
	{
		return 1;
	}
	else
		afisareEroare("Eroara de sintaxa!\n");
		//afisareEroare("Lipseste FINISH.\n");

	idxCrtAtom = startIdx;
	return 0;
}



int main()
{
	analizatorLexical();

	// analiza sintactica
	if (program())
	{
		printf("Codul a fost compilat fara erori de sintaxa!\n");
	}
	else
	{
		printf("Eroare de sintaxa!\n");
	}


	return 0;
}