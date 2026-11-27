# /********************
# Last names: Ilagan, Serios, Yardan
# Language: R
# Paradigm(s): Procedural
# ********************/

MAX_NAME_LEN <- 63
MAX_TRANSACTIONS <- 20000
STDIN_CON <- if (!interactive()) file("stdin", open = "r") else NULL

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

  if (interactive()) {
    input <- readline(promptText)
  } else {
    cat(promptText)
    input <- readLines(STDIN_CON, n = 1, warn = FALSE)
    if (length(input) == 0) {
      input <- ""
    }
  }
  return(input)
}

#' Round Half Up
#'
#' Rounds a numeric value to two decimal places using round-half-up.
#'
#' @param value A numeric value.
#'
#' @returns The rounded numeric value.
#'
roundHalfUp <- function(value) {
  result <- sign(value) * floor(abs(value) * 100.0 + 0.5) / 100.0
  result[result == 0] <- 0.0
  return(result)
}

#' Empty Transactions
#'
#' Creates an empty transactions data frame.
#'
#' @returns An empty transactions data frame.
#'
emptyTransactions <- function() {
  records <- data.frame(
    accountName = character(0),
    accountNumber = character(0),
    date = character(0),
    time = character(0),
    currency = character(0),
    amount = numeric(0),
    type = character(0),
    currentBalance = numeric(0),
    stringsAsFactors = FALSE
  )
  return(records)
}

#' Navigation Menu
#'
#' Prints out a navigation menu of the program as a guide for the user.
#'
printNavMenu <- function() {
  cat("Select Transaction:\n")
  cat("[1] Register Account\n")
  cat("[2] Deposit Amount\n")
  cat("[3] Withdraw Amount\n")
  cat("[4] Currency Exchange\n")
  cat("[5] Record Exchange Rates\n")
  cat("[6] Show Interest Amount\n")
  cat("[7] Generate Reports\n")
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
  cat("[6] Chinese Yuan Renminni (CNY)\n\n")
}

#' Report Menu
#'
#' Prints out a report menu of the program as a guide for the user.
#'
printReportMenu <- function() {
  cat("\nGenerate Reports\n")
  cat("[1] Load the file\n")
  cat("[2] Generate Reports\n")
  cat("[0] Back\n\n")
}

#' Convert Currency
#'
#' Converts a source currency amount into a target currency amount.
#'
#' @param amount         The amount to convert.
#' @param source         The source currency index or code.
#' @param target         The target currency index or code.
#' @param globalCurrency A named vector of exchange rates against PHP.
#'
#' @returns The converted currency amount.
#'
convertCurrency <- function(amount, source, target, globalCurrency) {
  result <- as.numeric(amount * globalCurrency[source] / globalCurrency[target])
  return(result)
}

#' Parse Strict Integer
#'
#' Parses a string only if it contains a valid whole number.
#'
#' @param text The input string.
#'
#' @returns The integer value, or NA if invalid.
#'
parseStrictInteger <- function(text) {
  if (!grepl("^\\s*[+-]?[0-9]+\\s*$", text)) {
    return(NA)
  }
  return(suppressWarnings(as.integer(text)))
}

#' Parse Numeric Field
#'
#' Parses a numeric CSV field, rejecting empty values, hex strings, NaN, and Inf.
#'
#' @param text The raw field text.
#'
#' @returns The numeric value, or NA if invalid.
#'
parseNumericField <- function(text) {
  if (nchar(text) == 0 || grepl("\\s$", text) || grepl("[xX]", text)) {
    return(NA)
  }
  value <- suppressWarnings(as.numeric(text))
  if (is.na(value) || !is.finite(value)) {
    return(NA)
  }
  return(value)
}

