/* player.c */

#include "player.h"
#include "objs.h"
#include "bool.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/** Inits the game
  * @param player The player structure
  */
void init_game(Player * player)
{
	init_cmds();
	init_locs();
	init_objs();

	player->num_loc = 0;
	player->num_turns = 0;
}

/** Ends the game */
void end_game(Player * player)
{
    print( "Introduces la tarjeta en su ranura, y el ascensor se pone en "
           "marcha con un gran estruendo. Es asombroso que la maquinaria siga "
           "funcionando.\n" );
    print( "Tras unos instantes, que se han hecho eternos, llegas arriba." );
    print( "Caminas por entre las ruinas de la base, con innumerables preguntas "
           "sobre lo que has descubierto en sus muelles secretos.\n" );
    input( PROMPT_WAIT );
    print( "\nA punto de salir de las ruinas del edificio principal, descubres que se "
           "acercan varias furgonetas negras.\n" );
    input( PROMPT_WAIT );
    print( "\nDecides tomar un rodeo. No sabes lo que ha pasado, pero desde luego no te "
           "vas a quedar a hacer preguntas.\n" );
    print( "Fin.\n" );

    if ( !ask_yes_no( "Volver a jugar (s/n): " ) ) {
        cls();
        exit( 0 );
    } else {
        restart( player );
    }
}

/** Asks yes o no to the user, given a message
    @param msg The message to show to the user.
    @return true if the first letter of the user's reply is 's' or 'y'
*/
bool ask_yes_no(const char * msg)
{
    char answer = 'n';
    static char buffer[3];

    printf( msg );
    fgets( buffer, 2, stdin );
    answer = tolower( buffer[ 0 ] );

    return ( answer == 's' || answer == 'y' );
}

/** Executed just after describing the current loc
  * @param player The player structure
  */
void proc1(Player * player)
{
    static bool isSubDesc = false;
    Obj * obj_aletas = locate_obj_by_id( "aletas" );
    Obj * obj_tarjeta = locate_obj_by_id( "tarjeta" );

	// You take the sea with you (yeah!)
	if ( player->num_loc <= 7 ) {
        Obj * obj_mar = locate_obj_by_id( "mar" );

		obj_mar->num_loc = player->num_loc;
	}

	// Initial message about the submarine
	if ( !isSubDesc
	  && player->num_loc == 7 )
	{
		print( "Efectivamente, se trata de un submarino de mediados del "
					"siglo pasado.\n" );
		isSubDesc = true;
	}

	// You leave your equipment in the first loc of the base
	if ( player->num_loc == 2
	  && obj_aletas->num_loc == PLAYER_NUM_LOC )
	{
		print( "Incapaz de utilizarlas caminando, dejas las aletas en el suelo." );
		obj_aletas->num_loc = 2;
	}

    // Move office furniture when needed.
    if ( player->num_loc == 16
      || player->num_loc == 17 )
    {
        Obj * obj_silla = locate_obj_by_id( "silla" );
        Obj * obj_mesa = locate_obj_by_id( "mesa" );
        Obj * obj_archivador = locate_obj_by_id( "archivador" );

        obj_silla->num_loc = player->num_loc;
        obj_mesa->num_loc = player->num_loc;
        obj_archivador->num_loc = player->num_loc;
    }

    // Move dust around
    if ( player->num_loc >= 9 ) {
        locate_obj_by_id( "polvo" )->num_loc = player->num_loc;
    }

    // Move shelves
    if ( player->num_loc == 18
      || player->num_loc == 19 )
    {
        Obj * obj_baldas = locate_obj_by_id( "baldas" );
        obj_baldas->num_loc = player->num_loc;
    }

    // End of game?
    if ( player->num_loc == 10
      && obj_tarjeta->num_loc == PLAYER_NUM_LOC )
    {
        end_game( player );
    }
}

/** Executed after a successful command
  * @param player The player structure
  */
void proc2(Player * player)
{
	if ( player->num_turns == 3 ) {
		printf( "\n" );
		print(
			"Repentinamente, notas una irrefrenable fuerza que te succiona... "
			"El agua te arrastra hacia las rocas... pero en lugar de rocas, "
			"encuentras algo raro: una apertura, una compuerta que se ha "
			"abierto a un lugar escondido en las profundidades."
		);
		printf( "\n" );
		input( PROMPT_WAIT );
		cls();
		print(
			"Tras unos interminables minutos, has acabado dentro de una gigante "
			"estructura. Como si fuera un hangar para aviones, pero con agua. "
			"Algo parecido a un muelle se encuentra frente a ti, y "
			"decides abandonar el agua usando una escalerilla."
		);
		printf( "\n" );
		player->num_loc = 2;
	}
}

