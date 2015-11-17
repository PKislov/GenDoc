%{
#include <cstdio>
#include <string>
#include <cctype>
#include "fileRing.h"
using namespace std;

static char tName1 [L_tmpnam]; // выходной временный файл
static const char *fileIn;

// основной объект дерева
static FileTree *tree;

static void getIncludeRef ();
%}
%%  
@[I|i][N|n][C|c][L|l][U|u][D|d][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]+\"[ \t]*\}		getIncludeRef ();
%%
int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		puts("Не указано имя файла!");
		return 1;
	}
	fileIn = argv[1];
	if (!(yyin = fopen(fileIn, "rt" )))
	{
		printf ("Не удалось открыть файл \"%s\"\n", fileIn);
		return 1;
	}
	if(!tmpnam (tName1)) // сгенерировать имя временного файла
	{
		puts("Внутренняя ошибка приложения.");
		return 1;
	}
	if (!(yyout = fopen(tName1, "w+t"))) // ненужный выходной поток, генерируемый лексером, сбрасывать во временный файл
	{
		printf ("Не удалось открыть файл \"%s\"\n", tName1);
		return 1;
	}
	tree = new FileTree(fileIn); // создать корневой элемент дерева с именем входного файла
	yylex(); // запуск лексера. Если ошибки включений файлов будут обнаружены, на экран выведется сообщение и приложение прекратится exit(0)
	delete tree;
	fclose(yyin);
	fclose(yyout);
	remove(tName1);
	//puts("Ошибок в подключении файлов не обнаружено.");
	return 0;
}

void getIncludeRef ()
{
	if (ftell(yyout)) // если содержимое вых. файла не пустое
	{
		fseek(yyout, -1, SEEK_CUR);
		if (getc(yyout) == '\\') // если последний символ в файле '\\'
		{
			fseek(yyout, -1, SEEK_CUR); // поставить курсор перед знаком '\\'
			fprintf(yyout, "%s", yytext); // на его место записать команду @include и выйти
			return;
		}
		else
			{
				fseek (yyout, 0, SEEK_END); // вернуть курсор в конец файла
			}	
	}

	if (tree->temp->countF) // счетчик выполненных команд include в файле, используется для "прогонки" файла через yyl 
	{	
		--tree->temp->countF;
		return;
	}
	char *ptr = yytext;
	while(*ptr!='\"') ++ptr; ++ptr; // найти имя подключаемого файла в строке команды
	string name;
	while(*ptr!='\"') name.push_back(*ptr), ++ptr;
	FILE *temp;
	if (!(temp = fopen(name.c_str(), "rt" ))) // проверить, что файл существует
	{
		printf("Не удалось открыть файл \"%s\"\n", name.c_str());
		exit (1);
	}
	else
	{
		fclose(temp);
		if(!tree->addElem(name.c_str()))
		{
			printf("Файл %s включает самого себя!\n", name.c_str());
			exit(1);
		}
		tree->temp->parent->countF = tree->temp->parent->children.size();
		fclose(yyin);
		yyrestart(yyin); // сброс настроек лексера
		if (!(yyin = fopen(name.c_str(), "rt" )))
		{
			printf("Не удалось открыть файл \"%s\"\n", name.c_str());
			exit (1);
		}
		yylex();
		tree->temp = tree->temp->parent;
		fclose(yyin);
		if(!(yyin = fopen(tree->getCurName(), "rt")))
		{
			printf("Не удалось открыть файл \"%s\"\n", tree->getCurName());
			exit (1);
		}
	}
}
