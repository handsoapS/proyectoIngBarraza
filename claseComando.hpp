#ifndef CLASECOMANDO_H_
#define CLASECOMANDO_H_

#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

class claseComando {
	int _nComando;
	
public:
	claseComando() : _nComando(0) {}
	int Ejecutar();
	
private:
	void MostrarPrompt();
	string LeerComando();
	bool AnalizaLineaComandos(string&, vector<string>&, string&, string&, vector<string>&);
	bool ProcesaComando(string, vector<string>&, string, string, vector<string>&);
};

#endif
