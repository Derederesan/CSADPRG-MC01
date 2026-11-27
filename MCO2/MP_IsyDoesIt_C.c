#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_NAME_LEN 63
#define MAX_ACCOUNT_NUM 100
#define MAX_TRANSACTIONS 20000

// 7, because index 0 is a placeholder
#define MAX_CURRENCIES 7

typedef struct {
    char accountName[64];
    char accountNumber[11];
    char date[11];
    char time[9];
    char currency[4];
    double amount;
    char type[11];
    double currentBalance;
} Transaction;

typedef struct {
    char code[4];
    double rate;
} Currency;

typedef struct {
	char accountName[64];
	char accountNumber[11];
	char date[11];
	double totalWithdrawalAmounts;
	int numberOfDeposits;
	double totalDepositAmounts;
	double turnoverVelocity;
	int velocityAvailable;
	int suspiciousFlag;
} DailyActivity;

typedef struct {
    char accountName[64];
    char accountNumber[11];
    int accountAge;
    double currentBalance;
    double averageDailyBalance;
    char privilegeTier[10];
} PrivilegeAccount;

/*
===================== FUNCTION PROTOTYPES ======================
*/

void cleanInputBuffer();
int loadTransactions(char *filename, Transaction records[], int maxCount);
Transaction *findAccount(Transaction records[], int count, char *accountNumber);
unsigned long long stringToAccountNumber(char *str);
unsigned long long getNextAccountNumber(Transaction records[], int count);
int saveTransaction(char *filename, Transaction *record);

void printNavMenu();
void printExchangeMenu();
double convertCurrency(double amount, int source, int target, Currency rates[MAX_CURRENCIES]);

int getValidChoice(int lb, int ub);
int getValidChoiceClean(int lb, int ub);
double getValidAmount();
void getValidName(char *nameBuffer, int maxLen);
int getValidYN();

int validateTransaction(Transaction *record);
int createAccountTransaction(Transaction *record, unsigned long long number, double deposit);
int getValidCurrency(Currency rates[MAX_CURRENCIES]);

int registerAccount(Transaction records[], int *count, int maxCount, char *filename);
int setTransactionDateTime(Transaction *record);

int depositAmount(Transaction records[], int *count, int maxCount,
                  char *filename, Currency rates[MAX_CURRENCIES]);

int withdrawAmount(Transaction records[], int *count, int maxCount,
                   char *filename, Currency rates[MAX_CURRENCIES]);

void currencyExchange(Currency rates[MAX_CURRENCIES]);
void recordExchangeRates(Currency rates[MAX_CURRENCIES]);
int showInterestAmount(Transaction records[], int count);

void transactionSelect(int choice, Currency rates[MAX_CURRENCIES],
                       Transaction records[], int *count,
                       int maxCount, char *filename);

void printReportMenu();
void generateReportsMenu(Transaction records[], int *count, int maxCount, char *filename);

long long getTransactionDateValue(Transaction *record);
int getLatestAccountIndexes(Transaction records[], int count, int accountIndexes[]);
double calculateExpectedInterest(double startingBalance);
int filterInterestAccounts(Transaction records[], int accountIndexes[], int accountCount, double expectedInterest[]);
void sortInterestAccounts(Transaction records[], int accountIndexes[], double expectedInterest[], int qualifiedCount);
int exportInterestReport(Transaction records[], int accountIndexes[], double expectedInterest[], int qualifiedCount);
int generateInterestReport(Transaction records[], int count);
int getDailyActivities(Transaction records[], int count, DailyActivity activities[]);
void calculateRiskFlags(DailyActivity activities[], int activityCount);
void sortDailyActivities(DailyActivity activities[], int activityCount);
int exportSuspiciousReport(DailyActivity activities[], int activityCount);
int generateSuspiciousReport(Transaction records[], int count);

int getDaysInMonth(int month, int year);
int dateToDayCount(char *date);
double calculateAverageDailyBalance(Transaction records[], int count,
                                    char *accountNumber, int month, int year);
void sortPrivilegeAccounts(PrivilegeAccount accounts[], int qualifiedCount);
int exportPrivilegeReport(PrivilegeAccount accounts[], int qualifiedCount);
int generatePrivilegeReport(Transaction records[], int count);

/*
===================== FUNCTION PROTOTYPES ======================
*/

/********************
Last names: Ilagan, Serios, Yardan
Language: C
Paradigm(s): Procedural
********************/

/**
 * @file
 * @brief C implementation of the Banking and Currency Exchange Application.
 * @author Alexie Ilagan, Justine Serios, Andre Yardan
 *
 * Developed by Team Isy Does It for CSADPRG MCO1.
 */

/**
 * @brief Cleans retained string data in memory.
 *
 * @post The input buffer is cleaned for next scanf input.
 */

