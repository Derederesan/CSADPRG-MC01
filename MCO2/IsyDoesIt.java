/********************
Last names: Ilagan, Serios, Yardan
Language: Java
Paradigm(s): Object Oriented Programming 
********************/
import java.util.*;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.time.LocalDate;
import java.time.LocalTime;
import java.time.format.DateTimeFormatter;
import java.util.function.Function;

public class IsyDoesIt
{
    private static final Scanner scanner = new Scanner(System.in);
    private static final DateTimeFormatter DATE_FORMAT= DateTimeFormatter.ofPattern("dd/MM/yyyy"); 
    private static final DateTimeFormatter TIME_FORMAT = DateTimeFormatter.ofPattern("H:mm:ss");
    //defined data model for common transactions  
    public record Transaction(
        String accountName,
        String accountNumber,
        LocalDate date,
        LocalTime time,
        String currency,
        double amount,
        String type,
        double currentBalance
    ) {}

    //defined data model for report 1 
    public record PrivilegeReport(
        String accountName, 
        long accountAge,
        double currentBalance, 
        double averageDaily, 
        String privilegeTier 
    ){}
    //defined data model for report 2 
    public record InterestReport(
        String accountName, 
        double currentBalance, 
        double expectedInterest
    ){}
    //defined data model for report 3 
    public record SuspiciousReport(
        String accountName, 
        LocalDate date, 
        double totalWithdraw, 
        int numDeposit, 
        double totalDeposit, 
        double turnoverVelocity, 
        String riskFlag
    ){}
    //defined data model for report(Low activity account report) 
    public record CustomReport(
        String accountNumber, 
        String accountName, 
        LocalDate lastTransactionDate, 
        int transactionCount, 
        double currentBalance, 
        String activityStatus
    ){}
    /*
    * Helper function  
    * @Purpose: to print previews for generate report 
    */
    public static <T> void printPreviewTable(String title, String csvFileName, String header, List<T> rows, Function<T, String> rowFormatter) {
        System.out.println("\n" + title);
        System.out.println(header);

        int limit = Math.min(2, rows.size());
        for (int i = 0; i < limit; i++) {
            System.out.println(rowFormatter.apply(rows.get(i)));
        }

        System.out.println("\n(Full table exported to " + csvFileName + ")");
    }
    /*
    * Helper function  
    * @Purpose: to read csv file
    * @ return: returns the transactions 
    */
   public static List<Transaction> loadTransactions(String filePath) throws IOException {
        List<Transaction> transactions = new ArrayList<>();
        List<String> lines = Files.readAllLines(Path.of(filePath));

        for (int i = 1; i < lines.size(); i++) {
            String line = lines.get(i).trim();
            if (line.isEmpty()) continue;

            String[] cols = line.split(",");
            transactions.add(new Transaction(
                cols[0],
                cols[1],
                LocalDate.parse(cols[2], DATE_FORMAT),
                LocalTime.parse(cols[3], TIME_FORMAT),
                cols[4],
                Double.parseDouble(cols[5]),
                cols[6],
                Double.parseDouble(cols[7])
            ));
        }
        return transactions;
    }

    /*
    * Helper function 
    * @Purpose: to generate the next account number 
    * @Return: the account number  
    */
    public static String generateNextAccountNumber(List<Transaction> transactions) 
    {
        // Fallback starting number if the CSV is completely empty
        long maxAccountNum = 1092000000L; 

        for (Transaction tx : transactions) {
            long currentNum = Long.parseLong(tx.accountNumber());
            if (currentNum > maxAccountNum) {
                maxAccountNum = currentNum;
            }
        }

        return String.valueOf(maxAccountNum + 1);
    } 

    /*
    * Helper function 
    * @Purpose: writes the Transaction into the CSV 
    */
    public static void appendTransaction(String filePath, Transaction tx) throws IOException {
        String csvLine = String.join(",",
            tx.accountName(),
            tx.accountNumber(),
            tx.date().format(DATE_FORMAT),
            tx.time().format(TIME_FORMAT),
            tx.currency(),
            String.format("%.2f", tx.amount()),
            tx.type(),
            String.format("%.2f", tx.currentBalance())
        );

        Files.writeString(
            Path.of(filePath),
            System.lineSeparator() + csvLine,
            StandardOpenOption.CREATE,
            StandardOpenOption.APPEND
        );
    }

    /*
    * Helper function 
    * @ Purpose: checks if provided choice is valid (number from 0-6)
    * @ return: returns the user's valid input
    */
    public static int valid_choice(String prompt)
    {
        while(true)
        {
            try
            {
                System.out.print(prompt); 
                int input = scanner.nextInt(); 
                scanner.nextLine();
                // Check if the input is within the valid range
                if (input >= 0 && input <= 7) 
                {
                    return input; // Valid input, exit the loop and return
                } else {
                    System.out.println("Invalid choice! Please enter a number between 0 and 6.");
                }

            }
            catch(InputMismatchException e)
            {
                System.out.println("Invalid choice! Please enter a number between 0 and 6."); 
                scanner.next();
            }
        }
    }  


    /*
    * Helper function 
    * @ Purpose: to check if user inputted a valid name 
    * checks each character of the input
    * returns false if an integer is detected 
    */
    public static boolean name(String name)
    {
       if (name == null || name.trim().isEmpty()) 
        {
            return false;
        }
        
        // Check every character to see if it's a number
        for (int i = 0; i < name.length(); i++) 
        {
            if (Character.isDigit(name.charAt(i))) 
            {
                return false; 
            }
        }
        
        return true;
    }

