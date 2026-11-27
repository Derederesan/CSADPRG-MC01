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
 * Developed by Team Isy Does It for CSADPRG MCO2.
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

/** @brief Maximum length that can be given to an account name string. */
#define MAX_NAME_LEN 63

/** @brief Maximum number of transaction records that can be loaded in memory. */
#define MAX_TRANSACTIONS 20000

// 7, because index 0 is a placeholder
/** @brief Maximum size of the global currency array, including index 0 as a placeholder. */
#define MAX_CURRENCIES 7

/**
 * @brief Struct that holds the details of a single bank transaction record.
 */
typedef struct {
    char accountName[64];     /**< Name of the account holder. */
    char accountNumber[11];   /**< 10-digit account number string. */
    char date[11];            /**< Date of the transaction in DD/MM/YYYY format. */
    char time[9];             /**< Time of the transaction in HH:MM:SS format. */
    char currency[4];         /**< Currency code of the transaction. */
    double amount;            /**< Transaction amount. */
    char type[11];            /**< Transaction type, deposit or withdrawal. */
    double currentBalance;    /**< Account balance after the transaction. */
} Transaction;

/**
 * @brief Struct that holds the values for both the currency code and exchange rate.
 */
typedef struct {
    char code[4];             /**< Currency code string. ie, USD. */
    double rate;              /**< PHP-relative exchange rate of the currency. */
} Currency;

/**
 * @brief Struct that holds the daily activity details of an account for Report 3.
 */
typedef struct {
	char accountName[64];          /**< Name of the account holder. */
	char accountNumber[11];        /**< 10-digit account number string. */
	char date[11];                 /**< Date of the daily activity. */
	double totalWithdrawalAmounts; /**< Total withdrawal amount for the day. */
	int numberOfDeposits;          /**< Number of deposits made for the day. */
	double totalDepositAmounts;    /**< Total deposit amount for the day. */
	double turnoverVelocity;       /**< Turnover velocity percentage for the day. */
	int velocityAvailable;         /**< 1 if total deposits is greater than 0, 0 if N/A. */
	int suspiciousFlag;            /**< 1 if flagged as Suspicious, 0 if Normal. */
} DailyActivity;

/**
 * @brief Struct that holds the privilege tier details of a qualified account for Report 1.
 */
typedef struct {
    char accountName[64];          /**< Name of the account holder. */
    char accountNumber[11];        /**< 10-digit account number string. */
    int accountAge;                /**< Age of the account in days. */
    double currentBalance;         /**< Latest current balance of the account. */
    double averageDailyBalance;    /**< Average daily balance of the account for the report month. */
    char privilegeTier[10];        /**< Privilege tier string: Silver, Gold, or Platinum. */
} PrivilegeAccount;

/**
 * @brief Struct that holds the balance depletion details of a watchlist account for Report 4.
 */
typedef struct {
    char accountName[64];          /**< Name of the account holder. */
    char accountNumber[11];        /**< 10-digit account number string. */
    double balance30DaysAgo;       /**< Account balance 30 days before the latest date. */
    double currentBalance;         /**< Latest current balance of the account. */
    double balanceDrop;            /**< Difference between balance 30 days ago and current balance. */
    double depletionRate;          /**< Depletion rate percentage of the account. */
    char riskLevel[10];            /**< Risk level string: Moderate, High, or Critical. */
} DepletionAccount;


/*
================================================================
===================== FUNCTION PROTOTYPES ======================
================================================================
*/

// INPUT HANDLING, ERROR VALIDATION
void cleanInputBuffer();
int getValidChoice(int lb, int ub);
int getValidChoiceClean(int lb, int ub);
double getValidAmount();
void getValidName(char *nameBuffer, int maxLen);
int getValidYN();
int getValidCurrency(Currency rates[MAX_CURRENCIES]);

// DATE AND TIME HELPER FUNCTIONS
int setTransactionDateTime(Transaction *record);
long long getTransactionDateValue(Transaction *record);
int getDaysInMonth(int month, int year);
int dateToDayCount(char *date);

