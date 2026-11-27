
/********************
Last names: Ilagan, Serios, Yardan
Language: Kotlin
Paradigm(s): Procedural
********************/

import java.io.File
import java.time.LocalDate
import java.time.LocalTime
import java.time.YearMonth
import java.time.format.DateTimeFormatter
import java.time.temporal.ChronoUnit


/**
 * This function generates the next account number based on the highest
 * account number in the csv (comma-separated-value) file.
 * @param fileName The string file name of the csv file.
 * @return The account number with Long Int data type to accomodate all
 * possible 10-digit number.
 */
fun generateNextAccNum (fileName : String) : Long {
    val file = File(fileName)
    var highestNumber = 999999999L

    if (file.exists()) {
        val csvLines = file.readLines()

        //traverse all lines except the header
        for (i in 1 until csvLines.size) {
            val line = csvLines[i].trim()
            if(line.isNotEmpty()) {
                val attributes = line.split(",")
                try {
                    val curAccNum = attributes[1].trim().toLong()
                    if (curAccNum > highestNumber) {
                        highestNumber = curAccNum
                    }
                }
                catch (e: Exception) {
                    println("ERROR: data type mismatch in account number column.")
                }
            }
        }
    }
    return highestNumber + 1L
}

/**
 * This function prompts the user to enter an initial deposit of at least 5000 Pesos.
 * @return the initial deposit at least 5000 in Peso.
 */
fun promptInitialDeposit() : Float {
    var deposit = 0.0f

    while(deposit < 5000.0F) {
        print("Enter Initial Deposit in PHP (Min. 5000.00): ")
        deposit = promptFloat()
        if(deposit < 5000.0F) {
            println("ERROR! Deposit money at least Php. 5000.00.")
        }
    }
    return deposit
}

fun saveTransactions(fileName : String, account : BankAccount, transactionType: String, amount: Float) {
    val file = File(fileName)

    if(!(file.exists())) {
        file.writeText("account_name,account_number,date,time,currency,amount,type,current_balance\n")
    }

    //create the time attributes aligned to the csv file format..
    val currentDate = LocalDate.now().format(DateTimeFormatter.ofPattern("d/M/yyyy"))
    val currentTime = LocalTime.now().format(DateTimeFormatter.ofPattern("H:mm:ss"))

    val row = String.format("%s,%d,%s,%s,%s,%.2f,%s,%.2f\n",
        account.accountName,
        account.accountNumber,
        currentDate,
        currentTime,
        account.currency,
        amount,
        transactionType,
        account.balance)

    file.appendText(row)
}

fun registerAccount(accounts: ArrayList<BankAccount>) {
    val fileName = "bank_transactions.csv"
    var boolLoop: Boolean

    do{
        println("\nRegistration of Account")
        val accountName = promptName()
        val accIdx = accountIndex(accountName, accounts)

        if(accIdx == -1) {
            val newAccNum = generateNextAccNum(fileName)
            val initialDeposit = promptInitialDeposit()
            val newAcc = BankAccount(newAccNum, accountName, initialDeposit, "PHP")
            accounts.add(newAcc)
            saveTransactions(fileName, newAcc, "deposit", initialDeposit)
            println("\nAccount created successfully!\n")
        }
        else {
            println("This account already exists")
        }
        print("Register another account? (Y/N): ")
        boolLoop = promptYN()
    }while (boolLoop)
}

fun cleanFinance(value: String): Float{
    val cleaned = value.trim().replace(",", "")
        .replace("php", "", ignoreCase = true)
        .replace("php.", "", ignoreCase = true)
        .trim()
    return cleaned.toFloat()
}
fun boolMissingCols (column: List<String>) : Boolean {
    var isMissing = false

    if(column.size < 8) {
        isMissing = true
    }
    else {
        for (j in 0..7) {
            if(column[j].trim().isEmpty())
                isMissing = true
        }
    }
    return isMissing
}

