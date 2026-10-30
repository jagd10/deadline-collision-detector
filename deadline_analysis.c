#include "deadline_collision_detector.h"

Task **heap = NULL;
int heapSize = 0;

Node *bstInsert(Node *root, Task *t)
{
    if (root == NULL) {
        root = (Node *)malloc(sizeof(Node));
        if (root == NULL) {
            printf("Out of memory.\n");
            exit(1);
        }
        root->task = t;
        root->left = NULL;
        root->right = NULL;
        return root;
    }
    if (t->key < root->task->key)
        root->left = bstInsert(root->left, t);
    else
        root->right = bstInsert(root->right, t);
    return root;
}

Node *buildBST()
{
    Node *root = NULL;
    Task *p = head;
    while (p != NULL) {
        root = bstInsert(root, p);
        p = p->next;
    }
    return root;
}

void freeBST(Node *root)
{
    if (root == NULL)
        return;
    freeBST(root->left);
    freeBST(root->right);
    free(root);
}

void bstInorder(Node *root, long now)
{
    if (root == NULL)
        return;
    bstInorder(root->left, now);
    printTask(root->task, now);
    bstInorder(root->right, now);
}

int bstRange(Node *root, long low, long high, long now)
{
    int count = 0;
    if (root == NULL)
        return 0;
    if (root->task->key > low)
        count += bstRange(root->left, low, high, now);
    if (root->task->key >= low && root->task->key <= high) {
        printTask(root->task, now);
        count++;
    }
    if (root->task->key <= high)
        count += bstRange(root->right, low, high, now);
    return count;
}

void heapPush(Task *t)
{
    int i = heapSize++;
    while (i > 0 && heap[(i - 1) / 2]->key > t->key) {
        heap[i] = heap[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    heap[i] = t;
}

Task *heapPop()
{
    Task *top = heap[0];
    Task *last = heap[--heapSize];
    int i = 0, child;

    while (2 * i + 1 < heapSize) {
        child = 2 * i + 1;
        if (child + 1 < heapSize && heap[child + 1]->key < heap[child]->key)
            child++;
        if (heap[child]->key >= last->key)
            break;
        heap[i] = heap[child];
        i = child;
    }
    if (heapSize > 0)
        heap[i] = last;
    return top;
}

void sortTasks(Task **a, int n, int ascending)
{
    int i, j;
    Task *temp;

    for (i = 1; i < n; i++) {
        temp = a[i];
        j = i - 1;
        while (j >= 0 && ((ascending && a[j]->key > temp->key) || (!ascending && a[j]->key < temp->key))) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = temp;
    }
}

Task **collectTasks(int upcomingOnly, long now, int *count)
{
    int n = 0;
    Task *p;
    Task **arr;

    for (p = head; p != NULL; p = p->next)
        if (!upcomingOnly || p->key >= now)
            n++;
    *count = n;
    if (n == 0)
        return NULL;
    arr = (Task **)malloc(n * sizeof(Task *));
    if (arr == NULL) {
        printf("Out of memory.\n");
        exit(1);
    }
    n = 0;
    for (p = head; p != NULL; p = p->next)
        if (!upcomingOnly || p->key >= now)
            arr[n++] = p;
    return arr;
}

void viewAll()
{
    Node *root;
    long now = nowKey();

    if (head == NULL) {
        printf("\nNo deadlines stored.\n");
        return;
    }
    root = buildBST();
    printHeader();
    bstInorder(root, now);
    freeBST(root);
}

void searchDeadline()
{
    int choice;
    char buf[50];
    char lowKeyword[50];
    char lowName[50];
    char lowSubject[30];
    Task t;
    Node *root;
    Task *p;
    long now = nowKey();
    long dayStart;
    int found = 0;

    if (head == NULL) {
        printf("\nNo deadlines stored.\n");
        return;
    }
    printf("\n1. Search by due date\n");
    printf("2. Search by task name or subject\n");
    choice = readInt("Choose: ", 1, 2);

    if (choice == 1) {
        readDate(&t);
        dayStart = daysFromCivil(t.year, t.month, t.day) * 1440;
        root = buildBST();
        printHeader();
        found = bstRange(root, dayStart, dayStart + 1439, now);
        freeBST(root);
        if (!found)
            printf("No deadlines on that date.\n");
    } else {
        readLine("Enter keyword: ", buf, sizeof(buf));
        toLowerStr(buf, lowKeyword);
        for (p = head; p != NULL; p = p->next) {
            toLowerStr(p->name, lowName);
            toLowerStr(p->subject, lowSubject);
            if (strstr(lowName, lowKeyword) != NULL || strstr(lowSubject, lowKeyword) != NULL) {
                if (!found)
                    printHeader();
                printTask(p, now);
                found = 1;
            }
        }
        if (!found)
            printf("No matching deadlines found.\n");
    }
}

void nearestDeadlines()
{
    long now = nowKey();
    int n, count, i;
    Task *p;

    if (head == NULL) {
        printf("\nNo deadlines stored.\n");
        return;
    }
    n = 0;
    for (p = head; p != NULL; p = p->next)
        if (p->key >= now)
            n++;
    if (n == 0) {
        printf("\nNo upcoming deadlines.\n");
        return;
    }
    count = readInt("How many nearest deadlines to show? ", 1, 1000);
    if (count > n)
        count = n;

    heap = (Task **)malloc(n * sizeof(Task *));
    if (heap == NULL) {
        printf("Out of memory.\n");
        return;
    }
    heapSize = 0;
    for (p = head; p != NULL; p = p->next)
        if (p->key >= now)
            heapPush(p);

    printHeader();
    for (i = 0; i < count; i++)
        printTask(heapPop(), now);
    free(heap);
    heap = NULL;
    heapSize = 0;
}

void sortByUrgency()
{
    long now = nowKey();
    int n, i, choice;
    Task **arr;

    if (head == NULL) {
        printf("\nNo deadlines stored.\n");
        return;
    }
    printf("\n1. Most urgent first\n");
    printf("2. Least urgent first\n");
    choice = readInt("Choose: ", 1, 2);
    arr = collectTasks(0, now, &n);
    sortTasks(arr, n, choice == 1);
    printHeader();
    for (i = 0; i < n; i++)
        printTask(arr[i], now);
    free(arr);
}