// FILE I/O ++ ACCOUNT AND TRANSACTION HELPER FUNCTIONS
int loadTransactions(char *filename, Transaction records[], int maxCount);
int saveTransaction(char *filename, Transaction *record);
int validateTransaction(Transaction *record);
int createAccountTransaction(Transaction *record, unsigned long long number, double deposit);
Transaction *findAccount(Transaction records[], int count, char *accountNumber);
unsigned long long stringToAccountNumber(char *str);
unsigned long long getNextAccountNumber(Transaction records[], int count);
int getLatestAccountIndexes(Transaction records[], int count, int accountIndexes[]);

// PRINTABLE MENUS
void printNavMenu();
void printExchangeMenu();
void printReportMenu();

void transactionSelect(int choice, Currency rates[MAX_CURRENCIES],
                       Transaction records[], int *count,
                       int maxCount, char *filename);
void generateReportsMenu(Transaction records[], int *count, int maxCount, char *filename);

// BANKING OPERATIONS
int registerAccount(Transaction records[], int *count, int maxCount, char *filename);
int depositAmount(Transaction records[], int *count, int maxCount,
                  char *filename, Currency rates[MAX_CURRENCIES]);
int withdrawAmount(Transaction records[], int *count, int maxCount,
                   char *filename, Currency rates[MAX_CURRENCIES]);
int showInterestAmount(Transaction records[], int count);

// CURRENCY EXCHANGE
double convertCurrency(double amount, int source, int target, Currency rates[MAX_CURRENCIES]);
void currencyExchange(Currency rates[MAX_CURRENCIES]);
void recordExchangeRates(Currency rates[MAX_CURRENCIES]);

	/*
	===================== REPORTING, ANALYTICS ======================
	*/

		// Interest Report
		double calculateExpectedInterest(double startingBalance);
		int filterInterestAccounts(Transaction records[], int accountIndexes[], int accountCount, double expectedInterest[]);
		void sortInterestAccounts(Transaction records[], int accountIndexes[], double expectedInterest[], int qualifiedCount);
		int exportInterestReport(Transaction records[], int accountIndexes[], double expectedInterest[], int qualifiedCount);
		int generateInterestReport(Transaction records[], int count);

		// Suspicious Activity Report
		int getDailyActivities(Transaction records[], int count, DailyActivity activities[]);
		void calculateRiskFlags(DailyActivity activities[], int activityCount);
		void sortDailyActivities(DailyActivity activities[], int activityCount);
		int exportSuspiciousReport(DailyActivity activities[], int activityCount);
		int generateSuspiciousReport(Transaction records[], int count);

		// Privilege Account Report
		double calculateAverageDailyBalance(Transaction records[], int count,
											char *accountNumber, int month, int year);
		void sortPrivilegeAccounts(PrivilegeAccount accounts[], int qualifiedCount);
		int exportPrivilegeReport(PrivilegeAccount accounts[], int qualifiedCount);
		int generatePrivilegeReport(Transaction records[], int count);

		// Depletion Account Report
		void sortDepletionAccounts(DepletionAccount accounts[], int qualifiedCount);
		int exportDepletionReport(DepletionAccount accounts[], int qualifiedCount);
		int generateDepletionReport(Transaction records[], int count);

		// Summary Export
		int generateSummaryJson(Transaction records[], int count);

	/*
	===================== REPORTING, ANALYTICS ======================
	*/

/*
================================================================
===================== FUNCTION PROTOTYPES ======================
================================================================
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

/**
 * @brief Loads and validates PHP transaction records from a CSV file into the records array.
 *
 * @param[in]  filename  String containing the CSV filename to be opened.
 * @param[out] records   Array of Transaction structs that holds the accepted transaction records.
 * @param[in]  maxCount  Maximum number of accepted transactions that can be loaded.
 *
 * @warning Non-PHP rows are filtered out, and invalid rows print an error message with the row number.
 *
 * @return The total number of accepted transaction rows, or -1 if the CSV file cannot be opened.
 */

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

/**
 * @brief Finds the latest transaction record matching a given account number.
 *
 * @param[in] records        Array of Transaction structs containing loaded transaction records.
 * @param[in] count          Total number of transactions in the records array.
 * @param[in] accountNumber  Account number string to search for.
 *
 * @return Pointer to the latest matching Transaction record, or NULL if not found.
 */

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