void cleanInputBuffer(){
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

int loadTransactions(char *filename, Transaction records[], int maxCount){
	FILE *file;
	Transaction *ptr;
	int i = 0;
	int fields = 0;
	int totalRows = 0;
	int invalidRows = 0;
	int filteredRows = 0;
	int lineLength = 0;
	char line[256] = {0};
	char endChar = '\0';
	char extraChar = '\0';

	file = fopen(filename, "r");

	if(file == NULL){
		printf("Cannot open CSV.\n");
		return -1;
	}

	fscanf(file, "%*[^\n]");
	fscanf(file, "%*c");

	while(i < maxCount){
		line[0] = '\0';
		fields = fscanf(file, "%255[^\n]%c", line, &endChar);

		if(fields == EOF){
			break;
		}

		totalRows++;

		if(fields == 0){
			fscanf(file, "%c", &endChar);
			invalidRows++;
			printf("Invalid transaction at row %d.\n", totalRows + 1);
			continue;
		}

		if(fields == 2 && endChar != '\n'){
			while(fscanf(file, "%c", &endChar) == 1 && endChar != '\n');
			invalidRows++;
			printf("Invalid transaction at row %d.\n", totalRows + 1);
			continue;
		}

		lineLength = strlen(line);

		if(lineLength > 0 && line[lineLength - 1] == '\r'){
			line[lineLength - 1] = '\0';
		}

		ptr = records + i;

		fields = sscanf(line, "%63[^,],%10[^,],%10[^,],%8[^,],%3[^,],%lf,%10[^,],%lf%c",
			ptr->accountName,
			ptr->accountNumber,
			ptr->date,
			ptr->time,
			ptr->currency,
			&ptr->amount,
			ptr->type,
			&ptr->currentBalance,
			&extraChar
		);

		if(fields != 8){
			invalidRows++;
			printf("Invalid transaction at row %d.\n", totalRows + 1);
			continue;
		}

		if(strcmp(ptr->currency, "PHP") != 0){
			filteredRows++;
			continue;
		}

		if(validateTransaction(ptr) == 0){
			invalidRows++;
			printf("Invalid transaction at row %d.\n", totalRows + 1);
			continue;
		}

		i++;
	}

	fclose(file);

	printf("Total rows: %d\n", totalRows);
	printf("Invalid rows: %d\n", invalidRows);
	printf("Filtered rows: %d\n", filteredRows);
	printf("Accepted rows: %d\n", i);

	return i;
}

Transaction *findAccount(Transaction records[], int count, char *accountNumber) {
    Transaction *latest = NULL;
    long long latestDate = -1;
    for (int i = 0; i < count; i++) {
        if (strcmp(records[i].accountNumber, accountNumber) == 0) {
            long long d = getTransactionDateValue(records + i);
            if (d >= latestDate) {
                latestDate = d;
                latest = records + i;
            }
        }
    }
    return latest;
}

unsigned long long stringToAccountNumber(char *str) {
    unsigned long long number = 0;
    char *ptr = str;
    int isValid = 1;

    if (str == NULL || strlen(str) != 10) {
        isValid = 0;
    }

    while (isValid && *ptr != '\0') {
        if (*ptr < '0' || *ptr > '9') {
            isValid = 0;
            number = 0;
        } else {
            number = number * 10 + (*ptr - '0');
            ptr++;
        }
    }

    return number;
}

unsigned long long getNextAccountNumber(Transaction records[], int count){
	
	unsigned long long highest = 0;
	unsigned long long current = 0;
	unsigned long long next = 0;
	int i = 0;
	
	for(i = 0; i < count; i++){
		current = stringToAccountNumber((records+i)->accountNumber);
		
		if(current == 0){
			printf("Invalid account number.\n");
			return 0;
		}
		
		if(current>highest){
			highest = current;
		}
	}

	next = highest + 1;

	if (next > 9999999999ULL){
		printf("Account number limits reached.\n");
		return 0;
	}

	return next;
}

int saveTransaction(char *filename, Transaction *record){
    FILE *file;
	int saveResult = 0;
    file = fopen(filename, "a");

    if(file == NULL){
        printf("Cannot open CSV.\n");
        return saveResult;
    }

    if(fprintf(file, "%s,%s,%s,%s,%s,%.2lf,%s,%.2lf\n",
        record->accountName,
        record->accountNumber,
        record->date,
        record->time,
        record->currency,
        record->amount,
        record->type,
        record->currentBalance
    ) < 0){
        fclose(file);
        return saveResult;
    }

    if(fclose(file) != 0){
        return saveResult;
    }
	
	saveResult = 1;
    return saveResult;
}

/**
 * @brief Prints a purely visual guide on the menu navigation.   
 * 
 * @warning Includes a double newline at the end of its printing.
 */


void printNavMenu(){
	printf("Select Transaction:\n");
	printf("[1] Register Account\n");
	printf("[2] Deposit Amount\n");
	printf("[3] Withdraw Amount\n");
	printf("[4] Currency Exchange\n");
	printf("[5] Record Exchange Rates\n");
	printf("[6] Show Interest Amount\n");
	printf("[7] Generate Reports\n");
	printf("[0] Exit Transaction\n\n");
}

/**
 * @brief Prints a purely visual guide on the different exchange currencies.
 * 
 * @warning Includes a double newline at the end of its printing.
 */

void printExchangeMenu(){
	printf("[1] Philippine Peso (PHP)\n");
	printf("[2] United States Dollar (USD)\n");
	printf("[3] Japanese Yen (JPY)\n");
	printf("[4] British Pound Sterling (GBP)\n");
	printf("[5] Euro (EUR)\n");
	printf("[6] Chinese Yuan Renminni (CNY)\n\n");
}

/**
 * @brief Prints a purely visual guide on report generation.   
 * 
 * @warning Includes a double newline at the end of its printing.
 */
 
void printReportMenu(){
	printf("\nGenerate Reports\n");
	printf("[1] Load the file\n");
	printf("[2] Generate Reports\n");
	printf("[0] Back\n\n");
}


/**
 * @brief Converts a source currency amount to a target currency
 *        using their respective PHP-relative exchange rates.
 *
 * @param[in] amount  Amount in the source currency.
 * @param[in] source  Index of the source currency.
 * @param[in] target  Index of the target currency.
 * @param[in] rates   Array containing currency codes and exchange rates.
 *
 * @pre Source and target indices must be valid.
 * @pre The target currency exchange rate must not be zero.
 *
 * @return The converted amount in the target currency.
 */
double convertCurrency(double amount, int source, int target, Currency rates[MAX_CURRENCIES]){
	return amount * rates[source].rate / rates[target].rate;
}

/**
 * @brief 	Prints lines to guide UI, then takes an integer input value, accepting only values in a strict range. 
 * 
 * @pre 	Standard input stream must be available.
 * @warning Currently only supports integer-based console inputs through stdin.
 * 			Prints an error message in case of invalid input, cleans the input buffer afterwards.
 *			Highly coupled with the main function, has several prints that guide the UI.
 *
 * @param[in]     lb  Inclusive lower bound that scanf checks for error detection.
 * @param[in]     ub  Inclusive upper bound that scanf checks for error detection.
 * 
 * @return The exact valid integer value input from stdin.
 */

int getValidChoice(int lb, int ub){
	int validInput = 0;
	int valSelect = -1;
	
	while(!validInput) {
		printf("Choice: ");		
		if(scanf("%d", &valSelect) == 1 && valSelect >= lb && valSelect <= ub){
			validInput = 1;
			cleanInputBuffer();
			printf("\n***\n");
			printf("Choice = %d\n", valSelect);			
		}
		else {
			printf("Incorrect input. Please try again.\n");
			cleanInputBuffer();
		}
	}
	return valSelect;
}

/**
 * @brief 	Cleanly takes an integer input value, accepting only values in a strict range.
 * 
 * @pre 	Standard input stream must be available.
 * @warning Currently only supports integer-based console inputs through stdin.
 * 			Prints an error message in case of invalid input, cleans the input buffer afterwards.
 *
 * @param[in]     lb  Inclusive lower bound that scanf checks for error detection.
 * @param[in]     ub  Inclusive upper bound that scanf checks for error detection.
 * 
 * @return The exact valid integer value input from stdin.
 */

int getValidChoiceClean(int lb, int ub){
	int validInput = 0;
	int valSelect = 0;
	
	while(!validInput) {	
		if(scanf("%d", &valSelect) == 1 && valSelect >= lb && valSelect <= ub){
			validInput = 1;
			cleanInputBuffer();	
		}
		else {
			printf("Incorrect input. Please try again.\n");
			cleanInputBuffer();
		}
	}
	return valSelect;
}

/**
 * @brief 	Cleanly takes double input value.
 * 
 * @pre 	Standard input stream must be available.
 * @warning Currently only supports double-based console inputs through stdin.
 * 			Prints an error message in case of invalid input, cleans the input buffer afterwards.
 * 
 * @return The exact valid double value input from stdin.
 */

double getValidAmount(){
	int validInput = 0;
	double valAmount = -1;
	
	while(!validInput) {
		if(scanf("%lf", &valAmount) == 1){
			validInput = 1;
			cleanInputBuffer();	
		}
		else {
			printf("Incorrect input. Please try again.\n");
			cleanInputBuffer();
		}
	}
	return valAmount;
}

/**
 * @brief 	Cleanly takes a valid string input value, accepting only strings up to a defined max length.
 * 
 * @pre 		The max length must be defined.
 * @warning 	Uses a scanf formatting to specify all leftover newlines are ignored, and 
 *              then to capture everything until next newline.
 *
 * @param[out]    nameBuffer  String to be modified according to the user's input.
 * @param[in]     maxLen  	  Maximum length that can be given to nameBuffer.
 */

void getValidName(char *nameBuffer, int maxLen) {
    int validFlag = 0;
    int nameLength = -1;

    while (!validFlag) {
        if (scanf(" %63[^\n]", nameBuffer) == 1) {
            cleanInputBuffer();
            nameLength = (int)strlen(nameBuffer);

            if (nameLength > 0 && nameLength < maxLen &&
                strchr(nameBuffer, ',') == NULL) {
                validFlag = 1;
            } else {
                printf("Invalid name.\n");
            }
        } else {
            cleanInputBuffer();
        }
    }
}

/**
 * @brief   Prompts the user for a Y/N character input and validates it.
 * 
 * @pre     Standard input stream must be available.
 * @warning Consumes the first non-whitespace character and flushes the rest of the line.
 *          Prints an error message in case of invalid input, cleans the input buffer afterwards.
 * 
 * @return  1 if Y/y, 0 if N/n.
 */
int getValidYN(){
    int result = 0;
    int validFlag = 0;
    char decision = '\0';

    while(!validFlag){
        if(scanf(" %c", &decision) == 1){
            cleanInputBuffer();
            if(decision == 'Y' || decision == 'y'){
                result = 1;
                validFlag = 1;
            } else if(decision == 'N' || decision == 'n'){
                result = 0;
                validFlag = 1;
            } else {
                printf("\nInvalid input. Please enter Y or N.\n");
            }
        } else {
            cleanInputBuffer();
            printf("\nIncorrect input. Please enter Y or N: ");
        }
    }
    return result;
}


int validateTransaction(Transaction *record){
	int validFlag = 1;
	int i = 0;
	int day = 0, month = 0, year = 0;
	int hour = 0, minute = 0, second = 0;
	int daysInMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int timeLength = strlen(record->time);
	int colon1 = 2, colon2 = 5;

	if(strlen(record->accountNumber) != 10){
		validFlag = 0;
	}

	for(i = 0; record->accountNumber[i] != '\0'; i++){
		if(record->accountNumber[i] < '0' || record->accountNumber[i] > '9'){
			validFlag = 0;
		}
	}

	if(record->accountName[0] == '\0'){
		validFlag = 0;
	}

	if(strlen(record->date) != 10){
		validFlag = 0;
	}
	
	if (record->amount <= 0 || record->currentBalance < 0){
		validFlag = 0;
	}
	
	if (strlen(record->date) == 10){
	
		if(record->date[2] != '/' || record->date[5] != '/'){
			validFlag = 0;
		}
		
		for(i = 0; i < 10; i++){
			if(i != 2 && i != 5){
				if(record->date[i] < '0' || record->date[i] > '9'){
					validFlag = 0;
				}
			}
		}
		
	}

	if(sscanf(record->date, "%2d/%2d/%4d", &day, &month, &year) != 3){
		validFlag = 0;
	}

	if(year < 1 || month < 1 || month > 12 || day < 1){
		validFlag = 0;
	}

	if(year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)){
		daysInMonth[1] = 29;
	}

	if(month >= 1 && month <= 12){
		if(day > daysInMonth[month - 1]){
			validFlag = 0;
		}
	}

	if(timeLength == 7){
		colon1 = 1;
		colon2 = 4;
	}

	if(timeLength != 7 && timeLength != 8){
		validFlag = 0;
	} else if (timeLength == 7 || timeLength == 8 ){
		if(record->time[colon1] != ':' || record->time[colon2] != ':'){
			validFlag = 0;
		}
	}

	for(i = 0; i < timeLength; i++){
		if(i != colon1 && i != colon2){
			if(record->time[i] < '0' || record->time[i] > '9'){
				validFlag = 0;
			}
		}
}

	if(sscanf(record->time, "%2d:%2d:%2d", &hour, &minute, &second) != 3){
		validFlag = 0;
	}

	if(hour < 0 || hour > 23 || minute < 0 || minute > 59 ||
	   second < 0 || second > 59){
		validFlag = 0;
	}

	if(strcmp(record->currency, "PHP") != 0){
		validFlag = 0;
	}

	if(strcmp(record->type, "deposit") != 0 && strcmp(record->type, "withdrawal") != 0){
		validFlag = 0;
	}
	return validFlag;
}

