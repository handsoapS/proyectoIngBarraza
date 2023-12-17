#include <sstream>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <limits.h>
#include <pwd.h>

using namespace std;

class claseComando {
	int numeroComando;
	char dir[PATH_MAX];
    char hostName[HOST_NAME_MAX];
    uid_t userID;
    passwd *user;
public:
	claseComando() : numeroComando(0) {}
	void Ejecutar();
	
private:
	void MostrarPrompt();
	string LeerComando();
	bool AnalizaLineaComandos(string&, vector<string>&, string&, string&, vector<string>&);
	bool ProcesaComando(string, vector<string>&, string, string, vector<string>&);
	void EjecutarComandoSimple(string, vector<string>&, string, string);
	void EjecutarConTuberia(string, vector<string>&, vector<string>&);
};


// Usammos esta funcion para poder hacer una conversion de un vector a una matriz char* (arreglo de punteros de caracteres) 
char** VectorToChar(vector<string>& Parametros)
{
    char** argv = new char*[Parametros.size()+1];

	unsigned i;
    for (i=0; i<Parametros.size(); i++) { // Se recorre todo el vector de strings para posteriormente copiarlos en un espacio de memoria reservado en la nueva matriz argv
        argv[i] = new char[Parametros[i].size()+1];
        strncpy(argv[i], Parametros[i].c_str(), Parametros[i].size()+1);
    }
    argv[i] = NULL; // Esto es necesario para indicar el final de la lista de argumentos en el estilo utilizado por funciones como execvp.

	return argv; // Se devuelve un array de punteros
}


// Definimos al metodo ejecutar de la clase Comando el cual llama a los otros metodos 
// y es donde esta la iteracion mediante un do-while
void claseComando::Ejecutar()
{
	bool Salir = false; // Usamos una bandera para indicar cuando el programa acaba o no 

	system("clear");
	cout << endl << "Bienvenido!  " << endl << endl;
	
	do {
		string Comando, ArchivoIn, ArchivoOut;
		vector<string> Parametros, Tuberia;
		
		MostrarPrompt(); 
		Comando = LeerComando();
		if(AnalizaLineaComandos(Comando, Parametros, ArchivoIn, ArchivoOut, Tuberia)) Salir = ProcesaComando(Comando, Parametros, ArchivoIn, ArchivoOut, Tuberia);
	} while(!Salir);
}

string claseComando::LeerComando() // Recibimos una linea de comando desde el terminal
{
	string Linea;
	getline(cin, Linea);
	return Linea;
}

void claseComando::MostrarPrompt() //Mostramos el prompt personalizao
{
	gethostname(hostName, HOST_NAME_MAX); //Obtenemos el nombre del host
    userID = geteuid(); // Obtenemos el ID del usuario
    user = getpwuid(userID); // Obtenemos una dato de tipo estructura que se almacena en user
	getcwd(dir, PATH_MAX); // Obtenemos el directorio de trabajo actual
	cout<< "#" << setw(3)<<++numeroComando<<" # "; cout<<user->pw_name<<"@"<<hostName<<":"<<dir<< "$ ";
	fflush(stdout); // Vaciamos el buffer
}


bool claseComando::AnalizaLineaComandos(string& Comando, vector<string>& Parametros, string& ArchivoIn, string& ArchivoOut, vector<string>& Tuberia) {
    stringstream Entrada(Comando); // Crear un flujo de cadena para leer elementos individualmente
    string Elemento; // Variable para navegar a través de la línea de comando
    vector<string>* Destino = &Parametros; // Puntero para determinar dónde se colocarán los elementos analizados

    while (Entrada >> Elemento) { // Leer cada elemento de la línea de comando
        switch (Elemento[0]) { // Verificar el primer carácter del elemento
            case '<':
                Entrada >> ArchivoIn; // Si es '<', leer el siguiente elemento como ArchivoIn
                break;
            case '>':
                Entrada >> ArchivoOut; // Si es '>', leer el siguiente elemento como ArchivoOut
                break;
            case '|':
                Destino = &Tuberia; // Si es '|', cambiar el destino para los elementos a Tuberia
                break;
            default:
                Destino->push_back(Elemento); // Agregar el elemento al vector correspondiente (Parametros o Tuberia)
        }
    }

    if (Parametros.size()) {
        Comando = Parametros[0]; // Si hay elementos en Parametros, Comando será igual al primer elemento
    }

    return Parametros.size(); // Devolver la cantidad de elementos en Parametros como un valor booleano, igual a 0 retorna false, >0 retonar true
}