/**
 * @brief Converts a 10-digit account number string into an unsigned long long value.
 *
 * @param[in] str  String containing the account number to be converted.
 *
 * @return The converted numeric account number, or 0 if the string is invalid.
 */

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

/**
 * @brief Finds the highest existing account number in the records array and calculates the next one.
 *
 * @param[in] records  Array of Transaction structs containing loaded transaction records.
 * @param[in] count    Total number of transactions in the records array.
 *
 * @return The next available account number, or 0 if an invalid account number is found or the limit is reached.
 */

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

/**
 * @brief Saves a single transaction record to the end of the CSV file.
 *
 * @param[in] filename  String containing the CSV filename to open.
 * @param[in] record    Pointer to the Transaction struct to be written to the file.
 *
 * @return 1 if saved successfully, 0 if an error occurs while opening, writing, or closing the file.
 */

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

/**
 * @brief Validates the fields of a single transaction record.
 *
 * @param[in] record  Pointer to the Transaction struct to be validated.
 *
 * @return 1 if all fields of the transaction are valid, 0 otherwise.
 */

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

/**
 * @brief Sets up the account number, currency, deposit type, balance, and current date and time
 *        for a new account transaction record.
 *
 * @param[in,out] record   Pointer to the Transaction struct to be filled.
 * @param[in]     number   Numeric account number to be converted into the 10-digit string.
 * @param[in]     deposit  Initial deposit amount in PHP.
 *
 * @return 1 if created successfully, 0 if setting the date or time fails.
 */

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
 * @brief 	Prints the prompt for the user to register account name details and an initial deposit,
 *          then saves the new account into the records array and CSV file. [REQ-0009]
 *
 * @param[in,out] records   Array of Transaction structs that holds all loaded transactions.
 * @param[in,out] count     Pointer to the total number of loaded transactions.
 * @param[in]     maxCount  Maximum number of transactions that can be stored in records.
 * @param[in]     filename  String containing the CSV filename where the new account is saved.
 *
 * @post If valid, the new account transaction is saved to the CSV file and added to records.
 *
 * @return 1 if registered successfully, 0 if an error occurs or the transaction limit is reached.
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

/**
 * @brief Sets the current date and time onto a transaction record.
 *
 * @param[out] record  Pointer to the Transaction struct whose date and time will be modified.
 *
 * @return 1 if the date and time are set successfully, 0 otherwise.
 */

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

/**
 * @brief Calculates the turnover velocity and suspicious flag for each daily activity entry.
 *
 * @param[in,out] activities     Array of DailyActivity structs to be evaluated and updated.
 * @param[in]     activityCount  Total number of daily activity entries in the array.
 *
 * @post Each DailyActivity entry has its turnover velocity, velocity availability,
 *       and suspicious flag updated.
 */

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

/**
 * @brief Groups transaction records by account number and date to calculate daily deposit and withdrawal totals.
 *
 * @param[in]  records     Array of Transaction structs containing loaded transaction records.
 * @param[in]  count       Total number of transactions in the records array.
 * @param[out] activities  Array of DailyActivity structs filled with the grouped daily activity values.
 *
 * @return The total number of daily activity entries created.
 */

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

/**
 * @brief Sorts the daily activity entries by date, then by suspicious flag, then by account number.
 *
 * @param[in,out] activities     Array of DailyActivity structs to be sorted.
 * @param[in]     activityCount  Total number of daily activity entries in the array.
 *
 * @post The activities array is sorted for Report 3 output.
 */

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

