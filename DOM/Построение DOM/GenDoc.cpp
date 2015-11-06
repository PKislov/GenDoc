#include <string>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <algorithm>
#include "GenDoc.h"

bool GenDoc::genForm (const std::string &fName, const bool findErrSyntax, const std::string &format)
{
    // сначала открыть шаблон в LaTeX и вывести результат в файл result,
    // затем перевести result в требуемый формат (format)

    std::string result = fName + ".tex"; // имя файла, куда предварительно записать документ в LaTeX
    std::string templ = "templateForm.tex"; // имя файла шаблона
    FILE *fres; // результат в LaTeX
    FILE *ftempl; // шаблон
    //remove(result.c_str());

    // поиск синтаксических ошибок в DOM
    if (findErrSyntax)
    {
        vInclFiles.clear(); // очистить вектор с именами и содержимым подкл. файлов
        vErrMess.clear(); // список мест ошибок
        // проверить доступ к файлу с именами подключаемых файлов
        bflist = (flist = fopen(listFileName, "rt" ));
        findSyntaxErr(root); // поиск ошибок
        if (bflist) fclose (flist);
        showErrMessage(); // отображение списка ошибок
        vInclFiles.clear();
        vErrMess.clear();
    }
    remove(listFileName);


    if (!(ftempl = fopen(templ.c_str(), "rt")))
    {
        printf ("Отсутствует файл шаблона формуляра \"%s\"!\n", result.c_str());
		exit (1);
    }

    std::string templBuf(""); // буфер содержимого файла шаблона
    char ch;
    while(!(feof(ftempl))) // записать в буфер файла шаблона
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
        templBuf = "mk4ht oolatex " + result; // сформировать команду
        if (std::system(templBuf.c_str()))
        {
            puts("Ошибка приложения TexLive!");
            exit(1);
        }
        remove(result.c_str()); // удалить файл tex
        // удаление служебных файлов
        remove((fName + ".4ct").c_str());
        remove((fName + ".4tc").c_str());
        remove((fName + ".aux").c_str());
        remove((fName + ".dvi").c_str());
        remove((fName + ".idv").c_str());
        remove((fName + ".lg").c_str());
        remove((fName + ".log").c_str());
        remove((fName + ".tmp").c_str());
        remove((fName + ".xref").c_str());
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
                templBuf = "pdflatex " + result;
				if (std::system(templBuf.c_str()))// перевести в Pdf
				{
				    puts("Ошибка приложения TexLive!");
                    exit(1);
				}
				// дважды запустить программу pdflatex, иначе оглавление и ссылки не отобразятся
				if (std::system(templBuf.c_str()))// перевести в Pdf
				{
				    puts("Ошибка приложения TexLive!");
                    exit(1);
				}
				remove(result.c_str()); // удалить файл tex
                remove((fName + ".aux").c_str()); // файл сформированный pdflatex
                remove((fName + ".log").c_str()); // файл сформированный pdflatex
			}
			else
                if (format == "html")
                {
                    templBuf = "mk4ht htlatex " + result;
                    if (std::system(templBuf.c_str()))
                    {
                        puts("Ошибка приложения TexLive!");
                        exit(1);
                    }
                    remove(result.c_str()); // удалить файл tex
                    remove((fName + ".4ct").c_str());
                    remove((fName + ".4tc").c_str());
                    remove((fName + ".aux").c_str());
                    remove((fName + ".dvi").c_str());
                    remove((fName + ".idv").c_str());
                    remove((fName + ".lg").c_str());
                    remove((fName + ".log").c_str());
                    remove((fName + ".tmp").c_str());
                    remove((fName + ".xref").c_str());
                }
                else
                    if (format == "xml")
                    {
                        templBuf = "mk4ht xhmlatex " + result;
                        if (std::system(templBuf.c_str()))
                        {
                            puts("Ошибка приложения TexLive!");
                            exit(1);
                        }
                        remove(result.c_str()); // удалить файл tex
                        remove((fName + ".4ct").c_str());
                        remove((fName + ".4tc").c_str());
                        remove((fName + ".aux").c_str());
                        remove((fName + ".dvi").c_str());
                        remove((fName + ".idv").c_str());
                        remove((fName + ".lg").c_str());
                        remove((fName + ".log").c_str());
                        remove((fName + ".tmp").c_str());
                        remove((fName + ".xref").c_str());
                    }
                    else
                        if (format == "tei") // формат TEI XML
                        {
                            templBuf = "mk4ht teilatex " + result;
                            if (std::system(templBuf.c_str()))
                            {
                                puts("Ошибка приложения TexLive!");
                                exit(1);
                            }
                            remove(result.c_str()); // удалить файл tex
                            remove((fName + ".4ct").c_str());
                            remove((fName + ".4tc").c_str());
                            remove((fName + ".aux").c_str());
                            remove((fName + ".dvi").c_str());
                            remove((fName + ".idv").c_str());
                            remove((fName + ".lg").c_str());
                            remove((fName + ".log").c_str());
                            remove((fName + ".tmp").c_str());
                            remove((fName + ".xref").c_str());
                        }
                        else
                            if (format == "db") // формат DocBook
                            {
                                templBuf = "mk4ht dblatex " + result;
                                if (std::system(templBuf.c_str()))
                                {
                                    puts("Ошибка приложения TexLive!");
                                    exit(1);
                                }
                                remove(result.c_str()); // удалить файл tex
                                remove((fName + ".4ct").c_str());
                                remove((fName + ".4tc").c_str());
                                remove((fName + ".aux").c_str());
                                remove((fName + ".dvi").c_str());
                                remove((fName + ".idv").c_str());
                                remove((fName + ".lg").c_str());
                                remove((fName + ".log").c_str());
                                remove((fName + ".tmp").c_str());
                                remove((fName + ".xref").c_str());
                            }
                            else
                                if (format == "doc")
                                {
                                    // сначала перевести в html
                                    templBuf = "mk4ht htlatex " + result;
                                    if (std::system(templBuf.c_str()))
                                    {
                                        puts("Ошибка приложения TexLive!");
                                        exit(1);
                                    }
                                    remove(result.c_str()); // удалить файл tex
                                    remove((fName + ".4ct").c_str());
                                    remove((fName + ".4tc").c_str());
                                    remove((fName + ".aux").c_str());
                                    remove((fName + ".dvi").c_str());
                                    remove((fName + ".idv").c_str());
                                    remove((fName + ".lg").c_str());
                                    remove((fName + ".log").c_str());
                                    remove((fName + ".tmp").c_str());
                                    remove((fName + ".xref").c_str());

                                    // теперь перевести в doc
                                    templBuf = "pandoc -o " + fName + ".doc " + fName + ".html";
                                    if (std::system(templBuf.c_str()))
                                    {
                                        puts("Ошибка приложения pandoc!");
                                        exit(1);
                                    }
                                    remove((fName + ".css").c_str());
                                    remove((fName + ".html").c_str());

                                }
                                else
                                    if (format == "docx")
                                    {
                                        templBuf = "pandoc -o " + fName + ".docx " + result;
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
                                            templBuf = "pandoc -o " + fName + ".md " + result;
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

void GenDoc::makeBuff (const decltype(root) p, std::string &buf)
{
    if (!p || !p->children.size()) return; // если пустрое дерево

    // массив указателей на методы writeSection
    void (GenDoc::*pfaddSec[])(const std::string &, std::string &, const std::string &,  const bool) const = {&GenDoc::writeSection1, &GenDoc::writeSection2, &GenDoc::writeSection3, &GenDoc::writeSection4};

    for (decltype(p->children.size()) i=0; i < p->children.size(); ++i) // цикл прохода дерева
    {
        if (isSectionBegin(p->children[i]))
        {
            sectionBegToSection(p->children[i]->id); // меняем id на section
        }

        //puts(p->children[i]->id.c_str());
        if (p->children[i]->id == title)
        {
            writeTitle(p->children[i]->value[0], buf); // заполняет в шаблоне заголовок документа (title)
        }
        else // начало команды @title
            if(p->children[i]->id == titleBegin)
            {
                puts ("Отсутствует окончание команды \"@title ... @end title\"!\n");
                exit(1);
            }
            else
                if (p->children[i]->id == toc)
                {
                    writeText("\\tableofcontents\n\\newpage\n", buf);
                }
                else
                    if (isSection(p->children[i]))
                    {
                        (this->*(pfaddSec[p->children[i]->id[p->children[i]->id.size()-1]-'0'-1]))(p->children[i]->value[1], buf, p->children[i]->value[3], p->children[i]->value[2] == "id");
                    }
                    else
                        if (p->children[i]->id == text)
                        {
                            writeTextLatex(p->children[i]->value[0], buf);
                        }
                        else
                            if (p->children[i]->id == image) // рисунок
                            {
                                writeImage(p->children[i], buf);
                            }
                            else
                                if (p->children[i]->id == idRef) // ссылкa - команда &... {id:"..."}
                                {
                                    writeId(p->children[i], buf);
                                }
                                else
                                    if (p->children[i]->id == pageid)
                                    {
                                        writePageId(p->children[i], buf);
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
    const std::string comm = "\\title{}"; // найти в buf вхождение этой команды, и вставить между скобок заголовок из t
    for (decltype(buf.size()) i=0; i < buf.size() - comm.size() + 1; i++)
    {
        if(!buf.compare(i, comm.size(), comm)) // нашли начало команды title
        {
            std::string stemp = t;
            buf.insert(i + comm.size() - 1, headerToLaTex(stemp));
            return;
        }
    }
}
void GenDoc::writeSection1 (const std::string &t, std::string &buf, const std::string &id, const bool f) const
{
    std::string stemp = t, label = id; // копии создать чтоб не изменять исходные данные методом headerToLaTex
    writeText(std::string("\\chapter {") + headerToLaTex(stemp) + "}" + (f ? "\\label{" + headerToLaTex(label) + "}" : "") + "\n", buf);
}
void GenDoc::writeSection2 (const std::string &t, std::string &buf, const std::string &id, const bool f) const
{
    std::string stemp = t, label = id;
    writeText(std::string("\\section {") + headerToLaTex(stemp) + "}" + (f ? "\\label{" + headerToLaTex(label) + "}" : "") + "\n", buf);
}
void GenDoc::writeSection3 (const std::string &t, std::string &buf, const std::string &id, const bool f) const
{
    std::string stemp = t, label = id;
    writeText(std::string("\\subsection {") + headerToLaTex(stemp) + "}" + (f ? "\\label{" + headerToLaTex(label) + "}" : "") + "\n", buf);
}
void GenDoc::writeSection4 (const std::string &t, std::string &buf, const std::string &id, const bool f) const
{
    std::string stemp = t, label = id;
    writeText(std::string("\\subsubsection {") + headerToLaTex(stemp) + "}" + (f ? "\\label{" + headerToLaTex(label) + "}" : "") + "\n", buf);
}

// метод возвращает индекс перед выражением "\end{document}" в строке buf, используется для определения места вставки текста
std::string::size_type GenDoc::whereInsertText (const std::string &buf) const
{
    const std::string sEnd = "\\end{document}";
    for (decltype(buf.size()) i = buf.size() - sEnd.size(); i >= 0; i--)
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
    decltype(p->value[3].size()) i = 0, j = 0;
    while(i < p->value[3].size()) // поиск расширения в имени файла с рисунком
    {
        if (p->value[3][i] == '.') j = i;
        ++i;
    }
    std::string stemp("");
    if (j) // если имя файла содержит расширение
    {
        ++j;
        stemp.insert(0, p->value[3], j, p->value[3].size()-j); // вставить расширение имени файла в строку stemp
        if (stemp != "pdf" && stemp != "png" && stemp != "jpg" && stemp != "jpeg") // если расширение не .pdf, .png, .jpg, .jpeg
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
	std::string scaption; // подпись к рисунку
	// если параметр "text" в векторе value не указан (пустая строка), то подпись
	// к рисунку не делается и рисунок в документе не нумеруется
	if (p->value[0].size())
	{
	    scaption = p->value[1];
	    scaption = "\\caption{" + headerToLaTex(scaption) + "}\n";
	}

	std::string label; // ссылка к рисунку
	if (p->value.size() >= 8) // если указана ссылка
	{
        label = p->value[5];
        label = "\\label{" + headerToLaTex(label) + "}";
	}

    stemp = "\\begin{figure}[h]\n\\center{\\includegraphics{" + p->value[3] + "}}\n" + scaption + label + "\n\\end{figure}";
    writeText(stemp, buf);
}

// ищет в строке s служебные символы языка LaTex и заменяет их командами LaTex, позволяющими печатать эти символы, например все найденные символы # заменит на \# (применяется в методе writeText и др.)
const std::string& GenDoc::strToLaTex (std::string &s) const
{
    headerToLaTex(s);
    for(decltype(s.size()) i=0; i < s.size(); ++i)
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
        for(decltype(s.size()) i=0; i < s.size(); ++i)
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
                    i += std::strlen("\\textbackslash ") - 1;
                    break;
                case '~':
                    s.erase(i,1);
                    s.insert(i, "\\textasciitilde ");
                    i += std::strlen("\\textasciitilde ") - 1;
                    break;
                case '^':
                    s.erase(i,1);
                    s.insert(i, "\\textasciicircum ");
                    i += std::strlen("\\textasciicircum ") - 1;
                    break;
                case '"':
                    s.erase(i,1);
                    s.insert(i, "\\dq ");
                    i += std::strlen("\\dq ") - 1;
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

    void GenDoc::findSyntaxErr(const decltype(root) p)
    {
        if (!p || !p->children.size()) return; // если пустрое дерево

        std::string stempLowReg; // команда в нижнем регистре

        for (decltype(p->children.size()) i=0; i < p->children.size(); ++i) // цикл прохода дерева
        {
            if(p->children[i]->id == titleBegin) // найти место ошибки
            {
                puts ("Отсутствует окончание команды \"@title ... @end title\"!");
            }
            // искать ошибки в тексте
            if (p->children[i]->id == text || p->children[i]->id == titleBegin)
            {
                const std::string &stemp = p->children[i]->value[0]; // записать содержимое текста
                for (decltype(stemp.size()) i1=0; i1 < stemp.size(); ++i1) // поиск подстрок в тексте
                {
                    // проход вектора с командами, ищем вхождения команд в тексте (stemp)
                    for (decltype(vcommands.size()) i2=0; i2 < vcommands.size(); ++i2)
                    {
                        stempLowReg = ""; // перевести команду в нижний регистр
                        for(decltype(stemp.size()) i3=0; i3 < vcommands[i2].size(); ++i3)
                            stempLowReg.push_back(std::tolower(stemp[i1+i3]));

                        // подстрока найдена, значит есть синтакс. ошибка
                        if (stempLowReg == vcommands[i2])
                        {
                             //индекс конца некорр. команды в stemp
                            decltype(stemp.size()) endc;
                            //длина некорр. команды
                            decltype(stemp.size()) lengc;
                            // найти индекс конца некорр. команды в DOM
                            endc = i1 + vcommands[i2].size();
                            // некорр. команда должна оканчиваться '\n' и быть не длиннее 150 знаков
                            for (int i3=0; i3 < 150 && stemp[endc] != '\n' && endc < stemp.size(); ++i3, ++endc);
                            lengc = endc - i1 /*+ 1*/; // найти длину некорр. команды

                            // вывод сообщения о возможной синтакс. ошибке
                            if(!bflist) // если нет доступа к файлу со списком подключаемых файлов
                            {
                                // проход списка ошибок
                                for (decltype(vErrMess.size()) i3=0; i3 < vErrMess.size(); ++i3)
                                {
                                    // если в векторе сообщений найден дубликат сообщения об ошибке
                                    if (!vErrMess[i3].commande.compare(0, lengc, stemp, i1, lengc) && vErrMess[i3].fname == "" && vErrMess[i3].title == (isSection(p) ? p->value[1] : "") && !vErrMess[i3].line)
                                        goto met2; // не записывать ошибку в список
                                }
                                // в список сообщений ошибок записать данные
                                vErrMess.push_back({"", "", (isSection(p) ? p->value[1] : ""), 0});
                                // в массив сообщений ошибок скопировать текст некорректной команды
                                vErrMess[vErrMess.size()-1].commande.insert(0, stemp, i1, lengc);
                                // в сообщении имя файла и номер строки обнулены
                                met2:;
                            }
                            else // найти некорректную команду в главном и подключаемых файлах
                                {
                                    FILE *ftemp;
                                    char ch;
                                    //номер строки в файле, где содержится ошибка
                                    decltype(vInclFiles.size()) countLine;
                                    // при нахождении первой ошибки заполнить вектор подключаемых файлов
                                    if (vInclFiles.empty())
                                    {
                                        // записать список имен файлов из файла "list.tmp"
                                        for(decltype(vInclFiles.size()) i3=0; !(feof(flist)); ++i3)
                                        {
                                            vInclFiles.push_back({"", ""});
                                            while(!feof(flist))
                                            {
                                                ch = fgetc(flist);
                                                if (ch == '\n') break; // имена оканчиваются новой строкой
                                                if (!feof(flist))
                                                    vInclFiles[i3].name.push_back(ch);
                                            }
                                        }
                                        // удалить последний элемент - переход на новую строку
                                        vInclFiles.pop_back();
                                        // теперь записать в вектор содержимое каждого подключаемого файла
                                        for(decltype(vInclFiles.size()) i3=0; i3 < vInclFiles.size(); ++i3)
                                        {
                                            if (!(ftemp = fopen(vInclFiles[i3].name.c_str(), "rt")))
                                                continue;
                                            while(!(feof(ftemp)))
                                            {
                                                ch = fgetc(ftemp);
                                                if (!feof(ftemp))
                                                	vInclFiles[i3].text.push_back(ch);
                                            }
                                            fclose(ftemp);
                                            // в содержимом файла заменить последовательности символов \" на " т.к. в тексте в DOM уже заменено
                                            if (vInclFiles[i3].text.size() >= 2)
                                            {
                                                // проход содержимого файла
                                                for(decltype(vInclFiles[i3].text.size()) i4=0; i4 < vInclFiles[i3].text.size()-1; ++i4)
                                                {
                                                    if(vInclFiles[i3].text[i4] == '\\' && vInclFiles[i3].text[i4+1] == '\"')
                                                        vInclFiles[i3].text.erase(i4, 1); // удалить "\"
                                                }
                                            }
                                        }

                                    } // if (vInclFiles.empty())

                                    // найти, в каком файле, в какой строке содержится ошибочная команда
                                    for(decltype(vInclFiles.size()) i4=0; i4 < vInclFiles.size(); ++i4)
                                    {
                                        if (vInclFiles[i4].text.size()) // поиск подстроки
                                            {
                                                // если содержимое файла длиннее либо равно длине фрагмента команды, то искать вхождение некорр. команды
                                                if (vInclFiles[i4].text.size() >= lengc)
                                                {
                                                    // проход содержимого файла
                                                    for (decltype(lengc) i3=0; i3 < vInclFiles[i4].text.size() - lengc + 1; ++i3)
                                                    {
                                                        // если нашли вхождение некорр. команды в файле
                                                        if (!vInclFiles[i4].text.compare(i3, lengc, stemp, i1, lengc))
                                                        {
                                                            countLine = 1; // счетчик строк
                                                            for (decltype(lengc) i5=0; i5 <= i3; ++i5) // сколько символов '\n' перед началом команды
                                                                if (vInclFiles[i4].text[i5] == '\n')
                                                                    ++countLine;
                                                            // проход списка ошибок
                                                            for (decltype(vErrMess.size()) i5=0; i5 < vErrMess.size(); ++i5)
                                                            {
                                                                // если в векторе сообщений найден дубликат сообщения об ошибке
                                                                if (!vErrMess[i5].commande.compare(0, lengc, stemp, i1, lengc) && vErrMess[i5].fname == vInclFiles[i4].name && /*vErrMess[i5].title == (isSection(p) ? p->value[1] : "") &&*/ vErrMess[i5].line == countLine)
                                                                    goto met1; // не записывать ошибку в список
                                                            }
                                                            // в список сообщений ошибок записать данные, заносим заголовок только если ошибка в корневом файле
                                                            vErrMess.push_back({"", vInclFiles[i4].name, isSection(p) && !i4 ? p->value[1] : "", countLine});
                                                            // в массив сообщений ошибок скопировать текст некорректной команды
                                                            vErrMess[vErrMess.size()-1].commande.insert(0, stemp, i1, lengc);
                                                            met1:;
                                                        }
                                                    }
                                                }
                                            }
                                    }
                                }
                        }
                    }
                }
            }
            if (p->children[i]->children.size())
            {
                findSyntaxErr(p->children[i]);
            }
        }
    }

// отображение на экране списка синтакс. ошибок из вектора vErrMess
void GenDoc::showErrMessage()
{
    if (vErrMess.size()) // если есть ошибки
    {
        // сначала провести сортировку списка ошибок

        // функтор для сравнения по имени файла
        struct
        {
            inline bool operator ()(const struct errMessage &v1, const struct errMessage &v2)
            {
                return v1.fname < v2.fname;
            }
        } s1;
        // функтор для сравнения по номеру строки
        struct
        {
            inline bool operator ()(const struct errMessage &v1, const struct errMessage &v2)
            {
                return v1.line < v2.line;
            }
        } s2;
        std::sort(vErrMess.begin(), vErrMess.end(), s1);

        // сортировка по номерам строк в каждом имени файла
        for (decltype(vErrMess.size())  i=0; i < vErrMess.size(); ++i)
        {
            decltype(i) i1 = i + 1;
            // найти индекс последнего нахождения имени файла
            for (;  i1 < vErrMess.size(); ++i1)
                if (vErrMess[i1].fname != vErrMess[i].fname)
                    break;
            --i1;
            std::sort(vErrMess.begin()+i, vErrMess.begin()+i1, s2);
            i = i1; // перейти к следующему имени файла
        }

        // вывод сообщений об ошибках
        printf("\n\nВозможные синтаксическиие ошибки:\n");

        for (decltype(vErrMess.size()) i=0; i < vErrMess.size(); ++i) // проход сообщений
        {
            // если есть заголовок, вывести имя заголовка, где находится ошибка
            if (vErrMess[i].title.size())
            {
                printf("\n\t(под заголовком \"%s\")\n", vErrMess[i].title.c_str());
            }
            // вывод некорр. команды
            printf("%s", vErrMess[i].commande.c_str());
            // вывод имени файла и номера строки, где содержится некорр. команда
            if (vErrMess[i].fname.size() && vErrMess[i].line)
                printf("\n\t(файл \"%s\", строка %ld)\n\n", vErrMess[i].fname.c_str(), vErrMess[i].line);
        }
        printf ("\nВсего %ld ошибок(ки)(ка)\nНажмите <Ввод> для продолжения генерации документа...", vErrMess.size());
        fflush(stdin);
        getchar();
    }
}

// заполняет в шаблоне (в строке buf) ссылку - команда &... {id:"..."}
void GenDoc::writeId (const decltype(root) p, std::string &buf) const
{
    const struct node *pfind = findIdInDom(root, p->value[1]); // поиск ссылки в DOM

    if (!pfind)
    {
        printf ("Необъявленная ссылка \"%s\"  в выражении \"%s\"!\n", p->value[1].c_str(), p->value[3].c_str());
        printf("Игнорировать [Y/N] ?");
        if (!getAnswer()) // ответ N
        {
            puts("\nГенерация документа прервана.");
            exit(1);
        }
    }
    else
        if(pfind->id != p->value[5]) // если типы ссылок не совпадают
        {
            // индекс выражения yytext в найденном узле (зависит от типа узла)
            auto indexYYtextRes = getIndexYytext(pfind);
            printf ("Ссылка \"%s\"  в выражении \"%s\" по типу не соответствует объявленной в выражении \"%s\"!\n", p->value[1].c_str(), p->value[3].c_str(), pfind->value[indexYYtextRes].c_str());
            exit(1);
        }
        std::string stemp = p->value[1];
        headerToLaTex(stemp);
        // команда на номер странцы - "\pageref"{...}, на остальные ресурсы - "\ref"{...}
        writeText("\\" + std::string(p->value[5] == "pageid" ? "page" : "") + "ref{" + stemp + "}", buf);
}

// возвращает ответ на вопрос вида Y/N
bool GenDoc::getAnswer() const
{
    char ch;
    met1: fflush(stdin);
    ch = getchar();
    switch(std::tolower(ch))
    {
    case 'y':
        return true;
        break;
    case 'n':
        return false;
        break;
    default:
        goto met1;
        break;
    }
}

// создать ссылку на страницу - команда @pageid	{id:" ... "}
void GenDoc::writePageId(const decltype(root) p, std::string &buf)
{
    std::string stemp = p->value[1];
    headerToLaTex(stemp);
    writeText("\\label{" + stemp + "}", buf);
}
