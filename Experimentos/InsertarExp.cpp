#include <iostream>
#include <time.h>
#include <unistd.h>
#include "ListaAtomica.hpp"
#include <thread> 
#include <vector>

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
void f(int cantidadInserciones, int modo, ListaAtomica <int> &l){
    for(int i = 0 ; i < cantidadInserciones; i++){
        l.insertar(1, modo);
    }
}
int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Error: faltan argumentos." << std::endl;
        std::cout << std::endl;
        std::cout << "Modo de uso: " << argv[0] << " <threads>" << std::endl;
        std::cout << "    " << "<Cantidad Inserciones>" << std::endl;
        std::cout << std::endl;
        return 1;
    }
    int cantThreads = std::stoi(argv[1]);
    int cantidadInserciones = std::stoi(argv[2]);
    ListaAtomica <int> l1,l2,l3;
    std::vector <std::thread> threads(cantThreads);
    struct timespec start, finish, delta;
    clock_gettime(CLOCK_REALTIME, &start);
    for(auto &t: threads){
        t = std::thread(f, cantidadInserciones/cantThreads, 1, std::ref(l1));
    }
    for(auto &t: threads){
        t.join();
    }
    clock_gettime(CLOCK_REALTIME, &finish);
    computar_delta(start, finish, &delta);
    std::cout<< delta.tv_sec + double(delta.tv_nsec)/double(NS_PER_SECOND)<<" ";

    

    clock_gettime(CLOCK_REALTIME, &start);
    for(auto &t: threads){
        t = std::thread(f, cantidadInserciones/cantThreads, 2, std::ref(l2));
    }
    for(auto &t: threads){
        t.join();
    }
    clock_gettime(CLOCK_REALTIME, &finish);
    computar_delta(start, finish, &delta);
    std::cout<< delta.tv_sec + double(delta.tv_nsec)/double(NS_PER_SECOND)<<" ";
    
    clock_gettime(CLOCK_REALTIME, &start);
    for(auto &t: threads){
        t = std::thread(f, cantidadInserciones/cantThreads, 3, std::ref(l3));
    }
    for(auto &t: threads){
        t.join();
    }
    clock_gettime(CLOCK_REALTIME, &finish);
    computar_delta(start, finish, &delta);
    std::cout<< delta.tv_sec + double(delta.tv_nsec)/double(NS_PER_SECOND)<<std::endl;    

    return 0;
}
