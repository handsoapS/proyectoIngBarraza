#include <sstream>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "claseComando.hpp"

char** StlACpp(vector<string>& Parametros)
{
    char** argv = new char*[Parametros.size()+1];

	unsigned i;
    for (i=0; i<Parametros.size(); i++) {
        argv[i] = new char[Parametros[i].size()+1];
        strncpy(argv[i], Parametros[i].c_str(), Parametros[i].size()+1);
    }
    argv[i] = NULL;

	return argv;	
}

int claseComando::Ejecutar()
{
	bool Salir = false;

	system("clear");
	cout << endl << "Bienvenido ..." << endl << endl;
	
	do {
		string Comando, ArchivoIn, ArchivoOut;
		vector<string> Parametros, Pipe;
		
		MostrarPrompt(); 
		Comando = LeerComando();
		if(AnalizaLineaComandos(Comando, Parametros, ArchivoIn, ArchivoOut, Pipe))
			Salir = ProcesaComando(Comando, Parametros, ArchivoIn, ArchivoOut, Pipe);
	} while(!Salir);
	
	return 0;
}

void claseComando::MostrarPrompt()
{
	cout << "[" << setw(3) << ++_nComando << "] -> ";
}

string claseComando::LeerComando()
{
	string Linea;
	
	getline(cin, Linea);
	return Linea;
}

bool claseComando::AnalizaLineaComandos(string& Comando, vector<string>& Parametros, string& ArchivoIn, string& ArchivoOut, vector<string>& Pipe)
{
	stringstream Entrada(Comando); 
	string Elemento;
	vector<string>* Destino = &Parametros;
		
	while(Entrada >> Elemento) 
		switch(Elemento[0]) {
			case '<': 
			    Entrada >> ArchivoIn;
				break;			    
			case '>':
			    Entrada >> ArchivoOut;
			    break;
			case '|':
			    Destino = &Pipe;
			    break; 
			default:  
	    		Destino->push_back(Elemento); 
		}
	
	if(Parametros.size())
		Comando = Parametros[0]; 

	return Parametros.size();
}


bool claseComando::ProcesaComando(string Comando, vector<string>& Parametros, string ArchivoIn, string ArchivoOut, vector<string>& Pipe)
{

	if(Comando == "exit") return true;
	

	if(Pipe.size()) {
		int fds[2];
		pipe(fds); 
		
		if(!fork()) { 
			close(fds[0]); 
			dup2(fds[1], STDOUT_FILENO); 
			
			char** argv = StlACpp(Parametros); 
	 	    if(execvp(Comando.c_str(), argv) == -1) {
	 	    	cout << "Fallo al intentar ejecutar " << Comando << endl;
	 	    	exit(-1);
	 	    }
		}
		
		if(!fork()) {
			close(fds[1]);
			dup2(fds[0], STDIN_FILENO);
			
			char** argv = StlACpp(Pipe);
	 	    if(execvp(Pipe[0].c_str(), argv) == -1) {
	 	    	cout << "Fallo al intentar ejecutar " << Pipe[0] << endl;
	 	    	exit(-1);
	 	    }
		}
		
		close(fds[0]);
		close(fds[1]);
		
		wait(NULL);
		wait(NULL);
		
		return false;
	}
	
	int f = fork();
	if(!f) { 
		
        char** argv = StlACpp(Parametros);
        
		if(!ArchivoIn.empty()) {
			int fIn = open(ArchivoIn.c_str(), O_RDONLY);
			if(fIn == -1) {
				cout << "Fallo al abrir el archivo " << ArchivoIn << endl;
				exit(-1);
			}
			dup2(fIn, STDIN_FILENO);
		}
		
		if(!ArchivoOut.empty()) {
			int fOut = open(ArchivoOut.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if(fOut == -1) {
				cout << "Fallo al crear el archivo " << ArchivoOut << endl;
				exit(-1);
			}
			dup2(fOut, STDOUT_FILENO);
		}

 	    if(execvp(Comando.c_str(), argv) == -1) {
 	    	cout << "Fallo al intentar ejecutar " << Comando << endl;
 	    	exit(-1);
 	    }
	} else { 
		wait(NULL); 
	}
		
	return false; 
}

