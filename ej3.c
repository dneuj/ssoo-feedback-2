#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#define MAX_PATH 1024

int main(int argc, char *argv[])
{
    // Verificamos que se ha pasado los parámetros correctos
    // Para el apartado 1: directorio, archivo1, archivo2, nuevoarchivo3
    // Para el apartado 2: directorio
    if (argc != 5)
    {
        // Arrojamos mensaje de uso correcto si no se pasan 4 argumentos
        printf("Faltan parámetros. Debes proporcionar un directorio y tres archivos\n");
        printf("Ejemplo: %s /home/user/directorio archivo1 archivo2 nuevoarchivo3\n", argv[0]);
        return 1;
    }

    // Definimos variables para los nombres de archivos y directorio
    char *directorio = argv[1];
    char *archivo1 = argv[2];
    char *archivo2 = argv[3];
    char *archivo_salida = argv[4];

    // Definimos rutas completas para los archivos
    char ruta_completa1[MAX_PATH];
    char ruta_completa2[MAX_PATH];
    char ruta_completa_salida[MAX_PATH];

    // Construimos rutas completas de archivos a partir del directorio
    snprintf(ruta_completa1, sizeof(ruta_completa1), "%s/%s", directorio, archivo1);
    snprintf(ruta_completa2, sizeof(ruta_completa2), "%s/%s", directorio, archivo2);
    snprintf(ruta_completa_salida, sizeof(ruta_completa_salida), "%s/%s", directorio, archivo_salida);

    // Verificamos que los archivos existen
    struct stat stat_info;
    if (stat(ruta_completa1, &stat_info) == -1)
    {
        printf("Error: El archivo %s no existe en el directorio %s\n", archivo1, directorio);
        return 1;
    }

    if (stat(ruta_completa2, &stat_info) == -1)
    {
        printf("Error: El archivo %s no existe en el directorio %s\n", archivo2, directorio);
        return 1;
    }

    // Abrimos archivos para lectura
    FILE *fp1 = fopen(ruta_completa1, "r");
    if (fp1 == NULL)
    {
        printf("Error: No se puede abrir %s para lectura\n", ruta_completa1);
        return 1;
    }

    FILE *fp2 = fopen(ruta_completa2, "r");
    if (fp2 == NULL)
    {
        printf("Error: No se puede abrir %s para lectura\n", ruta_completa2);
        fclose(fp1);
        return 1;
    }

    // Creamos archivo de salida para escritura
    FILE *fpOut = fopen(ruta_completa_salida, "w");
    if (fpOut == NULL)
    {
        printf("Error: No se puede crear %s\n", ruta_completa_salida);
        fclose(fp1);
        fclose(fp2);
        return 1;
    }

    // Copiamos contenido del primer archivo
    int c;
    printf("Copiando contenido de %s...\n", archivo1);
    while ((c = fgetc(fp1)) != EOF)
    {
        fputc(c, fpOut);
    }

    // Copiamos contenido del segundo archivo
    printf("Copiando contenido de %s...\n", archivo2);
    while ((c = fgetc(fp2)) != EOF)
    {
        fputc(c, fpOut);
    }

    // Cerramos archivos
    fclose(fp1);
    fclose(fp2);
    fclose(fpOut);

    printf("Archivo %s creado exitosamente con el contenido de %s y %s\n",
           archivo_salida, archivo1, archivo2);

    // Apartado 2
    // Declaramos variables para leer el directorio
    DIR *dir;
    struct dirent *entrada;
    char ruta_completa_directorio[MAX_PATH];

    // Abrimos el directorio
    dir = opendir(directorio);
    if (dir == NULL)
    {
        printf("Error: No se puede abrir el directorio %s\n", directorio);
    }

    printf("\n========== LISTADO DE ARCHIVOS E INODOS ==========\n");
    printf("Directorio: %s\n\n", directorio);
    printf("%-30s %-15s %-15s\n", "Nombre del archivo", "Inodo", "Enlaces duros");
    printf("=====================================================\n");

    // Leemos el contenido del directorio
    while ((entrada = readdir(dir)) != NULL)
    {
        // Ignora los directorios . y ..
        if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0)
        {
            continue;
        }

        // Construimos la ruta completa del archivo
        snprintf(ruta_completa_directorio, sizeof(ruta_completa_directorio), "%s/%s", directorio, entrada->d_name);

        // Obtenemos información del archivo
        if (stat(ruta_completa_directorio, &stat_info) == -1)
        {
            printf("Error al obtener información de %s\n", entrada->d_name);
            continue;
        }
        // Mostramos nombre, inodo y número de enlaces duros
        printf("%-30s %-15lu %-15lu\n",
               entrada->d_name,
               (unsigned long)stat_info.st_ino,
               (unsigned long)stat_info.st_nlink);
    }
    printf("=====================================================\n\n");
    // Cerramos el directorio
    closedir(dir);
    return 0;
}
