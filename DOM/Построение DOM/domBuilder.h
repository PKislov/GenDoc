#ifndef DOMBUILDER_INCLUDED
#define DOMBUILDER_INCLUDED
#include <string>
#include <vector>

class Dom
{
    // ���� �����
    const std::string document="document"; // ������ ������, ���� ������������ �������������, ����� ���������
    const std::string title="title"; // ��������� ���������
    const std::string toc="toc"; // ���������� (����������)
    const std::string section1="section1";
    const std::string section2="section2";
    const std::string section3="section3";
    const std::string section4="section4";
    const std::string text="text"; // ������������ �����
    const std::string image="image";
    const std::string nothing="nothing"; // ������ ����

    struct node
    {
        node *parent; // ��������� �� �������� ����
        std::vector <node*>children; // ������ ���������� �� �������� ����
        std::string id; // ��� ���� (�����, �������, ��������� � �.�.)
        std::vector <std::string>value; // ������ �������� (����������) ���� (� ������ ����� ������ �����)
    };

    node *root; // ������ ������ (������ id ����� "document")
    node *temp; // ��������� �� ����, � ������� � ��������� ��� ����������� ��������

	struct node* addChild(struct node *p); // �������� ������� ���� p � ���������� �������
	struct node* addBro(struct node *p); // �������� � ������ ���� ����������� �p���� � p � ���������� ���
	void delElem(struct node *p); // ������� ��� ����� ������ ����� ���� p. ���. � �����������.
	inline bool isSection (const struct node *p) const; // ���������� true, ���� ���� � - ���������

 public:

    Dom();
    ~Dom();

    // ���������� ��������� � ������ (������ ��� �������):
    void addText(FILE *f, const char *name); // �����
    void addToc(); // ����������
    void addTitle(const char *s); // �������� ���������
    void addImageId(const char *s); // ������� � id
    void addImageRef(const char *s); // ������� � �������
    void addSection1(const char *s); // ��������� ������ 1
    void addSection2(const char *s); // ��������� ������ 2
    void addSection3(const char *s); // ��������� ������ 3
    void addSection4(const char *s); // ��������� ������ 4
};
#endif // DOMBUILDER_INCLUDED
