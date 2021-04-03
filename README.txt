File: Ex2 ex2final.c	
login name= doriy

Written by:Dor iuran,id=206123689



Program details:
The program provides Shell for the linux operation .
The program support variety of commands but for now "CD" command doesnt support.
The program support pipe and rediraction togther and alone.
Each command that the user insert to the program writting inside a txt file,
User can write commands as much he want.
To finish the program the user need to print "Done".
When "done" is wriiting ,few lines are printed:
                    1.Number of commands 
                    2.Length of all the commands
                    3.average of command.
                    4.Number of commands that using pipe;
                    5.Number of commmands  that using redirection.


Compile:gcc ex2final.c -o ex2final
Run: ./ex2final logs.txt

Program files:
  
1.ex2final.c -The main,the user interface with the program.

Input required:
The runing of the program depands on passing a file to the main.
Commands for the linux operation.
The porgram getting the main command and the arguments.
For example :pwd,ls,more,cat.and operttors like | , > , >> ,2> ,<.

output:
Foe each command the operation system turns on  it's methood.


