#ifndef HMC_HPP
#define HMC_HPP

#include <mutex>
#include <array>
#include <atomic>
#include <string>
#include <vector>
#include <semaphore.h>
#include "ListaAtomica.hpp"

typedef std::pair<std::string, unsigned int> hashMapPair;

class HashMapConcurrente {
 public:
    static constexpr int cantLetras = 26;

    HashMapConcurrente();

    void incrementar(std::string clave);
    std::vector<std::string> claves();
    unsigned int valor(std::string clave);

    hashMapPair maximo();
    hashMapPair maximoParalelo(unsigned int cantThreads);

 private:
    ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];
    
    //Esto se puede hacer?
    std::mutex letraMutex[cantLetras];
    static unsigned int hashIndex(std::string clave);
    
    void maximoThread(std::vector <std::atomic<int>> &contadoresPorLetras, hashMapPair &respuesta
                     ,std::atomic<int> &cantDeThreadTerminados, sem_t &barrera, int cant_threads);

};

#endif  /* HMC_HPP */
