/*
 * Project Title : Employee Payroll System
 * Student Name  : [Your Name]
 * Register No   : [Your Reg No]
 *
 * Concepts Used : structs, functions, switch/case, validation
 * Compatible    : GCC (local) + Emscripten WebAssembly (browser)
 *
 * Features:
 *   1. Add Employee
 *   2. Search Employee by ID
 *   3. Generate Payslip
 *   4. List All Employees
 *   5. Delete Employee
 *   6. Update Employee Details
 */

#include <stdio.h>
#include <string.h>

#define MAX_EMPLOYEES 100
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
void addEmployee(void);
void searchEmployee(void);
void generatePayslip(void);
void listAllEmployees(void);
void deleteEmployee(void);
void updateEmployee(void);
float calculateTax(float gross);
int findIndexByID(int id);

/* Helper: read line safely */
void readLine(char *buf, int len) {
    if (fgets(buf, len, stdin)) {
        buf[strcspn(buf, "\n")] = '\0';
    }
}

/* ─────────────────────────────────────────── */
int main(void) {
    int choice;

    printf("\n====================================\n");
    printf("        EMPLOYEE PAYROLL SYSTEM     \n");
    printf("====================================\n");

    do {
        printf("\n--- MAIN MENU ---\n");
        printf("1. Add Employee\n");
        printf("2. Search Employee by ID\n");
        printf("3. Generate Payslip\n");
        printf("4. List All Employees\n");
        printf("5. Delete Employee\n");
        printf("6. Update Employee Details\n");
        printf("0. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1: addEmployee();      break;
            case 2: searchEmployee();   break;
            case 3: generatePayslip();  break;
            case 4: listAllEmployees(); break;
            case 5: deleteEmployee();   break;
            case 6: updateEmployee();   break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);

    return 0;
}

/* Helper: find array index by employee ID (-1 if not found) */
int findIndexByID(int id) {
    for (int i = 0; i < totalEmployees; i++) {
        if (employees[i].empID == id) return i;
    }
    return -1;
}

/* Tax calculation */
float calculateTax(float gross) {
    if (gross <= 20000) return gross * 0.10f;
    else if (gross <= 50000) return gross * 0.20f;
    else return gross * 0.30f;
}

/* ─────────────────────────────────────────── */
/* Feature 1 – Add Employee                    */
void addEmployee(void) {
    if (totalEmployees >= MAX_EMPLOYEES) {
        printf("[Error] Employee limit reached.\n");
        return;
    }

    struct Employee e;

    printf("Enter Employee ID: ");
    scanf("%d", &e.empID);

    if (findIndexByID(e.empID) != -1) {
        printf("[Error] Employee ID %d already exists.\n", e.empID);
        while (getchar() != '\n');
        return;
    }

    getchar();
    printf("Enter Name       : ");
    readLine(e.name, 50);

    printf("Enter Basic Pay  : ");
    scanf("%f", &e.basicPay);

    printf("Enter OT Hours   : ");
    scanf("%d", &e.otHours);

    e.grossPay = e.basicPay + (e.otHours * OT_RATE);
    e.tax      = calculateTax(e.grossPay);
    e.netPay   = e.grossPay - e.tax;

    employees[totalEmployees++] = e;
    printf("[Success] Employee %d added. Net Pay: Rs %.2f\n", e.empID, e.netPay);
}

/* ─────────────────────────────────────────── */
/* Feature 2 – Search Employee by ID           */
void searchEmployee(void) {
    int id;
    printf("Enter Employee ID: ");
    scanf("%d", &id);

    int idx = findIndexByID(id);
    if (idx == -1) {
        printf("[Error] Employee not found.\n");
        return;
    }
    printf("Found  : %s (ID %d)\n", employees[idx].name, employees[idx].empID);
    printf("Gross  : Rs %.2f  |  Tax: Rs %.2f  |  Net: Rs %.2f\n",
           employees[idx].grossPay, employees[idx].tax, employees[idx].netPay);
}

/* ─────────────────────────────────────────── */
/* Feature 3 – Generate Payslip                */
void generatePayslip(void) {
    int id;
    printf("Enter Employee ID: ");
    scanf("%d", &id);

    int idx = findIndexByID(id);
    if (idx == -1) {
        printf("[Error] Employee not found.\n");
        return;
    }

    printf("\n========== PAYSLIP ==========\n");
    printf("ID        : %d\n",        employees[idx].empID);
    printf("Name      : %s\n",        employees[idx].name);
    printf("Basic Pay : Rs %10.2f\n", employees[idx].basicPay);
    printf("OT Hours  : %d hrs @ Rs %.0f/hr\n", employees[idx].otHours, OT_RATE);
    printf("OT Amount : Rs %10.2f\n", employees[idx].otHours * OT_RATE);
    printf("-----------------------------\n");
    printf("Gross Pay : Rs %10.2f\n", employees[idx].grossPay);
    printf("Tax       : Rs %10.2f\n", employees[idx].tax);
    printf("-----------------------------\n");
    printf("NET PAY   : Rs %10.2f\n", employees[idx].netPay);
    printf("=============================\n");
}

/* ─────────────────────────────────────────── */
/* Feature 4 – List All Employees              */
void listAllEmployees(void) {
    if (totalEmployees == 0) {
        printf("[Info] No employees on record.\n");
        return;
    }
    printf("\n%-6s %-20s %-12s %-12s %-12s\n",
           "ID", "Name", "Gross (Rs)", "Tax (Rs)", "Net (Rs)");
    printf("%-6s %-20s %-12s %-12s %-12s\n",
           "------", "--------------------",
           "------------", "------------", "------------");
    for (int i = 0; i < totalEmployees; i++) {
        printf("%-6d %-20s %-12.2f %-12.2f %-12.2f\n",
               employees[i].empID,
               employees[i].name,
               employees[i].grossPay,
               employees[i].tax,
               employees[i].netPay);
    }
    printf("\nTotal employees: %d\n", totalEmployees);
}

/* ─────────────────────────────────────────── */
/* Feature 5 – Delete Employee                 */
void deleteEmployee(void) {
    int id;
    printf("Enter Employee ID to delete: ");
    scanf("%d", &id);

    int idx = findIndexByID(id);
    if (idx == -1) {
        printf("[Error] Employee not found.\n");
        return;
    }

    char confirm;
    printf("Delete %s (ID %d)? (y/n): ", employees[idx].name, employees[idx].empID);
    scanf(" %c", &confirm);

    if (confirm != 'y' && confirm != 'Y') {
        printf("[Cancelled] No changes made.\n");
        return;
    }

    for (int i = idx; i < totalEmployees - 1; i++) {
        employees[i] = employees[i + 1];
    }
    totalEmployees--;

    printf("[Success] Employee %d deleted.\n", id);
}

/* ─────────────────────────────────────────── */
/* Feature 6 – Update Employee Details         */
void updateEmployee(void) {
    int id;
    printf("Enter Employee ID to update: ");
    scanf("%d", &id);

    int idx = findIndexByID(id);
    if (idx == -1) {
        printf("[Error] Employee not found.\n");
        return;
    }

    printf("Updating: %s (ID %d)\n", employees[idx].name, employees[idx].empID);
    printf("What would you like to update?\n");
    printf("  1. Basic Pay  (current: Rs %.2f)\n", employees[idx].basicPay);
    printf("  2. OT Hours   (current: %d hrs)\n",  employees[idx].otHours);
    printf("  3. Both\n");
    printf("  0. Cancel\n");
    printf("Choice: ");

    int sub;
    scanf("%d", &sub);

    if (sub == 0) { printf("[Cancelled]\n"); return; }

    if (sub == 1 || sub == 3) {
        printf("Enter new Basic Pay: ");
        scanf("%f", &employees[idx].basicPay);
    }
    if (sub == 2 || sub == 3) {
        printf("Enter new OT Hours: ");
        scanf("%d", &employees[idx].otHours);
    }
    if (sub < 1 || sub > 3) {
        printf("Invalid choice. No changes made.\n");
        return;
    }

    employees[idx].grossPay = employees[idx].basicPay + (employees[idx].otHours * OT_RATE);
    employees[idx].tax      = calculateTax(employees[idx].grossPay);
    employees[idx].netPay   = employees[idx].grossPay - employees[idx].tax;

    printf("[Success] Record updated.\n");
    printf("New Gross: Rs %.2f  |  Tax: Rs %.2f  |  Net: Rs %.2f\n",
           employees[idx].grossPay, employees[idx].tax, employees[idx].netPay);
}
