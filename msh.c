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

#define MAX_NUM_ARGUMENTS 11     // Mav shell only supports 11 arguments

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE ); //We need to start with getting our varaibles sorted with.
                                                      //We make a history array for saving our commands later
  char * history[15];
  for(int i=0; i<15; i++) //We need to malloc and zero out the uninitalized array to use it.
  {
    history[i] = (char*) malloc(MAX_COMMAND_SIZE);
    memset(history[i], 0, MAX_COMMAND_SIZE);
  }
  int h_index;

  pid_t p_id[15];     //A pid array to save our pids.

  int index = 0;
  int counter = 0; //Found it useful to keep track for how many iterations had happened in the program.
                  //It allows us to tell whether the arrays were max out or growing

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

    strncpy(history[h_index++], cmd_str, MAX_COMMAND_SIZE); //In order to print out the history array, we save the commands before it goes into tokens
                                                            //We also need to keep track for the individaul index for history as the program goes on.
    if(h_index>14)
    {
      h_index=0;
    }

    if(cmd_str[0]=='!')                           //As previously said, easier to handle the history before tokenization
    {                                             //By using an if statement, we can do a quick switch of our input by copying it into working_str
      int tuff = atoi(&cmd_str[1]);               // and letting it get tokenized
      strncpy(working_str, history[tuff], 255);
    }

    if(cmd_str[0] == '\n' || cmd_str[0] == ' ')   //Dealing with empty enters and spaces, in order not to seg fault in the menu
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

    if(strcmp(token[0],"quit")==0 || strcmp(token[0],"exit")==0 ) //Very self-explanotory, this if statement will be our loop breaker
    {
      break;
    }

    else if(strcmp(token[0],"showpids")==0) //Using our counter from before, we separate the print based on array length
    {                                       //Allows us to print each pid stored within our array
      if(counter < 15)
      {
        for(int i=0; i<index; i++)
        {
          printf("%d: %l\n", i, p_id[i]);
        }
      }
      else
      {
        for(int i=0; i<15; i++)
        {
          printf("%d: %l\n", i, p_id[i]);
        }
      }
      continue;
    }

    else if(strcmp(token[0],"history")==0)    //The portion for printing history is almost exactly
    {                                         //the same as the showpids
      int i;                                  //The only difference is that it prints the history array
      if(counter < 15)                        //(where we store our commands) instead
      {
        for(i=0; i<=index; i++)
        {
          printf("%d: %s\n", i, history[i]);
        }
      }
      else
      {
        int trace=h_index;
        for(int i=0; i<15; i++)
        {
          printf("%d: %s\n", i, history[trace]);
          if(trace>14)
          {
            trace=0;
          }
          trace++;
        }

      }
      free( working_root );       //Free the working_root because the loop will
      continue;                   //continue up to the beginning without clearing it first
                                  //so it is done here before it goes
    }

    pid_t pid = fork( );          //Our main driver of the shell
                                 //We fork to create the child in which it will
    if( pid == 0 )               //we turned into a different process using exec
    {
      int ret = execvp( token[0], &token[0] );

      if( ret == -1 )
      {
        printf("%s: Command not found.\n", token[0]);
        break;
      }
      p_id[index]=getpid();
    }

    else
    {
      int status;           //make the parent wait while the child is doing its thing
      wait( & status );     //The shell will keep forking new processes everytime
                            //A command is entered, but some commands are not built in
      if(strcmp(token[0],"cd")==0) //The change directory has to be implemented because
      {                             //it was not bulit in originally
        chdir(token[1]);
      }

       //We get our pid and store into our array in order
                            //to print it for later
      fflush(NULL);
    }

    index++;              //Increment our counters and indexes before we
    if(index>14)          //proceed back up to the loop
    {
      index=0;
    }

    free( working_root );
    counter++;
  }
  return 0;
}