/**
 * @brief Generates Report 3 (Suspicious Client Behavior) from the loaded transaction records.
 *
 * @param[in] records  Array of Transaction structs containing loaded transaction records.
 * @param[in] count    Total number of transactions in the records array.
 *
 * @return 1 if Report 3 is generated and exported successfully, 0 otherwise.
 */

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
 * @param[in,out] records   Array of Transaction structs that holds all loaded transactions.
 * @param[in,out] count     Pointer to the total number of loaded transactions.
 * @param[in]     maxCount  Maximum number of transactions that can be stored in records.
 * @param[in]     filename  String containing the CSV filename where the deposit is saved.
 * @param[in]     rates     Array of currency codes and exchange rates.
 *
 * @post The account balance is increased by the value
 *       of the deposited amount, and the new deposit transaction is saved.
 *
 * @return 1 if the deposit succeeds, 0 if an error occurs.
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

        double updatedBalance = round((account->currentBalance + amountPHP) * 100.0) / 100.0;
        if (updatedBalance == 0.0) {
            updatedBalance = 0.0;
        }
        newTransaction.currentBalance = updatedBalance;

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
 * @param[in,out] records   Array of Transaction structs that holds all loaded transactions.
 * @param[in,out] count     Pointer to the total number of loaded transactions.
 * @param[in]     maxCount  Maximum number of transactions that can be stored in records.
 * @param[in]     filename  String containing the CSV filename where the withdrawal is saved.
 * @param[in]     rates     Array of currency codes and exchange rates.
 *
 * @post The account balance is decreased by the value
 *       of the validated withdrawal amount, and the new withdrawal transaction is saved.
 *
 * @warning Withdrawals exceeding the available balance are rejected.
 *
 * @return 1 if the withdrawal succeeds, 0 if an error occurs.
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

            double updatedBalance = round((account->currentBalance - amountPHP) * 100.0) / 100.0;
            if (updatedBalance == 0.0) {
                updatedBalance = 0.0;
            }
            newTransaction.currentBalance = updatedBalance;

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
 * @param[in,out] rates  Array containing currency codes and exchange rates, modified when a new rate is recorded.
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
 * @param[in] records  Array of Transaction structs containing loaded transaction records.
 * @param[in] count    Total number of transactions in the records array.
 *
 * @note Daily interest is rounded to two decimal places before
 *       being added to the projected balance.
 *
 * @post Displays the daily interest and projected balance
 *       without modifying the actual account balance.
 *
 * @return 1 if the interest projection is displayed, 0 if the account is invalid or not found.
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

/**
 * @brief Handles the report generation submenu loop for loading the CSV file and generating all reports.
 *
 * @param[in,out] records   Array of Transaction structs that holds all loaded transactions.
 * @param[in,out] count     Pointer to the total number of loaded transactions.
 * @param[in]     maxCount  Maximum number of transactions that can be stored in records.
 * @param[in]     filename  String containing the CSV filename to load from.
 */

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
					generateDepletionReport(records, *count);
					generateSummaryJson(records, *count);
				}
				break;
			case 0:
				runningFlag = 0;
				break;
		}
	}
}

/**
 * @brief Converts a transaction's date and time strings into a single numeric value for comparison.
 *
 * @param[in] record  Pointer to the Transaction struct containing the date and time strings.
 *
 * @return A long long value formatted as YYYYMMDDHHMMSS.
 */

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

/**
 * @brief Finds the index of the latest transaction record for each unique account number.
 *
 * @param[in]  records         Array of Transaction structs containing loaded transaction records.
 * @param[in]  count           Total number of transactions in the records array.
 * @param[out] accountIndexes  Array of integers filled with the index of each account's latest transaction.
 *
 * @return The total number of unique accounts found.
 */

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

/**
 * @brief Calculates the expected 30-day compound interest for a given starting balance
 *        using a 5 percent annual interest rate.
 *
 * @param[in] startingBalance  Starting account balance in PHP.
 *
 * @note Daily interest is rounded to two decimal places before being added to the balance.
 *
 * @return The total expected interest after 30 days.
 */

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

/**
 * @brief Filters accounts with a current balance of at least PHP 10000.00
 *        and calculates their expected 30-day interest.
 *
 * @param[in]     records           Array of Transaction structs containing loaded transaction records.
 * @param[in,out] accountIndexes    Array of latest account indexes, modified to hold only qualified indexes.
 * @param[in]     accountCount      Total number of unique accounts in accountIndexes.
 * @param[out]    expectedInterest  Array filled with the expected interest of each qualified account.
 *
 * @return The total number of qualified accounts.
 */

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

/**
 * @brief Sorts qualified interest accounts by expected interest descending, then by account number.
 *
 * @param[in]     records           Array of Transaction structs containing loaded transaction records.
 * @param[in,out] accountIndexes    Array of qualified account indexes to be sorted.
 * @param[in,out] expectedInterest  Array of expected interest values to be sorted alongside accountIndexes.
 * @param[in]     qualifiedCount    Total number of qualified accounts.
 *
 * @post The accountIndexes and expectedInterest arrays are sorted for Report 2 output.
 */

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

