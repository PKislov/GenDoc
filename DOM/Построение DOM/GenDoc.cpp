#include <string>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include "GenDoc.h"

bool GenDoc::genForm (const std::string &fName, const std::string &format) const
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


    makeBuff(root, templBuf); // преобразования буфера согласно DOM


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

void GenDoc::makeBuff (const decltype(root) p, std::string &buf) const
{
    if (!p || !p->children.size()) return; // если пустрое дерево
    for (int i=0; i<p->children.size(); ++i) // цикл прохода дерева
    {
        //puts(p->children[i]->id.c_str());
        if (p->children[i]->id == title)
        {
            writeTitle(p->children[i]->value[0], buf); // заполняет в шаблоне заголовок документа (title)
        }
        else
            if (p->children[i]->id == toc)
            {
                writeText("\\tableofcontents\n\\newpage\n", buf);
            }
            else
                if (p->children[i]->id == section1)
                {
                    writeSection1(p->children[i]->value[0], buf);
                }
                else
                    if (p->children[i]->id == section2)
                    {
                        writeSection2(p->children[i]->value[0], buf);
                    }
                    else
                        if (p->children[i]->id == section3)
                        {
                            writeSection3(p->children[i]->value[0], buf);
                        }
                        else
                            if (p->children[i]->id == section4)
                            {
                                writeSection4(p->children[i]->value[0], buf);
                            }
                            else
                                if (p->children[i]->id == text)
                                {
                                    writeTextLatex(p->children[i]->value[0], buf);
                                }
                                else
                                    if (p->children[i]->id == image && p->children[i]->value[2] == "ref") // рисунок, путь к файлу рисунка
                                    {
                                        writeImage(p->children[i], buf);
                                    }

		if (p->children[i]->children.size())
        {
            makeBuff (p->children[i], buf);
        }
    }
}

// заполняет в шаблоне (в строке buf) заголовок документа (title, в строке t)
void GenDoc::writeTitle (const std::string &t, std::string &buf) const
{
    /*static bool f=false;
    if (f)
    {
        printf ("Команда \"@title\" была вызвана ранее, значение \"%s\" не будет указано.\n", t.c_str());
        return;
    }
    f=true;*/
    const std::string comm="\\title{}"; // найти в buf вхождение этой команды, и вставить между скобок заголовок из t
    for (int i=0; i<buf.size()-comm.size()+1; i++)
    {
        if(!buf.compare(i, comm.size(), comm)) // нашли начало команды title
        {
            std::string stemp=t;
            buf.insert(i+comm.size()-1, headerToLaTex(stemp));
            return;
        }
    }
}
void GenDoc::writeSection1 (const std::string &t, std::string &buf) const
{
    std::string stemp=t;
    writeText(std::string("\\chapter {")+headerToLaTex(stemp)+"}\n", buf);
}
void GenDoc::writeSection2 (const std::string &t, std::string &buf) const
{
    std::string stemp=t;
    writeText(std::string("\\section {")+headerToLaTex(stemp)+"}\n", buf);
}
void GenDoc::writeSection3 (const std::string &t, std::string &buf) const
{
    std::string stemp=t;
    writeText(std::string("\\subsection {")+headerToLaTex(stemp)+"}\n", buf);
}
void GenDoc::writeSection4 (const std::string &t, std::string &buf) const
{
    std::string stemp=t;
    writeText(std::string("\\subsubsection {")+headerToLaTex(stemp)+"}\n", buf);
}

// метод возвращает индекс перед выражением "\end{document}" в строке buf, используется для определения места вставки текста
unsigned GenDoc::whereInsertText (const std::string &buf) const
{
    const std::string sEnd="\\end{document}";
    for (int i=buf.size()-sEnd.size(); i>=0; i--)
    {
        if(!buf.compare(i, sEnd.size(), sEnd)) // нашли начало команды sEnd
        {
            return i;
        }
    }
    printf("Некорректно построен шаблон документа: отсутствует утверждение \"%s\"!\n", sEnd.c_str());
    exit(1);
}

// записывает текст из t в buf по индексу, вычисленному методом whereInsertText
void GenDoc::writeText (const std::string &t, std::string &buf) const
{
    buf.insert(whereInsertText(buf), t);
}

void GenDoc::writeImage (const decltype(root) p, std::string &buf) const
{
    int i = 0, j = 0;
    while(i< p->value[3].size()) // поиск расширения в имени файла с рисунком
    {
        if (p->value[3][i] == '.') j = i;
        ++i;
    }
    std::string stemp("");
    if (j) // если имя файла содержит расширение
    {
        ++j;
        stemp.insert(0, p->value[3], j, p->value[3].size()-j); // вставить расширение имени файла в строку stemp
        if (stemp!="pdf"&&stemp!="png"&&stemp!="jpg"&&stemp!="jpeg") // если расширение не .pdf, .png, .jpg, .jpeg
        {
            printf("Формат рисунка \"%s\" не поддерживается программой!\n", p->value[3].c_str());
            exit(1);
        }
    }
    else
    {
        printf("Некорректное имя файла рисунка \"%s\"!\n", p->value[3].c_str());
        exit(1);
    }

    // проверка на существование имени файла рисунка
    FILE *fp;
    if(!(fp = fopen(p->value[3].c_str(), "rt" )))
	{
		printf ("Не удалось найти файл рисунка \"%s\"\n", p->value[3].c_str());
		exit(1);
	}
	fclose(fp);
	// сформировать код вставки рисунка на LaTex
    stemp = "\\begin{figure}[h]\n\\center{\\includegraphics{"+p->value[3]+"}}\n\\caption{"+p->value[1]+"}\n%\\label{fig:image} % ссылка\n\\end{figure}";
    writeText(stemp, buf);
}

// ищет в строке s служебные символы языка LaTex и заменяет их командами LaTex, позволяющими печатать эти символы, например все найденные символы # заменит на \# (применяется в методе writeText и др.)
const std::string& GenDoc::strToLaTex (std::string &s) const
{
    headerToLaTex(s);
    for(int i=0; i<s.size(); ++i)
    {
        if(s[i] == '\n')
        {
            s.insert(i, "\n");
            ++i;
        }
    }
    return s;
}

// делает то же свмое, что strToLaTex, только не дублирует переходы на новую строку
    const std::string& GenDoc::headerToLaTex (std::string &s) const
    {
        for(int i=0; i<s.size(); ++i)
    {
        switch(s[i])
        {
        case '#':
        case '$':
        case '%':
        case '&':
        case '{':
        case '}':
        case '_':
            s.insert(i, "\\");
            ++i;
            break;
        case '\\':
            s.erase(i,1);
            s.insert(i, "\\textbackslash ");
            i += std::strlen("\\textbackslash ")-1;
            break;
        case '~':
            s.erase(i,1);
            s.insert(i, "\\textasciitilde ");
            i += std::strlen("\\textasciitilde ")-1;
            break;
        case '^':
            s.erase(i,1);
            s.insert(i, "\\textasciicircum ");
            i += std::strlen("\\textasciicircum ")-1;
            break;
        case '"':
            s.erase(i,1);
            s.insert(i, "\\dq ");
            i += std::strlen("\\dq ")-1;
            break;
        }
    }
    return s;
    }

// записывает текст из t в buf по индексу, вычисленному методом whereInsertText, преобразует текст методом strToLaTex
    void GenDoc::writeTextLatex (const std::string &t, std::string &buf) const
    {
        std::string stemp = t;
        writeText(strToLaTex(stemp), buf);
    }