fun loadCleanCsv(fileName : String, cleanedRecord: ArrayList<TransactionRecord>) {
    val file = File(fileName)

    if(!(file.exists())) {
        println("ERROR: file does not exist.")
    }
    //if the file exists, clean the dataset.
    else {
        cleanedRecord.clear()
        val lines = file.readLines()
        var numErrors = 0
        var numNonPHP = 0
        val dateFormat = DateTimeFormatter.ofPattern("d/M/yyyy")
        val timeFormat = DateTimeFormatter.ofPattern("H:mm:ss")

        val totalDataRows : Int = if(lines.size > 1)
            lines.size - 1
        else
            0

        //traverse all lines except header
        for (i in 1 until lines.size) {
            val line = lines[i].trim()
            //log empty rows
            if(line.isEmpty()) {
                numErrors++
                println("WARNING! Row ${i + 1} not found.")
            }
            else {
                val columns = line.split(",")
                //log missing column field in a row
                if(boolMissingCols(columns)) {
                    numErrors++
                    println("WARNING! Row ${i+1} has a missing field.")
                }
                else {
                    try {
                        val accountName = columns[0].trim()
                        val accountNumber = columns[1].trim().toLong()
                        //parse dates or use date data types
                        val parsedDate = LocalDate.parse(columns[2].trim(), dateFormat)
                        val parsedTime = LocalTime.parse(columns[3].trim(), timeFormat)
                        val currency = columns[4].trim().uppercase()
                        val amount = cleanFinance(columns[5])
                        val type = columns[6].trim().lowercase()
                        val currentBalance = cleanFinance(columns[7])

                        //log transaction type error
                        if (type != "deposit" && type != "withdrawal") {
                            numErrors++
                            println("WARNING! Row ${i+1}: Invalid transaction type.")
                        }
                        else if (currency == "PHP") {
                            cleanedRecord.add(
                                TransactionRecord(
                                    accountName = accountName,
                                    accountNumber = accountNumber,
                                    date = parsedDate,
                                    time = parsedTime,
                                    currency = currency,
                                    amount = amount,
                                    type = type,
                                    currentBalance = currentBalance
                                )
                            )
                        }
                        else
                            numNonPHP++
                    } catch (e: Exception) {
                        numErrors++
                        if(numErrors <= 5)
                            println("WARNING! Row ${i+1}: Parsing Error.")
                    }
                }
            }
        }
        println("Processing dataset... ($totalDataRows rows loaded, ${cleanedRecord.size} filtered for PHP currency)")
        println("\n$numNonPHP rows of non-PHP currency records filtered.\n" +
                "$numErrors incomplete or invalid rows skipped.")
    }
}

fun syncAccounts(cleanedRecord: ArrayList<TransactionRecord>, accounts: ArrayList<BankAccount>) {
    accounts.clear()
    val latestDates = ArrayList<LocalDate>()
    val latestTimes = ArrayList<LocalTime>()

    //traverse all clean records
    for(i in 0 until cleanedRecord.size) {
        val record = cleanedRecord[i]
        val accIdx = accountIndex(record.accountName, accounts)

        //if a cleaned record is not present in accounts, add it.
        if (accIdx == -1) {
            accounts.add(BankAccount(accountNumber = record.accountNumber,
                accountName = record.accountName,
                balance = record.currentBalance,
                currency = record.currency))
            latestDates.add(record.date)
            latestTimes.add(record.time)
        }
        else {
            //update the balance of this account with record's current balance
            val isLatestDate = record.date.isAfter(latestDates[accIdx])
            val isLatestTime = record.date.isEqual(latestDates[accIdx]) && !record.time.isBefore(latestTimes[accIdx])
            if(isLatestDate || isLatestTime) {
                accounts[accIdx].balance = record.currentBalance
                latestDates[accIdx] = record.date
                latestTimes[accIdx] = record.time
            }
        }
    }
}

