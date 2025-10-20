#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <direct.h>
#include <dirent.h>
#include <stdbool.h>
#include <ctype.h>
//Default input string length
#define STRING_SIZE 16
//Default filename length
#define FILENAME_SIZE 21

//User datatype declaration
//idNum and accountNum is string/char array so can do error checking by evaluating the chars
struct accountData {
    char name[64];
    char idNum[16];
    int accountType;
    char accountNum[16];
    float accountBalance;
};

//Functions used
//Functions defined below main() for readability(?)
void mainMenu(int *ptr);
void createAccount(struct accountData *);
void bankDeposit(struct accountData *, float);
int bankWithdrawal(struct accountData *, float);
void sendRemittance(struct accountData *, struct accountData *, float);
void deleteAccount();
bool isNum(char strInput[STRING_SIZE]);
bool isPosNum(char strInput[STRING_SIZE]);
bool isFloat(char strInput[STRING_SIZE]);

int main(void) {
    int menuSelect = 0;
    //Standard variable for taking floats, set as string/char array so can do error checking
    char moneyIO[STRING_SIZE];
    //Define 2 structs for user and recipient, initialize all values with empty values
    //Empty values so reduce chances of undefined behaviour from pre-existing values
    struct accountData user = {"", "", 0, "", 0};
    struct accountData recipient = {"", "", 0, "", 0};
    //If _mkdir returns -1, assume database already exists and printf as such
    if (_mkdir("Database") == -1) {
        printf("Database already exists.\n");
    }
    while (menuSelect != -1) {
        //menuSelect reset to 0 for every loop of while after the first
        menuSelect = 0;
        mainMenu(&menuSelect);
        switch (menuSelect) {
            case 1:
                createAccount(&user);
                break;
            case 2:
                //If user's accountNum is empty, keep asking until valid response given
                while (user.accountNum == "\000") {
                    printf("Please enter your account number: ");
                    //%16s to truncate whatever user types into stdin to 16 characters
                    scanf("%16s", user.accountNum);

                    while (!isPosNum(user.accountNum)) {
                        printf("Please enter a valid account number (numerics only): ");
                        scanf("%16s", user.accountNum);
                    }
                }
                printf("Please enter your depositing amount: ");
                //%16s to truncate user's input, should be able to prevent overflow
                scanf("%16s", moneyIO);
                //Keep asking user for valid input if user enters funny inputs
                while (!isFloat(moneyIO)) {
                    printf("Invalid input. Please enter a proper amount: ");
                    scanf("%16s", moneyIO);
                }
                //Convert moneyIO to float with atof() then pass into bankDeposit function
                bankDeposit(&user, atof(moneyIO));
                //Empty moneyIO contents for next loop
                strcpy(moneyIO, "");
                break;
            case 3:
                //If user's accountNum is empty, keep asking for a valid accountNum
                while (user.accountNum == "") {
                    printf("Please enter your account number: ");
                    //%16s should truncate user's input if over 16 char
                    scanf("%16s", user.accountNum);
                    while (!isPosNum(user.accountNum)) {
                        printf("Please enter a valid account number (numerics only): ");
                        scanf("%16s", user.accountNum);
                    }
                }
                printf("Please enter your withdrawing amount: ");
                //%16s should truncate user's input if over 16 char
                scanf("%16s", moneyIO);
                //Keep asking for valid input if contents of moneyIO isn't a float
                while (!isFloat(moneyIO)) {
                    printf("Invalid input. Please enter a proper amount: ");
                    scanf("%16s", moneyIO);
                }
                //atof() converts moneyIO as string to float for bankWithdrawal function
                bankWithdrawal(&user, atof(moneyIO));
                //Empty moneyIO contents for next loop
                strcpy(moneyIO, "");
                break;
            case 4:
                //Keep asking for valid float until it is given
                printf("Please enter remittance amount: ");
                //%16s should truncate user's input if over 16 char
                scanf("%16s", moneyIO);
                while (!isFloat(moneyIO)) {
                    printf("Invalid input. Please enter a proper amount: ");
                    scanf("%16s", moneyIO);
                }
                //If recipient's saved accountNum on this session is empty, ask for it
                while (recipient.accountNum == "") {
                    printf("Please enter the recipient's account number: ");
                    //%16s should truncate user's input if over 16 char
                    scanf("%16s", &recipient.accountNum);
                    while (!isPosNum(recipient.accountNum)) {
                        printf("Please enter a valid account number: ");
                        scanf("%16s", &recipient.accountNum);
                    }
                }
                //atof() converts moneyIO to float before passing to sendRemittance
                sendRemittance(&user, &recipient, atof(moneyIO));
                //Empty moneyIO contents for next loop
                strcpy(moneyIO, "");
                break;
            case 5:
                deleteAccount();
                break;
            case -1:
                //Exit message
                printf("Thank you, have a nice day.\n");
                break;
            default:
                //Error checking function only checks for if the input is a number, not if the number is in the menu range
                //This default should catch any inputs that are num but aren't a valid menu option
                printf("Unknown input, please try again.\n");
        }
    }
}

