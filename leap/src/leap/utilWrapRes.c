/*
 *	File:	utilWrapRes.c
 *
 ************************************************************************
 *                            LEAP                                      *
 *                                                                      *
 *                   Copyright (c) 1992, 1995                           *
 *           Regents of the University of California                    *
 *                     All Rights Reserved.                             *
 *                                                                      *
 *  This software provided pursuant to a license agreement containing   *
 *  restrictions on its disclosure, duplication, and use. This software *
 *  contains confidential and proprietary information, and may not be   *
 *  extracted or distributed, in whole or in part, for any purpose      *
 *  whatsoever, without the express written permission of the authors.  *
 *  This notice, and the associated author list, must be attached to    *
 *  all copies, or extracts, of this software. Any additional           *
 *  restrictions set forth in the license agreement also apply to this  *
 *  software.                                                           *
 ************************************************************************
 *                                                                      *
 *     Designed by:    Christian Schafmeister                           *
 *     Author:         Christian Schafmeister                           *
 *                                                                      *
 *     VERSION: 1.0                                                     *
 *     Programmers:                                                     *
 *             Christian Schafmeister                                   *
 *             David Rivkin                                             *
 *                                                                      *
 *     Principal Investigator: Peter A. Kollman                         *
 *                                                                      *
 ************************************************************************
 *
 *	Description:
 *		This utility reads an X-Windows resource
 *		file and wraps it within a C file that
 *		defines the resources within the resource
 *		file in an array of Strings.
 */



#include	"basics.h"


#define	MAX_LONG_STRING	2000

typedef	char	LONGSTRING[MAX_LONG_STRING];




/*
 *	ClipSpaces
 *
 *	Author:	Christian Schafmeister (1991)
 *
 *	Clip spaces from the front and back of the string.
 */
void	ClipSpaces( sLine )
LONGSTRING		sLine;
{
int		iStart, iStop, i;


    for ( i=0; i<strlen(sLine); i++ ) {
	if ( sLine[i] != ' ' && sLine[i] != '\t' ) {
	    iStart = i;
	    break;
	}
    }

    if ( iStart == 0 ) return;
    sLine[0] = ' ';
    for ( i=0; i<strlen(sLine)-iStart+1; i++ ) {
	sLine[i+1] = sLine[i+iStart];
    }

}

	
/*
 *	bReadResource
 *
 *	Author:	Christian Schafmeister (1991)
 *
 *	Read a single resource from the input file.
 *	Return FALSE if there are no more.
 */
BOOL	bReadResource( fIn, sResource )
FILE*		fIn;
LONGSTRING	sResource;
{
LONGSTRING	sLine;
int		i, j;
BOOL		bOnlySpaces;


    strcpy( sResource, "" );
    do {
	fgets( sLine, MAX_LONG_STRING, fIn );
	if ( feof(fIn) ) return(FALSE);

        bOnlySpaces = TRUE;

		/* Look for comment characters */
	for ( i=0; i<strlen(sLine); i++ ) {

	    if ( sLine[i] == '\n' ) {
		sLine[i] = '\0';
		break;
	    }
	    if ( sLine[i] == '!' ) {
		sLine[i] = '\0';
		break;
	    }

	    if ( sLine[i] != ' ' && sLine[i] != '\t' ) bOnlySpaces = FALSE;

		/* Insert '\\' infront of every double quote */
	    if ( sLine[i] == '"' ) {
		for ( j=strlen(sLine); j>=i; j-- ) sLine[j+1] = sLine[j];
		sLine[i] = '\\';
		i++;
	    }
	}
	if ( strlen(sLine) == 0 ) continue;
	if ( bOnlySpaces ) continue;

		/* concat the current line to the current resource def */

	ClipSpaces( sLine );
	strcat( sResource, sLine );

	if ( sResource[strlen(sResource)-1] !='\\' ) break;
	sResource[strlen(sResource)-1] = '\0';

    } while ( 1==1 );

    return(TRUE);
}





void main( argc, argv )
int		argc;
char*		argv[];
{
FILE*		fIn;
FILE*		fOut;
LONGSTRING	sResource;
BOOL		bFirst;


    BasicsInitialize();

    if ( argc != 3 ) {
	printf( "Usage: %s {resourcefile_in} {header_out}\n",
		argv[0] );
	exit(1);
    }

    fIn = fopen( argv[1], "r" );
    fOut = fopen( argv[2], "w" );

    fprintf( fOut, "/*\n" );
    fprintf( fOut, " *    File: %s\n", argv[2] );
    fprintf( fOut, " *\n" );
    fprintf( fOut, " *    Description:\n" );
    fprintf( fOut, " *        Header file generated by utilWrapRes.\n" );
    fprintf( fOut, " *        Description of resource file is in NULL terminated\n" ); 
    fprintf( fOut, " *        String    GsaResources[]\n" );
    fprintf( fOut, " */\n" );

    fprintf( fOut, "\n\n\n" );
    fprintf( fOut, "String	GsaResources[] = {\n" );
   
    bFirst = TRUE; 
    while ( bReadResource( fIn, sResource ) ) {
	fprintf( fOut, "\"%s\", \n", sResource );
    }
    fprintf( fOut, "NULL };\n\n\n" );

    fclose(fIn);
    fclose(fOut); 
}