    /*
    * Helper function to check if user inputted a valid choice (Y or N) 
    * returns false if none of the valid choices are detected  
    */
    public static boolean choice(String input)
    {
        if ( input== null || input.length() != 1) 
        {
            return false;
        }
        char c = input.charAt(0);
        return c == 'Y' || c == 'y' || c == 'N' || c == 'n';
    }

    /*
    * Helper function to check if user inputted a valid amount of money 
    * @return: double containing user's valid input 
    */
    public static double money(String prompt)
    {
        while(true)
        {
            try
            {
                System.out.print(prompt); 
                String input = scanner.nextLine();
                Double amount = Double.parseDouble(input); 
                // Check if the input is within the valid range
                if (amount>=0) 
                {
                    return amount; // Valid input, exit the loop and return
                } else {
                    System.out.println("Please enter an amount greater than 0.");
                }

            }
            catch(NumberFormatException e)
            {
                System.out.println("Please enter a valid amount!(double only)"); 
            }
        }  
    }

    /*
    * Helper function to convert into different currency 
    * @return: double containing converted amount
    */
    public static double convert(double amount, int from, int to, double rate[])
    {
        double base = amount*rate[from];
        return base / rate[to];
    }

    /* Helper function 
    * @Purpose: to find account name based on account number
    * @return: account name
     */
    public static String findAccountName(List<Transaction> transactions, String accountNumber)
    {
        for(Transaction tx: transactions){
            if(tx. accountNumber().equals(accountNumber.trim()))
                return tx.accountName(); 
        }
                return "";
    }
    /*
    * Helper function 
    * @Purpose: to filter transactions in Philippine Currency  
    * @return: filtered transactions 
    */
   
   public static List<Transaction> filterTransactions(List<Transaction> transactions)
    {
        List<Transaction> current = new ArrayList<>(); 

        for (Transaction tx: transactions){
            if("PHP".equalsIgnoreCase(tx.currency))
            {
                current.add(tx); 
            }
        }

        return current; 
    }
    

    /*
    * Helper function
    * @Purpose: log total row count 
    * @return: the total row count
    */
    public static int totalRow(List<Transaction> transactions)
    {
       return transactions.size();
    }
    /* 
    * Helper function 
    * @ Purpose: filters transactions to remove incomplete rows  
    */

    public static void filterIncomplete(List<Transaction> transactions){

         transactions.removeIf(tx->tx.accountName() == null || tx.accountName().isBlank() ||
                tx.accountNumber() == null || tx.accountNumber().isBlank() ||
                tx.date() == null ||
                tx.time() == null ||
                tx.currency() == null || tx.currency().isBlank() ||
                tx.type() == null || tx.type().isBlank());
    }
    /*
    * Helper function
    * @Purpose: filters transactions>=100k 
    * @return: filtered transactions 
    */
    public static List<PrivilegeReport> filterPrivilege(List<PrivilegeReport> reportRows){
        List<PrivilegeReport> privilege = new ArrayList<>(); 
       for (PrivilegeReport row : reportRows) {
            if (row.averageDaily() >= 100000.00) {
                privilege.add(row);
            }
        }

        return privilege; 
    }
    
     /*
    * Helper function
    * @Purpose: filters current balance
    * @return: filtered transactions 
    */
    public static List<InterestReport> filterBalance(List<InterestReport> reportRows){
        List<InterestReport> balance = new ArrayList<>(); 
        for (InterestReport row : reportRows) {
            if (row.currentBalance() >= 10000.00) {
                balance.add(row);
            }
        }

        return balance; 
    }
    
    /*
    * Helper function
    * @Purpose: gets end of day balance
    * @return: returns end of day balance 
    */
    public static double getEodBalance(List<Transaction> accountTxs, LocalDate targetDate) {
        double eodBalance = 0.00;

        for (Transaction tx : accountTxs) {
            // If tx.date() is BEFORE or ON targetDate
            if (!tx.date().isAfter(targetDate)) {
                eodBalance = tx.currentBalance();
            }
    }

    return eodBalance;
}

    /*
    * Helper function
    * @Purpose: calculates average daily balance 
    * @return: average daily balance 
    */
    public static double calculateADB(List<Transaction> accountTxs)
    {
        LocalDate lastMonth = LocalDate.now().minusMonths(1);
        int year = lastMonth.getYear();
        int month = lastMonth.getMonthValue();
        int totalDays = lastMonth.lengthOfMonth();

        double sum = 0.00; 

        for (int day = 1; day <= totalDays; day++) {
        LocalDate currentDate = LocalDate.of(year, month, day);
        sum += getEodBalance(accountTxs, currentDate);
        }

        return sum / totalDays;
    }


    /*
    * Helper function
    * @Purpose: calculates account age 
    * @return: account age
    */
    public static int calculateAccountAge(LocalDate openedDate)
    {
        LocalDate today = LocalDate.now();
        int days = 0;
        LocalDate current = openedDate;

        while (current.isBefore(today)) {
            days++;
            current = current.plusDays(1);
        }

        return days;
    }

    /*
    * Helper function
    * @Purpose: gets unique account numbers
    * @return: unique accounts 
    */
   public static List<String> getUniqueAccounts(List<Transaction> transactions) {
        List<String> uniqueAccounts = new ArrayList<>();

        for (Transaction tx : transactions) {
            if (!uniqueAccounts.contains(tx.accountNumber())) {
                uniqueAccounts.add(tx.accountNumber());
            }
        }
        return uniqueAccounts;
    }

