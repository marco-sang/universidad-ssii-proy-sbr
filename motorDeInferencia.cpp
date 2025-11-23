#include "SBR.h"

using namespace std;

bool encadenamientoHaciaAtras(Hecho meta, BH &bh, BC &bc)
{
    // BH es inicial (asumimos que ya contiene hechos base)
    if (verificar(meta, bh, bc))
        return true;
    else
        return false;
}

unique_ptr<list<Hecho&>> buscarCondiciones(shared_ptr<Regla> r, BH& base)
{
    auto hechos = make_unique<list<Hecho&>>();
    for(auto& nombreH: r->getCausa()->getPartes()){
        Hecho act;
        act.insert(nombreH);
        if(base.existeHecho(act))
            hechos->push_back(base.getHecho(act.getPartes().front())); //Se guarda el hecho encontrado en base
    }
    return hechos;
}

void despacharRegla(shared_ptr<Regla> regla, BH& baseH, size_t allConditions)
{   // Si la regla tiene una causa compuesta pero no es disyunción ni dispone de todas las condiciones, entonces no se aplica.
    if(allConditions < regla->getCausa()->getPartes().size() && regla->getCausa()->getTipo() != disy)
        return;
    
    auto hechos = buscarCondiciones(regla, baseH);
    if(hechos->empty())
        return;
    
    switch (regla->getCausa()->getTipo())
    {
    case conj:
        /* code */
        break;
    case disy:
        /*todo*/
        break;
    
    default:
        break;
    }
    /*todo:*/
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