fun genReportsMenu(fileName: String, cleanedRecord: ArrayList<TransactionRecord>, accounts: ArrayList<BankAccount>) {
    var exit = false

    while(!exit) {
        println("\nGenerate Reports")
        println("[1] Load the file.")
        println("[2] Generate Reports")
        println("[3] Exit Reports Menu")
        print("Enter choice: ")

        when (promptOneToN(3)) {
            1 -> {
                loadCleanCsv(fileName, cleanedRecord)
                syncAccounts(cleanedRecord, accounts)
            }

            2 -> {
                if (cleanedRecord.isEmpty()) {
                    println("Record is empty! Select [1] to load the file.")
                } else {
                    println("Generating reports...\nOutputs saved to individual files...")
                    genReport1(accounts, cleanedRecord)
                    genReport2(accounts, 0.05F)
                }
            }

            3 -> exit = true
        }
    }
}

fun genReport1(accounts : ArrayList<BankAccount>, cleanedRecord: ArrayList<TransactionRecord>) {
    //get the date of the latest transaction
    val latestDate = if (cleanedRecord.isNotEmpty()) {
        cleanedRecord.maxOf { trans -> trans.date }
    } else {
        LocalDate.now()
    }

    val lastMonth = YearMonth.from(latestDate).minusMonths(1)
    val lastMonthDays = lastMonth.lengthOfMonth()

    val transByAcc = cleanedRecord.groupBy {it.accountNumber}
    /*transform accounts, compute age and average daily balance, filter out accounts with avgdailybalance below 100k
    and sort in descending order based on the avgdailybalance*/
    val repRows = accounts.map{ acc ->
        val transOfAcc = transByAcc[acc.accountNumber]
            .orEmpty()
            .sortedBy { it.date }

        val accOpenDate = if (transOfAcc.isNotEmpty()) {
            transOfAcc.first().date
        } else{
            latestDate
        }
        val accAge = ChronoUnit.DAYS.between(accOpenDate, latestDate)

        var eodSumBalance = 0.0f
        //traverse all last month days
        for (day in 1..lastMonthDays) {
            val presentDay = lastMonth.atDay(day)
            //end of present day balance computed as sum of deposits and deductions from withdrawals
            val eodBalance = transOfAcc.filter{ trans -> !trans.date.isAfter(presentDay)}
                .sumOf {trans ->
                    if(trans.type.equals("Deposit", ignoreCase = true)) {
                        trans.amount.toDouble()
                    }
                    else {
                        -trans.amount.toDouble()
                    }
                }.toFloat()

            eodSumBalance += eodBalance
        }

        val avgDailyBalance = eodSumBalance / lastMonthDays.toFloat()

        val tier = when {
            avgDailyBalance >= 1000000.0f -> "Platinum"
            avgDailyBalance >= 500000.0f -> "Gold"
            else -> "Silver"
        }

        TierRow(accountName = acc.accountName,
            accAge = accAge,
            currentBalance = acc.balance,
            avgDailyBalance = avgDailyBalance,
            tier = tier,)
    }.filter {row -> row.avgDailyBalance >= 100000.0f }
        .sortedByDescending { row -> row.avgDailyBalance }

    val header = "AccountName,AccountAge,CurrentBalance,AverageDailyBalance,PrivilegeTier\n"
    val body = repRows.joinToString("\n") { row -> String.format(
        "%s,%d,%.2f,%.2f,%s",
        row.accountName,
        row.accAge,
        row.currentBalance,
        row.avgDailyBalance,
        row.tier)
    }

    //export the full table to a csv file
    val exportFile = File("MC02_Report1.csv")
    exportFile.writeText(header + body + "\n")

    // print the 2 line preview
    println("\nReport 1: Banking Privilege Tiers")
    println(String.format("%-25s | %-10s | %-15s | %-20s | %-15s",
        "AccountName",
        "AccountAge",
        "CurrentBalance",
        "AverageDailyBalance",
        "PrivilegeTier",)
    )

    repRows.take(2).forEach { row-> println(String.format(
        "%-25s | %-10d | %-15.2f | %-20.2f | %-15s",
        row.accountName,
        row.accAge,
        row.currentBalance,
        row.avgDailyBalance,
        row.tier))
    }
}

