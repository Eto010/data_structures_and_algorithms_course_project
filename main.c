#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#pragma pack(push, 1) // Убираем отсупы в памяти
struct Book{ // Структура, в которую будет парситься запись из БД
    char aut[12];
    char tit[32];
    char pub[16];
    short int year;
    short int cop;
};
#pragma pack(pop)

void printBook(struct Book data) // Вывод данных о книге
{
    printf("Author: %.*s\nTitle: %.*s\nPublisher: %.*s\nYear of publication: %hd\nCount of pages: %hd\n", 12, data.aut, 32, data.tit, 16, data.pub, data.year, data.cop);
}

struct List{ // Структура списка, в который будет записываться БД
    struct Book data;
    struct List * next;
};

void printList(const struct List *head) { // Итеративно выводим список
    while (head != NULL) {
        printBook(head->data);
        head = head->next;
    }
}

struct List * appendList(struct List *head, char * aut, char * tit, char * pub, short int year, short int cop)
{
    struct List * temp = malloc(sizeof(struct List)); //Выделение дин. памяти
    if (!temp) {
        perror("Error in malloc, line 19");
        return head;
    }
    for(int i = 0; i < 12; i++)//Внесение данных в структуру
    {
        temp->data.aut[i] = aut[i];
    }
    for(int i = 0; i < 32; i++)
    {
        temp->data.tit[i] = tit[i];
    }
    for(int i = 0; i < 16; i++)
    {
        temp->data.pub[i] = pub[i];
    }
    temp->data.cop = cop;
    temp->data.year = year;
    temp->next = NULL;
    if (head == NULL)//Возврат головы списка
    {
        return temp;
    }
    else
    {
        struct List *headt = head;
        while (headt->next != NULL)
        {
            headt = headt->next;
        }
        headt->next = temp;
        return head;
    }
}

struct List *openBase(char * namefile)
{
    FILE *file = fopen(namefile, "rb");
    if (!file)
    {
        printf("Error in openBase in fopen\n");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    long count_books = file_size / sizeof(struct Book);
    if (count_books == 0)
    {
        printf("Error in openBase counter\n");
        fclose(file);
        return NULL;
    }
    struct List* head = NULL;
    struct Book temp;
    for (int i = 0; i < count_books; i++)
    {
        if (fread(&temp, sizeof(struct Book), 1, file) != 1) 
        {
            printf("Read error in openBase#%d\n", i+1);
            break;
        }
        head = appendList(head, temp.aut, temp.tit, temp.pub, temp.year, temp.cop);
    }
    fclose(file);
    return head;
}

void freeList(struct List *head) {
    while (head) {
        struct List *tmp = head;
        head = head->next;
        free(tmp);
    }
}

int main()
{
    struct List *base = openBase("testBase1.dat");
    printList(base);
    freeList(base);
    return 0;
}