#ifndef DEADLINE_COLLISION_DETECTOR_H
#define DEADLINE_COLLISION_DETECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define FILE_NAME "deadlines.txt"

typedef struct Task {
    int id;
    char name[50];
    char subject[30];
    char type[15];
    int day, month, year;
    int hour, minute;
    long key;
    struct Task *next;
} Task;

typedef struct Node {
    Task *task;
    struct Node *left;
    struct Node *right;
} Node;

extern Task *head;
extern int nextId;
extern Task **heap;
extern int heapSize;

/* Shared date, input and task helpers */
void readLine(const char *prompt, char *buf, int size);
int readInt(const char *prompt, int low, int high);
int isLeap(int y);
int daysInMonth(int m, int y);
int validDate(int d, int m, int y);
long daysFromCivil(int y, int m, int d);
long makeKey(int d, int m, int y, int hh, int mm);
long nowKey(void);
void toLowerStr(const char *src, char *dst);
void readDate(Task *t);
void readTime(Task *t);
void readType(Task *t);
void readTaskFields(Task *t);
void appendTask(Task *t);
Task *findById(int id);
int countTasks(void);
void printHeader(void);
void printTask(Task *t, long now);

/* Person 1: task management */
void addDeadline(void);
void editDeadline(void);
void deleteDeadline(void);

/* Person 2: deadline analysis */
Node *bstInsert(Node *root, Task *t);
Node *buildBST(void);
void freeBST(Node *root);
void bstInorder(Node *root, long now);
int bstRange(Node *root, long low, long high, long now);
void heapPush(Task *t);
Task *heapPop(void);
void sortTasks(Task **a, int n, int ascending);
Task **collectTasks(int upcomingOnly, long now, int *count);
void viewAll(void);
void searchDeadline(void);
void nearestDeadlines(void);
void sortByUrgency(void);

/* Person 3: collision detection and persistence */
void checkCollisions(void);
void saveTasks(void);
void loadTasks(void);
void freeList(void);

#endif
