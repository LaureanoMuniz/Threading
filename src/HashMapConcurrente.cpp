#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
// alternativamente #include <pthread.h>
#include <iostream>
#include <fstream>
// Inclui semaforos WARNING
#include <semaphore.h>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>()
        ;
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

void HashMapConcurrente::maximoThread(std::vector <std::atomic<int>> &contadoresPorLetras, hashMapPair &respuesta,
                                      std::atomic<int> &cantDeThreadTerminados, sem_t &barrera, int cant_threads){
    std::vector <unsigned int> mutexTomados;
    for(int i = 0; i < cantLetras; i++){
        if(contadoresPorLetras[i].fetch_add(1) == 0){
            //Soy el primer en llegar, tomo el lock
            mutexTomados.push_back(i);
            letraMutex[i].lock();
            //Calculo el maximo
            for (auto &p : *tabla[i]) {
                if (p.second > respuesta.second) {
                    respuesta.first = p.first;
                    respuesta.second = p.second;
                }
            }
        }
    }

    //Barrera
    if(cantDeThreadTerminados.fetch_add(1) == cant_threads - 1){
        sem_post(&barrera);
    }
    sem_wait(&barrera);
    sem_post(&barrera);
    
    for(auto letra:mutexTomados){
        letraMutex[letra].unlock();
    }
}


hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cant_threads) {
    std::vector<std::thread> threads(cant_threads);
    
    sem_t barrera;
    sem_init(&barrera, 0, 0);
    
    std::atomic<int> cantDeThreadTerminados;
    cantDeThreadTerminados = 0;
    
    std::vector<std::atomic<int> > contadoresPorLetra(cantLetras);
    for(unsigned int i = 0; i < cantLetras; i++){
        contadoresPorLetra[i] = 0;
    }
    std::vector<hashMapPair> respuestaDeThread(cant_threads, {"", 0});
    hashMapPair *max = new hashMapPair();
    max->second = 0;

    for(unsigned int i = 0; i < cant_threads; i++){
        threads[i] = std::thread(&HashMapConcurrente::maximoThread, this, std::ref(contadoresPorLetra), std::ref(respuestaDeThread[i]),
                                std::ref(cantDeThreadTerminados), std::ref(barrera), cant_threads);
    }
    
    for(unsigned int i = 0; i < cant_threads; i++){
        threads[i].join();
        if( max -> second < respuestaDeThread[i].second){
            max -> first = respuestaDeThread[i].first;
            max -> second = respuestaDeThread[i].second;
        }
    }
    sem_destroy(&barrera);
    return *max;
}

#endif
