#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 10   // Número de posiciones del array compartido
#define NHP 35 // Número de hebras productoras

// Tipo de datos para estructura de identificador y valor
typedef struct
{
    int id;
    int val;
} hebval_t;

// Array para almacenar N estructuras de tipo hebval_t
hebval_t arrayCompartido[N];

// Puntero de inserción compartido
int punteroInsercion = 0;

// Bandera para indicar si el array está lleno
int arrayLleno = 0;

// Sincronización
pthread_mutex_t mutexProductor = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condConsumidor = PTHREAD_COND_INITIALIZER;

// Función para hebra productora
void *hebraProductora(void *arg)
{
    hebval_t *estructura = (hebval_t *)arg;

    pthread_mutex_lock(&mutexProductor);

    // Verificar si el array está lleno (no permitir inserciones)
    if (arrayLleno)
    {
        printf("Hebra %d: Array lleno, no se puede insertar\n", estructura->id);
        pthread_mutex_unlock(&mutexProductor);
        return NULL;
    }

    // Copiar la estructura en la posición actual del puntero de inserción
    arrayCompartido[punteroInsercion] = *estructura;
    printf("Hebra %d: Elemento insertado en posición %d (valor: %d)\n",
           estructura->id, punteroInsercion, estructura->val);

    // Avanzar el puntero de inserción
    punteroInsercion++;

    // Detectar si el array está lleno
    if (punteroInsercion >= N)
    {
        arrayLleno = 1;
        punteroInsercion = 0;
        printf("Hebra %d: Array lleno. Se resetea puntero y se activa sincronización\n", estructura->id);
        // Activar mecanismo de sincronización para la hebra consumidora
        pthread_cond_signal(&condConsumidor);
    }

    pthread_mutex_unlock(&mutexProductor);
    return NULL;
}

// Función para hebra consumidora
void *hebraConsumidora(void *arg)
{
    pthread_mutex_lock(&mutexProductor);

    // Esperar a que el array esté lleno
    while (!arrayLleno)
    {
        pthread_cond_wait(&condConsumidor, &mutexProductor);
    }

    // Mostrar todos los elementos del array
    printf("\nContenido del array compartido:\n");
    printf("[ ");
    for (int i = 0; i < N; i++)
    {
        printf("[id %d, val %d]", arrayCompartido[i].id, arrayCompartido[i].val);
        if (i < N - 1)
        {
            printf(", ");
        }
    }
    printf(" ]\n");

    printf("\nHebra Consumidora: Fin de la ejecución de todas las hebras\n");

    pthread_mutex_unlock(&mutexProductor);
    return NULL;
}

int main()
{
    // 1) Declarar arrays para las hebras
    pthread_t hebrasProductoras[NHP];
    pthread_t hebraConsumidoraTh;

    // Array de estructuras para pasar como argumentos a las hebras productoras
    hebval_t estructurasProductoras[NHP];

    // 2) Inicializar todas las estructuras con -1 en ambos campos
    for (int i = 0; i < N; i++)
    {
        arrayCompartido[i].id = -1;
        arrayCompartido[i].val = -1;
    }

    printf("Array inicializado con -1 en todos los campos\n\n");

    // 3) Lanzar las NHP hebras productoras
    for (int i = 0; i < NHP; i++)
    {
        // Inicializar estructura con valor secuencial
        estructurasProductoras[i].id = i;
        estructurasProductoras[i].val = i;

        // Crear la hebra productora
        pthread_create(&hebrasProductoras[i], NULL, hebraProductora, (void *)&estructurasProductoras[i]);
    }

    printf("Se han lanzado %d hebras productoras\n\n", NHP);

    // 4) Lanzar la hebra consumidora
    pthread_create(&hebraConsumidoraTh, NULL, hebraConsumidora, NULL);

    printf("Se ha lanzado la hebra consumidora\n\n");

    // 5) Esperar por la terminación de todas las hebras productoras
    for (int i = 0; i < NHP; i++)
    {
        pthread_join(hebrasProductoras[i], NULL);
    }

    // Esperar por la terminación de la hebra consumidora
    pthread_join(hebraConsumidoraTh, NULL);
    printf("Todas las hebras han terminado\n");
    return 0;
}