    /*
    * Helper function
    * @Purpose: gets transaction belonging to a single account
    * @return: grabs rows belonging to a single number 
    */
    public static List<Transaction> getTransactionsForAccount(List<Transaction> transactions, String accNum) {
        List<Transaction> accountTxs = new ArrayList<>();

        for (Transaction tx : transactions) {
            if (tx.accountNumber().equals(accNum)) {
                accountTxs.add(tx);
            }
        }
        return accountTxs;
    }

    /*
    * Helper function
    * @Purpose: gets tier based on calculated ADB 
    * @return: account's tier 
    */
    public static String privilegeTier(double calculatedADB)
    {
        if (calculatedADB < 500000.00) {
            return "Silver";
        } else if (calculatedADB < 1000000.00) {
            return "Gold";
        } else {
            return "Platinum";
        }
    }

    /*
    * Helper function
    * @Purpose: gets tier based on calculated ADB 
    * @return: account's tier 
    */
    public static double calculateExpectedInterest(double currentBalance)
    {
        double totalInterest = 0.0;
        double annualRate = 0.05;

        for (int day = 1; day <= 30; day++) {
            double dailyInterest = currentBalance * (annualRate / 365.0);
            totalInterest += dailyInterest;
        }

        return totalInterest;
    }

    /*
    * Helper function
    * @Purpose: gets totalwithdrawals
    * @return: totalwithdrawal 
    */
    public static double calculateTotalWithdrawals(List<Transaction> accountTxs, LocalDate date) {
        double total = 0.0;
        for (Transaction tx : accountTxs) {
            if (tx.date().equals(date) && "WITHDRAWAL".equalsIgnoreCase(tx.type())) {
                total += tx.amount();
            }
        }
        return total;
    }

    /*
    * Helper function
    * @Purpose: gets totaldeposit
    * @return: totaldeposit
    */
    public static int countDeposits(List<Transaction> accountTxs, LocalDate date) {
        int count = 0;
        for (Transaction tx : accountTxs) {
            if (tx.date().equals(date) && "DEPOSIT".equalsIgnoreCase(tx.type())) {
                count++;
            }
        }
        return count;
    }
    /*
    * Helper function
    * @Purpose: gets numdeposits 
    * @return: numdeposits
    */
    public static double calculateTotalDeposits(List<Transaction> accountTxs, LocalDate date) {
        double total = 0.0;
        for (Transaction tx : accountTxs) {
            if (tx.date().equals(date) && "DEPOSIT".equalsIgnoreCase(tx.type())) {
                total += tx.amount();
            }
        }
        return total;
    }
    /*
    * Helper function
    * @Purpose: calculateTurnoverVelocity 
    * @return: Turnovervelocity
    */
    public static double calculateTurnoverVelocity(double totalDeposit, double totalwithdrawal)
    {
        if (totalDeposit == 0.0) {
            return 0.0;
        }
        return totalwithdrawal/totalDeposit *100;
    }
     /*
    * Helper function
    * @Purpose: check for risks 
    * @return: riskflag label 
    */
    public static String riskFlag (double totalwithdrawal, double turnoverVelocity)
    {
        if(totalwithdrawal>=500000.00 || turnoverVelocity>=90.0)
        {
            return "Suspicious";
        }
        return "Normal";
    }
     /*
    * Helper function
    * @Purpose: determines the activity status of the account  
    * @return: activity status 
    */
    public static String determineActivityStatus(int transactionCount) {
        if (transactionCount <= 2) {
            return "Inactive";
        } else if (transactionCount <= 5) {
            return "Moderate";
        } else {
            return "Active";
        }
    }

    /*
    * Helper function
    * @Purpose: sorts by descending order
    */

    public static <T, U extends Comparable<U>> void sortDescending(List<T> list, Function<T, U> keyExtractor) {
        for (int i = 0; i < list.size() - 1; i++) {
            for (int j = 0; j < list.size() - 1 - i; j++) {
                U current = keyExtractor.apply(list.get(j));
                U next = keyExtractor.apply(list.get(j + 1));

                if (current.compareTo(next) < 0) {
                    T temp = list.get(j);
                    list.set(j, list.get(j + 1));
                    list.set(j + 1, temp);
                }
            }
        }
    }

    /*
    * Helper function
    * @Purpose: sorts by ascending order
    */
    public static <T, U extends Comparable<U>> void sortAscending(List<T> list, Function<T, U> keyExtractor) {
        for (int i = 0; i < list.size() - 1; i++) {
            for (int j = 0; j < list.size() - 1 - i; j++) {
                U current = keyExtractor.apply(list.get(j));
                U next = keyExtractor.apply(list.get(j + 1));

              
                if (current.compareTo(next) > 0) {
                    T temp = list.get(j);
                    list.set(j, list.get(j + 1));
                    list.set(j + 1, temp);
                }
            }
        }
    }

    /*
    * Helper function
    * @Purpose: writes the csv report  
    */
    public static <T> void writeReportToCsv(
            List<T> reportRows,
            String filePath,
            String reportTitle,
            String header,
            Function<T, String> rowFormatter) throws IOException {

        // 1. Write all rows to the CSV file
        List<String> lines = new ArrayList<>();
        lines.add(header);
        for (T row : reportRows) {
            lines.add(rowFormatter.apply(row));
        }
        Files.write(Path.of(filePath), lines);

        // 2. Print the 2-row console preview matching the specification
        System.out.println("\n" + reportTitle);
        System.out.println(header);

        int limit = Math.min(2, reportRows.size());
        for (int i = 0; i < limit; i++) {
            System.out.println(rowFormatter.apply(reportRows.get(i)));
        }

        System.out.println("\n(Full table exported to " + filePath + ")");
    }
    
