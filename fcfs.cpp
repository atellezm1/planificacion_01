#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <limits>
using namespace std;

struct Proceso {
    string id;
    int llegada;
    int servicio;
    int comienzo;
    int finalizacion;
    int espera;
    int estancia;
    double estanciaNormalizada;
};

// Lee un entero validando que este dentro del rango permitido
int leerEntero(const string& mensaje, int minimo, int maximo) {
    int valor;
    while (true) {
        cout << mensaje;
        if (cin >> valor && valor >= minimo && valor <= maximo) return valor;
        cout << "Valor invalido. Intente de nuevo.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int main() {
    const int MAXIMO = numeric_limits<int>::max();
    vector<Proceso> procesos;

    int n = leerEntero("Numero de procesos (3 a 5): ", 3, 5);
    for (int i = 0; i < n; i++) {
        Proceso p{};
        cout << "\nProceso " << i + 1 << "\n";
        cout << "Identificador: ";
        cin >> p.id;
        p.llegada  = leerEntero("Tiempo de llegada (>= 0): ", 0, MAXIMO);
        p.servicio = leerEntero("Tiempo de servicio (> 0): ", 1, MAXIMO);
        procesos.push_back(p);
    }

    // Orden por llegada; stable_sort respeta el orden de ingreso en empates
    stable_sort(procesos.begin(), procesos.end(),
        [](const Proceso& a, const Proceso& b) { return a.llegada < b.llegada; });

    int tiempoActual = 0;
    double sumaEspera = 0, sumaEstancia = 0, sumaNorm = 0;
    vector<pair<int,int>> ocios;

    for (auto& p : procesos) {
        if (p.llegada > tiempoActual) ocios.push_back({tiempoActual, p.llegada});
        p.comienzo = max(p.llegada, tiempoActual);          // TC = max(TL, tiempo actual)
        p.finalizacion = p.comienzo + p.servicio;           // TF = TC + Ts
        p.espera = p.comienzo - p.llegada;                  // TE = TC - TL
        p.estancia = p.finalizacion - p.llegada;            // Tr = TF - TL
        p.estanciaNormalizada = (double)p.estancia / p.servicio;
        tiempoActual = p.finalizacion;
        sumaEspera += p.espera;
        sumaEstancia += p.estancia;
        sumaNorm += p.estanciaNormalizada;
    }

    cout << "\n" << left << setw(9) << "Proceso" << setw(5) << "TL" << setw(5) << "Ts"
         << setw(5) << "TC" << setw(5) << "TF" << setw(5) << "TE" << setw(5) << "Tr"
         << "Tr/Ts\n";
    for (const auto& p : procesos) {
        cout << left << setw(9) << p.id << setw(5) << p.llegada << setw(5) << p.servicio
             << setw(5) << p.comienzo << setw(5) << p.finalizacion << setw(5) << p.espera
             << setw(5) << p.estancia << fixed << setprecision(2)
             << p.estanciaNormalizada << "\n";
    }

    cout << fixed << setprecision(2);
    cout << "\nPromedio de espera: " << sumaEspera / n;
    cout << "\nPromedio de estancia: " << sumaEstancia / n;
    cout << "\nPromedio de estancia normalizada: " << sumaNorm / n << "\n";

    // Diagrama de Gantt en texto
    cout << "\nDiagrama de Gantt:\n";
    int t = 0;
    for (const auto& p : procesos) {
        if (p.comienzo > t) cout << "| OCIOSO (" << t << "-" << p.comienzo << ") ";
        cout << "| " << p.id << " (" << p.comienzo << "-" << p.finalizacion << ") ";
        t = p.finalizacion;
    }
    cout << "|\n";

    cout << "\nIntervalos de procesador ocioso:\n";
    if (ocios.empty()) cout << "Ninguno\n";
    for (const auto& o : ocios) cout << "[" << o.first << ", " << o.second << "]\n";

    return 0;
}
