/* Name: Cesar Rea
   ID: 1001643491
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11     // Mav shell only supports five arguments

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

  char * history[15];//Malloc and zero it out
  history[0] = (char*) malloc(MAX_COMMAND_SIZE);
  memset(history[0], 0, MAX_COMMAND_SIZE);

  int h_index;

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;

    char *working_str  = strdup( cmd_str );

    //Save History here
    strncpy(history[h_index++], cmd_str, MAX_COMMAND_SIZE);
    if(h_index>14)
    {
      h_index=0;
    }

    if(cmd_str[0]=='!')
    {
      int index = atoi(&cmd_str[0][1]);
      if( index < 15)
      {
        for(int i=0;i<index;i++)
        {
          printf("%s\n", history[index]);
        }
      }

      else
      {
        index=h_index;
        for(int i=0; i<15; i++)
        {
          printf("%s\n", history[index]);
          if(index > 14)
          {
            index=0;
          }
        }
      }
    }

    else if(cmd_str[0] == '\n')
    {
      continue;
    }
    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check

    if(strcmp(token[0],"quit")==0 || strcmp(token[0],"exit")==0 )
    {
      break;
    }

    else if(strcmp(token[0],"cd")==0)
    {
      chdir(token[1]);
    }

    pid_t pid = fork( );
    if( pid == 0 )
    {
      int ret = execvp( token[0], &token[0] );

      if( ret == -1 )
      {
        printf("%s: Command not found.\n", token[0]);
        break;
      }
    }

    else
    {
      int status;
      wait( & status );

      fflush(NULL);
    }

    int token_index  = 0;
    for( token_index = 0; token_index < token_count; token_index ++ )
    {
      printf("token[%d] = %s\n", token_index, token[token_index] );
    }

    free( working_root );

  }
  return 0;
}
