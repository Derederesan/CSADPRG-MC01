********************
Last names: Last names: Ilagan, Serios, Yardan
Language: Java
Paradigm(s): Procedural 
********************

import java.util.*;

public class main_1 {

    // Global scanner accessible by all static methods
    static Scanner scanner = new Scanner(System.in);

    /*
     * checks for valid number input 
     * return: true or false 
     */
    public static boolean validChoice(String input) {
        try {
            int choice = Integer.parseInt(input.trim());
            return choice >= 1 && choice <= 6;
        } catch (NumberFormatException e) {
            return false;
        }
    }

    /*
     * print the main menu 
     * return : the user's choice 
     */
    public static int main_menu() {
        String input;

        System.out.println("Select Transaction: ");
        System.out.println("[1] Register Account Name");
        System.out.println("[2] Deposit Amount");
        System.out.println("[3] Withdraw Amount");
        System.out.println("[4] Currency Exchange");
        System.out.println("[5] Record Exchange Rates");
        System.out.println("[6] Show Interest Amount\n");

        do {
            System.out.print("Choice: ");
            input = scanner.nextLine();

            if (!validChoice(input)) {
                System.out.println("Invalid input! Please enter a number between 1 and 6.");
            }
        } while (!validChoice(input));

        int choice = Integer.parseInt(input.trim());

        System.out.println("\n***");
        System.out.println("Choice = " + choice);
        System.out.println();

        return choice;
    }

    /*
     * registers the user's account
     * [REQ-0004][REQ-0005][REQ-0006]
     */
    public static void register_account() {
        System.out.println("Register Account Name");
        System.out.print("Account Name: ");
        String name = scanner.nextLine();
        System.out.println();
        System.out.println("***");
        System.out.println("Account Name = " + name);
    }

    /*
     * Deposits amount based on user input
     * [REQ-0007][REQ-0008][REQ-0009][REQ-0010]
     */
    public static void deposit_amount() {
        System.out.println("Deposit Amount");
        System.out.print("Account Name: ");
        String name1 = scanner.nextLine();
        System.out.println("Current Balance: 1000");
        System.out.println("Currency: PHP"); 
        System.out.println();
        System.out.print("Deposit Amount: ");
        float amount = Float.parseFloat(scanner.nextLine());
        System.out.println();
        System.out.println("***");
        System.out.println("Account Name = " + name1);
        System.out.println("Deposit Amount = " + amount);
    }

    /*
     * Withdraws amount based on user input
     * [REQ-0011][REQ-0012][REQ-0013][REQ-0014]
     */
    public static void withdraw_amount() {
        System.out.println("Withdraw Amount");
        System.out.print("Account Name: ");
        String name2 = scanner.nextLine();
        System.out.println("Current Balance: 1000");
        System.out.println("Currency: PHP"); 
        System.out.println();
        System.out.print("Withdraw Amount: ");
        float amount1 = Float.parseFloat(scanner.nextLine());
        System.out.println();
        System.out.println("***");
        System.out.println("Account Name = " + name2);
        System.out.println("Withdraw Amount = " + amount1);
    }

    /*
     * Returns the exchange of a given amount 
     * [REQ-0015][REQ-0016][REQ-0017]
     */
    public static void currency_exchange() {
        System.out.println("Foreign Currency Exchange");
        System.out.print("Source Amount(PHP): ");
        double amount2 = Double.parseDouble(scanner.nextLine());
        System.out.println();
        System.out.println("Exchanged Currency");
        double rate1 = 1 * amount2;
        double rate2 = 62 * amount2;
        double rate3 = 0.40 * amount2;
        double rate4 = 84 * amount2;
        double rate5 = 72 * amount2;
        double rate6 = 9 * amount2;
        System.out.printf("[1] Philippine Peso (PHP) = %.2f%n", rate1);
        System.out.printf("[2] United States Dollar (USD) = %.2f%n", rate2);
        System.out.printf("[3] Japanese Yen (JPY) = %.2f%n", rate3);
        System.out.printf("[4] British Pound Sterling (GBP) = %.2f%n", rate4);
        System.out.printf("[5] Euro (EUR) = %.2f%n", rate5);
        System.out.printf("[6] Chinese Yuan Renminbi (CNY) = %.2f%n", rate6);
        System.out.println();
        System.out.println("***");
        System.out.println("Source Currency = Philippine Peso (PHP)");
        System.out.printf("Source Amount (PHP) = %.2f%n", amount2);
    }

    /*
     * Displays the selected exchange rate 
     * [REQ-0018][REQ-0019][REQ-0020]
     */
    public static void record_exchange() {
        double rate = 0;
        System.out.println("Record Exchange Rate");
        System.out.println();
        System.out.println("[1] Philippine Peso (PHP)");
        System.out.println("[2] United States Dollar (USD)"); 
        System.out.println("[3] Japanese Yen (JPY)"); 
        System.out.println("[4] British Pound Sterling (GBP) "); 
        System.out.println("[5] Euro (EUR) "); 
        System.out.println("[6] Chinese Yuan Renminni (CNY) "); 
        System.out.println();
        System.out.print("Select Foreign Currency: ");
        int currency = Integer.parseInt(scanner.nextLine());
        
        if (currency == 1) {
            rate = 1.00;
        } else if (currency == 2) {
            rate = 62.00;
        } else if (currency == 3) {
            rate = 0.40;
        } else if (currency == 4) {
            rate = 84.00;
        } else if (currency == 5) {
            rate = 72.00;
        } else if (currency == 6) {
            rate = 9.00;
        }

        System.out.println();
        System.out.println("***");
        System.out.println("Select Foreign Currency = " + currency);
         System.out.printf("Exchange Rate = %.2f%n", rate);
    }

    public static void main(String[] args) {
        int choice = main_menu(); // Store returned choice
        switch (choice) {
            case 1:
                register_account();
                break;
            case 2:
                deposit_amount();
                break;
            case 3:
                withdraw_amount();
                break;
            case 4:
                currency_exchange();
                break;
            case 5:
                record_exchange();
                break;
        }
    }
}