/**
 * @brief Exports Report 2 (Interest Expense Projection) to a CSV file and displays the top rows.
 *
 * @param[in] records           Array of Transaction structs containing loaded transaction records.
 * @param[in] accountIndexes    Array of sorted qualified account indexes.
 * @param[in] expectedInterest  Array of sorted expected interest values.
 * @param[in] qualifiedCount    Total number of qualified accounts.
 *
 * @return 1 if exported successfully, 0 if creating or saving the CSV fails.
 */

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

/**
 * @brief Exports Report 3 (Suspicious Client Behavior) to a CSV file and displays the top rows.
 *
 * @param[in] activities     Array of sorted DailyActivity structs.
 * @param[in] activityCount  Total number of daily activity entries.
 *
 * @return 1 if exported successfully, 0 if creating the CSV fails.
 */

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

/**
 * @brief Generates Report 2 (Interest Expense Projection) from the loaded transaction records.
 *
 * @param[in] records  Array of Transaction structs containing loaded transaction records.
 * @param[in] count    Total number of transactions in the records array.
 *
 * @return 1 if Report 2 is generated and exported successfully, 0 otherwise.
 */

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

/**
 * @brief Returns the number of days in a specified month and year, checking for leap years.
 *
 * @param[in] month  Month integer from 1 to 12.
 * @param[in] year   Year integer.
 *
 * @return The total number of days in the given month.
 */

int getDaysInMonth(int month, int year){
    int days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

    if(month == 2 && (year % 400 == 0 ||
       (year % 4 == 0 && year % 100 != 0))){
        return 29;
    }

    return days[month - 1];
}

/**
 * @brief Converts a date string in DD/MM/YYYY format into a total day count value.
 *
 * @param[in] date  Date string in DD/MM/YYYY format.
 *
 * @return The total day count integer for the given date.
 */

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

/**
 * @brief Calculates the average daily balance of an account for a specified month and year.
 *
 * @param[in] records        Array of Transaction structs containing loaded transaction records.
 * @param[in] count          Total number of transactions in the records array.
 * @param[in] accountNumber  Account number string to evaluate.
 * @param[in] month          Month integer to evaluate.
 * @param[in] year           Year integer to evaluate.
 *
 * @return The average daily balance for the specified month and year.
 */

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

/**
 * @brief Sorts qualified privilege accounts by average daily balance descending, then by account number.
 *
 * @param[in,out] accounts        Array of PrivilegeAccount structs to be sorted.
 * @param[in]     qualifiedCount  Total number of qualified accounts in the array.
 *
 * @post The accounts array is sorted for Report 1 output.
 */

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

/**
 * @brief Exports Report 1 (Banking Privilege Tiers) to a CSV file and displays the top rows.
 *
 * @param[in] accounts        Array of sorted PrivilegeAccount structs.
 * @param[in] qualifiedCount  Total number of qualified accounts.
 *
 * @return 1 if exported successfully, 0 if creating or saving the CSV fails.
 */

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

/**
 * @brief Generates Report 1 (Banking Privilege Tiers) from the loaded transaction records.
 *
 * @param[in] records  Array of Transaction structs containing loaded transaction records.
 * @param[in] count    Total number of transactions in the records array.
 *
 * @return 1 if Report 1 is generated and exported successfully, 0 otherwise.
 */

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
 * @brief Sorts depletion watchlist accounts by depletion rate descending,
 *        then by balance drop descending, then by account number.
 *
 * @param[in,out] accounts        Array of DepletionAccount structs to be sorted.
 * @param[in]     qualifiedCount  Total number of watchlist accounts in the array.
 *
 * @post The accounts array is sorted for Report 4 output.
 */

void sortDepletionAccounts(DepletionAccount accounts[], int qualifiedCount){
    DepletionAccount temp;
    int i = 0, j = 0;

    for(i = 0; i < qualifiedCount; i++){
        for(j = i + 1; j < qualifiedCount; j++){
            if(accounts[j].depletionRate > accounts[i].depletionRate ||
               (accounts[j].depletionRate == accounts[i].depletionRate &&
                accounts[j].balanceDrop > accounts[i].balanceDrop) ||
               (accounts[j].depletionRate == accounts[i].depletionRate &&
                accounts[j].balanceDrop == accounts[i].balanceDrop &&
                strcmp(accounts[j].accountNumber, accounts[i].accountNumber) < 0)){
                temp = accounts[i];
                accounts[i] = accounts[j];
                accounts[j] = temp;
            }
        }
    }
}

