#include <string>
#include <cstdio>
#include <cstdlib>
#include "GenDoc.h"

bool GenDoc::genForm (const std::string &fName, const std::string &format/*pdf*/)
{
    // сначала открыть шаблон в LaTeX и вывести результат в файл result,
    // затем перевести result в требуемый формат (format)
    std::string result = fName+".lex"; // имя файла, куда предварительно записать документ в LaTeX
    std::string templ = "templateForm.tex"; // имя файла шаблона
    FILE *fres; // результат в LaTeX
    FILE *ftempl; // шаблон
    remove(result.c_str());


    if (!(ftempl = fopen(templ.c_str(), "rt")))
    {
        printf ("Отсутствует файл шаблона формуляра \"%s\"!\n", result.c_str());
		exit (1);
    }

    std::string templBuf(""); // буфер содержимого файла шаблона
    char ch;
    while(!(feof(ftempl))) // записать в буфер файл шаблона
    {
        ch=fgetc(ftempl);
        if (!feof(ftempl)) templBuf.push_back(ch);
    }
    fclose(ftempl);



    // TODO: вставить код преобразования буфера согласно DOM



    // запись результата из буфера в файл
    if (!(fres = fopen(result.c_str(), "wt"))) // результат в LaTeX
    {
        printf ("Не удалось создать временный файл \"%s\"!\n", result.c_str());
		exit (1);
    }
    fputs(templBuf.c_str(), fres);
    fclose (fres);

    if (format == "doc") // если пользователь указал только имя файла, doc по умолчанию
	{
		 // перевести в Doc
	}
	else
	{
		/*if (format == "lex") // перевести в LaTeX
		{
            // уже готово
		}
		else*/
			if (format == "pdf")
			{
			    // сформировать команду
                templBuf="pdflatex "+result;
				if (std::system(templBuf.c_str()))// перевести в Pdf
				{
				    puts("Ошибка приложения TexLive");
                    exit(1);
				}
				remove(result.c_str()); // удалить файл lex
                remove((fName+".aux").c_str()); // файл сформированный pdflatex
                remove((fName+".log").c_str()); // файл сформированный pdflatex
			}
			else
				{
					printf ("Формат \"%s\" не поддерживается программой.\n", format.c_str());
					exit(1);
				}
	}
    return true;
}

