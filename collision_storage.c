#include "deadline_collision_detector.h"

void checkCollisions()
{
    long now = nowKey();
    int n, i, j, k;
    int found = 0;
    Task **arr = collectTasks(1, now, &n);

    if (n == 0) {
        printf("\nNo upcoming deadlines.\n");
        return;
    }
    sortTasks(arr, n, 1);

    printf("\n--- Same-day collisions ---\n");
    i = 0;
    while (i < n) {
        j = i;
        while (j < n && arr[j]->key / 1440 == arr[i]->key / 1440)
            j++;
        if (j - i >= 2) {
            found = 1;
            printf("WARNING: %d tasks on %02d/%02d/%04d\n", j - i,
                   arr[i]->day, arr[i]->month, arr[i]->year);
            for (k = i; k < j; k++)
                printf("   - %s (%s, %s) at %02d:%02d\n", arr[k]->name, arr[k]->subject,
                       arr[k]->type, arr[k]->hour, arr[k]->minute);
        }
        i = j;
    }
    if (!found)
        printf("None.\n");

    found = 0;
    printf("\n--- Three or more tasks within 24 hours ---\n");
    i = 0;
    while (i < n) {
        j = i;
        while (j < n && arr[j]->key - arr[i]->key <= 1440)
            j++;
        if (j - i >= 3) {
            found = 1;
            printf("WARNING: %d tasks between %02d/%02d/%04d %02d:%02d and %02d/%02d/%04d %02d:%02d\n",
                   j - i,
                   arr[i]->day, arr[i]->month, arr[i]->year, arr[i]->hour, arr[i]->minute,
                   arr[j - 1]->day, arr[j - 1]->month, arr[j - 1]->year,
                   arr[j - 1]->hour, arr[j - 1]->minute);
            for (k = i; k < j; k++)
                printf("   - %s (%s, %s)\n", arr[k]->name, arr[k]->subject, arr[k]->type);
            i = j;
        } else {
            i++;
        }
    }
    if (!found)
        printf("None.\n");
    free(arr);
}

void saveTasks()
{
    FILE *fp = fopen(FILE_NAME, "w");
    Task *p;

    if (fp == NULL) {
        printf("Could not open %s for writing.\n", FILE_NAME);
        return;
    }
    for (p = head; p != NULL; p = p->next)
        fprintf(fp, "%d|%s|%s|%s|%d|%d|%d|%d|%d\n", p->id, p->name, p->subject, p->type,
                p->day, p->month, p->year, p->hour, p->minute);
    fclose(fp);
    printf("Tasks saved to %s.\n", FILE_NAME);
}

void loadTasks()
{
    FILE *fp = fopen(FILE_NAME, "r");
    char line[300];
    Task *t;

    if (fp == NULL)
        return;
    while (fgets(line, sizeof(line), fp) != NULL) {
        t = (Task *)malloc(sizeof(Task));
        if (t == NULL)
            break;
        if (sscanf(line, "%d|%49[^|]|%29[^|]|%14[^|]|%d|%d|%d|%d|%d",
                   &t->id, t->name, t->subject, t->type,
                   &t->day, &t->month, &t->year, &t->hour, &t->minute) == 9
            && validDate(t->day, t->month, t->year)
            && t->hour >= 0 && t->hour <= 23 && t->minute >= 0 && t->minute <= 59) {
            t->key = makeKey(t->day, t->month, t->year, t->hour, t->minute);
            appendTask(t);
            if (t->id >= nextId)
                nextId = t->id + 1;
        } else {
            free(t);
        }
    }
    fclose(fp);
}

void freeList()
{
    Task *p;
    while (head != NULL) {
        p = head;
        head = head->next;
        free(p);
    }
}

int countTasks()
{
    int n = 0;
    Task *p = head;
    while (p != NULL) {
        n++;
        p = p->next;
    }
    return n;
}