void resp(Player * player, Order * order)
{
    static bool caja_abierta = false;
    bool intercepted = false;

    Obj * obj_lancha = locate_obj_by_id( "lancha" );
    Obj * obj_aletas = locate_obj_by_id( "aletas" );
    Obj * obj_escotilla = locate_obj_by_id( "escotilla" );
    Obj * obj_mesa = locate_obj_by_id( "mesa" );
    Obj * obj_docs = locate_obj_by_id( "documentos" );
    Obj * obj_archivador = locate_obj_by_id( "archivador" );
    Obj * obj_llave = locate_obj_by_id( "llave" );
    Obj * obj_martillo = locate_obj_by_id( "martillo" );
    Obj * obj_baldas = locate_obj_by_id( "baldas" );
    Obj * obj_caja = locate_obj_by_id( "caja" );
    Obj * obj_botonera = locate_obj_by_id( "botonera" );

    // Quitate el traje
    if ( order->cmd->cmdId == Cmd_Disrobe
      && !strcmp( order->obj1->id, "traje" ) )
    {
        intercepted = true;
        print( "No es buena idea, teniendo en cuenta las bajas temperaturas." );
        return;
    }

    // Deja o quitate las aletas
    if ( ( order->cmd->cmdId == Cmd_Disrobe
	|| order->cmd->cmdId == Cmd_Drop )
      && order->obj1 == obj_aletas )
    {
        intercepted = true;
        print( "No, las necesitas para nadar." );
        return;
    }

    // Coge las aletas
    if ( order->cmd->cmdId == Cmd_Take
      && order->obj1 == obj_aletas
      && order->obj1->num_loc == 2 )
    {
        intercepted = true;
        print( "Ahora ya son inservibles: no puedes usarlas caminando." );
        return;
    }

    // Intenta subir
    if ( order->cmd->cmdId == Cmd_Up
      && player->num_loc == 5 )
    {
        if ( obj_lancha->num_loc != 5 ) {
            intercepted = true;
            print( "No, no, te vas a cortar los pies." );
            return;
        } else {
            print( "Subiendo por encima de la lancha, por fin asciendes por la escalerilla." );
        }
    }

    // Deja la lancha
    if ( order->cmd->cmdId == Cmd_Drop
      && player->num_loc == 5
      && order->obj1 == obj_lancha )
    {
        print( "Tratas de colocar la lancha tapando los cristales..." );
    }

    // Examinas la escotilla del submarino
    if ( order->cmd->cmdId == Cmd_Examine
      && player->num_loc == 7
      && order->obj1 == obj_escotilla
      && obj_lancha->num_loc == LIMBO )
    {
        intercepted = true;
        print( "Abres la escotilla, que resulta ser un panel, y de su "
               "interior extraes una reducida lancha de salvamento." );
        obj_lancha->num_loc = 7;
        return;
    }

    // Examinar la mesa del despacho del general
    if ( player->num_loc == 16
      && order->cmd->cmdId == Cmd_Examine
      && order->obj1 == obj_mesa )
    {
        print( "Sobre ella, hay unos documentos." );
        intercepted = true;
    }

    // Intentar coger los papeles
    if ( player->num_loc == 16
      && order->cmd->cmdId == Cmd_Take
      && order->obj1 == obj_docs )
    {
        print( "Prefiero no cogerlos, parecen demasiado importantes." );
        intercepted = true;
    }

    // Hacer aparecer la llave en el despacho del secre
    if ( player->num_loc == 17
      && order->cmd->cmdId == Cmd_Examine
      && order->obj1 == obj_archivador
      && obj_llave->num_loc == LIMBO )
    {
        print( "Encima del archivador, encuentras una llave." );
        obj_llave->num_loc = 17;
        intercepted = true;
    }

    if ( player->num_loc == 23
      && order->cmd->cmdId == Cmd_West )
    {
        if ( obj_llave->num_loc != PLAYER_NUM_LOC ) {
            print( "Cerrada. Necesitas una llave." );
            intercepted = true;
        } else {
            print( "Abres la puerta..." );
        }
    }

    // Hacer aparecer el martillo en el archivo
    if ( player->num_loc == 19
      && order->cmd->cmdId == Cmd_Examine
      && order->obj1 == obj_baldas
      && obj_martillo->num_loc == LIMBO )
    {
        intercepted = true;
        obj_martillo->num_loc = 19;
        print( "Has encontrado un martillo encima de una balda." );
    }

    // Pulsa en el ascensor
    if ( player->num_loc == 10
      && order->cmd->cmdId == Cmd_Push
      && order->obj1 == obj_botonera )
    {
        intercepted = true;
        print( "No sucede nada. Hay una ranura debajo... es necesario meter algo." );
    }

    // Rompe la caja
    if ( player->num_loc == 18
      && order->cmd->cmdId == Cmd_Break
      && order->obj1 == obj_caja )
    {
        intercepted = true;

        if ( !caja_abierta ) {
            if ( obj_martillo->num_loc != PLAYER_NUM_LOC ) {
                print( "No puedes hacerlo con las manos desnudas." );
            } else {
                caja_abierta = true;
                print( "Rompes la caja a golpes, desparramando su contenido." );
            }
        } else {
            print( "Ya hay bastante estropicio..." );
        }
    }

    // Examina la caja
    if ( player->num_loc == 18
      && order->cmd->cmdId == Cmd_Examine
      && order->obj1 == obj_caja )
    {
        Obj * obj_tarjeta = locate_obj_by_id( "tarjeta" );


        if ( caja_abierta ) {
            intercepted = true;
            print( obj_caja->desc );
            print( "Su contenido se desparrama en derredor." );

            if ( obj_tarjeta->num_loc == LIMBO ) {
                print( "Hay muchos papeles, y piezas, pero no ves nada de "
                        "utilidad real... excepto una tarjeta." );
                obj_tarjeta->num_loc = 18;
            }
        }
    }


    if ( !intercepted ) {
        order->cmd->doIt( player, order );
    }
}

