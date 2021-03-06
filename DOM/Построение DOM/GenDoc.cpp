#include <string>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <algorithm>
#include "GenDoc.h"

// метод генерации документа, в параметрах - имя вых. файла без расширения, флаг findErrSyntax - запускать ли метод поиска синтаксических ошибок в дереве, формат вых. данных (pdf, odt, tex и т.д.).
// Возвращает истину, если генерация прошла успешно
// typeDoc - тип документа (формуляр, произв. текст)
bool GenDoc::genDocument (const std::string &fName, const bool findErrSyntax, const std::string &typeDoc, const std::string &format)
{
    // сначала открыть шаблон в LaTeX и вывести результат в файл result,
    // затем перевести result в требуемый формат (format)

    type = typeDoc;

    std::string result = fName + ".tex"; // имя файла, куда предварительно записать документ в LaTeX
    std::string templ;
    if (type == "form")
    {
        templ = "templateForm.tex"; // имя файла шаблона
    }
    else
    {
        templ = "templateText.tex";
    }
    FILE *fres; // результат в LaTeX
    FILE *ftempl; // шаблон

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
        printf ("Отсутствует файл шаблона документа \"%s\"!\n", templ.c_str());
		exit (1);
    }

    std::string templBuf; // буфер содержимого файла шаблона
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
                remove((fName + ".out").c_str());
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
                                    else
                                        if (p->children[i]->id == code)
                                        {
                                           writeCode(p->children[i], buf);
                                        }
                                        else
                                            if (p->children[i]->id == enumeration)
                                            {
                                               writeEnum(p->children[i], buf);
                                            }
                                            else
                                                if (p->children[i]->id == table)
                                                {
                                                   writeTable(p->children[i], buf);
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
    for (decltype(buf.size()) i = buf.size() - sEnd.size();; i--)
    {
        if(!buf.compare(i, sEnd.size(), sEnd)) // нашли начало команды sEnd
        {
            return i;
        }
        if (!i) break;
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
    std::string stemp;
    if (getExtFname(p->value[3], stemp).size()) // если имя файла содержит расширение
    {
        if (stemp != "pdf" && stemp != "png" && stemp != "jpg" && stemp != "jpeg")
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
    if(!existFile(p->value[3]))
    {
        printf ("Не удалось найти файл рисунка \"%s\"\n", p->value[3].c_str());
        exit(1);
    }
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

    stemp = "\\begin{figure}[ht]\n\\center{\\includegraphics{" + p->value[3] + "}}\n" + scaption + label + "\n\\end{figure}";
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
            else
                if (p->children[i]->id == codeBegin)
                {
                    printf ("Отсутствует окончание команды \"@code{%s} ... @end code{%s}\"!\n", p->children[i]->value[1].c_str(), p->children[i]->value[1].c_str());
                }
                else
                    if (p->children[i]->id == enumerationBegin)
                    {
                        std::string stemp(p->children[i]->value[0]);
                        // вывести команду без '\n' на конце
                        printf ("Отсутствует окончание команды \"%s ... @end enumerate\"!\n", delSymbsInEndStr(stemp).c_str());
                    }
                    else
                        if (p->children[i]->id == tableBegin)
                        {
                            std::string stemp(p->children[i]->value[0]);
                            // вывести команду без '\n' на конце
                            printf ("Отсутствует окончание команды \"%s ... @end table\"!\n", delSymbsInEndStr(stemp).c_str());
                        }
            // искать ошибки в тексте
            if (p->children[i]->id == text || p->children[i]->id == titleBegin ||\
                p->children[i]->id == codeBegin || p->children[i]->id == enumerationBegin ||\
                p->children[i]->id == tableBegin)
            {
                const std::string &stemp = p->children[i]->value[0]; // записать содержимое текста
                for (decltype(stemp.size()) i1=0; i1 < stemp.size(); ++i1) // поиск подстрок в тексте
                {
                    // если перед командой стоит знак '\\'
                    if (i1 >= symbCancel.size() && !stemp.compare(i1-symbCancel.size(), symbCancel.size(), symbCancel))
                    {
                        continue;
                    }
                    else
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
                            lengc = endc - i1; // найти длину некорр. команды

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
                                                            // если перед командой стоит знак '\\', не считать ошибку
                                                            if (i3 >= symbCancel.size() && !vInclFiles[i4].text.compare(i3-symbCancel.size(), symbCancel.size(), symbCancel))
                                                            {
                                                                continue;
                                                            }

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

        // исправление недостатка алгоритма поиска ошибки - если например в тексте
        // кода есть одновременно идентичные команды, но одна из них отключена знаком '\\'
        // например @include {odt;ref:"proba.odt"} и \\@include {odt;ref:"proba.odt"},
        // то здесь программа находила ошибку. Ниже удаляются такие ошибки из вектора ошибок.
        for (decltype(vErrMess.size()) i=0; i < vErrMess.size(); ++i) // проход сообщений
        {
            // найти, в каком файле, в какой строке содержится ошибочная команда
            for(decltype(vInclFiles.size()) i4=0; i4 < vInclFiles.size(); ++i4)
            {
                if (vInclFiles[i4].text.size()) // поиск подстроки
                {
                    // если содержимое файла длиннее либо равно длине фрагмента команды, то искать вхождение некорр. команды
                    if (vInclFiles[i4].text.size() >= vErrMess[i].commande.size())
                    {
                        // проход содержимого файла
                        for (decltype(vInclFiles[i4].text.size()) i3=0; i3 < vInclFiles[i4].text.size() - vErrMess[i].commande.size() + 1; ++i3)
                        {
                            // если нашли вхождение некорр. команды в файле
                            if (!vInclFiles[i4].text.compare(i3, vErrMess[i].commande.size(), vErrMess[i].commande, 0, vErrMess[i].commande.size()))
                            {
                                // если перед командой стоит знак '\\', не считать ошибку
                                if (i3 >= symbCancel.size() && !vInclFiles[i4].text.compare(i3-symbCancel.size(), symbCancel.size(), symbCancel))
                                {
                                    // удалить ошибку
                                    vErrMess.erase(vErrMess.begin()+i);
                                    --i;
                                    goto met1;
                                }
                            }
                        }
                    }
                }
            }
        met1:;
        }


        // провести сортировку списка ошибок

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
        printf("Игнорировать [Y/N] ? ");
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

// создать ссылку на страницу - команда @pageid	{id:" ... "}
void GenDoc::writePageId(const decltype(root) p, std::string &buf)
{
    std::string stemp = p->value[1];
    headerToLaTex(stemp);
    writeText("\\label{" + stemp + "}", buf);
}

// вставить код (LaTeX или другой) - команда @code {}
void GenDoc::writeCode(const decltype(root) p, std::string &buf)
{
    if (p->value[1] == "latex") // LaTeX-вставка
    {
        writeText(p->value[0], buf);
    }
    else
        if (p->value[1] == "") // оформить листинг программы без нумерации строк
        {
            writeText("\\begin{verbatim}" + p->value[0] + "\\end{verbatim}", buf);
        }
        else
            if (p->value[1] == "listing") // оформить листинг программы c нумерацией строк
            {
                writeText("\\begin{listing}{1}" + p->value[0] + "\\end{listing}", buf);
            }
}

// записывает в строку s начало описания перечисления в виде @enumerate {} ...
// метод используется в writeEnum для вывода на экран ошибок в перечислении
std::string& GenDoc::infoEnumInStr (const decltype(root) p, std::string &s) const
{
    s.clear();
    decltype (p->value[6].size()) i1; // индекс кода в векторе value
    if (p->id == enumeration)
    {
        i1 = 6;
    }
    else
        if (p->id == table)
        {
            i1 = 3;
        }
        else
            {
                return s;
            }
    s = p->value[0];
    delSymbsInEndStr(s);
    for (decltype(s.size()) i=0, j=0; i < p->value[i1].size(); ++i, ++j)
    {
        if (p->value[i1][i] != '\n' && p->value[i1][i] != 10)
            s.push_back(p->value[i1][i]);
        else
            --j;
        if (j == 15) break;
    }
    delSymbsInEndStr(s);
    s += "...";
    return s;
}

// вставить перечисление (команда @enumerate)
void GenDoc::writeEnum (const decltype(root) p, std::string &buf)
{
    std::string stemp;
    // если в строке перечисления нет вхождений из строки p->value[4] - символа начала пунктов, по умолчанию *
    if (p->value[6].find(p->value[4], 0) == std::string::npos)
    {
        // кратко отобразить на экране конструкцию перечисления
        printf("В перечислении \"%s\": отсутствуют символы \"%s\" для обозначения начала пунктов!\n", infoEnumInStr(p, stemp).c_str(), p->value[4].c_str());
        exit(1);
    }


    // TODO: вставить код выполнения вложенных в текст перечисления команд.
    // команды переводятся в LaTeX и их начало и конец обозначается
    // "\\a", остальной текст не трогается и в LaTeX не переводится


    std::vector<std::string> vsymb; // вектор символов обозначающих начала пунктов (*, **, ...)
    vsymb.push_back(p->value[4]); // *
    vsymb.push_back(vsymb.back() + p->value[4]); // **
    vsymb.push_back(vsymb.back() + p->value[4]); // ***
    vsymb.push_back(vsymb.back() + p->value[4]); // ****

    struct sItem // структура пункта
    {
        std::string level;  // уровень пункта (*, **, ...)
        std::string item;   // текст пункта
    };

    std::vector<struct sItem> vitem; // вектор пунктов перечисления

    decltype(p->value[6].size()) i = 0;
    const std::string &stext = p->value[6]; // ссылка на текст перечисления
    // найти начало первого пункта
    while(i < stext.size())
    {
        if (!std::isspace(stext[i])) break;
        ++i;
    }
    if (i == stext.size()) return; // если пустой текст перечисления
    // TODO: вставить код пропуска \\a...\\a
    decltype(i) i1;
    // проверить какой символ перед первым пунктом (*, **, ...)
    for (i1 = vsymb.size()-1;; --i1) // перебор вектора символов, начиная с ****
    {
        if (!stext.compare(i, vsymb[i1].size(), vsymb[i1]))
        {
            if (i1) // если первый пункт начинается не с *, а с ** *** ...
            {
                printf ("В перечислении \"%s\" первый пункт должен начинаться с \"%s\"!\n", infoEnumInStr(p, stemp).c_str(), vsymb[0].c_str());
                exit(1);
            }
            goto met1; // успешно, начинается с *
        }
        if (!i1) break;
    }

    printf ("В перечислении \"%s\" отсутствует символ \"%s\" перед первым пунктом!\n", infoEnumInStr(p, stemp).c_str(), vsymb[0].c_str());
    exit(1);

    met1: i += vsymb[0].size(); // перейти к началу текста первого пункта
    vitem.push_back({vsymb[0], ""}); // занести первый пункт
    i1 = i;
    while (i1 < stext.size()) // найти индекс окончания текста первого пункта
    {
        for (decltype(i) j = vsymb.size()-1;; --j)
        {
            // нашли символ начала нового пункта
            if (!stext.compare(i1, vsymb[j].size(), vsymb[j]))
            {
                goto met4;
            }
            if (!j) break;
        }
        ++i1;
    }
    // занести в вектор пунктов текст первого пункта
    met4: vitem.back().item.insert(0, stext, i, i1-i);

    // цикл поиска пунктов и заполнения вектора vitem
    for (i=i1; i < stext.size(); ++i)
    {
        vitem.push_back({"", ""});
        // перебор вектора символов
        for (i1 = vsymb.size()-1;; --i1)
        {
            if (!stext.compare(i, vsymb[i1].size(), vsymb[i1])) // нашли символ
            {
                // записать в вектор символ
                vitem.back().level.insert(0, stext, i, vsymb[i1].size());
                break;
            }
            if (!i1) break;
        }
        // пропустить символ начала пункта
        i += vitem.back().level.size();
        if (i == stext.size()) break; // если дошли до конца текста
        i1 = i;
        while (i1 < stext.size()) // найти окончание текста пункта
        {
            for (decltype(i) j = vsymb.size()-1;; --j)
            {
                if (!stext.compare(i1, vsymb[j].size(), vsymb[j])) // нашли символ
                {
                    goto met3;
                }
                if (!j) break;
            }
            ++i1;
        }
        met3: vitem.back().item.insert(0, stext, i, i1-i); // занести текст пункта
        i = i1 - 1;
    }

    // удалить из вектора пунктов те пункты, где символ начала пункта начинается с "\\"
    // и прибавить содержимое удаляемых пунктов к предыдущим
    for (decltype(vitem.size()) i1 = 1; i1 < vitem.size(); ++i1)
    {
        // если текст ghtlsleotuj пункта длиннее либо равен длине символа отмены команды
        if (vitem[i1-1].item.size() >= symbCancel.size())
        {
            auto i = vitem[i1-1].item.size() - symbCancel.size(); // индекс начала символа отмены команды на конце текста пункта
            // если на конце текста пред. пункта содержится символ отмены команды
            if(!vitem[i1-1].item.compare(i, symbCancel.size(), symbCancel))
            {
                // удалить символ отмены команды
                vitem[i1-1].item.erase(i, i+symbCancel.size());
                // копировать текущий пункт к содержимому предыд.
                vitem[i1-1].item += vitem[i1].level + vitem[i1].item;
                // удалить пункт
                vitem.erase(vitem.begin()+i1, vitem.begin()+i1+1);
                --i1;
            }
        }
    }

    // запись в буфер конструкции перечисления на языке LaTeX
    std::string scommBeg, scommEnd, stemp2;
    if (p->value[2] == "") // ненумерованое перечисление
    {
        scommBeg = "\\begin{itemize}\n";
        scommEnd = "\\end{itemize}";
    }
    else
    {
        if (p->value[2] == "numeric") // нумерованое
        {
            scommBeg = "\\begin{enumerate}\n";
            scommEnd = "\\end{enumerate}";
        }
    }

    for(i = 0; i < vitem.size(); ++i) // проход вектора пунктов
    {
        if (!i) // первый пункт
        {
            stemp += scommBeg;
        }
        else
            if (vitem[i-1].level.size() < vitem[i].level.size()) // если предыдущий пункт был меньшего уровня
            {
                // если предыдущий пункт на более чем 1 уровень меньше (пр. * и ***)
                if (vitem[i-1].level.size() + vsymb[0].size() != vitem[i].level.size())
                {
                    // построить сообщение об ошибочном пункте
                    met2: stemp2.clear();
                    for (decltype(i) j=0, j1=0; j < vitem[i].item.size(); ++j, ++j1)
                    {
                        if (vitem[i].item[j] != '\n' && vitem[i].item[j] != 10)
                            stemp2.push_back(vitem[i].item[j]);
                        else
                            --j1;
                        // в сообщение записать не более 8 первых символов пункта не считая переходов на новую строку
                        if (j1 == 8) break;
                    }
                    stemp2 += "...";
                    printf("В перечислении \"%s\" неправильный порядок обозначения подпунктов! (см. пункт №%ld \"%s\")\n", infoEnumInStr(p, stemp).c_str(), i+1, stemp2.c_str());
                    exit(1);
                }
                stemp += scommBeg + "\n";
            }
            else // если предыдущий пункт был большего уровня
                if (vitem[i-1].level.size() > vitem[i].level.size())
                {
                    // вписать команду scommEnd столько раз насколько пред. пункт большего уровня
                    for(auto i2 = vitem[i-1].level.size() - vitem[i].level.size(); i2 > 0; --i2)
                    {
                        stemp += scommEnd + "\n";
                    }
                }
        stemp2 = vitem[i].item;
        stemp += "\\item " + headerToLaTex(stemp2) + "\n";
    }
    stemp += scommEnd;
    writeText(stemp, buf);
}

// вставить таблицу (команда @table)
void GenDoc::writeTable (const decltype(root) p, std::string &buf)
{
    if (p->value[2] == "latex")
    {
        writeText(p->value[3], buf);
    }
    else
        if (p->value[2] == "tag")
        {
            // код таблицы на языке разметки
            const std::string &st = p->value[3];
            //////////////////////////////////////
            // сформировать таблицу на LaTeX
            FILE *fin, *fout;
            const char * const fnameIn = "tabletmp1.tmp"; // файл с описанием таблицы тэгами
            const char * const fnameOut = "tabletmp2.tmp"; // файл с описанием таблицы на LaTeX
            remove(fnameIn);
            remove(fnameOut);
            if(!(fin = fopen(fnameIn, "wt" )))
            {
                printf ("Не удалось открыть временный файл \"%s\"\n", fnameIn);
                exit (1);
            }
            fputs (st.c_str(), fin); // записать описанием таблицы тэгами
            fclose (fin);
            if (std::system ((std::string("./table ") + fnameIn + " " + fnameOut).c_str())) // сформировать код таблицы
            {
                std::string stemp;
                infoEnumInStr(p, stemp); // краткое описание таблицы на LaTeX
                printf ("Ошибка построения таблицы \"%s\"!\n", stemp.c_str());
                remove(fnameIn);
                remove(fnameOut);
                exit(1);
            }
            remove(fnameIn);
            if(!(fout = fopen(fnameOut, "rt" ))) // записать описание таблицы на LaTeX
            {
                printf ("Не удалось открыть временный файл \"%s\"\n", fnameOut);
                exit (1);
            }
            std::string stemp; // содержимое таблицы на LaTeX
            char ch;
            while(!feof(fout)) // копирование содержимого файла в вектор value узла
            {
                ch=getc(fout);
                if (!feof(fout)) stemp.push_back(ch);
            }
            fclose(fout);
            remove(fnameOut);

            // сформировать код вставки таблицы на LaTex
            std::string scaption; // подпись к таблице
            // если параметр "text" в векторе value не указан (пустая строка), то подпись
            // к таблице не делается и таблица в документе не нумеруется
            if (p->value[6].size())
            {
                scaption = p->value[7];
                scaption = "\\caption{" + headerToLaTex(scaption) + "}\n";
            }

            std::string label; // ссылка к таблице
            if (p->value[8].size()) // если указана ссылка
            {
                label = p->value[9];
                label = "\\label{" + headerToLaTex(label) + "}";
            }

            std::string sresult = "\\begin{table}[ht]\n\\begin{center}\n\\begin{tabular}"+\
                                    stemp +\
                                    "\n\\end{tabular}\n" +\
                                    scaption +\
                                    label +\
                                    "\n\\end{center}\n\\end{table}";
            writeText(sresult, buf);
        }
}
