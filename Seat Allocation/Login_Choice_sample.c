#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PREFERENCES 8
#define MAX_NAME_LENGTH 50
#define MAX_STUDENTS 2

struct Student {
    int applicationNumber;
    char password[4]; // Last 3 digits of application number + null terminator
    char choices[MAX_PREFERENCES][MAX_NAME_LENGTH];
    int numChoices;
    int mainsRank;
    int advancedRank;
};

struct Admin {
    int adminNumber;
    char password[4]; // Last 3 digits of admin number + null terminator
};

int isApplicationNumberPresent(int applicationNumber) {
    FILE *file = fopen("student_rank.csv", "r");
    if (file == NULL) {
        printf("Error opening file: student_rank.csv\n");
        return 0;
    }

    int currentAppNumber;
    char line[100]; // Assuming a maximum line length of 100 characters

    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%d", &currentAppNumber) == 1) {
            if (currentAppNumber == applicationNumber) {
                fclose(file);
                return 1; // Application number found
            }
        }
    }

    fclose(file);
    return 0; // Application number not found
}

void readRanksFromFile(struct Student students[], int numStudents, const char *filename, int applicationNumber) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    int currentAppNumber;
    int mainsRank;
    int advancedRank;
    char line[100]; // Assuming a maximum line length of 100 characters

    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%d, %d, %d", &currentAppNumber, &mainsRank, &advancedRank) == 3) {
            if (currentAppNumber == applicationNumber) {
                students[numStudents].mainsRank = mainsRank;
                students[numStudents].advancedRank = advancedRank;
                fclose(file);
                return;
            }
        }
    }

    fclose(file);
    printf("Application number not found in the file.\n");
}

