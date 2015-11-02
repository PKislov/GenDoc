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

#define GETTITLEBEGIN {\
	gen->addText(yyout, fileOut);/*записать в дерево фрагмент текста перед командой*/\
	/*После выполнения каждой команды содержимое вых. файла затирается*/\
	gen->addTitleBegin();\
		 }
// конструкция @title ... @end title разбита на три части и заносится в DOM отдельно (начало команды, текст названия и окончание команды)
#define GETTITLEEND {\
	gen->addText(yyout, fileOut);\
	gen->addTitle();\
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
	gen->addImageRef(yytext, 1, 3);\
		 }

#define	GETIMAGE2 {\
	gen->addText(yyout, fileOut);\
	gen->addImageRef(yytext, 3, 1);\
		 }

#define	GETIMAGE3 {\
	gen->addText(yyout, fileOut);\
	gen->addImageRef(yytext);\
		 }
// @[T|t][I|i][T|t][L|l][E|e][ \t]+[^@]+@[E|e][N|n][D|d][ \t]+[T|t][I|i][T|t][L|l][E|e]		GETTITLE
%}
%%  
@[T|t][I|i][T|t][L|l][E|e][ \t\n]+				GETTITLEBEGIN
[ \t\n]+@[E|e][N|n][D|d][ \t]+[T|t][I|i][T|t][L|l][E|e]		GETTITLEEND
@[T|t][O|o][C|c][ \t]*['\n']*				GETTOC
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]1[ \t]+[^'\n']+['\n']+			GETSECTION1
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]2[ \t]+[^'\n']+['\n']+			GETSECTION2
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]3[ \t]+[^'\n']+['\n']+			GETSECTION3
@[S|s][E|e][C|c][T|t][I|i][O|o][N|n]4[ \t]+[^'\n']+['\n']+			GETSECTION4
@[I|i][M|m][A|a][G|g][E|e][ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]+\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]+\"[ \t]*\}		GETIMAGE
@[F|f][I|i][G|g][U|u][R|r][E|e][ \t]*\{[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]+\"[ \t]*;[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]+\"[ \t]*\}		GETIMAGE
@[I|i][M|m][A|a][G|g][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]+\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]+\"[ \t]*\}		GETIMAGE2
@[F|f][I|i][G|g][U|u][R|r][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]+\"[ \t]*;[ \t]*[T|t][E|e][X|x][T|t][ \t]*:[ \t]*\"[^"\""]+\"[ \t]*\}		GETIMAGE2
@[F|f][I|i][G|g][U|u][R|r][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]+\"[ \t]*\}		GETIMAGE3
@[I|i][M|m][A|a][G|g][E|e][ \t]*\{[ \t]*[R|r][E|e][F|f][ \t]*:[ \t]*\"[^"\""]+\"[ \t]*\}		GETIMAGE3
%%

// В командной строке пользователь должен указать имя исходного файла, тип документа и формат выходного файла (по умолчанию pdf).
// Форматы: odt, tex, pdf, html, xml (в формате html), tei (формат TEI XML), db (DocBook, в xml), dvi - требуется TexLive
// 	    через pandoc: doc (сначала в html через TexLive, затем в doc через pandoc), docx, markdown
// Тип документа: form - формуляр
// Пример команды: ./GenDoc data.txt form odt
int main(int argc, char *argv[])
{
//##################################################
//############## ОТЛАДКА ###########################
argc = 4; argv[1] = "dat.txt"; argv[2] = "form"; argv[3] = "pdf";
//##################################################
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
	stemp = std::string("./include ") + argv[1] + " " + fSrcIncl; 
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
	for (decltype(type.size()) i=0; i < type.size(); ++i) type[i] = tolower(type[i]); // перевести в нижний регистр

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
			for (decltype(format.size()) i=0; i < format.size(); ++i) format[i] = tolower(format[i]); // перевести в нижний регистр
		}
		else
			format = "pdf"; // формат по умолчанию

	if (type == "form") // формуляр
	{		
		if (gen->getToc() && format == "pdf") // если указана команда toc (содержание), то дважды запустить программу pdflatex, иначе содержание в pdf не отобразится если команда toc в начале файла
		{
			gen->genForm(result, true, format); // запуск генератора документа
			gen->genForm(result, false, format); // false - не запускать поиск синтакс. ошибок
			remove((result + ".toc").c_str()); // удалить временный файл .toc
		}
		else
			{
				gen->genForm(result, true, format);
			}

		// сформировать строку result - нужна теперь только для добавления к имени документа "Формуляр " и вывода сообщения пользователю в конце функции main
		if (format == "tex" || format == "odt" || format == "pdf" || format == "doc"  || format == "docx")
		{
			stemp = "Формуляр " + result + "." + format;
			if(rename((result + "." + format).c_str(), stemp.c_str()))
			{
				puts("Внутренняя ошибка приложения.");
				return 1;
			}
			result=stemp;
		}
		else // если формат html, xml и др., не добавлять к имени "Формуляр "
			{
				if (format == "xml")
					result += ".html";
				else
					if (format == "tei" || format == "db")
						result += ".xml";
					else
						if (format == "markdown")
							result += ".md";
						else
							result += ("." + format);
			}
		
	} // TODO: вставить вызовы генерации других типов документов
	else // if (type == "form")
		{
			// TODO: вставить в сообщение типы всех возможных генерируемых документов
			printf ("Тип документа \"%s\" не поддерживается программой.\nНапример: form - формуляр\n", type.c_str());
			return 1;
		}


	delete gen; // удалить генератор из памяти
	printf("Выполнено. Данные записаны в \"%s\"\n", result.c_str());
	return 0;
}
