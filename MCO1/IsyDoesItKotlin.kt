
/********************
Last names: Ilagan, Serios, Yardan
Language: Kotlin
Paradigm(s): Procedural
********************/


/**
 * This class used to create bank account objects for encapsulation of data.
 * @param accountName String name of the bank user
 * @param balance Amount of money in a bank account
 * @param currency Name of the currency, PHP as the base currency.
 */
class BankAccount (val accountName : String, var balance : Float, val currency : String)

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
                "Choice:"
    )

    choice = promptOption()
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

            for(i in 0 until name.length)
                if(name[i].isDigit()) throw IllegalArgumentException()

        }catch (e : IllegalArgumentException){
            name = ""
            println("Enter a valid account name: ")
        }
    }
    return name
}

/**
 * Helper Function promptOption
 * This checks if a choice is a number and between 1 and 6
 * @return a valid choice which ranges from 1 to 6.
 */
fun promptOption(): Int {
    var choice = 0
    while(!(choice in 1..6)) {
        try {
            choice = readln().trim().toInt()
            if(!(choice in 1..6)) {
                println("Invalid! Number must be between 1 and 6. ")
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
                print("Invalid input!\n")
        } catch (e: NumberFormatException) {
            println("Invalid! Enter a valid float.\n")
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
            println("Currency input does not exist.")
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
 * This function creates an account by prompting a valid name, defaulting the balance to 1000
 * and defaulting the currency to the base currency, PHP.
 * @param accounts an arraylist of BankAccount objects.
 */
fun registerAccount (accounts : ArrayList<BankAccount>) {
    var accountName: String
    print("\nRegister Account Name\n")
    accountName = promptName()
    val idx = accountIndex(accountName, accounts)

    if(idx == -1) {
        accounts.add(BankAccount(accountName, 1000.0F, "PHP"))
    }
    else {
        println("This account already exists.")
    }
}

/**
 * This function deposits a peso-exchanged amount of chosen currency in the bank account of a user.
 * @param accounts an arraylist of BankAccount objects.
 * @param currExMap a map of currency keys and their exchange rate values.
 */
fun depositAmt(accounts: ArrayList<BankAccount>, currExMap : MutableMap<Int, Float>) {
    var accountName: String
    var depositAmount: Float
    var currIndex : Int
    var fRate : Float

    print("\nDeposit Amount\n")
    accountName = promptName()

    val idx = accountIndex(accountName, accounts)

    //if a username of bank account exist within the program, execute this block of code.
    if(idx >= 0) {
        print(String.format("Current Balance: %.2f\n", accounts[idx].balance) +
                "Currency (e.g PHP): "
        )
        currIndex = promptCurrency()
        println()

        // if a currency exists within the program, execute this block of code
        if(currIndex >= 0)
        {
            //get the value (exchange rate) of the currency or default value of 1 for null safety.
            fRate = currExMap.getOrDefault(currIndex, 1.0F)
            print("Deposit Amount: ")
            depositAmount = promptFloat()
            depositAmount *= fRate
            accounts[idx].balance += depositAmount
            println(String.format("Updated Balance = %.2f\n", accounts[idx].balance))
        }
    }
    else {
        println("Account does not exist.")
    }
}

/**
 * This function withdraws a peso-exchanged amount of chosen currency in the bank account of a user.
 * @param accounts an arraylist of BankAccount objects.
 * @param currExMap a map of currency keys and their exchange rate values.
 */
fun withdrawAmt(accounts: ArrayList<BankAccount>, currExMap : MutableMap<Int, Float>) {
    var accountName: String
    var withdrawAmount: Float
    var currIndex : Int
    var fRate : Float

    print("\nWithdraw Amount\n")
    accountName = promptName()

    val idx = accountIndex(accountName, accounts)

    if (idx >= 0) {
        print(String.format("Current Balance: %.2f\n", accounts[idx].balance) +
                "Currency (e.g PHP): "
        )

        currIndex = promptCurrency()
        println()

        if(currIndex >= 0)
        {
            fRate = currExMap.getOrDefault(currIndex, 1.0F)
            print("Withdraw Amount: ")
            withdrawAmount = promptFloat()
            withdrawAmount *= fRate
            accounts[idx].balance -= withdrawAmount
            println(String.format("Updated Balance = %.2f\n", accounts[idx].balance))
        }
    }
    else {
        println("Account does not exist.")
    }
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
    nOption = promptOption()

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
        srcOption = promptOption()
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
        exOption = promptOption()
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
        }

        println("Back to Main Menu (Y/N): ")
        returnToMenu = promptYN()
    } while (returnToMenu)
}