fun genReport2(accounts: ArrayList<BankAccount>, annualInterest: Float) {
    //filter accounts with >= 10000.0 balance and compute the expected Interest for day 30
    val repRows = accounts.filter { it.balance >= 10000.0f}
        .map { acc ->
            var updatedBalance = acc.balance
            var expectedInt = 0.0f
            var day = 1

            while (day <= 30) {
                val dailyInt = updatedBalance * (annualInterest / 365.0F)
                val roundedDaily = String.format("%.2f", dailyInt).toFloat()
                expectedInt += roundedDaily
                updatedBalance += roundedDaily
                day++
            }

            InterestProjectionRow(accountName = acc.accountName, currentBalance = acc.balance,
                expectedInterest = expectedInt)
        }.sortedByDescending { it.expectedInterest }

    val header = "AccountName,CurrentBalance,ExpectedInterest\n"
    val body = repRows.joinToString("\n") { row -> String.format(
        "%s,%.2f,%.2f", row.accountName, row.currentBalance, row.expectedInterest)
    }

    //export the full table to a csv file
    val exportFile = File("MC02_Report2.csv")
    exportFile.writeText(header + body + "\n")

    // print the 2 line preview
    println("\nReport 2: Interest Expense Projection")
    println(String.format("%-25s | %-15s | %-15s", "AccountName",
        "CurrentBalance", "ExpectedInterest"))

    repRows.take(2).forEach { row-> println(String.format(
        "%-25s | %-15.2f | %-15.2f",
        row.accountName,
        row.currentBalance,
        row.expectedInterest)
        )
    }
}

fun genReport3(accounts: ArrayList<BankAccount>, cleanedRecord: ArrayList<TransactionRecord>) {
    val distinctAccounts = accounts.map { it.accountName }.toSet()

    val repRows = cleanedRecord
        .asSequence()
        .filter { it.accountName in distinctAccounts }
        //for each account per day
        .groupBy { it.accountName to it.date}
        .map { }
}

/**
 * This class used to create bank account objects for encapsulation of data.
 * @param accountName String name of the bank user
 * @param balance Amount of money in a bank account
 * @param currency Name of the currency, PHP as the base currency.
 */
class BankAccount (val accountNumber : Long, val accountName : String, var balance : Float, val currency : String)

class TransactionRecord(val accountName: String, val accountNumber: Long, val date: LocalDate, val time: LocalTime,
                        val currency: String, val amount: Float, val type: String, val currentBalance: Float)
class TierRow(val accountName: String, val accAge: Long, val currentBalance: Float, val avgDailyBalance: Float, val tier: String)
class InterestProjectionRow(val accountName: String, val currentBalance: Float, val expectedInterest: Float)
class SuspiciousActRow(val accountName: String, val date: LocalDate, val sumWithdrawAmt: Double, numDeposits: Int,
    sumDepositAmt: Double, val turnoverVelo: Double, val riskFlagLvl: String)
/**
 * This function prompts the user to select a menu service.
 * @return choice of service from 1 to 6.
 */
fun mainMenu(): Int{
    var choice: Int
    print(
        message = "Select Transaction:\n" +
                "[1] Register Account Name\n" +
                "[2] Deposit Amount\n" +
                "[3] Withdraw Amount\n" +
                "[4] Currency Exchange\n" +
                "[5] Record Exchange Rates\n" +
                "[6] Show Interest Amount\n" +
                "[7] Generate Reports\n" +
                "Choice: "
    )

    choice = promptOneToN(7)
    return choice
}

/**
 * Helper function promptName to reduce code duplications
 * This checks if the Account Name input does not consist of numbers.
 * Additionally, this function trims and checks for whitespaces.
 * @return a valid bank account name.
 */
fun promptName() : String {
    var name = ""
    while (name.isEmpty()) {
        try {
            print("Account Name: ")
            name = readln().trim()

            if(name.isBlank()) throw IllegalArgumentException()

            for(i in 0 until name.length) {
                if (name[i].isDigit()) throw IllegalArgumentException()
                else if (name[i] == ',') throw IllegalArgumentException()
            }
        }catch (e : IllegalArgumentException){
            name = ""
            println("ERROR! Must not have any digits or commas. Enter a valid account name: ")
        }
    }
    return name
}