    /*
    * @ Purpose: displays the main menu
    * @ return int containing the chosen menu 
    */
    public static int main_menu()
    {
            System.out.println("Select Transaction: ");
            System.out.println("[1] Register Account Name"); 
            System.out.println("[2] Deposit Amount"); 
            System.out.println("[3] Withdraw Amount"); 
            System.out.println("[4] Currency Exchange"); 
            System.out.println("[5] Record Exchange Rate"); 
            System.out.println("[6] Show Interest Amount"); 
            System.out.println("[7] Generate Reports"); 
            System.out.println("[0] Exit Transaction");

            return valid_choice("Choice: ");
    }
    
    /*
    * @ Purpose: Registers the user's name [REQ-0009]
    * @ return String containing the user's name 
    */
    public static Transaction register_name(String filePath, List<Transaction> transactions) throws IOException
    {
        boolean isValid = false; 
        String input;
        double inDeposit = 0.0;

        System.out.println("Register Account");
        
        do
        {
            System.out.print("Account Name: ");
            input = scanner.nextLine().trim();
            if(name(input))
            {
                isValid = true;
            }
            else
                System.out.println("Please enter a valid name!");
        } while(!isValid);
        isValid = false; 
        do
        {
            inDeposit= money("Initial Deposit: ");
            if(inDeposit>=5000)
            {
                isValid = true; 
            }
            else
            {
                System.out.println("Initial Deposit must be 5,000 or more!");
            }
        }while(!isValid);

        // Automatically generate the next unique account number
        String newAccountNum = generateNextAccountNumber(transactions);
        System.out.println("Account created successfully!");

        Transaction newTx = new Transaction(
        input,
        newAccountNum,
        LocalDate.now(),
        LocalTime.now(),
        "PHP",
        inDeposit,
        "deposit",
        inDeposit
        );

        // Save to CSV and update the in-memory list right here
        appendTransaction(filePath, newTx);
        transactions.add(newTx);

        return newTx;

    }
    /*
    * @ Purpose: Deposits money to the user's bank account[REQ-0004]
    * @ Provision to set Philippine Peso(PHP) as Base Currency [REQ-0002]
    * @ return double containing the user's new balance 
    */
    public static double deposit_amount(double balance, String accountNumber,  double rate [], String filePath, List <Transaction> transactions) throws IOException
    { 
        double deposit = 0.0;
        String accountNum = ""; 
        String name = "";
        String currency = "";
        boolean isValid = false; 

        System.out.println("Deposit Amount");
        System.out.print("Enter Account number: "); 
        accountNum = scanner.nextLine().trim();
        name = findAccountName(transactions, accountNum);
        if(!name.isEmpty())
        {
            System.out.println("Welcome back" + name + "!");
            isValid = true; 
        }
        else
        {
            System.out.println("Account not found! Please register!(Option 1)"); 
            return 0.00; 
        }
        System.out.println("Account Name: " + name);
        System.out.printf("\nCurrent Balance: %.2f\n", balance);

        while(!isValid)
        {
            System.out.printf("\nCurrency (PHP/USD/JPY/GBP/EUR/CNY): "); 
            currency = scanner.nextLine().toUpperCase();
            if(currency.equals("PHP")||
               currency.equals("USD")||
               currency.equals("JPY")||
               currency.equals("GBP")||
               currency.equals("EUR")||
               currency.equals("CNY"))
                    isValid=true;
        } 

        deposit = money("\nDeposit Amount: ");
        
        switch(currency)
        {
                case "PHP":
                    deposit = convert(deposit,0,0,rate);
                    break; 
                case "USD":
                    deposit = convert (deposit,1,0,rate);
                    break;
                case "JPY":
                    deposit = convert(deposit,2,0,rate );
                    break; 
                case "GBP":
                    deposit = convert(deposit, 3,0,rate);
                    break; 
                case "EUR":
                    deposit = convert(deposit, 4,0,rate);
                    break; 
                case "CNY":
                    deposit = convert(deposit, 5,0,rate);
                    break; 
        }

        balance += deposit;
        System.out.printf("\nDeposit Amount: %.2f", deposit); 
        System.out.printf("\nUpdated Balance: %.2f", balance);

        Transaction newTx = new Transaction(
        name,
        accountNumber,
        LocalDate.now(),
        LocalTime.now(),
        "PHP",
        deposit,
        "deposit",
        deposit
        );

        // Save to CSV and update the in-memory list right here
        appendTransaction(filePath, newTx);
        transactions.add(newTx);

        return balance; 
    }
    /*
    * @ Purpose: Withdraws money from the user's bank account [REQ-0005]
    * @ Provision to set Philippine Peso(PHP) as Base Currency [REQ-0002]
    * @ return double containing the user's new balance 
    */
    public static double withdraw_amount(double balance, String accountNumber,  double rate [], String filePath, List <Transaction> transactions) throws IOException
    {
        String currency = ""; 
        String accountNum = ""; 
        String name = "";
        double withdraw = 0.0;
        boolean isValid = false;

        System.out.println("Withdraw Amount"); 
        System.out.print("Enter Account number: "); 
        accountNum = scanner.nextLine().trim();
        name = findAccountName(transactions, accountNum);
        if(!name.isEmpty())
        {
            System.out.println("Welcome back" + name + "!");
            isValid = true; 
        }
        else
        {
            System.out.println("Account not found! Please register!(Option 1)"); 
            return 0.00; 
        }
        System.out.println("Account Name: "+ name);
        System.out.printf("\nCurrent Balance: %.2f", balance);
        isValid = false; 
        while(!isValid)
        {
            System.out.printf("\nCurrency (PHP/USD/JPY/GBP/EUR/CNY): "); 
            currency = scanner.nextLine().toUpperCase();
            if(currency.equals("PHP")||
               currency.equals("USD")||
               currency.equals("JPY")||
               currency.equals("GBP")||
               currency.equals("EUR")||
               currency.equals("CNY"))
                    isValid=true;
        }
        
        withdraw = money("\nWithdraw Amount: ");
        switch(currency)
        {
                case "PHP":
                    withdraw = convert(withdraw,0,0,rate);
                    break; 
                case "USD":
                    withdraw = convert (withdraw,1,0,rate);
                    break;
                case "JPY":
                    withdraw = convert(withdraw,2,0,rate );
                    break; 
                case "GBP":
                    withdraw = convert(withdraw, 3,0,rate);
                    break; 
                case "EUR":
                    withdraw = convert(withdraw, 4,0,rate);
                    break; 
                case "CNY":
                    withdraw = convert(withdraw, 5,0,rate);
                    break; 
        }

        while(withdraw>balance)
        {
            System.out.println("Amount exceeds balance!");
            withdraw = money("\nWithdraw Amount (Enter 0 to cancel): ");

            if (withdraw==0)
            {
                System.out.println("Exiting..");
                return balance;
            }

            switch(currency)
            {
                    case "PHP":
                        withdraw = convert(withdraw,0,0,rate);
                        break; 
                    case "USD":
                        withdraw = convert (withdraw,1,0,rate);
                        break;
                    case "JPY":
                        withdraw = convert(withdraw,2,0,rate );
                        break; 
                    case "GBP":
                        withdraw = convert(withdraw, 3,0,rate);
                        break; 
                    case "EUR":
                        withdraw = convert(withdraw, 4,0,rate);
                        break; 
                    case "CNY":
                        withdraw = convert(withdraw, 5,0,rate);
                        break; 
            }

        }
        

        balance -= withdraw;
        System.out.printf("\nWithdraw Amount: %.2f", withdraw); 
        System.out.printf("\nUpdated Balance: %.2f", balance);
        return balance; 
    }

