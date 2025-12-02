#include "SBR.h"

using namespace std;

bool Regla::isCausa(Hecho& supCausa)
{
    int cuenta = 0;
    int partes = this->causa->getPartes().size();
    if(partes == 0)
        return false;
    
    for(auto&& act: supCausa.getPartes()){      //recorre cada una de las partes del hecho buscando coincidencia con las partes de la causa de esta regla.
        for(auto&& ract: causa->getPartes()){
            if(act != ract)
                continue;
            cuenta++;
            break;
        }
    }

    return cuenta == partes;
}

bool Regla::isConsecuencia(Hecho& supCons)
{
    int cuenta = 0;
    int partes = this->consecuente->getPartes().size();  
    if(partes == 0)
        return false;
    
    for(auto&& act: supCons.getPartes()){   //recorre cada una de las partes del hecho buscando coincidencia con las partes de la causa de esta regla.
        for(auto&& ract: consecuente->getPartes()){
            if(act != ract)
                continue;
            cuenta++;
            break;
        }
    }

    return cuenta == partes;
}

unique_ptr<list<shared_ptr<Regla>>> BC::reglasConConsecuente(Hecho& cons)
{
    auto lista = make_unique<list<shared_ptr<Regla>>>();
    for(auto& ract: this->banco){
        if(ract->isConsecuencia(cons))
            lista->push_back(ract);
    }
    return lista;
}

unique_ptr<list<shared_ptr<Regla>>> BC::reglasConCausa(Hecho& caus)
{
    auto lista = make_unique<list<shared_ptr<Regla>>>();
    for(auto& ract: this->banco){
        if(ract->isCausa(caus))
            lista->push_back(ract);
    }
    return lista;
}

Hecho& BH::getHecho(string nombreHecho)
{
    for(auto& hact: this->banco){
        if(hact.getTipo() != simple)
            continue;
        if(hact.getNombre() == nombreHecho)
            return hact;
    }

    throw runtime_error("Regla no encontrada. Antes compruebe que existe con existeHecho()");
}

bool BH::existeHecho(Hecho& h)
{
    if(h.getTipo() != simple)
        return false;

    string hnombre = h.getNombre();
    for(auto& hact: this->banco){
        if(hact.getNombre() == hnombre)
            return true;
    }
    return false;
}

