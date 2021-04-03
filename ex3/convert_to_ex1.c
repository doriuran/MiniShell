#define SIZE 2000 //CHECK
#define SIZE_COM 510
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>


int how_many_words(char *array);
char **make_and_fill_2d_array(int count_words, char *command);
void done_output(int howManyleft, int howManyComm);
void free_array(char **array, int count_words);
void fork_son(char **array_split, int count_words);

int pos_of_red(char *command);
int pos_of_let(char *command, int pos);
int delete_spaces(char * command,int last_known_pos);
int main()
{
    int howManyComm = 0;               //for counting commands
    int howManyleft = 0;               //for length

    while (1)                          //Endless loop
    {
        char arr[SIZE];       //hold the path of the dir
        char done[] = "done"; //for exiting the shell
        getcwd(arr, SIZE);
        struct passwd *p;
        p = getpwuid(getuid());
        printf("%s%s%s%s", p->pw_name, "@", arr, ">");
        char command[SIZE_COM];
        fgets(command, SIZE_COM, stdin); //getting command from the user
        command[strlen(command) - 1] = '\0'; //cuting the /n from the string

        int count_words = 0;
         //start other red/regular

                char duplicate_for_count[strlen(command)];
                strcpy(duplicate_for_count, command);

                count_words = how_many_words(duplicate_for_count);
                if (count_words != 0)
                {
                    howManyComm++;
                    howManyleft += strlen(command);
                    char **words = make_and_fill_2d_array(count_words, command);
                    if (strcmp(words[0], done) == 0)
                    {
                        done_output(howManyleft, howManyComm);
                        free_array(words, count_words);
                        exit(1);
                    }
                    if ((strcmp(words[0], "cd")) != 0)
                    {
                        fork_son(words, count_words);
                    }
                    else
                    {
                        printf("command not supported (Yet)\n");
                    }
                }
             //end of regular
    }
    return (0);
}

int how_many_words(char *array)
{
    int count_words = 0;
    char *temp = strtok(array, " "); //temp for cutting
    while (temp != NULL)
    {
        count_words++;
        temp = strtok(NULL, " ");
    }
    return count_words;
}
char **make_and_fill_2d_array(int count_words, char *command)
{
    char **words = (char **)malloc((count_words + 1) * sizeof(char *)); //n+1

    if (words == NULL)
    {
        printf(" ERR=Coudlnt allocate memory");
        exit(1);
    }

    char *split = strtok(command, " ");
    int s = 0;
    while (s < count_words) //split != NULL
    {
        words[s] = (char *)malloc(strlen(split) * sizeof(char));
        if (words[s] == NULL)
        {
            printf("ERR -Coudlnt allocate memory");
            exit(1);
        }
        else
        {
            strcpy(words[s], split); ///copy from split to words[s]
            s++;
        }
        split = strtok(NULL, " ");
    }
    words[s] = NULL;

    return words;
}
void done_output(int howManyleft, int howManyComm)
{
    double avg = (double)howManyleft / howManyComm;
    printf("Num of commands: %d\n", howManyComm);
    printf("Total length of all commands: %d\n", howManyleft);
    printf("Average length of all commands: %lf\n", avg);
    printf("See you Next time!\n");
}
void free_array(char **array, int count_words)
{
    for (int k = 0; k < count_words; k++)
        free(array[k]);
    free(array);
}
void fork_son(char **words, int count_words)
{
    pid_t son;
    son = fork();
    if (son == -1)
    {
        printf("ERR-fork Faild");
        exit(1);
    }
    if (son == 0)
    {
        execvp(words[0], words);
        exit(1); //if couldnt do the comman pid_t son;d
    }
    else
    {
        wait(NULL);
        free_array(words, count_words);

    }
}

int delete_spaces(char * command,int last_known_pos){
    int t=last_known_pos-1;
    int toReturn=last_known_pos;
    while (t>=0){
        if(command[t]==' '){
            toReturn=t;
            t--;
            
        }
        else if(command[t]!=' '){
            return toReturn;
            break;
        }
        t--;
        
        
    }
    return toReturn;

}
