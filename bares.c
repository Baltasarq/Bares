#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>

#include "player.h"
#include "cmds.h"
#include "locs.h"
#include "objs.h"
#include "bool.h"

/* Constants */
const int MAX_COL = 64;
const char * PROMPT = ":> ";
const char * PROMPT_WAIT = "Pulsa ENTER...";

/* Fns */
void cls();
char * input(const char *);
void print(const char * txt);
Order * parse(char * buffer);

void play_intro()
{
  cls();
  printf( "Bares v1.0 20160424\tBaltasar el arquero\n"
          "Teclea \"ayuda\" para conocer los posibles comandos.\n\n\n\n\n\n" );
  print( "Un perfecto tiempo de descanso en las profundidades de la estaca "
          "de Bares. Buceando, disfrutando del momento de paz y sosiego que "
          "tan solo las bajas temperaturas a diez metros bajo la superficie "
          "del mar pueden ofrecerte.\n\n"
  );
  input( PROMPT_WAIT );
}

void restart(Player * player)
{
    init_game( player );
    play_intro();
    cls();
}

void do_loc_desc(int num_loc)
{
	int num_objs = how_many_objs_in( num_loc );

	// Show loc desc
	printf( "\n\n" );
    print( locs[ num_loc ].desc );

	// Show objects here
	if ( num_objs > 0 ) {
		printf( "\nPuedes ver:" );
		num_objs = list_objs_in( num_loc );
	}

	printf( "\n\n" );
}

int main()
{
	char * buffer;
	Player player;
	Order * order;

	init_game( &player );
	play_intro();

	// Main loop
	cls();
	do {
		// Pro1
        set_default_colors();
		do_loc_desc( player->num_loc );
		proc1( &player );
        //draw(1,1,100,100);

		do {
			buffer = input( PROMPT );
		} while( strlen( buffer ) == 0 );

		// Parse order
        set_answer_colors();
		order = parse( buffer );
        resp( &player, order );

		// Proc2
		if ( order->cmd != cmdNop ) {
		  player.num_turns += 1;
		  proc2( &player );
		}
	} while( order->cmd.cmdId != Cmd_End );

	return EXIT_SUCCESS;
}

void set_default_colors()
{
    textbackground( WHITE );
    textcolor( BLUE );
}

void set_answer_colors()
{
    textbackground( WHITE );
    textcolor( BLACK );
}

/** Cleans the screen and sets the appropriate colors */
void cls() {
  clrscr();
  set_default_colors();
}

/** Takes an order from the user
  * @param msg The message to show to the user.
  * @return A pointer to the char[] buffer */
char * input(const char * msg)
{
	char * last_char = NULL;
	static char buffer[32];

	// Prompt
	textbackground( BLUE );
	textcolor( YELLOW );
	cprintf( "%s", msg );
	fflush( stdout );

	// Get order
	set_default_colors();
    textcolor( RED );
	printf( " " );
	fgets( buffer, 32, stdin );

	// Remove last '\n'
	last_char = buffer + ( strlen( buffer ) - 1 );

	if ( *last_char == '\n' ) {
		*last_char = 0;
	}

    set_default_colors();
	return buffer;
}

/** Prints a (possibly) long text, taking max columns into account
  * @param txt The text to print, as a char *
  */
void print(const char * txt)
{
  int margin = MAX_COL - 10;
  char * ptr = txt;
  bool look_for_cr = false;

  while ( *ptr != '\0' )
  {
    if ( look_for_cr
      && ( *ptr == ' '
        || *ptr == '\n' ) )
    {
      *ptr = '\n';
      look_for_cr = false;
    }
    else
    if ( ptr == '\n' ) {
        txt = ptr + 1;
    } else {
      int pos = ptr - txt;

      if ( pos > 0
        && pos % margin == 0 )
      {
        look_for_cr = true;
      }
    }

    ++ptr;
  }

  printf( "%s\n", txt );
}

bool is_word_in_syns(char * word, char * syns)
{
  static char search_word[LENGTH_WORD + 2];
  uint word_length = strlen( word );

  // Build search word
  *search_word = ' ';
  strcpy( search_word + 1, word );
  *( search_word + word_length + 1 ) = ' ';
  *( search_word + word_length + 2 ) = 0;

  // Determine if included
  //printf( "'%s' in '%s'%c", search_word, syns, 13 );
  return ( strstr( syns, search_word ) != NULL );
}

bool is_word_ignorable(char * word)
{
  static const char * ignorable_words =
      " un una unos unas el la los las "
      "y tambien luego "
      "a al ante bajo cabe con contra de desde en "
      "entre hacia hasta para por segun "
      "sin so sobre tras ";

  return is_word_in_syns( word, ignorable_words );
}

void assign_word(Order * order, char * word)
{
  /* Truncate word, if needed */
  if ( strlen( word ) > LENGTH_WORD ) {
    *( word + LENGTH_WORD ) = 0;
  }

  /* Assign word to the appropriate slot in the order */
  if ( order->word2 == NULL ) {                   // Still something to assign?
    if ( !is_word_ignorable( word ) ) {
      // Store word's address in Order's pointers
      if ( order->verb == NULL ) {
        order->verb = word;
      } else {
        if ( order->word1 == NULL ) {
          order->word1 = word;
        } else {
          order->word2 = word;
        }
      }
    }
  }

  return;
}

Cmd * find_cmd(char * word)
{
    Cmd * toret = NULL;
    int i = 0;

    if ( word != NULL
      && strlen( word ) > 0 )
    {
        // Find command
        for (; i < NUM_CMDS; ++i) {
            if ( is_word_in_syns( word, cmds[i].words ) ) {
                break;
            }
        }

        if ( i < NUM_CMDS ) {
          toret = &cmds[ i ];
        } else {
          toret = cmdNop;
        }
    }

    return toret;
}

Obj * find_obj(char * word)
{
	int i = 0;
	Obj * toret = NULL;

    if ( word != NULL
      && strlen( word ) > 0 )
    {
        for(; i < NUM_OBJS; ++i) {
            if ( is_word_in_syns( word, objs[ i ].words ) ) {
                toret = &objs[ i ];
                break;
            }
        }
    }

	return toret;
}

void find_words(Order * order)
{
    order->cmd = find_cmd( order->verb );
    order->obj1 = find_obj( order->word1 );
    order->obj2 = find_obj( order->word2 );
}

Order * parse(char * buffer)
{
	static Order toret;
	char * p = buffer;
	char * word = buffer;

	/* Init order */
	memset( &toret, 0, sizeof( toret ) );
	toret.cmd = cmdNop;

	/* Get to the input */
	while ( isspace( *p ) ) {
		++p;
		++word;
	}

	/* Run all over the input */
	while ( *p != 0
	   && toret.word2 == NULL )
	{
		if ( *p == ' ' ) {
			*p = 0;
			assign_word( &toret, word );
			word = p + 1;
		} else {
			*p = tolower( *p );
		}

		++p;
	}

	// Last word
	*p = 0;
	assign_word( &toret, word );

	find_words( &toret );

//	printf( "Verb: '%s', Word1: '%s', Word2: '%s'\n",
//				toret.verb, toret.word1, toret.word2 );
//	printf( "Cmd: %d, Obj1: %d, Obj2: %d\n",
//				toret.cmd->cmdId, toret.obj1, toret.obj2 );
//	if ( toret.obj1 != NULL ) {
//		printf( "Objeto: '%s', loc: %d, desc: '%s'\n",
//			toret.obj1->id, toret.obj1->num_loc, toret.obj1->desc );
//	}

	return &toret;
}
