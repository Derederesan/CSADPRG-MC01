# /********************
# Last names: Ilagan, Serios, Yardan
# Language: R
# Paradigm(s): Procedural
# ********************/

#' Clean Console Input
#'
#' Reads an entire line.
#'
#' @param promptText  A specified string value that is printed to help guide the user.
#'
#' @returns A given string value from stdin.
#'

getConsoleInput <- function(promptText) {
  input <- ""
  cat(promptText)
  input <- readLines("stdin", n = 1) 
  if (length(input) == 0) return("") 
  return(input)
}

#' Navigation Menu
#'
#' Prints out a navigation menu of the program as a guide for the user. 
#'
printNavMenu <- function() {
  cat("Select Transaction:\n")
  cat("[1] Register Account Name\n")
  cat("[2] Deposit Amount\n")
  cat("[3] Withdraw Amount\n")
  cat("[4] Currency Exchange\n")
  cat("[5] Record Exchange Rates\n")
  cat("[6] Show Interest Amount\n")
  cat("[0] Exit Transaction\n\n")
}

#' Exchange Menu
#'
#' Prints out the currency exchange list as a guide for the user.
#'
printExchangeMenu <- function() {
  cat("[1] Philippine Peso (PHP)\n")
  cat("[2] United States Dollar (USD)\n")
  cat("[3] Japanese Yen (JPY)\n")
  cat("[4] British Pound Sterling (GBP)\n")
  cat("[5] Euro (EUR)\n")
  cat("[6] Chinese Yuan Renminni (CNY)\n")
  cat("[0] Exit Exchange\n\n")
}

#' Convert Currency 
#'
#' Converts a source currency amount into a target currency amount by multiplying it by a factor of
#' the rates of the source divided by the rates of the target. No validation, divison by zero may occur.
#'
#' @param amount		 A double value that can represent any amount of currency recorded in data.
#' @param source         Either an integer, or name that is passed as a source index to 
#'                        specify the exact global currency that corresponds to param amount.
#' @param target         Either an integer, or name that is passed as a target index to 
#'                        specify the exact global currency in the vector, used for the conversion.
#' @param globalCurrency A vector that contains values representing each global currency.
#'
#' @returns The target currency amount.
#'

convertCurrency <- function(amount, source, target, globalCurrency) {
	return (amount * globalCurrency[source] / globalCurrency[target])
}

#' Validate a Choice I/O
#'
#' Validates a user's input value, and only allows them to escape the function if their input is within the
#' defined bounds of the function. Otherwise, it re-prompts the user for a valid input. 
#'
#' @param lb An integer value representing an inclusive lower 
#'         bound that getConsoleInput checks for error detection. 
#' @param ub An integer value representing an inclusive upper 
#'         bound that getConsoleInput checks for error detection.
#'
#' @returns The integer value that corresponds to the user's valid input.
#'

getValidChoice <- function(lb, ub) {
  validInput <- FALSE
  valSelect <- -1
  
  while (!validInput) {
    input <- getConsoleInput("Choice: ")
    valSelect <- suppressWarnings(as.integer(input))
    
    if (!is.na(valSelect) && valSelect >= lb && valSelect <= ub) {
      validInput <- TRUE
      cat("\n***\n")
      cat(sprintf("Choice = %d\n", valSelect))
    } else {
      cat("Incorrect input. Please try again.\n")
    }
  }
  return(valSelect)
}

#' Validate a Choice I/O Without Prints
#'
#' Validates a user's input value, and only allows them to escape the function if their input is within the
#' defined bounds of the function. Otherwise, it re-prompts the user for a valid input. Does not print any
#' prompt text to guide the user. This function is called to use readLines present in getConsoleInput
#' for specific text formatting.
#'
#' @param lb 		  An integer value representing an inclusive lower bound 
#'                   that getConsoleInput checks for error detection.
#' @param ub		  An integer value representing an inclusive upper bound 
#'                   that getConsoleInput checks for error detection.
#' @param promptText  An empty string that is passed into getConsoleInput
#'                   for text formatting purposes.
#'
#' @returns The integer value that corresponds to the user's input.
#'

