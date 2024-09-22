#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <string.h>
#include <ctime>  // Para la función time()

#define FRAME_SIZE 4      // Tamaño del marco en KB 8
#define NUM_PAGES 8       // Número de páginas en memoria 16 
#define MEMORY_SIZE 32    // Tamaño total de memoria en KB 128

struct Pagina {
    int numero_pagina;      // Número de la página
    int en_memoria;         // 1 si la página está cargada en memoria, 0 si no
    int marco_memoria;      // Número del marco en memoria si está cargada
    char proceso;           // Letra del proceso al que pertenece la página
};

struct TablaPaginas {
    struct Pagina paginas[NUM_PAGES];  // Array de páginas
};

std::queue<char> cola_espera;

void inicializarTabla(struct TablaPaginas *tabla) {
    for (int i = 0; i < NUM_PAGES; i++) {
        tabla->paginas[i].numero_pagina = i;
        tabla->paginas[i].en_memoria = 0;   // Ninguna página está en memoria inicialmente
        tabla->paginas[i].marco_memoria = -1; // -1 significa que no tiene un marco asignado
        tabla->paginas[i].proceso = '\0';   // Ningún proceso asignado
    }
}

bool cargarPagina(struct TablaPaginas *tabla, int pagina, int marco, char proceso) {
    // Verificar si el marco ya está ocupado por otro proceso
    for (int i = 0; i < NUM_PAGES; i++) {
        if (tabla->paginas[i].marco_memoria == marco && tabla->paginas[i].proceso != proceso) {
            printf("Error: El marco %d ya está ocupado por el proceso %c\n", marco, tabla->paginas[i].proceso);
            return false;
        }
    }

    if (tabla->paginas[pagina].en_memoria) {
        return false; // La página ya está cargada
    }
    tabla->paginas[pagina].en_memoria = 1;  // Marca la página como cargada en memoria
    tabla->paginas[pagina].marco_memoria = marco; // Asigna el número de marco
    tabla->paginas[pagina].proceso = proceso; // Asigna el proceso
    printf("Pagina %d del proceso %c cargada en el marco %d\n", pagina, proceso, marco);
    return true;
}

void accederPagina(struct TablaPaginas *tabla, int pagina) {
    if (pagina >= NUM_PAGES) {
        printf("Error: numero de página %d fuera de rango\n", pagina);
        return;
    }

    if (tabla->paginas[pagina].en_memoria) {
        printf("Acceso exitoso a la pagina %d en el marco %d\n", 
               pagina, tabla->paginas[pagina].marco_memoria);
    } else {
        printf("Fallo de pagina al acceder a la pagina %d\n", pagina);
    }
}

void mostrarTabla(struct TablaPaginas *tabla) {
    printf("\nEstado de la tabla de paginas:\n");
    printf("Pagina\tEn Memoria\tMarco\tProceso\n");
    for (int i = 0; i < NUM_PAGES; i++) {
        printf("%d\t%s\t\t%d\t%c\n", 
               tabla->paginas[i].numero_pagina, 
               tabla->paginas[i].en_memoria ? "Si" : "No", 
               tabla->paginas[i].marco_memoria,
               tabla->paginas[i].proceso);
    }
}

void terminarProceso(struct TablaPaginas *tabla, char proceso) {
    for (int i = 0; i < NUM_PAGES; i++) {
        if (tabla->paginas[i].proceso == proceso) {
            tabla->paginas[i].en_memoria = 0;
            tabla->paginas[i].marco_memoria = -1;
            tabla->paginas[i].proceso = '\0';
            printf("Pagina %d del proceso %c liberada\n", i, proceso);
        }
    }
}

void manejarColaEspera(struct TablaPaginas *tabla) {
    while (!cola_espera.empty()) {
        char proceso = cola_espera.front();
        cola_espera.pop();
        printf("Proceso %c sacado de la cola de espera\n", proceso);
        // Intentar cargar el proceso en memoria
        for (int i = 0; i < NUM_PAGES; i++) {
            if (!tabla->paginas[i].en_memoria) {
                cargarPagina(tabla, i, i, proceso);
                break;
            }
        }
    }
}

void mostrarColaEspera() {
    printf("\nProcesos en espera:\n");
    std::queue<char> copia_cola = cola_espera; // Copia de la cola para no modificar la original
    while (!copia_cola.empty()) {
        char proceso = copia_cola.front();
        copia_cola.pop();
        printf("Proceso %c\n", proceso);
    }
    printf("Tamaño de la cola de espera: %lu\n", cola_espera.size());
}

int main() {
    srand(time(0));  // Inicializa la semilla para números aleatorios
    struct TablaPaginas tabla;  // Declaración de la tabla de páginas
    inicializarTabla(&tabla);   // Inicializa la tabla
    int pagina, marco;
    char proceso;
    
    // Simulación automática con asignación aleatoria
    for (int i = 0; i < NUM_PAGES; i++) {
        pagina = rand() % NUM_PAGES;
        marco = rand() % NUM_PAGES;
        proceso = 'A' + (rand() % 4); // Asigna procesos aleatorios A, B, C, D
        if (!cargarPagina(&tabla, pagina, marco, proceso)) {
            cola_espera.push(proceso);
        }
    }
    
    mostrarTabla(&tabla);
    mostrarColaEspera();
    manejarColaEspera(&tabla);
    mostrarTabla(&tabla);
    system("pause");
    return 0;
}
