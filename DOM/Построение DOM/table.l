%{
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cctype>
#include "table.h"
//компилировать совместно с table.cpp

// основной объект генератора
static Table *tab;

static const char * fileOut;


// {tr}
#define	GETTR {\
	tab->addText(yyout, fileOut);/*записать в дерево фрагмент текста перед командой*/\
	tab->addTR(yytext);/*После выполнения каждой команды содержимое вых. файла затирается*/\
		 }
// {td]
#define	GETTD {\
	tab->addText(yyout, fileOut);\
	tab->addTD(yytext, 0, 0);\
		 }
// {td; rowspan:" ... "} или {rowspan:" ... "; td}
#define	GETTD_ROW {\
	tab->addText(yyout, fileOut);\
	tab->addTD(yytext, 1, 0);\
		 }
// {td; colspan:" ... "} или {colspan:" ... "; td}
#define	GETTD_COL {\
	tab->addText(yyout, fileOut);\
	tab->addTD(yytext, 0, 1);\
		 }

// {td; colspan:" ... "; rowspan:" ... "} или {colspan:" ... "; rowspan:" ... "; td}  или {colspan:" ... "; td; rowspan:" ... "}
#define	GETTD_COL_ROW_1 {\
	tab->addText(yyout, fileOut);\
	tab->addTD(yytext, 2, 1);\
		 }
// {td; rowspan:" ... "; colspan:" ... "} или {rowspan:" ... "; colspan:" ... "; td} или {rowspan:" ... "; td; colspan:" ... "}
#define	GETTD_COL_ROW_2 {\
	tab->addText(yyout, fileOut);\
	tab->addTD(yytext, 1, 2);\
		 }