getValidChoiceClean <- function(lb, ub, promptText = "") {
  validInput <- FALSE
  valSelect <- -1
  
  while (!validInput) {
    input <- getConsoleInput(promptText)
    valSelect <- suppressWarnings(as.integer(input))

    if (!is.na(valSelect) && valSelect >= lb && valSelect <= ub) {
      validInput <- TRUE
    } else {
      cat("Incorrect input. Please try again.\n")
    }
  }
  return(valSelect)
}

#' Valid Choice I/O For Numeric/Double Value
#'
#' Instead of printing lines that help guide the user, only validates specific numeric input. 
#'
#' @param promptText  A specified string value that is printed to help guide the user.
#'
#' @returns The numeric/double value that corresponds to the user's input.
#'

getValidAmount <- function(promptText) {
  validInput <- FALSE
  valAmount <- -1.0
  input = ""
  
  while (!validInput) {
    input <- getConsoleInput(promptText)
    valAmount <- suppressWarnings(as.numeric(input))
    
    if (!is.na(valAmount)) {
      validInput <- TRUE
    } else {
      cat("Incorrect input. Please try again.\n")
    }
  }
  return(valAmount)
}

#' Valid Choice I/O For Name
#'
#' Takes a string, and uses nchar as a way to count the number of 
#' characters present in the stdin, ensures it is within maxLen.
#'
#' @param promptText  A specified string value that is printed to help guide the user.
#' @param maxLen      A defined integer value that determines the maximum size of the string. 
#'
#' @returns A validated string of characters.
#'

getValidName <- function(promptText, maxLen) {
  validFlag <- FALSE
  nameBuffer <- ""
  
  while (!validFlag) {
    nameBuffer <- getConsoleInput(promptText)
    if (nchar(nameBuffer) > 0 && nchar(nameBuffer) <= maxLen) {
      validFlag <- TRUE
    }
  }
  return(nameBuffer)
}

#' Validate a Yes/No Response
#'
#' Prompts the user for a Y/N character input and
#' repeatedly requests input until a valid response
#' is provided.
#'
#' @param promptText A character string displayed
#'   to the user before requesting input.
#'
#' @returns TRUE for Y/y and FALSE for N/n.
#'

getValidYN <- function(promptText){
	validYN <- c("Y", "N")
	decision <- toupper(getConsoleInput(promptText))
	while(!(decision %in% validYN)){
		cat("Invalid input. Please enter Y or N.\n")
		decision <- toupper(getConsoleInput(promptText))
	}	
	return(decision == "Y")
}

#' Register Account [REQ-0009]
#'
#' Prompts the user to input an account name 
#'  and then prints their resulting output 
#'
#' @param maxLen      A defined integer value that determines the maximum size of the string. 
#'
#' @returns The validated account name string value associated with the user's input.
#'

registerAccountName <- function(maxLen) {
  accountName = ""
	
  cat("Register Account Name\n")
  accountName <- getValidName("Account Name: ", maxLen)
  cat("\n***\n")
  cat(sprintf("Account Name = %s\n", accountName))
  
  return(accountName)
}

#' Deposit Amount [REQ-0004] [REQ-0008]
#'
#' Prompts the user to input a double value that 
#'   will be added to their current held balance. 
#'
#' @param accountName    The string value that represents the current user's account name from main.
#' @param balance		 A numeric value that represents the user's currently held balance from main.
#' @param globalCurrency A vector that contains values representing each global currency.
#'
#' @returns The new balance value after being evaluated by the function.
#'

