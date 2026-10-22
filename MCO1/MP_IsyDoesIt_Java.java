/********************
Last names: Ilagan, Serios, Yardan
Language: Java
Paradigm(s): Procedural
********************/
import java.util.*;

public class MP_IsyDoesIt_Java
{
    private static final Scanner scanner = new Scanner(System.in);

    /*
    * Helper function 
    * @ Purpose: checks if provided choice is valid (number from 0-6)
    */
    public static boolean valid_choice(int choice)
    {
        return (choice == 0 ||
                choice == 1 || 
                choice == 2 ||
                choice == 3 ||
                choice == 4 ||
                choice == 5 ||
                choice == 6);
    }  
    /*
    *
    */


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
        if ( input== null || input.length() != 1) {
        return false;
    }
    char c = input.charAt(0);
    return c == 'Y' || c == 'y' || c == 'N' || c == 'n';
    }

    /*
    * @ Purpose: displays the main menu
    * @ return int containing the chosen menu 
    */
    public static int main_menu()
    {
        boolean isValid = false; 
        int input=0;
        do
        {
            System.out.println("Select Transaction: ");
            System.out.println("[0] Terminate program");
            System.out.println("[1] Register Account Name"); 
            System.out.println("[2] Deposit Amount"); 
            System.out.println("[3] Withdraw Amount"); 
            System.out.println("[4] Currency Exchange"); 
            System.out.println("[5] Record Exchange Rate"); 
            System.out.println("[6] Show Interest Amount"); 

            try
            {
                input = scanner.nextInt();
                if(valid_choice(input))
                {
                    isValid = true;
                }
                else
                {
                    System.out.println("Please enter a valid choice!");
                }  
            }
            catch (InputMismatchException e)
            {
                System.out.println("Please enter a valid choice!");
                scanner.next();
            }
            
        } while(!isValid);
        return input; 
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
    public static double deposit_amount(double balance, String name)
    {
        String input; 
        double deposit = 0.0;
        boolean isValid = false;
        System.out.println("Deposit Amount");
        System.out.println("Account Name: " + name);
        System.out.printf("\nCurrent Balance: %.2f\n", balance);
        System.out.println("Currency: PHP"); 

        scanner.nextLine();
        
        do
        {
            System.out.print("\nDeposit Amount: "); 
            input = scanner.nextLine();
            try
            {
                deposit = Double.parseDouble(input);
                if(deposit>0)
                {
                    isValid=true;
                }
                else
                {
                    System.out.println("Please enter an amount greater than 0.");
                }
                    
            } catch (NumberFormatException e)
            {
                System.out.println("Please enter a valid amount!(double only)");
            }
        } while (!isValid);
         
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
    public static double withdraw_amount(double balance, String name)
    {
        String input; 
        double withdraw = 0.0;
        boolean isValid = false;

        System.out.println("Withdraw Amount"); 
        System.out.println("Account Name: "+ name);
        System.out.printf("\nCurrent Balance: %.2f\n", balance); 
        System.out.println("Currency: PHP"); 

        scanner.nextLine();
        do
        {
            System.out.printf("\nWithdraw Amount: ");
            input = scanner.nextLine();
            try
            {
                withdraw = Double.parseDouble(input);

                if(withdraw>0 && withdraw<balance)
                {
                    isValid = true;
                }
                else if(withdraw>balance)
                {
                    System.out.println("Value exceeds current balance!"); 
                }
                else if(withdraw==0)
                {
                    System.out.println("Please input a number greater than 0.");
                }
            }
            catch (NumberFormatException e)
            {
                System.out.println("Please enter a valid amount!(double only)");
            }
        }while(!isValid);
        
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
        boolean isValid = false; 
        int input=0;
        double foreign=0.0; 
        scanner.nextLine();
        do
        {
            System.out.println("Record Exchange Rate ");
            exchange_menu();

            System.out.printf("\nSelect Foreign Currency: ");
            input = scanner.nextInt();
            if(valid_choice(input))
            {
                isValid = true;
            }
            else
                System.out.println("Please enter a valid choice!");
            

        } while(!isValid);

        isValid = false; 
        do
        {
            System.out.printf("\nExchange Rate: ");
            try
            {
                foreign = scanner.nextDouble();
                if(foreign>0)
                {
                    isValid = true;
                }
                else
                {
                    System.out.println("Exchange rate must be greater than 0!");
                }
            }
            catch (InputMismatchException e)
            {
                System.out.println("Please enter a valid amount! (Numbers only)");
                scanner.next(); 
            }
        }while(!isValid);

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
        boolean isValid = false; 
        int s_currency=0, e_currency=0; 
        double source= 0.0, convert = 0.0; 

            System.out.println("Foreign Currency Exchange");
            scanner.nextLine();
            do
            {
                System.out.println("Source Currency Option: ");
                exchange_menu();

                System.out.printf("\nSource Currency: ");
                s_currency = scanner.nextInt();
                if(valid_choice(s_currency))
                {
                    isValid = true;
                }
                else
                    System.out.println("Please enter a valid choice!");
            } while(!isValid);
            
            isValid = false; 
            do
            {
                System.out.printf("\nSource Amount: ");
                source = scanner.nextDouble();
                try
                {
                    if(source>0)
                    {
                        isValid = true;
                    }
                    else
                    {
                        System.out.println("Source amount must be greater than 0!");
                    }
                }
                catch (NumberFormatException e)
                {
                    System.out.println("Please enter a valid amount!(double only)");
                }
            }while(!isValid);

            switch(s_currency)
            {
                case 1:
                    source *= rate[0]; 
                    break;
                case 2:
                    source *= rate[1]; 
                    break;
                case 3:
                    source *= rate[2];  
                    break;
                case 4:
                    source *= rate[3]; 
                    break;
                case 5:
                    source *= rate[4]; 
                    break; 
                case 6:
                    source *= rate[5]; 
                    break; 
            }


            isValid = false; 
            do
            {
                System.out.println("Exchanged Currency Options: ");
                exchange_menu();

                System.out.printf("\nExchange Currency: ");
                e_currency = scanner.nextInt();
                if(valid_choice(e_currency))
                {
                    isValid = true;
                }
                else
                    System.out.println("Please enter a valid choice!");
            } while(!isValid);

            switch(e_currency)
            {
                case 1:
                    convert = source/rate[0]; 
                    break;
                case 2:
                    convert = source/rate[1]; 
                    break;
                case 3:
                    convert = source/rate[2]; 
                    break;
                case 4:
                    convert = source/rate[3]; 
                    break;
                case 5:
                    convert = source/rate[4];  
                    break; 
                case 6:
                    convert = source/rate[5];  
                    break; 
            }

            System.out.printf("\nExchange Amount: %.2f", convert);
        

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
        double balance = 0;
        double[] rate = {1.00, 62.00, 0.40, 84.00, 72.00, 9.00};  
        String name="", inputStr=""; 
        do
        {
            choice = main_menu();
            switch(choice)
            {
                case 0: 
                    isEnd = true; 
                    break;
                case 1:
                    name = register_name();
                    break;
                case 2: 
                    balance= deposit_amount(balance, name);
                    break;
                case 3:
                    balance= withdraw_amount(balance, name);
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
            if (choice !=0 && choice!=4)
            {
                do
                {
                     System.out.print("\nBack to the Main Menu (Y/N): ");
                    inputStr = scanner.next();

                    if (choice(inputStr)) {
                        isChoice = true;
                        choice2 = inputStr.charAt(0);
                        
                        if (choice2 == 'N' || choice2 == 'n') {
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
