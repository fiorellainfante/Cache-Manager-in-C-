#include "cache.h"
using namespace std;

class Student { 
    int id; //un número identificador del estudiante -atributo
    int value; //otro número (nota o valor cualquiera) -atributo
    string data; //un texto (string) que guarda información (ejemplo: el nombre) -atributo

public:
    static const string class_name; //como es static, pertenece a la clase y no a cada objeto -miembro estatico de clase
    
    Student(int _id=0, int _value=0, string _data="") //constructor de la clase. Tiene valores por defecto
        : id(_id), value(_value), data(_data) {} //lista de inicialización para asignar los valores a los atributos internos

    void print() const { //muestra por pantalla los datos. const significa que este método no modifica el objeto(lo lee)
        cout << "Student: id=" << id 
            << ", value=" << value 
            << ", data=" << data << endl;
    }

    void save(ofstream &out) const { //guarda los datos en un archivo usando ofstream
        out << id << " " << value << " " << data; //escribe los tres atributos separados por espacios
    }

    void load(ifstream &in) { //lee los datos desde un archivo usando ifstream
        in >> id >> value >> data; //asigna lo que lee a los atributos id, value, data
    }
};

const string Student::class_name = "StudentClass"; //como class_name es estático, se inicializa fuera de la clase

int main() {
    ofstream clearFile("cache_data.txt", ios::trunc); //abre o crea el archivo .txt en modo truncar(si el archivo ya existia borra su contenido)
    clearFile.close(); //cierra el archivo

    CacheManager<Student> my_cache(3); //crea un objeto llamado my_cache que es un manejador de caché capaz de almacenar objetos de tipo Student, con una capacidad máxima de 3 elementos en memoria

    my_cache.insert("0", Student(0,22,"stud1")); //cada insert agrega o actualiza un objeto student tanto en la 
    my_cache.insert("1", Student(1,23,"stud2")); //memoria de la caché como en el archivo cache_data.txt
    my_cache.insert("2", Student(2,25,"stud3")); //clave "2" Student(id=2, value=25, data="stud3")
    my_cache.show_cache(); //imprime en pantalla el contenido actual de la caché en memoria.

    cout << "---- Insert extra (debe sacar LRU) ----" << endl;
    my_cache.insert("3", Student(3,29,"stud4"));
    my_cache.show_cache();

    cout << "---- Update ----" << endl;
    my_cache.insert("2", Student(22,222,"EZE")); 
    my_cache.show_cache();

    cout << "---- Get existente en cache ----" << endl;
    Student s1 = my_cache.get("2");
    s1.print();
    my_cache.show_cache();

    cout << "---- Get que estaba solo en archivo ----" << endl;
    try {
        Student s2 = my_cache.get("0");
        s2.print();
        my_cache.show_cache();
    } catch (runtime_error &e) {
        cout << e.what() << endl;
    }

    cout << "---- Get inexistente ----" << endl;
    try {
        Student s3 = my_cache.get("999");
        s3.print();
    } catch (runtime_error &e) {
        cout << e.what() << endl;
    }

    return 0;
}