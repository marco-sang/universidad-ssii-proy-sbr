#ifndef __SBR_H
#define __SBR_H

#include <list>
#include <iostream>
#include <memory>

using namespace std;

/*Enumerado para representar los tipos de hechos*/
enum tipoHecho {simple, conj, disy};

/*Clase para modelar el tipo de hecho*/
class Hecho {
	private:
		tipoHecho tipo;
		list<string> partes;
        float fc = 0;

    public:
		Hecho(tipoHecho tipoH = simple) { tipo = tipoH;}

        /*
        * Obtiene los subhechos que componen este hecho
        */
		list<string>& getPartes() {return partes;}

        /*
        * Obtiene el nombre de este hecho.
        * Retorna una cadena vacía si el hecho no es simple.
        */
       string getNombre()
       {
        if(this->tipo != simple)
            return "";
        return partes.front();
       }

        /*
        * Retorna el factor de certidumbre de este hecho.
        */
        float getFC() {return fc;}

        /*
        * Obtiene el tipo de hecho.
        */
        tipoHecho getTipo() {return tipo;}

        /*
        * Inserta un nuevo subhecho en las partes de este hecho.
        *
        * Se recomienta cambiar el tipo de hecho con setTipo después de ejecutar esta acción.
        */
        void insert(string parte)
        {
            partes.push_back(parte);
            partes.sort();
        }

        /*
        * Establece el tipo de Hecho.
        */
        void setTipo(tipoHecho t) {tipo = t;}

        /*
        * Establece el factor de certidumbre de este hecho.
        */
        void setFC(float factor) {fc = factor;}

};

/*Clase para representar las reglas de la base de conocimiento*/
class Regla{
    private:
        shared_ptr<Hecho> causa;
		shared_ptr<Hecho> consecuente;
		string nombre;
		float fc;

    public:
	    Regla(string name_rule,  shared_ptr<Hecho> h1, shared_ptr<Hecho> h2, float fc){
            causa = h1;
            consecuente = h2;
            nombre = name_rule;
            this->fc = fc;
        }

        /*
        * Retorna un puntero al hecho que representa la causa de esta regla.
        */
        shared_ptr<Hecho> getCausa() {return causa;}

        /*
        * Retorna un puntero al hecho que representa la consecuencia de esta regla.
        */
        shared_ptr<Hecho> getConsecuente() {return consecuente;}

        /*
        * Retorna el nombre que identifica esta regla.
        */
        string getNombre() {return nombre;}

        /*
        * Retorna el factor de certidumbre de esta regla.
        */
        float getFC() {return fc;}

        /*
        * Establece el factor de certidumbre de esta regla
        */
        void setFC(float n) {fc = n;}

        /*
        * Método que regresa true si la causa de esta regla es igual al
        * hecho que se pasa como parametro.
        */
        bool isCausa(Hecho& supCausa);

        /*
        * Método que regresa true si el consecuente de esta regla es igual al
        * hecho que se pasa como parametro.
        */
        bool isConsecuencia(Hecho& supCons);

};

/*
* Clase para representar la base de conocimiento.
*/
class BC {
	private:
		list<shared_ptr<Regla>> banco;

	public:
	    /*
	    * Retorna una lista con punteros a las reglas almacenadas en la base de conocimiento.
	    */
	    list<shared_ptr<Regla>>& getBanco() {return banco;}

	    /*
		* Inserta una nueva regla en la BC.
		*/
		void pushRegla(shared_ptr<Regla> R) {
            banco.push_back(R);
        }

        /*
        * Devuelve la regla con nombre = nombreRegla si existe en la BC.
        */
        shared_ptr<Regla> getRegla (string nombreRegla){
            for(auto& r: banco){
                if(r->getNombre()==nombreRegla)
                return r;
            }
            return nullptr;
        }

        /*
        * Método que busca en la base de conocimiento las reglas que tienen como
        * consecuente el hecho que se pasa como parámetro.
        */
        unique_ptr<list<shared_ptr<Regla>>> reglasConConsecuente(Hecho& cons);

        /*
        * Método que busca en la base de conocimiento las reglas
        * que tienen como causa el hecho que se pasa como parámetro.
        */
        unique_ptr<list<shared_ptr<Regla>>> reglasConCausa(Hecho& caus);
};

/*
* Clase para representar la base de hechos.
*
* Por defecto solo deja insertar objetos de tipo Hecho cuyo atributo "tipo" sea
* igual a simple.
*/
class BH {
    private:
        list<Hecho> banco;

    public:

        list<Hecho>& getBanco() {return banco;}

        /*
        * Inserta un nuevo hecho en la BH.
        */
        void insertHecho(Hecho newHecho) {
            if(newHecho.getPartes().size() > 0 && newHecho.getTipo() == simple)
                banco.push_back(newHecho);
        }

        /*
        * Regresa el hecho solicitado en el parametro
        */
        Hecho& getHecho(string nombreHecho);

        /*
        * Método comprobar si existe en la base de hechos el Hecho que se pasa como parámetro.
        */
        bool existeHecho(Hecho& h);
};


///// MÉTODOS DEL MOTOR DE INFERENCIA /////

/*
* Aplica las reglas de la base de conocimiento bc usando la información de la
* base de hechos bh para tratar de obtener el hecho objetivo meta.
*/
bool verificar(Hecho meta, BH &bh, BC &bc);

/*
* Indica si el hecho meta es alcanzable con la base de hechos bh y la base de
* conocimiento bc actuales. Regresa el factor de certidumbre en caso de que sí se pueda.
*/
float encadenamientoHaciaAtras(Hecho meta, BH &bh, BC &bc);

/*
* Si la regla aplica, guarda el hecho conseguido con esta regla en la base de hechos bh .
*
* allConditions sirve para indicar si todas las condiciones de esta regla fueron encontradas.
*/
void despacharRegla(shared_ptr<Regla> regla, BH& bh, size_t allConditions);

/*
* Optiene de la base de hechos "base" los hechos que necesita la regla "r" para aplicarse.
*/
list<reference_wrapper<Hecho>> buscarCondiciones(shared_ptr<Regla> r, BH& base);

/*
* Aplica el caso 1 de combinación de factores de certeza sobre la lista hechos.
* andor indica qué operación usar en los hechos.
*/
float caso1(list<reference_wrapper<Hecho>>& hechos, tipoHecho andor);

/*
* Aplica el caso 2 de combinación de factores de certeza.
* Recibe dos factores de certeza para hacer el calculo.
*/
float caso2(float a, float b);

#endif
