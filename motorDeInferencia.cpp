#include <algorithm>
#include "SBR.h"

using namespace std;

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
{
    list<reference_wrapper<Hecho>>::iterator it = hechos.begin();
    Hecho& h1 = *it;
    //it++;

    while(++it != hechos.end()){
        Hecho& h2 = *it;

        if(andor == conj)
            h1.setFC(min(h1.getFC(), h2.getFC()));
        else
            h1.setFC(max(h1.getFC(), h2.getFC()));

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
            // Solo b es cero → depende del signo de a
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
    if(allConditions < regla->getCausa()->getPartes().size() && regla->getCausa()->getTipo() != disy)
        return;
    
    auto hechos = buscarCondiciones(regla, baseH);  // recolecta las condiciones existentes en la base de datos.
    if(hechos.empty())
        return;
    
    // Aquí puede ir la un print
    shared_ptr<Hecho> rCondicion = regla->getCausa(); // saca la condición de la regla para modificarla después
    switch (regla->getCausa()->getTipo())
    {
    case conj:
        rCondicion->setFC(caso1(hechos, conj));
        break;
    case disy:
        rCondicion->setFC(caso1(hechos, disy));
        break;
    default:
        Hecho& hOri = hechos.front();
        rCondicion->setFC(hOri.getFC());
    }
    Hecho propagado;
    propagado.insert(regla->getConsecuente()->getNombre()); // Se saca el hecho derecho de la regla
    // caso 3
    propagado.setFC(regla->getFC() * max(float(0), rCondicion->getFC()));
    
    if(baseH.existeHecho(propagado)){   // Se aplica el caso 2 si el hecho ya existe
        //caso 2
        auto& hechoEnBase = baseH.getHecho(propagado.getNombre());
        hechoEnBase.setFC(caso2(propagado.getFC(), hechoEnBase.getFC() ));
    }
    else
        baseH.insertHecho(propagado);
}


bool verificar(Hecho meta, BH &bh, BC &bc)
{
    //bool verificado = false;

    // 1. ¿Meta ya está en la base de hechos?
    if (bh.existeHecho(meta))
        return true;

    // 2. Obtener reglas cuyos consecuentes igualan la meta
    auto reglasCC = bc.reglasConConsecuente(meta);  // CC

    // 3. Mientras haya reglas y no verificado
    while (!reglasCC->empty())
    {
        // Tomar una regla (Resolver(CC))
        auto regla = reglasCC->front();
        reglasCC->pop_front();  // Eliminar(R,CC)

        // 4. Extraer nuevos antecedentes
        auto causa = regla->getCausa();    // es un Hecho complejo (conj o disy)
        list<string> nuevasMetas = causa->getPartes(); // ExtraerAntecedentes(R)

        int verificados = 0;  // asumimos éxito, pero puede fallar luego

        // 5. Para cada nueva meta, verificarla recursivamente
        while (!nuevasMetas.empty())
        {
            string nombreMeta = nuevasMetas.front();
            nuevasMetas.pop_front();   // Eliminar(Nmet, NuevasMetas)

            Hecho subMeta;
            subMeta.insert(nombreMeta);   // construir Hecho simple

            // llamada recursiva
            if(verificar(subMeta, bh, bc))
                verificados++;
        }

        // 6. Insertamos las acciones de la regla en la base de hecho si aplica.
        despacharRegla(regla, bh, (size_t)verificados);
        /*if (verificado)
            bh.insertHecho(meta);*/
    }

    return bh.existeHecho(meta);
}
