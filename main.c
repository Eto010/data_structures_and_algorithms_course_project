#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
    FILE *file = fopen(namefile, "rb");// Открываем БД бинарником, из-за этого в WSL была ошибка кодировки
    if (!file)//
    {
        printf("Error in openBase in fopen\n");
        return NULL;
    }
    fseek(file, 0, SEEK_END);// Смотрим размер файла
    long file_size = ftell(file);
    rewind(file);
    long count_books = file_size / sizeof(struct Book);// Считаем количество записей о книгах в БД
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
        if (fread(&temp, sizeof(struct Book), 1, file) != 1) // Считываем запись о книге
        {
            printf("Read error in openBase#%d\n", i+1);
            break;
        }
        head = appendList(head, temp.aut, temp.tit, temp.pub, temp.year, temp.cop);// Добавляем запись в список
    }
    fclose(file);//
    return head;
}

void freeList(struct List *head) {//Очищаем список
    while (head) {
        struct List *tmp = head;
        head = head->next;
        free(tmp);
    }
}

void print20ElList(struct List *head)//Выводим только 20 элементов
{
    if (head == NULL) return;
    struct List * inhead = NULL;
    for(int i = 0; i < 20; i++)
    {
       inhead = appendList(inhead, head->data.aut, head->data.tit, head->data.pub, head->data.year, head->data.cop);
    }
    printList(inhead);
    freeList(inhead);
    return;
}

struct List retElAtInd(struct List* head, int n)//Функция для возвращения элемента списка по индексу
{
    for(int i = 0; i < n; i++)//Переходим к элементу с виртуальным номером n в списке
    {
        if (head->next == NULL)//Проверяем, не является ли следующий пустым
        {
            return *head;
        }
        else
        {
            head = head->next;//Переходим к следующему элементу списка, если он есть
        }
    }
    return * head;
}

struct sortQueue{//Структура очередей для сортировки
    int n;
    struct sortQueue * next;
};

struct sortQueue * appendSQ(struct sortQueue * head, int n)//Добавляем элемент в сортировочную очередь
{
    struct sortQueue * temp = malloc(sizeof(struct sortQueue));//Выделяем дин. память
    if(!temp)
    {
        perror("in appendSQ");
        return head;
    }
    temp->n = n;// Приравниваем содержимое к полученному значению
    if (head == NULL)
    {
        return temp; //Если очередь пустая - просто возвращаем темповую как голову 
    }
    else
    {
        struct sortQueue * headt = malloc(sizeof(struct sortQueue));//Иначе выделяем дин.память и добавляем темповую через доп.темповую в конец очереди
        if(!headt)
        {
            perror("in appendSQ");
            return head;
        }
        headt = head; 
        while(headt->next != NULL)
        {
            headt = headt->next;
        }
        headt->next = temp;
        free(headt);
        return head;
    }
}

void delSQ(struct sortQueue * head)//Удаление очереди из дин. памяти 
{
    while (head) {
        struct sortQueue *tmp = head;
        head = head->next;
        free(tmp);
    }
}

int * mergeQueue(struct sortQueue ** arr, int len, int * indArr) // Принимаем указатель на массив указателей на очереди, длину индексного массива и индексный массив соответственно
{
    int ctrlSum1 = 0;//Считаем и сверяем контрольные суммы
    for (int i = 0; i < 256; i++)
    {
        struct sortQueue * temp = arr[i];
        while(temp->next != NULL)
        {
            ctrlSum1 += temp->n;
            temp = temp->next;           
        }
    }
    int ctrlSum2 = 0;
    for (int i = 0; i < len; i++)
    {
        ctrlSum2 += indArr[i];
    }
    if (ctrlSum1 != ctrlSum2)//Если не равны возвращаем ошибку
    {
        perror("Ошибка сортировки: контрольные суммы не равны 1");
        return indArr;
    }
    int j = 0;
    for(int i = 0; i < 256; i++)//Поэлементно переносим содержимое очередей в индексный массив
    {
        struct sortQueue * temp = arr[i];
        while(temp->next != NULL)
        {
            temp->n = indArr[j];
            j++;
            temp = temp->next;         
        }
    }
    for (int i = 0; i < len; i++)//Пересчитываем и сверяем контрольные суммы
    {
        ctrlSum2 += indArr[i];
    }
    if (ctrlSum1 != ctrlSum2)
    {
        perror("Ошибка сортировки: контрольные суммы не равны 2");
        return indArr;
    }
    return indArr;
}

int * digitSort(struct List * head)
{
    int len = 0;
    while(head->next != NULL)
    {
        len++;
    }
    int * indArr = malloc(sizeof(int) * len);
    for(int i = 0; i < len; i++)
    {
        indArr[i] = i;
    }
    struct sortQueue ** bytes = malloc(sizeof(struct  sortQueue *) * 256);
    struct List *temp = head;
    for(int i = 3; i >= 0;  i--)
    {
        for(int j = 0; j < len; j++)
        {
            int code = (int)(retElAtInd(temp, indArr[j]).data.aut[i]);
            appendSQ(bytes[code], indArr[i]);
            if (temp->next == NULL)
            {
                perror("Конец списка раньше ожидаемого, ошибка в digitSort");
                return indArr;
            }
            temp = temp->next;
        }
        temp = head;
        indArr = mergeQueue(bytes, len, indArr);
        for(int j = 0; j < 256; j++)
        {
            delSQ(bytes[j]);
            bytes[j] = NULL; 
        }
    }
    for(int i = 3; i >= 0;  i--)
    {
        for(int j = 0; j < len; j++)
        {
            int code = (int)(retElAtInd(temp, indArr[j]).data.pub[i]);
            appendSQ(bytes[code], indArr[i]);
            if (temp->next == NULL)
            {
                perror("Конец списка раньше ожидаемого, ошибка в digitSort");
                return indArr;
            }
            temp = temp->next;
        }
        temp = head;
        indArr = mergeQueue(bytes, len, indArr);
        for(int j = 0; j < 256; j++)
        {
            delSQ(bytes[j]);
            bytes[j] = NULL; 
        }
    }
} 


int main()
{
    struct List *base = openBase("testBase1.dat");
    print20ElList(base);
    freeList(base);
    return 0;
}