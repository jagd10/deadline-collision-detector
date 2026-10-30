#include "deadline_collision_detector.h"

int main()
{
    int choice;

    loadTasks();
    printf("Loaded %d deadline(s).\n", countTasks());

    do {
        printf("\n===== DEADLINE COLLISION DETECTOR =====\n");
        printf("1. Add a deadline\n");
        printf("2. View all deadlines\n");
        printf("3. Search for a deadline\n");
        printf("4. Edit a deadline\n");
        printf("5. Delete a deadline\n");
        printf("6. Check deadline collisions\n");
        printf("7. View nearest upcoming deadlines\n");
        printf("8. Sort tasks by urgency\n");
        printf("9. Save and exit\n");
        choice = readInt("Enter your choice: ", 1, 9);

        switch (choice) {
        case 1: addDeadline(); break;
        case 2: viewAll(); break;
        case 3: searchDeadline(); break;
        case 4: editDeadline(); break;
        case 5: deleteDeadline(); break;
        case 6: checkCollisions(); break;
        case 7: nearestDeadlines(); break;
        case 8: sortByUrgency(); break;
        case 9: saveTasks(); break;
        }
    } while (choice != 9);

    freeList();
    return 0;
}
