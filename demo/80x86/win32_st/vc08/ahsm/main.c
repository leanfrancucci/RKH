/*
 * main.c
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "rkh.h"
#include "my.h"
#include "myevt.h"
#include "rkhdata.h"
#include "rkhassert.h"


#define tostring( expr )		#expr
#define ESC						0x1B
#define forever					for(;;)
#define kbmap( c )				( c - '0' )

#ifdef __VC__
	#include <conio.h>
	#define mygetch()			getch()
#elif __LNXGCC__
	#define mygetch()			gnu_getch()
#endif


static char fmt[ 64 ];
static MYEVT_T mye;
FILE *fdbg;


static const char *tremap[] =
{
	tostring( RKHTR_EVENT 		),
	tostring( RKHTR_TRN_SRC		),
	tostring( RKHTR_TRN_TGT		),
	tostring( RKHTR_NXT_STATE	),
	tostring( RKHTR_INT_TRAN	),
	tostring( RKHTR_ENTRY		),
	tostring( RKHTR_EXIT		),
	tostring( RKHTR_INIT_HSM	),
	tostring( RKHTR_SGT_TGT		),
	tostring( RKHTR_RTN_CODE	),
	tostring( RKHTR_NUM_ENEX	),
	tostring( RKHTR_NUM_ACTSGT	)
};


static const char *smmap[] =
{
	tostring( MY )
};


static const char *rcmap[] =
{
	tostring( RKH_OK 						),
	tostring( RKH_INPUT_NOT_FOUND			),
	tostring( RKH_CONDITIONAL_NOT_FOUND		),
	tostring( RKH_CONDITION_NOT_FOUND		),
	tostring( RKH_GUARD_FALSE				),
	tostring( RKH_UNKNOWN_STATE				),
	tostring( RKH_EXCEED_HCAL_LEVEL			),
	tostring( RKH_EXCEED_TR_SEGS 			)
};


static
char *
format_trevt_args( RKHTREVT_T *ptre )
{
	switch( ptre->id )
	{
		case RKHTR_INIT_HSM:
			sprintf( fmt, "is = %s [%d]", ptre->sb, ptre->num );
			break;
		case RKHTR_INT_TRAN:
			sprintf( fmt, "" );
			break;
		case RKHTR_SGT_TGT:
		case RKHTR_TRN_SRC:
		case RKHTR_TRN_TGT:
		case RKHTR_NXT_STATE:
		case RKHTR_ENTRY:
		case RKHTR_EXIT:
			sprintf( fmt, "%s [%d]", ptre->sb, ptre->num );
			break;
		case RKHTR_RTN_CODE:
			sprintf( fmt, "%s", rcmap[ ptre->num ] );
			break;
		case RKHTR_NUM_ENEX:
		case RKHTR_NUM_ACTSGT:
			sprintf( fmt, "%d - %d", ( ( ptre->num ) >> 4 ) & 0x0F, 
										( ptre->num & 0x0F  ) );
			break;
		case RKHTR_EVENT:
			sprintf( fmt, "%d", ptre->num );
			break;
		default:
			return NULL;
	}
	return fmt;
}


static
void
print_banner( void )
{
	printf( "Demo description: \n\n" );
	printf( "The goal of this demo application is to explain how to \n" );
	printf( "represent a state machine using the RKH framework. To do \n" );
	printf( "that is proposed a simple and abstract example, which is \n" );
	printf( "shown in the documentation file Figure 1 section \n" );
	printf( "\"Representing a State Machine\". \n\n\n" );

	printf( "1.- Press <numbers> to send events to state machine. \n" );
	printf( "2.- Press 'p' to see related information about transitions, \n" );
	printf( "    state changes, and so on.\n" );
	printf( "3.- Press 'r' to reset state machine.\n" );
	printf( "4.- Press ESC to quit \n\n\n" );
}


void 
rkh_trace_open( void )
{
	RKHTRCFG_T *pcfg;

	rkh_trinit();
	rkh_trconfig( MY, RKH_TRLOG, RKH_TRPRINT );
	rkh_trcontrol( MY, RKH_TRSTART );

	if( ( fdbg = fopen( "../mylog.txt", "w+" ) ) == NULL )
	{
		perror( "Can't open file\n" );
		exit( EXIT_FAILURE );
	}

	fprintf( fdbg, "---- RKH trace log session - "__DATE__" - "__TIME__" ----\n\n" );
	
	pcfg = rkh_trgetcfg( MY );
	if( pcfg->print == RKH_TRPRINT )
		printf( "---- RKH trace log session - "__DATE__" - "__TIME__" ----\n\n" );
}


void 
rkh_trace_close( void )
{
	fclose( fdbg );
}


RKHTS_T 
rkh_trace_getts( void )
{
	return ( RKHTS_T )clock();
}


void 
rkh_trace_flush( void )
{
	RKHTREVT_T te;
	RKHTRCFG_T *pcfg;

	while( rkh_trgetnext( &te ) != RKH_TREMPTY )
	{
		pcfg = rkh_trgetcfg( te.smix );

		if( pcfg->log == RKH_TRLOG )
			fprintf( fdbg, "%05d [ %-16s ] - %s : %s\n",
													rkh_trgetts(),
													tremap[ te.id ],
													smmap[ te.smix ],
													format_trevt_args( &te ) );
		if( pcfg->print == RKH_TRPRINT )
			printf( "%05d [ %-16s ] - %s : %s\n",
													rkh_trgetts(),
													tremap[ te.id ],
													smmap[ te.smix ],
													format_trevt_args( &te ) );
	}
}


void 
rkh_assert( rkhrom char * const file, HUInt fnum, int line )
{
	printf( "RKHASSERT: [%d] line from %s file (#%02d)", line, file, fnum );
	__debugbreak();
}


int
main( void )
{
	int c;

	rkh_init_hsm( my );
	srand( ( unsigned )time( NULL ) );

	print_banner();
	rkh_trace_open();

	forever
	{
		c = mygetch();
		
		if( c == 'p' )
			rkh_trace_flush();
		else if ( c == ESC )
			break;
		else if ( c == 'r' )
			rkh_init_hsm( my );
		else
		{
			rkh_set_static_event( &mye, kbmap( c ) );
			mye.ts = ( rkhuint16 )rand();
			rkh_engine( my, ( RKHEVT_T* )&mye );
		}
	}

	rkh_trace_close();
}