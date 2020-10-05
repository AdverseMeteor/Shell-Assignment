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

  pid_t p_id[15];

  int index = 0;
  int counter = 0;

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


    if(cmd_str[0]=='!')
    {
      int tuff = atoi(&token[0][1]);
      strncpy(working_str, history[tuff], 255);
    }

    if(cmd_str[0] == '\n' || cmd_str[0] == ' ')
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

    else if(strcmp(token[0],"showpids")==0)
    {
      if(counter < 15)
      {
        for(int i=0; i<index; i++)
        {
          printf("%d: %d\n", i, p_id[i]);
        }
      }
      else
      {
        for(int i=0; i<15; i++)
        {
          printf("%d: %d\n", i, p_id[i]);
        }
      }
      continue;
    }

    else if(strcmp(token[0],"history")==0)
    {
      if(counter < 15)
      {
        for(int i=0; i<index; i++)
        {
          printf("%d: %s\n", i, history[i]);
        }
      }
      else
      {
        for(int i=0; i<15; i++)
        {
          printf("%d: %s\n", i, history[i]);
        }
      }
      continue;
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

      if(strcmp(token[0],"cd")==0)
      {
        chdir(token[1]);
      }

      p_id[index]=getpid();

      fflush(NULL);
    }

    int token_index  = 0;
    for( token_index = 0; token_index < token_count; token_index ++ )
    {
      printf("token[%d] = %s\n", token_index, token[token_index] );
    }

    strncpy(history[index], token[0][0], MAX_COMMAND_SIZE);

    index++;
    if(index>14)
    {
      index=0;
    }

    free( working_root );
    counter++;
  }
  return 0;
}
