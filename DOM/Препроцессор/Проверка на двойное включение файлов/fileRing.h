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
        int countF; // ������� ����������� ������ include � �����, ������������ ��� "��������" ����� ����� yylex() ����� ���������� ������ ������� �� ����� ������������� �������.
        node *parent; // ��������� �� �������� ����
        std::vector <node*>children; // ������ ���������� �� �������� ����
        std::string name; // ��� �����
    };

    node *root; // ������ ������, ������������� "��������" �����
    node *temp; // ��������� �� ����, � ������� � ��������� ��� ����������� �������� ���������� �������

private:
	struct node* addChild(struct node *p); // �������� ������� ���� p � ���������� �������
	void delElem(struct node *p); // ������� ��� ����� ������ ����� ���� p. ���. � �����������.
public:
    FileTree(const char*); // � ���������� - ��� "��������" �����
    ~FileTree();

    const char *getCurName() const // ���������� ������� ��� �����
    {
        return temp->name.c_str();
    }

    // ���������� ��������� � ������. � ���������� - ��� �����
    bool addElem(const char *name); // ���������� false � ������ ����������������
};
#endif // FILERING_INCLUDED