//FUNCTION DEFINITION

//Print menu, return input
void mainMenu(int *choice) {
    char value[STRING_SIZE] = "";
    //char defined for clearing stdin in case of overflow
    char c;
    printf("\n1. Create new account\n");
    printf("2. Bank deposit\n");
    printf("3. Bank withdrawal\n");
    printf("4. Send remittance\n");
    printf("5. Delete bank account\n");
    printf("-1. Exit\n");
    printf("\nPlease select an option: ");
    //scanf works but only up until a whitespace this includes space, tab, and newline
    //leftover values after whitespace remains in stdin
    //even though having any whitespace already makes the input invalid
    //while ((c = getchar() != '\n') && c != EOF) clears leftover chars in stdin if user input is over string size

    scanf("%16s", &value);

    // fgets(value, STRING_SIZE, stdin);
    while((c = getchar()) != '\n' && c != EOF);
    // strcpy(value, "string input here bruh");
    while (!isNum(value)) {
        printf("Unknown input, please try again: ");
        scanf("%16s", &value);
        // fgets(value, STRING_SIZE, stdin);
        while ((c = getchar()) != '\n' && c != EOF);
        // strcpy(value, "1");
    }
    //Converts value to num for main() function to match case
    *choice = atoi(value);
}

//Create new account
void createAccount(struct accountData *ptr) {
    //sets seed of rand() to NULL; makes numbers more random
    //Default rand() will return consistent list of numbers, setting seed to NULL seems to solve this issue
    srand(time(NULL));

    //Valid account type checker
    bool isValidAccount = false;
    //Temp char for clearing stdin
    char c;
    //Yser input
    //No error checking for name input, whatever character is accepted
    printf("Enter your name (64 Characters): ");
    fgets(ptr->name, 64, stdin);
    //Clear stdin after name input, just in case
    //while((c = getchar()) != '\n' && c != EOF);
    printf("Enter your identification number: ");
    scanf("%16s", &ptr->idNum);
    while (!isPosNum(ptr->idNum)) {
        printf("Invalid identification number, please try again: ");
        scanf("%16s", &ptr->idNum);
    }
    //Previous scanf asked for characters, next scanf will ask for numbers
    //Better to clear stdin to prevent leftover values being accepted by next scanf
    while (getchar() != '\n');
    //Assume no error checking needed, any non-integer input here when converted to decimal is surely not 1 or 2
    printf("Enter your desired account type (1. Savings, 2. Current): ");
    ("%d", &ptr->accountType);
    while (ptr->accountType != 1 && ptr->accountType != 2) {
        printf("Unknown input, please choose 1. Savings, 2. Current: ");
        //clear the accountType, otherwise infinite looping this error code
        ptr->accountType = 0;
        scanf("%d", &ptr->accountType);
    }
    //Generation of bank account number and balance
    //Mathematical expression is equation to generate random number within a range of values
    //(<MAX> + rand() / (RAND_MAX / (<MAX> - <MIN> + 1) + 1))
    //<MAX>: Maximum value    |    <MIN>: Minimum value   |    RAND_MAX: Constant of the highest number rand() can actually generate
    //snprintf to format random number as int type into ptr->accountNum of char[] type
    snprintf(ptr->accountNum, STRING_SIZE, "%d", (10000 + rand() / (RAND_MAX / (1000000 - 10000 + 1) + 1)));
    printf("\nYour account number is: %s\n", ptr->accountNum);
    ptr->accountBalance = 0.00;

    //checking for if this bank account number already exists
    FILE *fPtr;
    char filename[FILENAME_SIZE];
    snprintf(filename, FILENAME_SIZE, "./Database/%s.txt", ptr->accountNum);
    while (fopen(filename, "r") != NULL) {
        //If bank account .txt file already exists, regenerate another number then try again
        snprintf(ptr->accountNum, 16, "%d", (10000 + rand() / (RAND_MAX / (1000000 - 10000 + 1) + 1)));
        snprintf(filename, FILENAME_SIZE, "%s.txt", ptr->accountNum);
    }

    //write into file
    fPtr = fopen(filename, "w");
    fprintf(fPtr, "%s", ptr->name);
    fprintf(fPtr, "%s\n", ptr->idNum);
    fprintf(fPtr, "%d\n", ptr->accountType);
    fprintf(fPtr, "%s\n", ptr->accountNum);
    //%.2f to format float to 2 decimal places
    fprintf(fPtr, "%.2f\n", ptr->accountBalance);
    printf("Account creation successful!");
    //fclose(fPtr) to close the file
    //Set fPtr to NULL just in case another function somewhere uses it, creating undefined behaviour
    fclose(fPtr);
    fPtr = NULL;
}

