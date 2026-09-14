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

int globalCountPrintBook = 0;

void printBook(struct Book data) // Вывод данных о книге
{
    globalCountPrintBook++;
    printf("%d. Author: %.*s | Title: %.*s | Publisher: %.*s | Year of publication: %hd | Count of pages: %hd\n", globalCountPrintBook, 12, data.aut, 32, data.tit, 16, data.pub, data.year, data.cop);
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
    struct List * headt = head;
    for(int i = 0; i < 20 && headt != NULL; i++)
    {
       printBook(headt->data);
       headt = headt->next;
    }
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
    temp->next = NULL;
    if (head == NULL)
    {
        return temp; //Если очередь пустая - просто возвращаем темповую как голову 
    }
    else
    {
        struct sortQueue * headt = head;//Иначе добавляем темповую через доп.темповую в конец очереди
        while(headt->next != NULL)
        {
            headt = headt->next;
        }
        headt->next = temp;
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
        while(temp != NULL)
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
        while(temp != NULL)
        {
            indArr[j] = temp->n;
            j++;
            temp = temp->next;         
        }
    }
    ctrlSum2 = 0;
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
    struct List * headt = head; //Задаем темповую переменную для головы
    while(headt != NULL)
    {
        len++;// Находим длину списка
        headt = headt->next;
    }
    int * indArr = malloc(sizeof(int) * len); //Выделяем память
    for(int i = 0; i < len; i++)
    {
        indArr[i] = i; //Заполняем индексный массив
    }
    struct sortQueue ** bytes = malloc(sizeof(struct  sortQueue *) * 256); //Создаем массив из сортировочных очередей
    if(!bytes)
    {
        perror("in bytes"); //Проверяем выделение памяти
        return indArr;
    }
    for(int i = 0; i < 256; i++)
    {
        bytes[i] = NULL; //Заполняем поля массива сортировочных очередей пустыми указателям
    }
    struct List *temp = head; // Создаем еще одну темповую переменную
    for(int i = 11; i >= 0;  i--)// Сортировка по автору
    {
        for(int j = 0; j < len; j++)
        {
            int code = (int)(retElAtInd(head, indArr[j]).data.aut[i]) + 128; // Разбивка по кодам символов
            bytes[code] = appendSQ(bytes[code], indArr[j]); // Добавление в соответствующие очереди
            temp = temp->next;// Перевод указателя
        }
        temp = head;
        indArr = mergeQueue(bytes, len, indArr); //Слияние очередей
        
        for(int j = 0; j < 256; j++)
        {
            delSQ(bytes[j]);// Удаление старых значений
            bytes[j] = NULL; 
        }
    }
    for(int i = 15; i >= 0;  i--)// Тоже самое, но по издательству
    {
        for(int j = 0; j < len; j++)
        {
            int code = (int)(retElAtInd(head, indArr[j]).data.pub[i]) + 128;
            bytes[code] = appendSQ(bytes[code], indArr[j]);
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
    free(bytes); // Очистка массивов
    return indArr;
} 
struct Book * retElAtIndS(struct List* head, int n)//Функция для возвращения элемента списка по индексу
{
    for(int i = 0; i < n; i++)//Переходим к элементу с виртуальным номером n в списке
    {
        if (head->next == NULL)//Проверяем, не является ли следующий пустым
        {
            return &(head->data);
        }
        else
        {
            head = head->next;//Переходим к следующему элементу списка, если он есть
        }
    }
    return &(head->data);
}

struct SortList // Структура для сбора данных в упорядоченный список
{
    struct Book * data;
    struct SortList * next;
};

struct SortList * fillSortList(int * indArr, struct List *  old, int len) // Заполнение отсортированного списка
{
    struct SortList * temp1; // Создаем структуры
    struct SortList * new;
    temp1 = (struct SortList *)malloc(sizeof(struct SortList));
        if (temp1== NULL)
        {
            return NULL;
        }
    new = temp1;
    for (int i = 0; i < len; i++)// Переносим данные из индексного массива в упорядоченный список
    {
        temp1->next = (struct SortList *)malloc(sizeof(struct SortList));
        if (temp1->next == NULL)
        {
            return NULL;
        }
        temp1->data = (retElAtIndS(old, indArr[i]));
        temp1 = temp1->next;
        temp1->next = NULL;
    }
    return new;
}

void printSortList(struct SortList * a)
{
    while (a != NULL)
    {
        printBook(*(a->data));
        a = a->next;
    }
}
void print20ElSortList(struct SortList * a)
{
    int n = 0; 
    while ((a != NULL) && (n < 20))
    {
        printBook((*a->data));
        a = a->next;
        n++;
    }
}

void controlPrintSortedList(struct SortList * a) // Контролируемый вывод
{
    char i;
    while(1)
    {
        printf("Print all = Q\nPrint 20 elements = W\nEnd = E\n");
        scanf(" %c", &i);
        if(((int)i == (int)'e')||((int)i == (int)'E'))
        {
            break;
            return;
        }
        if(((int)i == (int)'q')||((int)i == (int)'Q'))
        {
            printSortList(a);
        }
        if(((int)i == (int)'w')||((int)i == (int)'W'))
        {
            print20ElSortList(a);
            for(int i = 0; i < 20; i++)
            {
                a = a->next;
            }
        }
    }
}

void binarySearch(struct List *head, int *indArr, char *sought, int low, int high) // Прототип бинарной сортировки
{
    if (low > high)
    {
        printf("Not search");
        return;                   
    }
    int mid = low + (high - low) / 2;

    struct Book *b = retElAtIndS(head, indArr[mid]);
    if (b == NULL)
        return;                  

    struct Book temp = *b;

    if ((int)temp.pub[0] == (int)sought[0])
    {
        if ((int)temp.pub[1] == (int)sought[1])
        {
            if ((int)temp.pub[2] == (int)sought[2])
            {
                printBook(temp);
                return;         
            }
            else if ((int)temp.pub[2] < (int)sought[2])
                binarySearch(head, indArr, sought, mid + 1, high);   
            else
                binarySearch(head, indArr, sought, low, mid - 1);    
        }
        else if ((int)temp.pub[1] < (int)sought[1])
            binarySearch(head, indArr, sought, mid + 1, high);
        else
            binarySearch(head, indArr, sought, low, mid - 1);
    }
    else if ((int)temp.pub[0] < (int)sought[0])
        binarySearch(head, indArr, sought, mid + 1, high);
    else
        binarySearch(head, indArr, sought, low, mid - 1);
}

int main()
{
    struct List *base = openBase("testBase1.dat");
    printf("Not sorted\n");
    print20ElList(base);
    int * indArr = digitSort(base);
    struct SortList * sort = fillSortList(indArr, base, 4000);
    globalCountPrintBook = 0;
    printf("Sorted\n");
    controlPrintSortedList(sort);
    int code0 = (int)(retElAtInd(base, indArr[0]).data.pub[0]);
    int code1 = (int)(retElAtInd(base, indArr[0]).data.pub[1]);
    int code2 = (int)(retElAtInd(base, indArr[0]).data.pub[2]);
    char sought[3] = {(char)code0, (char)code1, (char)code2};
    binarySearch(base, indArr, sought,  0, 4000);
    freeList(base);
    free(sort);
    free(indArr);
    return 0;
}