/**
 * @brief Exports Report 4 (Account Balance Depletion Watchlist) to a CSV file and displays the top rows.
 *
 * @param[in] accounts        Array of sorted DepletionAccount structs.
 * @param[in] qualifiedCount  Total number of watchlist accounts.
 *
 * @return 1 if exported successfully, 0 if creating or saving the CSV fails.
 */

int exportDepletionReport(DepletionAccount accounts[], int qualifiedCount){
    FILE *file;
    int i = 0;

    file = fopen("report4_balance_depletion_watchlist.csv", "w");

    if(file == NULL){
        printf("Cannot create Report 4 CSV.\n");
        return 0;
    }

    fprintf(file, "AccountName,Balance30DaysAgo,CurrentBalance,BalanceDrop,DepletionRate (%%),RiskLevel\n");

    printf("\nReport 4: Account Balance Depletion Watchlist\n");
    printf("%-24s %-18s %-16s %-14s %-18s %-10s\n",
           "AccountName", "Balance30DaysAgo", "CurrentBalance",
           "BalanceDrop", "DepletionRate (%)", "RiskLevel");

    for(i = 0; i < qualifiedCount; i++){
        fprintf(file, "%s,%.2f,%.2f,%.2f,%.2f,%s\n",
                accounts[i].accountName,
                accounts[i].balance30DaysAgo,
                accounts[i].currentBalance,
                accounts[i].balanceDrop,
                accounts[i].depletionRate,
                accounts[i].riskLevel);

        if(i < 2){
            printf("%-24s %-18.2f %-16.2f %-14.2f %-18.2f %-10s\n",
                   accounts[i].accountName,
                   accounts[i].balance30DaysAgo,
                   accounts[i].currentBalance,
                   accounts[i].balanceDrop,
                   accounts[i].depletionRate,
                   accounts[i].riskLevel);
        }
    }

    if(fclose(file) != 0){
        printf("Error saving Report 4 CSV.\n");
        return 0;
    }

    printf("(Full table exported to report4_balance_depletion_watchlist.csv)\n");
    printf("Watchlist accounts: %d\n", qualifiedCount);
    return 1;
}

/**
 * @brief Generates Report 4 (Account Balance Depletion Watchlist) from the loaded transaction records.
 *
 * @param[in] records  Array of Transaction structs containing loaded transaction records.
 * @param[in] count    Total number of transactions in the records array.
 *
 * @return 1 if Report 4 is generated and exported successfully, 0 otherwise.
 */

int generateDepletionReport(Transaction records[], int count){
    static int accountIndexes[MAX_TRANSACTIONS];
    static DepletionAccount accounts[MAX_TRANSACTIONS];
    int accountCount = 0;
    int qualifiedCount = 0;
    int i = 0, j = 0;
    int latestIndex = 0;
    int referenceDay = 0;
    int cutoffDay = 0;
    int recordDay = 0;
    int hasPriorBalance = 0;
    double balance30DaysAgo = 0.0;
    double balanceDrop = 0.0;
    double depletionRate = 0.0;
    long long latestDate = -1;
    long long transactionDate = 0;
    long long priorDate = -1;
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
    cutoffDay = referenceDay - 30;

    accountCount = getLatestAccountIndexes(records, count, accountIndexes);

    for(i = 0; i < accountCount; i++){
        account = records + accountIndexes[i];
        hasPriorBalance = 0;
        priorDate = -1;
        balance30DaysAgo = 0.0;

        for(j = 0; j < count; j++){
            if(strcmp(records[j].accountNumber, account->accountNumber) == 0){
                recordDay = dateToDayCount(records[j].date);

                if(recordDay <= cutoffDay){
                    transactionDate = getTransactionDateValue(records + j);

                    if(!hasPriorBalance || transactionDate >= priorDate){
                        hasPriorBalance = 1;
                        priorDate = transactionDate;
                        balance30DaysAgo = records[j].currentBalance;
                    }
                }
            }
        }

        if(hasPriorBalance && balance30DaysAgo > 0.0){
            balanceDrop = balance30DaysAgo - account->currentBalance;
            depletionRate = (balanceDrop / balance30DaysAgo) * 100.0;

            if(depletionRate >= 10.0){
                strcpy(accounts[qualifiedCount].accountName, account->accountName);
                strcpy(accounts[qualifiedCount].accountNumber, account->accountNumber);
                accounts[qualifiedCount].balance30DaysAgo = balance30DaysAgo;
                accounts[qualifiedCount].currentBalance = account->currentBalance;
                accounts[qualifiedCount].balanceDrop = balanceDrop;
                accounts[qualifiedCount].depletionRate = depletionRate;

                if(depletionRate >= 40.0){
                    strcpy(accounts[qualifiedCount].riskLevel, "Critical");
                } else if(depletionRate >= 25.0){
                    strcpy(accounts[qualifiedCount].riskLevel, "High");
                } else {
                    strcpy(accounts[qualifiedCount].riskLevel, "Moderate");
                }

                qualifiedCount++;
            }
        }
    }

    sortDepletionAccounts(accounts, qualifiedCount);

    return exportDepletionReport(accounts, qualifiedCount);
}

