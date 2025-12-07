#include <algorithm>
#include <fstream>
#include "SBR.h"
#include "utils.h"

using namespace std;

extern ofstream logFile;

float encadenamientoHaciaAtras(Hecho meta, BH &bh, BC &bc)
{
    if (verificar(meta, bh, bc)){
        return bh.getHecho(meta.getNombre()).getFC();
    }
    else
        return 0.0f;
}

list<reference_wrapper<Hecho>> buscarCondiciones(shared_ptr<Regla> r, BH& base)
{
    auto hechos = list<reference_wrapper<Hecho>>();
    for(auto& nombreH: r->getCausa()->getPartes()){
        Hecho act;
        act.insert(nombreH);    // Creo un hecho simple
        if(base.existeHecho(act))   // Confirmo de que act se encuentre en la base de hecho
            hechos.push_back(base.getHecho(act.getNombre())); //Se guarda el hecho encontrado en base
    }
    return hechos;
}

float caso1(list<reference_wrapper<Hecho>>& hechos, tipoHecho andor)
{   // hechos debe tener solo hechos simples para que se impriman bien los mensajes.
    list<reference_wrapper<Hecho>>::iterator it = hechos.begin();
    Hecho& h1 = *it;

    logFile << h1.getNombre();   // Salida de log

    while(++it != hechos.end()){
        Hecho& h2 = *it;

        if(andor == conj){
            logFile << " y "; // Salida de log
            h1.setFC(min(h1.getFC(), h2.getFC()));
        }
        else{
            logFile << " o "; // Salida de log
            h1.setFC(max(h1.getFC(), h2.getFC()));
        }
        logFile << h2.getNombre(); // Salida de log
    }
    return h1.getFC();
}

float caso2(float a, float b)
{
    if (a*b < 0)   //de distinto signo
    {
        return (a + b)/(1-min(abs(a), abs(b)));
    }
    else if (a*b == 0.0f) { // Alguna de las partes es 0
        if (a == 0.0f) {
            if (b > 0)
                return a + b*(1-a);
            else
                return a + b*(1+a);
            }
        else if (b == 0.0f) {
            if (a > 0)
                return a + b*(1-a);
            else
                return a + b*(1+a);
        }
    }

    else if (a > 0 && b > 0) {
        return a + b*(1-a);
    }
    /*Si no sale por los casos anterires entonces es porque los dos elementos son menores que 0*/
    return a + b*(1+a);
}


void despacharRegla(shared_ptr<Regla> regla, BH& baseH, size_t allConditions)
{   // Si la regla tiene una causa compuesta pero no es disyunción ni dispone de todas las condiciones, entonces no se aplica.
    if(allConditions < regla->getCausa()->getPartes().size() && regla->getCausa()->getTipo() != disy){
        logFile << "     --No aplicable\n";
        return;
    }
    auto hechos = buscarCondiciones(regla, baseH);  // recolecta las condiciones existentes en la base de datos.
    if(hechos.empty()){
        logFile << "     --No aplicable\n";
        return;
    }
    logFile << "     --Aplicable\n";
    logFile << " Propagando " << regla->getNombre()<< "(FC="<< regla->getFC() << "):\n"; // Salida de log
    shared_ptr<Hecho> rCondicion = regla->getCausa(); // saca la condición de la regla para modificarla después
    switch (regla->getCausa()->getTipo())
    {
    case conj:
        logFile << "\tCaso 1: ";    //salida log
        rCondicion->setFC(caso1(hechos, conj));
        logFile << ", FC=" << rCondicion->getFC() << endl;  // salida log
        break;
    case disy:
        logFile << "\tCaso 1: ";    //salida log
        rCondicion->setFC(caso1(hechos, disy));
        logFile << ", FC=" << rCondicion->getFC() << endl;  // salida log
        break;
    default:
        Hecho& hOri = hechos.front();
        rCondicion->setFC(hOri.getFC());
    }
    Hecho propagado;
    propagado.insert(regla->getConsecuente()->getNombre()); // Se saca el hecho derecho de la regla
    // caso 3
    propagado.setFC(regla->getFC() * max(float(0), rCondicion->getFC()));
    logFile << "\tCaso 3: "<< propagado.getNombre()<< ", FC="<< propagado.getFC()<< endl ;    // salida de log
    if(baseH.existeHecho(propagado)){   // Se aplica el caso 2 si el hecho ya existe
        //caso 2
        auto& hechoEnBase = baseH.getHecho(propagado.getNombre());
        hechoEnBase.setFC(caso2(propagado.getFC(), hechoEnBase.getFC() ));
        logFile << "Caso 2: "<< hechoEnBase.getNombre()<< ", FC="<< hechoEnBase.getFC()<< endl;
    }
    else
        baseH.insertHecho(propagado);
}


bool verificar(Hecho meta, BH &bh, BC &bc)
{
    // 1. verificar si bh ya contiene la meta
    if (bh.existeHecho(meta)){
        logFile << "Hecho "<< meta.getNombre()<< " "<< "encontrado (FC="<< bh.getHecho(meta.getNombre()).getFC()<< ")\n";
        return true;
    }
    // 2. Obtener reglas cuyos consecuentes igualan la meta
    logFile << "Buscando reglas para calcular "<< meta.getNombre()<< endl;
    auto reglasCC = bc.reglasConConsecuente(meta);  // CC

    if(reglasCC->empty())
        logFile << "--No hay reglas para calcular "<< meta.getNombre()<< endl;
    else{
        logFile << "Añadiendo ";
        printReglas(*reglasCC, logFile);
        logFile << " al conjunto conflicto.\n";
    }
    // 3. Mientras haya reglas en CC
    while (!reglasCC->empty())
    {
        auto regla = reglasCC->front();
        reglasCC->pop_front();  // Eliminar(R,CC)
        logFile << "Buscando antecedentes de "<< regla->getNombre()<< endl;

        // 4. Extraer nuevos antecedentes
        auto causa = regla->getCausa();
        list<string> nuevasMetas = causa->getPartes(); // conjunto de hechos que necesita la regla actual.
        logFile << "Añadiendo {";
        printStrList(nuevasMetas, logFile);
        logFile << "} a Nuevas Metas\n";

        int verificados = 0;  // cuenta de los antecedentes encontrados en bh

        // 5. Para cada nueva meta, verificarla recursivamente
        while (!nuevasMetas.empty())
        {
            string nombreMeta = nuevasMetas.front();
            nuevasMetas.pop_front();

            Hecho subMeta;
            subMeta.insert(nombreMeta);   // construir Hecho simple

            // llamada recursiva
            if(verificar(subMeta, bh, bc))
                verificados++;
        }

        // 6. Insertamos las acciones de la regla en la base de hechos si aplica.
        logFile << "Despachando "<< regla->getNombre()<< endl;
        despacharRegla(regla, bh, (size_t)verificados);
    }

    return bh.existeHecho(meta);
}