#' Validate a Choice I/O
#'
#' Validates an integer menu choice within [lb, ub] and prints confirmation.
#'
#' @param lb Inclusive lower bound.
#' @param ub Inclusive upper bound.
#'
#' @returns The validated integer choice.
#'
getValidChoice <- function(lb, ub) {
  validInput <- FALSE
  valSelect <- -1

  while (!validInput) {
    input <- getConsoleInput("Choice: ")
    valSelect <- parseStrictInteger(input)

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
#' Validates an integer choice within [lb, ub] without extra confirmation text.
#'
#' @param lb         Inclusive lower bound.
#' @param ub         Inclusive upper bound.
#' @param promptText Prompt displayed before reading input.
#'
#' @returns The validated integer choice.
#'
getValidChoiceClean <- function(lb, ub, promptText = "") {
  validInput <- FALSE
  valSelect <- -1

  while (!validInput) {
    input <- getConsoleInput(promptText)
    valSelect <- parseStrictInteger(input)

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
#' Reads and validates a finite numeric input.
#'
#' @param promptText Prompt displayed before reading input.
#'
#' @returns The validated numeric value.
#'
getValidAmount <- function(promptText = "") {
  validInput <- FALSE
  valAmount <- -1.0

  while (!validInput) {
    input <- getConsoleInput(promptText)
    valAmount <- suppressWarnings(as.numeric(input))

    if (!is.na(valAmount) && is.finite(valAmount)) {
      validInput <- TRUE
    } else {
      cat("Incorrect input. Please try again.\n")
    }
  }
  return(valAmount)
}

#' Valid Choice I/O For Name
#'
#' Reads and validates an account name (non-empty, within maxLen, no commas).
#'
#' @param promptText Prompt displayed before reading input.
#' @param maxLen     Maximum allowed string length.
#'
#' @returns A validated name string.
#'
getValidName <- function(promptText, maxLen) {
  validFlag <- FALSE
  nameBuffer <- ""

  while (!validFlag) {
    nameBuffer <- getConsoleInput(promptText)
    nameLength <- nchar(nameBuffer)
    if (nameLength > 0 && nameLength <= maxLen && !grepl(",", nameBuffer, fixed = TRUE)) {
      validFlag <- TRUE
    } else {
      cat("Invalid name.\n")
    }
  }
  return(nameBuffer)
}

#' Validate a Yes/No Response
#'
#' Prompts until the user enters Y or N.
#'
#' @param promptText Prompt displayed before reading input.
#'
#' @returns TRUE for Y/y and FALSE for N/n.
#'
getValidYN <- function(promptText) {
  decision <- toupper(trimws(getConsoleInput(promptText)))
  while (decision != "Y" && decision != "N") {
    cat("\nInvalid input. Please enter Y or N.\n")
    decision <- toupper(trimws(getConsoleInput(promptText)))
  }
  return(decision == "Y")
}

#' Validate Currency Input
#'
#' Prompts the user for a registered currency code.
#'
#' @param globalCurrency A named vector of exchange rates.
#'
#' @returns A validated currency code.
#'
getValidCurrency <- function(globalCurrency) {
  currencyChoice <- ""
  validFlag <- FALSE

  while (!validFlag) {
    input <- toupper(trimws(getConsoleInput("Currency (PHP/USD/JPY/GBP/EUR/CNY): ")))
    if (input %in% names(globalCurrency)) {
      currencyChoice <- input
      validFlag <- TRUE
    } else {
      cat("That currency is not registered in the database.\n")
    }
  }
  return(currencyChoice)
}

#' String To Account Number
#'
#' Converts 10-digit account number strings to numeric values (0 if invalid).
#'
#' @param str A string or character vector of account numbers.
#'
#' @returns A numeric vector of account numbers.
#'
stringToAccountNumber <- function(str) {
  number <- rep(0, length(str))
  isValid <- !is.na(str) & grepl("^[0-9]{10}$", str)
  if (any(isValid)) {
    number[isValid] <- as.numeric(str[isValid])
  }
  return(number)
}

#' Is Leap Year
#'
#' Checks whether year values are leap years.
#'
#' @param year Integer or numeric year value(s).
#'
#' @returns Logical value(s) indicating leap years.
#'
isLeapYear <- function(year) {
  return((year %% 400 == 0) | (year %% 4 == 0 & year %% 100 != 0))
}

#' Get Days In Month
#'
#' Returns the number of days in a given month and year.
#'
#' @param month The month integer (1-12).
#' @param year  The year integer.
#'
#' @returns The number of days in the month.
#'
getDaysInMonth <- function(month, year) {
  daysInMonth <- c(31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31)
  result <- daysInMonth[month]
  if (month == 2 && isLeapYear(year)) {
    result <- 29
  }
  return(result)
}

#' Parse Date Parts
#'
#' Extracts day, month, and year integers from DD/MM/YYYY date strings.
#'
#' @param dates A character vector of DD/MM/YYYY strings.
#'
#' @returns A list containing day, month, and year integer vectors.
#'
parseDateParts <- function(dates) {
  day <- as.integer(substr(dates, 1, 2))
  month <- as.integer(substr(dates, 4, 5))
  year <- as.integer(substr(dates, 7, 10))
  return(list(day = day, month = month, year = year))
}

#' Parse Time Parts
#'
#' Extracts hour, minute, and second integers from H:MM:SS or HH:MM:SS strings.
#'
#' @param times A character vector of time strings.
#'
#' @returns A list containing hour, minute, and second integer vectors.
#'
parseTimeParts <- function(times) {
  # ":MM:SS" is always the last 6 characters, so hour is everything before that
  timeLength <- nchar(times)
  hour <- as.integer(substr(times, 1, timeLength - 6))
  minute <- as.integer(substr(times, timeLength - 4, timeLength - 3))
  second <- as.integer(substr(times, timeLength - 1, timeLength))
  return(list(hour = hour, minute = minute, second = second))
}

#' Date To Sortable Number
#'
#' Converts DD/MM/YYYY strings into YYYYMMDD numbers for easy date sorting.
#'
#' @param dates A character vector of DD/MM/YYYY strings.
#'
#' @returns A numeric vector of YYYYMMDD values.
#'
dateToSortableNumber <- function(dates) {
  dateParts <- parseDateParts(dates)
  return(dateParts$year * 10000 + dateParts$month * 100 + dateParts$day)
}

#' Get Transaction Date Value
#'
#' Converts transaction date and time into YYYYMMDDHHMMSS sortable numbers.
#'
#' @param records The transaction records data frame.
#'
#' @returns A numeric vector of timestamp values.
#'
getTransactionDateValue <- function(records) {
  dateValue <- numeric(0)

  if (nrow(records) > 0) {
    dateParts <- parseDateParts(records$date)
    timeParts <- parseTimeParts(records$time)
    dateValue <- (((((as.numeric(dateParts$year) * 100 + dateParts$month) * 100 + dateParts$day) * 100 + timeParts$hour) * 100 + timeParts$minute) * 100 + timeParts$second)
  }
  return(dateValue)
}

#' Date To Day Count
#'
#' Converts DD/MM/YYYY strings to total calendar day counts.
#'
#' @param dates A character vector of DD/MM/YYYY strings.
#'
#' @returns A numeric vector of day counts.
#'
dateToDayCount <- function(dates) {
  total <- numeric(0)

  if (length(dates) > 0) {
    dateParts <- parseDateParts(dates)
    previousYear <- dateParts$year - 1
    cumulativeDays <- c(0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334)
    leapYearBonus <- as.integer(dateParts$month > 2 & isLeapYear(dateParts$year))

    total <- previousYear * 365 +
      previousYear %/% 4 -
      previousYear %/% 100 +
      previousYear %/% 400 +
      cumulativeDays[dateParts$month] +
      leapYearBonus +
      dateParts$day
  }
  return(total)
}

#' Is Valid Date
#'
#' Checks whether a single date string is a valid DD/MM/YYYY calendar date.
#'
#' @param dateString A single date string.
#'
#' @returns TRUE if valid, FALSE otherwise.
#'
isValidDate <- function(dateString) {
  if (!grepl("^[0-9]{2}/[0-9]{2}/[0-9]{4}$", dateString)) {
    return(FALSE)
  }
  dateParts <- parseDateParts(dateString)
  if (dateParts$year < 1 || dateParts$month < 1 || dateParts$month > 12 || dateParts$day < 1) {
    return(FALSE)
  }
  daysInMonth <- getDaysInMonth(dateParts$month, dateParts$year)
  return(dateParts$day <= daysInMonth)
}

#' Is Valid Time
#'
#' Checks whether a single time string is a valid H:MM:SS or HH:MM:SS clock time.
#'
#' @param timeString A single time string.
#'
#' @returns TRUE if valid, FALSE otherwise.
#'
isValidTime <- function(timeString) {
  if (!grepl("^[0-9]{1,2}:[0-9]{2}:[0-9]{2}$", timeString)) {
    return(FALSE)
  }
  timeParts <- parseTimeParts(timeString)
  return(timeParts$hour >= 0 && timeParts$hour <= 23 &&
    timeParts$minute >= 0 && timeParts$minute <= 59 &&
    timeParts$second >= 0 && timeParts$second <= 59)
}

#' Parse CSV Line
#'
#' Splits and checks basic structure/lengths/numbers of one raw CSV line.
#'
#' @param line A single CSV line without trailing newline.
#'
#' @returns A list of parsed fields, or NULL if structurally invalid.
#'
parseCsvLine <- function(line) {
  lineLength <- nchar(line)
  if (lineLength == 0 || lineLength > 255 || endsWith(line, ",")) {
    return(NULL)
  }

  fields <- strsplit(line, ",", fixed = TRUE)[[1]]
  if (length(fields) != 8) {
    return(NULL)
  }

  if (nchar(fields[1]) < 1 || nchar(fields[1]) > 63) return(NULL)
  if (nchar(fields[2]) < 1 || nchar(fields[2]) > 10) return(NULL)
  if (nchar(fields[3]) < 1 || nchar(fields[3]) > 10) return(NULL)
  if (nchar(fields[4]) < 1 || nchar(fields[4]) > 8) return(NULL)
  if (nchar(fields[5]) < 1 || nchar(fields[5]) > 3) return(NULL)
  if (nchar(fields[7]) < 1 || nchar(fields[7]) > 10) return(NULL)

  amountValue <- parseNumericField(fields[6])
  balanceValue <- parseNumericField(fields[8])
  if (is.na(amountValue) || is.na(balanceValue)) {
    return(NULL)
  }

  parsedRecord <- list(
    accountName = fields[1],
    accountNumber = fields[2],
    date = fields[3],
    time = fields[4],
    currency = fields[5],
    amount = amountValue,
    type = fields[7],
    currentBalance = balanceValue
  )
  return(parsedRecord)
}

#' Validate Transaction
#'
#' Validates all business rules on a single parsed PHP transaction row.
#'
#' @param record A parsed record list from parseCsvLine.
#'
#' @returns TRUE if valid, FALSE otherwise.
#'
validateTransaction <- function(record) {
  if (!grepl("^[0-9]{10}$", record$accountNumber)) return(FALSE)
  if (nchar(record$accountName) <= 0) return(FALSE)
  if (record$amount <= 0) return(FALSE)
  if (record$currentBalance < 0) return(FALSE)
  if (record$currency != "PHP") return(FALSE)
  if (record$type != "deposit" && record$type != "withdrawal") return(FALSE)
  if (!isValidDate(record$date)) return(FALSE)
  if (!isValidTime(record$time)) return(FALSE)
  return(TRUE)
}

#' Load Transactions
#'
#' Loads and validates PHP transaction records from a CSV file line by line.
#'
#' @param filename The CSV filename.
#' @param maxCount The maximum number of accepted rows.
#'
#' @returns The loaded transaction records data frame, or NULL on error.
#'
loadTransactions <- function(filename, maxCount) {
  rawLines <- tryCatch(
    readLines(filename, warn = FALSE),
    error = function(e) NULL
  )

  if (is.null(rawLines)) {
    cat("Cannot open CSV.\n")
    return(NULL)
  }

  dataLines <- if (length(rawLines) > 1) rawLines[-1] else character(0)
  dataLines <- sub("\r$", "", dataLines)
  totalLines <- length(dataLines)

  # Pre-allocate parallel arrays (like fixed-size arrays in C)
  maxCapacity <- min(totalLines, maxCount)
  accountNames <- character(maxCapacity)
  accountNumbers <- character(maxCapacity)
  dates <- character(maxCapacity)
  times <- character(maxCapacity)
  currencies <- character(maxCapacity)
  amounts <- numeric(maxCapacity)
  types <- character(maxCapacity)
  currentBalances <- numeric(maxCapacity)

  totalRows <- 0
  invalidRows <- 0
  filteredRows <- 0
  acceptedRows <- 0

  if (totalLines > 0) {
    for (i in 1:totalLines) {
      if (acceptedRows >= maxCount) {
        break
      }

      totalRows <- totalRows + 1
      parsedRecord <- parseCsvLine(dataLines[i])

      if (is.null(parsedRecord)) {
        invalidRows <- invalidRows + 1
        cat(sprintf("Invalid transaction at row %d.\n", i + 1))
      } else if (parsedRecord$currency != "PHP") {
        filteredRows <- filteredRows + 1
      } else if (!validateTransaction(parsedRecord)) {
        invalidRows <- invalidRows + 1
        cat(sprintf("Invalid transaction at row %d.\n", i + 1))
      } else {
        acceptedRows <- acceptedRows + 1
        accountNames[acceptedRows] <- parsedRecord$accountName
        accountNumbers[acceptedRows] <- parsedRecord$accountNumber
        dates[acceptedRows] <- parsedRecord$date
        times[acceptedRows] <- parsedRecord$time
        currencies[acceptedRows] <- parsedRecord$currency
        amounts[acceptedRows] <- parsedRecord$amount
        types[acceptedRows] <- parsedRecord$type
        currentBalances[acceptedRows] <- parsedRecord$currentBalance
      }
    }
  }

  cat(sprintf("Total rows: %d\n", totalRows))
  cat(sprintf("Invalid rows: %d\n", invalidRows))
  cat(sprintf("Filtered rows: %d\n", filteredRows))
  cat(sprintf("Accepted rows: %d\n", acceptedRows))

  if (acceptedRows == 0) {
    return(emptyTransactions())
  }

  records <- data.frame(
    accountName = accountNames[1:acceptedRows],
    accountNumber = accountNumbers[1:acceptedRows],
    date = dates[1:acceptedRows],
    time = times[1:acceptedRows],
    currency = currencies[1:acceptedRows],
    amount = amounts[1:acceptedRows],
    type = types[1:acceptedRows],
    currentBalance = currentBalances[1:acceptedRows],
    stringsAsFactors = FALSE
  )
  return(records)
}

#' Get Latest Account Indexes
#'
#' Finds the row index of the latest transaction for each unique account.
#'
#' @param records The transaction records.
#'
#' @returns An integer vector of row indexes.
#'
getLatestAccountIndexes <- function(records) {
  accountIndexes <- integer(0)
  recordCount <- nrow(records)

  if (recordCount > 0) {
    dateValues <- getTransactionDateValue(records)
    # Sort newest-first; tie-break by later row index
    rowOrder <- order(dateValues, 1:recordCount, decreasing = TRUE)
    orderedAccounts <- records$accountNumber[rowOrder]
    # First appearance of each account in newest-first order is its latest row
    accountIndexes <- rowOrder[!duplicated(orderedAccounts)]
  }
  return(accountIndexes)
}

#' Find Account
#'
#' Finds the latest transaction record for a specific account number.
#'
#' @param records       The transaction records.
#' @param accountNumber The 10-digit account number string.
#'
#' @returns A 1-row data frame for the latest transaction, or NULL if not found.
#'
findAccount <- function(records, accountNumber) {
  latest <- NULL

  if (nrow(records) > 0) {
    matches <- which(records$accountNumber == accountNumber)
    if (length(matches) > 0) {
      dateValues <- getTransactionDateValue(records[matches, ])
      bestIndex <- order(dateValues, matches, decreasing = TRUE)[1]
      latest <- records[matches[bestIndex], ]
    }
  }
  return(latest)
}

#' Get Next Account Number
#'
#' Calculates the next sequential account number from existing records.
#'
#' @param records The transaction records.
#'
#' @returns The next numeric account number, or 0 on error.
#'
getNextAccountNumber <- function(records) {
  nextNum <- 0
  highest <- 0
  validFlag <- TRUE

  if (nrow(records) > 0) {
    currentNums <- stringToAccountNumber(records$accountNumber)
    if (any(currentNums == 0)) {
      cat("Invalid account number.\n")
      validFlag <- FALSE
    } else {
      highest <- max(currentNums)
    }
  }

  if (validFlag) {
    nextNum <- highest + 1
    if (nextNum > 9999999999) {
      cat("Account number limits reached.\n")
      nextNum <- 0
    }
  }
  return(nextNum)
}

#' Save Transaction
#'
#' Appends a single transaction record to the CSV file.
#'
#' @param filename The CSV filename.
#' @param record   A 1-row transaction data frame.
#'
#' @returns TRUE if saved, FALSE on error.
#'
saveTransaction <- function(filename, record) {
  line <- sprintf(
    "%s,%s,%s,%s,%s,%.2f,%s,%.2f\n",
    record$accountName,
    record$accountNumber,
    record$date,
    record$time,
    record$currency,
    record$amount,
    record$type,
    record$currentBalance
  )

  saveResult <- tryCatch({
    cat(line, file = filename, append = TRUE)
    TRUE
  }, error = function(e) {
    cat("Cannot open CSV.\n")
    FALSE
  })
  return(saveResult)
}

#' Create Account Transaction
#'
#' Creates a new account registration transaction record.
#'
#' @param accountName The account name string.
#' @param number      The new numeric account number.
#' @param deposit     The initial deposit amount.
#'
#' @returns A 1-row transaction data frame.
#'
createAccountTransaction <- function(accountName, number, deposit) {
  currentTime <- Sys.time()
  record <- data.frame(
    accountName = accountName,
    accountNumber = sprintf("%010.0f", number),
    date = format(currentTime, "%d/%m/%Y"),
    time = format(currentTime, "%H:%M:%S"),
    currency = "PHP",
    amount = deposit,
    type = "deposit",
    currentBalance = deposit,
    stringsAsFactors = FALSE
  )
  return(record)
}

#' Set Transaction Date Time
#'
#' Updates a transaction record with the current system date and time.
#'
#' @param record A 1-row transaction data frame.
#'
#' @returns The updated transaction record.
#'
setTransactionDateTime <- function(record) {
  currentTime <- Sys.time()
  record$date <- format(currentTime, "%d/%m/%Y")
  record$time <- format(currentTime, "%H:%M:%S")
  return(record)
}

#' Register Account [REQ-0009]
#'
#' Prompts for an account name and initial deposit, then saves the new account.
#'
#' @param records  The transaction records.
#' @param maxCount Maximum allowed transactions.
#' @param filename The CSV filename.
#'
#' @returns The updated transaction records.
#'
registerAccount <- function(records, maxCount, filename) {
  status <- TRUE

  if (nrow(records) >= maxCount) {
    cat("Transaction limit reached.\n")
    status <- FALSE
  }

  if (status) {
    cat("Register Account\n")
    accountName <- getValidName("Account Name: ", MAX_NAME_LEN)

    initialDeposit <- getValidAmount("Initial Deposit: ")
    while (initialDeposit < 5000.00) {
      cat("Minimum initial deposit is PHP 5000.00.\n")
      initialDeposit <- getValidAmount("Initial Deposit: ")
    }

    newNumber <- getNextAccountNumber(records)
    if (newNumber == 0) {
      status <- FALSE
    }
  }

  if (status) {
    newTransaction <- createAccountTransaction(accountName, newNumber, initialDeposit)
    if (!saveTransaction(filename, newTransaction)) {
      cat("Cannot save account.\n")
      status <- FALSE
    }
  }

  if (status) {
    records <- rbind(records, newTransaction)
    rownames(records) <- NULL

    cat("\nAccount registered successfully!\n")
    cat(sprintf("Account Name: %s\n", newTransaction$accountName))
    cat(sprintf("Account Number: %s\n", newTransaction$accountNumber))
    cat(sprintf("Current Balance: %.2f\n", newTransaction$currentBalance))
  }
  return(records)
}

#' Deposit Amount [REQ-0004] [REQ-0008]
#'
#' Deposits a converted amount into an existing account.
#'
#' @param records        The transaction records.
#' @param maxCount       Maximum allowed transactions.
#' @param filename       The CSV filename.
#' @param globalCurrency Named vector of exchange rates.
#'
#' @returns The updated transaction records.
#'
depositAmount <- function(records, maxCount, filename, globalCurrency) {
  account <- NULL
  status <- TRUE

  if (nrow(records) >= maxCount) {
    cat("Transaction limit reached.\n")
    status <- FALSE
  }

  if (status) {
    cat("Deposit Amount\n")
    accountNumber <- trimws(getConsoleInput("Account Number: "))
    if (nchar(accountNumber) != 10 || stringToAccountNumber(accountNumber) == 0) {
      cat("Invalid account number.\n")
      status <- FALSE
    }
  }

  if (status) {
    account <- findAccount(records, accountNumber)
    if (is.null(account)) {
      cat("Account not found.\n")
      status <- FALSE
    }
  }

  if (status) {
    cat(sprintf("Account Name: %s\n", account$accountName))
    cat(sprintf("Current Balance: %.2f\n", account$currentBalance))

    currencyChoice <- getValidCurrency(globalCurrency)
    valDeposit <- getValidAmount("Deposit Amount: ")
    amountPHP <- roundHalfUp(convertCurrency(valDeposit, currencyChoice, "PHP", globalCurrency))

    while (valDeposit <= 0 || !is.finite(amountPHP) || amountPHP <= 0) {
      cat("Please enter a valid amount.\n")
      valDeposit <- getValidAmount("")
      amountPHP <- roundHalfUp(convertCurrency(valDeposit, currencyChoice, "PHP", globalCurrency))
    }

    newTransaction <- account
    newTransaction$currency <- "PHP"
    newTransaction$type <- "deposit"
    newTransaction$amount <- amountPHP
    newTransaction$currentBalance <- roundHalfUp(account$currentBalance + amountPHP)
    newTransaction <- setTransactionDateTime(newTransaction)

    if (!saveTransaction(filename, newTransaction)) {
      cat("Cannot save deposit.\n")
      status <- FALSE
    }
  }

  if (status) {
    records <- rbind(records, newTransaction)
    rownames(records) <- NULL

    cat(sprintf("\nDeposit Amount = %.2f %s\n", valDeposit, currencyChoice))
    cat(sprintf("Updated Balance = %.2f\n", newTransaction$currentBalance))
  }
  return(records)
}

#' Withdraw Amount [REQ-0005] [REQ-0008]
#'
#' Withdraws a converted amount from an existing account.
#'
#' @param records        The transaction records.
#' @param maxCount       Maximum allowed transactions.
#' @param filename       The CSV filename.
#' @param globalCurrency Named vector of exchange rates.
#'
#' @returns The updated transaction records.
#'
withdrawAmount <- function(records, maxCount, filename, globalCurrency) {
  account <- NULL
  status <- TRUE

  if (nrow(records) >= maxCount) {
    cat("Transaction limit reached.\n")
    status <- FALSE
  }

  if (status) {
    cat("Withdraw Amount\n")
    accountNumber <- trimws(getConsoleInput("Account Number: "))
    if (nchar(accountNumber) != 10 || stringToAccountNumber(accountNumber) == 0) {
      cat("Invalid account number.\n")
      status <- FALSE
    }
  }

  if (status) {
    account <- findAccount(records, accountNumber)
    if (is.null(account)) {
      cat("Account not found.\n")
      status <- FALSE
    }
  }

  if (status) {
    cat(sprintf("Account Name: %s\n", account$accountName))
    cat(sprintf("Current Balance: %.2f\n", account$currentBalance))

    if (account$currentBalance <= 0) {
      cat("Insufficient balance to withdraw.\n")
      status <- FALSE
    } else {
      currencyChoice <- getValidCurrency(globalCurrency)
      valWithdraw <- getValidAmount("\nWithdraw Amount: ")
      amountPHP <- roundHalfUp(convertCurrency(valWithdraw, currencyChoice, "PHP", globalCurrency))

      while (valWithdraw <= 0 || !is.finite(amountPHP) || amountPHP <= 0 || amountPHP > account$currentBalance) {
        cat("Please enter a valid amount.\n")
        valWithdraw <- getValidAmount("")
        amountPHP <- roundHalfUp(convertCurrency(valWithdraw, currencyChoice, "PHP", globalCurrency))
      }

      newTransaction <- account
      newTransaction$currency <- "PHP"
      newTransaction$type <- "withdrawal"
      newTransaction$amount <- amountPHP
      newTransaction$currentBalance <- roundHalfUp(account$currentBalance - amountPHP)
      newTransaction <- setTransactionDateTime(newTransaction)

      if (!saveTransaction(filename, newTransaction)) {
        cat("Cannot save withdrawal.\n")
        status <- FALSE
      }
    }
  }

  if (status) {
    records <- rbind(records, newTransaction)
    rownames(records) <- NULL

    cat("\n***\n")
    cat(sprintf("Account Name = %s\n", account$accountName))
    cat(sprintf("Withdraw Amount = %.2f %s\n", valWithdraw, currencyChoice))
    cat(sprintf("Updated Balance = %.2f\n", newTransaction$currentBalance))
  }
  return(records)
}

#' Currency Exchange [REQ-0003]
#'
#' Converts an entered amount between two selected currencies.
#'
#' @param globalCurrency Named vector of exchange rates.
#'
currencyExchange <- function(globalCurrency) {
  runningFlag <- TRUE

  while (runningFlag) {
    cat("Foreign Currency Exchange\n")
    cat("Source Currency Option:\n")
    printExchangeMenu()
    sourceChoice <- getValidChoiceClean(1, 6, "Source Currency: ")

    source <- getValidAmount("Source Amount: ")
    while (source <= 0) {
      source <- getValidAmount("Please enter a valid amount: ")
    }

    cat("\nExchanged Currency Options:\n")
    printExchangeMenu()
    targetChoice <- getValidChoiceClean(1, 6, "Exchange Currency: ")

    result <- convertCurrency(source, sourceChoice, targetChoice, globalCurrency)
    cat("\n***\n")
    cat(sprintf("Exchange Currency = %d\n", targetChoice))
    cat(sprintf("Exchange Amount = %.2f\n\n", result))

    runningFlag <- getValidYN("Convert another currency (Y/N)? . . . ")
  }
}

#' Record Exchange Rate [REQ-0001] [REQ-0002]
#'
#' Updates the exchange rate of a selected foreign currency.
#'
#' @param globalCurrency Named vector of exchange rates.
#'
#' @returns The updated globalCurrency vector.
#'
recordExchangeRates <- function(globalCurrency) {
  cat("Record Exchange Rate\n\n")
  printExchangeMenu()
  choice <- getValidChoiceClean(2, 6, "Select Foreign Currency: ")

  if (choice >= 2) {
    exchangeRate <- getValidAmount("Exchange Rate: ")
    while (exchangeRate <= 0) {
      exchangeRate <- getValidAmount("Please enter a positive exchange rate: ")
    }

    globalCurrency[choice] <- exchangeRate
    cat("\n***\n")
    cat(sprintf("Select Foreign Currency = [%d]\n", choice))
    cat(sprintf("Exchange Rate = %.2f\n", exchangeRate))
  }
  return(globalCurrency)
}

#' Show Interest Amount [REQ-0006] [REQ-0007]
#'
#' Displays daily compounded interest projections for a selected account.
#'
#' @param records The transaction records.
#'
#' @returns TRUE if displayed, FALSE on error.
#'
showInterestAmount <- function(records) {
  annualRate <- 0.05
  status <- TRUE
  account <- NULL

  cat("Show Interest Amount\n")
  accountNumber <- trimws(getConsoleInput("Account Number: "))

  if (nchar(accountNumber) != 10 || stringToAccountNumber(accountNumber) == 0) {
    cat("Invalid account number.\n")
    status <- FALSE
  }

  if (status) {
    account <- findAccount(records, accountNumber)
    if (is.null(account)) {
      cat("Account not found.\n")
      status <- FALSE
    }
  }

  if (status) {
    eodBalance <- account$currentBalance

    cat(sprintf("Account Name: %s\n", account$accountName))
    cat(sprintf("Current Balance: %.2f\n", eodBalance))
    cat("Currency: PHP\n")
    cat("Interest Rate: 5%\n\n")

    numDays <- getValidChoiceClean(1, 3650, "Total number of days: ")
    cat(sprintf("\n%-5s | %-10s | %-10s\n", "Day", "Interest", "Balance"))

    for (i in 1:numDays) {
      dailyInterest <- roundHalfUp(eodBalance * (annualRate / 365))
      eodBalance <- eodBalance + dailyInterest
      cat(sprintf("%-5d | %-10.2f | %-10.2f\n", i, dailyInterest, eodBalance))
    }
  }
  return(status)
}

#' Calculate Expected Interest
#'
#' Computes 30-day compounded interest for a starting balance.
#'
#' @param startingBalance A numeric balance value.
#'
#' @returns The expected 30-day interest amount.
#'
calculateExpectedInterest <- function(startingBalance) {
  balance <- startingBalance

  for (day in 1:30) {
    dailyInterest <- roundHalfUp(balance * (0.05 / 365.0))
    balance <- balance + dailyInterest
  }

  return(roundHalfUp(balance - startingBalance))
}

#' Filter Interest Accounts
#'
#' Selects accounts with balance >= 10,000 PHP and calculates their 30-day interest.
#'
#' @param records        The transaction records.
#' @param accountIndexes Row indexes of each account's latest transaction.
#'
#' @returns A list with qualified accountIndexes and expectedInterest.
#'
filterInterestAccounts <- function(records, accountIndexes) {
  accountCount <- length(accountIndexes)
  qualifiedIndexes <- integer(accountCount)
  expectedInterest <- numeric(accountCount)
  qualifiedCount <- 0

  if (accountCount > 0) {
    for (i in 1:accountCount) {
      rowIndex <- accountIndexes[i]
      currentBalance <- records$currentBalance[rowIndex]
      if (currentBalance >= 10000.00) {
        qualifiedCount <- qualifiedCount + 1
        qualifiedIndexes[qualifiedCount] <- rowIndex
        expectedInterest[qualifiedCount] <- calculateExpectedInterest(currentBalance)
      }
    }
  }

  if (qualifiedCount > 0) {
    qualifiedIndexes <- qualifiedIndexes[1:qualifiedCount]
    expectedInterest <- expectedInterest[1:qualifiedCount]
  } else {
    qualifiedIndexes <- integer(0)
    expectedInterest <- numeric(0)
  }

  result <- list(
    accountIndexes = qualifiedIndexes,
    expectedInterest = expectedInterest
  )
  return(result)
}

#' Sort Interest Accounts
#'
#' Sorts interest accounts by expected interest descending, then account number ascending.
#'
#' @param records          The transaction records.
#' @param accountIndexes   Qualified account row indexes.
#' @param expectedInterest Expected interest values.
#'
#' @returns A list of sorted accountIndexes and expectedInterest.
#'
sortInterestAccounts <- function(records, accountIndexes, expectedInterest) {
  if (length(accountIndexes) > 1) {
    sortOrder <- order(-expectedInterest, records$accountNumber[accountIndexes])
    accountIndexes <- accountIndexes[sortOrder]
    expectedInterest <- expectedInterest[sortOrder]
  }

  result <- list(
    accountIndexes = accountIndexes,
    expectedInterest = expectedInterest
  )
  return(result)
}

#' Export Interest Report
#'
#' Exports Report 2 to CSV and prints a 2-row console preview.
#'
#' @param records          The transaction records.
#' @param accountIndexes   Sorted account row indexes.
#' @param expectedInterest Sorted expected interest values.
#'
#' @returns TRUE if exported, FALSE on error.
#'
exportInterestReport <- function(records, accountIndexes, expectedInterest) {
  qualifiedCount <- length(accountIndexes)
  header <- "AccountName,CurrentBalance,ExpectedInterest"
  rows <- character(0)

  if (qualifiedCount > 0) {
    rows <- sprintf(
      "%s,%.2f,%.2f",
      records$accountName[accountIndexes],
      records$currentBalance[accountIndexes],
      expectedInterest
    )
  }

  exportResult <- tryCatch({
    writeLines(c(header, rows), con = "report2_interest_expense_projection.csv")
    TRUE
  }, error = function(e) {
    cat("Cannot create Report 2 CSV.\n")
    FALSE
  })

  if (exportResult) {
    cat("\nReport 2: Interest Expense Projection\n")
    cat(sprintf("%-24s %-18s %-18s\n", "AccountName", "CurrentBalance", "ExpectedInterest"))

    previewCount <- min(2, qualifiedCount)
    if (previewCount > 0) {
      for (i in 1:previewCount) {
        rowIndex <- accountIndexes[i]
        cat(sprintf(
          "%-24s %-18.2f %-18.2f\n",
          records$accountName[rowIndex],
          records$currentBalance[rowIndex],
          expectedInterest[i]
        ))
      }
    }

    cat("(Full table exported to report2_interest_expense_projection.csv)\n")
    cat(sprintf("Eligible accounts: %d\n", qualifiedCount))
  }
  return(exportResult)
}

#' Generate Interest Report
#'
#' Generates Report 2 from transaction records.
#'
#' @param records The transaction records.
#'
#' @returns TRUE if generated, FALSE on error.
#'
generateInterestReport <- function(records) {
  result <- FALSE

  if (nrow(records) <= 0 || nrow(records) > MAX_TRANSACTIONS) {
    cat("No valid transactions available.\n")
  } else {
    accountIndexes <- getLatestAccountIndexes(records)
    filtered <- filterInterestAccounts(records, accountIndexes)
    sorted <- sortInterestAccounts(records, filtered$accountIndexes, filtered$expectedInterest)
    result <- exportInterestReport(records, sorted$accountIndexes, sorted$expectedInterest)
  }
  return(result)
}

#' Get Daily Activities
#'
#' Groups transactions by account and date in a single pass after sorting.
#'
#' @param records The transaction records.
#'
#' @returns A data frame of daily activity totals per account and date.
#'
getDailyActivities <- function(records) {
  recordCount <- nrow(records)
  # Sort so matching (accountNumber, date) rows sit right next to each other
  sortOrder <- order(records$accountNumber, records$date)

  accountNames <- character(recordCount)
  accountNumbers <- character(recordCount)
  dates <- character(recordCount)
  totalWithdrawalAmounts <- numeric(recordCount)
  numberOfDeposits <- integer(recordCount)
  totalDepositAmounts <- numeric(recordCount)
  activityCount <- 0

  if (recordCount > 0) {
    for (i in 1:recordCount) {
      rowIndex <- sortOrder[i]
      currentAccountNumber <- records$accountNumber[rowIndex]
      currentDate <- records$date[rowIndex]
      currentAmount <- records$amount[rowIndex]
      currentType <- records$type[rowIndex]

      if (activityCount == 0 ||
          accountNumbers[activityCount] != currentAccountNumber ||
          dates[activityCount] != currentDate) {
        activityCount <- activityCount + 1
        accountNames[activityCount] <- records$accountName[rowIndex]
        accountNumbers[activityCount] <- currentAccountNumber
        dates[activityCount] <- currentDate
      }

      if (currentType == "withdrawal") {
        totalWithdrawalAmounts[activityCount] <- totalWithdrawalAmounts[activityCount] + currentAmount
      } else if (currentType == "deposit") {
        numberOfDeposits[activityCount] <- numberOfDeposits[activityCount] + 1
        totalDepositAmounts[activityCount] <- totalDepositAmounts[activityCount] + currentAmount
      }
    }
  }

  activities <- data.frame(
    accountName = accountNames[1:activityCount],
    accountNumber = accountNumbers[1:activityCount],
    date = dates[1:activityCount],
    totalWithdrawalAmounts = totalWithdrawalAmounts[1:activityCount],
    numberOfDeposits = numberOfDeposits[1:activityCount],
    totalDepositAmounts = totalDepositAmounts[1:activityCount],
    turnoverVelocity = numeric(activityCount),
    velocityAvailable = rep(FALSE, activityCount),
    suspiciousFlag = integer(activityCount),
    stringsAsFactors = FALSE
  )
  return(activities)
}

#' Calculate Risk Flags
#'
#' Computes turnover velocity and suspicious flags for each daily activity row.
#'
#' @param activities The daily activities data frame.
#'
#' @returns The updated daily activities data frame.
#'
calculateRiskFlags <- function(activities) {
  activityCount <- nrow(activities)

  if (activityCount > 0) {
    totalWithdrawalAmounts <- activities$totalWithdrawalAmounts
    totalDepositAmounts <- activities$totalDepositAmounts

    velocityAvailable <- rep(FALSE, activityCount)
    turnoverVelocity <- numeric(activityCount)
    suspiciousFlag <- integer(activityCount)

    for (i in 1:activityCount) {
      if (totalDepositAmounts[i] > 0) {
        velocityAvailable[i] <- TRUE
        turnoverVelocity[i] <- (totalWithdrawalAmounts[i] / totalDepositAmounts[i]) * 100.0
      }

      if (totalWithdrawalAmounts[i] >= 500000.00 ||
          (velocityAvailable[i] && turnoverVelocity[i] >= 90.0)) {
        suspiciousFlag[i] <- 1
      }
    }

    activities$velocityAvailable <- velocityAvailable
    activities$turnoverVelocity <- turnoverVelocity
    activities$suspiciousFlag <- suspiciousFlag
  }
  return(activities)
}

#' Sort Daily Activities
#'
#' Sorts daily activities by date ascending, suspicious flag descending, and account number ascending.
#'
#' @param activities The daily activities data frame.
#'
#' @returns The sorted daily activities data frame.
#'
sortDailyActivities <- function(activities) {
  if (nrow(activities) > 1) {
    dateValues <- dateToSortableNumber(activities$date)
    sortOrder <- order(dateValues, -activities$suspiciousFlag, activities$accountNumber)
    activities <- activities[sortOrder, ]
    rownames(activities) <- NULL
  }
  return(activities)
}

#' Export Suspicious Report
#'
#' Exports Report 3 to CSV and prints a 2-row console preview.
#'
#' @param activities The sorted daily activities data frame.
#'
#' @returns TRUE if exported, FALSE on error.
#'
exportSuspiciousReport <- function(activities) {
  activityCount <- nrow(activities)
  header <- "AccountName,Date,TotalWithdrawalAmounts,NumberOfDeposits,TotalDepositAmounts,TurnoverVelocity (%),RiskFlagLevel"
  rows <- character(0)
  riskLevel <- character(activityCount)
  velocityText <- character(activityCount)

  if (activityCount > 0) {
    for (i in 1:activityCount) {
      if (activities$suspiciousFlag[i] == 1) {
        riskLevel[i] <- "Suspicious"
      } else {
        riskLevel[i] <- "Normal"
      }

      if (activities$velocityAvailable[i]) {
        velocityText[i] <- sprintf("%.2f", activities$turnoverVelocity[i])
      } else {
        velocityText[i] <- "N/A"
      }
    }

    rows <- sprintf(
      "%s,%s,%.2f,%d,%.2f,%s,%s",
      activities$accountName,
      activities$date,
      activities$totalWithdrawalAmounts,
      activities$numberOfDeposits,
      activities$totalDepositAmounts,
      velocityText,
      riskLevel
    )
  }

  exportResult <- tryCatch({
    writeLines(c(header, rows), con = "report3_suspicious_client_behavior.csv")
    TRUE
  }, error = function(e) {
    cat("Cannot create Report 3 CSV.\n")
    FALSE
  })

  if (exportResult) {
    cat("\nReport 3: Suspicious Client Behavior\n")
    cat(sprintf(
      "%-24s %-12s %18s %12s %18s %18s %-12s\n",
      "AccountName", "Date", "Withdrawals", "Deposits", "Deposit Total", "Velocity (%)", "Risk"
    ))

    previewCount <- min(2, activityCount)
    if (previewCount > 0) {
      for (i in 1:previewCount) {
        cat(sprintf(
          "%-24s %-12s %18.2f %12d %18.2f %18s %-12s\n",
          activities$accountName[i],
          activities$date[i],
          activities$totalWithdrawalAmounts[i],
          activities$numberOfDeposits[i],
          activities$totalDepositAmounts[i],
          velocityText[i],
          riskLevel[i]
        ))
      }
    }

    cat("(Full table exported to report3_suspicious_client_behavior.csv)\n")
    cat(sprintf("Daily activities: %d\n", activityCount))
  }
  return(exportResult)
}

#' Generate Suspicious Report
#'
#' Generates Report 3 from transaction records.
#'
#' @param records The transaction records.
#'
#' @returns TRUE if generated, FALSE on error.
#'
generateSuspiciousReport <- function(records) {
  result <- FALSE

  if (nrow(records) <= 0 || nrow(records) > MAX_TRANSACTIONS) {
    cat("No valid transactions available.\n")
  } else {
    activities <- getDailyActivities(records)
    activities <- calculateRiskFlags(activities)
    activities <- sortDailyActivities(activities)
    result <- exportSuspiciousReport(activities)
  }
  return(result)
}

#' Calculate Single Average Daily Balance
#'
#' Walks day-by-day through a month using one account's chronologically sorted transactions.
#'
#' @param accountDates    Sorted YYYYMMDDHHMMSS timestamps for one account.
#' @param accountBalances Corresponding balances for that account.
#' @param month           Report month (1-12).
#' @param year            Report year.
#'
#' @returns The average daily balance for that account.
#'
calculateSingleAverageDailyBalance <- function(accountDates, accountBalances, month, year) {
  daysInMonth <- getDaysInMonth(month, year)
  transactionCount <- length(accountDates)
  transactionIndex <- 1
  currentBalance <- 0.0
  totalDailyBalance <- 0.0

  for (day in 1:daysInMonth) {
    endOfDay <- ((year * 100 + month) * 100 + day) * 1000000 + 235959

    # Advance through all transactions that happened on or before endOfDay
    while (transactionIndex <= transactionCount && accountDates[transactionIndex] <= endOfDay) {
      currentBalance <- accountBalances[transactionIndex]
      transactionIndex <- transactionIndex + 1
    }

    totalDailyBalance <- totalDailyBalance + currentBalance
  }

  return(totalDailyBalance / daysInMonth)
}

#' Calculate Average Daily Balance
#'
#' Calculates the average daily balance for every account in a given month and year.
#'
#' @param records        The transaction records.
#' @param accountNumbers Character vector of unique account numbers.
#' @param month          The report month.
#' @param year           The report year.
#'
#' @returns A numeric vector of average daily balances.
#'
calculateAverageDailyBalance <- function(records, accountNumbers, month, year) {
  recordCount <- nrow(records)
  allDateValues <- getTransactionDateValue(records)
  sortedOrder <- order(allDateValues, 1:recordCount)

  sortedAccounts <- records$accountNumber[sortedOrder]
  sortedDates <- allDateValues[sortedOrder]
  sortedBalances <- records$currentBalance[sortedOrder]

  accountCount <- length(accountNumbers)
  averageDailyBalances <- numeric(accountCount)

  if (accountCount > 0) {
    for (i in 1:accountCount) {
      accountRowIndexes <- which(sortedAccounts == accountNumbers[i])
      averageDailyBalances[i] <- calculateSingleAverageDailyBalance(
        sortedDates[accountRowIndexes],
        sortedBalances[accountRowIndexes],
        month,
        year
      )
    }
  }
  return(averageDailyBalances)
}

#' Get Privilege Tier
#'
#' Classifies an average daily balance into Silver, Gold, or Platinum.
#'
#' @param averageDailyBalance The average daily balance.
#'
#' @returns The tier name string.
#'
getPrivilegeTier <- function(averageDailyBalance) {
  if (averageDailyBalance < 500000.00) {
    return("Silver")
  } else if (averageDailyBalance < 1000000.00) {
    return("Gold")
  } else {
    return("Platinum")
  }
}

#' Sort Privilege Accounts
#'
#' Sorts privilege accounts by average daily balance descending, then account number ascending.
#'
#' @param accounts The privilege accounts data frame.
#'
#' @returns The sorted privilege accounts data frame.
#'
sortPrivilegeAccounts <- function(accounts) {
  if (nrow(accounts) > 1) {
    sortOrder <- order(-accounts$averageDailyBalance, accounts$accountNumber)
    accounts <- accounts[sortOrder, ]
    rownames(accounts) <- NULL
  }
  return(accounts)
}

#' Export Privilege Report
#'
#' Exports Report 1 to CSV and prints a 2-row console preview.
#'
#' @param accounts The sorted privilege accounts data frame.
#'
#' @returns TRUE if exported, FALSE on error.
#'
exportPrivilegeReport <- function(accounts) {
  qualifiedCount <- nrow(accounts)
  header <- "AccountName,AccountAge,CurrentBalance,AverageDailyBalance,PrivilegeTier"
  rows <- character(0)

  if (qualifiedCount > 0) {
    rows <- sprintf(
      "%s,%d,%.2f,%.2f,%s",
      accounts$accountName,
      accounts$accountAge,
      accounts$currentBalance,
      accounts$averageDailyBalance,
      accounts$privilegeTier
    )
  }

  exportResult <- tryCatch({
    writeLines(c(header, rows), con = "report1_banking_privilege_tiers.csv")
    TRUE
  }, error = function(e) {
    cat("Cannot create Report 1 CSV.\n")
    FALSE
  })

  if (exportResult) {
    cat("\nReport 1: Banking Privilege Tiers\n")
    cat(sprintf(
      "%-24s %-11s %-16s %-20s %-14s\n",
      "AccountName", "AccountAge", "CurrentBalance", "AverageDailyBalance", "PrivilegeTier"
    ))

    previewCount <- min(2, qualifiedCount)
    if (previewCount > 0) {
      for (i in 1:previewCount) {
        cat(sprintf(
          "%-24s %-11d %-16.2f %-20.2f %-14s\n",
          accounts$accountName[i],
          accounts$accountAge[i],
          accounts$currentBalance[i],
          accounts$averageDailyBalance[i],
          accounts$privilegeTier[i]
        ))
      }
    }

    cat("(Full table exported to report1_banking_privilege_tiers.csv)\n")
    cat(sprintf("Qualified accounts: %d\n", qualifiedCount))
  }
  return(exportResult)
}

#' Generate Privilege Report
#'
#' Generates Report 1 from transaction records.
#'
#' @param records The transaction records.
#'
#' @returns TRUE if generated, FALSE on error.
#'
generatePrivilegeReport <- function(records) {
  result <- FALSE
  recordCount <- nrow(records)

  if (recordCount <= 0 || recordCount > MAX_TRANSACTIONS) {
    cat("No valid transactions available.\n")
  } else {
    allDateValues <- getTransactionDateValue(records)
    latestIndex <- order(allDateValues, 1:recordCount, decreasing = TRUE)[1]
    referenceDay <- dateToDayCount(records$date[latestIndex])

    latestDate <- parseDateParts(records$date[latestIndex])
    reportMonth <- latestDate$month - 1
    reportYear <- latestDate$year
    if (reportMonth == 0) {
      reportMonth <- 12
      reportYear <- reportYear - 1
    }

    accountIndexes <- getLatestAccountIndexes(records)
    latestAccounts <- records[accountIndexes, ]
    accountCount <- nrow(latestAccounts)
    averageDailyBalances <- calculateAverageDailyBalance(
      records, latestAccounts$accountNumber, reportMonth, reportYear
    )
    allDayCounts <- dateToDayCount(records$date)

    accountNames <- character(accountCount)
    accountNumbers <- character(accountCount)
    accountAges <- integer(accountCount)
    currentBalances <- numeric(accountCount)
    qualifiedAverageBalances <- numeric(accountCount)
    privilegeTiers <- character(accountCount)
    qualifiedCount <- 0

    if (accountCount > 0) {
      for (i in 1:accountCount) {
        averageDailyBalance <- averageDailyBalances[i]
        if (averageDailyBalance >= 100000.00) {
          qualifiedCount <- qualifiedCount + 1
          accountNumber <- latestAccounts$accountNumber[i]
          earliestDay <- min(allDayCounts[records$accountNumber == accountNumber])

          accountNames[qualifiedCount] <- latestAccounts$accountName[i]
          accountNumbers[qualifiedCount] <- accountNumber
          accountAges[qualifiedCount] <- as.integer(referenceDay - earliestDay)
          currentBalances[qualifiedCount] <- latestAccounts$currentBalance[i]
          qualifiedAverageBalances[qualifiedCount] <- averageDailyBalance
          privilegeTiers[qualifiedCount] <- getPrivilegeTier(averageDailyBalance)
        }
      }
    }

    if (qualifiedCount > 0) {
      accounts <- data.frame(
        accountName = accountNames[1:qualifiedCount],
        accountNumber = accountNumbers[1:qualifiedCount],
        accountAge = accountAges[1:qualifiedCount],
        currentBalance = currentBalances[1:qualifiedCount],
        averageDailyBalance = qualifiedAverageBalances[1:qualifiedCount],
        privilegeTier = privilegeTiers[1:qualifiedCount],
        stringsAsFactors = FALSE
      )
    } else {
      accounts <- data.frame(
        accountName = character(0),
        accountNumber = character(0),
        accountAge = integer(0),
        currentBalance = numeric(0),
        averageDailyBalance = numeric(0),
        privilegeTier = character(0),
        stringsAsFactors = FALSE
      )
    }

    accounts <- sortPrivilegeAccounts(accounts)
    result <- exportPrivilegeReport(accounts)
  }
  return(result)
}

#' Get Depletion Risk Level
#'
#' Classifies a depletion rate into Moderate, High, or Critical.
#'
#' @param depletionRate The depletion rate percentage.
#'
#' @returns The risk level string.
#'
getDepletionRiskLevel <- function(depletionRate) {
  if (depletionRate >= 40.0) {
    return("Critical")
  } else if (depletionRate >= 25.0) {
    return("High")
  } else {
    return("Moderate")
  }
}

#' Sort Depletion Accounts
#'
#' Sorts depletion accounts by depletion rate descending, balance drop descending, and account number ascending.
#'
#' @param accounts The depletion accounts data frame.
#'
#' @returns The sorted depletion accounts data frame.
#'
sortDepletionAccounts <- function(accounts) {
  if (nrow(accounts) > 1) {
    sortOrder <- order(-accounts$depletionRate, -accounts$balanceDrop, accounts$accountNumber)
    accounts <- accounts[sortOrder, ]
    rownames(accounts) <- NULL
  }
  return(accounts)
}

#' Export Depletion Report
#'
#' Exports Report 4 to CSV and prints a 2-row console preview.
#'
#' @param accounts The sorted depletion accounts data frame.
#'
#' @returns TRUE if exported, FALSE on error.
#'
exportDepletionReport <- function(accounts) {
  qualifiedCount <- nrow(accounts)
  header <- "AccountName,Balance30DaysAgo,CurrentBalance,BalanceDrop,DepletionRate (%),RiskLevel"
  rows <- character(0)

  if (qualifiedCount > 0) {
    rows <- sprintf(
      "%s,%.2f,%.2f,%.2f,%.2f,%s",
      accounts$accountName,
      accounts$balance30DaysAgo,
      accounts$currentBalance,
      accounts$balanceDrop,
      accounts$depletionRate,
      accounts$riskLevel
    )
  }

  exportResult <- tryCatch({
    writeLines(c(header, rows), con = "report4_balance_depletion_watchlist.csv")
    TRUE
  }, error = function(e) {
    cat("Cannot create Report 4 CSV.\n")
    FALSE
  })

  if (exportResult) {
    cat("\nReport 4: Account Balance Depletion Watchlist\n")
    cat(sprintf(
      "%-24s %-18s %-16s %-14s %-18s %-10s\n",
      "AccountName", "Balance30DaysAgo", "CurrentBalance",
      "BalanceDrop", "DepletionRate (%)", "RiskLevel"
    ))

    previewCount <- min(2, qualifiedCount)
    if (previewCount > 0) {
      for (i in 1:previewCount) {
        cat(sprintf(
          "%-24s %-18.2f %-16.2f %-14.2f %-18.2f %-10s\n",
          accounts$accountName[i],
          accounts$balance30DaysAgo[i],
          accounts$currentBalance[i],
          accounts$balanceDrop[i],
          accounts$depletionRate[i],
          accounts$riskLevel[i]
        ))
      }
    }

    cat("(Full table exported to report4_balance_depletion_watchlist.csv)\n")
    cat(sprintf("Watchlist accounts: %d\n", qualifiedCount))
  }
  return(exportResult)
}

#' Generate Depletion Report
#'
#' Generates Report 4 from transaction records.
#'
#' @param records The transaction records.
#'
#' @returns TRUE if generated, FALSE on error.
#'
generateDepletionReport <- function(records) {
  result <- FALSE
  recordCount <- nrow(records)

  if (recordCount <= 0 || recordCount > MAX_TRANSACTIONS) {
    cat("No valid transactions available.\n")
  } else {
    allDateValues <- getTransactionDateValue(records)
    latestIndex <- order(allDateValues, 1:recordCount, decreasing = TRUE)[1]
    referenceDay <- dateToDayCount(records$date[latestIndex])
    cutoffDay <- referenceDay - 30

    accountIndexes <- getLatestAccountIndexes(records)
    latestAccounts <- records[accountIndexes, ]
    accountCount <- nrow(latestAccounts)

    recordDays <- dateToDayCount(records$date)
    priorRecords <- records[recordDays <= cutoffDay, ]
    priorLatest <- if (nrow(priorRecords) > 0) {
      priorRecords[getLatestAccountIndexes(priorRecords), ]
    } else {
      emptyTransactions()
    }

    accountNames <- character(accountCount)
    accountNumbers <- character(accountCount)
    balance30DaysAgoList <- numeric(accountCount)
    currentBalanceList <- numeric(accountCount)
    balanceDropList <- numeric(accountCount)
    depletionRateList <- numeric(accountCount)
    riskLevelList <- character(accountCount)
    qualifiedCount <- 0

    if (accountCount > 0 && nrow(priorLatest) > 0) {
      for (i in 1:accountCount) {
        accountNumber <- latestAccounts$accountNumber[i]
        priorMatchIndexes <- which(priorLatest$accountNumber == accountNumber)

        if (length(priorMatchIndexes) > 0) {
          balance30DaysAgo <- priorLatest$currentBalance[priorMatchIndexes[1]]
          currentBalance <- latestAccounts$currentBalance[i]

          if (balance30DaysAgo > 0.0) {
            balanceDrop <- balance30DaysAgo - currentBalance
            depletionRate <- (balanceDrop / balance30DaysAgo) * 100.0

            if (depletionRate >= 10.0) {
              qualifiedCount <- qualifiedCount + 1
              accountNames[qualifiedCount] <- latestAccounts$accountName[i]
              accountNumbers[qualifiedCount] <- accountNumber
              balance30DaysAgoList[qualifiedCount] <- balance30DaysAgo
              currentBalanceList[qualifiedCount] <- currentBalance
              balanceDropList[qualifiedCount] <- balanceDrop
              depletionRateList[qualifiedCount] <- depletionRate
              riskLevelList[qualifiedCount] <- getDepletionRiskLevel(depletionRate)
            }
          }
        }
      }
    }

    if (qualifiedCount > 0) {
      accounts <- data.frame(
        accountName = accountNames[1:qualifiedCount],
        accountNumber = accountNumbers[1:qualifiedCount],
        balance30DaysAgo = balance30DaysAgoList[1:qualifiedCount],
        currentBalance = currentBalanceList[1:qualifiedCount],
        balanceDrop = balanceDropList[1:qualifiedCount],
        depletionRate = depletionRateList[1:qualifiedCount],
        riskLevel = riskLevelList[1:qualifiedCount],
        stringsAsFactors = FALSE
      )
    } else {
      accounts <- data.frame(
        accountName = character(0),
        accountNumber = character(0),
        balance30DaysAgo = numeric(0),
        currentBalance = numeric(0),
        balanceDrop = numeric(0),
        depletionRate = numeric(0),
        riskLevel = character(0),
        stringsAsFactors = FALSE
      )
    }

    accounts <- sortDepletionAccounts(accounts)
    result <- exportDepletionReport(accounts)
  }
  return(result)
}

#' Generate Summary Json
#'
#' Generates summary.json from transaction records.
#'
#' @param records The transaction records.
#'
#' @returns TRUE if generated, FALSE on error.
#'
generateSummaryJson <- function(records) {
  result <- FALSE

  if (nrow(records) <= 0 || nrow(records) > MAX_TRANSACTIONS) {
    cat("No valid transactions available.\n")
  } else {
    totalDeposits <- sum(records$amount[records$type == "deposit"])
    totalWithdrawals <- sum(records$amount[records$type == "withdrawal"])

    accountIndexes <- getLatestAccountIndexes(records)
    accountCount <- length(accountIndexes)
    latestBalances <- records$currentBalance[accountIndexes]
    totalBalance <- sum(latestBalances)

    totalInterestExpense <- 0.0
    if (accountCount > 0) {
      for (i in 1:accountCount) {
        if (latestBalances[i] >= 10000.00) {
          totalInterestExpense <- totalInterestExpense + calculateExpectedInterest(latestBalances[i])
        }
      }
    }

    jsonLine <- sprintf(
      "{\"total_accounts\": %d, \"total_deposits\": %.2f, \"total_withdrawals\": %.2f, \"total_balance\": %.2f, \"total_interest_expense\": %.2f}",
      accountCount,
      totalDeposits,
      totalWithdrawals,
      totalBalance,
      totalInterestExpense
    )

    result <- tryCatch({
      writeLines(jsonLine, con = "summary.json")
      TRUE
    }, error = function(e) {
      cat("Cannot create summary.json.\n")
      FALSE
    })

    if (result) {
      cat("\nSummary State (summary.json):\n")
      cat(jsonLine, "\n", sep = "")
    }
  }
  return(result)
}

#' Generate Reports Menu
#'
#' Handles the report menu loop for loading the file and generating reports.
#'
#' @param records  The transaction records.
#' @param maxCount Maximum allowed transactions.
#' @param filename The CSV filename.
#'
#' @returns The updated transaction records.
#'
generateReportsMenu <- function(records, maxCount, filename) {
  runningFlag <- TRUE

  while (runningFlag) {
    printReportMenu()
    choice <- getValidChoiceClean(0, 2, "Enter choice: ")

    if (choice == 1) {
      cat("Processing dataset...\n")
      loadedRecords <- loadTransactions(filename, maxCount)
      if (is.null(loadedRecords)) {
        cat("Error in loading dataset.\n")
      } else {
        records <- loadedRecords
        cat(sprintf("Dataset loaded! %d PHP transactions.\n", nrow(records)))
      }
    } else if (choice == 2) {
      if (nrow(records) <= 0) {
        cat("There is no currently loaded dataset.\n")
      } else {
        generatePrivilegeReport(records)
        generateInterestReport(records)
        generateSuspiciousReport(records)
        generateDepletionReport(records)
        generateSummaryJson(records)
      }
    } else if (choice == 0) {
      runningFlag <- FALSE
    }
  }
  return(records)
}

#' Transaction Select
#'
#' Dispatches the user's selected menu option.
#'
#' @param choice         The selected choice integer.
#' @param globalCurrency Named vector of exchange rates.
#' @param records        The transaction records.
#' @param maxCount       Maximum allowed transactions.
#' @param filename       The CSV filename.
#'
#' @returns A list of updated records and globalCurrency.
#'
transactionSelect <- function(choice, globalCurrency, records, maxCount, filename) {
  if (choice == 1) {
    records <- registerAccount(records, maxCount, filename)
  } else if (choice == 2) {
    records <- depositAmount(records, maxCount, filename, globalCurrency)
  } else if (choice == 3) {
    records <- withdrawAmount(records, maxCount, filename, globalCurrency)
  } else if (choice == 4) {
    currencyExchange(globalCurrency)
  } else if (choice == 5) {
    globalCurrency <- recordExchangeRates(globalCurrency)
  } else if (choice == 6) {
    showInterestAmount(records)
  } else if (choice == 7) {
    records <- generateReportsMenu(records, maxCount, filename)
  } else {
    cat("ERROR: Invalid selection.\n")
  }

  result <- list(records = records, globalCurrency = globalCurrency)
  return(result)
}

#' Main Program Function
#'
#' Runs the main banking and currency exchange loop.
#'
main <- function() {
  globalCurrency <- c(
    PHP = 1.00,
    USD = 62.00,
    JPY = 0.40,
    GBP = 84.00,
    EUR = 72.00,
    CNY = 9.00
  )

  runningFlag <- TRUE
  filename <- "bank_transactions.csv"

  records <- loadTransactions(filename, MAX_TRANSACTIONS)

  if (is.null(records)) {
    cat("Error in loading the transactions.\n")
    runningFlag <- FALSE
  } else {
    cat(sprintf("Successfully loaded %d transactions.\n", nrow(records)))
  }

  while (runningFlag) {
    printNavMenu()
    choice <- getValidChoice(0, 7)

    if (choice != 0) {
      state <- transactionSelect(choice, globalCurrency, records, MAX_TRANSACTIONS, filename)
      records <- state$records
      globalCurrency <- state$globalCurrency

      if (choice != 4 && choice != 7) {
        runningFlag <- getValidYN("Back to Main Menu (Y/N): ")
      }
    } else {
      runningFlag <- FALSE
    }
  }

  if (!is.null(STDIN_CON)) {
    close(STDIN_CON)
  }

  if (!is.null(records)) {
    cat("Exiting..\n")
  }
}

main()