/**
 * @brief Calculates the summary totals from the loaded transaction records
 *        and exports them to summary.json.
 *
 * @param[in] records  Array of Transaction structs containing loaded transaction records.
 * @param[in] count    Total number of transactions in the records array.
 *
 * @return 1 if summary.json is created and saved successfully, 0 otherwise.
 */

int generateSummaryJson(Transaction records[], int count){
    static int accountIndexes[MAX_TRANSACTIONS];
    FILE *file;
    Transaction *account;
    int accountCount = 0;
    int i = 0;
    double totalDeposits = 0.0;
    double totalWithdrawals = 0.0;
    double totalBalance = 0.0;
    double totalInterestExpense = 0.0;

    if(count <= 0 || count > MAX_TRANSACTIONS){
        printf("No valid transactions available.\n");
        return 0;
    }

    for(i = 0; i < count; i++){
        if(strcmp(records[i].type, "deposit") == 0){
            totalDeposits += records[i].amount;
        } else if(strcmp(records[i].type, "withdrawal") == 0){
            totalWithdrawals += records[i].amount;
        }
    }

    accountCount = getLatestAccountIndexes(records, count, accountIndexes);

    for(i = 0; i < accountCount; i++){
        account = records + accountIndexes[i];
        totalBalance += account->currentBalance;

        if(account->currentBalance >= 10000.00){
            totalInterestExpense += calculateExpectedInterest(account->currentBalance);
        }
    }

    file = fopen("summary.json", "w");

    if(file == NULL){
        printf("Cannot create summary.json.\n");
        return 0;
    }

    fprintf(file,
            "{\"total_accounts\": %d, \"total_deposits\": %.2f, \"total_withdrawals\": %.2f, \"total_balance\": %.2f, \"total_interest_expense\": %.2f}\n",
            accountCount,
            totalDeposits,
            totalWithdrawals,
            totalBalance,
            totalInterestExpense);

    if(fclose(file) != 0){
        printf("Error saving summary.json.\n");
        return 0;
    }

    printf("\nSummary State (summary.json):\n");
    printf("{\"total_accounts\": %d, \"total_deposits\": %.2f, \"total_withdrawals\": %.2f, \"total_balance\": %.2f, \"total_interest_expense\": %.2f}\n",
           accountCount,
           totalDeposits,
           totalWithdrawals,
           totalBalance,
           totalInterestExpense);

    return 1;
}


/**
 * @brief Dispatches the selected menu transaction to its
 *        corresponding function.
 *
 * @param[in]     choice    Selected transaction menu option.
 * @param[in,out] rates     Exchange rates, modified when recording rates.
 * @param[in,out] records   Array of Transaction structs, modified by transactions and loading.
 * @param[in,out] count     Pointer to the total number of loaded transactions.
 * @param[in]     maxCount  Maximum number of transactions that can be stored in records.
 * @param[in]     filename  String containing the CSV filename used for loading and saving.
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
 * @return 0 when exiting normally, 1 if the transactions fail to load.
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
