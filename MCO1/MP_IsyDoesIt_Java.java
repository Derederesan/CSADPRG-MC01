/********************
Last names: Ilagan, Serios, Yardan
Language: Java
Paradigm(s): Procedural
********************/
import java.util.*;

public class IsyDoesIt
{
    private static final Scanner scanner = new Scanner(System.in);

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
                if (input >= 0 && input <= 6) 
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
            System.out.println("[0] Exit Transaction"); 

            return valid_choice("Choice: ");
    }
    /*
    * @ Purpose: Registers the user's name [REQ-0009]
    * @ return String containing the user's name 
    */
    public static String register_name()
    {
        boolean isValid = false; 
        String input;

        System.out.println("Register Account Name");
        
        //Input for account name 
        scanner.nextLine();
        do
        {
            System.out.print("Account Name: ");
            input = scanner.nextLine();
            if(name(input))
            {
                isValid = true;
            }
            else
                System.out.println("Please enter a valid name!");
        } while(!isValid);

        System.out.println("Account name registered!");
        return input; 
    }
    /*
    * @ Purpose: Deposits money to the user's bank account[REQ-0004]
    * @ Provision to set Philippine Peso(PHP) as Base Currency [REQ-0002]
    * @ return double containing the user's new balance 
    */
    public static double deposit_amount(double balance, String name, double rate [])
    { 
        double deposit = 0.0;
        String currency = "";
        boolean isValid = false; 
        System.out.println("Deposit Amount");
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
        return balance; 
    }
    /*
    * @ Purpose: Withdraws money from the user's bank account [REQ-0005]
    * @ Provision to set Philippine Peso(PHP) as Base Currency [REQ-0002]
    * @ return double containing the user's new balance 
    */
    public static double withdraw_amount(double balance, String name, double rate[])
    {
        String currency = ""; 
        double withdraw = 0.0;
        boolean isValid = false;

        System.out.println("Withdraw Amount"); 
        System.out.println("Account Name: "+ name);
        System.out.printf("\nCurrent Balance: %.2f", balance);
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
    public static void main (String[] args)
    {
      
        boolean isEnd = false, isChoice = false;
        int choice;
        char choice2=' ';
        double balance = 1000;
        double[] rate = {1.00, 62.00, 0.40, 84.00, 72.00, 9.00};  
        String name="", inputStr=""; 
        do
        {
            choice = main_menu();
            switch(choice)
            {
                case 0:
                    System.out.println("Exiting..");
                    isEnd = true;
                case 1:
                    name = register_name();
                    break;
                case 2: 
                    balance= deposit_amount(balance, name, rate);
                    break;
                case 3:
                    balance= withdraw_amount(balance, name, rate);
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
            } 
            if ((choice >=1 && choice<=3) || (choice>=5))
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
            else if (choice == 4)
            {
                do
                {
                    System.out.print("\nConvert another currency (Y/N)?: ");
                    inputStr = scanner.next();

                    if (choice(inputStr)) {
                        isChoice = true; // FIX: Mark choice as valid to exit input-check loop
                        choice2 = inputStr.charAt(0);
                        
                        if (choice2 == 'Y' || choice2 == 'y') {
                            currency_exchange(rate); // Call again if Y
                        } else if (choice2 == 'N' || choice2 == 'n') {
                            System.out.println("Exiting..");
                            // Returns to main menu naturally on next main loop iteration
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