int createAccountTransaction(Transaction *record, unsigned long long number, double deposit){
    time_t currentTime = time(NULL);
    struct tm *dateTime = localtime(&currentTime);
    int i = 0;
	int creationResult = 0;
	
    if(dateTime == NULL){
        return creationResult;
    }

    for(i = 9; i >= 0; i--){
        record->accountNumber[i] = '0' + (number % 10);
        number /= 10;
    }

    record->accountNumber[10] = '\0';

    strcpy(record->currency, "PHP");
    strcpy(record->type, "deposit");

    record->amount = deposit;
    record->currentBalance = deposit;

    if(strftime(record->date, sizeof(record->date),
        "%d/%m/%Y", dateTime) == 0){
        return creationResult;
    }

    if(strftime(record->time, sizeof(record->time),
        "%H:%M:%S", dateTime) == 0){
        return creationResult;
    }
	
	creationResult = 1;
    return creationResult;
}

/**
 * @brief   Prompts the user for a valid, registered currency code.
 * 
 * @param[in]       rates  Represents the array of structs that hold the values
 *                           for both the currency rate, and currency code. ie, USD
 *
 * @pre     Standard input stream must be available.
 * @return  Integer value that represents an index position mapped to a global currency.
 */

int getValidCurrency(Currency rates[MAX_CURRENCIES]){
	char input[8] = {0};
	int i = 0;
	int currencyChoice = -1;
	
	while (currencyChoice == -1){
		printf("Currency (PHP/USD/JPY/GBP/EUR/CNY): ");
		
		if (scanf("%7s", input) == 1){
			cleanInputBuffer();
			
			for(i = 0; input[i] != '\0'; i++){
				if(input[i] >= 'a' && input[i] <= 'z'){
					input[i] = input[i] - ('a' - 'A');
				}
			}
			
			for(i = 1; i < MAX_CURRENCIES; i++){
				if(strcmp(input, rates[i].code) == 0){
					currencyChoice = i;
				}
			}
			
			if (currencyChoice == -1){
				printf("That currency is not registered in the database.\n");
			}
		} else {
			cleanInputBuffer();
			printf("Please select a valid currency.\n");
		}
	}
	return currencyChoice;
}

