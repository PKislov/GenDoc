#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <vector>
#include "table.h"

Table::Table()
{
	temp = root = new struct node;
    root->parent = NULL;
	root->id = table;
}
Table::~Table()
{
    delElem(root); // удалить всю часть дерева после узла root
	delete root;
}
void Table::delElem(struct node *p) // удаление из памяти элементов дерева после узла p
{
	if (!p || !p->children.size()) return;
	for (decltype(p->children.size()) i=0; i < p->children.size(); ++i)
		if (p->children[i]->children.size())
            delElem (p->children[i]);
	for (decltype(p->children.size()) i=0; i < p->children.size(); ++i)
	{
		delete p->children[i];
		p->children[i] = NULL;
	}
	p->children.clear();
}
// добавить потомка узлу p и возвратить потомка
struct Table::node* Table::addChild(struct node *p)
{
    if (!p) return NULL;
    p->children.push_back(new struct node);
    p->children.back()->parent = p;
    return p->children.back();
}

void Table::addTR(const char *s) // команда {tr}
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    temp = addChild(root);
	temp->id = tr;
	temp = root;
}


void Table::addText(FILE *f, const char *name) // Текст ячейки
{
    // Считать из файла текст и добавить в дерево/
    // В файл f перенаправлен поток символов между командами.
    // Перед обработкой любой команды текст нужно записать
    // в дерево и обнулить файл.
    fclose(f);
    if (!(f = fopen(name, "rt")) || feof (f))
    {
        printf ("Не удалось открыть временный файл \"%s\"\n", name);
		exit (1);
    }
    // узлы "text" идущие друг за другом объединять
    if (temp && temp->children.size() && temp->children.back()->id == text)
        {
            temp = temp->children.back();
        }
        else
        {
            temp = addChild(root);
            temp->id  = text;
            temp->value.push_back("");
        }

    char ch;
    while(!(feof(f)))
    {
        ch=fgetc(f);
        if (!feof(f)) temp->value.back().push_back(ch);
    }
    fclose(f);
    remove(name);
    // замена в строке последовательности символов "\\a" на "\""
    SeqSymbContrReplace(temp->value.back(), false);
    if (!(f = fopen(name, "wt")))
    {
        printf ("Не удалось открыть временный файл \"%s\"\n", name);
		exit (1);
    }
    temp = root;
}


// заменять ли при занесении данных в DOM последовательности символов "\\a"
// на на одну кавычку " (если параметр fPareamInQuotes истина), иначе на "\\""
const std::string& Table::SeqSymbContrReplace (std::string &s, const bool fPareamInQuotes) const
{
    if (s.size() >= 2)
    {
        for(decltype(s.size()) i=0; i < s.size()-1; ++i)
            if (s[i] == '\\' && s[i+1] == '\a')
            {
                s[i+1] = '\"';
                if (fPareamInQuotes)
                    s.erase(i,1); // удалить косую черту
                else
                    ++i;
            }
    }
    return s;
}

// удаляет все заданные символы на конце строки
const std::string& Table::delSymbsInEndStr (std::string &s, const char ch) const
{
    while (s.size())
    {
        if (s.back() == ch)
            s.pop_back();
        else
            break;
    }
    return s;
}


// команда {td; rowspan:" ... "; colspan:" ... "}
// индексы n1 и n2 - порядковые номера следования параметров rowspan и colspan
void Table::addTD(const char *s, int n1, int n2)
{
    // если перед командой стоит знак '\\'
    if (cancelComm (s))
    {
        return;
    }
    temp = addChild(root);
    temp->id = td;
    temp->value.push_back("");   // [0] - rowspan
    temp->value.push_back("");   // [1] - значение rowspan
    temp->value.push_back("");   // [2] - colspan
    temp->value.push_back("");   // [3] - значение colspan
    temp->value.push_back(s);    // [4] - yytext - для отображения ошибок

    if (n1) // если задан параметр rowspan
    {
        auto refBeg = s;
        for (int i=0; i < n1*2-1; ++i)
        {
            while(*refBeg != '\"') ++refBeg; ++refBeg; // дойти до начала параметра rowspan
        }
        while(*refBeg != '\"') temp->value[1].push_back(*refBeg), ++refBeg; ++refBeg; // записать значение rowspan
        int i = std::atoi(temp->value[1].c_str());
        if (i < 1)
        {
            printf ("В команде \"%s\" можно указывать значения только от 1!\n", s);
            exit(1);
        }
        else
            if (i == 1) // команда rowspan не имеет действия при параметре 1
            {
                temp->value[1].clear();
            }
            else
            {
                temp->value[0] = "rowspan";
            }
    }
    if (n2) // если задан параметр colspan
    {
        auto refBeg = s; // записать параметр colspan
        for (int i=0; i < n2*2-1; ++i)
        {
            while(*refBeg != '\"') ++refBeg; ++refBeg; // дойти до начала colspan
        }
        while(*refBeg != '\"') temp->value[3].push_back(*refBeg), ++refBeg; ++refBeg; // записать значение colspan
        int i = std::atoi(temp->value[3].c_str());
        if (i < 1)
        {
            printf ("В команде \"%s\" можно указывать значения только от 1!\n", s);
            exit(1);
        }
        else
            if (i == 1) // команда colspan не имеет действия
            {
                temp->value[3].clear();
            }
            else
            {
                temp->value[2] = "colspan";
            }
    }
    temp = root;
}

