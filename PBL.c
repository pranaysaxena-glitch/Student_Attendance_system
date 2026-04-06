#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "attendance.txt"
#define USER_FILE "users.txt"
#define MAX_DAYS 100

// ---------------- STRUCTURES ----------------
typedef struct
{
    char date[15];
    int present;
} Attendance;

typedef struct
{
    int id;
    char name[50];
    char course[50];
    int totalClasses;
    int attendedClasses;
    Attendance records[MAX_DAYS];
} Student;

typedef struct
{
    char username[30];
    char password[30];
} User;

Student *students = NULL;
int count = 0;

// ---------------- LOGIN SYSTEM ----------------
void createDefaultUser()
{
    FILE *fp = fopen(USER_FILE, "rb");

    if (fp == NULL)
    {
        fp = fopen(USER_FILE, "wb");
        User admin = {"admin", "1234"};
        fwrite(&admin, sizeof(User), 1, fp);
        printf("Default Admin Created!\nUsername: admin\nPassword: 1234\n");
    }

    fclose(fp);
}

int login()
{
    char username[30], password[30];
    User u;
    FILE *fp = fopen(USER_FILE, "rb");

    if (!fp)
        return 0;

    printf("\n===== LOGIN =====\n");
    printf("Username: ");
    scanf("%s", username);

    printf("Password: ");
    scanf("%s", password);

    while (fread(&u, sizeof(User), 1, fp))
    {
        if (strcmp(u.username, username) == 0 &&
            strcmp(u.password, password) == 0)
        {
            fclose(fp);
            printf("Login Successful!\n");
            return 1;
        }
    }

    fclose(fp);
    printf("Invalid Credentials!\n");
    return 0;
}

// ---------------- CORE FUNCTIONS ----------------
void addStudent()
{
    students = realloc(students, (count + 1) * sizeof(Student));

    printf("Enter ID: ");
    scanf("%d", &students[count].id);

    printf("Enter Name: ");
    scanf(" %[^\n]", students[count].name);

    printf("Enter Course: ");
    scanf(" %[^\n]", students[count].course);

    students[count].totalClasses = 0;
    students[count].attendedClasses = 0;

    count++;
    printf("Student added!\n");
}

void displayStudents()
{
    for (int i = 0; i < count; i++)
    {
        printf("%d | %s | %s | %d/%d\n",
               students[i].id,
               students[i].name,
               students[i].course,
               students[i].attendedClasses,
               students[i].totalClasses);
    }
}

void markAttendance()
{
    int id, present;
    char date[15];

    printf("Enter Student ID: ");
    scanf("%d", &id);

    printf("Enter Date (DD-MM-YYYY): ");
    scanf("%s", date);

    for (int i = 0; i < count; i++)
    {
        if (students[i].id == id)
        {

            printf("1. Present\n0. Absent\nEnter: ");
            scanf("%d", &present);

            int index = students[i].totalClasses;

            strcpy(students[i].records[index].date, date);
            students[i].records[index].present = present;

            students[i].totalClasses++;
            if (present == 1)
                students[i].attendedClasses++;

            printf("Attendance recorded!\n");
            return;
        }
    }

    printf("Student not found!\n");
}

void searchByName()
{
    char name[50];
    int found = 0;

    printf("Enter name to search: ");
    scanf(" %[^\n]", name);

    for (int i = 0; i < count; i++)
    {
        if (strstr(students[i].name, name))
        {
            printf("Found: %d | %s | %s\n",
                   students[i].id,
                   students[i].name,
                   students[i].course);
            found = 1;
        }
    }

    if (!found)
        printf("No match found!\n");
}

void deleteStudent()
{
    int id, index = -1;

    printf("Enter ID to delete: ");
    scanf("%d", &id);

    for (int i = 0; i < count; i++)
    {
        if (students[i].id == id)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        printf("Student not found!\n");
        return;
    }

    for (int i = index; i < count - 1; i++)
    {
        students[i] = students[i + 1];
    }

    count--;
    students = realloc(students, count * sizeof(Student));

    printf("Student deleted!\n");
}

void viewDateWiseAttendance()
{
    int id;

    printf("Enter Student ID: ");
    scanf("%d", &id);

    for (int i = 0; i < count; i++)
    {
        if (students[i].id == id)
        {
            printf("\nDate-wise Attendance for %s:\n", students[i].name);

            for (int j = 0; j < students[i].totalClasses; j++)
            {
                printf("%s : %s\n",
                       students[i].records[j].date,
                       students[i].records[j].present ? "Present" : "Absent");
            }
            return;
        }
    }

    printf("Student not found!\n");
}

void updateAttendance()
{
    int id;

    printf("Enter Student ID: ");
    scanf("%d", &id);

    for (int i = 0; i < count; i++)
    {
        if (students[i].id == id)
        {
            printf("Enter attended classes: ");
            scanf("%d", &students[i].attendedClasses);

            printf("Enter total classes: ");
            scanf("%d", &students[i].totalClasses);

            printf("Updated!\n");
            return;
        }
    }
}

void calculateAttendance()
{
    for (int i = 0; i < count; i++)
    {
        if (students[i].totalClasses == 0)
        {
            printf("%s: No data\n", students[i].name);
        }
        else
        {
            float percent = (students[i].attendedClasses * 100.0) /
                            students[i].totalClasses;

            printf("%s: %.2f%%\n", students[i].name, percent);
        }
    }
}

// ---------------- FILE HANDLING ----------------
void saveToFile()
{
    FILE *fp = fopen(FILE_NAME, "wb");
    if (!fp)
        return;

    fwrite(&count, sizeof(int), 1, fp);
    fwrite(students, sizeof(Student), count, fp);

    fclose(fp);
}

void loadFromFile()
{
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp)
        return;

    fread(&count, sizeof(int), 1, fp);
    students = malloc(count * sizeof(Student));
    fread(students, sizeof(Student), count, fp);

    fclose(fp);
}

// ---------------- MAIN ----------------
int main()
{
    int choice;

    createDefaultUser();

    if (!login())
    {
        printf("Exiting...\n");
        return 0;
    }

    loadFromFile();

    while (1)
    {
        printf("\n===== Student Attendance System =====\n");
        printf("1. Add Student\n");
        printf("2. Display Students\n");
        printf("3. Mark Attendance (Date-wise)\n");
        printf("4. Update Attendance\n");
        printf("5. Calculate Attendance %%\n");
        printf("6. Search by Name\n");
        printf("7. Delete Student\n");
        printf("8. View Date-wise Attendance\n");
        printf("9. Save to File\n");
        printf("10. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            addStudent();
            break;
        case 2:
            displayStudents();
            break;
        case 3:
            markAttendance();
            break;
        case 4:
            updateAttendance();
            break;
        case 5:
            calculateAttendance();
            break;
        case 6:
            searchByName();
            break;
        case 7:
            deleteStudent();
            break;
        case 8:
            viewDateWiseAttendance();
            break;
        case 9:
            saveToFile();
            break;
        case 10:
            saveToFile();
            printf("Exiting...\n");
            exit(0);
        default:
            printf("Invalid choice!\n");
        }
    }
}