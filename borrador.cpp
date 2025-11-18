El fichero conteniendo la BC tendrá la siguiente estructura (palabras utilizadas no sensitivas):
    Nº Reglas
    Identificador: Regla, factor
    …

- "Nº Reglas" indica la cantidad de reglas que compone la BC.
- El "Identificador" está dado como Ri (desde i=1 hasta el Nº Reglas)
- La "Regla" tendrá la forma "Si alfa Entonces beta" donde alfa puede ser un literal, una conjunción de
literales o una disyunción de literales, y beta es un literal; y "factor" indica el factor de certeza
asignado a la “regla”. Este se representa con FC=nº donde nº esta en [-1,1] y “FC=” palabra reservada.

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

std::vector<Regla> leerBaseConocimiento(const std::string& nombreFichero) {
    std::vector<Regla> reglas;
    std::ifstream archivo(nombreFichero);

    if (!archivo.is_open()) {
        std::cerr << "Error: no se pudo abrir el fichero " << nombreFichero << "\n";
        return reglas;
    }

    int numeroReglas = 0;
    std::string linea;

    // ---- Leer primera línea: Nº Reglas ----
    if (!std::getline(archivo, linea)) {
        std::cerr << "Error: fichero vacío o mal formato\n";
        return reglas;
    }
    numeroReglas = std::stoi(linea);

    // ---- Leer cada regla ----
    for (int i = 0; i < numeroReglas; ++i) {
        if (!std::getline(archivo, linea)) {
            std::cerr << "Error: reglas insuficientes en el fichero\n";
            break;
        }

        Regla r;
        std::stringstream ss(linea);

        // 1. Leer identificador: Ej "R1:"
        ss >> r.id;
        if (!r.id.empty() && r.id.back() == ':')
            r.id.pop_back(); // quitar ':'

        // 2. Recuperar el resto de la línea completa (regla + FC)
        std::string resto;
        std::getline(ss, resto);

        // ---- Separar "Si ... Entonces ..." de "FC=..." ----

        // Buscar FC=n
        size_t posFC = resto.find("FC=");
        if (posFC == std::string::npos) {
            std::cerr << "Error: la regla no contiene FC=\n";
            continue;
        }

        std::string reglaStr = resto.substr(0, posFC);
        std::string fcStr = resto.substr(posFC + 3); // tras "FC="

        r.factorCerteza = std::stod(fcStr);

        // ---- Extraer partes alfa y beta ----

        size_t posSi = reglaStr.find("Si");
        size_t posEntonces = reglaStr.find("Entonces");

        if (posSi == std::string::npos || posEntonces == std::string::npos) {
            std::cerr << "Error: formato incorrecto en la regla\n";
            continue;
        }

        r.condicion = reglaStr.substr(posSi + 2, posEntonces - (posSi + 2));
        r.conclusion = reglaStr.substr(posEntonces + 8);

        // Limpieza de espacios extra
        auto trim = [](std::string& s) {
            size_t start = s.find_first_not_of(" \t");
            size_t end = s.find_last_not_of(" \t");
            if (start == std::string::npos) s = "";
            else s = s.substr(start, end - start + 1);
        };

        trim(r.condicion);
        trim(r.conclusion);

        reglas.push_back(r);
    }

    return reglas;
}
