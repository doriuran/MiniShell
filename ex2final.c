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

#define STDOUT_TO_FILE 1     // >
#define STFOUT_TO_FILE_CON 2 //>>
#define STDIN 3              //<
#define PERR 4               //2>
#define NOT_RED 0
int how_many_words(char *array);
char **make_and_fill_2d_array(int count_words, char *command);
void done_output(int howManyleft, int howManyComm, int pipe_Comannd, int redirection_Command);
void free_array(char **array, int count_words);
void fork_son(char **array_split, int count_words);
int serach_for_pipe(char *command);
int pos_of_red(char *command);
int pos_of_let(char *command, int pos);
int find_kind_redirection(char *command);
void fork_for_STDOUT_TO_FILE_(char **command, char *name);
void fork_for_STFOUT_TO_FILE_CON(char **command, char *name);
void fork_for_STDIN(char **command, char *name);
void fork_for_PERR(char **command, char *name);
int combiand_pipe_red(char *command);
int delete_spaces(char * command,int last_known_pos);
int main(int argc, char *argv[])
{
    int fd_write;
    fd_write = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    int howManyComm = 0;               //for counting commands
    int howManyleft = 0;               //for length
    int pipe_Command = 0;              //pipe counter
    int redirection_Command = 0;       //red counter
    int flag_for_pipe = 0;             //zero=their no pipe int the string
    int combaind_pipe_redirection = 0; //0 =not found one

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
        write(fd_write, command, strlen(command));
        command[strlen(command) - 1] = '\0'; //cuting the /n from the string
        //write(fd_write, command, strlen(command));

        combaind_pipe_redirection = combiand_pipe_red(command); //checking if their | and > ,>>,2>

        int count_words = 0;
        char spilt1_comm_pipe[strlen(command)];
        char spilt2_comm_pipe[strlen(command)];
        flag_for_pipe = serach_for_pipe(command); //checking if their a pipe

        if (flag_for_pipe != 0) //their a pipe their!
        {
            char duplicat_split1[strlen(command)];
            char duplicat_split2[strlen(command)];
            int count_pL__words = 0;
            int count_pR_words = 0;
            char combaind2[strlen(command)];
            char combaind1[strlen(command)];
            int kind_red;
            if (combaind_pipe_redirection != 0)
            {   //their a pipe and red togther in the string
                //int's for cutting corectley
                int red_combianed = pos_of_red(command);
                int first_letter_comb = pos_of_let(command, red_combianed);
                kind_red = find_kind_redirection(command); //holding the redd symbol
                combaind1[strlen(command)];
                if(kind_red==PERR||kind_red==STFOUT_TO_FILE_CON){

                   red_combianed=red_combianed-1;
                }
                    strncpy(combaind1, command, red_combianed); //copy to a new string without the redierect
              
                
                combaind1[delete_spaces(combaind1,red_combianed)] = '\0';
                combaind2[strlen(command)];
                strncpy(combaind2, command + first_letter_comb, strlen(command));
                strcpy(command,combaind1);
                
            }// now comabind 1 holding the pipe and combaind 2 just where the output need to go

            strncpy(spilt1_comm_pipe, command, flag_for_pipe);

            spilt1_comm_pipe[delete_spaces(spilt1_comm_pipe,flag_for_pipe)] = '\0';
            strcpy(duplicat_split1, spilt1_comm_pipe); //copy for counting words
            count_pL__words = how_many_words(duplicat_split1);

            strncpy(spilt2_comm_pipe, (command + flag_for_pipe + 1), strlen(command)); //right side of the pipe                                     //copy for counting words
            strcpy(duplicat_split2, spilt2_comm_pipe);
            count_pR_words = how_many_words(duplicat_split2);

            if (count_pL__words == 0 && count_pR_words == 0)
            {
                return (0);
            }
            else
            {
                if(kind_red!=0){
                pipe_Command++;
                howManyComm++;
                howManyleft += strlen(command);
                }
                char **words_pL_pipe = make_and_fill_2d_array(count_pL__words, spilt1_comm_pipe);
                char **words_pR_pipe = make_and_fill_2d_array(count_pR_words, spilt2_comm_pipe);
                //pipe
                int pipefd[2];

                pid_t leftpid, rightpid;
                int fd;
                pipe(pipefd);
                if ((leftpid = fork()) == 0)
                {
                    close(pipefd[0]);
                    //
                    dup2(pipefd[1], STDOUT_FILENO);
                    execvp(words_pL_pipe[0], words_pL_pipe); //EXECV
                }
                else if ((rightpid = fork()) == 0)
                {
                    close(pipefd[1]);
                    dup2(pipefd[0], STDIN_FILENO);

                    if(combaind_pipe_redirection==0)
                    {
                        execvp(words_pR_pipe[0], words_pR_pipe);
                        exit(0);
                    }
                    else
                     {
                        if(kind_red==STDOUT_TO_FILE){
                            fork_for_STDOUT_TO_FILE_(words_pR_pipe,combaind2);
                             exit(0);
                        }
                        if(kind_red==STFOUT_TO_FILE_CON){
                            fork_for_STFOUT_TO_FILE_CON(words_pR_pipe,combaind2);
                            exit(0);
                        }
                        if(kind_red==PERR){
                           
                            fork_for_PERR(words_pR_pipe,combaind2);
                            exit(0);
                        }                        
                    }
                }
                else
                {
                    close(pipefd[0]);
                    close(pipefd[1]);
                    wait(NULL);
                    wait(NULL);
                }
                free_array(words_pL_pipe, count_pL__words);
                free_array(words_pR_pipe, count_pR_words);
            }

        } //end of pipe |

        else
        { //start other red/regular

            int result_redirection = find_kind_redirection(command);
            if (result_redirection == NOT_RED)
            { //regular

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
                        done_output(howManyleft, howManyComm, pipe_Command, redirection_Command);
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
            } //end of regular
            else
            { //start of redirection
                int pos_of = pos_of_red(command);
                char red[strlen(command)];
                char name[strlen(command)];
                int first_let_singelchar = pos_of_let(command, pos_of);
                int red_words = 0;
                redirection_Command++;
                howManyComm++;
                howManyleft += strlen(command);
                char dup_r[strlen(red)];

                if (result_redirection == STDOUT_TO_FILE || result_redirection == STDIN)
                {
                    strncpy(red, command, pos_of);
                    red[pos_of] = '\0';
                    strcpy(dup_r, red);
                    red_words = how_many_words(dup_r);

                    char **words_r = make_and_fill_2d_array(red_words, red);
                    strncpy(name, command + first_let_singelchar, strlen(command));

                    if (result_redirection == STDOUT_TO_FILE)
                    {
                        fork_for_STDOUT_TO_FILE_(words_r, name);
                        free_array(words_r, red_words);
                    }
                    if (result_redirection == STDIN)
                    {
                        fork_for_STDIN(words_r, name);
                        free_array(words_r, red_words);
                    }
                }
                if (result_redirection == STFOUT_TO_FILE_CON || result_redirection == PERR)
                { //>>
                    strncpy(red, command, pos_of - 1);
                    red[pos_of - 1] = '\0';

                    strcpy(dup_r, red);
                    red_words = how_many_words(dup_r);

                    char **words_r = make_and_fill_2d_array(red_words, red);
                    strncpy(name, (command + first_let_singelchar), strlen(command));

                    if (result_redirection == STFOUT_TO_FILE_CON)
                    {
                        fork_for_STFOUT_TO_FILE_CON(words_r, name);
                        free_array(words_r, red_words);
                    }
                    if (result_redirection == PERR)
                    {
                        fork_for_PERR(words_r, name);
                        free_array(words_r, red_words);
                    }
                }
            } //end of redirection
        }
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
void done_output(int howManyleft, int howManyComm, int pipe_Command, int redirection_Command)
{
    double avg = (double)howManyleft / howManyComm;
    printf("Num of commands: %d\n", howManyComm);
    printf("Total length of all commands: %d\n", howManyleft);
    printf("Average length of all commands: %lf\n", avg);
    printf("Number of command that include pipe: %d\n", pipe_Command);
    printf("Number of command that include redirection: %d\n", redirection_Command);
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
int serach_for_pipe(char *command)
{
    int flag = 0;
    int k = 0;
    while (k < strlen(command))
    { //Check if their a command
        if (command[k] == '|')
        {
            return k;
            break;
        }
        else
            k++;
    }
    return flag;
}
int find_kind_redirection(char *command)
{
    int s = 0;
    while (s < strlen(command))
    {
        if (command[s] == '>' && command[s + 1] == '>')
        {
;
            return STFOUT_TO_FILE_CON; //>>
            break;
        }
        if (command[s] == '>')
        {

            return STDOUT_FILENO;

            break;
        }

        if (command[s] == '<')
        {

            return STDIN; //<

            break;
        }
        if (command[s] == '2' && command[s + 1] == '>')
        {
            return PERR; //2>

            break;
        }
        s++;
    }
    return NOT_RED;
}
int pos_of_red(char *command)
{
    int y = 0;
    while (y < strlen(command))
    {
        if (command[y] == '2' && command[y + 1] == '>')
        {
            return y + 1; //chagned from y to y+1
            break;
        }
        if (command[y] == '<' || command[y] == '>' && command[y + 1] != '>')
        {
            return y;
            break;
        }
        if (command[y] == '>' && command[y + 1] == '>')
        {
            return y + 1; //chagned from y to y+1
            break;
        }
        y++;
    }
    return 0;
}
int pos_of_let(char *command, int pos)
{
    int j = pos; //changed from pos+1 to pos
    while (j < strlen(command))
    {
        if (command[j] == ' ' && command[j + 1] != ' ')
        {
            return j + 1;
            break;
        }
        j++;
    }
    return pos + 1; //?
}
void fork_for_STDOUT_TO_FILE_(char **command, char *name)
{
    int fd, pid;
    pid = fork();
    if (pid == -1)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
        dup2(fd, STDOUT_FILENO);
        execvp(command[0], command);
    }
    else
    {
        wait(NULL);
    }
}
void fork_for_STFOUT_TO_FILE_CON(char **command, char *name)
{
    int fd, pid;
    pid = fork();
    if (pid == -1)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        
        fd = open(name, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
        dup2(fd, STDOUT_FILENO);
        execvp(command[0], command);
    }
    else
    {
        wait(NULL);
    }
}
void fork_for_STDIN(char **command, char *name)
{
    int fd, pid;
    pid = fork();
    if (pid == -1)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        fd = open(name, O_RDONLY, S_IRWXU);
        dup2(fd, STDIN_FILENO);
        execvp(command[0], command);
        close(fd);
    }
    else
    {
        wait(NULL);
    }
}
void fork_for_PERR(char **command, char *name)
{
    int fd, pid;
    pid = fork();
    if (pid == -1)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
        dup2(fd, STDERR_FILENO);
        execvp(command[0], command);
        close(fd);
    }
    else
    {
        wait(NULL);
    }
}
int combiand_pipe_red(char *command)
{ //will return the token for redrirection
    int s = 0;
    int temp = 0;
    int to_return = pos_of_red(command);
    while (s < strlen(command))
    {
        if (command[s] == '|')
        {
            temp = 1;
            break;
        }
        s++;
    }
    if (temp != 0 && to_return != 0)
    {
        return to_return;
    }
    else
    {
        return 0;
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