    /*
    * @Purpose: to display the exchange rate menu 
    */
    public static void exchange_menu()
    {
        System.out.println("[1] Philipine Peso (PHP)"); 
        System.out.println("[2] United States Dollar (USD)"); 
        System.out.println("[3] Japanese Yen (JPY)"); 
        System.out.println("[4] British Pound Sterling (GBP)"); 
        System.out.println("[5] Euro (EUR)"); 
        System.out.println("[6] Chinese Yuan Renminni (CNY)");
    }
    /*
    * @ Purpose: records the current rate exchange for the given foreign currencies [REQ-001]
    * @ return the inputted exchange rate 
    */
    public static double exchange_rate(double rate[])
    {
        int input=0;
        double foreign=0.0; 
        
        System.out.println("Record Exchange Rate ");
        exchange_menu();
        input=valid_choice("\nSelect Foreign Currency:");

        foreign = money("\nExchange Rate: ");

        System.out.printf("\nExchange rate updated successfully to %.2f!\n", foreign);

        switch(input)
        {
            case 1:
                return rate[0] = foreign; 
            case 2:
                return rate[1] = foreign; 
            case 3:
                return rate [2] = foreign; 
            case 4:
                return rate [3] = foreign; 
            case 5:
                return rate [4] = foreign; 
            case 6:
                return rate [5] = foreign;
        }

        return foreign; 
    }
     /*
    * @ Purpose: to convert amount from one currency to another currency [REQ-0003]
    */
    public static void currency_exchange(double rate[])
    {
        int s_currency=0, e_currency=0; 
        double source= 0.0, convert = 0.0; 

            System.out.println("Foreign Currency Exchange");
            System.out.println("Source Currency Option: ");
            exchange_menu();

            s_currency = valid_choice("\nSource Currency: ");
                
            source = money("\nSource Amount: "); 

            System.out.println("Exchanged Currency Options: ");
            exchange_menu();

            e_currency = valid_choice("\nExchange Currency: ");

            convert=convert(source, s_currency-1,e_currency-1,rate);
              
            System.out.printf("Exchange Amount: %.2f", convert);
        
    }
   