void writeStudentsToFile(struct Student students[], int numStudents, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }
    // Write the header
    fprintf(file, "Application Number, Mains Rank, Advanced Rank, Choice1, Choice2, Choice3, Choice4, Choice5, Choice6, Choice7, Choice8 \n");

    for (int i = 0; i < numStudents; i++) {
        fprintf(file, "%d, %d, %d,", students[i].applicationNumber, students[i].mainsRank, students[i].advancedRank);
        for (int j = 0; j < students[i].numChoices; j++) {
            fprintf(file, " %s,", students[i].choices[j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

int isValidChoice(const char *choice, int advancedRank) {
    const char *validChoices[] = {"IITA CSE", "IITB CSE", "IITA IT", "IITB IT", "NITA CSE", "NITB CSE", "NITA IT", "NITB IT"};
    int numValidChoices = sizeof(validChoices) / sizeof(validChoices[0]);

    if (advancedRank == 0) {
        const char *eligibleChoices[] = {"NITA CSE", "NITA IT", "NITB CSE", "NITB IT"};
        int numEligibleChoices = sizeof(eligibleChoices) / sizeof(eligibleChoices[0]);

        for (int i = 0; i < numEligibleChoices; i++) {
            if (strcmp(choice, eligibleChoices[i]) == 0) {
                return 1; // Valid choice for students with advanced rank 0
            }
        }

        return 0; // Invalid choice for students with advanced rank 0
    }

    for (int i = 0; i < numValidChoices; i++) {
        if (strcmp(choice, validChoices[i]) == 0) {
            return 1; // Valid choice for students with non-zero advanced rank
        }
    }

    return 0; // Invalid choice for students with non-zero advanced rank
}

int isDuplicateChoice(struct Student student, const char *choice) {
    for (int i = 0; i < student.numChoices; i++) {
        if (strcmp(student.choices[i], choice) == 0) {
            return 1; // Duplicate choice
        }
    }
    return 0; // Not a duplicate choice
}

int isStudentLoggedIn(struct Student students[], int numStudents, int applicationNumber) {
    for (int i = 0; i < numStudents; i++) {
        if (students[i].applicationNumber == applicationNumber) {
            return 1; // Student is already logged in
        }
    }
    return 0; // Student is not logged in
}

int isAdminLoggedIn(struct Admin admin, int adminNumber, const char *enteredPassword) {
    if ((adminNumber == 12345 || adminNumber == 67890) && strcmp(enteredPassword, admin.password) == 0) {
        return 1; // Admin is logged in
    } else {
        printf("Invalid admin number or password! Admin login failed.\n");
        return 0; // Admin login failed
    }
}

int allStudentsEnteredChoices(struct Student students[], int numStudents) {
    for (int i = 0; i < numStudents; i++) {
        if (students[i].numChoices == 0) {
            return 0; // At least one student has not entered choices
        }
    }
    return 1; // All students have entered choices
}

int main() {
    struct Student students[MAX_STUDENTS];
    int numStudents = 0;

    struct Admin admin;
    admin.adminNumber = 12345;
    strcpy(admin.password, "345"); // Last 3 digits of admin number

    int isAdminLogged = 0; // Flag to keep track of admin login status
    int startChoiceFilling = 0; // Flag to check if choice filling process should start

    // Admin login
    printf("======== ADMIN LOGIN ========\n");
    int adminNumber;
    printf("Enter admin number (5 digits): ");
    scanf("%d", &adminNumber);
    getchar(); // Consume the newline character left in the input buffer

    char enteredPassword[4];
    sprintf(enteredPassword, "%03d", adminNumber % 1000); // Password is the last 3 digits of admin number

    if (isAdminLoggedIn(admin, adminNumber, enteredPassword)) {
        isAdminLogged = 1;

        printf("Enter password: ");
        char password[4];
        scanf("%3s", password);
        getchar(); // Consume the newline character left in the input buffer

        if (strcmp(password, enteredPassword) == 0) {
            printf("Password is correct.\n");

            printf("Start choice filling process? (Y/N): ");
            char choice;
            scanf(" %c", &choice);
            getchar(); // Consume the newline character left in the input buffer

            if (choice == 'Y' || choice == 'y') {
                startChoiceFilling = 1;
                printf("Choice filling process started.\n");
            } else {
                startChoiceFilling = 0;
                printf("Choice filling process not started.\n");
            }
        } else {
            printf("Invalid password! Admin login failed.\n");
            isAdminLogged = 0;
        }
    } else {
        isAdminLogged = 0;
        printf("Admin login failed. Exiting...\n");
        return 0;
    }

    if (startChoiceFilling) {
        // Student login and choice filling
        printf("======== STUDENT LOGIN ========\n");
        while (numStudents < MAX_STUDENTS) {
            printf("Enter application number (5 digits): ");
            scanf("%d", &students[numStudents].applicationNumber);
            getchar(); // Consume the newline character left in the input buffer

            // Check if application number is present in the CSV file
            if (!isApplicationNumberPresent(students[numStudents].applicationNumber)) {
                printf("Invalid login! Please try again.\n");
                continue;
            }

            // Check if student is already logged in
            if (isStudentLoggedIn(students, numStudents, students[numStudents].applicationNumber)) {
                printf("Student is already logged in! Please try again later.\n");
                continue;
            }

            // Generate password as last 3 digits of the application number
            sprintf(students[numStudents].password, "%03d", students[numStudents].applicationNumber % 1000);

            // Enter password
            char enteredPassword[4];
            do {
                printf("Enter password: ");
                scanf("%3s", enteredPassword);
                getchar(); // Consume the newline character left in the input buffer

                if (strcmp(enteredPassword, students[numStudents].password) == 0) {
                    printf("Login successful!\n");
                    break;
                } else {
                    printf("Invalid password! Please try again.\n");
                }
            } while (1);

            // Read Mains and Advanced ranks from the CSV file
            readRanksFromFile(students, numStudents, "student_rank.csv", students[numStudents].applicationNumber);

            printf("Mains Rank: %d\n", students[numStudents].mainsRank);
            printf("Advanced Rank: %d\n", students[numStudents].advancedRank);

            if (students[numStudents].advancedRank == 0) {
                printf("You are only eligible for NITs.\n");
                printf("Colleges available:\n");
                printf("College     Seats\n");
                printf("NITA CSE    2\n");
                printf("NITA IT     2\n");
                printf("NITB CSE    2\n");
                printf("NITB IT     2\n");
            } else {
                printf("You are eligible for both NITs and IITs.\n");
                printf("Colleges available:\n");
                printf("College     Seats\n");
                printf("IITA CSE    2\n");
                printf("IITA IT     2\n");
                printf("IITB CSE    2\n");
                printf("IITB IT     2\n");
                printf("NITA CSE    2\n");
                printf("NITA IT     2\n");
                printf("NITB CSE    2\n");
                printf("NITB IT     2\n");
            }

            printf("========= CHOICE FILLING =========\n");
            printf("Enter choices (one per line, enter 'q' to quit):\n");
            students[numStudents].numChoices = 0;

            while (students[numStudents].numChoices < MAX_PREFERENCES) {
                printf("Choice %d: ", students[numStudents].numChoices + 1);

                char choice[MAX_NAME_LENGTH];
                fgets(choice, sizeof(choice), stdin);
                choice[strcspn(choice, "\n")] = '\0';

                if (strcmp(choice, "q") == 0) {
                    break;
                }

                if (isDuplicateChoice(students[numStudents], choice)) {
                    printf("Duplicate choice! Please enter a different choice.\n");
                    continue;
                }

                if (!isValidChoice(choice, students[numStudents].advancedRank)) {
                    if (students[numStudents].advancedRank == 0) {
                        printf("Invalid choice! You can only select from NITA CSE, NITA IT, NITB CSE, and NITB IT.\n");
                    } else {
                        printf("Invalid choice! Please enter a valid choice.\n");
                    }
                    continue;
                }

                strcpy(students[numStudents].choices[students[numStudents].numChoices], choice);
                students[numStudents].numChoices++;

                // Limit the number of choices to 4 when advanced rank is 0 and mains rank is greater than 0
                if (students[numStudents].advancedRank == 0 && students[numStudents].mainsRank > 0 && students[numStudents].numChoices >= 4) {
                    printf("You can only choose 4 options. Exiting choice filling...\n");
                    break;
                }
            }

            printf("\n");

            numStudents++;

            if (numStudents >= MAX_STUDENTS) {
                printf("Maximum number of students reached.\n");
                printf("Student Choice List stored in student_choices.csv\n");
                break;
            }
        }

        if (allStudentsEnteredChoices(students, numStudents)) {
            printf("All students have entered their choices.\n");
            allocateSeats(students, numStudents);
        } else {
            printf("Not all students have entered their choices. Seat allocation process cannot be started.\n");
        }
    }
    // Write students to a CSV file
    writeStudentsToFile(students, numStudents, "sample.csv");

    return 0;
}