fun promptOneToN(max: Int): Int {
    var choice = 0
    while(!(choice in 1..max)) {
        try {
            choice = readln().trim().toInt()
            if(!(choice in 1..max)) {
                println("Invalid! Number must be between 1 and $max. ")
            }
        }
        catch (e: NumberFormatException) {
            println("Invalid! Enter a valid number.")
        }
    }
    return choice
}
/**
 * Helper Function promptFloat
 * Checks whether the input is a valid float and non-negative.
 * @return a valid float number.
 */
fun promptFloat() : Float {
    var floatAmt = -1F
    while(floatAmt < 0) {
        try {
            floatAmt = readln().trim().toFloat()
            if(floatAmt < 0)
                print("Invalid input! Enter a positive value: ")
        } catch (e: NumberFormatException) {
            print("Invalid! Enter a valid float: ")
        }
    }
    return floatAmt
}

/**
 * Helper Function promptYN
 * This function prompts the user to answer either yes or no.
 * @return a boolean value where choice Y is true and N is false.
 */
fun promptYN () : Boolean {
    var choice = 'a'
    var bool : Boolean
    while(choice == 'a') {
        try {
            choice = readln()[0]

            if(!(choice == 'y' || choice == 'n' || choice == 'Y' || choice == 'N'))
                throw IllegalArgumentException()
        }
        catch (e: IllegalArgumentException) {
            println("Invalid input!")
            choice = 'a'
        }
    }

    if (choice == 'Y' || choice == 'y')
        bool = true
    else 
        bool = false

    return bool
}

/**
 * Helper Function promptCurrency
 * This function prompts the user to enter existing acronym of a currency in this app.
 * @return the value of the currency in mapOfCurrencies.
 */
fun promptCurrency() : Int {
    val mapOfCurrencies = mutableMapOf("PHP" to 1, "USD" to 2, "JPY" to 3,
                                        "GBP" to 4, "EUR" to 5, "CNY" to 6)
    var value : Int = -1
    var isValidCurrency = false

    while(!isValidCurrency) {
        val currency = readln().trim().uppercase()
        value = mapOfCurrencies.getOrDefault(currency, -1)
        if(value != -1) {
            isValidCurrency = true
        }
        else {
            print("Currency input does not exist. Enter a valid currency (e.g PHP): ")
        }
    }

    return value
}

/**
 * This function returns the index of an account in the collection of BankAccount after
 * matching a given name.
 * @param name the given name to match account names in the collection of BankAccount
 * @param accounts an arraylist of BankAccount objects.
 * @return the account index of BankAccount
 */
fun accountIndex (name : String, accounts : ArrayList<BankAccount>) : Int {
    var idx = -1
    for(account in accounts.withIndex()) {
        val acc = account.value
        val accIndex = account.index
        if(acc.accountName.equals(name, ignoreCase = false))
            idx = accIndex
    }

    return idx
}

/**
 * This function deposits a peso-exchanged amount of chosen currency in the bank account of a user.
 * @param accounts an arraylist of BankAccount objects.
 * @param currExMap a map of currency keys and their exchange rate values.
 */
fun depositAmt(accounts: ArrayList<BankAccount>, currExMap : MutableMap<Int, Float>) {
    val fileName = "bank_transactions.csv"
    var boolLoop : Boolean

    do{
        print("\nDeposit Amount\n")
        val accountName = promptName()
        val idx = accountIndex(accountName, accounts)

        if(idx >= 0) {
            print(String.format("Current Balance: %.2f\n", accounts[idx].balance) +
                    "Currency (e.g PHP): "
            )
            val currIndex = promptCurrency()
            println()

            // if a currency exists within the program, execute this block of code
            if(currIndex >= 0)
            {
                //get the value (exchange rate) of the currency or default value of 1 for null safety.
                val fRate = currExMap.getOrDefault(currIndex, 1.0F)
                print("Deposit Amount: ")
                var depositAmount = promptFloat()
                depositAmount *= fRate
                accounts[idx].balance += depositAmount
                println(String.format("\nUpdated Balance = %.2f\n", accounts[idx].balance))

                saveTransactions(fileName, accounts[idx], "deposit", depositAmount)
            }
        }
        else {
            println("Account does not exist.")
        }
        print("Deposit again? (Y/N): ")
        boolLoop = promptYN()
    }while(boolLoop)
}

