#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include "cs165-p3.h"

FILE *ifile;
FILE *ofile;
system_t sys;



/******************************************************************************

Command processing functions

*******************************************************************************/

/******************************************************************************

Function: find_command
Input Args: 
          cmdbuf - buffer for the command line from the input file
          len - length of the cmdbuf
Returns: a command type or an error value (<0)
         
find_command determines the specific command and checks the argument
count.

*******************************************************************************/


int find_command( char *cmdbuf, int len )
{
  int ct =  strct( cmdbuf, len );

  switch( cmdbuf[0] ) {
  case 'a':   // add
    if ( ct != 5 ) {
      return -1;
    }
    return Q_ADD;
    break;
  case 'c':   // change
    if ( ct != 4 ) {
      return -1;
    }
    return Q_CHANGE;
    break;
  case 'l':   // login
    if ( ct != 2 ) {
      return -1;
    }
    return Q_LOGIN;
    break;
  case '#':   // comment lines
  case '%':
  case '\n':
    return Q_SKIP;
    break;
  }

  return -1;
}


/******************************************************************************

Function: apply_command
Input Args: 
          cmd - command number
          args - arguments string for the command
Returns: 0 on success and <0 on error
         
Run the respective command for the question.

*******************************************************************************/

int apply_command( int cmd, char *question_index, char *args )
{
  char *arg1 = NULL, *arg2 = NULL, *arg3 = NULL;
  int res = 0;
  
  switch( cmd ) {
  case Q_ADD:
    if (( sscanf( args, "%ms %ms %ms", &arg1, &arg2, &arg3 )) != 3 ) {
	return -1;
    }
    question_t *q= add_question( question_index, arg1, arg2, arg3 );
    if ( q == NULL ) {
      res = -1;
    }
    fprintf( ofile, "ADD for question %s %s %s %s : result %d\n", question_index, arg1, arg2, arg3, res );
    return res;
    break;
  case Q_CHANGE:
    if (( sscanf( args, "%ms %ms", &arg1, &arg2 )) != 2 ) {
	return -1;
    }
    res = change_question( question_index, arg1, arg2 );
    fprintf( ofile, "CHANGE for question %s %s %s : result %d\n", question_index, arg1, arg2, res );
    return res;
    break;
  case Q_LOGIN:
    res = login_question( question_index );
    fprintf( ofile, "LOGIN with question %s gives login result: %s\n",
             question_index, ( res ? "Failed" : "Success" ) );
    return res;
    break;
  default:
    fprintf( ofile, "Skip: apply_command - Invalid command: %d\n", cmd );
    return -1;
    break;
  }
}


  
/*********************************
  
Question commands

**********************************/


question_t *find_question( char *question_index, int *qindex )
{
  *qindex = atoi( question_index );

  if ( *qindex < MAX_QUESTIONS ) {
    return sys.questions[*qindex];
  }
      
  return (question_t *)NULL;
}


question_t *add_question( char *question_index, char *question_type, char *question, char* answer )
{
  int err;
  question_t *q;
  int_q *iq;
  string_q *sq;
  int int_answer, i;
  int qindex;
  
  find_question( question_index, &qindex );
  if ( qindex >= MAX_QUESTIONS ) {
    return (question_t *)NULL;
  } 
  
  switch ( question_type[0] ) {
  case 'i':
    iq = (int_q *) calloc( 1, sizeof( int_q ) );
    for ( i = 0; answer[i] != '\0'; i++ ) {
      if (( answer[i] < 48 ) || ( answer[i] > 57 )) {
	return (question_t *)NULL;
      }
    }
    int_answer = atoi( answer );
    iq->add = intq_add;
    err = iq->add( qindex, question, int_answer, iq );
    q = (question_t *)iq;
    break;
  case 's':
    sq = (string_q *) calloc( 1, sizeof( string_q ) );
    sq->add = stringq_add;
    err = sq->add( qindex, question, answer, sq );
    q = (question_t *)sq;
    break;
  default:
    fprintf( ofile,  "ERR: add_question: Not a valid question type: %s\n", question_type );
    return (question_t *)NULL;   
  }

  // update count
  if ( err < 0 ) {
    return (question_t *)NULL;
  }
  
  sys.questions[qindex] = q;
  sys.qct++;
  return q;
}



int change_question( char *question_index, char *question, char *answer )
{
  int int_answer;
  int_q *iq;
  string_q *sq;
  int qtype = INT_QUESTION, i;
  int qindex;
  question_t *q = find_question( question_index, &qindex );

  if ( q == NULL )
    return -1;

  for ( i = 0; answer[i] != '\0'; i++ ) {
    if (( answer[i] < 48 ) || ( answer[i] > 57 )) {
      qtype = STRING_QUESTION;
      break;
    }
  }  
  
  switch( qtype ) {
  case INT_QUESTION:
    int_answer = atoi( answer );
    iq = (int_q *)q;
    return iq->change( question, int_answer, iq );
    break;
  case STRING_QUESTION:
    sq = (string_q *)q;
    return sq->change( question, answer, sq );
    break;
  default:
    return -1;
    break;
  }
  return -1;
}