/**
 * @brief 	Prints the prompt for the user to register account name details. [REQ-0009]
 *
 * @param[out]    accountName  String to be modified according to the user's input.
 */


int registerAccount(Transaction records[], int *count, int maxCount, char *filename){
    Transaction newTransaction = {0};
    double initialDeposit = 0.0;
    unsigned long long newNumber = 0;
	int registerResult = 0;	
    if(*count >= maxCount){
        printf("Transaction limit reached.\n");
        return registerResult;
    }

    printf("Register Account\n");
    printf("Account Name: ");
    getValidName(newTransaction.accountName, MAX_NAME_LEN + 1);

    printf("Initial Deposit: ");
    initialDeposit = getValidAmount();

    while(initialDeposit < 5000.00){
        printf("Minimum initial deposit is PHP 5000.00.\n");
        printf("Initial Deposit: ");
        initialDeposit = getValidAmount();
    }

    newNumber = getNextAccountNumber(records, *count);

    if(newNumber == 0){
        return registerResult;
    }

    if(!createAccountTransaction(&newTransaction, newNumber, initialDeposit)){
        return registerResult;
    }

    if(!saveTransaction(filename, &newTransaction)){
        printf("Cannot save account.\n");
        return registerResult;
    }
	
    records[*count] = newTransaction;
    (*count)++;

    printf("\nAccount registered successfully!\n");
    printf("Account Name: %s\n", newTransaction.accountName);
    printf("Account Number: %s\n", newTransaction.accountNumber);
    printf("Current Balance: %.2lf\n", newTransaction.currentBalance);

	registerResult = 1;
    return registerResult;
}


int setTransactionDateTime(Transaction *record){
    time_t currentTime = time(NULL);
    struct tm *dateTime = localtime(&currentTime);

    if(dateTime == NULL){
        return 0;
    }

    if(strftime(record->date, sizeof(record->date),
        "%d/%m/%Y", dateTime) == 0){
        return 0;
    }

    if(strftime(record->time, sizeof(record->time),
        "%H:%M:%S", dateTime) == 0){
        return 0;
    }

    return 1;
}

void calculateRiskFlags(DailyActivity activities[], int activityCount){
	int i = 0;

	for(i = 0; i < activityCount; i++){
		if(activities[i].totalDepositAmounts > 0){
			activities[i].turnoverVelocity = (activities[i].totalWithdrawalAmounts / activities[i].totalDepositAmounts) * 100.0;
			activities[i].velocityAvailable = 1;
		} else {
			activities[i].turnoverVelocity = 0.0;
			activities[i].velocityAvailable = 0;
		}

		if(activities[i].totalWithdrawalAmounts >= 500000.00 ||(activities[i].velocityAvailable && activities[i].turnoverVelocity >= 90.0)){
			activities[i].suspiciousFlag = 1;
		} else {
			activities[i].suspiciousFlag = 0;
		}
	}
}


int getDailyActivities(Transaction records[], int count, DailyActivity activities[]){
	int activityCount = 0;
	int i = 0, j = 0;

	for(i = 0; i < count; i++){
		for(j = 0; j < activityCount; j++){
			if(strcmp(records[i].accountNumber, activities[j].accountNumber) == 0 &&
			   strcmp(records[i].date, activities[j].date) == 0){
				break;
			}
		}

		if(j == activityCount){
			strcpy(activities[j].accountName, records[i].accountName);
			strcpy(activities[j].accountNumber, records[i].accountNumber);
			strcpy(activities[j].date, records[i].date);

			activities[j].totalWithdrawalAmounts = 0.0;
			activities[j].numberOfDeposits = 0;
			activities[j].totalDepositAmounts = 0.0;
			activities[j].turnoverVelocity = 0.0;
			activities[j].velocityAvailable = 0;
			activities[j].suspiciousFlag = 0;

			activityCount++;
		}

		if(strcmp(records[i].type, "deposit") == 0){
			activities[j].numberOfDeposits++;
			activities[j].totalDepositAmounts += records[i].amount;
		} else if(strcmp(records[i].type, "withdrawal") == 0){
			activities[j].totalWithdrawalAmounts += records[i].amount;
		}
	}

	return activityCount;
}


void sortDailyActivities(DailyActivity activities[], int activityCount){
	DailyActivity temp;
	static int dateValues[MAX_TRANSACTIONS];

	int i = 0, j = 0;
	int day = 0, month = 0, year = 0;
	int tempDate = 0;
	int swapFlag = 0;

	for(i = 0; i < activityCount; i++){
		sscanf(activities[i].date, "%2d/%2d/%4d", &day, &month, &year);
		dateValues[i] = year * 10000 + month * 100 + day;
	}

	for(i = 0; i < activityCount; i++){
		for(j = i + 1; j < activityCount; j++){
			swapFlag = 0;

			if(dateValues[j] < dateValues[i]){
				swapFlag = 1;
			} else if(dateValues[j] == dateValues[i]){
				if(activities[j].suspiciousFlag > activities[i].suspiciousFlag){
					swapFlag = 1;
				} else if(activities[j].suspiciousFlag == activities[i].suspiciousFlag && strcmp(activities[j].accountNumber, activities[i].accountNumber) < 0){
					swapFlag = 1;
				}
			}

			if(swapFlag){
				temp = activities[i];
				activities[i] = activities[j];
				activities[j] = temp;

				tempDate = dateValues[i];
				dateValues[i] = dateValues[j];
				dateValues[j] = tempDate;
			}
		}
	}
}

int generateSuspiciousReport(Transaction records[], int count){
	static DailyActivity activities[MAX_TRANSACTIONS];
	int activityCount = 0;

	if(count <= 0 || count > MAX_TRANSACTIONS){
		printf("No valid transactions available.\n");
		return 0;
	}

	activityCount = getDailyActivities(records, count, activities);

	calculateRiskFlags(activities, activityCount);

	sortDailyActivities(activities, activityCount);

	return exportSuspiciousReport(activities, activityCount);
}


/**
 * @brief Deposits a positive amount into the user's account,
 *        converting foreign currencies into PHP when necessary. [REQ-0004], [REQ-0008]
 *
 * @param[in] accountName   Name of the account holder.
 * @param[in,out] balance   Current account balance.
 * @param[in] rates         Array of currency codes and exchange rates.
 *
 * @post The account balance is increased by the value
 *       of the deposited amount.
 */