//Bank deposit
void bankDeposit(struct accountData *ptr, float deposit) {
    char filename[FILENAME_SIZE];
    //Buffer for storing file lines
    char lines[64];
    snprintf(filename, FILENAME_SIZE, "./Database/%s.txt", ptr->accountNum);
    //Checking for if file doesn't exist
    //fptr is opened outside of while loop instead of put inside the condition. When inside the condition I don't know how to close the file, no fptr
    FILE *fptr = fopen(filename, "r");
    while (fptr == NULL) {
        printf("User account not found. Please enter a valid account number: ");
        //Immediately close the file pointer before reassigning it to new file for good measure
        fclose(fptr);
        //%16s to truncate user's input in case over 16 chars
        scanf("%16s", &ptr->accountNum);
        while (!isPosNum(ptr->accountNum)) {
            printf("Please enter a valid account number (numerics only): ");
            scanf("%16s", &ptr->accountNum);
        }
        snprintf(filename, FILENAME_SIZE, "./Database/%s.txt", ptr->accountNum);
        fptr = fopen(filename, "r");
    }
    FILE *fptr_temp = fopen("./Database/temp.txt", "w");
    //Copy paste first 4 lines from original to temp file
    for (int i = 0; i < 4; i++) {
        fgets(lines, 64, fptr);
        fprintf(fptr_temp, "%s", lines);
    }
    //This line contains the accountBalance
    fgets(lines, 64, fptr);
    //Store the final account balance in current session's accountBalance
    //Use atof() on lines because lines read from file are stored as strings
    ptr->accountBalance = deposit + atof(lines);
    fprintf(fptr_temp, "%.2f\n", ptr->accountBalance);
    //Close all files and set their pointers to NULL
    fclose(fptr);
    fclose(fptr_temp);
    fptr = NULL;
    fptr_temp = NULL;
    //Files must all be closed beforehand, otherwise removing and renaming files returns error: permission denied
    //Permission was denied because some file pointers still had the files open
    if (remove(filename) != 0) {
        printf("Error removing file\n");
    }
    if (rename("./Database/temp.txt", filename) != 0) {
        printf("Error renaming file\n");
    }
    printf("Amount deposited successfully!\n");
}

