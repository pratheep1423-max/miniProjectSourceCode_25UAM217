#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct clientData {
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void displayAll(FILE *fPtr);
void searchAccount(FILE *fPtr);
void totalBalance(FILE *fPtr);
void searchByName(FILE *fPtr);
void login();
void changePassword();

int main() {
    FILE *cfPtr;
    unsigned int choice;

    login();

    cfPtr = fopen("credit.dat", "rb+");

    if (cfPtr == NULL) {
        cfPtr = fopen("credit.dat", "wb+");
        if (cfPtr == NULL) {
            printf("File could not be opened.\n");
            exit(1);
        }

        struct clientData blank = {0, "", "", 0.0};
        for (int i = 0; i < 100; i++) {
            fwrite(&blank, sizeof(struct clientData), 1, cfPtr);
        }
    }

    while ((choice = enterChoice()) != 7) {
        switch (choice) {
            case 1: textFile(cfPtr); break;
            case 2: updateRecord(cfPtr); break;
            case 3: newRecord(cfPtr); break;
            case 4: deleteRecord(cfPtr); break;
            case 5: displayAll(cfPtr); break;
            case 6: searchAccount(cfPtr); break;
            case 8: totalBalance(cfPtr); break;
            case 9: searchByName(cfPtr); break;
            case 10: changePassword(); break;
            default: puts("Incorrect choice"); break;
        }
    }

    fclose(cfPtr);
    return 0;
}

void login() {
    char pass[20], savedPass[20];
    FILE *fp = fopen("password.txt", "r");

    if (fp == NULL) {
        fp = fopen("password.txt", "w");
        printf("Set new password: ");
        scanf("%s", pass);
        fprintf(fp, "%s", pass);
        fclose(fp);
        printf("Password saved! Restart program.\n");
        exit(0);
    }

    fscanf(fp, "%s", savedPass);
    fclose(fp);

    printf("Enter password: ");
    scanf("%s", pass);

    if (strcmp(pass, savedPass) != 0) {
        printf("Wrong password! Exiting...\n");
        exit(1);
    }
}

void changePassword() {
    char newPass[20];
    FILE *fp = fopen("password.txt", "w");

    if (fp == NULL) {
        printf("Error opening file\n");
        return;
    }

    printf("Enter new password: ");
    scanf("%s", newPass);

    fprintf(fp, "%s", newPass);
    fclose(fp);

    printf("Password changed successfully!\n");
}

void textFile(FILE *readPtr) {
    FILE *writePtr;
    struct clientData client;

    writePtr = fopen("accounts.txt", "w");
    if (writePtr == NULL) {
        printf("File could not be opened.\n");
        return;
    }

    rewind(readPtr);
    fprintf(writePtr, "%-6s%-16s%-11s%10s\n",
            "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(struct clientData), 1, readPtr)) {
        if (client.acctNum != 0) {
            fprintf(writePtr, "%-6u%-16s%-11s%10.2f\n",
                    client.acctNum, client.lastName,
                    client.firstName, client.balance);
        }
    }
    fclose(writePtr);
}

void updateRecord(FILE *fPtr) {
    unsigned int account;
    double transaction;
    struct clientData client;

    printf("Enter account (1-100): ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0) {
        printf("No account found.\n");
        return;
    }

    printf("Balance: %.2f\n", client.balance);
    printf("Enter amount (+/-): ");
    scanf("%lf", &transaction);

    if (client.balance + transaction < 100) {
        printf("Minimum balance ₹100 required!\n");
        return;
    }

    client.balance += transaction;

    FILE *log = fopen("transactions.txt", "a");
    if (log) {
        time_t t = time(NULL);
        fprintf(log, "Date: %sAccount: %u | Amount: %.2f | Balance: %.2f\n",
                ctime(&t), client.acctNum,
                transaction, client.balance);
        fclose(log);
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Updated successfully!\n");
}

void newRecord(FILE *fPtr) {
    struct clientData client = {0, "", "", 0.0};
    unsigned int account;

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0) {
        printf("Already exists.\n");
        return;
    }

    printf("Enter lastname firstname balance: ");
    scanf("%14s%9s%lf",
          client.lastName,
          client.firstName,
          &client.balance);

    client.acctNum = account;

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Account created!\n");
}

void deleteRecord(FILE *fPtr) {
    struct clientData blank = {0, "", "", 0};
    struct clientData client;
    unsigned int account;

    printf("Enter account to delete: ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0) {
        printf("Not found.\n");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blank, sizeof(struct clientData), 1, fPtr);

    printf("Deleted!\n");
}

void displayAll(FILE *fPtr) {
    struct clientData client;

    rewind(fPtr);
    printf("\n%-6s%-16s%-11s%10s\n",
           "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(struct clientData), 1, fPtr)) {
        if (client.acctNum != 0) {
            printf("%-6u%-16s%-11s%10.2f\n",
                   client.acctNum,
                   client.lastName,
                   client.firstName,
                   client.balance);
        }
    }
}

void searchAccount(FILE *fPtr) {
    struct clientData client;
    unsigned int account;

    printf("Enter account: ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
        printf("Not found\n");
    else
        printf("%u %s %s %.2f\n",
               client.acctNum,
               client.lastName,
               client.firstName,
               client.balance);
}

void totalBalance(FILE *fPtr) {
    struct clientData client;
    double total = 0;

    rewind(fPtr);
    while (fread(&client, sizeof(struct clientData), 1, fPtr)) {
        if (client.acctNum != 0)
            total += client.balance;
    }

    printf("Total Bank Balance: %.2f\n", total);
}

void searchByName(FILE *fPtr) {
    struct clientData client;
    char name[15];

    printf("Enter last name: ");
    scanf("%s", name);

    rewind(fPtr);
    while (fread(&client, sizeof(struct clientData), 1, fPtr)) {
        if (strcmp(client.lastName, name) == 0) {
            printf("%u %s %s %.2f\n",
                   client.acctNum,
                   client.lastName,
                   client.firstName,
                   client.balance);
        }
    }
}

unsigned int enterChoice(void) {
    unsigned int choice;

    printf("\n--- Bank Menu ---\n"
           "1 - Export accounts\n"
           "2 - Update account\n"
           "3 - Add account\n"
           "4 - Delete account\n"
           "5 - Display all\n"
           "6 - Search account\n"
           "7 - Exit\n"
           "8 - Total balance\n"
           "9 - Search by name\n"
           "10 - Change password\n"
           "Enter choice: ");

    scanf("%u", &choice);
    return choice;
}