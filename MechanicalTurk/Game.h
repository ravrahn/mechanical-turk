
/*
 *  Game.h
 *  1917 2013 ver 0.0
 *  Do not alter this file
 *
 *  Created by Richard Buckland on 2/2/2013
 *  Licensed under Creative Commons SA-BY-NC 3.0.
 *
 */


// player ID of each university
#define UNI_A 0
#define UNI_B 1
#define UNI_C 2

#define NUM_UNIS 3

// contents of an ARC
#define VACANT_ARC 0
#define ARC_A 1
#define ARC_B 2
#define ARC_C 3

// contents of a VERTEX
#define VACANT_VERTEX 0
#define CAMPUS_A 1
#define CAMPUS_B 2
#define CAMPUS_C 3
#define GO8_CAMPUS_A 4
#define GO8_CAMPUS_B 5
#define GO8_CAMPUS_C 6

// action codes
#define PASS 0
#define BUILD_CAMPUS 1
#define BUILD_GO8 2
#define CREATE_ARC 3
#define START_SPINOFF 4
#define OBTAIN_PUBLICATION 5
#define OBTAIN_IP_PATENT 6
#define RETRAIN_STUDENTS 7
#define RESERVED_1 8
#define RESERVED_2 9

#define MAX_LEGAL_ACTION RESERVED_2

// degree types
#define STUDENT_THD 0
#define STUDENT_BPS 1
#define STUDENT_BQN 2
#define STUDENT_MJ  3
#define STUDENT_MTV 4
#define STUDENT_MMONEY 5

#define NUM_REGIONS 19

#define TRUE 1
#define FALSE 0

// the Game types
// each region is specified by a pair of co-ordinates
// the centre of the board is at 0,0
// the x co-ordinate specifies the column,
// the y co-ordinate specifies the row if each column was shifted up
// half a hexagon relative to the column on its left
typedef struct _region {
   int x;
   int y;
} region;

// two adjacent regions specifies an arc
typedef struct _arc {
   region region0;
   region region1;
} arc;

// three adjacent regions specifies a vertex
typedef struct _vertex {
   region region0;
   region region1;
   region region2;
} vertex;


typedef int degree;
typedef int uni;

// actions are what the player AI returns.
// The playGame function will ask the current player which action they
// wish to perform next, then it will check that this is a legal action
// in the current state of the Game (using isLegalAction()), then it
// will perform that action for the player (using makeAction()), then
// it will ask the same player for another action, and repeat this
// sequence over and over again until the player eventually returns
// PASS at which time the playGame function will throw the dice
// (using throwDice()) and play will advance to the next player.
// And repeat.
//
typedef struct _action {
   int actionCode;  // see #defines above
   vertex targetVertex; // if the action operates on a vertex or ARC
   arc    targetARC;    // these fields specify *which* vertex or ARC
   degree retrainFrom;  // used for the retrain students action
   degree retrainTo;    // used for the retrain students action
} action;


typedef struct _game *Game;
// the Game type is a pointer to a game struct
// the game struct is defined by you in game.c

/* **** Functions which change the Game aka SETTERS **** */
// given a Game this function makes it a new Game, resetting everything and
// setting the the type of degree produced by each
// region, and the value on the dice discs in each region.
// note: each array must be NUM_REGIONS long
// data passed into the arrays is for the non-sea regions.
// the first entries relate to the regions with the smallest
// x co-ordinate (in increasing order of y-ordinate) and then the
// next smallest x-ordinates, and so on.
// eg for the badly drawn map the regions would be specified in the
// order [(-2,0),(-2,1),(-2,2),(-1,-1,),(-1,0)....(2,-1),(2,0)]
//
// this function must set up the Game to be in the initial state.
// eg if you are using my sample Game struct in Game.c this function
// would need to set the field currentTurn to -1.  (because the turn
// number is -1 at the start of the Game)
//
Game newGame (int degree[], int dice[]);

// after week 8 we will talk about implementing this. for now
// you can have it doing nothing
void disposeGame (Game g);

// make the specified action for the current player and update the
// Game state accordingly.
// The function may assume that the action requested is legal.
// START_SPINOFF is not a legal action here
void makeAction (Game g, action a);

// advance the Game to the next turn,
// assuming that the dice has just been rolled and produced diceScore
// the Game starts in turn -1 (we call this state "Terra Nullis") and
// moves to turn 0 as soon as the first dice is thrown.
void throwDice (Game g, int diceScore);

/* **** Functions which GET data about the Game aka GETTERS **** */

// true if the region is not one of the land regions of knowledge island
int isSea (Game g, region r);

// what type of students are produced by the specified land region?
// see degree discipline codes above
degree getDegree (Game g, region r);

// what dice value produces students in the specified land region?
// 2..12
int getDiceValue (Game g, region r);

// which university currently has the prestige award for the most ARCs?
// this deemed to be UNI_C at the start of the Game.
uni getMostARCs (Game g);

// which university currently has the prestige award for the most pubs?
// this is deemed to be UNI_C at the start of the Game.
uni getMostPublications (Game g);

// return the current turn number of the Game -1,0,1, ..
int getTurnNumber (Game g);

// return the player id of the player whose turn it is
// the result of this function is UNI_C during Terra Nullis
uni getWhoseTurn (Game g);

// return the contents of the given vertex (ie campus code or
// VACANT_VERTEX)
int getCampus (Game g, vertex v);

// the contents of the given edge (ie ARC code or vacent ARC)
int getARC (Game g, arc a);

// returns TRUE if it is legal for the current player
// to make the specified move, FALSE otherwise.
// legal means everything is legal eg when placing a campus consider
// such things as:
//   * is the specified vertex a valid land vertex?
//   * is it vacant?
//   * under the rules of the Game is the current player allowed to
//     place a campus at that vertex?  (eg is it adjacent to one of
//     their ARCs?)
//   * does the player have the 4 specific students required to pay for
//     that campus?
// It is not legal to make any action during Terra Nullis ie
// before the Game has started.
// It is not legal for a player to make the moves OBTAIN_PUBLICATION
// or OBTAIN_IP_PATENT (they can make the move START_SPINOFF)
//
int isLegalAction (Game g, action a);

// --- get data about a specified player ---

// return the number of KPI points the specified player currently has
int getKPIpoints (Game g, uni player);

// return the number of ARC grants the specified player currently has
int getARCs (Game g, uni player);

// return the number of GO8 campuses the specified player currently has
int getGO8s (Game g, uni player);

// return the number of normal Campuses the specified player currently has
int getCampuses (Game g, uni player);

// return the number of IP Patents the specified player currently has
int getIPs (Game g, uni player);

// return the number of Publications the specified player currently has
int getPublications (Game g, uni player);

// return the number of students of the specified discipline type
// the specified player currently has
int getStudents (Game g, uni player, degree discipline);

// return how many students of degree type tradeFrom
// the specified player would need to retrain in order to get one
// student of degree type trainTo.  This will depend
// on what retraining centers, if any, they have a campus at.
int getExchangeRate (Game g, uni player,
                     degree tradeFrom, degree tradeTo);

