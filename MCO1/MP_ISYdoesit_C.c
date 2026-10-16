#include <stdio.h>
#include <string.h>
#define MAX_NAME_LEN 63
#define MAX_ACCOUNT_NUM 100

// 7, because index 0 is a placeholder
#define MAX_CURRENCIES 7

/********************
Last names: Ilagan, Serios, Yardan
Language: C
Paradigm(s): Procedural
********************/

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
 * @brief Prints a purely visual guide on the menu navigation.     [REQ-0001]
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
 * @brief Converts a source currency amount into a target currency amount by multiplying it by a factor of
 * 		  the rates of the source divided by the rates of the target. 
 *
 * @return  The target currency amount.
 * @warning The function parameter double values must not be 0.
 */
double convertCurrency(double amount, int source, int target, double rates[MAX_CURRENCIES]){
	return amount * rates[source] / rates[target];
}

/**
 * @brief 	Prints lines to guide UI, then takes an integer input value, accepting only values in a strict range. [REQ-0002], [REQ-0003]
 * 
 * @pre 	Standard input stream must be available.
 * @warning Currently only supports integer-based console inputs through in_stream.
 * 			Prints an error message in case of invalid input, cleans the input buffer afterwards.
 *			Highly coupled with the main function, has several prints that guide the UI.
 *
 * @param[in]     lb  Inclusive lower bound that scanf checks for error detection.
 * @param[in]     ub  Inclusive lower bound that scanf checks for error detection.
 * 
 * @return The exact valid integer value input from in_stream.
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
 * @warning Currently only supports integer-based console inputs through in_stream.
 * 			Prints an error message in case of invalid input, cleans the input buffer afterwards.
 *
 * @param[in]     lb  Inclusive lower bound that scanf checks for error detection.
 * @param[in]     ub  Inclusive lower bound that scanf checks for error detection.
 * 
 * @return The exact valid integer value input from in_stream.
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
 * @warning Currently only supports double-based console inputs through in_stream.
 * 			Prints an error message in case of invalid input, cleans the input buffer afterwards.
 * 
 * @return The exact valid double value input from in_stream.
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
                printf("\nIncorrect input. Please enter Y or N: ");
            }
        } else {
            cleanInputBuffer();
            printf("\nIncorrect input. Please enter Y or N: ");
        }
    }
    return result;
}

/**
 * @brief 	Prints the prompt for the user to register account name details. [REQ-0004], [REQ-0005], [REQ-0006]
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
 * @brief 	Prints the prompt for the user to deposit balance from their account. [REQ-0008], [REQ-0009] [REQ-0010]
 *
 * @param[in]     accountName  Represents the user's account.
 * @param[out]    balance      The value of the user's balance that is to be modified.
 *
 * @post The user's balance value is added by their withdraw amount. 
 */

void depositAmount(char *accountName, double *balance){
	double valDeposit = -1;
	printf("Deposit Amount\n");
	printf("Account Name: %s\n", accountName);
	printf("Current Balance: %.2lf\n", *balance);
	printf("Currency: PHP\n");
	printf("\nDeposit Amount: ");
	valDeposit = getValidAmount();
	while(valDeposit < 0){
		valDeposit = getValidAmount();
	}
	*balance += valDeposit;
	printf("\n***\n");
	printf("Account Name = %s\n", accountName);	
	printf("Deposit Amount = %.2lf\n", valDeposit);
}

/**
 * @brief 	Prints the prompt for the user to withdraw balance from their account. 
 * 			Does not allow for withdrawal amounts larger than the account balance. [REQ-0012], [REQ-0013], [REQ-0014]
 *
 * @param[in]     accountName  Represents the user's account.
 * @param[out]    balance      The value of the user's balance that is to be modified.
 *
 * @post The user's balance value is subtracted by their deposited amount.
 */

void withdrawAmount(char *accountName, double *balance){
	double valWithdraw = -1;
	printf("Withdraw Amount\n");
	printf("Account Name: %s\n", accountName);
	printf("Current Balance: %.2lf\n", *balance);
	printf("Currency: PHP\n");
	printf("\nWithdraw Amount: ");
	valWithdraw = getValidAmount();
	while(valWithdraw < 0 || valWithdraw > *balance){
		valWithdraw = getValidAmount();
	}
	*balance -= valWithdraw;
	printf("\n***\n");
	printf("Account Name = %s\n", accountName);	
	printf("Withdraw Amount = %.2lf\n", valWithdraw);
}

