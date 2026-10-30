# Deadline Collision Detector (C)

A command-line deadline manager that stores tasks, validates due dates/times, searches and sorts deadlines, finds upcoming deadlines, detects deadline collisions, and saves data to a text file.

## Build

```bash
gcc main.c task_management.c deadline_analysis.c collision_storage.c -o deadline_detector
```

Run:
- Linux/macOS: `./deadline_detector`
- Windows (MinGW): `deadline_detector.exe`

The program creates/uses `deadlines.txt` in the current working directory.

## Team contribution split

- **Person 1 — Task Management:** `task_management.c`
  - Input validation, date/time helpers, linked-list operations, add/edit/delete.
- **Person 2 — Deadline Analysis:** `deadline_analysis.c`
  - Binary search tree, min-heap, sorting, searching, nearest deadlines.
- **Person 3 — Collision Detection & Storage:** `collision_storage.c`
  - Collision detection, saving/loading tasks, task count and memory cleanup.

`deadline_collision_detector.h` is the shared interface used by all modules. `main.c` connects the menu to each module.

## GitHub collaboration

Each member should commit their assigned `.c` file on their own branch. The shared header and `main.c` should be added by the repository owner as integration files. Before merging, coordinate changes to the shared header so everyone uses the same function declarations.
