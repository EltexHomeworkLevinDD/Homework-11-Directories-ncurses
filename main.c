#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file;
    char str[] = "Super mega ultima string\n";

    // Открытие на запись
    file = fopen("output.txt", "w");
    if (file == NULL) {
        printf("Error opening file\n");
        exit(-1);
    }

    fprintf(file, "%s", str);
    fclose(file);

    // Открытие на чтение
    file = fopen("output.txt", "r");
    if (file == NULL) {
        printf("Error opening file\n");
        exit(-1);
    }

    // Перемещаюсь в конец файла
    fseek(file, 0, SEEK_END);
    // Получаю текущую позицию в потоке (количество символов) 
    int size = ftell(file);

    // Считываю строку с конца а вывожу на экран
    char ch;
    for (int i = size - 1; i >= 0; i--) {
        // Смещаюсь назад
        fseek(file, i, SEEK_SET);
        ch = fgetc(file);
        printf("%c", ch);
    }

    fclose(file);

    return 0;
}