/**
 * @brief 	Prompts a user for a source amount, then exchanges it to other currency equivalencies. [REQ-0019], [REQ-0020]
 *
 * @warning  The exchange values are hardcoded in function printExchangedCurrencyMenu.
 *
 * @post  Prints the exchange equivalency according to the hardcoded exchange values.
 */

void currencyExchange(double rates[MAX_CURRENCIES]){
	double source = 0.0;
	double result = 0.0;
	int sourceChoice = -1;
	int targetChoice = -1;
	
	printf("Foreign Currency Exchange\n");
	printf("Source Currency Option: ");
	printExchangeMenu();
	sourceChoice = getValidChoiceClean(1,6);
	
	printf("Source Amount: ");	
	source = getValidAmount();
	
	printf("Exchanged Currency Options:\n");
	printExchangeMenu();
	
	printf("Exchange Currency: ");
	targetChoice = getValidChoiceClean(1,6);
	
	result = convertCurrency(source, sourceChoice, targetChoice, rates);
	printf("\n***\n");
    printf("Exchange Currency = %d\n", targetChoice);
    printf("Exchange Amount = %.2lf\n\n", result);	
}

/**
 * @brief Records the user's input for a global currency's exchange value. [REQ-0016], [REQ-0017]
 * 
 */

void recordExchangeRates(double rates[MAX_CURRENCIES]){
    int choice = -1;
    double currency = 0.0;
    printf("Record Exchange Rate\n\n");
    printExchangeMenu();
    printf("Select Foreign Currency: ");
    
    choice = getValidChoiceClean(2,6);
    if(choice){
        printf("Exchange Rate: ");
        currency = getValidAmount();
        rates[choice] = currency;
        printf("\n***\n");
        printf("Select Foreign Currency = [%d]\n", choice);
        printf("Exchange Rate = %.2lf\n", currency);
    }
}

/**
 * @brief Computes and displays the interest amounts and balance for a given number of days. [REQ-0006], [REQ-0007]
 * 
 * @param[in]     accountName  Represents the user's account.
 * @param[out]    balance      The value of the user's balance. 
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
	
	printf("Total number of days:\n");
	numDays = getValidChoiceClean(1, 3650)
	printf("%-5s | %-10s | %-10s\n", "Day", "Interest", "Balance");
	for(i = 0; i <= numDays; i++){
		dailyInterest = eodBalance*(annualRate/365)
		eodBalance += dailyInterest;
		printf("%-5d | %-10.2lf | %-10.2lf\n", day, dailyInterest, projectedBalance);
	}
}

/**
 * @brief Traversal function to guide the user to different transaction selections. [REQ-0004], [REQ-0007], [REQ-0011], [REQ-0015], [REQ-0018]
 * 
 */
 
void transactionSelect(int choice, char *accountName, double *balance, double rates[MAX_CURRENCIES]){
	switch(choice){
		case 1:
			registerAccountName(accountName);
			break;
		case 2:
			depositAmount(accountName, balance);
			break;
		case 3:
			withdrawAmount(accountName, balance);
			break;
		case 4:
			currencyExchange(rates);
			break;
		case 5:
			recordExchangeRates(rates);
			break;
		case 6:
			showInterestAmount();
			break;
		default:
			printf("ERROR: Invalid selection.\n");
			break;
	}
}
 
/**
 * @brief Main program loop, constantly checks for user's valid input to navigate menu. [REQ-0002], [REQ-0003]
 * 
 */ 
 
int main(){
	
	// +1 so that the rates are no longer zero indexed, which align with user input 1-6. index 0 is a placeholder
	double globalCurrency[MAX_CURRENCIES] = {
		-1.00,
		1.00,
		62.00,
		0.40,
		84.00,
		72.00,
		9.00
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
		} else {
			runningFlag = 0;
		}
	}
	printf("Exiting..\n");
	return 0;
}

