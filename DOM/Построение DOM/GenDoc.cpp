#include <string>
#include <cstdio>
#include "GenDoc.h"

bool GenDoc::genForm (const std::string &fName, const std::string &format)
{
    // сначала открыть шаблон в LaTeX и вывести результат в файл result,
    // затем перевести result в требуемый формат (format)
    std::string result = fName+".tex"; // имя файла, куда предварительно записать документ в LaTeX
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


    if (!(fres = fopen(result.c_str(), "wt"))) // результат в LaTeX
    {
        printf ("Не удалось создать файл \"%s\"!\n", result.c_str());
		exit (1);
    }

    if (format == "doc") // если пользователь указал только имя файла, doc по умолчанию
	{
		 // перевести в Doc
	}
	else
	{
		if (format == "lex")
		{
			// перевести в LaTeX
		}
		else
			if (format == "pdf")
			{
				// перевести в Pdf
			}
			else
				{
					printf ("Формат \"%s\" не поддерживается программой.\n", format.c_str());
					return 1;
				}
	}
    return true;
}

