%{
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cctype>
#include "GenDoc.h"
//компилировать совместно с "domBuilder.cpp" и GenDoc.cpp

// основной объект генератора
static GenDoc *gen;

static const char *fileIn;
static const char * const fileOut = "temp.tmp"; // временный файл, нельзя получать библиотечной функцией, т.к. постоянно происходит открытие/закрытие файла

#define GETTITLE {\
	gen->addText(yyout, fileOut);/*записать в дерево фрагмент текста перед командой*/\
	/*После выполнения каждой команды содержимое вых. файла затирается*/\
	gen->addTitle(yytext);\
		 }

#define GETTOC	{\
	gen->addText(yyout, fileOut);\
	gen->addToc();\
		}

#define	GETSECTION1 {\
	gen->addText(yyout, fileOut);\
	gen->addSection1(yytext);\
		 }

#define	GETSECTION2 {\
	gen->addText(yyout, fileOut);\
	gen->addSection2(yytext);\
		 }

#define	GETSECTION3 {\
	gen->addText(yyout, fileOut);\
	gen->addSection3(yytext);\
		 }

#define	GETSECTION4 {\
	gen->addText(yyout, fileOut);\
	gen->addSection4(yytext);\
		 }

#define	GETIMAGE {\
	gen->addText(yyout, fileOut);\
	gen->addImageRef(yytext);\
		 }
/*
#define	GETIMAGEID {\
	gen->addText(yyout, fileOut);\
	gen->addImageId(yytext);\
		 }
*/
//@image[ \t]*\{[ \t]*text[ \t]*:[ \t]*\"[^"\""]+\"[ \t]*id[ \t]*:[ \t]*[a-zA-Z][a-zA-Z0-9]*[ \t]*\}	GETIMAGEID
//@figure[ \t]*\{[ \t]*text[ \t]*:[ \t]*\"[^"\""]+\"[ \t]*id[ \t]*:[ \t]*[a-zA-Z][a-zA-Z0-9]*[ \t]*\}	GETIMAGEID

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
%%

// В командной строке пользователь должен указать имя исходного файла, тип документа и формат выходного файла (по умолчанию doc).
// Форматы: doc, lex, pdf
// Тип документа: form - формуляр
// Пример команды: ./GenDoc data.txt form pdf
int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		puts("Не указано имя входного файла!");
		return 1;
	}
	if (argc < 3)
	{
		// TODO: вставить в сообщение типы всех возможных генерируемых документов
		puts("Не указан тип выходного документа!\nНапример: form - формуляр\n");
		return 1;
	}


	std::string stemp("./fileRing ");
	stemp += argv[1];
	if (std::system (stemp.c_str())) // выполнить проверку на корректность включения файлов
		return 1;


	const char * const fSrcIncl = "srcincl.tmp"; // временный файл для выполнения команд @include
	remove(fSrcIncl);
	stemp = std::string("./include ")+argv[1]+" "+fSrcIncl; 
	if (std::system (stemp.c_str())) // выполнить команды @include, результат запишется в файл fSrcIncl
		return 1;
	fileIn = fSrcIncl;


	if(!(yyin = fopen(fileIn, "rt" )))
	{
		printf ("Не удалось открыть файл \"%s\"\n", fileIn);
		return 1;
	}

	remove(fileOut);
	if(!(yyout = fopen(fileOut, "wt" ))) // ненужный выходной поток, генерируемый лексером, сбрасывать во временный файл, из которого считывать произвольный текст
	{
		printf ("Не удалось открыть временный файл \"%s\"\n", fileOut);
		return 1;
	}
	gen = new GenDoc(); 
	yylex(); // запуск лексера, заполнение дерева
	gen->addText(yyout, fileOut); // записать в дерево произвольный текст после последней команды, или всё содержимое вх. файла, если ни одной команды не встретилось

	fclose(yyin);
	fclose(yyout);
	remove(fileOut); // удалить временный файл
	remove(fSrcIncl); // удалить временный файл	
	fileIn = argv[1];


	std::string result(""); // имя генерируемого документа
	std::string format; // расширение генерируемого документа. Переменная нужна для преобразования строки формата в нижний регистр
	std::string type = argv[2]; // тип генерируемого документа (формуляр, ...)
	for (int i=0; i < type.size(); ++i) type[i] = tolower(type[i]); // перевести в нижний регистр

	// получение имени и расширения генерируемого документа
		int i = 0;
		const char *p = fileIn;
		while(*p) // поиск последнего вхождения '.' в имени файла
		{
			if (*p == '.') i = p-fileIn;
			++p;
		}
		if (i) // если имя вх. файла содержит расширение
			result.insert(0, fileIn, i);
		else 
			result += fileIn;

		if (argc > 3) // если пользователь указал формат вых. файла
		{
			format = argv[3];
			for (int i=0; i < format.size(); ++i) format[i] = tolower(format[i]); // перевести в нижний регистр
			result += ("." + format);
		}
		else
			{
				format = "doc";
				result += ".doc"; // формат по умолчанию
			}

	if (type == "form")
	{
		result = "Формуляр "+result;
		gen->genForm(result, format);
	} // TODO: вставить вызовы генерации других типов документов
	else
		{
			printf ("Тип документа \"%s\" не поддерживается программой.\nНапример: form - формуляр\n", type.c_str());
			return 1;
		}


	delete gen; // удалить генератор из памяти
	printf("Выполнено. Данные записаны в \"%s\"\n", result.c_str());
	return 0;
}
