#include "SBR.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>

#define END_RULE_NAME ':'
#define RULE_DELIMITER ','

using namespace std;

BC bc; //base de conocimiento;
BH bh; //base de hecho;

regex p_si("^si$", regex_constants::icase);
regex p_entonces("^entonces$", regex_constants::icase);
regex p_y("^y$", regex_constants::icase);
regex p_o("^o$", regex_constants::icase);


shared_ptr<Hecho> leerCondiciones(stringstream& linea)
{
    string palAct, op;
    list<string> condiciones;

    linea >> palAct;
    do{
        if(regex_match(palAct, p_o) || regex_match(palAct, p_y)) //comprueba si la palabra actual es 'y' u 'o'
            op = palAct;
        else if(!regex_match(palAct, p_si)) //comprueba si la palabra actual es 'si'
            condiciones.push_back(palAct);
        linea >> palAct;
    }while (!regex_match(palAct, p_entonces));

    auto hecho = make_shared<Hecho>();  //crea dinamicamente un objeto Hecho.
    if(regex_match(op, p_o))
        hecho->setTipo(disy);
    else if(regex_match(op, p_y))
        hecho->setTipo(conj);

    for(auto hact: condiciones){
        hecho->insert(hact);
    }

    return hecho;
}

void leerBaseConocimiento(const string& nombreFichero)
{
    ifstream archivo(nombreFichero);

    if (!archivo.is_open()) {
        cerr << "Error: no se pudo abrir el fichero " << nombreFichero << "\n";
        exit(EXIT_FAILURE);
    }

    int numeroReglas = 0;
    string linea;

    archivo >> numeroReglas;

    // ---- Leer cada regla ----
    for (int i = 0; i < numeroReglas; ++i) {
        archivo >> ws;   //consume los blancos
        if (!getline(archivo, linea)) {
            cerr << "Error: reglas insuficientes en el fichero\n";
            break;
        }
        stringstream ss(linea);
        string rnombre;

        //Leer identificador: Ej "R1:"
        ss >> rnombre;
        if (!rnombre.empty() && rnombre.back() == END_RULE_NAME)
            rnombre.pop_back(); // quitar ':'

        //leer condiciones
        auto causa = leerCondiciones(ss);
        string rconsecuencia;

        //leer la segunda parte de la implicación
        ss >> rconsecuencia;
        if (!rconsecuencia.empty() && rconsecuencia.back() == RULE_DELIMITER)
            rconsecuencia.pop_back(); // quitar ','
        auto consecuencia = make_shared<Hecho>(); // creación del hecho.
        consecuencia->insert(rconsecuencia);

        // leer el factor de certidumbre
        string rfc;
        ss >> rfc;
        float fc = stof(rfc.substr(rfc.find("FC=") + 3));

        //crear la regla
        auto r = make_shared<Regla>(rnombre, causa, consecuencia,fc);
        bc.pushRegla(r);
    }
    archivo.close();
}

string leerBaseHechos(string nombreFichero)
{
    ifstream archivo(nombreFichero);

    if (!archivo.is_open()) {
        cerr << "Error: no se pudo abrir el fichero " << nombreFichero << "\n";
        exit(EXIT_FAILURE);
    }

    int numeroHechos = 0;
    string linea;

    archivo >> numeroHechos;

    // ---- Leer cada hecho ----
    for (int i = 0; i < numeroHechos; ++i) {
        archivo >> ws;   //consume los blancos
        if (!getline(archivo, linea)) {
            cerr << "Error: reglas insuficientes en el fichero\n";
            break;
        }
        stringstream ss(linea);
        string hechoAct;

        //Leer el hecho
        ss >> hechoAct;
        if (!hechoAct.empty() && hechoAct.back() == RULE_DELIMITER)
            hechoAct.pop_back(); // quitar ','
        //leer factor de certidumbre
        string hfc;
        ss >> hfc;
        float fc = stof(hfc.substr(hfc.find("FC=") + 3));

        Hecho newHecho;
        newHecho.insert(hechoAct);
        newHecho.setFC(fc);
        bh.insertHecho(newHecho);
    }
    archivo >> linea;
    string obj;
    if(linea == "Objetivo")
        archivo >> obj;
    archivo.close();
    return obj;
}


int main(int argc, char *argv[]){

    cout << argv[1]<<endl;
    leerBaseConocimiento(argv[1]);
    string objetivo = leerBaseHechos(argv[2]);

    for(auto& ract: bc.getBanco()){
        cout << ract->getNombre() << ' ';
        for(auto& hact: ract->getCausa()->getPartes()){
            cout << hact << ' ';
            if(ract->getCausa()->getTipo() == conj)
                cout<< 'y' << ' ';
            else if(ract->getCausa()->getTipo() == disy)
                cout<< 'o' << ' ';
        }
        cout << "entonces " << ract->getConsecuente()->getPartes().front()<< ", ";
        cout << "FC="<<ract->getFc()<<endl;
    }

    cout << argv[2]<< endl;
    for(auto& hact: bh.getBanco()){
        cout << hact.getPartes().front() << ", FC=" << hact.getFC()<<endl;
    }

    cout << "OBJETIVO: "<< objetivo;
}
