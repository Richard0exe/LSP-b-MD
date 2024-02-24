#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>

typedef struct file
{
    char *name;
    char *path;
    int size;
    char *md5;
    char *date;
} file;

typedef struct Node {
    file data;
    struct Node* next;
} Node;

typedef struct HashTable {
    int size;
    Node** array;
} HashTable;


void freeHashTable(HashTable* ht) {
    for (int i = 0; i < ht->size; i++) {
        Node* node = ht->array[i];
        while (node != NULL) {
            Node* temp = node;
            node = node->next;
            free(temp->data.name);
            free(temp->data.path);
            free(temp->data.date);
            free(temp);
        }
    }
    free(ht->array);
    free(ht);
}

HashTable* createHashTable(int size) {
    HashTable* ht = malloc(sizeof(HashTable));
    ht->size = size;
    ht->array = malloc(size * sizeof(Node*));
    for (int i = 0; i < size; i++) {
        ht->array[i] = NULL;
    }
    return ht;
}

unsigned long hashFunction(HashTable* ht, char* str) {
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % ht->size; // Return a value within the size of the array
}

void insertIntoHashTable(HashTable* ht, file f) {
    char key[255];
    sprintf(key, "%d/%s", f.size, f.name);
    unsigned long index = hashFunction(ht, key);
    Node* newNode = malloc(sizeof(Node));
    newNode->data = f;
    newNode->next = ht->array[index];
    ht->array[index] = newNode;
}

void insertIntoHashTableWithTime(HashTable* ht, file f) {
    char key[255];
    sprintf(key, "%d/%s/%s", f.size, f.name, f.date);
    unsigned long index = hashFunction(ht, key);
    Node* newNode = malloc(sizeof(Node));
    newNode->data = f;
    newNode->next = ht->array[index];
    ht->array[index] = newNode;
}


void printHashTable(HashTable* ht) {
    for (int i = 0; i < ht->size; i++) {
        Node* node = ht->array[i];
        if (node != NULL && node->next != NULL) { // Check if the linked list has more than one node
            while (node != NULL) {
                printf("%s\n",node->data.path);
                node = node->next;
            }
        }
    }
}


void meklet_failus(const char *pathdir, HashTable* ht, char *flag)
{
    // by recursion search all files in main directory
    char full_path[255] = "";
    char path[255] = "";

    strcpy(path, pathdir);

    if (path[strlen(path) - 1] != '/')
    {
        strcat(path, "/");
    }

    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        printf("Cannot open directory '%s'\n", path);
        return;
    }

    struct dirent *de;
    struct stat buf;

    while (de = readdir(dir))
    {

        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
        {
            continue;
        }

        if (strcmp(path, "./") == 0)
        {
            strcpy(full_path, de->d_name);
        }
        else
        {
            strcpy(full_path, path);
            strcat(full_path, de->d_name);
        }

        if (lstat(full_path, &buf) < 0)
        {
            printf("Kļūda saucot stat!\n");
            continue;
        }
        if (S_ISLNK(buf.st_mode))
            continue;
        if (S_ISDIR(buf.st_mode))
        {
            meklet_failus(full_path, ht, flag);
        }
        else
        {
            file f;
            f.name = strdup(de->d_name);
            f.path = strdup(full_path);
            f.size = buf.st_size;
            f.date = strdup(ctime(&buf.st_mtime));
            if (strcmp(flag, "-d") == 0)
            {
                insertIntoHashTableWithTime(ht, f);
            }
            else if (strcmp(flag, "0") == 0)
            {
                insertIntoHashTable(ht, f);
            }
           

        }
    }
}

//TODO
//Create a function to create hash table for all files in directory (hash table should be created with linked lists for each item)

//Create hash function which takes as key file object size and name(concatenate them and separate
//by "/" for example 1003/in.txt) and creates bucket

int main(int argc, char **argv)
{
    char *path = ".";
    char *flag = "";
    flag = argv[1];
    HashTable* ht = createHashTable(10000);

    if (argc == 2)
    {
        // search files with  different flags
        if (strcmp(flag, "-d") == 0)
        {
            printf("Search by name,size,date:\n");
            meklet_failus(path, ht, flag);
            printHashTable(ht);
        }
        else if (strcmp(flag, "-m") == 0)
        {
            printf("Search by MD5 values:\n");
        }
        else if (strcmp(flag, "-h") == 0)
        {
            printf("HELP:\n");
            printf("Usage: md_dir [OPTION] [OPTION]\n");
            printf("[OPTION] : 'none' - search by name and size\n");
            printf("[OPTION] : -d   - search by date, name and size\n");
            printf("[OPTION] : -m   - search by MD5 values\n");
            printf("[OPTION] : -h   - help\n");
            printf("\nImportant: you can use either 1 or 2 flags at the same time\n");
        }
        else
        {
            printf("Invalid flag use -h for help\n");
        }
    }
    else if (argc == 1)
    {
        printf("Search by name,size:\n");
        // search files only by name and size
        flag = "0";
        meklet_failus(path, ht,flag);
        printHashTable(ht);

    }
    freeHashTable(ht);
}