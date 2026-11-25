/* George Delgado
 * gdelgado@pdx.edu
 * cs333
 * lab3
*/

#pragma once

#ifndef _REXTOP_H
#define _REXTOP_H

#define MONITOR 'm'
#define INTERVAL 'i'
#define COUNT 'c'
#define WIDTH 'w'
#define VERBOSE 'v'
#define HELP 'h'
#define OPTIONS "i:c:w:vhm"

#define QUIT_APP 'q'
#define INVALID_CMD_OPTION 0

//add defualt values
#define DEFAULT_NUM_PROC 25 
#define DEFAULT_INTERVAL 2 //seconds
#define DEFAULT_COUNT 0 //don't stop
#define DEFAULT_WIDTH 100

void help(char *);
#endif // _REXTOP_H
