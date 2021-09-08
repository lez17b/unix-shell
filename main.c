/*************************************************************************
 * 12/31/2016                                                            *
 * shell.c was downloaded from MTU:                                      *
 * http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/fork/exec.html     *
 *                                                                       *
 * NOTE: The original code has quite ugly parse() routine with dangerous *
 * library routine gets(). To address these deficiences, I (Gang-Ryung)  *
 * rewrote main() and parse() routine using fgets and strtok.            *
 *************************************************************************/

/* ----------------------------------------------------------------- */
/* PROGRAM  shell.c                                                  */
/*    This program reads in an input line, parses the input line     */
/* into tokens, and use execvp() to execute the command.             */
/* ----------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

#define MAXLINE 512
#define MAXCMD 64


/* ----------------------------------------------------------------- */
/* FUNCTION  parse:                                                  */
/*    This function takes an input line and parse it into tokens.    */
/* It first replaces all white spaces with zeros until it hits a     */
/* non-white space character which indicates the beginning of an     */
/* argument.  It saves the address to argv[], and then skips all     */
/* non-white spaces which constitute the argument.                   */
/* ----------------------------------------------------------------- */

void  parse(char *line, char **argv)
{
    static char* delimiter = " \n\t";
    static char* cmd_sep = ";";
    char* cmds[MAXCMD];
    char *token = strtok(line, delimiter);

    while (token != NULL)
    {
        *argv++ = token;
        token = strtok(NULL, delimiter);
    }

    *argv = (char *)'\0';                             /* mark the end of argument list  */
}

/* ----------------------------------------------------------------- */
/* FUNCTION execute:                                                 */
/*    This function receives a command line argument list with the   */
/* first one being a file name followed by its arguments.  Then,     */
/* this function forks a child process to execute the command using  */
/* system call execvp().                                             */
/* ----------------------------------------------------------------- */


void  execute(char **argv)
{
    pid_t  pid;
    int    status;

    if ((pid = fork()) < 0)
    {                                                        /* fork a child process           */
        printf("*** ERROR: forking child process failed\n");
        exit(1);
    }
    else if (pid == 0)
    {                                                        /* for the child process:         */
        if (execvp(*argv, argv) < 0)
        {                                                    /* execute the command  */
            printf("*** ERROR: exec failed\n");
            exit(1);
        }
    }
    else
    {                                                        /* for the parent:      */
        while (wait(&status) != pid)                         /* wait for completion  */
            ;
    }
}


/* ----------------------------------------------------------------- */
/*                  The main program starts here                     */
/* ----------------------------------------------------------------- */

int main(int argc, char *argv[])
{
    char  line[1024];                                /* the input line                 */
    char  *c = NULL;
    int batch = 0;
    printf("Shell -> ");                             /*   display a prompt             */

    if (argc > 1)
    {
        freopen(argv[1], "r", stdin);
        if (stdin == NULL)
        {
            perror("**Batch file failed to open. Exiting.\n");
            exit(EXIT_FAILURE);
        }
        batch = 1;
    }


    while (fgets(line, sizeof(line), stdin))
    {
        if (strlen(line) > MAXLINE)
        {
            perror("Input line max exceeded. Command Ignored. Retry.\n");
            if(!batch)
                printf("Shell -> ");
            continue;
        }
        if (line[0] == 0 || line[0] == 13 || line[0] == 10 ||
            line[0] == 32 || line[0] == 9)
        {
            if(!batch)
                printf("Shell -> ");
            continue;
        }

        /* repeat until EOF .... */
        parse(line, argv);                           /*   parse the line     */
        if (strcmp(argv[0], "exit") == 0)            /* is it an "exit"?     */
            exit(0);                                 /*   exit if it is      */
            execute(argv);                           /* otherwise, execute the command */
            printf("Shell -> ");                     /*   display a prompt             */
    }
    return 0;
}

// Helper functions: