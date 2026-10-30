#include "deadline_collision_detector.h"

Task *head = NULL;
int nextId = 1;

void readLine(const char *prompt, char *buf, int size)
{
    int i, blank;
    int c;
    char *nl;

    while (1) {
        printf("%s", prompt);
        if (fgets(buf, size, stdin) == NULL)
            exit(0);
        nl = strchr(buf, '\n');
        if (nl != NULL)
            *nl = '\0';
        else {
            while ((c = getchar()) != '\n' && c != EOF)
                ;
        }
        blank = 1;
        for (i = 0; buf[i] != '\0'; i++) {
            if (buf[i] == '|')
                buf[i] = ' ';
            if (!isspace((unsigned char)buf[i]))
                blank = 0;
        }
        if (!blank)
            return;
        printf("Input cannot be empty.\n");
    }
}

int readInt(const char *prompt, int low, int high)
{
    char buf[50];
    char *end;
    long v;
    int c;
    char *nl;

    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL)
            exit(0);
        nl = strchr(buf, '\n');
        if (nl == NULL) {
            while ((c = getchar()) != '\n' && c != EOF)
                ;
        }
        v = strtol(buf, &end, 10);
        if (end != buf && (*end == '\n' || *end == '\0') && v >= low && v <= high)
            return (int)v;
        printf("Enter a number between %d and %d.\n", low, high);
    }
}

int isLeap(int y)
{
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

int daysInMonth(int m, int y)
{
    int d[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && isLeap(y))
        return 29;
    return d[m - 1];
}

int validDate(int d, int m, int y)
{
    if (y < 2000 || y > 2100)
        return 0;
    if (m < 1 || m > 12)
        return 0;
    if (d < 1 || d > daysInMonth(m, y))
        return 0;
    return 1;
}

long daysFromCivil(int y, int m, int d)
{
    long era, yoe, doy, doe;

    if (m <= 2)
        y--;
    era = (y >= 0 ? y : y - 399) / 400;
    yoe = y - era * 400;
    doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

long makeKey(int d, int m, int y, int hh, int mm)
{
    return daysFromCivil(y, m, d) * 1440 + hh * 60 + mm;
}

long nowKey()
{
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);
    return makeKey(lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900, lt->tm_hour, lt->tm_min);
}

void toLowerStr(const char *src, char *dst)
{
    int i;
    for (i = 0; src[i] != '\0'; i++)
        dst[i] = (char)tolower((unsigned char)src[i]);
    dst[i] = '\0';
}

void readDate(Task *t)
{
    char buf[50];
    int d, m, y;

    while (1) {
        readLine("Due date (DD/MM/YYYY): ", buf, sizeof(buf));
        if (sscanf(buf, "%d/%d/%d", &d, &m, &y) == 3 && validDate(d, m, y)) {
            t->day = d;
            t->month = m;
            t->year = y;
            return;
        }
        printf("Invalid date. Use DD/MM/YYYY, for example 25/10/2026.\n");
    }
}

void readTime(Task *t)
{
    char buf[50];
    int h, m;

    while (1) {
        readLine("Due time (HH:MM, 24-hour): ", buf, sizeof(buf));
        if (sscanf(buf, "%d:%d", &h, &m) == 2 && h >= 0 && h <= 23 && m >= 0 && m <= 59) {
            t->hour = h;
            t->minute = m;
            return;
        }
        printf("Invalid time. Use HH:MM, for example 17:30.\n");
    }
}

void readType(Task *t)
{
    int choice;
    printf("Task type:\n");
    printf("1. Assignment\n");
    printf("2. Lab Record\n");
    printf("3. Test\n");
    printf("4. Exam\n");
    choice = readInt("Choose type: ", 1, 4);
    if (choice == 1)
        strcpy(t->type, "Assignment");
    else if (choice == 2)
        strcpy(t->type, "Lab Record");
    else if (choice == 3)
        strcpy(t->type, "Test");
    else
        strcpy(t->type, "Exam");
}

void readTaskFields(Task *t)
{
    readLine("Task name: ", t->name, sizeof(t->name));
    readLine("Subject: ", t->subject, sizeof(t->subject));
    readType(t);
    readDate(t);
    readTime(t);
    t->key = makeKey(t->day, t->month, t->year, t->hour, t->minute);
}

void appendTask(Task *t)
{
    Task *p;
    t->next = NULL;
    if (head == NULL) {
        head = t;
        return;
    }
    p = head;
    while (p->next != NULL)
        p = p->next;
    p->next = t;
}

Task *findById(int id)
{
    Task *p = head;
    while (p != NULL) {
        if (p->id == id)
            return p;
        p = p->next;
    }
    return NULL;
}

void printHeader()
{
    printf("\n%-4s %-22s %-14s %-12s %-17s %s\n", "ID", "Task", "Subject", "Type", "Due", "Status");
    printf("--------------------------------------------------------------------------------------\n");
}

void printTask(Task *t, long now)
{
    long diff = t->key - now;
    char status[60];

    if (diff < 0) {
        diff = -diff;
        sprintf(status, "!! OVERDUE by %ldd %ldh %ldm", diff / 1440, (diff % 1440) / 60, diff % 60);
    } else {
        sprintf(status, "%ldd %ldh %ldm left", diff / 1440, (diff % 1440) / 60, diff % 60);
    }
    printf("%-4d %-22.22s %-14.14s %-12.12s %02d/%02d/%04d %02d:%02d  %s\n",
           t->id, t->name, t->subject, t->type,
           t->day, t->month, t->year, t->hour, t->minute, status);
}

void addDeadline()
{
    Task *t = (Task *)malloc(sizeof(Task));
    if (t == NULL) {
        printf("Out of memory.\n");
        return;
    }
    printf("\n--- Add a Deadline ---\n");
    readTaskFields(t);
    t->id = nextId++;
    appendTask(t);
    printf("Deadline added with ID %d.\n", t->id);
}

void editDeadline()
{
    int id;
    Task *t;
    long now = nowKey();

    if (head == NULL) {
        printf("\nNo deadlines stored.\n");
        return;
    }
    id = readInt("Enter ID of the deadline to edit: ", 1, 1000000);
    t = findById(id);
    if (t == NULL) {
        printf("No deadline with that ID.\n");
        return;
    }
    printf("\nCurrent details:");
    printHeader();
    printTask(t, now);
    printf("\nEnter new details:\n");
    readTaskFields(t);
    printf("Deadline updated.\n");
}

void deleteDeadline()
{
    int id;
    Task *p, *prev = NULL;

    if (head == NULL) {
        printf("\nNo deadlines stored.\n");
        return;
    }
    id = readInt("Enter ID of the deadline to delete: ", 1, 1000000);
    p = head;
    while (p != NULL && p->id != id) {
        prev = p;
        p = p->next;
    }
    if (p == NULL) {
        printf("No deadline with that ID.\n");
        return;
    }
    if (prev == NULL)
        head = p->next;
    else
        prev->next = p->next;
    free(p);
    printf("Deadline deleted.\n");
}
