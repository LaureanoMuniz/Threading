#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <thread>

#include "CargarArchivos.hpp"

int cargarArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath
) {
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual) {
        // Que pasa si dos threads abren el mismo archivo?, Creo que nada
        hashMap.incrementar(palabraActual);
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}

void cargarArchivosPorThread(
    std::vector<std::string> &filePaths, 
    HashMapConcurrente &hashMap,
    std::vector<std::atomic<int> > &contadoresPorArchivo)
    {
    int cantFiles = filePaths.size();
    for(int i = 0; i < cantFiles; i++){
        if(contadoresPorArchivo[i].fetch_add(1) == 0){
            cargarArchivo(hashMap,filePaths[i]);
        }
    }
}

void cargarMultiplesArchivos(
    HashMapConcurrente &hashMap,
    unsigned int cantThreads,
    std::vector<std::string> filePaths
) {
    int cantFiles = filePaths.size();
    std::vector<std::atomic<int> > contadoresPorArchivo(cantFiles);
    for(int i = 0; i < cantFiles; i++){
        contadoresPorArchivo[i] = 0;
    }

    std::vector <std::thread> threads(cantThreads);
    
    for(auto &t: threads){
        t = std::thread(cargarArchivosPorThread, std::ref(filePaths), std::ref(hashMap), 
                        std::ref(contadoresPorArchivo));
    }

    for(auto &t: threads){
        t.join();
    }
}

#endif