int login_question( char *question_index )
{
  int_q *iq;
  string_q *sq;
  int qindex;
  question_t *q = find_question( question_index, &qindex );

  if ( q == NULL )
    return -1;

  switch( q->type ) {
  case INT_QUESTION:
    iq = (int_q *)q;
    return iq->login( iq );
    break;
  case STRING_QUESTION:
    sq = (string_q *)q;
    return sq->login( sq );
    break;
  default:
    return -1;
    break;
  }
  return -1;
}


/*********************************
  
string question commands

**********************************/

int stringq_add( int qindex, char *question, char *answer, string_q *sq )
{
  // sq->add - must already be defined
  sq->change = stringq_change;
  sq->login = stringq_login;
  sq->index = qindex;
  sq->type = STRING_QUESTION;
  strcpy( sq->question, question ); 
  strcpy( sq->answer, answer ); 
  
  return 0;
}


int stringq_change( char *question, char *answer, string_q *sq )
{
  strcpy( sq->question, question ); 
  strcpy( sq->answer, answer ); 

  return 0;
}


int stringq_login( string_q *sq )
{
  char *input = NULL;
  int ret = -1;

  printf("Question: %s\nAnswer? \n", sq->question );
  fscanf( ifile, "%ms", &input );

  if ( strcmp( input, sq->answer ) == 0 ) {
    ret = 0;
  }
  else {
    ret = -1;
  }

  free(input);
  return ret;
}



/*********************************
  
int question commands

**********************************/

int intq_add ( int qindex, char *question, int answer, int_q *iq )
{
  // iq->add - must already be defined
  iq->change = intq_change;
  iq->login = intq_login;
  iq->index = qindex;
  iq->type = INT_QUESTION;
  strncpy( iq->question, question, STRING1 ); 
  iq->answer = answer;

  return 0;
}


int intq_change( char *question, int answer, int_q *iq )
{
  strncpy( iq->question, question, STRING1 ); 
  iq->answer = answer;

  return 0;
}


int intq_login( int_q *iq )
{
  int input;

  printf("Question: %s\nAnswer? \n", iq->question );
  fscanf( ifile, "%d", &input );

  if ( input == iq->answer ) {
    return 0;
  }

  return -1;
}



/******************************************************************************

Commands:

add <question_index> <question_type> <question> <answer(s)>
  Add a new question (<question_type> <question> <answer(s)>) of <question_index> 

change <question_index> <question> <answer(s)>
  Change question ( <question> <answer(s)>) for <question_index> 

*******************************************************************************/



/******************************************************************************

Function: main
Input Args: 
          argc - The number of command line arguments (3)
          argv - Command line arguments ( cs165-p3 <input_file> <output_file>

Main runs the sequence of commands listed in <input_file> and stores the 
results of the commands in <output_file>.  The output fprintf statements
are provided.

*******************************************************************************/

int main( int argc, char *argv[] )
{
	FILE *file;
	char *line = (char *)NULL;
	int cmd;
	char *cmdstr=NULL, *args=NULL;
	int res;
	size_t len; 
	char *qindex_str=NULL;
	
	// check usage
	if ( argc != 3 ) {
	  printf( "Main: Err: Incorrect usage: %s\n", USAGE );
	}

	// open fds
	if (( file = fopen( argv[1], "r" )) == NULL ) {
	  exit(-1);
	}

	ifile = file;

	if (( file = fopen( argv[2], "w+" )) == NULL ) {
	  exit(-1);
	}

	ofile = file;
	 
	// get input - line-by-line from the input file
	while (( res = getline( &line, &len, ifile )) >= 0 ) {
	  cmdstr = line;
	  
	  // determine command from command string (first arg in line)

	  cmd = find_command( cmdstr, strlen( cmdstr ));

	  if ( cmd < 0 ) {
	    fprintf( ofile, "ERR: invalid command line: %s", line );
	    continue;
	  }

	  if ( cmd == Q_SKIP ) {
	    continue;
	  }
	  
	  while ( (++cmdstr)[0] != ' ' );
	  cmdstr++;
	  sscanf( cmdstr, "%ms", &qindex_str );  

	  // get reference to args from cmdstr
	  args = cmdstr;
	  while ( (++args)[0] != ' ' );
	  args++;
	  // strip '\n' if present
	  int i;
	  for ( i = 0; i < strlen(args); i++ ) { if ( args[i] == '\n' ) { args[i] = '\0'; break; }}

	  // apply the command
	  res = apply_command( cmd, qindex_str, args );
	  
	  if ( res < 0 ) {
	    fprintf( ofile, "ERR: failed command: cmd index %d for line %s\n", cmd, line );
	    continue;
	  }
	}

	exit( 0 );
}



