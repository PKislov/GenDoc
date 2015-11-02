#include <cstdio>
#include <cstring>
#include <cctype>
#include <vector>
#include "domBuilder.h"

Dom::Dom()
{
	root = new struct node;
    root->parent = NULL;
	root->id = document;
	temp = root;
}
Dom::~Dom()
{
    delElem(root); // удалить всю часть дерева после узла root
	delete root;
}
void Dom::delElem(struct node *p) // удаление из памяти элементов дерева после узла p
{
	if (!p || !p->children.size()) return;
	for (decltype(p->children.size()) i=0; i < p->children.size(); ++i)
		if (p->children[i]->children.size())
            delElem (p->children[i]);
	for (decltype(p->children.size()) i=0; i < p->children.size(); ++i)
	{
		//puts(p->children[i]->id.c_str());
		delete p->children[i];
		p->children[i] = NULL;
	}
	p->children.clear();
}
// добавить потомка узлу p и возвратить потомка
struct Dom::node* Dom::addChild(struct node *p)
{
    if (!p) return NULL;
    p->children.push_back(new struct node);
    p->children.back()->parent = p;
    return p->children.back();
}
// добавить узел одинакового уpовня с p и возвратить его
/*struct Dom::node* Dom::addBro(struct node *p)
{
    if (!p || !p->parent) return NULL;
    p->parent->children.push_back(new struct node);
    p->parent->children.back()->parent = p;
    return p->parent->children.back();
}*/

// возвращает true, если узел р - заголовок
bool Dom::isSection (const struct node * p) const
{
    return p->id == section1 || p->id == section2 || p->id == section3 || p->id == section4;
}

/*void Dom::addTitle(const char *s) // Название документа
{
    temp = addChild(root);
    temp->id = title;
    temp->value.push_back("");
    s += std::strlen("@title ");
    while(*s != '@')
    {
        temp->value.back().push_back(*s);
        ++s;
    }
    temp = temp->parent;
}*/

void Dom::addTitleBegin() // начало команды @title
{
    static bool f = false;
    if (f)
    {
        puts ("Команда \"@title\" указана более одного раза!\n");
        exit(1);
    }
    f = true;
    temp = addChild(root);
    temp->id = titleBegin;
    temp = temp->parent;
}
void Dom::addTitle() // окончание команды - @end title
{
    static bool f = false;
    if (f)
    {
        puts ("Команда \"@end title\" указана более одного раза!\n");
        exit(1);
    }
    f = true;
    // найти в дереве начало команды @title
    for (decltype(root->children.size()) i=0; i < root->children.size(); ++i)
    {
        if (root->children[i]->id == titleBegin) // нашли начало команды
        {
            root->children[i]->id = title;
            root->children[i]->value.push_back(""); // под название документа
            // если пустой заголовок
            if(i+1 == root->children.size())
                return;

            // копирование названия документа в узел root->children[i], удаление текстовых узлов после начала команды title
            for (decltype(root->children.size()) i1=i+1; i1 < root->children.size(); ++i1)
            {

                if(root->children[i1]->id != text)
                {
                    puts ("В теле команды \"@title ... @end title\" необходимо указывать только текст!\n");
                    exit(1);
                }
                root->children[i]->value[0] += root->children[i1]->value[0];
                delete root->children[i1];
                root->children[i1] = NULL;
                root->children.erase(root->children.begin()+i1);
                i1 = i;
            }
            return;
        }
    }
    puts ("Отсутствует начало команды \"@end title\"!\n");
    exit(1);
}

void Dom::addSection1(const char *s) // заголовок уровня 1
{
    temp = addChild(root);
    temp->id = section1;
    temp->value.push_back("");
    s += std::strlen("@section1 ");
	while(*s) temp->value.back().push_back(*s), ++s;
	delNewLineEndStr(temp->value.back());
}

void Dom::addSection2(const char *s) // заголовок уровня 2
{
    decltype(temp) plast = NULL; // проверка
    for (decltype(root->children.size()) i=0; i < root->children.size(); ++i)
        if (root->children[i]->id == section1)
            plast = root->children[i];
    if (!plast)
    {
        printf ("Заголовок \"%s\" опреден как \"section2\", не может быть объявлен раньше, чем \"section1\".\n", s);
        exit(1);
    }

    temp = addChild(plast);
    temp->id = section2;
    temp->value.push_back("");
    s += std::strlen("@section2 ");
	while(*s) temp->value.back().push_back(*s), ++s;
	delNewLineEndStr( temp->value.back());
}