int depositAmount(Transaction records[], int *count, int maxCount, char *filename, Currency rates[MAX_CURRENCIES]) {
    Transaction *account = NULL;
    Transaction newTransaction;
    char accountNumber[12] = {0};
    double valDeposit = 0.0;
    double amountPHP = 0.0;
    int currencyChoice = -1;
    int status = 1;

    if (*count >= maxCount) {
        printf("Transaction limit reached.\n");
        status = 0;
    }

    if (status) {
        printf("Deposit Amount\n");
        printf("Account Number: ");

        if (scanf("%11s", accountNumber) != 1) {
            status = 0;
        }
        cleanInputBuffer();
    }

    if (status) {
        if (strlen(accountNumber) != 10 ||
            stringToAccountNumber(accountNumber) == 0) {
            printf("Invalid account number.\n");
            status = 0;
        }
    }

    if (status) {
        account = findAccount(records, *count, accountNumber);

        if (account == NULL) {
            printf("Account not found.\n");
            status = 0;
        }
    }

    if (status) {
        printf("Account Name: %s\n", account->accountName);
        printf("Current Balance: %.2lf\n", account->currentBalance);

        currencyChoice = getValidCurrency(rates);

        printf("Deposit Amount: ");
        valDeposit = getValidAmount();

        amountPHP = round(convertCurrency(valDeposit, currencyChoice, 1, rates) * 100.0) / 100.0;
        while (valDeposit <= 0 || amountPHP <= 0) {
            printf("Please enter a valid amount.\n");
            valDeposit = getValidAmount();
            amountPHP = round(convertCurrency(valDeposit, currencyChoice, 1, rates) * 100.0) / 100.0;
        }

        newTransaction = *account;

        strcpy(newTransaction.currency, "PHP");
        strcpy(newTransaction.type, "deposit");
        newTransaction.amount = amountPHP;
        newTransaction.currentBalance = account->currentBalance + amountPHP;

        if (!setTransactionDateTime(&newTransaction)) {
            status = 0;
        }
    }

    if (status) {
        if (!saveTransaction(filename, &newTransaction)) {
            printf("Cannot save deposit.\n");
            status = 0;
        }
    }

    if (status) {
        records[*count] = newTransaction;
        (*count)++;

        printf("\nDeposit Amount = %.2lf %s\n",
               valDeposit, rates[currencyChoice].code);
        printf("Updated Balance = %.2lf\n", newTransaction.currentBalance);
    }

    return status;
}

/**
 * @brief Withdraws an amount from the user's account,
 *        converting foreign currencies into PHP when necessary. [REQ-0005], [REQ-0008]
 *
 * @param[in] accountName   Name of the account holder.
 * @param[in,out] balance   Current account balance.
 * @param[in] rates         Array of currency codes and exchange rates.
 *
	 * @post The account balance is decreased by the value
	 *       of the validated withdrawal amount.
 *
 * @warning Withdrawals exceeding the available balance are rejected.
 */
 
int withdrawAmount(Transaction records[], int *count, int maxCount,
                   char *filename, Currency rates[MAX_CURRENCIES]) {
    Transaction *account = NULL;
    Transaction newTransaction;
    char accountNumber[12] = {0};
    double valWithdraw = -1.0;
    double amountPHP = 0.0;
    int currencyChoice = -1;
    int status = 1;

    if (*count >= maxCount) {
        printf("Transaction limit reached.\n");
        status = 0;
    }

    if (status) {
        printf("Withdraw Amount\n");
        printf("Account Number: ");

        if (scanf("%11s", accountNumber) != 1) {
            status = 0;
        }
        cleanInputBuffer();
    }

    if (status) {
        if (strlen(accountNumber) != 10 ||
            stringToAccountNumber(accountNumber) == 0) {
            printf("Invalid account number.\n");
            status = 0;
        }
    }

    if (status) {
        account = findAccount(records, *count, accountNumber);

        if (account == NULL) {
            printf("Account not found.\n");
            status = 0;
        }
    }

	if (status) {
        printf("Account Name: %s\n", account->accountName);
        printf("Current Balance: %.2lf\n", account->currentBalance);

        if (account->currentBalance <= 0) {
            printf("Insufficient balance to withdraw.\n");
            status = 0;
        } else {
            currencyChoice = getValidCurrency(rates);

            printf("\nWithdraw Amount: ");
            valWithdraw = getValidAmount();

            amountPHP = round(convertCurrency(valWithdraw, currencyChoice, 1, rates) * 100.0) / 100.0;
            while (valWithdraw <= 0 || amountPHP <= 0 || amountPHP > account->currentBalance) {
                printf("Please enter a valid amount.\n");
                valWithdraw = getValidAmount();
                amountPHP = round(convertCurrency(valWithdraw, currencyChoice, 1, rates) * 100.0) / 100.0;
            }

            newTransaction = *account;
            strcpy(newTransaction.currency, "PHP");
            strcpy(newTransaction.type, "withdrawal");
            newTransaction.amount = amountPHP;
            newTransaction.currentBalance = account->currentBalance - amountPHP;

            if (!setTransactionDateTime(&newTransaction)) {
                status = 0;
            }
        }
    }

    if (status) {
        if (!saveTransaction(filename, &newTransaction)) {
            printf("Cannot save withdrawal.\n");
            status = 0;
        }
    }
	
    if (status) {
        records[*count] = newTransaction;
        (*count)++;

        printf("\n***\n");
        printf("Account Name = %s\n", account->accountName);
        printf("Withdraw Amount = %.2lf %s\n", valWithdraw, rates[currencyChoice].code);
        printf("Updated Balance = %.2lf\n", newTransaction.currentBalance);
    }

    return status;
}

/**
 * @brief Converts a specified amount between two supported currencies. [REQ-0003]
 *
 * @param[in] rates  Array containing the current currency exchange rates.
 *
 * @pre Exchange rates must be positive and valid.
 *
 * @post Displays the converted amount and allows the user
 *       to perform additional currency conversions.
 *
 * @note Currency exchanges do not modify the account balance.
 */

void currencyExchange(Currency rates[MAX_CURRENCIES]){
	double source = 0.0;
	double result = 0.0;
	int sourceChoice = -1;
	int targetChoice = -1;
	int runningFlag = 1;
	
	while(runningFlag){
		printf("Foreign Currency Exchange\n");
		printf("Source Currency Option:\n");
		printExchangeMenu();
		printf("Source Currency: ");
		sourceChoice = getValidChoiceClean(1,6);
		
		printf("Source Amount: ");	
		source = getValidAmount();
		while (source <= 0) {
			printf("Please enter a valid amount: ");
			source = getValidAmount();
		}		
		
		printf("\nExchanged Currency Options:\n");
		printExchangeMenu();
		
		printf("Exchange Currency: ");
		targetChoice = getValidChoiceClean(1,6);
		
		result = convertCurrency(source, sourceChoice, targetChoice, rates);
		printf("\n***\n");
		printf("Exchange Currency = %d\n", targetChoice);
		printf("Exchange Amount = %.2lf\n\n", result);	
		
		printf("Convert another currency (Y/N)? . . . ");
		runningFlag = getValidYN();
	}
}

/**
 * @brief Records the user's input for a global currency's exchange value. [REQ-0001] [REQ-0002]
 * 
 */