    public static void interest_amount(String name, double balance)
    {
        boolean isValid=false; 
        int days=0, input=0;

        System.out.println("Show Interest Amount");
        System.out.println("Account Name: " + name);
        System.out.printf("\nCurrent Balance: %.2f\n", balance);
        System.out.println("Currency: PHP");
        System.out.println("Interest Rate: 5%");

        do
        {
            System.out.printf("\nTotal Number of Days: ");
            input = scanner.nextInt();
            try
            {
                if(input>0)
                {
                    isValid = true;
                }
                else
                {
                    System.out.println("Days must be greater than 0!");
                }
            }
            catch (NumberFormatException e)
            {
                System.out.println("Please enter a valid integer for days!");
            }
        }while(!isValid);

        days = input; 
        System.out.println("\nTotal Number of Days: " + days + " days\n");
        System.out.println("Day | Interest | Balance |");
        
        double currentBalance = balance;
        double annualRate = 0.05; 

        
        for (int i = 1; i <= days; i++)
        {
            double dailyInterest = currentBalance * (annualRate / 365.0);
            currentBalance += dailyInterest;
            
            System.out.printf("%-3d | %-8.2f | %-7.2f |\n", i, dailyInterest, currentBalance);
        }
    }
    /*
    * @Purpose: to generate report 1 [REQ-0006]
    * @return: sorted CSV 
    */
    public static void generatePrivilegeReport(List<Transaction> transactions) throws IOException 
    {
        filterIncomplete(transactions);
        List<Transaction> phpTxs = filterTransactions(transactions);

        List<String> uniqueAccounts = getUniqueAccounts(phpTxs);
        List<PrivilegeReport> reportRows = new ArrayList<>();

        for (String accNum : uniqueAccounts) {
            List<Transaction> accountTxs = getTransactionsForAccount(phpTxs, accNum);
            double adb = calculateADB(accountTxs);

            Transaction firstTx = accountTxs.get(0);
            Transaction latestTx = accountTxs.get(accountTxs.size() - 1);

            int age = calculateAccountAge(firstTx.date());
            String tier = privilegeTier(adb);

            reportRows.add(new PrivilegeReport(
                latestTx.accountName(),
                age,
                latestTx.currentBalance(),
                adb,
                tier
            ));
        }

        List<PrivilegeReport> filteredRows = filterPrivilege(reportRows);

        sortDescending(filteredRows, PrivilegeReport::averageDaily);

        writeReportToCsv(
            filteredRows,
            "privilege_report.csv",
            "Report 1: Banking Privilege Tiers",
            "AccountName    AccountAge     CurrentBalance     AverageDailyBalance    PrivilegeTier",
            row -> String.format("%s,%d,%.2f,%.2f,%s",
                row.accountName(),
                row.accountAge(),
                row.currentBalance(),
                row.averageDaily(),
                row.privilegeTier()
            )
        );
    }

    /*
    * @Purpose: to generate report 2 [REQ-0007]
    * @return: sorted CSV 
    */
    public static void generateInterestReport(List<Transaction> transactions) throws IOException 
    {
        filterIncomplete(transactions);
        List<Transaction> phpTxs = filterTransactions(transactions);

        List<String> uniqueAccounts = getUniqueAccounts(phpTxs);
        List<InterestReport> reportRows = new ArrayList<>();

        //filter must be based on latest current balance 
        for (String accNum : uniqueAccounts) {
            List<Transaction> accountTxs = getTransactionsForAccount(phpTxs, accNum);
            accountTxs.sort(Comparator.comparing(Transaction::date));
            Transaction latestTx = accountTxs.get(accountTxs.size() - 1);
            double expectedInterest = calculateExpectedInterest(latestTx.currentBalance());
            reportRows.add(new InterestReport(
                latestTx.accountName(),
                latestTx.currentBalance(),
                expectedInterest
            ));
        }

        List<InterestReport> filteredRows = filterBalance(reportRows);

        sortDescending(filteredRows, InterestReport::expectedInterest);

       writeReportToCsv(
            filteredRows,
            "interest_report.csv",
            "Report 2: Interest Expense Projection",
            "AccountName    CurrentBalance      ExpectedInterest",
            row -> String.format("%s,%.2f,%.2f",
                row.accountName(),
                row.currentBalance(),
                row.expectedInterest()
            )
        );
    }

    /*
    * @Purpose: to generate report 3 [REQ-0008]
    * @return: sorted CSV 
    */
    public static void generateSuspiciousReport(List<Transaction> transactions) throws IOException 
    {
        filterIncomplete(transactions);
        List<Transaction> phpTxs = filterTransactions(transactions);

        List<String> uniqueAccounts = getUniqueAccounts(phpTxs);
        List<SuspiciousReport> reportRows = new ArrayList<>();

        for (String accNum : uniqueAccounts) {
            List<Transaction> accountTxs = getTransactionsForAccount(phpTxs, accNum);
            accountTxs.sort(Comparator.comparing(Transaction::date));
            String accountName = accountTxs.get(0).accountName();

            //getting unique dates for this account 
            List<LocalDate> uniqueDates = new ArrayList<>();
            for (Transaction tx : accountTxs) {
                if (!uniqueDates.contains(tx.date())) {
                    uniqueDates.add(tx.date());
                }
            }
            //Process day separately 
            for (LocalDate date : uniqueDates){
                double totalWithdrawals = calculateTotalWithdrawals(accountTxs, date);
                int numberOfDeposits = countDeposits(accountTxs, date);
                double totalDeposits = calculateTotalDeposits(accountTxs, date);

                double turnoverVelocity = calculateTurnoverVelocity(totalDeposits, totalWithdrawals);
                String flag = riskFlag(totalWithdrawals, turnoverVelocity);

                reportRows.add(new SuspiciousReport(
                    accountName,
                    date,
                    totalWithdrawals,
                    numberOfDeposits,
                    totalDeposits,
                    turnoverVelocity,
                    flag
                ));
            }
        }

        //sort ascending by date, descendng by riskflag level 
        sortDescending(reportRows, SuspiciousReport::riskFlag);
        sortAscending(reportRows, SuspiciousReport::date);
        //write to csv 
        writeReportToCsv(
            reportRows,
            "suspicious_report.csv",
            "Report 3: Suspicious Client Behavior",
            "AccountName        Date        TotalWithdrawalAmounts      NumberOfDeposits        TotalDepositAmounts     TurnoverVelocity        RiskFlagLevel",
            row -> String.format("%s,%s,%.2f,%d,%.2f,%.2f,%s",
                row.accountName(),
                row.date(),
                row.totalWithdraw(),
                row.numDeposit(),
                row.totalDeposit(),
                row.turnoverVelocity(),
                row.riskFlag()
        )
    );
    }

