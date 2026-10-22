#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_NAME_LEN 63
#define MAX_ACCOUNT_NUM 100

// 7, because index 0 is a placeholder
#define MAX_CURRENCIES 7

typedef struct {
    char code[4];
    double rate;
} Currency;

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

/**
 * @brief Prints a purely visual guide on the menu navigation.   
 * 
 * @warning Includes a double newline at the end of its printing.
 */


void printNavMenu(){
	printf("Select Transaction:\n");
	printf("[1] Register Account Name\n");
	printf("[2] Deposit Amount\n");
	printf("[3] Withdraw Amount\n");
	printf("[4] Currency Exchange\n");
	printf("[5] Record Exchange Rates\n");
	printf("[6] Show Interest Amount\n");
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

void getValidName(char *nameBuffer, int maxLen){
    int validFlag = 0;
    int nameLength = -1;
    while(!validFlag){
        if(scanf(" %63[^\n]", nameBuffer) == 1){
            nameLength = strlen(nameBuffer);
            if(nameLength > 0 && nameLength < maxLen){
                validFlag = 1; 
				cleanInputBuffer();
            }
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

void registerAccountName(char *accountName){
	printf("Register Account Name\n");
	printf("Account Name: ");
	getValidName(accountName, MAX_NAME_LEN + 1);
	printf("\n***\n");
	printf("Account Name = %s\n", accountName);
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

void depositAmount(char *accountName, double *balance, Currency rates[MAX_CURRENCIES]){
	double valDeposit = -1;
	int currencyChoice = -1;
	
	printf("Deposit Amount\n");
	printf("Account Name: %s\n", accountName);
	printf("Current Balance: %.2lf\n", *balance);
	currencyChoice = getValidCurrency(rates);
	
	printf("Deposit Amount: ");
	valDeposit = getValidAmount();
	while(valDeposit <= 0 || !isfinite(valDeposit)){
		printf("Please enter a valid amount.\n");
		valDeposit = getValidAmount();
	}
	*balance += convertCurrency(valDeposit, currencyChoice, 1, rates);
	printf("\n***\n");
	printf("Account Name = %s\n", accountName);	
	printf("Deposit Amount = %.2lf %s\n", valDeposit, rates[currencyChoice].code);
	printf("Updated Balance = %.2lf\n", *balance);
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
 
void withdrawAmount(char *accountName, double *balance, Currency rates[MAX_CURRENCIES]){
	double valWithdraw = -1;
	int currencyChoice = -1;
	double totalWithdraw = 0.0;
	
	printf("Withdraw Amount\n");
	printf("Account Name: %s\n", accountName);
	printf("Current Balance: %.2lf\n", *balance);
	currencyChoice = getValidCurrency(rates);
	
	printf("\nWithdraw Amount: ");
	valWithdraw = getValidAmount();
	totalWithdraw = convertCurrency(valWithdraw, currencyChoice, 1, rates);
	
	while(valWithdraw <= 0 || !isfinite(valWithdraw) || !isfinite(totalWithdraw) || totalWithdraw > *balance){
		printf("Please enter a valid amount.\n");
		valWithdraw = getValidAmount();
		totalWithdraw = convertCurrency(valWithdraw, currencyChoice, 1, rates);
	}
	*balance -= totalWithdraw;
	printf("\n***\n");
	printf("Account Name = %s\n", accountName);	
	printf("Withdraw Amount = %.2lf %s\n", valWithdraw, rates[currencyChoice].code);
	printf("Updated Balance = %.2lf\n", *balance);	
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
		while(source <= 0 || !isfinite(source)){
			printf("Please enter a valid amount.\n");
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
		while(currency <= 0 || !isfinite(currency)){
			printf("Please enter a valid exchange rate.\n");
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

void showInterestAmount(char* accountName, double balance){
	int numDays = 0;
	int i = 0;
	double annualRate = 0.05;
	double dailyInterest = -1; 
	double eodBalance = balance;
	
	printf("Show Interest Amount\n");
	printf("Account Name: %s\n", accountName);
	printf("Current Balance: %.2lf\n", balance);
	printf("Currency: PHP\n");
	printf("Interest Rate: 5%%\n\n");
	
	printf("Total number of days: ");
	numDays = getValidChoiceClean(1, 3650);
	printf("\n%-5s | %-10s | %-10s\n", "Day", "Interest", "Balance");
	for(i = 1; i <= numDays; i++){
		dailyInterest = round(eodBalance*(annualRate/365) * 100.0) / 100.0;
		eodBalance += dailyInterest;
		printf("%-5d | %-10.2lf | %-10.2lf\n", i, dailyInterest, eodBalance);
	}
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
 
void transactionSelect(int choice, char *accountName, double *balance, Currency rates[MAX_CURRENCIES]){
	switch(choice){
		case 1:
			registerAccountName(accountName);
			break;
		case 2:
			depositAmount(accountName, balance, rates);
			break;
		case 3:
			withdrawAmount(accountName, balance, rates);
			break;
		case 4:
			currencyExchange(rates);
			break;
		case 5:
			recordExchangeRates(rates);
			break;
		case 6:
			showInterestAmount(accountName, *balance);
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
	
	char accountName[MAX_NAME_LEN + 1] = {0};
	int runningFlag = 1;
	int choice = -1;
	double balance = 1000.00;
	
	while(runningFlag){
		printNavMenu();
		choice = getValidChoice(0, 6);
		if (choice){
			transactionSelect(choice, accountName, &balance, globalCurrency);
			if(choice != 4){
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