// возвращает истину, если строка состоит из пробельных символов
bool Table::isSpace(const std::string &s) const
{
    for (decltype(s.size()) i=0; i < s.size(); ++i)
    {
        if (!std::isspace(s[i])) return false;
    }
    return true;
}
// поиск синтаксических ошибок описания таблицы
void Table::findError() const
{
    std::vector <struct node*> &vn = temp->children;

    if (!temp || !temp->children.size())
    {
        puts("Пустое описание таблицы!");
        exit (1);
    }

    // если первый узел - не пробельная строка
    if (vn[0]->id == text)
    {
        if (!isSpace(vn[0]->value[0]))
        {
            met1: puts("Описание таблицы должно начинаться с команды \"{tr}\"!");
            exit (1);
        }
        else
        {
            delete vn[0]; // удалить пробельный текст перед первой командой tr
            vn.erase(vn.begin());
        }
    }
    // проверить есть ли команды tr и td
    bool f1 = false, f2 = false;
    decltype(vn.size()) i = 0;
    for (; i < vn.size(); ++i)
    {
        if (vn[i]->id == tr) f1 = true;
            else
                if (vn[i]->id == td) f2 = true;
    }
    if (!f1)
    {
        puts("В описании таблицы отсутствует  команда \"{tr}\"!");
        exit (1);
    }
    if (!f2)
    {
        puts("В описании таблицы отсутствует  команда \"{td}\"!");
        exit (1);
    }
    if (vn[0]->id != tr) // если описание таблицы не начинается с команды {tr}
    {
        goto met1;
    }
    // проверить и удалить пробельный текст между tr и td
    if (vn.size() >= 3)
    {
        for (i = 0; i < vn.size()-1; ++i)
        {
            if (vn[i]->id == tr && vn[i+1]->id == text)
            {
                if (!isSpace(vn[i+1]->value[0]))
                {
                    puts("Пропущена команда \"{td}\"!");
                    exit(1);
                }
                else
                {
                    delete vn[i+1]; // удалить пробельный текст перед командой td
                    vn.erase(vn.begin()+i+1);
                }
            }
        }
    }
}

