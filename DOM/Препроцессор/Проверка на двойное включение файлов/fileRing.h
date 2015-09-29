#ifndef FILERING_INCLUDED
#define FILERING_INCLUDED
#include <iostream>
#include <string>
#include <vector>

class FileTree
{
public:
    struct node
    {
        int countF; // счетчик выполненных команд include в файле, используется для "прогонки" файла через yylex() после выполнения каждой команды до новой невыполненной команды.
        node *parent; // указатель на родителя узла
        std::vector <node*>children; // массив указателей на потомков узла
        std::string name; // имя файла
    };

    node *root; // корень дерева, соответствует "главному" файлу
    node *temp; // указатель на узел, с которым в последний раз проводилась операция добавления потомка

private:
	struct node* addChild(struct node *p); // добавить потомка узлу p и возвратить потомка
	void delElem(struct node *p); // удаляет всю часть дерева после узла p. Исп. в деструкторе.
public:
    FileTree(const char*); // в параметрах - имя "главного" файла
    ~FileTree();

    const char *getCurName() const // возвратить текущее имя файла
    {
        return temp->name.c_str();
    }

    // добавление элементов в дерево. В параметрах - имя файла
    bool addElem(const char *name); // возвращает false в случае закольцованности
};
#endif // FILERING_INCLUDED