bool claseComando::ProcesaComando(string Comando, vector<string>& Parametros, string ArchivoIn, string ArchivoOut, vector<string>& Tuberia) {
    if (Comando == "salir") {
        fflush(stdout); // Limpia el búfer de salida
        return true; // El usuario quiere salir del intérprete
    } else if (Comando == "cd") {
        if (Parametros.size() < 2) {
            cout << "Recuerda que: cd <directorio>" << endl;
        } else {
            if (chdir(Parametros[1].c_str()) == -1) {
                cout << "Error con el directorio!" << endl;
            }
        }
        return false;
    }

    // Verifica si hay una tubería
    if (Tuberia.size()) {
        EjecutarConTuberia(Comando, Parametros, Tuberia);
        return false;
    }

    // Ejecución normal de comandos sin tubería
    EjecutarComandoSimple(Comando, Parametros, ArchivoIn, ArchivoOut);
    return false;
}

void claseComando::EjecutarComandoSimple(string Comando, vector<string>& Parametros, string ArchivoIn, string ArchivoOut) {
    pid_t pid = fork(); // Crea un nuevo proceso

    if (pid == -1) {
        cout << "Error al crear el proceso hijo." << endl;
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Este es el proceso hijo
        char** argv = VectorToChar(Parametros); // Prepara los argumentos para execvp()

        // Redirecciona la entrada y salida estándar si se proporcionan archivos
        if (!ArchivoIn.empty()) {
            // Abre el archivo de entrada y redirige la entrada estándar
            // Si hay un fallo, muestra un mensaje y termina el proceso hijo
            int fIn = open(ArchivoIn.c_str(), O_RDONLY);
            if (fIn == -1) {
                cout << "Fallo al abrir el archivo " << ArchivoIn << endl;
                exit(EXIT_FAILURE);
            }
            dup2(fIn, STDIN_FILENO);
        }

        if (!ArchivoOut.empty()) {
            // Abre el archivo de salida y redirige la salida estándar
            // Si hay un fallo, muestra un mensaje y termina el proceso hijo
            int fOut = open(ArchivoOut.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fOut == -1) {
                cout << "Fallo al crear el archivo " << ArchivoOut << endl;
                exit(EXIT_FAILURE);
            }
            dup2(fOut, STDOUT_FILENO);
        }

        // Ejecuta el comando proporcionado por execvp()
        // Si hay un fallo, muestra un mensaje y termina el proceso hijo
        if (execvp(Comando.c_str(), argv) == -1) {
            cout << "Fallo al intentar ejecutar " << Comando << endl;
            exit(EXIT_FAILURE);
        }
    } else {
        // Este es el proceso padre
        wait(NULL); // Espera a que el proceso hijo termine
    }
}

void claseComando::EjecutarConTuberia(string Comando1, vector<string>& Parametros1, vector<string>& Tuberia) {
    int fds[2];
    pipe(fds); // Crea un conducto

    // Crea un proceso hijo para ejecutar el primer comando
    if (!fork()) {
        close(fds[0]); // Cierra el extremo de lectura del conducto
        dup2(fds[1], STDOUT_FILENO); // Redirige la salida estándar al conducto

        char** argv1 = VectorToChar(Parametros1); // Prepara los argumentos para execvp()
        if (execvp(Comando1.c_str(), argv1) == -1) { // Ejecuta el primer comando
            cout << "Fallo al intentar ejecutar " << Comando1 << endl;
            exit(-1);
        }
    }

    // Crea un proceso hijo para ejecutar el segundo comando usando el conducto
    if (!fork()) {
        close(fds[1]); // Cierra el extremo de escritura del conducto
        dup2(fds[0], STDIN_FILENO); // Redirige la entrada estándar al conducto

        char** argv2 = VectorToChar(Tuberia); // Prepara los argumentos para execvp()
        if (execvp(Tuberia[0].c_str(), argv2) == -1) { // Ejecuta el segundo comando
            cout << "Fallo al intentar ejecutar " << Tuberia[0] << endl;
            exit(-1);
        }
    }

    close(fds[0]); // Cierra el conducto
    close(fds[1]); // Cierra el conducto
    wait(NULL); // Espera a que terminen ambos procesos hijos
    wait(NULL);
}