void recordExchangeRates(Currency rates[MAX_CURRENCIES]){
    int choice = -1;
    double currency = 0.0;
    printf("Record Exchange Rate\n\n");
    printExchangeMenu();
    printf("Select Foreign Currency: ");
    
    choice = getValidChoiceClean(2,6);
    if(choice){
        printf("Exchange Rate: ");
        currency = getValidAmount();
			while (currency <= 0) {
				printf("Please enter a positive exchange rate: ");
				currency = getValidAmount();
			}
        rates[choice].rate = currency;
        printf("\n***\n");
        printf("Select Foreign Currency = [%d]\n", choice);
        printf("Exchange Rate = %.2lf\n", currency);
    }
}

/**
 * @brief Calculates and displays a daily compound-interest projection
 *        using a fixed annual interest rate of 5 percent. [REQ-0006] [REQ-0007]
 *
 * @param[in] accountName  Name of the account holder.
 * @param[in] balance      Starting account balance.
 *
 * @note Daily interest is rounded to two decimal places before
 *       being added to the projected balance.
 *
 * @post Displays the daily interest and projected balance
 *       without modifying the actual account balance.
 */

int showInterestAmount(Transaction records[], int count){
    Transaction *account;
    char accountNumber[12] = {0};
    int numDays = 0;
    int i = 0;
    double annualRate = 0.05;
    double dailyInterest = 0.0;
    double eodBalance = 0.0;

    printf("Show Interest Amount\n");
    printf("Account Number: ");

    if(scanf("%11s", accountNumber) != 1){
        cleanInputBuffer();
        return 0;
    }

    cleanInputBuffer();

    if(strlen(accountNumber) != 10 ||
       stringToAccountNumber(accountNumber) == 0){
        printf("Invalid account number.\n");
        return 0;
    }

    account = findAccount(records, count, accountNumber);

    if(account == NULL){
        printf("Account not found.\n");
        return 0;
    }

    eodBalance = account->currentBalance;

    printf("Account Name: %s\n", account->accountName);
    printf("Current Balance: %.2lf\n", eodBalance);
    printf("Currency: PHP\n");
    printf("Interest Rate: 5%%\n\n");

    printf("Total number of days: ");
    numDays = getValidChoiceClean(1, 3650);

    printf("\n%-5s | %-10s | %-10s\n", "Day", "Interest", "Balance");

    for(i = 1; i <= numDays; i++){
        dailyInterest = round(eodBalance * (annualRate / 365) * 100.0) / 100.0;
        eodBalance += dailyInterest;

        printf("%-5d | %-10.2lf | %-10.2lf\n",
               i, dailyInterest, eodBalance);
    }
    return 1;
}

void generateReportsMenu(Transaction records[], int *count, int maxCount, char *filename){
	int runningFlag = 1;
	int choice = 0;
	int loadedCount = 0;
	
	while (runningFlag){
		printReportMenu();
		printf("Enter choice: ");
		choice = getValidChoiceClean(0,2);
		
		switch(choice){
			case 1:
				printf("Processing dataset...\n");
				loadedCount = loadTransactions(filename, records, maxCount);
				
				if (loadedCount < 0){
					printf("Error in loading dataset.\n");
				} else {
					*count = loadedCount;
					printf("Dataset loaded! %d PHP transactions.\n", *count);
				}
				break;
			case 2:
				if(*count <= 0){
					printf("There is no currently loaded dataset.\n");
				} else {
					generatePrivilegeReport(records, *count);
					generateInterestReport(records, *count);
					generateSuspiciousReport(records, *count);
				}
				break;
			case 0:
				runningFlag = 0;
				break;
		}
	}
}


long long getTransactionDateValue(Transaction *record){
	int dateDay = 0, dateMonth = 0, dateYear = 0;
	int hour = 0, minute = 0, second = 0;
	long long dateValue = 0;

	sscanf(record->date, "%2d/%2d/%4d", &dateDay, &dateMonth, &dateYear);
	sscanf(record->time, "%2d:%2d:%2d", &hour, &minute, &second);

	dateValue = dateYear;
	dateValue = dateValue * 100 + dateMonth;
	dateValue = dateValue * 100 + dateDay;
	dateValue = dateValue * 100 + hour;
	dateValue = dateValue * 100 + minute;
	dateValue = dateValue * 100 + second;

	return dateValue;
}

int getLatestAccountIndexes(Transaction records[], int count, int accountIndexes[]){
	static long long accountDates[MAX_TRANSACTIONS];
	long long dateValue = 0;
	int accountCount = 0;
	int i = 0, j = 0;

	for(i = 0; i < count; i++){
		dateValue = getTransactionDateValue(records + i);

		for(j = 0; j < accountCount; j++){
			if(strcmp(records[i].accountNumber,
				records[accountIndexes[j]].accountNumber) == 0){
				break;
			}
		}

		if(j == accountCount){
			accountIndexes[accountCount] = i;
			accountDates[accountCount] = dateValue;
			accountCount++;
		} else if(dateValue >= accountDates[j]){
			accountIndexes[j] = i;
			accountDates[j] = dateValue;
		}
	}

	return accountCount;
}

double calculateExpectedInterest(double startingBalance){
	double balance = startingBalance;
	double dailyInterest = 0.0;
	int day = 0;

	for(day = 0; day < 30; day++){
		dailyInterest = round(balance * (0.05 / 365.0) * 100.0) / 100.0;
		balance += dailyInterest;
	}

	return round((balance - startingBalance) * 100.0) / 100.0;
}

int filterInterestAccounts(Transaction records[], int accountIndexes[],
						   int accountCount, double expectedInterest[]){
	int qualifiedCount = 0;
	int i = 0;

	for(i = 0; i < accountCount; i++){
		if(records[accountIndexes[i]].currentBalance >= 10000.00){
			expectedInterest[qualifiedCount] =
				calculateExpectedInterest(records[accountIndexes[i]].currentBalance);

			accountIndexes[qualifiedCount] = accountIndexes[i];
			qualifiedCount++;
		}
	}

	return qualifiedCount;
}

void sortInterestAccounts(Transaction records[], int accountIndexes[],
						  double expectedInterest[], int qualifiedCount){
	int i = 0, j = 0;
	int tempIndex = 0;
	double tempInterest = 0.0;

	for(i = 0; i < qualifiedCount; i++){
		for(j = i + 1; j < qualifiedCount; j++){
			if(expectedInterest[j] > expectedInterest[i] ||
				(expectedInterest[j] == expectedInterest[i] &&
				strcmp(records[accountIndexes[j]].accountNumber,
				records[accountIndexes[i]].accountNumber) < 0)){

				tempIndex = accountIndexes[i];
				accountIndexes[i] = accountIndexes[j];
				accountIndexes[j] = tempIndex;

				tempInterest = expectedInterest[i];
				expectedInterest[i] = expectedInterest[j];
				expectedInterest[j] = tempInterest;
			}
		}
	}
}

