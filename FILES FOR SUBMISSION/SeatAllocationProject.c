#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PREFERENCES 8
#define MAX_NAME_LENGTH 50
#define MAX_STUDENTS 80

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

// Structure to store applicant data
typedef struct {
    char regno[10];
    int mains_rank;
    int advance_rank;
    char preferences[MAX_PREFERENCES][10];
    char allocated_seat[10];
} Applicant;

// Structure to store college allocation data
typedef struct {
    char college_name[10];
    char branch[10];
    char regno[10];
    int mains_rank;
    int advance_rank;
} CollegeAllocation;

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

// CSV Parser Function
int parseCSV(const char* filename, Applicant applicants[], int max_applicants) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return -1;
    }

    char line[1024];
    int num_applicants = 0;

    // Skip the header line
    fgets(line, sizeof(line), file);

    // Read applicant data line by line
    while (fgets(line, sizeof(line), file) != NULL) {
        if (num_applicants >= max_applicants) {
            printf("Maximum number of applicants reached.\n");
            break;
        }

        // Tokenize the line using comma as delimiter
        char* token = strtok(line, ",");
        int token_index = 0;

        // Parse each token and store it in the applicant structure
        while (token != NULL) {
            switch (token_index) {
                case 0:
                    strcpy(applicants[num_applicants].regno, token);
                    break;
                case 1:
                    applicants[num_applicants].mains_rank = atoi(token);
                    break;
                case 2:
                    applicants[num_applicants].advance_rank = atoi(token);
                    break;
                default:
                    if (token_index - 3 < MAX_PREFERENCES) {
                        strcpy(applicants[num_applicants].preferences[token_index - 3], token);
                    }
                    break;
            }
            token = strtok(NULL, ",");
            token_index++;
        }

        num_applicants++;
    }

    fclose(file);

    return num_applicants;
}

// Function to compare two applicants based on their advanced ranks (excluding 0 ranks)
int compareApplicants(const void* a, const void* b) {
    const Applicant* applicant1 = (const Applicant*)a;
    const Applicant* applicant2 = (const Applicant*)b;

    if (applicant1->advance_rank == 0 && applicant2->advance_rank != 0) {
        return 1;
    } else if (applicant1->advance_rank != 0 && applicant2->advance_rank == 0) {
        return -1;
    } else if (applicant1->advance_rank == 0 && applicant2->advance_rank == 0) {
        return applicant1->mains_rank - applicant2->mains_rank; // Compare based on mains rank
    } else {
        return applicant1->advance_rank - applicant2->advance_rank;
    }
}

// Function to allocate seats based on ranks and preferences
void allocateSeats(Applicant applicants[], int num_applicants) {
    char available_seats[][10] = {
        "IITA CSE", "IITB CSE", "IITA IT", "IITB IT",
        "NITA CSE", "NITB CSE", "NITA IT", "NITB IT"
    };
    int seats_count[] = {2, 2, 2, 2, 2, 2, 2, 2};

    // Sort applicants based on advanced rank (excluding 0 ranks)
qsort(applicants, num_applicants, sizeof(Applicant), compareApplicants);

FILE* student_file = fopen("student_allocation.csv", "w");
if (student_file == NULL) {
    printf("Failed to create the student allocation file.\n");
    return;
}

fprintf(student_file, "Application Number,Mains Rank,Advanced Rank,Seat Allotted\n");

for (int i = 0; i < num_applicants; i++) {
    bool seat_allocated = false;

    if (applicants[i].advance_rank > 0) {
        // Allocate seat based on first choice and rank if advanced rank is present
        for (int j = 0; j < MAX_PREFERENCES; j++) {
            if (strcmp(applicants[i].preferences[j], "IIT") == 0) {
                int previous_index = i - 1;
                if (previous_index >= 0 && applicants[i].advance_rank == applicants[previous_index].advance_rank) {
                    continue; // Skip allocation if the advanced ranks are the same
                }
            }

            for (int k = 0; k < sizeof(available_seats) / sizeof(available_seats[0]); k++) {
                if (strcmp(applicants[i].preferences[j], available_seats[k]) == 0 && seats_count[k] > 0) {
                    strcpy(applicants[i].allocated_seat, available_seats[k]);
                    seats_count[k]--;
                    seat_allocated = true;
                    break;
                }
            }

            if (seat_allocated) {
                break;
            }
        }
    } else {
        // Sort applicants based on mains rank
        qsort(applicants + i, num_applicants - i, sizeof(Applicant), compareApplicants);

        // Allocate seat based on first choice and rank if no advanced rank
        for (int j = i; j < num_applicants; j++) {
            for (int k = 0; k < MAX_PREFERENCES; k++) {
                for (int l = 0; l < sizeof(available_seats) / sizeof(available_seats[0]); l++) {
                    if (strcmp(applicants[j].preferences[k], available_seats[l]) == 0 && seats_count[l] > 0) {
                        strcpy(applicants[j].allocated_seat, available_seats[l]);
                        seats_count[l]--;
                        seat_allocated = true;
                        break;
                    }
                }
                if (seat_allocated) {
                    break;
                }
            }
            if (seat_allocated) {
                break;
            }
        }
    }

    if (!seat_allocated) {
        fprintf(student_file, "%s,%d,%d,Seat Not Allotted\n", applicants[i].regno, applicants[i].mains_rank, applicants[i].advance_rank);
    } else {
        fprintf(student_file, "%s,%d,%d,%s\n", applicants[i].regno, applicants[i].mains_rank, applicants[i].advance_rank, applicants[i].allocated_seat);
    }
}

fclose(student_file);
printf("Allocation details are stored in student_allocation.csv\n");
}