    /*
    * @Purpose: to generate report 4 [REQ-0009], checks for any accounts that are dormant/inactive 
    * @return: sorted CSV 
    */

    public static void generateActivityReport(List<Transaction> transactions) throws IOException {
        filterIncomplete(transactions);
        List<Transaction> phpTxs = filterTransactions(transactions);

        List<String> uniqueAccounts = getUniqueAccounts(phpTxs);
        List<CustomReport> reportRows = new ArrayList<>();

        for (String accNum : uniqueAccounts) {
            List<Transaction> accountTxs = getTransactionsForAccount(phpTxs, accNum);
            accountTxs.sort(Comparator.comparing(Transaction::date));

            Transaction latestTx = accountTxs.get(accountTxs.size() - 1);
            int txCount = accountTxs.size();
            String status = determineActivityStatus(txCount);

            reportRows.add(new CustomReport(
                accNum,
                latestTx.accountName(),
                latestTx.date(),
                txCount,
                latestTx.currentBalance(),
                status
            ));
        }

        // Sort ascending by LastTransactionDate (oldest/most dormant accounts at the top)
        sortAscending(reportRows,CustomReport::lastTransactionDate);

        writeReportToCsv(
            reportRows,
            "activity_report.csv",
            "Report 4: Account Activity Status",
            "AccountNumber      AccountName     LastTransactionDate     TransactionCount        CurrentBalance      ActivityStatus",
            row -> String.format("%s,%s,%s,%d,%.2f,%s",
                row.accountNumber(),
                row.accountName(),
                row.lastTransactionDate(),
                row.transactionCount(),
                row.currentBalance(),
                row.activityStatus()
            )
        );
    }

    /*
    * @Purpose: to produce summary.json [REQ-0010]
    * @return: json file  
    */
    public static void generateSummaryJson(List<Transaction> transactions) throws IOException {
        filterIncomplete(transactions);
        List<Transaction> phpTxs = filterTransactions(transactions);
        List<String> uniqueAccounts = getUniqueAccounts(phpTxs);

        int totalAccounts = uniqueAccounts.size();
        double totalDeposits = 0.0;
        double totalWithdrawals = 0.0;
        double totalBalance = 0.0;
        double totalInterestExpense = 0.0;

        for (Transaction tx : phpTxs) {
            if ("DEPOSIT".equalsIgnoreCase(tx.type())) {
                totalDeposits += tx.amount();
            } else if ("WITHDRAWAL".equalsIgnoreCase(tx.type())) {
                totalWithdrawals += tx.amount();
            }
        }

        for (String accNum : uniqueAccounts) {
            List<Transaction> accountTxs = getTransactionsForAccount(phpTxs, accNum);
            accountTxs.sort(Comparator.comparing(Transaction::date));

            Transaction latestTx = accountTxs.get(accountTxs.size() - 1);
            totalBalance += latestTx.currentBalance();
            totalInterestExpense += calculateExpectedInterest(latestTx.currentBalance());
        }

        String jsonContent = String.format(
            "{\"total_accounts\": %d, \"total_deposits\": %.2f, \"total_withdrawals\": %.2f, \"total_balance\": %.2f, \"total_interest_expense\": %.2f}",
            totalAccounts,
            totalDeposits,
            totalWithdrawals,
            totalBalance,
            totalInterestExpense
        );

        // Export to summary.json
        Files.writeString(Path.of("summary.json"), jsonContent);

        // Display in console under Summary State
        System.out.println("\nSummary State (summary.json):");
        System.out.println(jsonContent);
    }

    /*
    * @Purpose: generate reports 
    */

    public static void generate_reports_menu(String file, List<Transaction> transactions) throws IOException {
        List<Transaction> phpTransactions = new ArrayList<>();
        boolean isLoaded = false;
        int subChoice;

        do {
            System.out.println("\nGenerate Reports\n");
            System.out.println("[1] Load the file");
            System.out.println("[2] Generate Reports");
            System.out.print("Enter choice: ");
            subChoice = scanner.nextInt();

            switch (subChoice) {
                case 1:
                    transactions.clear();
                    transactions.addAll(loadTransactions(file)); 
                    int loadedCount = transactions.size();

                    filterIncomplete(transactions);
                    phpTransactions = filterTransactions(transactions);
                    int phpCount = phpTransactions.size();

                    isLoaded = true;
                    System.out.printf("\nProcessing dataset... (%,d rows loaded, %,d filtered for PHP currency)\n",
                        loadedCount, phpCount);
                    break;

                case 2:
                    if (!isLoaded) {
                        System.out.println("\nPlease load the file first ([1]).");
                        break;
                    }
                    System.out.println("\nGenerating reports...");
                    System.out.println("Outputs saved to individual files...");

                    generatePrivilegeReport(phpTransactions);
                    generateInterestReport(phpTransactions);
                    generateSuspiciousReport(phpTransactions);
                    generateActivityReport(phpTransactions);
                    generateSummaryJson(phpTransactions);
                    break;

                default:
                    System.out.println("\nInvalid choice. Please enter 1 or 2.");
            }
        } while (subChoice != 2 || !isLoaded);
    }