// возвращает строку, содержащую код таблицы на языке LaTeX
std::string& Table::genTableCode ()
{
    findError();
    rows.clear(); // вектор строк таблицы
    sresult.clear(); // строка кода таблицы на языке LaTeX

    std::vector <struct node*> &vn = temp->children;
    x = 0, y = 0; // размеры таблицы
    decltype(vn.size()) count, count2;
    decltype(vn.size()) i, i1, i2, i3;
    for (i = 0; i < vn.size(); ++i) // подсчитать предварительное кол-во строк таблицы не учитывая команды растяжения
    {
        if (vn[i]->id == tr)
        {
            ++y;
        }
    }

    for (i = 0; i < vn.size(); ++i) // подсчитать предварительное кол-во столбцов таблицы
    {
        if (vn[i]->id == tr)
        {
            for (++i, count = 0; i < vn.size(); ++i)
            {
                if (vn[i]->id == tr)
                {
                    --i;
                    break;
                }
                else
                    if (vn[i]->id == td)
                        ++count;
            }
            if (x < count) x = count;
        }
    }
    struct Row temprow;
    struct Cell tempcell,tempcell2;
    tempcell.backfilled = true;
    for (i = 0; i < x; ++i) temprow.cells.push_back(tempcell); // сгенерировать таблицу по предв. размерам
    for (i = 0; i < y; ++i) rows.push_back(temprow);
    // заполнение ячеек таблицы
    for (i = 0, i1 = -1; i < vn.size(); ++i)
    {
        if (vn[i]->id == tr)
        {
            ++i1;
            for (++i, i2=0; i < vn.size(); ++i)
            {
                if (vn[i]->id == tr)
                {
                    --i;
                    break;
                }
                else
                    if (vn[i]->id == text)
                    {
                        rows[i1].cells[i2].backfilled = false; // текст ячейки указан в описании
                        delSymbsInEndStr(rows[i1].cells[i2].st = vn[i]->value[0]);
                        if (vn[i-1]->id == td)
                        {
                            if (vn[i-1]->value[0] == "rowspan") // записать индексы растяжения
                            {
                                rows[i1].cells[i2].rowspan = std::atoi(vn[i-1]->value[1].c_str());
                            }
                            if (vn[i-1]->value[2] == "colspan")
                            {
                                rows[i1].cells[i2].colspan = std::atoi(vn[i-1]->value[3].c_str());
                            }
                            rows[i1].cells[i2].comm = vn[i-1]->value[4]; // записать команду растяжения ячейки
                        }
                        ++i2;
                    }
            }
        }
    } // заполнение ячеек таблицы

    std::string stemp;

    // прераспределение ячеек согласно командам  colspan rowspan
    met1: for (i = 0; i < rows.size(); ++i) // строки
    {
        for (i1 = 0; i1 < rows[i].cells.size(); ++i1) // ячейки
        {
            count = rows[i].cells[i1].colspan; // индексы растяжения
            count2 = rows[i].cells[i1].rowspan;
            if (count > 1 && count2 <= 1) // выполнение команды {td;colspan:"..."}
            {
                runColspan(i, i1, count, rows[i].cells[i1].comm);
            }
            else
                if (count2 > 1 && count <= 1) // выполнение команды {td;rowspan:"..."}
                {
                    runRowspan(i, i1, count2, rows[i].cells[i1].comm);
                }
                else
                    if (count > 1 && count2 > 1) // выполнение команды {td;rowspan:"...";colspan:"..."}
                    {
                        runColspan(i, i1, count, rows[i].cells[i1].comm); // растянуть ячейку вправо
                        for (i2=0; i2 < count; ++i2) // проход ячеек области
                        {
                            runRowspan(i, i1+i2, count2, rows[i].cells[i1].comm);
                            if (i2)
                            {
                                rows[i].cells[i1+i2].busy = true;
                                rows[i].cells[i1+i2].verticbusy = true;
                            }
                        }
                    }
        }
    }

    //////////////////////////////////////////////
    // +генерация кода таблицы на языке LaTeX

    sresult = "{";
    for (i = 0; i < x; ++i) sresult += "|c"; // указание кол-ва столбцов таблицы
    sresult += "|} \\hline\n";
    for (i = 0; i < y; ++i) // проход строк
    {
        for (i1 = 0; i1 < x; ++i1) // проход столбцов
        {
            if (rows[i].cells[i1].nothing) continue; // ячейка занята командой {td;rowspan:"...";colspan:"..."}

            if (rows[i].cells[i1].colspan > 1 && rows[i].cells[i1].rowspan == 1) //  команда {td;colspan:"..."}
            {
                sresult += "\\multicolumn{" + intInString(stemp, rows[i].cells[i1].colspan) +\
                + "}{|c|}{" + rows[i].cells[i1].st + "}";
                // записать число в строку
                if (i1 < x - rows[i].cells[i1].colspan) sresult += " & ";
            }
            else
                if (rows[i].cells[i1].rowspan > 1 && rows[i].cells[i1].colspan == 1) // команда {td;rowspan:"..."}
                {
                    sresult += "\\multirow{" + intInString(stemp, rows[i].cells[i1].rowspan) +\
                    "}*{" + rows[i].cells[i1].st + "}";
                    if (i1 < x - rows[i].cells[i1].colspan) sresult += " & ";
                }
                else
                    if (rows[i].cells[i1].rowspan > 1 && rows[i].cells[i1].colspan > 1) // команда {td;rowspan:"...";colspan:"..."}
                    {
                        sresult +=  "\\multicolumn{" +\
                                    intInString(stemp, rows[i].cells[i1].colspan) +\
                                    + "}{c|}";
                        sresult +=  "{\\multirow{" +\
                                    intInString(stemp, rows[i].cells[i1].rowspan) +\
                                    "}*{" + rows[i].cells[i1].st + "}}";
                        if (i1 < x - rows[i].cells[i1].colspan) sresult += " & ";
                        tempcell2.st = "\\multicolumn{" +\
                                        intInString(stemp, rows[i].cells[i1].colspan) +\
                                        + "}{c|}{}";
                        tempcell2.verticbusy = true;
                        for (i2=i+1; i2 < rows[i].cells[i1].rowspan + i; ++i2)
                        {
                            rows[i2].cells[i1] = tempcell2;
                        }
                        for (i3 = i1 + 1; i3 < rows[i].cells[i1].colspan + i1; ++i3)
                        {
                            for (i2=i+1; i2 < rows[i].cells[i1].rowspan + i; ++i2)
                            {
                                rows[i2].cells[i3].nothing = true;
                            }
                        }

                        i1 += rows[i].cells[i1].colspan-1; // пропуск области
                    }
                    else
                        if (!rows[i].cells[i1].busy) // текст ячейки
                        {
                            sresult += rows[i].cells[i1].st;
                            if (i1 < x - 1) sresult += " & ";
                        }
                        else
                            if (rows[i].cells[i1].verticbusy)
                            {
                                if (i1 < x - 1) sresult += " & ";
                            }
        }

        sresult += "\\\\ "; // команда завершения строки таблицы
        std::vector <decltype(i)> vind; // вектор с номерами ячеек, под которыми проводить горизонтальную черту
        for (i1=0; i1 < x; ++i1)
        {
            if (!(rows[i].cells[i1].rowspan >= 2 || rows[i].cells[i1].verticbusy && \
                i < y-1 && rows[i+1].cells[i1].verticbusy)) // если не команда rowspan
                    vind.push_back(i1+1); // записать индекс ячейки
        }

        if (vind.size()) // если есть такие ячейки
        {
            if (vind.size() == 1) // если ячейка одна
            {
                sresult += "\\cline{" + intInString(stemp, vind[0]) + "-" + \
                intInString(stemp, vind[0]) + "}";
            }
            else // ячеек несколько
            {
                i1 = 0;
                met12: count = vind[i1];
                for (i2 = i1 + 1; i2 < vind.size(); ++i2) // найти диапазоны ячеек через которые проводить черту
                {
                    if (vind[i2]-vind[i2-1] != 1) break;
                }
                sresult += "\\cline{" + intInString(stemp, count);
                sresult += "-" + intInString(stemp, vind[i2-1]) + "}";

                if (i2 < vind.size())
                {
                    i1 = i2;
                    goto met12; // к поиску нового диапазона
                }
            }
        }

        sresult += "\n";
    }

    return sresult;
}