/**
 * This function withdraws a peso-exchanged amount of chosen currency in the bank account of a user.
 * @param accounts an arraylist of BankAccount objects.
 * @param currExMap a map of currency keys and their exchange rate values.
 */
fun withdrawAmt(accounts: ArrayList<BankAccount>, currExMap : MutableMap<Int, Float>) {
    val fileName = "bank_transactions.csv"
    var boolLoop : Boolean

    do{
        print("\nWithdraw Amount\n")
        val accountName = promptName()
        val idx = accountIndex(accountName, accounts)

        if(idx >= 0) {
            print(String.format("Current Balance: %.2f\n", accounts[idx].balance) +
                    "Currency (e.g PHP): "
            )
            val currIndex = promptCurrency()
            println()

            // if a currency exists within the program, execute this block of code
            if(currIndex >= 0)
            {
                //get the value (exchange rate) of the currency or default value of 1 for null safety.
                val fRate = currExMap.getOrDefault(currIndex, 1.0F)
                print("Withdraw Amount: ")
                var withdrawAmt = promptFloat()
                withdrawAmt *= fRate
                if(withdrawAmt <= accounts[idx].balance) {
                    accounts[idx].balance -= withdrawAmt
                    println(String.format("Updated Balance = %.2f\n", accounts[idx].balance))
                    saveTransactions(fileName, accounts[idx], "withdrawal", withdrawAmt)
                }
                else {
                    println("Transaction failed! Insufficient balance.")
                }
            }
        }
        else {
            println("Account does not exist.")
        }
        print("Withdraw again? (Y/N): ")
        boolLoop = promptYN()
    }while(boolLoop)
}


/**
 * This function records the exchange rate of a chosen foreign currency based on 1 Philippine Peso.
 * @param currExMap a map of currency keys and their exchange rate values.
 */
fun recordExRate(currExMap : MutableMap<Int, Float>) {
    var nOption: Int
    var fRate : Float
    print("\nRecord Exchange Rate\n" +
            "\n[1] Philippine Peso (PHP)\n" +
            "[2] United States Dollar (USD)\n" +
            "[3] Japanese Yen (JPY)\n" +
            "[4] British Pound Sterling (GBP)\n" +
            "[5] Euro (EUR)\n" +
            "[6] Chinese Yuan Renminbi (CNY)\n" +

            "\nSelect Foreign Currency: ")
    nOption = promptOneToN(6)

    if(nOption == 1){
        fRate = 1.0F
        println("PHP is the base currency! Defaulting to 1.00.")
    }

    else {
        print("Exchange Rate: ")
        do{
            fRate = promptFloat()
            if(fRate == 0F) {
                println("Exchange Rate must not be zero.")
            }
        }while(fRate == 0F)
    }

    currExMap[nOption] = fRate
    println("***\n+ CurrExMap[$nOption] = ${String.format("%.2f", fRate)}")
}

/**
 * This function displays the amount from a source foreign currency amount to an exchanged currency
 * @param currExMap a map of currency keys and their exchange rate values.
 */
fun curExchange(currExMap: MutableMap<Int, Float>) {
    var srcAmt : Float
    var srcOption: Int
    var exOption: Int
    var boolLoop : Boolean
    do {
        print("\nForeign Currency Exchange\n" +
                "Source Currency Option:\n" +
                "[1] Philippine Peso (PHP)\n" +
                "[2] United States Dollar (USD)\n" +
                "[3] Japanese Yen (JPY)\n" +
                "[4] British Pound Sterling (GBP)\n" +
                "[5] Euro (EUR)\n" +
                "[6] Chinese Yuan Renminbi (CNY)\n")

        print("Source Currency: ")
        srcOption = promptOneToN(6)
        print("Source Amount: ")
        srcAmt = promptFloat()

        print("\nExchanged Currency Options:\n" +
                "[1] Philippine Peso (PHP)\n" +
                "[2] United States Dollar (USD)\n" +
                "[3] Japanese Yen (JPY)\n" +
                "[4] British Pound Sterling (GBP)\n" +
                "[5] Euro (EUR)\n" +
                "[6] Chinese Yuan Renminbi (CNY)\n")

        print("Exchange Currency: ")
        exOption = promptOneToN(6)
        //Default to zero for null safety
        val srcRate = currExMap.getOrDefault(srcOption, defaultValue = 0.0F)
        val exRate = currExMap.getOrDefault(exOption, defaultValue = 0.0F)
        val exAmt = srcAmt * (srcRate / exRate)
        if(srcRate == 0.0F || exRate == 0.0F) {
            println("No valid rate available.")
        }
        else {
            println(String.format("Exchange Amount: %.2f\n", exAmt))
        }

        println("\nConvert another currency (Y/N)?")
        boolLoop = promptYN()
    }while(boolLoop)
}

