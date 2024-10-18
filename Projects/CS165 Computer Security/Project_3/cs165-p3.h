#define Q_ADD 0
#define Q_REMOVE 1
#define Q_CHANGE 2
#define Q_LINK 4
#define Q_LOGIN 5
#define Q_NEW 6
#define Q_SKIP 7

#define INT_QUESTION 0
#define STRING_QUESTION 1

#define MAX_QUESTIONS 10
#define MAX_LINE 100
#define QNEEDED 1

// To be provided by the TA
#define STRING1 28
#define STRING2 27


// fixed
#define ANS_STRING 30
#define USAGE "cs165-p3 cmd-file output-file"


typedef struct question {
	int index;
        int type;
} question_t;

typedef struct int_question {
	int index;
        int type;
        char question[STRING1];
	int answer;
        int (*add) ( int qindex, char *question, int answer, struct int_question *sq );
        int (*change) ( char *question, int answer, struct int_question *sq );
        int (*login) ( struct int_question *iq );
} int_q; 

typedef struct string_question {
	int index;
        int type;
        char question[STRING2];
	char answer[ANS_STRING];
        int (*add) ( int qindex, char *question, char *answer, struct string_question *sq );
        int (*change) ( char *question, char *answer, struct string_question *sq );
	int (*login) ( struct string_question *sq );
} string_q; 


typedef struct system {
        int qct;
        question_t *questions[MAX_QUESTIONS];
} system_t;



#define SPACE_CHAR( str,i ) \
	(( (str)[(i)] == ' ' ) || ( (str)[(i)] == '\t' ) || ( (str)[(i)] == '\n' )) 


static inline int strct( char *str, int len )
{
  int ct = 0;
  int instr = 0;
  int i;

  for ( i = 0; i < len; i++ ) {
    if (( !SPACE_CHAR( str, i )) && instr );
    else if (( !SPACE_CHAR( str, i )) && !instr ) { instr = 1, ct++; }
    else if ( SPACE_CHAR( str, i )) { instr = 0; }
    else { return -1; }
  }

  return ct;
}

/********************

major functions for processing

*********************/

extern question_t *find_question( char *question_index, int *qindex );
extern question_t *add_question( char *question_index, char *question_type, char *question, char* answer );
extern int change_question( char *question_index, char *question, char *answer );
extern int login_question( char *question_index );

/********************

externs for function pointer implementations 

*********************/

// questions
extern int intq_add( int qindex, char *question, int answer, int_q *iq );
extern int intq_change( char *question, int answer, int_q *iq );
extern int intq_login( int_q *iq );
extern int stringq_add( int qindex, char *question, char *answer, string_q *sq );
extern int stringq_change( char *question, char *answer, string_q *sq );
extern int stringq_login( string_q *sq );
