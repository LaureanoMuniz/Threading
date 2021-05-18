#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
// alternativamente #include <pthread.h>
#include <iostream>
#include <fstream>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    unsigned int index = hashIndex(clave);
    bool agregue = false;

    this->letraMutex[index].lock();
    
    ListaAtomica<hashMapPair>::iterator it = this->tabla[index]->begin(); 
    for(; it != this->tabla[index]->end(); ++(it)){
        if((*it).first == clave){
            (*it).second++;
            agregue = true;
        }
    }
    if(!agregue){
        this->tabla[index]->insertar({clave,1});
    }

    this->letraMutex[index].unlock();
}

std::vector<std::string> HashMapConcurrente::claves() {
    std::vector <std::string> clavesADevolver;

    for(int letra = 0; letra < cantLetras; letra++){
        ListaAtomica<hashMapPair>::iterator it = this->tabla[letra]->begin(); 
        for(; it != this->tabla[letra]->end(); ++(it)){
            clavesADevolver.push_back((*it).first);
        }
    }
    return clavesADevolver;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    int index = hashIndex(clave);
    ListaAtomica<hashMapPair>::iterator it = this->tabla[index]->begin();
    int respuesta = 0;

    for(; it != this->tabla[index]->end(); ++(it)){
        if((*it).first == clave){
            respuesta = (*it).second;
        }
    }

    return respuesta; 
}

hashMapPair HashMapConcurrente::maximo() {
    hashMapPair *max = new hashMapPair();
    max->second = 0;

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        this->letraMutex[index].lock();
        for (auto &p : *tabla[index]) {
            if (p.second > max->second) {
                max->first = p.first;
                max->second = p.second;
            }
        }
    }
    for(unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++){
        this->letraMutex[index].unlock();
    }
    return *max;
}



hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cant_threads) {
    // Completar (Ejercicio 3)
}

#endif