    public static void main (String[] args)
    {
        String file = "bank_transactions.csv";
        List<Transaction> transactions;

        try {
            transactions = loadTransactions(file);
        } catch (IOException e) {
            System.err.println("Could not load CSV: " + e.getMessage());
            return;
        }

        boolean isEnd = false, isChoice = false;
        int choice;
        char choice2=' ';
        double balance = 1000;
        double[] rate = {1.00, 62.00, 0.40, 84.00, 72.00, 9.00};  
        String name="", inputStr="", accountNumber = ""; 
        do
        {
            choice = main_menu();
            switch(choice)
            {
                case 0:
                    System.out.println("Exiting..");
                    isEnd = true;
                    break;
                case 1:
                    try {
                    Transaction created = register_name(file, transactions);
                    name = created.accountName();
                    balance = created.currentBalance();
                    accountNumber = created.accountNumber(); 
                    } catch (IOException e) {
                        System.err.println("Failed to save account: " + e.getMessage());
                    }
                    break;
                case 2: 
                    try{
                        balance= deposit_amount(balance, accountNumber, rate, file, transactions);
                    }
                    catch(IOException e)
                    {
                        System.err.println("Failed to save deposit: " + e.getMessage());
                    }
                    break;
                case 3:
                    try{
                        balance= withdraw_amount(balance, accountNumber, rate, file, transactions);
                    }
                    catch(IOException e)
                    {
                        System.err.println("Failed to save withdrawal: " + e.getMessage());
                    }
                    break;
                case 4:
                    currency_exchange(rate);
                    break;
                case 5:
                    exchange_rate(rate);
                    break;
                case 6: 
                    interest_amount(name, balance);
                    break;
                case 7:
                    try {
                        generate_reports_menu(file, transactions);
                    } catch (IOException e) {
                        System.err.println("Failed to generate reports: " + e.getMessage());
                    }
                    break; 
            } 
            if (choice>=5)
            {
                do
                {
                     System.out.print("\nBack to the Main Menu (Y/N): ");
                    inputStr = scanner.next();

                    if (choice(inputStr)) {
                        isChoice = true;
                        choice2 = inputStr.charAt(0);
                        
                        if (choice2 == 'N' || choice2 == 'n') {
                            System.out.println("Exiting..");
                            isEnd = true;
                        }
                    } else {
                        isChoice = false;
                        System.out.println("Invalid input!");
                    }
                }while(!isChoice);
               
            }
            else if (choice == 1)
            {
                do
                {
                    System.out.print("\nRegister another account? (Y/N)?: ");
                    inputStr = scanner.next();
                    scanner.nextLine();

                    if (choice(inputStr)) {
                        isChoice = true; 
                        choice2 = inputStr.charAt(0);
                        
                        if (choice2 == 'Y' || choice2 == 'y') {
                           try {
                            Transaction created = register_name(file, transactions);
                            name = created.accountName();
                            balance = created.currentBalance();
                            } catch (IOException e) {
                                System.err.println("Failed to save account: " + e.getMessage());
                            }
                        } else if (choice2 == 'N' || choice2 == 'n') {
                            System.out.println("Exiting..");
                            
                        }
                    } else {
                        isChoice = false;
                        System.out.println("Invalid input!");
                    }
                } while (!isChoice || choice2 == 'Y' || choice2 == 'y');
            }
            else if (choice == 2 && !accountNumber.isEmpty())
            {
                do
                {
                    System.out.print("\nRDeposit another amount? (Y/N)?: ");
                    inputStr = scanner.next();
                    scanner.nextLine(); 
                    if (choice(inputStr)) {
                        isChoice = true; 
                        choice2 = inputStr.charAt(0);
                        
                        if (choice2 == 'Y' || choice2 == 'y') {
                           try{
                                balance= deposit_amount(balance, accountNumber, rate, file, transactions);
                            }
                            catch(IOException e)
                            {
                                System.err.println("Failed to save deposit: " + e.getMessage());
                            }
                        } else if (choice2 == 'N' || choice2 == 'n') {
                            System.out.println("Exiting..");
                            
                        }
                    } else {
                        isChoice = false;
                        System.out.println("Invalid input!");
                    }
                } while (!isChoice || choice2 == 'Y' || choice2 == 'y');
            }
            else if (choice == 3 && !accountNumber.isEmpty())
            {
                do
                {
                    System.out.print("\nWithdraw another amount? (Y/N)?: ");
                    inputStr = scanner.next();

                    if (choice(inputStr)) {
                        isChoice = true; 
                        choice2 = inputStr.charAt(0);
                        
                        if (choice2 == 'Y' || choice2 == 'y') {
                            try{
                                balance= withdraw_amount(balance, accountNumber, rate, file, transactions);
                            }
                            catch(IOException e)
                            {
                                System.err.println("Failed to save withdrawal: " + e.getMessage());
                            }
                        } else if (choice2 == 'N' || choice2 == 'n') {
                            System.out.println("Exiting..");
                        }
                    } else {
                        isChoice = false;
                        System.out.println("Invalid input!");
                    }
                } while (!isChoice || choice2 == 'Y' || choice2 == 'y');
            }
            else if (choice == 4)
            {
                do
                {
                    System.out.print("\nConvert another currency (Y/N)?: ");
                    inputStr = scanner.next();

                    if (choice(inputStr)) {
                        isChoice = true;
                        choice2 = inputStr.charAt(0);
                        
                        if (choice2 == 'Y' || choice2 == 'y') {
                            currency_exchange(rate);
                        } else if (choice2 == 'N' || choice2 == 'n') {
                            System.out.println("Exiting..");
                           
                        }
                    } else {
                        isChoice = false;
                        System.out.println("Invalid input!");
                    }
                } while (!isChoice || choice2 == 'Y' || choice2 == 'y');
            }
        }
        while(!isEnd);
    }
}