depositAmount <- function(accountName, balance, globalCurrency) {
  valDeposit <- 0.0
  currencyChoice <- 1

  cat("Deposit Amount\n")
  cat(sprintf("Account Name: %s\n", accountName))
  cat(sprintf("Current Balance: %.2f\n\n", balance))
  currencyChoice <- toupper(getConsoleInput("Currency (PHP/USD/JPY/GBP/EUR/CNY): "))
  
  while (!(currencyChoice %in% names(globalCurrency))) {
	cat("Currency input does not exist in database.\n")
	currencyChoice <- toupper(getConsoleInput("Currency (PHP/USD/JPY/GBP/EUR/CNY): "))
  }
  
  valDeposit <- getValidAmount("Deposit Amount: ")
  while(valDeposit <= 0 || valDeposit > .Machine$double.xmax){
	cat("Please enter a valid amount.\n")
	valDeposit <- getValidAmount("Deposit Amount: ")
  }
  balance <- balance + convertCurrency(valDeposit, currencyChoice, "PHP", globalCurrency)
  
  cat("\n***\n")
  cat(sprintf("Account Name = %s\n", accountName))
  cat(sprintf("Deposit Amount = %.2f %s\n", valDeposit, currencyChoice))
  cat(sprintf("Updated Balance = %.2f\n", balance))
  
  return(balance) 
}
#' Withdraw Amount [REQ-0005] [REQ-0008]
#'
#' Prompts the user to input a numeric/double value that will be subtracted from their current held balance. 
#'
#' @param accountName    The string value that represents the current user's account name from main.
#' @param balance		 A numeric value that represents the user's currently held balance from main.
#' @param globalCurrency A vector that contains values representing each global currency.
#'
#' @returns The new balance value after being evaluated by the function.
#'

withdrawAmount <- function(accountName, balance, globalCurrency) {
  valWithdraw <- 0.0
  currencyChoice <- 1
  totalWithdraw <- 0
  
  cat("Withdraw Amount\n")
  cat(sprintf("Account Name: %s\n", accountName))
  cat(sprintf("Current Balance: %.2f\n", balance))
  currencyChoice <- toupper(getConsoleInput("Currency (PHP/USD/JPY/GBP/EUR/CNY): "))
 
  while (!(currencyChoice %in% names(globalCurrency))) {
	cat("Currency input does not exist in database.\n")
	currencyChoice <- toupper(getConsoleInput("Currency (PHP/USD/JPY/GBP/EUR/CNY): "))
  } 
  
  valWithdraw <- getValidAmount("Withdraw Amount: ")
  totalWithdraw <- convertCurrency(valWithdraw, currencyChoice, "PHP", globalCurrency)
  
  while(valWithdraw <= 0 || totalWithdraw > balance || valWithdraw > .Machine$double.xmax){
	cat("Please enter a valid amount.\n")
	valWithdraw <- getValidAmount("Withdraw Amount: ")
    totalWithdraw <- convertCurrency(valWithdraw, currencyChoice, "PHP", globalCurrency)	
  }  
  balance <- balance - totalWithdraw
  
  cat("\n***\n")
  cat(sprintf("Account Name = %s\n", accountName))
  cat(sprintf("Withdraw Amount = %.2f %s\n", valWithdraw, currencyChoice))
  cat(sprintf("Updated Balance = %.2f\n", balance)) 
  
  return(balance)
}

#' Currency Exchange [REQ-0003]
#'
#' Prompts the user to input a source amount, and then calculates the post-conversion value.
#'
#' @param globalCurrency A vector that contains values representing each global currency.
#'
currencyExchange <- function(globalCurrency) {
  source <- -1.0
  result <- source
  sourceChoice <- -1
  targetChoice <- sourceChoice
  runningFlag <- 1
  while(runningFlag){
	  cat("Foreign Currency Exchange\n")
	  
	  cat("Source Currency Option:\n")
	  printExchangeMenu()
	  sourceChoice <- getValidChoiceClean(1, 6, "Source Currency: ")
	  source <- getValidAmount("Source Amount: ")
	  while(source <= 0 || source > .Machine$double.xmax){
		cat("Please enter a valid amount.\n")
		source <- getValidAmount("Source Amount: ")
	  }
	  
	  cat("Exchanged Currency Options:\n")
	  printExchangeMenu()
	  targetChoice <- getValidChoiceClean(1, 6, "Exchange Currency: ")  
	  
	  result <- convertCurrency(source, sourceChoice, targetChoice, globalCurrency)
	  cat(sprintf("Exchange Amount = %.2f\n\n", result)) 
	  runningFlag <- getValidYN("Convert another currency (Y/N)? . . . ")
  }
}

