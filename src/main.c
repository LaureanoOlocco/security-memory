#include <stdio.h>
#include <stdlib.h>
#include "memory.h" // Asegúrate de incluir el archivo de cabecera adecuado

int main() {

    // Establecer el método de asignación de memoria
    set_method(0); // 0 para primer ajuste, 1 para mejor ajuste

    // Asignar memoria usando malloc
    debug_message("Antes de malloc\n");

    int *arr = (int *)malloc(10 * sizeof(int));

    debug_message("Despues de malloc\n");
    
    if (arr == NULL) {
        printf("Error al asignar memoria\n");
        return 1;
    }

    // Inicializar y mostrar los valores
    for (int i = 0; i < 10; i++) {
        arr[i] = i * 10;
        printf("\narr[%d] = %d\n", i, arr[i]);
    }
    printf("\nLlamando a la funcion check_heap\n");
    // Verificar el estado del heap
    check_heap(arr);

    printf("\nLlamando a la funcion memory_usage para reportar el uso de memoria\n");
    // Reportar el uso de memoria
    memory_usage();

    // Reasignar memoria usando realloc
    arr = (int *)realloc(arr, 20 * sizeof(int));
    if (arr == NULL) {
        printf("Error al reasignarrr memoria\n");
        return 1;
    }

    // Inicializar los nuevos elementos
    for (int i = 10; i < 20; i++) {
        arr[i] = i * 10;
    }

    // Mostrar todos los valores
    for (int i = 0; i < 20; i++) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }

    // Verificar el estado del heap nuevamente
    check_heap(arr);

   
    //Reportar el uso de memoria
    memory_usage();

    // Asignar memoria usando calloc
    int *zeroed_arr = (int *)calloc(5, sizeof(int));
    if (zeroed_arr == NULL) {
        printf("Error al asignar memoria con calloc\n");
        return 1;
    }

    // Mostrar los valores inicializados a cero
    for (int i = 0; i < 5; i++) {
        printf("zeroed_arr[%d] = %d\n", i, zeroed_arr[i]);
    }

    // Liberar la memoria
    free(arr);
    free(zeroed_arr);

    // Verificar el estado del heap después de liberar
    check_heap(arr);

    // Reportar el uso de memoria final
    memory_usage();

    return 0;
}