int exportInterestReport(Transaction records[], int accountIndexes[],
						 double expectedInterest[], int qualifiedCount){
	FILE *file;
	Transaction *account;
	int i = 0;

	file = fopen("report2_interest_expense_projection.csv", "w");

	if(file == NULL){
		printf("Cannot create Report 2 CSV.\n");
		return 0;
	}

	fprintf(file, "AccountName,CurrentBalance,ExpectedInterest\n");

	printf("\nReport 2: Interest Expense Projection\n");
	printf("%-24s %-18s %-18s\n",
		"AccountName", "CurrentBalance", "ExpectedInterest");

	for(i = 0; i < qualifiedCount; i++){
		account = records + accountIndexes[i];

		fprintf(file, "%s,%.2f,%.2f\n",
			account->accountName,
			account->currentBalance,
			expectedInterest[i]);

		if(i < 2){
			printf("%-24s %-18.2f %-18.2f\n",
				account->accountName,
				account->currentBalance,
				expectedInterest[i]);
		}
	}

	if(fclose(file) != 0){
		printf("Error saving Report 2 CSV.\n");
		return 0;
	}

	printf("(Full table exported to report2_interest_expense_projection.csv)\n");
	printf("Eligible accounts: %d\n", qualifiedCount);

	return 1;
}


int exportSuspiciousReport(DailyActivity activities[], int activityCount){
	FILE *file;
	int i = 0;
	char *riskLevel;

	file = fopen("report3_suspicious_client_behavior.csv", "w");

	if(file == NULL){
		printf("Cannot create Report 3 CSV.\n");
		return 0;
	}
 
	fprintf(file, "AccountName,Date,TotalWithdrawalAmounts,NumberOfDeposits,TotalDepositAmounts,TurnoverVelocity (%%),RiskFlagLevel\n");

	printf("\nReport 3: Suspicious Client Behavior\n");
	printf("%-24s %-12s %18s %12s %18s %18s %-12s\n",
		"AccountName", "Date", "Withdrawals", "Deposits", "Deposit Total", "Velocity (%)", "Risk");

	for(i = 0; i < activityCount; i++){
		if(activities[i].suspiciousFlag){
			riskLevel = "Suspicious";
		} else {
			riskLevel = "Normal";
		}

		if(activities[i].velocityAvailable){
			fprintf(file, "%s,%s,%.2f,%d,%.2f,%.2f,%s\n",
				activities[i].accountName,
				activities[i].date,
				activities[i].totalWithdrawalAmounts,
				activities[i].numberOfDeposits,
				activities[i].totalDepositAmounts,
				activities[i].turnoverVelocity,
				riskLevel);
		} else {
			fprintf(file, "%s,%s,%.2f,%d,%.2f,N/A,%s\n",
				activities[i].accountName,
				activities[i].date,
				activities[i].totalWithdrawalAmounts,
				activities[i].numberOfDeposits,
				activities[i].totalDepositAmounts,
				riskLevel);
		}

		if(i < 2){
			if(activities[i].velocityAvailable){
				printf("%-24s %-12s %18.2f %12d %18.2f %18.2f %-12s\n",
					activities[i].accountName,
					activities[i].date,
					activities[i].totalWithdrawalAmounts,
					activities[i].numberOfDeposits,
					activities[i].totalDepositAmounts,
					activities[i].turnoverVelocity,
					riskLevel);
			} else {
				printf("%-24s %-12s %18.2f %12d %18.2f %18s %-12s\n",
					activities[i].accountName,
					activities[i].date,
					activities[i].totalWithdrawalAmounts,
					activities[i].numberOfDeposits,
					activities[i].totalDepositAmounts,
					"N/A",
					riskLevel);
			}
		}
	}

	fclose(file);

	printf("(Full table exported to report3_suspicious_client_behavior.csv)\n");
	printf("Daily activities: %d\n", activityCount);

	return 1;
}


int generateInterestReport(Transaction records[], int count){
	static int accountIndexes[MAX_TRANSACTIONS];
	static double expectedInterest[MAX_TRANSACTIONS];
	int accountCount = 0;
	int qualifiedCount = 0;

	if(count <= 0 || count > MAX_TRANSACTIONS){
		printf("No valid transactions available.\n");
		return 0;
	}

	accountCount = getLatestAccountIndexes(records, count, accountIndexes);

	qualifiedCount = filterInterestAccounts(records, accountIndexes,
		accountCount, expectedInterest);

	sortInterestAccounts(records, accountIndexes, expectedInterest, qualifiedCount);

	return exportInterestReport(records, accountIndexes, expectedInterest, qualifiedCount);
}


int getDaysInMonth(int month, int year){
    int days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

    if(month == 2 && (year % 400 == 0 ||
       (year % 4 == 0 && year % 100 != 0))){
        return 29;
    }

    return days[month - 1];
}

int dateToDayCount(char *date){
    int day = 0, month = 0, year = 0;
    int i = 0;
    int total = 0;
    int previousYear = 0;

    sscanf(date, "%2d/%2d/%4d", &day, &month, &year);

    previousYear = year - 1;
    total = previousYear * 365 + previousYear / 4
            - previousYear / 100 + previousYear / 400;

    for(i = 1; i < month; i++){
        total += getDaysInMonth(i, year);
    }

    return total + day;
}

double calculateAverageDailyBalance(Transaction records[], int count,
                                    char *accountNumber, int month, int year){
    int day = 0, i = 0;
    int days = getDaysInMonth(month, year);
    double sumOfBalances = 0.0;
    double endOfDayBalance = 0.0;
    long long endOfDay = 0;
    long long latestTransaction = -1;
    long long transactionDate = 0;

    for(day = 1; day <= days; day++){
        endOfDay = year;
        endOfDay = endOfDay * 100 + month;
        endOfDay = endOfDay * 100 + day;
        endOfDay = endOfDay * 100 + 23;
        endOfDay = endOfDay * 100 + 59;
        endOfDay = endOfDay * 100 + 59;

        endOfDayBalance = 0.0;
        latestTransaction = -1;

        for(i = 0; i < count; i++){
            if(strcmp(records[i].accountNumber, accountNumber) == 0){
                transactionDate = getTransactionDateValue(records + i);

                if(transactionDate <= endOfDay &&
                   transactionDate >= latestTransaction){
                    latestTransaction = transactionDate;
                    endOfDayBalance = records[i].currentBalance;
                }
            }
        }

        sumOfBalances += endOfDayBalance;
    }

    return sumOfBalances / days;
}

void sortPrivilegeAccounts(PrivilegeAccount accounts[], int qualifiedCount){
    PrivilegeAccount temp;
    int i = 0, j = 0;

    for(i = 0; i < qualifiedCount; i++){
        for(j = i + 1; j < qualifiedCount; j++){
            if(accounts[j].averageDailyBalance > accounts[i].averageDailyBalance ||
               (accounts[j].averageDailyBalance == accounts[i].averageDailyBalance &&
                strcmp(accounts[j].accountNumber, accounts[i].accountNumber) < 0)){
                temp = accounts[i];
                accounts[i] = accounts[j];
                accounts[j] = temp;
            }
        }
    }
}