void Dom::addSection3(const char *s) // заголовок уровня 3
{
    decltype(temp) plast=NULL; // проверка
    for (decltype(root->children.size()) i=0; i < root->children.size(); ++i)
        if (root->children[i]->id == section1)
            for (decltype(i) j=0; j < root->children[i]->children.size(); ++j)
            {
                if (root->children[i]->children[j]->id == section2)
                    plast = root->children[i]->children[j];
            }

    if (!plast)
    {
        printf ("Заголовок \"%s\" опреден как \"section3\", не может быть объявлен раньше, чем заголовки более высокого уровня.\n", s);
        exit(1);
    }

    temp = addChild(plast);
    temp->id = section3;
    temp->value.push_back("");
    s += std::strlen("@section3 ");
	while(*s) temp->value.back().push_back(*s), ++s;
	delNewLineEndStr( temp->value.back());
}

void Dom::addSection4(const char *s) // заголовок уровня 4
{
    decltype(temp) plast=NULL; // проверка
    for (decltype(root->children.size()) i=0; i < root->children.size(); ++i)
        if (root->children[i]->id == section1)
            for (decltype(i) j=0; j < root->children[i]->children.size(); ++j)
                if (root->children[i]->children[j]->id == section2)
                    for (decltype(i) jj=0; jj < root->children[i]->children[j]->children.size(); ++jj)
                        if (root->children[i]->children[j]->children[jj]->id == section3)
                            plast = root->children[i]->children[j]->children[jj];

    if (!plast)
    {
        printf ("Заголовок \"%s\" опреден как \"section4\", не может быть объявлен раньше, чем заголовки более высокого уровня.\n", s);
        exit(1);
    }

    temp = addChild(plast);
    temp->id = section4;
    temp->value.push_back("");
    s += std::strlen("@section4 ");
	while(*s) temp->value.back().push_back(*s), ++s;
	delNewLineEndStr( temp->value.back());
}

void Dom::addToc() // Содержание
{
    temp = addChild(root);
	temp->id = toc;
	temp = temp->parent;
	ftoc = true;
}

void Dom::addImageRef(const char *s,  decltype(root->children.size()) n1, decltype(n1) n2) // рисунок с ref
{
    temp = addChild(temp);
    temp->id = image;
    temp->value.push_back("text");
    temp->value.push_back(""); // подпись рисунка
    temp->value.push_back("ref");
    temp->value.push_back(""); // значение ref
    while(*s != '\"') ++s; ++s; // дойти до начала подписи
    while(*s != '\"') temp->value[n1].push_back(*s), ++s; ++s; // записать подпись
	while(*s != '\"') ++s; ++s;
	while (isspace(*s)) ++s;
	while(*s != '\"') temp->value[n2].push_back(*s), ++s;
	temp = temp->parent;
}
void Dom::addImageRef(const char *s) // рисунок с ref
{
    temp = addChild(temp);
    temp->id = image;
    temp->value.push_back("");
    temp->value.push_back(""); // подпись рисунка
    temp->value.push_back("ref");
    temp->value.push_back(""); // значение ref
    while(*s != '\"') ++s; ++s; // дойти до начала ref
    while(*s != '\"') temp->value[3].push_back(*s), ++s; ++s; // записать ref
	temp = temp->parent;
}

void Dom::addText(FILE *f, const char *name) // Текст
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
    temp = addChild(temp);
    temp->id  = text;
    temp->value.push_back("");

    char ch;
    while(!(feof(f)))
    {
        ch=fgetc(f);
        if (!feof(f)) temp->value.back().push_back(ch);
    }
    fclose(f);
    remove(name);
    if (!(f = fopen(name, "wt")))
    {
        printf ("Не удалось открыть временный файл \"%s\"\n", name);
		exit (1);
    }
    temp = temp->parent;
}

void Dom::delNewLineEndStr (std::string &s) const
{
    // удалить переходы на новую строку в конце строки
    for(decltype(s.size()) i=s.size()-1; i >= 0; --i)
    {
        if(s[i] == 10 || s[i] == 13 || s[i] == '\n')
            s.erase(i);
        else
            break;
    }
}
