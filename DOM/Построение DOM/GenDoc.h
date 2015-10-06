#ifndef GENDOC_H_INCLUDED
#define GENDOC_H_INCLUDED

#include <string>
#include "domBuilder.h"

class GenDoc : public Dom
{
    std::string fName; // имя вых.файла
public:
    GenDoc(const char*n) : Dom() { fName = n; }
    ~GenDoc(){}
};

#endif // GENDOC_H_INCLUDED