/**
 * This functions displays the user daily interest based on the number of days inputted.
 * @param days The number of expected days
 * @param balance The current balance of the user's bank account
 * @param annualInterest The interest of the user's current bank account.
 */
fun displayListOfDays(days : Int, balance : Float, annualInterest : Float) {
    println(String.format("%-3s | %-8s | %-9s |", "Day", "Interest", "Balance"))
    var startingBalance = balance
    var dailyInterest : Float

    for (i in 1..days) {
        dailyInterest = startingBalance * (annualInterest / 365.0F)
        startingBalance += String.format("%.2f", dailyInterest).toFloat()
        println(String.format("%-3d | %-8.2f | %-9.2f |", i, dailyInterest, startingBalance))
    }
}

/**
 * This function facilitates the inputs for the flow of showing the daily interest of a user's bank account
 * @param accounts an arraylist of BankAccount objects.
 * @param annualInterest The interest of the user's current bank account.
 */
fun showIntAmt (accounts : ArrayList<BankAccount>, annualInterest : Float) {
    var name : String
    var index : Int
    var days : Int = -1

    println("Show Interest Amount")
    name = promptName()
    index = accountIndex(name, accounts)

    if(index >= 0) {
        print("Current Balance: ")
        print(String.format("%.2f", accounts[index].balance))
        print("\nCurrency: " + accounts[index].currency)
        println()
        print("\nTotal Number of Days: ")

        while (days < 0) {
            try {
                days = readln().trim().toInt()
                if (days < 0)
                    println("Invalid Number of Days!")
            } catch (e: NumberFormatException) {
                println("Invalid Input!")
                days = -1
            }
        }
        println()

        displayListOfDays(days, accounts[index].balance, annualInterest)
    }
    else {
        println("Account does not exist.")
    }

}

fun main() {
    var choice : Int
    var returnToMenu : Boolean
    val bankAccounts = ArrayList<BankAccount>()
    val currExMap = mutableMapOf<Int, Float>()
    val annualInterest = 0.05F

    val fileName = "bank_transactions.csv"
    val cleanedRecord = ArrayList<TransactionRecord>()

    if(File(fileName).exists()) {
        loadCleanCsv(fileName, cleanedRecord)
        syncAccounts(cleanedRecord, bankAccounts)
    }
    //default rate of base currency is always 1
    currExMap[1] = 1.0F
    //USD
    currExMap[2] = 62.0F
    //JPY
    currExMap[3] = 0.40F
    //GBP
    currExMap[4] = 84.0F
    //EUR
    currExMap[5] = 72.0F
    //CNY
    currExMap[6] = 9.0F

    do {
        choice = mainMenu()

        when(choice) {
            1 -> registerAccount(accounts = bankAccounts)
            2 -> depositAmt(accounts = bankAccounts, currExMap)
            3 -> withdrawAmt(accounts = bankAccounts, currExMap)
            4 -> curExchange(currExMap)
            5 -> recordExRate(currExMap)
            6 -> showIntAmt (accounts = bankAccounts, annualInterest)
            7 -> genReportsMenu(fileName, cleanedRecord, bankAccounts)
        }

        print("Back to Main Menu (Y/N): ")
        returnToMenu = promptYN()
    } while (returnToMenu)
}