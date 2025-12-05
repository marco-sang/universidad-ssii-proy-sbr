#ifndef __UTILS_H
#define __UTILS_H

#include "SBR.h"

/*
* Imprime los nombres de una lista de reglas.
*/
void printReglas(const list<shared_ptr<Regla>>& reglasCC, ofstream& file);

/*
* Imprime una lista de strings.
*/
void printStrList(list<string>& hechos, ofstream& file);


#endif