// выполнение команды rowspan, i - номер строки, i1 - номер ячейки в строке, count - смещение
void Table::runRowspan (decltype(x) i, decltype(i) i1, decltype(i) count, const std::string &s)
{
    struct Cell tempcell;
    decltype(i) i2, i3;

    // создание новых строк если необходимо
    i2 = i + count - 1; // индекс последней строки в команде rowspan
    auto yprev = i2;
    if (i2 + 1 > y) // если требуется нарастить строки
    {
        struct Row temprow;
        tempcell.backfilled = true; // ячейка не указана пользователем в описании таблицы
        for (i3 = 0; i3 < x; ++i3) temprow.cells.push_back(tempcell); // формировать строку
        for (i3 = 0; i3 < i2+1-y; ++i3)
        {
            rows.push_back(temprow);
        }
        y = rows.size(); // записать новый размер таблицы
        tempcell.backfilled = false;
    }

    // смещение в нижних строках ячеек на 1 вправо
    decltype (yprev) xprev = x;
    for (i2 = i + 1; i2 < count + i; ++i2) // проход строк
    {
        if (i2 <= yprev) // если не дошли до наращенных строк
        {
            runColspan(i2, i1-1, 2, s);
            rows[i2].cells[i1].verticbusy = true;
        }
        else
            {
                rows[i2].cells[i1].busy = rows[i2].cells[i1].verticbusy = true; // установить флаги занятости ячейки
            }
    }
}