// Function to display allotted seat based on application number
void displayAllottedSeat(const char* applicationNumber) {
    FILE* allotmentFile = fopen("student_allocation.csv", "r");
    if (allotmentFile == NULL) {
        printf("Failed to open the allotment file.\n");
        return;
    }

    char line[1024];

    // Skip the header line
    fgets(line, sizeof(line), allotmentFile);

    // Read and display allotted seat for the given application number
    bool found = false;
    while (fgets(line, sizeof(line), allotmentFile) != NULL) {
        char currentApplicationNumber[10];
        char allottedSeat[10];

        sscanf(line, "%[^,],%*d,%*d,%[^\n]", currentApplicationNumber, allottedSeat);

        if (strcmp(currentApplicationNumber, applicationNumber) == 0) {
            printf("Allotted Seat: %s\n", allottedSeat);
            found = true;
            break;
        }
    }

    if (!found) {
        printf("No seat allotted for the given application number.\n");
    }

    fclose(allotmentFile);
}

void displayStudentsByCollege(const char* collegeName, Applicant applicants[], int numApplicants) {
    printf("Students allotted in %s:\n", collegeName);
    printf("Reg No\t\tMains Rank\tAdvanced Rank\n");

    int count = 0;
    for (int i = 0; i < numApplicants; i++) {
        if (strcmp(applicants[i].allocated_seat, collegeName) == 0) {
            printf("%s\t\t%d\t\t%d\n", applicants[i].regno, applicants[i].mains_rank, applicants[i].advance_rank);
            count++;
        }
    }

    printf("Seats left in %s: %d\n", collegeName, 2 - count);
    printf("\n");
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
        while (numStudents < MAX_STUDENTS) {
            printf("======== STUDENT LOGIN ========\n");
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

    Applicant applicants[MAX_STUDENTS];
    printf("\n");
    printf("============= ALLOCATION PROCESS =============\n");
    int num_applicants = parseCSV("student_matrix (2).csv", applicants, MAX_STUDENTS);

    if (num_applicants > 0) {
        allocateSeats(applicants, num_applicants);
    }

    int adminNum;
    char password[4];

    printf("\n");
    printf("========== ADMIN LOGIN ==========\n");
    printf("Enter the admin number: ");
    scanf("%d", &adminNum);

    if (adminNum == 12345) {
    int enteredPassword;
    printf("Enter the password: ");
    scanf("%d", &enteredPassword);

    if (enteredPassword == 345) {
       // Display students allotted in each college and the number of seats left
        const char* collegeNames[8] = {
        "IITA CSE", "IITB CSE", "IITA IT", "IITB IT",
        "NITA CSE", "NITB CSE", "NITA IT", "NITB IT"
    };
    printf("========== COLLEGE DETAILS ==========");
    for (int i = 0; i < 8; i++) {
        displayStudentsByCollege(collegeNames[i], applicants, MAX_STUDENTS);
    }
    } else {
        printf("Invalid password. Access denied.\n");
    }
    } else {
        printf("Invalid admin number. Access denied.\n");
    }


    char applicationNumber[10];
    printf("\n");
    printf("========DISPLAY ALLOCATION========\n");
    printf("Enter the application number (or 'q' to quit): ");
    scanf("%s", applicationNumber);

    while (applicationNumber[0] != 'q') {
        displayAllottedSeat(applicationNumber);

        printf("Enter the application number (or 'q' to quit): ");
        scanf("%s", applicationNumber);
    }

    return 0;
}