#ifndef LISTA_ATOMICA_HPP
#define LISTA_ATOMICA_HPP

#include <atomic>
#include <cstddef>

template<typename T>
class ListaAtomica {
 private:
    struct Nodo {
        Nodo(const T &val) : _valor(val), _siguiente(nullptr) {}

        T _valor;
        Nodo *_siguiente;
    };

    std::atomic<Nodo *> _cabeza;

 public:
    ListaAtomica() : _cabeza(nullptr) {}

    ~ListaAtomica() {
        Nodo *n, *t;
        n = _cabeza.load();
        while (n) {
            t = n;
            n = n->_siguiente;
            delete t;
        }
    }

    void insertar(const T &valor) {
        Nodo* nuevo = new Nodo(valor);
        // En la primera iteracion nuevo->siguiente  = NULL
        // Si la lista es vacia y _cabeza sigue siendo NULL, entonces agregamos el nuevo nodo como cabeza
        // Si la lista no es vacia, se realiza una primer iteracion donde compare and exchange da falso
        // Y luego se realiza nuevo->siguiente = _cabeza
        // En caso de que compare and exchange es verdadero, se reemplaza la cabeza por el nuevo nodo
        // Quizas comernos una iteracion al pedo este mal
        while(!(_cabeza.compare_exchange_strong(nuevo->_siguiente, nuevo)));
    }

    T& operator[](size_t i) const {
        Nodo *n = _cabeza.load();
        for (size_t j = 0; j < i; j++) {
            n = n->_siguiente;
        }
        return n->_valor;
    }

    unsigned int longitud() const {
        Nodo *n = _cabeza.load();
        unsigned int cant = 0;
        while (n != nullptr) {
            cant++;
            n = n->_siguiente;
        }
        return cant;
    }

    struct iterator {
    private:
        ListaAtomica *_lista;

        typename ListaAtomica::Nodo *_nodo_sig;

        iterator(ListaAtomica<T> *lista, typename ListaAtomica<T>::Nodo *sig)
            : _lista(lista), _nodo_sig(sig) {}

    public:
        iterator &operator=(const typename ListaAtomica::iterator &otro) {
            _lista = otro._lista;
            _nodo_sig = otro._nodo_sig;
            return *this;
        }

        T& operator*() {
            return _nodo_sig->_valor;
        }

        iterator& operator++() { 
            _nodo_sig = _nodo_sig->_siguiente;
            return *this;
        }

        iterator operator++(int) { 
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const typename ListaAtomica::iterator &otro) const {
            return _lista->_cabeza.load() == otro._lista->_cabeza.load()
                && _nodo_sig == otro._nodo_sig;
        }

        bool operator!=(const typename ListaAtomica::iterator &otro) const {
            return !(*this == otro);
        }

        friend iterator ListaAtomica<T>::begin();
        friend iterator ListaAtomica<T>::end();
    };

    iterator begin() { 
        return iterator(this, _cabeza);
    }

    iterator end() { 
        return iterator(this, nullptr);
    }
};

#endif /* LISTA_ATOMICA_HPP */
