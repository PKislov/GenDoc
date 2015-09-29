%{
#include <stdio.h>
#include "domBuilder.h"
//компилировать совместно с "domBuilder.cpp"

// основной объект дерева
static Dom *tree;

static const char *fileIn="data.txt";
static const char *fileOut="result.txt";

#define GETTITLE {\
	tree->addText(yyout, fileOut);/*записать в дерево фрагмент текста перед командой*/\
	/*После выполнения каждой команды содержимое вых. файла затирается*/\
	tree->addTitle(yytext);\
		 }

#define GETTOC	{\
	tree->addText(yyout, fileOut);\
	tree->addToc();\
		}

#define	GETSECTION1 {\
	tree->addText(yyout, fileOut);\
	tree->addSection1(yytext);\
		 }

#define	GETSECTION2 {\
	tree->addText(yyout, fileOut);\
	tree->addSection2(yytext);\
		 }

#define	GETSECTION3 {\
	tree->addText(yyout, fileOut);\
	tree->addSection3(yytext);\
		 }

#define	GETSECTION4 {\
	tree->addText(yyout, fileOut);\
	tree->addSection4(yytext);\
		 }

#define	GETIMAGE {\
	tree->addText(yyout, fileOut);\
	tree->addImageRef(yytext);\
		 }

#define	GETIMAGEID {\
	tree->addText(yyout, fileOut);\
	tree->addImageId(yytext);\
		 }
%}
%%  
@title[ \t]+[^"@"]+@end[ \t]+title		GETTITLE
@toc[ \t]*['\n']*				GETTOC
@section1[ \t]+[^'\n']+['\n']+			GETSECTION1
@section2[ \t]+[^'\n']+['\n']+			GETSECTION2
@section3[ \t]+[^'\n']+['\n']+			GETSECTION3
@section4[ \t]+[^'\n']+['\n']+			GETSECTION4
@image[ \t]*\{[ \t]*text[ \t]*:[ \t]*\"[^"\""]+\"[ \t]*ref[ \t]*:[ \t]*\"[^"\""]+\"[ \t]*\}		GETIMAGE
@figure[ \t]*\{[ \t]*text[ \t]*:[ \t]*\"[^"\""]+\"[ \t]*ref[ \t]*:[ \t]*\"[^"\""]+\"[ \t]*\}		GETIMAGE
@image[ \t]*\{[ \t]*text[ \t]*:[ \t]*\"[^"\""]+\"[ \t]*id[ \t]*:[ \t]*[a-zA-Z][a-zA-Z0-9]*[ \t]*\}	GETIMAGEID
@figure[ \t]*\{[ \t]*text[ \t]*:[ \t]*\"[^"\""]+\"[ \t]*id[ \t]*:[ \t]*[a-zA-Z][a-zA-Z0-9]*[ \t]*\}	GETIMAGEID
%%
int main()
{
	tree = new Dom;
	if(!(yyin = fopen(fileIn, "rt" )))
	{
		printf ("Не удалось открыть файл \"%s\"\n", fileIn);
		return 0;
	}
	if(!(yyout = fopen(fileOut, "wt" )))
	{
		printf ("Не удалось открыть файл \"%s\"\n", fileOut);
		return 0;
	}   
	yylex();
	tree->addText(yyout, fileOut); // записать в дерево произвольный текст после последней команды, или всё содержимое вх. файла, если ни одной команды не встретилось
	delete tree; // удалить дерево из памяти
	fclose(yyin);
	fclose(yyout);
	puts("Выполнено.");
	return 0;
}