#' Record Exchange Rate [REQ-0001] [REQ-0002]
#'
#' Prompts the user to select a currency, and then prompts an exchange rate numeric value. 
#'
#' @param globalCurrency A vector that contains values representing each global currency.
#'
#' @returns The modified vector, following copy-on-modify behavior 
#'

recordExchangeRates <- function(globalCurrency) {
  currency <- -1
  choice <- -1
  
  cat("Record Exchange Rate\n\n")
  printExchangeMenu()
  choice <- getValidChoiceClean(0, 6, "Select Foreign Currency: ")
  
  if (choice == 1){
	cat("PHP is currently set as the base currency. Please select another currency.\n")
  } 
  
  else if (choice != 0) {
    currency <- getValidAmount("Exchange Rate: ")
	
	while (currency <= 0 || currency > .Machine$double.xmax){
		cat("Please enter a valid amount.\n")
		currency <- getValidAmount("Exchange Rate: ")	
	}
	
	globalCurrency[choice] <- currency
    cat("\n***\n")
    cat(sprintf("Select Foreign Currency = [%d]\n", choice))
    cat(sprintf("Exchange Rate = %.2f\n", currency))
  }
  return(globalCurrency)
}
 
#' Show Interest Amount [REQ-0006] [REQ-0007]
#'
#' Prints the user's account details, ie. name, balance, base currency, interest rate.
#' Afterwards, prompts the user for the total number of days, using it to evaluate daily interest
#' behavior based on the amount of days the user has selected. It then prints the details in a
#' formatted string, displaying the details cleanly.
#' 
#' @param accountName    The string value that represents the current user's account name from main.
#' @param balance		 A numeric value that represents the user's currently held balance from main.
#'
 
showInterestAmount <- function(accountName, balance) {
  annualRate <- 0.05
  dailyInterest <- -1
  eodBalance <- balance
  numDays <- 0
  
  cat("Show Interest Amount\n")
  cat(sprintf("Account Name: %s\n", accountName))
  cat(sprintf("Current Balance: %.2f\n", balance))
  cat("Currency: PHP\n")
  cat("Interest Rate: 5%\n\n")
  
  numDays <- getValidChoiceClean(1, 3650, "Total Number of Days: ")
  cat(sprintf("%-5s | %-10s | %-10s\n", "Day", "Interest", "Balance"))
  
  for(i in seq_len(numDays)){
	dailyInterest <- round(eodBalance * (annualRate/365), 2)
	eodBalance <- eodBalance + dailyInterest
	cat(sprintf("%-5d | %-10.2f | %-10.2f\n", i, dailyInterest, eodBalance))
  }
}

#' Main Program Function
#'
#' Handles the main running of the banking exchange program. 
#'	

main <- function() {
  accountName <- ""
  runningFlag <- TRUE
  choice <- -1
  balance <- 1000.00
  MAX_NAME_LEN <- 100
  globalCurrency <- c(
		PHP = 1.00,
		USD = 62.00,
		JPY = 0.40,
		GBP = 84.00,
		EUR = 72.00,
		CNY = 9.00  
  )
  
  while (runningFlag) {
    printNavMenu()
    choice <- getValidChoice(0, 6)
    
    if (choice != 0) {
      if (choice == 1) {
        accountName <- registerAccountName(MAX_NAME_LEN)
		runningFlag <- getValidYN("Back to Main Menu (Y/N): ")		
      } else if (choice == 2) {
        balance <- depositAmount(accountName, balance, globalCurrency)
		runningFlag <- getValidYN("Back to Main Menu (Y/N): ")
      } else if (choice == 3) {
        balance <- withdrawAmount(accountName, balance, globalCurrency)
		runningFlag <- getValidYN("Back to Main Menu (Y/N): ")
      } else if (choice == 4) {
        currencyExchange(globalCurrency)
      } else if (choice == 5) {
		globalCurrency <- recordExchangeRates(globalCurrency)
		runningFlag <- getValidYN("Back to Main Menu (Y/N): ")
      } else if (choice == 6) {
        showInterestAmount(accountName, balance)
		runningFlag <- getValidYN("Back to Main Menu (Y/N): ")
      }
    } else {
      runningFlag <- FALSE
    }
  }
  cat("Exiting..\n")
}

main()
