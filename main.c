/*
 * Employee Payroll System
 * Student Name : [Your Name]
 * Reg No       : [Your Reg No]
 *
 * Features: Add, Search, Payslip, List, Delete, Update
 */

#include <stdio.h>
#include <string.h>

#define MAX_EMP  100
#define OT_RATE  100.0f

struct Employee {
    int id;
    char name[50];
    float basic;
    int otHours;
    float gross;
    float tax;
    float net;
};

struct Employee emp[MAX_EMP];
int count = 0;

void addEmployee();
void searchEmployee();
void generatePayslip();
void listAll();
void deleteEmployee();
void updateEmployee();
float calcTax(float gross);
int findByID(int id);

void readLine(char *buf, int len) {
    if (fgets(buf, len, stdin))
        buf[strcspn(buf, "\n")] = '\0';
}

int main() {
    int choice;

    printf("\n===================================\n");
    printf("      EMPLOYEE PAYROLL SYSTEM\n");
    printf("===================================\n");

    do {
        printf("\n--- MENU ---\n");
        printf("1. Add Employee\n");
        printf("2. Search Employee\n");
        printf("3. Generate Payslip\n");
        printf("4. List All Employees\n");
        printf("5. Delete Employee\n");
        printf("6. Update Employee\n");
        printf("0. Exit\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1: addEmployee();     break;
            case 2: searchEmployee();  break;
            case 3: generatePayslip(); break;
            case 4: listAll();         break;
            case 5: deleteEmployee();  break;
            case 6: updateEmployee();  break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);

    return 0;
}

int findByID(int id) {
    for (int i = 0; i < count; i++) {
        if (emp[i].id == id)
            return i;
    }
    return -1;
}

float calcTax(float gross) {
    if (gross <= 20000)
        return gross * 0.10f;
    else if (gross <= 50000)
        return gross * 0.20f;
    else
        return gross * 0.30f;
}

void addEmployee() {
    if (count >= MAX_EMP) {
        printf("Employee limit reached.\n");
        return;
    }

    struct Employee e;

    printf("Enter Employee ID: ");
    scanf("%d", &e.id);

    if (findByID(e.id) != -1) {
        printf("ID %d already exists.\n", e.id);
        while (getchar() != '\n');
        return;
    }

    getchar();
    printf("Enter Name: ");
    readLine(e.name, 50);

    printf("Enter Basic Pay: ");
    scanf("%f", &e.basic);

    printf("Enter OT Hours: ");
    scanf("%d", &e.otHours);

    e.gross = e.basic + (e.otHours * OT_RATE);
    e.tax   = calcTax(e.gross);
    e.net   = e.gross - e.tax;

    emp[count++] = e;
    printf("Employee added. Net Pay: Rs %.2f\n", e.net);
}

void searchEmployee() {
    int id;
    printf("Enter Employee ID: ");
    scanf("%d", &id);

    int i = findByID(id);
    if (i == -1) {
        printf("Employee not found.\n");
        return;
    }

    printf("Name  : %s (ID %d)\n", emp[i].name, emp[i].id);
    printf("Gross : Rs %.2f  Tax: Rs %.2f  Net: Rs %.2f\n",
           emp[i].gross, emp[i].tax, emp[i].net);
}

void generatePayslip() {
    int id;
    printf("Enter Employee ID: ");
    scanf("%d", &id);

    int i = findByID(id);
    if (i == -1) {
        printf("Employee not found.\n");
        return;
    }

    printf("\n========== PAYSLIP ==========\n");
    printf("ID        : %d\n", emp[i].id);
    printf("Name      : %s\n", emp[i].name);
    printf("Basic Pay : Rs %.2f\n", emp[i].basic);
    printf("OT Hours  : %d hrs @ Rs %.0f/hr\n", emp[i].otHours, OT_RATE);
    printf("OT Amount : Rs %.2f\n", emp[i].otHours * OT_RATE);
    printf("-----------------------------\n");
    printf("Gross Pay : Rs %.2f\n", emp[i].gross);
    printf("Tax       : Rs %.2f\n", emp[i].tax);
    printf("-----------------------------\n");
    printf("NET PAY   : Rs %.2f\n", emp[i].net);
    printf("=============================\n");
}

void listAll() {
    if (count == 0) {
        printf("No employees on record.\n");
        return;
    }

    printf("\n%-6s %-20s %-12s %-12s %-12s\n",
           "ID", "Name", "Gross", "Tax", "Net");
    printf("--------------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        printf("%-6d %-20s %-12.2f %-12.2f %-12.2f\n",
               emp[i].id, emp[i].name,
               emp[i].gross, emp[i].tax, emp[i].net);
    }

    printf("\nTotal: %d employee(s)\n", count);
}

void deleteEmployee() {
    int id;
    printf("Enter Employee ID to delete: ");
    scanf("%d", &id);

    int i = findByID(id);
    if (i == -1) {
        printf("Employee not found.\n");
        return;
    }

    char confirm;
    printf("Delete %s? (y/n): ", emp[i].name);
    scanf(" %c", &confirm);

    if (confirm != 'y' && confirm != 'Y') {
        printf("Cancelled.\n");
        return;
    }

    for (int j = i; j < count - 1; j++) {
        emp[j] = emp[j + 1];
    }
    count--;

    printf("Employee %d deleted.\n", id);
}

void updateEmployee() {
    int id;
    printf("Enter Employee ID to update: ");
    scanf("%d", &id);

    int i = findByID(id);
    if (i == -1) {
        printf("Employee not found.\n");
        return;
    }

    printf("Updating: %s (ID %d)\n", emp[i].name, emp[i].id);
    printf("1. Basic Pay (current: Rs %.2f)\n", emp[i].basic);
    printf("2. OT Hours  (current: %d hrs)\n",  emp[i].otHours);
    printf("3. Both\n");
    printf("0. Cancel\n");
    printf("Choice: ");

    int sub;
    scanf("%d", &sub);

    if (sub == 0) { printf("Cancelled.\n"); return; }

    if (sub == 1 || sub == 3) {
        printf("New Basic Pay: ");
        scanf("%f", &emp[i].basic);
    }
    if (sub == 2 || sub == 3) {
        printf("New OT Hours: ");
        scanf("%d", &emp[i].otHours);
    }
    if (sub < 1 || sub > 3) {
        printf("Invalid choice. No changes made.\n");
        return;
    }

    emp[i].gross = emp[i].basic + (emp[i].otHours * OT_RATE);
    emp[i].tax   = calcTax(emp[i].gross);
    emp[i].net   = emp[i].gross - emp[i].tax;

    printf("Updated. Gross: Rs %.2f  Tax: Rs %.2f  Net: Rs %.2f\n",
           emp[i].gross, emp[i].tax, emp[i].net);
}
