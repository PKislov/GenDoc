%{
#include <cstdio>
#include <string>
#include <cctype>
#include <cstdlib>
#include <ctime>
using namespace std;

static const char *fileOut;
static const char *fileIn;
static bool runIncludeRef; // равно true если в последнем вызове yylex() была выполнена хоть одна команда GETINCLUDEREF. Если true, то запустить лексер выходного файла, и так пока не будет false (т.е. не включатся все вложенные файлы).

static void getIncludeRef ();

/*#define	GETINCLUDEID {\
	char *ptr=yytext;\
	fprintf(yyout,"Включить файл: идентификатор \"");\
	while(*ptr!=':')++ptr;++ptr;\
	while (isspace(*ptr))++ptr;\
	while(isalpha(*ptr)||isdigit(*ptr))putc(*ptr, yyout),++ptr;\
	fprintf(yyout, "\".\n");\
		 }
*/
//@include[ \t]*\{[ \t]*id[ \t]*:[ \t]*[a-zA-Z][a-zA-Z0-9]*[ \t]*\}	GETINCLUDEID
%}
%%  
@include[ \t]*\{[ \t]*ref[ \t]*:[ \t]*\"[^"\""]+\"[ \t]*\}		getIncludeRef ();
%%
int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		puts("Необходимо указать имена входного и выходного файлов!");
		return 1;
	}
	fileIn = argv[1];
	fileOut = argv[2];
	if (!(yyin = fopen(fileIn, "rt" )))
	{
		printf ("Не удалось открыть файл \"%s\"\n", fileIn);
		return 1;
	}
	if (!(yyout = fopen(fileOut, "wt")))
	{
		printf ("Не удалось открыть файл \"%s\"\n", fileOut);
		return 1;
	}
	runIncludeRef=false;
	yylex(); // запускк лексера
	fclose(yyin);
	fclose(yyout);	
	
	if(runIncludeRef) // если в последнем вызове yylex() была выполнена хоть одна команда GETINCLUDEREF (подключали ли хоть один файл)
	{
		char tName1 [L_tmpnam], tName2 [L_tmpnam]; // сгенерировать уникальные имена файлов
		if (!tmpnam (tName1) || !tmpnam (tName2))
		{
			puts("Внутренняя ошибка приложения.");
			return 1;
		}
		if (rename(fileOut, tName1))
		{
			puts("Внутренняя ошибка приложения.");
			return 1;
		}

		while(runIncludeRef) // пока не вставим содержимое всех вложенных подключаемых файлов
		{
			runIncludeRef=false;
			if (!(yyin = fopen(tName1, "rt" )))
			{
				printf ("Не удалось открыть файл \"%s\"\n", fileIn);
				return 1;
			}
			if (!(yyout = fopen(tName2, "wt")))
			{
				printf ("Не удалось открыть файл \"%s\"\n", fileOut);
				return 1;
			}
			yylex();
			fclose(yyin);
			fclose(yyout);
			if (rename(tName2, tName1)) // перейти к включению в вых. файл содержимого следующих вложенных подключаемых файлов
			{
				puts("Внутренняя ошибка приложения.");
				return 1;
			}
		}
		if (rename(tName1, fileOut))
		{
			puts("Внутренняя ошибка приложения.");
			return 1;
		}
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
		//fprintf(yyout,"Не удалось открыть файл \"%s\"", name.c_str());
		exit(1);
	}
	else
	{
		char ch;
		string stemp(""); // в буфер stemp записать содержимое файла temp и записать в файл yyout, кроме символа с кодом 10 в конце буфера, если такой имеется (исправление особенности текстового редактора gedit, дописывающего этот символ в конце каждого текстового файла) 
		while(!feof(temp))
		{
			ch=getc(temp);
			if(!feof(temp)) stemp.push_back (ch)/*putc(ch, yyout)*/;
		}
		fclose(temp);	
		if (stemp[stemp.size()-1] == 10) stemp.erase(stemp.size()-1); // удаляем символ 10
		int stempSize = stemp.size(); // записываем буфер в файл
		for (int i=0; i< stempSize; i++) putc(stemp[i], yyout);
		runIncludeRef=true;
	}
}
