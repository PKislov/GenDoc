#include <cstdio>
#include "fileRing.h"

FileTree::FileTree(const char *n) // в параметрах - имя "главного" файла
{
	root = new struct node;
    root->parent=NULL;
    root->name=n;
	temp = root;
	root->countF=0;
}
FileTree::~FileTree()
{
	delElem(root); // удалить всю часть дерева после узла root
	delete root;
}

// добавить потомка узлу p и возвратить потомка
struct FileTree::node* FileTree::addChild(struct node *p)
{
    if (!p) return NULL;
    p->children.push_back(new struct node);
    p->children.back()->parent=p;
    return p->children.back();
}
// добавление элементов в дерево.
bool FileTree::addElem(const char *name) // В параметрах - имя файла
{
	temp=addChild(temp);
	temp->countF=0;
	temp->name = name;

    auto ptemp=temp->parent;
    while (ptemp) // поиск закольцованностей
    {
        if (ptemp->name == temp->name) return false;
        ptemp=ptemp->parent;
    }
    return true;
}
void FileTree::delElem(struct node *p) // удаление из памяти элементов дерева после узла p
{
	if (!p || !p->children.size()) return;
	for (int i=0; i<p->children.size(); ++i)
		if (p->children[i]->children.size()) delElem (p->children[i]);
	for (int i=0; i<p->children.size(); ++i)
	{
		//puts(p->children[i]->name.c_str());
		delete p->children[i];
		p->children[i] = NULL;
	}
	p->children.clear();
}