int bankWithdrawal(struct accountData *ptr, float withdrawal) {
    char filename[FILENAME_SIZE];
    char lines[64];
    //This function returns an error so that I can check for failed withdrawals in sendRemittance
    int err = 1;
    snprintf(filename, FILENAME_SIZE, "./Database/%s.txt", ptr->accountNum);
    //Checking for if file doesn't exist
    //fptr is opened outside of while loop instead of put inside the condition. When inside the condition I don't know how to close the file, no fptr
    FILE *fptr = fopen(filename, "r");
    while (fptr == NULL) {
        printf("User account not found. Please enter a valid account number: ");
        //Immediately close the file pointer before reassigning it to new file for good measure
        fclose(fptr);
        scanf("%16s", &ptr->accountNum);
        while (!isPosNum(ptr->accountNum)) {
            printf("Please enter a valid account number (numerics only): ");
            scanf("%16s", &ptr->accountNum);
        }
        snprintf(filename, FILENAME_SIZE, "./Database/%s.txt", ptr->accountNum);
        fptr = fopen(filename, "r");
    }
    FILE *fptr_temp = fopen("./Database/temp.txt", "w");
    //Copy paste first 4 lines from original to temp file
    for (int i = 0; i < 4; i++) {
        fgets(lines, 64, fptr);
        fprintf(fptr_temp, "%s", lines);
    }
    //This line contains the accountBalance
    fgets(lines, 64, fptr);
    //Checking if there is enough in the accountBalance
    //'>=' for if user is withdrawing entire account balance
    if (atof(lines) >= withdrawal) {
        withdrawal = atof(lines) - withdrawal;
        fprintf(fptr_temp, "%.2f\n", withdrawal);
        printf("Amount withdrawn successfully!\n");
        err = 0;
    }
    else {
        //Return error when insufficient balance
        printf("Insufficient balance.\n");
        fprintf(fptr_temp, lines);
        err = 1;
    }
    //Close all files and set their pointers to NULL
    fclose(fptr);
    fclose(fptr_temp);
    fptr = NULL;
    fptr_temp == NULL;
    //Closing files is done beforehand, otherwise removing and renaming files returns error: permission denied
    //Permission denied because file pointers still had some files open
    if (remove(filename) != 0) {
        printf("Error removing file\n");
    }
    if (rename("./Database/temp.txt", filename) != 0) {
        printf("Error renaming file\n");
    }
    return err;
}

void sendRemittance(struct accountData *user, struct accountData *recipient, float remit) {
    float moneyOut;
    char userfilename[FILENAME_SIZE];
    char recipfilename[FILENAME_SIZE];
    char lines[64];
    int err;
    snprintf(userfilename, FILENAME_SIZE, "./Database/%s.txt", user->accountNum);
    //Checking if file doesn't exist
    //fptr is opened outside of while loop instead of put inside the condition. When inside the condition I don't know how to close the file, no fptr
    FILE *fptrUser = fopen(userfilename, "r");
    while (fptrUser == NULL) {
        //Error checking for valid account number
        printf("User account not found. Please enter a valid account number: ");
        //Immediately close the file pointer before reassigning it to new file for good measure
        fclose(fptrUser);
        scanf("%16s", &user->accountNum);
        while (!isPosNum(user->accountNum)) {
            printf("Please enter a valid account number (numerics only): ");
            scanf("%16s", &user->accountNum);
        }
        snprintf(userfilename, FILENAME_SIZE, "./Database/%s.txt", user->accountNum);
        fptrUser = fopen(userfilename, "r");
    }
    //Skip first 3 lines
    for (int i = 0; i < 3; i++) {
        fgets(lines, 64, fptrUser);
    }
    //This value is user's accountType; Save it
    user->accountType = atoi(lines);
    fclose(fptrUser);
    snprintf(recipfilename, FILENAME_SIZE, "./Database/%s.txt", recipient->accountNum);
    //Checking if file doesn't exist
    //fptr is opened outside of while loop instead of put inside the condition. When inside the condition I don't know how to close the file, no fptr
    FILE *fptrRecip = fopen(recipfilename, "r");
    while (fptrRecip == NULL) {
        printf("Recipient account not found. Please enter a valid account number: ");
        //Immediately close the file pointer before reassigning it to new file for good measure
        fclose(fptrRecip);
        scanf("%16s", &recipient->accountNum);
        while (!isPosNum(recipient->accountNum)) {
            printf("Please enter a valid account number (numerics only): ");
            scanf("%16s", &recipient->accountNum);
        }
        snprintf(recipfilename, FILENAME_SIZE, "./Database/%s.txt", recipient->accountNum);
        fptrRecip = fopen(recipfilename, "r");
    }
    //Skip first 3 lines
    for (int i = 0; i < 3; i++) {
        fgets(lines, 64, fptrRecip);
    }
    //This value is recipient's accountType; Save it
    recipient->accountType = atoi(lines);
    //Close all files and set their pointers to NULL
    fclose(fptrRecip);
    fptrRecip = NULL;
    fptrUser = NULL;
    //Set user's withdrawal amount based on what accountType is sending to what accountType
    if (user->accountType == 1 && recipient->accountType == 2) {
        moneyOut = remit * 1.02;
    }
    else if (user->accountType == 2 && recipient->accountType == 1) {
        moneyOut = remit * 1.03;
    }
    else {
        //If same accountType is sending to each other, no tax applied
        moneyOut = remit;
    }
    //Try to withdraw the amount from the user's account
    //Store the error value
    err = bankWithdrawal(user, moneyOut);
    if (err == 0) {
        //If no error, deposit to recipient's account the remittance
        bankDeposit(recipient, remit);
    }
    else {
        //No need to redeposit back to user as no amount is deducted if insufficient balance
        printf("Insufficient account balance!\n");
    }
}

void deleteAccount () {
    //Defines a directory pointer to a requested directory
    DIR *directory = opendir("./Database");
    //File entries returned are of struct dirent type, so needs a struct variable to store it
    struct dirent *entry;
    char accountNum[STRING_SIZE];
    if (directory == NULL) {
        printf("Error opening directory\n");
    }
    //Recursively fetches all entries in directory
    while ((entry = readdir(directory)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    if (closedir(directory) == -1) {
        printf("Error closing directory\n");
    }
    //Prompt for account number
    printf("\nEnter account to delete: ");
    scanf("%16s", &accountNum);
    //Error check to ensure just numbers
    while (!isPosNum(accountNum)) {
        printf("Please enter a valid account number (numerics only): ");
        scanf("%16s", &accountNum);
    }
    char filename[FILENAME_SIZE];
    snprintf(filename, FILENAME_SIZE, "./Database/%s.txt", accountNum);
    //Checking if file doesn't exist
    //fptr is opened outside of while loop instead of put inside the condition. When inside the condition I don't know how to close the file, no fptr
    FILE *fptr = fopen(filename, "r");
    while (fptr == NULL) {
        printf("User account not found. Please enter a valid account number: ");
        //Immediately close the file pointer before reassigning it to new file for good measure
        fclose(fptr);
        scanf("%16s", &accountNum);
        //Error check again to ensure just numbers
        while (!isPosNum(accountNum)) {
            printf("Please enter a valid account number (numerics only): ");
            scanf("%16s", &accountNum);
        }
        snprintf(filename, FILENAME_SIZE, "./Database/%s.txt", accountNum);
        fptr = fopen(filename, "r");
    }
    fclose(fptr);
    fptr = NULL;
    //Closing files is done beforehand, otherwise removing and renaming files returns error: permission denied
    //Permission denied because file pointers still had some files open
    if (remove(filename) != 0) {
        printf("Error removing file.\n");
    }
    else {
        printf("Account deleted successfully.\n");
    }
}

bool isNum(char strInput[16]) {
    bool valid = true;
    int len = strlen(strInput);
    //Keep a count of the number of times a '-' shows up
    int dashCount = 0;
    //trims trailing whitespace
    // while (len > 0 && isspace(strInput[len - 1])) {
    //     len--;
    // }
    //If empty, automatically invalid
    if (len == 0) {
        valid = false;
    }
    //loops thorugh every char for number or '-'
    //if any character is not a number, or '-' shows up twice; automatically invalid
    for (int i = 0; i < len; i++) {
        if (!isdigit(strInput[i])) {
            //only increment dotCount if this is the first '.' to appear
            if (strInput[i] == '-' && dashCount == 0) {
                dashCount++;
            }
            else {
                valid = false;
                return valid;
            }
        }
    }
    return valid;
}

bool isPosNum(char strInput[16]) {
    bool valid = true;
    int len = strlen(strInput);
    //trims trailing whitespace
    // while (len > 0 && isspace(strInput[len - 1])) {
    //     len--;
    // }
    //if empty, automatically invalid
    if (len == 0) {
        valid = false;
    }
    //loops through every char for number
    //if any character is not a number, automatically invalid
    for (int i = 0; i < len; i++) {
        if (!isdigit(strInput[i])) {
            valid = false;
            break;
        }
    }
    return valid;
}

bool isFloat(char strInput[16]) {
    bool valid = true;
    int len = strlen(strInput);
    //Keep a count of the number of times a '.' shows up
    int dotCount = 0;
    //If string is empty, automatically invalid
    if (len == 0) {
        valid = false;
    }
    //loops thorugh every char for number or '.'
    //if any character is not a number, or '.' shows up twice; automatically invalid
    for (int i = 0; i < len; i++) {
        if (!isdigit(strInput[i])) {
            //only increment dotCount if this is the first '.' to appear
            if (strInput[i] == '.' && dotCount == 0) {
                dotCount++;
            }
            else {
                valid = false;
                return valid;
            }
        }
    }
    return valid;
}