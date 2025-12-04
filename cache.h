#ifndef CACHE_H //si NO está definido el identificador CACHE_H
#define CACHE_H //define la macro CACHE_H
#include <iostream> //para imprimir y leer datos
#include <string> //para usar objetos de texto
#include <fstream>  //ifstream, ofstream
#include <map> //importa el contenedor std::map
#include <limits> //permite acceder a límites numéricos de los tipos (int, float, etc.) mediante std::numeric_limits
#include <vector> //contenedor std::vector
#include <algorithm> //incluye algoritmos genéricos como std::sort, std::find, std::max, etc
using namespace std;

template <class T> //crea una plantilla CacheManager que puede trabajar con objetos de cualquier clase
class CacheManager {
    int capacity; //guarda la capacidad máxima de la cache en memoria RAM
    int current_mru; //es un contador global que se incrementa cada vez que accedés (insert o get)
    map<string, pair<T, int>> cache_data; //el mapa que guarda todo <clave, <objeto, MRU>>

    bool write_file(string key, T obj) { //escribe en archivo la clave y el objeto
        ofstream file("cache_data.txt", ios::app); //append: escribe al final del archivo, sin borrar lo anterior
        if (!file.is_open()) return false; //si no se puede abrir el archivo devuelve false
        file << key << " ";
        obj.save(file); //usamos save() definido en T
        file << endl;
        file.close();
        return true;
    }

    bool read_file(string key, T &obj) { //lee del archivo y devuelve true si lo encontró
        ifstream file("cache_data.txt");
        if (!file.is_open()) return false; //si no se puede abrir el archivo devuelve false
        string k; //k almacenará las claves a medida que se lean
        while (file >> k) { //lee la siguiente palabra del archivo y la guarda en k
            T temp; //crea un objeto temporal del tipo de datos
            temp.load(file); //usamos load() definido en T
            if (k == key) { //compara la clave leída con la que buscamos
                obj = temp; //copia el objeto temporal en obj
                return true;
            }
        }
        return false; //si no hay coincidencias devuelve false
    }

    void evict_lru() { //elimina el LRU
        string lru_key; 
        int min_mru = numeric_limits<int>::max(); //arranca con el máximo entero posible, así cualquier MRU real será menor
        for (auto &p : cache_data) { //para cada elemento p dentro de cache_data, donde p es una referencia al par <clave, valor> que hay en el map, deduce el tipo automáticamente
            if (p.second.second < min_mru) { //si el MRU es menor que el minimo hasta ahora
                min_mru = p.second.second; //actualizamos con ese valor
                lru_key = p.first; //guardamos en lru la clave de este elemento
            }
        }
        cache_data.erase(lru_key);
    }

public:
    CacheManager(int cap) { //cuando el nombre de la función es el de la clase, es el constructor
        capacity = cap;
        current_mru = 0;
    }

    void insert(string key, T obj) { //insertar o actualizar un par tanto en la memoria (map) como en el archivo
        current_mru++; //se actualiza el mru
        if (cache_data.find(key) != cache_data.end()) { //si ya existe la clave
            cache_data[key] = {obj, current_mru}; //reemplaza por el nuevo objeto y mru
        } else { //si no existe
            if ((int)cache_data.size() >= capacity) { //si la cant de elementos es igual o mayor a la capacidad
                evict_lru();
            }
            cache_data[key] = {obj, current_mru}; ////reemplaza por el nuevo objeto y mru
        }
        write_file(key, obj); //siempre escribe en archivo
    }

    T get(string key) {
        current_mru++;
        if (cache_data.find(key) != cache_data.end()) { //si está en cache
            cache_data[key].second = current_mru; //actualiza mru
            return cache_data[key].first; //devuelve el objeto
        }
        T obj; //creamos un objeto vacio
        if (read_file(key, obj)) { //si no está en cache, lo busco en archivo
            if ((int)cache_data.size() >= capacity) { //si está en el archivo, chequea si la memoria está llena
                evict_lru();
            }
            cache_data[key] = {obj, current_mru}; //inserta el objeto en el map
            return obj; //devuelve el objeto
        }
        throw runtime_error("Clave no encontrada: " + key); //si no está en ningún lado
    }

    void show_cache() {
        cout << "CACHE (" << cache_data.size() << "/" << capacity << "):" << endl; //cant. actual de elementos/capacidad

        vector<pair<string, pair<T,int>>> items(cache_data.begin(), cache_data.end()); //copiamos a un vector para poder ordenar

    sort(items.begin(), items.end(),
        [](const pair<string, pair<T,int>> &a,
        const pair<string, pair<T,int>> &b) {
        return a.second.second > b.second.second;
    });

        for (auto &p : items) {
            cout << "Key: " << p.first << " [MRU=" << p.second.second << "] ";
            p.second.first.print();
        }
        cout << "-------------------------------------" << endl;
    }
};

#endif