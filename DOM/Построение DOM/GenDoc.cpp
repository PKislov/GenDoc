#include <string>
#include <cstdio>
#include <cstdlib>
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
////////////////////////////////////////////////////////////


    // TODO: вставить код преобразования буфера согласно DOM


////////////////////////////////////////////////////////////
    // запись результата из буфера в файл
    if (!(fres = fopen(result.c_str(), "wt"))) // результат в LaTeX
    {
        printf ("Не удалось создать временный файл \"%s\"!\n", result.c_str());
		exit (1);
    }
    fputs(templBuf.c_str(), fres);
    fclose (fres);


    if (format == "odt")
	{
		 // перевести в odt
        templBuf="mk4ht oolatex "+result; // сформировать команду
        if (std::system(templBuf.c_str()))
        {
            puts("Ошибка приложения TexLive!");
            exit(1);
        }
        remove(result.c_str()); // удалить файл tex
        // удаление служебных файлов
        remove((fName+".4ct").c_str());
        remove((fName+".4tc").c_str());
        remove((fName+".aux").c_str());
        remove((fName+".dvi").c_str());
        remove((fName+".idv").c_str());
        remove((fName+".lg").c_str());
        remove((fName+".log").c_str());
        remove((fName+".tmp").c_str());
        remove((fName+".xref").c_str());
	}
	else
	{
		if (format == "tex") // перевести в LaTeX
		{
            // уже готово
		}
		else
			if (format == "pdf")
			{
			    // сформировать команду
                templBuf="pdflatex "+result;
				if (std::system(templBuf.c_str()))// перевести в Pdf
				{
				    puts("Ошибка приложения TexLive!");
                    exit(1);
				}
				remove(result.c_str()); // удалить файл tex
                remove((fName+".aux").c_str()); // файл сформированный pdflatex
                remove((fName+".log").c_str()); // файл сформированный pdflatex
			}
			else
                if (format == "html")
                {
                    templBuf="mk4ht htlatex "+result;
                    if (std::system(templBuf.c_str()))
                    {
                        puts("Ошибка приложения TexLive!");
                        exit(1);
                    }
                    remove(result.c_str()); // удалить файл tex
                    remove((fName+".4ct").c_str());
                    remove((fName+".4tc").c_str());
                    remove((fName+".aux").c_str());
                    remove((fName+".dvi").c_str());
                    remove((fName+".idv").c_str());
                    remove((fName+".lg").c_str());
                    remove((fName+".log").c_str());
                    remove((fName+".tmp").c_str());
                    remove((fName+".xref").c_str());
                }
                else
                    if (format == "xml")
                    {
                        templBuf="mk4ht xhmlatex "+result;
                        if (std::system(templBuf.c_str()))
                        {
                            puts("Ошибка приложения TexLive!");
                            exit(1);
                        }
                        remove(result.c_str()); // удалить файл tex
                        remove((fName+".4ct").c_str());
                        remove((fName+".4tc").c_str());
                        remove((fName+".aux").c_str());
                        remove((fName+".dvi").c_str());
                        remove((fName+".idv").c_str());
                        remove((fName+".lg").c_str());
                        remove((fName+".log").c_str());
                        remove((fName+".tmp").c_str());
                        remove((fName+".xref").c_str());
                    }
                    else
                        if (format == "tei") // формат TEI XML
                        {
                            templBuf="mk4ht teilatex "+result;
                            if (std::system(templBuf.c_str()))
                            {
                                puts("Ошибка приложения TexLive!");
                                exit(1);
                            }
                            remove(result.c_str()); // удалить файл tex
                            remove((fName+".4ct").c_str());
                            remove((fName+".4tc").c_str());
                            remove((fName+".aux").c_str());
                            remove((fName+".dvi").c_str());
                            remove((fName+".idv").c_str());
                            remove((fName+".lg").c_str());
                            remove((fName+".log").c_str());
                            remove((fName+".tmp").c_str());
                            remove((fName+".xref").c_str());
                        }
                        else
                            if (format == "db") // формат DocBook
                            {
                                templBuf="mk4ht dblatex "+result;
                                if (std::system(templBuf.c_str()))
                                {
                                    puts("Ошибка приложения TexLive!");
                                    exit(1);
                                }
                                remove(result.c_str()); // удалить файл tex
                                remove((fName+".4ct").c_str());
                                remove((fName+".4tc").c_str());
                                remove((fName+".aux").c_str());
                                remove((fName+".dvi").c_str());
                                remove((fName+".idv").c_str());
                                remove((fName+".lg").c_str());
                                remove((fName+".log").c_str());
                                remove((fName+".tmp").c_str());
                                remove((fName+".xref").c_str());
                            }
                            else
                                if (format == "doc")
                                {
                                    // сначала в html
                                    templBuf="mk4ht htlatex "+result;
                                    if (std::system(templBuf.c_str()))
                                    {
                                        puts("Ошибка приложения TexLive!");
                                        exit(1);
                                    }
                                    remove(result.c_str()); // удалить файл tex
                                    remove((fName+".4ct").c_str());
                                    remove((fName+".4tc").c_str());
                                    remove((fName+".aux").c_str());
                                    remove((fName+".dvi").c_str());
                                    remove((fName+".idv").c_str());
                                    remove((fName+".lg").c_str());
                                    remove((fName+".log").c_str());
                                    remove((fName+".tmp").c_str());
                                    remove((fName+".xref").c_str());

                                    // теперь в doc
                                    templBuf = "pandoc -o "+fName+".doc "+fName+".html";
                                    if (std::system(templBuf.c_str()))
                                    {
                                        puts("Ошибка приложения pandoc!");
                                        exit(1);
                                    }
                                    remove((fName+".css").c_str());
                                    remove((fName+".html").c_str());

                                }
                                else
                                    if (format == "docx")
                                    {
                                        templBuf = "pandoc -o "+fName+".docx "+result;
                                        if (std::system(templBuf.c_str()))
                                        {
                                            puts("Ошибка приложения pandoc!");
                                            exit(1);
                                        }
                                        remove(result.c_str()); // удалить файл tex
                                    }
                                    else
                                        if (format == "markdown")
                                        {
                                            templBuf = "pandoc -o "+fName+".md "+result;
                                            if (std::system(templBuf.c_str()))
                                            {
                                                puts("Ошибка приложения pandoc!");
                                                exit(1);
                                            }
                                            remove(result.c_str()); // удалить файл tex
                                        }
                                        else
                                            {
                                                printf ("Формат \"%s\" не поддерживается программой.\n", format.c_str());
                                                exit(1);
                                            }
	}
    return true;
}