// выполнение команды colspan, i - номер строки, i1 - номер ячейки в строке, count - смещение
void Table::runColspan (decltype(x) i, decltype(i) i1, decltype(i) count, const std::string &s)
{
    struct Cell tempcell, tempc;
    decltype(i) i2, i3, xprev;
    for (i2 = i1 + 1; i2 < count + i1; ++i2) // проверить перекрытие команд colspan и rowspan
    {
        if (i2 >= rows[i].cells.size()) break;
        if (rows[i].cells[i2].busy) // ошли до ячейки занятой другой командой
        {
            std::string scomm; // текст команды, с которой перекрывается, найти её
            for(i3 = i-1; i3 >= 0; --i3)
            {
                if (rows[i3].cells[i2].comm.size())
                {
                    scomm = rows[i3].cells[i2].comm;
                    break;
                }
            }
            printf("В описании таблицы команды colspan и rowspan перекрываются!\nСтрока %ld, столбец %ld, команды \"%s\" и \"%s\"\n", i+1, i2+1, s.c_str(), scomm.c_str());
            exit (1);
        }
    }

    ///////////////
    xprev = rows[i].cells.size();
    // смещение ячеек вправо
    tempcell.busy = true;
    for (i2 = 1; i2 < count; ++i2) // проход области вправо
    {
        if (i1 + i2 < rows[i].cells.size() && rows[i].cells[i1+i2].backfilled) // если ячейка добавлена программно
        {
            rows[i].cells[i1+i2] = tempcell;
        }
        else
        {
            rows[i].cells.insert(rows[i].cells.begin()+i1+1,tempcell); // вставить новую ячейку после команды, ячейки идущие следом сдвигаются вправо
            for (i3 = i1+1; i3 < rows[i].cells.size()-1; ++i3) // проверить, не сместили ли ячейки занятые командой rowspan
            {
                if (!rows[i].cells[i3].busy &&\
                    rows[i].cells[i3+1].busy &&\
                    rows[i].cells[i3+1].verticbusy)
                {
                    // обменять содержимое ячеек, выравнивается блок ячеек занятых командой rowspan
                    tempc =  rows[i].cells[i3];
                    rows[i].cells[i3] = rows[i].cells[i3+1];
                    rows[i].cells[i3+1] = tempc;
                }
            }
        }
    }

    i3 = rows[i].cells.size();

    // если строка стала длиннее, то удалить на конце строки ячейки помеченные как backfilled
    // на добавленном участке ячеек
    if (i3 > x)
    {
        for (i2 = x; i2 < i3; ++i2)
        {
            if (!rows[i].cells.back().backfilled) break;
            rows[i].cells.pop_back();
        }
    }

    // нарастить размеры остальных строк
    x = rows[i].cells.size();
    tempcell.busy = false;
    tempcell.backfilled = true;
    for (i2 = 0; i2 < rows.size(); ++i2)
    {
        if (i2 == i) continue;
        for (i3=rows[i2].cells.size(); i3 < rows[i].cells.size(); ++i3)
            rows[i2].cells.push_back(tempcell);
    }
}

// функция обработки отмены команды, например "\\@tr"
// возвращает истину, если перед командой находится символ symbCancel
bool Table::cancelComm (const char *s)
{
    // если предыдущий узел - текст и на конце текста знак '\\'
    if (temp && temp->children.size() &&\
        temp->children.back()->id == text &&\
        temp->children.back()->value[0].size() >= symbCancel.size())
    {
        // индекс начала символа отмены команды на конце текста
        auto i =  temp->children.back()->value[0].size() - symbCancel.size();
        // если на конце текста пред. пункта содержится символ отмены команды
        if(!temp->children.back()->value[0].compare(i, symbCancel.size(), symbCancel))
        {
            // удалить символ отмены команды
            temp->children.back()->value[0].erase(i, i+symbCancel.size());
            std::string stemp(s);
            // к концу текста добавить текст команды, перед которой стоял знак
            temp->children.back()->value[0] += SeqSymbContrReplace(stemp, false);
            return true;
        }
    }
    return false;
}


// записывает число n в строку s
std::string& Table::intInString (std::string &s, unsigned n) const
{
    int r=0, n1=n; // разрядность
    while (n1 > 0)
    {
        n1 /= 10;
        ++r;
    }
    char *p = new char [r+1];
    p[r] = '\0';
    sprintf(p, "%d", n);
    s = p;
    delete[]p;
    return s;
}
