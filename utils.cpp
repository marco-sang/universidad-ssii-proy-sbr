#include <fstream>
#include "utils.h"

using namespace std;

void printReglas(const list<shared_ptr<Regla>>& reglas, ofstream& file)
{
    unsigned spaces = 0;
    for(auto&& ract: reglas){
        file << ract->getNombre();
        if (++spaces < reglas.size())
            file <<", ";
    }
}

void printStrList(list<string>& hechos, ofstream& file)
{
    unsigned spaces = 0;
    for(auto&& s: hechos){
        file << s;
    if (++spaces < hechos.size())
            file <<", ";
    }
}