int exportPrivilegeReport(PrivilegeAccount accounts[], int qualifiedCount){
    FILE *file;
    int i = 0;

    file = fopen("report1_banking_privilege_tiers.csv", "w");

    if(file == NULL){
        printf("Cannot create Report 1 CSV.\n");
        return 0;
    }

    fprintf(file, "AccountName,AccountAge,CurrentBalance,AverageDailyBalance,PrivilegeTier\n");

    printf("\nReport 1: Banking Privilege Tiers\n");
    printf("%-24s %-11s %-16s %-20s %-14s\n",
           "AccountName", "AccountAge", "CurrentBalance",
           "AverageDailyBalance", "PrivilegeTier");

    for(i = 0; i < qualifiedCount; i++){
        fprintf(file, "%s,%d,%.2f,%.2f,%s\n",
                accounts[i].accountName,
                accounts[i].accountAge,
                accounts[i].currentBalance,
                accounts[i].averageDailyBalance,
                accounts[i].privilegeTier);

        if(i < 2){
            printf("%-24s %-11d %-16.2f %-20.2f %-14s\n",
                   accounts[i].accountName,
                   accounts[i].accountAge,
                   accounts[i].currentBalance,
                   accounts[i].averageDailyBalance,
                   accounts[i].privilegeTier);
        }
    }

    if(fclose(file) != 0){
        printf("Error saving Report 1 CSV.\n");
        return 0;
    }

    printf("(Full table exported to report1_banking_privilege_tiers.csv)\n");
    printf("Qualified accounts: %d\n", qualifiedCount);
    return 1;
}

int generatePrivilegeReport(Transaction records[], int count){
    static int accountIndexes[MAX_TRANSACTIONS];
    static PrivilegeAccount accounts[MAX_TRANSACTIONS];
    int accountCount = 0;
    int qualifiedCount = 0;
    int i = 0, j = 0;
    int latestIndex = 0;
    int earliestDay = 0;
    int currentDay = 0;
    int referenceDay = 0;
    int reportMonth = 0, reportYear = 0;
    int ignoredDay = 0;
    double averageBalance = 0.0;
    long long latestDate = -1;
    long long transactionDate = 0;
    Transaction *account;

    if(count <= 0 || count > MAX_TRANSACTIONS){
        printf("No valid transactions available.\n");
        return 0;
    }

    for(i = 0; i < count; i++){
        transactionDate = getTransactionDateValue(records + i);

        if(transactionDate >= latestDate){
            latestDate = transactionDate;
            latestIndex = i;
        }
    }

    referenceDay = dateToDayCount(records[latestIndex].date);

    sscanf(records[latestIndex].date, "%2d/%2d/%4d",
           &ignoredDay, &reportMonth, &reportYear);

    reportMonth--;

    if(reportMonth == 0){
        reportMonth = 12;
        reportYear--;
    }

    accountCount = getLatestAccountIndexes(records, count, accountIndexes);

    for(i = 0; i < accountCount; i++){
        account = records + accountIndexes[i];

        averageBalance = calculateAverageDailyBalance(
            records, count, account->accountNumber, reportMonth, reportYear
        );

        if(averageBalance >= 100000.00){
            strcpy(accounts[qualifiedCount].accountName, account->accountName);
            strcpy(accounts[qualifiedCount].accountNumber, account->accountNumber);

            accounts[qualifiedCount].currentBalance = account->currentBalance;
            accounts[qualifiedCount].averageDailyBalance = averageBalance;

            earliestDay = referenceDay;

            for(j = 0; j < count; j++){
                if(strcmp(records[j].accountNumber, account->accountNumber) == 0){
                    currentDay = dateToDayCount(records[j].date);

                    if(currentDay < earliestDay){
                        earliestDay = currentDay;
                    }
                }
            }

            accounts[qualifiedCount].accountAge = referenceDay - earliestDay;

            if(averageBalance < 500000.00){
                strcpy(accounts[qualifiedCount].privilegeTier, "Silver");
            } else if(averageBalance < 1000000.00){
                strcpy(accounts[qualifiedCount].privilegeTier, "Gold");
            } else {
                strcpy(accounts[qualifiedCount].privilegeTier, "Platinum");
            }

            qualifiedCount++;
        }
    }

    sortPrivilegeAccounts(accounts, qualifiedCount);

    return exportPrivilegeReport(accounts, qualifiedCount);
}


/**
 * @brief Dispatches the selected menu transaction to its
 *        corresponding function.
 *
 * @param[in] choice          Selected transaction menu option.
 * @param[in,out] accountName Account name, modified during registration.
 * @param[in,out] balance     Account balance, modified by transactions.
 * @param[in,out] rates       Exchange rates, modified when recording rates.
 */
 
void transactionSelect(int choice, Currency rates[MAX_CURRENCIES],
                       Transaction records[], int *count,
                       int maxCount, char *filename){
	switch(choice){
		case 1:
			registerAccount(records, count, maxCount, filename);
			break;
		case 2:
			depositAmount(records, count, maxCount, filename, rates);
			break;
		case 3:
			withdrawAmount(records, count, maxCount, filename, rates);
			break;
		case 4:
			currencyExchange(rates);
			break;
		case 5:
			recordExchangeRates(rates);
			break;
		case 6:
			showInterestAmount(records, *count);
			break;
		case 7:
			generateReportsMenu(records, count, maxCount, filename);
			break;			
		default:
			printf("ERROR: Invalid selection.\n");
			break;
	}
}
 
/**
 * @brief Main program loop, constantly checks for user's valid input to navigate menu.
 * 
 */ 
 
int main(){

	// +1 so that the rates are no longer zero indexed, which align with user input 1-6. index 0 is a placeholder
	Currency globalCurrency[MAX_CURRENCIES] = {
		{"",    -1.00},
		{"PHP",  1.00},
		{"USD", 62.00},
		{"JPY",  0.40},
		{"GBP", 84.00},
		{"EUR", 72.00},
		{"CNY",  9.00}
	};
	
	static Transaction records[MAX_TRANSACTIONS];
	int runningFlag = 1;
	int choice = -1;
	int transactionCount = 0;
	char filename[] = "bank_transactions.csv";
	
	transactionCount = loadTransactions(filename, records, MAX_TRANSACTIONS);
	
	if(transactionCount < 0){
		printf("Error in loading the transactions.\n");
		return 1;
	}
	
	printf("Successfully loaded %d transactions.\n", transactionCount);
	
	while(runningFlag){
		printNavMenu();
		choice = getValidChoice(0, 7);
		if (choice){
				transactionSelect(choice, globalCurrency, records,
								  &transactionCount, MAX_TRANSACTIONS, filename);
			if(choice != 4 && choice != 7){
				printf("Back to Main Menu (Y/N): ");
				runningFlag = getValidYN();
			}
		} else {
			runningFlag = 0;
		}
	}
	printf("Exiting..\n");
	return 0;
}

