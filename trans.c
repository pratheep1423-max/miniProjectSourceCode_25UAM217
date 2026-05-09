#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// clientData structure definition
struct clientData
{
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

int main()
{
    FILE *cfPtr;
    unsigned int choice;

    login();

    // try opening existing file
    cfPtr = fopen("credit.dat", "rb+");

    // if not exist, create and initialize
    if (cfPtr == NULL)
    {
        cfPtr = fopen("credit.dat", "wb+");

        if (cfPtr == NULL)
        {
            printf("File could not be opened.\n");
            exit(1);
        }

        // create 100 empty records
        struct clientData blank = {0, "", "", 0.0};

        for (int i = 0; i < 100; i++)
        {
            fwrite(&blank, sizeof(struct clientData), 1, cfPtr);
        }
    }

    while ((choice = enterChoice()) != 7)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;

        case 2:
            updateRecord(cfPtr);
            break;

        case 3:
            newRecord(cfPtr);
            break;

        case 4:
            deleteRecord(cfPtr);
            break;

        case 5:
            displayAll(cfPtr);
            break;

        case 6:
            searchAccount(cfPtr);
            break;

        case 8:
            totalBalance(cfPtr);
            break;

        case 9:
            searchByName(cfPtr);
            break;

        case 10:
            changePassword();
            break;

        default:
            puts("Incorrect choice");
            break;
        }
    }

    fclose(cfPtr);
    return 0;
}

// login system
void login()
{
    char pass[20], savedPass[20];

    FILE *fp = fopen("password.txt", "r");

    // first time password setup
    if (fp == NULL)
    {
        fp = fopen("password.txt", "w");

        printf("Set new password: ");
        scanf("%19s", pass);

        fprintf(fp, "%s", pass);

        fclose(fp);

        printf("Password saved! Restart program.\n");
        exit(0);
    }

    fscanf(fp, "%19s", savedPass);
    fclose(fp);

    printf("Enter password: ");
    scanf("%19s", pass);

    if (strcmp(pass, savedPass) != 0)
    {
        printf("Wrong password! Exiting...\n");
        exit(1);
    }
}

// change password
void changePassword()
{
    char newPass[20];

    FILE *fp = fopen("password.txt", "w");

    if (fp == NULL)
    {
        printf("Error opening file\n");
        return;
    }

    printf("Enter new password: ");
    scanf("%19s", newPass);

    fprintf(fp, "%s", newPass);

    fclose(fp);

    printf("Password changed successfully!\n");
}

// create formatted text file
void textFile(FILE *readPtr)
{
    FILE *writePtr;
    struct clientData client;

    writePtr = fopen("accounts.txt", "w");

    if (writePtr == NULL)
    {
        puts("File could not be opened.");
        return;
    }

    rewind(readPtr);

    fprintf(writePtr,
            "%-6s%-16s%-11s%10s\n",
            "Acct",
            "Last Name",
            "First Name",
            "Balance");

    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr,
                    "%-6u%-16s%-11s%10.2f\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.balance);
        }
    }

    fclose(writePtr);

    printf("Accounts exported to accounts.txt successfully.\n");
}

// update balance
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;
    struct clientData client;

    printf("Enter account to update (1 - 100): ");
    scanf("%u", &account);

    if (account < 1 || account > 100)
    {
        printf("Invalid account number!\n");
        return;
    }

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account #%u has no information.\n", account);
        return;
    }

    printf("Current balance: %.2f\n", client.balance);

    printf("Enter deposit (+) or withdraw (-): ");
    scanf("%lf", &transaction);

    if (client.balance + transaction < 100)
    {
        printf("Minimum balance ₹100 required!\n");
        return;
    }

    client.balance += transaction;

    // log transaction
    FILE *log = fopen("transactions.txt", "a");

    if (log != NULL)
    {
        time_t t = time(NULL);

        fprintf(log,
                "Date: %sAccount: %u | Amount: %.2f | New Balance: %.2f\n",
                ctime(&t),
                client.acctNum,
                transaction,
                client.balance);

        fclose(log);
    }

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&client,
           sizeof(struct clientData),
           1,
           fPtr);

    printf("Updated balance: %.2f\n", client.balance);
}

// add new record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};

    unsigned int account;

    printf("Enter new account number (1 - 100): ");
    scanf("%u", &account);

    if (account < 1 || account > 100)
    {
        printf("Invalid account number!\n");
        return;
    }

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum != 0)
    {
        printf("Account already exists.\n");
        return;
    }

    printf("Enter lastname firstname balance:\n");

    scanf("%14s%9s%lf",
          client.lastName,
          client.firstName,
          &client.balance);

    if (client.balance < 100)
    {
        printf("Minimum opening balance is ₹100\n");
        return;
    }

    client.acctNum = account;

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&client,
           sizeof(struct clientData),
           1,
           fPtr);

    printf("Account created successfully.\n");
}

// delete record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blank = {0, "", "", 0};

    unsigned int account;

    printf("Enter account number to delete (1 - 100): ");
    scanf("%u", &account);

    if (account < 1 || account > 100)
    {
        printf("Invalid account number!\n");
        return;
    }

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account does not exist.\n");
        return;
    }

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&blank,
           sizeof(struct clientData),
           1,
           fPtr);

    printf("Account deleted successfully.\n");
}

// display all accounts
void displayAll(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    printf("\n%-6s%-16s%-11s%10s\n",
           "Acct",
           "Last Name",
           "First Name",
           "Balance");

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n",
                   client.acctNum,
                   client.lastName,
                   client.firstName,
                   client.balance);
        }
    }
}

// search account
void searchAccount(FILE *fPtr)
{
    struct clientData client;
    unsigned int account;

    printf("Enter account number to search: ");
    scanf("%u", &account);

    if (account < 1 || account > 100)
    {
        printf("Invalid account number!\n");
        return;
    }

    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
    }
    else
    {
        printf("\n%-6u%-16s%-11s%10.2f\n",
               client.acctNum,
               client.lastName,
               client.firstName,
               client.balance);
    }
}

// total bank balance
void totalBalance(FILE *fPtr)
{
    struct clientData client;
    double total = 0;

    rewind(fPtr);

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            total += client.balance;
        }
    }

    printf("Total Bank Balance: %.2f\n", total);
}

// search by last name
void searchByName(FILE *fPtr)
{
    struct clientData client;
    char name[15];
    int found = 0;

    printf("Enter last name: ");
    scanf("%14s", name);

    rewind(fPtr);

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 fPtr) == 1)
    {
        if (strcmp(client.lastName, name) == 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n",
                   client.acctNum,
                   client.lastName,
                   client.firstName,
                   client.balance);

            found = 1;
        }
    }

    if (!found)
    {
        printf("No account found with that name.\n");
    }
}

// menu
unsigned int enterChoice(void)
{
    unsigned int choice;

    printf("\n--- Bank Menu ---\n"
           "1 - Export accounts to text file\n"
           "2 - Update account\n"
           "3 - Add new account\n"
           "4 - Delete account\n"
           "5 - Display all accounts\n"
           "6 - Search account\n"
           "7 - Exit\n"
           "8 - Total balance\n"
           "9 - Search by name\n"
           "10 - Change password\n"
           "Enter choice: ");

    scanf("%u", &choice);

    return choice;
}