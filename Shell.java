import java.util.*;

public class Shell extends Thread 
{

    public void Shell() {}
    public void Shell(String[] args) {}

    public void run()
    {
        int count = 1;
        while(true)
        {
            // Simulate shell and gather input from console
            SysLib.cout("Shell[" + count +"]% ");
            StringBuffer buf = new StringBuffer();
            SysLib.cin(buf);

            // Exit condition
            if(buf.toString().equals("exit"))
            {
                break;
            }
            // Separate input into arguement array
            String[] temp = SysLib.stringToArgs(buf.toString());

            //Parse input into command strings and delimiters
            String[] commands = new String[temp.length + 1];

            int command_index = 0;
            for(int i = 0; i < temp.length; i++)
            {
                // If string is a delimiter eg "&" or ";"
              if(temp[i].equals("&") || temp[i].equals(";"))
              {
                 commands[command_index + 1] = temp[i];
                 command_index += 2;
              }

               else
               {
               	 // Handle null index
                 if(commands[command_index] == null) 
                     {
                       commands[command_index] = temp[i];
                     }

                 // Otherwise, it's a command arguement eg "PingPong" or "abc" or "50"
                 else  
                   {
                       commands[command_index] = commands[command_index] + " " + temp[i];
                   }
               }
            }
       
            // Now, with an array of commands with arguements and delimiters, 
            // begin executing commands
            for(int i = 0; commands[i] != null; i++)
            {
                // If sequential execution
                if(commands[i].equals(";"))
                {
                    // Wait for the termination of the child thread
                    int tin = SysLib.join();
                    if(tin == -1)
                    {
                        SysLib.cerr("Error: failure to join\n");
                    }
                }
                if(commands[i].equals("&"))
                    {
                        continue;
                    }
                // Concurrent exeuction
                else if(!commands[i].equals("&"))
                {
                    // Parse command string and execute as individual arguements
                    int tid = SysLib.exec(SysLib.stringToArgs(commands[i]));
                    if(tid == -1)
                        {
                            SysLib.cerr("Error: failure to execute\n");
                        }
                }
            }
            // Increment coutn for shell prompt
            count++;
        }
        SysLib.sync();
        SysLib.exit();
    }
}

