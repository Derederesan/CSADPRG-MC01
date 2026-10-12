//Register account name 
import java.util.*;
//checking if account name is correct 
public class RegisterName
{
    
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

    //checking if input for prompt is correct 
    public static boolean choice(char choice)
    {
        return (choice == 'Y' || choice == 'y' || choice == 'n' || choice == 'N');
    }

    public static void main (String[] args)
    {
        Scanner scanner = new Scanner(System.in);
        boolean isValid = false; 
        String input = "";
        char choose = ' ';

        System.out.println("Register Account Name");
        //Input for account name 
        do
        {
            System.out.print("Account Name: ");
            input = scanner.nextLine();

            if(name(input))
                isValid = true;
            else
                System.out.println("Please enter a valid name!");
        } while(!isValid);

        //Prompt to go back to the main menu 

        isValid = false; 

        do
        {
            System.out.print("Back to the Main Menu(Y/N): ");
            choose = scanner.next().charAt(0);

            if(choice(choose))
                isValid = true;
            else
                System.out.println("Y or N only!");
        } while(!isValid);
        scanner.close();
        
    }
}


