#ifndef DOMBUILDER_INCLUDED
#define DOMBUILDER_INCLUDED
#include <string>
#include <vector>

class Dom
{
    // òèïû óçëîâ
    const std::string document="document"; // êîðåíü äåðåâà, óçåë ãåíåðèðóåòñÿ êîíñòðóêòîðîì, íóæåí ôîðìàëüíî
    const std::string title="title"; // çàãîëîâîê äîêóìåíòà
    const std::string toc="toc"; // ñîäåðæàíèå (îãëàâëåíèå)
    const std::string section1="section1";
    const std::string section2="section2";
    const std::string section3="section3";
    const std::string section4="section4";
    const std::string text="text"; // ïðîèçâîëüíûé òåêñò
    const std::string image="image";
    const std::string nothing="nothing"; // ïóñòîé óçåë

    struct node
    {
        node *parent; // óêàçàòåëü íà ðîäèòåëÿ óçëà
        std::vector <node*>children; // ìàññèâ óêàçàòåëåé íà ïîòîìêîâ óçëà
        std::string id; // òèï óçëà (òåêñò, ðèñóíîê, çàãîëîâîê è ò.ä.)
        std::vector <std::string>value; // ìàññèâ çíà÷åíèé (àòòðèáóòîâ) óçëà (ó ðàçíûõ òèïîâ ðàçíûé íàáîð)
    };

    node *root; // êîðåíü äåðåâà (âñåãäà id ðàâíî "document")
    node *temp; // óêàçàòåëü íà óçåë, ñ êîòîðûì â ïîñëåäíèé ðàç ïðîâîäèëàñü îïåðàöèÿ

	struct node* addChild(struct node *p); // äîáàâèòü ïîòîìêà óçëó p è âîçâðàòèòü ïîòîìêà
	struct node* addBro(struct node *p); // äîáàâèòü â äåðåâî óçåë îäèíàêîâîãî ópîâíÿ ñ p è âîçâðàòèòü åãî
	void delElem(struct node *p); // óäàëÿåò âñþ ÷àñòü äåðåâà ïîñëå óçëà p. Èñï. â äåñòðóêòîðå.
	inline bool isSection (const struct node *p) const; // âîçâðàùàåò true, åñëè óçåë ð - çàãîëîâîê

 public:

    Dom();
    ~Dom();

    // äîáàâëåíèå ýëåìåíòîâ â äåðåâî (ìåòîäû äëÿ ëåêñåðà):
    void addText(FILE *f, const char *name); // Òåêñò
    void addToc(); // Ñîäåðæàíèå
    void addTitle(const char *s); // Íàçâàíèå äîêóìåíòà
    void addImageId(const char *s); // ðèñóíîê ñ id
    void addImageRef(const char *s); // ðèñóíîê ñ ññûëêîé
    void addSection1(const char *s); // çàãîëîâîê óðîâíÿ 1
    void addSection2(const char *s); // çàãîëîâîê óðîâíÿ 2
    void addSection3(const char *s); // çàãîëîâîê óðîâíÿ 3
    void addSection4(const char *s); // çàãîëîâîê óðîâíÿ 4
    
private:
    Dom (const Dom &d) {}
    Dom& operator= (const Dom &d) { return *this;}
};
#endif // DOMBUILDER_INCLUDED
