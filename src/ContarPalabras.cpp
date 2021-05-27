#include <iostream>
#include <time.h>
#include <unistd.h>


#include "HashMapConcurrente.hpp"
#include "CargarArchivos.hpp"
#define NS_PER_SECOND  1000000000
void computar_delta(struct timespec t1, struct timespec t2, struct timespec *td)
{
    td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
    td->tv_sec  = t2.tv_sec - t1.tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0)
    {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    }
    else if (td->tv_sec < 0 && td->tv_nsec > 0)
    {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}

int main(int argc, char **argv) {
    if (argc < 4) {
        std::cout << "Error: faltan argumentos." << std::endl;
        std::cout << std::endl;
        std::cout << "Modo de uso: " << argv[0] << " <threads_lectura> <threads_maximo>" << std::endl;
        std::cout << "    " << "<archivo1> [<archivo2>...]" << std::endl;
        std::cout << std::endl;
        std::cout << "    threads_lectura: "
            << "Cantidad de threads a usar para leer archivos." << std::endl;
        std::cout << "    threads_maximo: "
            << "Cantidad de threads a usar para computar máximo." << std::endl;
        std::cout << "    archivo1, archivo2...: "
            << "Archivos a procesar." << std::endl;
        return 1;
    }
    int cantThreadsLectura = std::stoi(argv[1]);
    int cantThreadsMaximo = std::stoi(argv[2]);

    std::vector<std::string> filePaths = {};
    for (int i = 3; i < argc; i++) {
        filePaths.push_back(argv[i]);
    }

    HashMapConcurrente hashMap{}; // = HashMapConcurrente();
    
    struct timespec start, finish, delta;
    clock_gettime(CLOCK_REALTIME, &start);
    cargarMultiplesArchivos(hashMap, cantThreadsLectura, filePaths);
    clock_gettime(CLOCK_REALTIME, &finish);
    computar_delta(start, finish, &delta);
    std::cout<< delta.tv_sec + double(delta.tv_nsec)/double(NS_PER_SECOND)<<" ";

    clock_gettime(CLOCK_REALTIME, &start);
    auto maximo = hashMap.maximoParalelo(cantThreadsMaximo);
    clock_gettime(CLOCK_REALTIME, &finish);
    computar_delta(start, finish, &delta);
    std::cout<< delta.tv_sec + double(delta.tv_nsec)/double(NS_PER_SECOND)<<std::endl;
    //std::cout << maximo.first << " " << maximo.second << std::endl;

    return 0;
}
