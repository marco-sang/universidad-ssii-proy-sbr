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
        int fc = 0;

    public:
		Hecho(tipoHecho tipoH = simple) { tipo = tipoH;}
		//Hecho(Hecho &otro){ partes = otro.partes; }
        list<string>& getPartes() {return partes;}
        int getFC() {return fc;}
        tipoHecho getTipo() {return tipo;}
        void insert(string parte)
        {
            partes.push_back(parte);
            partes.sort();
        }

        void setTipo(tipoHecho t) {tipo = t;}
        void setFC(int factor) {fc = factor;}

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

            shared_ptr<Hecho> getCausa() {return causa;}
            shared_ptr<Hecho> getConsecuente() {return consecuente;}
            string getNombre() {return nombre;}
            float getFc() {return fc;}
            void setFc(float n) {fc = n;}

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

    class BC {
		private:
			list<shared_ptr<Regla>> banco;

		public:
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


#endif
