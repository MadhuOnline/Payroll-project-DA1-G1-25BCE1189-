/*
 * Project Title : Employee Payroll System
 * Student Name  : [Your Name]
 * Register No   : [Your Reg No]
 *
 * Concepts Used : structs, functions, file I/O, switch/case, validation
 * Compatible    : GCC (local) + Emscripten WebAssembly (browser)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMPLOYEES 100
#define FILE_NAME     "payroll.txt"
#define OT_RATE       100.0f   /* Rs per OT hour */

struct Employee {
    int empID;
    char name[50];
    float basicPay;
    int otHours;
    float grossPay;
    float tax;
    float netPay;
};

/* Global data */
struct Employee employees[MAX_EMPLOYEES];
int totalEmployees = 0;

/* Function prototypes */
void loadFromFile(void);
void saveToFile(void);
void addEmployee(void);
void searchEmployee(void);
void generatePayslip(void);
float calculateTax(float gross);
void listAllEmployees(void);

/* Helper: read line safely */
void readLine(char *buf, int len) {
    if (fgets(buf, len, stdin)) {
        buf[strcspn(buf, "\n")] = '\0';
    }
}

int main(void) {
    int choice;
    loadFromFile();

    printf("\n====================================\n");
    printf("        EMPLOYEE PAYROLL SYSTEM     \n");
    printf("====================================\n");

    do {
        printf("\n--- MAIN MENU ---\n");
        printf("1. Add Employee\n");
        printf("2. Search Employee by ID\n");
        printf("3. Generate Payslip\n");
        printf("4. List All Employees\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            while(getchar()!='\n'); /* clear buffer */
            continue;
        }

        switch(choice) {
            case 1: addEmployee(); break;
            case 2: searchEmployee(); break;
            case 3: generatePayslip(); break;
            case 4: listAllEmployees(); break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while(choice != 0);

    return 0;
}

/* Load existing records */
void loadFromFile(void) {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return;
    totalEmployees = 0;
    while (totalEmployees < MAX_EMPLOYEES &&
           fscanf(fp, "%d,%49[^,],%f,%d,%f,%f,%f\n",
                  &employees[totalEmployees].empID,
                  employees[totalEmployees].name,
                  &employees[totalEmployees].basicPay,
                  &employees[totalEmployees].otHours,
                  &employees[totalEmployees].grossPay,
                  &employees[totalEmployees].tax,
                  &employees[totalEmployees].netPay) == 7) {
        totalEmployees++;
    }
    fclose(fp);
    printf("[Info] Loaded %d employee(s).\n", totalEmployees);
}

/* Save records */
void saveToFile(void) {
    FILE *fp = fopen(FILE_NAME, "w");
    if (!fp) {
        printf("[Error] Could not save file.\n");
        return;
    }
    for (int i=0; i<totalEmployees; i++) {
        fprintf(fp, "%d,%s,%.2f,%d,%.2f,%.2f,%.2f\n",
                employees[i].empID,
                employees[i].name,
                employees[i].basicPay,
                employees[i].otHours,
                employees[i].grossPay,
                employees[i].tax,
                employees[i].netPay);
    }
    fclose(fp);
}

/* Add new employee */
void addEmployee(void) {
    if (totalEmployees >= MAX_EMPLOYEES) {
        printf("[Error] Limit reached.\n");
        return;
    }
    struct Employee e;
    printf("Enter Employee ID: ");
    scanf("%d", &e.empID);
    getchar(); /* clear newline */
    printf("Enter Name: ");
    readLine(e.name, 50);
    printf("Enter Basic Pay: ");
    scanf("%f", &e.basicPay);
    printf("Enter OT Hours: ");
    scanf("%d", &e.otHours);

    e.grossPay = e.basicPay + (e.otHours * OT_RATE);
    e.tax      = calculateTax(e.grossPay);
    e.netPay   = e.grossPay - e.tax;

    employees[totalEmployees++] = e;
    saveToFile();
    printf("[Success] Employee added.\n");
}

/* Search employee */
void searchEmployee(void) {
    int id;
    printf("Enter Employee ID: ");
    scanf("%d", &id);
    for (int i=0; i<totalEmployees; i++) {
        if (employees[i].empID == id) {
            printf("Found: %s, Net Pay Rs %.2f\n",
                   employees[i].name, employees[i].netPay);
            return;
        }
    }
    printf("[Error] Employee not found.\n");
}

/* Generate payslip */
void generatePayslip(void) {
    int id;
    printf("Enter Employee ID: ");
    scanf("%d", &id);
    for (int i=0; i<totalEmployees; i++) {
        if (employees[i].empID == id) {
            printf("\n--- PAYSLIP ---\n");
            printf("ID       : %d\n", employees[i].empID);
            printf("Name     : %s\n", employees[i].name);
            printf("BasicPay : Rs %.2f\n", employees[i].basicPay);
            printf("OT Hours : %d\n", employees[i].otHours);
            printf("GrossPay : Rs %.2f\n", employees[i].grossPay);
            printf("Tax      : Rs %.2f\n", employees[i].tax);
            printf("NetPay   : Rs %.2f\n", employees[i].netPay);
            printf("----------------\n");
            return;
        }
    }
    printf("[Error] Employee not found.\n");
}

/* Tax calculation */
float calculateTax(float gross) {
    if (gross <= 20000) return gross * 0.1f;
    else if (gross <= 50000) return gross * 0.2f;
    else return gross * 0.3f;
}

/* List all employees */
void listAllEmployees(void) {
    printf("\n%-6s %-15s %-10s %-10s %-10s\n",
           "ID","Name","Gross","Tax","Net");
    for (int i=0; i<totalEmployees; i++) {
        printf("%-6d %-15s %-10.2f %-10.2f %-10.2f\n",
               employees[i].empID,
               employees[i].name,
               employees[i].grossPay,
               employees[i].tax,
               employees[i].netPay);
    }
}