%}
%%  
\{[ \t]*[T|t][R|r][ \t]*\}		GETTR
\{[ \t]*[T|t][D|d][ \t]*\}		GETTD
\{[ \t]*[T|t][D|d][ \t]*;[ \t]*[R|r][O|o][W|w][S|s][P|p][A|a][N|n][ \t]*:[ \t]*\"[ \t]*[1|2|3|4|5|6|7|8|9|0]+[ \t]*\"[ \t]*\}		GETTD_ROW
\{[ \t]*[R|r][O|o][W|w][S|s][P|p][A|a][N|n][ \t]*:[ \t]*\"[ \t]*[1|2|3|4|5|6|7|8|9|0]+[ \t]*\"[ \t]*;[ \t]*[T|t][D|d][ \t]*\}		GETTD_ROW
\{[ \t]*[T|t][D|d][ \t]*;[ \t]*[C|c][O|o][L|l][S|s][P|p][A|a][N|n][ \t]*:[ \t]*\"[ \t]*[1|2|3|4|5|6|7|8|9|0]+[ \t]*\"[ \t]*\}		GETTD_COL
\{[ \t]*[C|c][O|o][L|l][S|s][P|p][A|a][N|n][ \t]*:[ \t]*\"[ \t]*[1|2|3|4|5|6|7|8|9|0]+[ \t]*\"[ \t]*;[ \t]*[T|t][D|d][ \t]*\}		GETTD_COL
\{[ \t]*[T|t][D|d][ \t]*;[ \t]*[C|c][O|o][L|l][S|s][P|p][A|a][N|n][ \t]*:[ \t]*\"[ \t]*[1|2|3|4|5|6|7|8|9|0]+[ \t]*\"[ \t]*;[ \t]*[R|r][O|o][W|w][S|s][P|p][A|a][N|n][ \t]*:[ \t]*\"[ \t]*[1|2|3|4|5|6|7|8|9|0]+[ \t]*\"[ \t]*\}		GETTD_COL_ROW_1
\{[ \t]*[T|t][D|d][ \t]*;[ \t]*[R|r][O|o][W|w][S|s][P|p][A|a][N|n][ \t]*:[ \t]*\"[ \t]*[1|2|3|4|5|6|7|8|9|0]+[ \t]*\"[ \t]*;[ \t]*[C|c][O|o][L|l][S|s][P|p][A|a][N|n][ \t]*:[ \t]*\"[ \t]*[1|2|3|4|5|6|7|8|9|0]+[ \t]*\"[ \t]*\}		GETTD_COL_ROW_2
\{[ \t]*[C|c][O|o][L|l][S|s][P|p][A|a][N|n][ \t]*:[ \t]*\"[ \t]*[1|2|3|4|5|6|7|8|9|0]+[ \t]*\"[ \t]*;[ \t]*[T|t][D|d][ \t]*;[ \t]*[R|r][O|o][W|w][S|s][P|p][A|a][N|n][ \t]*:[ \t]*\"[ \t]*[1|2|3|4|5|6|7|8|9|0]+[ \t]*\"[ \t]*\}		GETTD_COL_ROW_1
\{[ \t]*[R|r][O|o][W|w][S|s][P|p][A|a][N|n][ \t]*:[ \t]*\"[ \t]*[1|2|3|4|5|6|7|8|9|0]+[ \t]*\"[ \t]*;[ \t]*[T|t][D|d][ \t]*;[ \t]*[C|c][O|o][L|l][S|s][P|p][A|a][N|n][ \t]*:[ \t]*\"[ \t]*[1|2|3|4|5|6|7|8|9|0]+[ \t]*\"[ \t]*\}		GETTD_COL_ROW_2
\{[ \t]*[C|c][O|o][L|l][S|s][P|p][A|a][N|n][ \t]*:[ \t]*\"[ \t]*[1|2|3|4|5|6|7|8|9|0]+[ \t]*\"[ \t]*;[ \t]*[R|r][O|o][W|w][S|s][P|p][A|a][N|n][ \t]*:[ \t]*\"[ \t]*[1|2|3|4|5|6|7|8|9|0]+[ \t]*\"[ \t]*;[ \t]*[T|t][D|d][ \t]*\}		GETTD_COL_ROW_1
\{[ \t]*[R|r][O|o][W|w][S|s][P|p][A|a][N|n][ \t]*:[ \t]*\"[ \t]*[1|2|3|4|5|6|7|8|9|0]+[ \t]*\"[ \t]*;[ \t]*[C|c][O|o][L|l][S|s][P|p][A|a][N|n][ \t]*:[ \t]*\"[ \t]*[1|2|3|4|5|6|7|8|9|0]+[ \t]*\"[ \t]*;[ \t]*[T|t][D|d][ \t]*\}		GETTD_COL_ROW_2
%%

// в командной строке указывается имя файла с текстом таблицы на языке разметки и имя выходного файла
int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		puts("Внутренняя ошибка приложения!");
		return 1;
	}

	if(!(yyin = fopen(argv[1], "rt" )))
	{
		printf ("Не удалось открыть временный файл \"%s\"\n", argv[1]);
		return 1;
	}
	
	fileOut = argv[2];
	remove(fileOut);

	if(!(yyout = fopen(fileOut, "wt" ))) // ненужный выходной поток, генерируемый лексером, сбрасывать во временный файл, из которого считывать произвольный текст
	{
		printf ("Не удалось открыть временный файл \"%s\"\n", fileOut);
		return 1;
	}
	tab = new Table(); 
	yylex(); // запуск лексера, заполнение списка лексем
	tab->addText(yyout, fileOut); // записать в список лексем произвольный текст после последней команды, или всё содержимое вх. файла, если ни одной команды не встретилось
	fclose(yyin);
	fclose(yyout);
	remove(fileOut); // удалить временный файл
	if(!(yyout = fopen(fileOut, "wt" ))) // запись результата в файл
	{
		printf ("Не удалось открыть временный файл \"%s\"\n", fileOut);
		return 1;
	}
	fputs (tab->genTableCode().c_str(), yyout); // сгенерировать код таблицы на языке LaTeX
	fclose(yyout);
	delete tab; // удалить таблицу из памяти
	return 0;
}
