%{
#include <stdio.h>
#include <string>
#include <ctype.h>
#include <cstdlib>
#include <ctime>
using namespace std;

static const char *fileOut="/home/pavel/Рабочий стол/построение DOM/data.txt";
static const char *fileIn="data.txt";
static bool runIncludeRef; // равно true если в последнем вызове yylex() была выполнена хоть одна команда GETINCLUDEREF. Если true, то запустить лексер выходного файла, и так пока не будет false (т.е. не включатся все вложенные файлы).

static void getIncludeRef ();

#define	GETINCLUDEID {\
	/*char *ptr=yytext;\
	fprintf(yyout,"Включить файл: идентификатор \"");\
	while(*ptr!=':')++ptr;++ptr;\
	while (isspace(*ptr))++ptr;\
	while(isalpha(*ptr)||isdigit(*ptr))putc(*ptr, yyout),++ptr;\
	fprintf(yyout, "\".\n");*/\
		 }

%}
%%  
@include[ \t]*\{[ \t]*id[ \t]*:[ \t]*[a-zA-Z][a-zA-Z0-9]*[ \t]*\}	GETINCLUDEID
@include[ \t]*\{[ \t]*ref[ \t]*:[ \t]*\"[^"\""]+\"[ \t]*\}		getIncludeRef ();
%%
int main()
{
	if (!(yyin = fopen(fileIn, "rt" )))
	{
		printf ("Не удалось открыть файл \"%s\"\n", fileIn);
		return 0;
	}
	if (!(yyout = fopen(fileOut, "wt")))
	{
		printf ("Не удалось открыть файл \"%s\"\n", fileOut);
		return 0;
	}
	runIncludeRef=false;
	yylex(); // запускк лексера
	fclose(yyin);
	fclose(yyout);	
	
	if(runIncludeRef) // если в последнем вызове yylex() была выполнена хоть одна команда GETINCLUDEREF (подключали ли хоть один файл)
	{
		char tName1 [100], tName2 [100];
		tmpnam (tName1); tmpnam (tName2); // сгенерировать уникальные имена файлов
		rename(fileOut, tName1);

		while(runIncludeRef) // пока не вставим содержимое всех вложенных подключаемых файлов
		{
			runIncludeRef=false;
			if (!(yyin = fopen(tName1, "rt" )))
			{
				printf ("Не удалось открыть файл \"%s\"\n", fileIn);
				return 0;
			}
			if (!(yyout = fopen(tName2, "wt")))
			{
				printf ("Не удалось открыть файл \"%s\"\n", fileOut);
				return 0;
			}
			yylex();
			fclose(yyin);
			fclose(yyout);
			rename(tName2, tName1); // перейти к включению в вых. файл содержимого следующих вложенных подключаемых файлов
		}
		rename(tName1, fileOut);
	}
	return 0;
}

void getIncludeRef ()
{
	char *ptr=yytext;
	while(*ptr!='\"') ++ptr; ++ptr;
	string name("");
	while(*ptr!='\"') name.append(1, *ptr), ++ptr; ++ptr;
	FILE *temp;
	if (!(temp = fopen(name.c_str(), "rt" )))
	{
		printf("Не удалось открыть файл \"%s\"\n", name.c_str());
		fprintf(yyout,"Не удалось открыть файл \"%s\"", name.c_str());
	}
	else
	{
		char ch;
		while(!feof(temp))
		{
			ch=getc(temp);
			if(!feof(temp)) putc(ch, yyout);
		}
		fclose(temp);
		runIncludeRef=true;
	}
}
