//EVERYONE - Get into chat right NOW! (click the speech bubble)

// Please put FOUR spaces in a tab, not three
// Also, recall that this was due ON SUNDAY.

/*
 *  Game.c
 *  1917 2013 ver 0.0
 *  Add to and change this file as you wish to implement the
 *  interface functions in Game.h
 *
 *  Created by Richard Buckland on 2/5/2013
 *  Licensed under Creative Commons SA-BY-NC 3.0. *
 */

// this entire time we haven’t had stdio
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Game.h"

#define NUM_DEGREE_TYPES 6
#define MAX_ARCS 100
#define MAX_CAMPUSES 100
#define MAX_GO8S 8

#define DEFAULT_NUM_GO8S 0
#define DEFAULT_NUM_ARCS 0
#define DEFAULT_NUM_CAMPUSES 2
#define LEFT_SEA_BOUNDRY -3
#define RIGHT_SEA_BOUNDRY 3
#define TERRA_NULLIUS -1

//RegionToArray Conversion Constants
#define COL_0_START 0
#define COL_1_START 3
#define COL_2_START 7
#define COL_3_START 12
#define COL_4_START 16
#define COL_END 19

static int verticesAreEqual(vertex a, vertex b);
static int regionsAreEqual(region a, region b);
static int regionsAreAdjacent(region a, region b);
static region convertToRegion(Game g, int regionNumber);
static int arcSharesRegion(arc a, region b);
static int arcsAreEqual(arc a, arc b);
static int vertexSharesRegion(vertex a, region b);
static int convertRegionToArray(region r);
static int isLegalCampus(Game g, action a);
static int isLegalGO8(Game g, action a);
static int isLegalArcBuild(Game g, action a);
static int isLegalSpinoff(Game g, action a);
static int isLegalRetrain(Game g, action a);
static void setStartingCampuses(Game g, uni player);
static region newRegion(int x, int y);

static int sumArray(int a[], int num_elements);

//struct _player
typedef struct _player{
    int studentCount[NUM_DEGREE_TYPES];
    int publications;
    //int spinoffs;
    int ipPatents;
    
} player;

//struct _game
// your tutorial class designs this type (not us)
// store in this struct all the things you might want to know about
// the Game so you can write the interface functions in this header
// eg you might want to store the current turn number (so i've put
// it in for you as an example but take it out if you don't want it)
// your group puts here the details of the data
// you want to store in the _game struct
typedef struct _game {
    
    int currentTurn;
    player unis[NUM_UNIS];
    
    //These variables deal with the game board's regions
    int regionResource[NUM_REGIONS];
    int regionDiceValue[NUM_REGIONS];
    
    
    
    //WARNING!!!!! THIS CHANGED RECENTLY
    //IF YOU SEE “uniACampuses[]” or “uniA_ARCS[]” orsomething of the sort, replace it
    // with this new bit!!!!
    vertex campuses[NUM_UNIS][MAX_CAMPUSES];
    arc arcGrants[NUM_UNIS][MAX_ARCS];
    vertex GO8campuses[NUM_UNIS][MAX_CAMPUSES];
    
    //These store the number of campuses for each player
    //and the number of arcs for each player.
    // e.g. numberOfCampuses[UNI_A] is the number of campuses that uniA has
    // numberOfCampuses[UNI_B] is the number of campuses that uniB has, etc...
    int numberOfCampuses[NUM_UNIS];
    int numberOfGO8[NUM_UNIS];
    int numberOfArcs[NUM_UNIS];
    
    //The most number of publication is held by this uni
    int numberOfPublications [NUM_UNIS];
    //Store the previous owner of the Most Publications title
    int previousMostPublications;
    //Store the previous owner of the Most Arcs title
    int previousMostArcs;
    
} game;


//newGame
/* **** Functions which change the Game aka SETTERS **** */
// given a Game this function makes it a new Game, resetting everything and
// setting the the type of degree produced by each
// region, and the value on the dice discs in each region.
//
Game newGame (int degree[], int dice[]) {
    Game g = malloc(sizeof(struct _game));  // set memory aside for Game
    assert (g!= NULL);                 // check it worked
    g->currentTurn = TERRA_NULLIUS;               // initialise currentTurn field
    
    //Set up the resources and dice values
    int index = 0;
    while(index < NUM_REGIONS){
        g->regionResource[index] = degree[index];
        g->regionDiceValue[index] = dice[index];
        index++;
    }
    
    //Now initilise all the uni related data.
    //Goes through each player one by one and sets up their data.
    int currentUniversity = UNI_A;
    while(currentUniversity < NUM_UNIS){
        g->numberOfCampuses[currentUniversity] = DEFAULT_NUM_CAMPUSES; //Set to default campuses
        g->numberOfArcs[currentUniversity] = DEFAULT_NUM_ARCS;
        g->numberOfGO8[currentUniversity] = DEFAULT_NUM_GO8S;
        
        if(currentUniversity == UNI_A){
            //Put the first 2 campuses for player A down in the default places
            setStartingCampuses(g, UNI_A);
        } else if (currentUniversity == UNI_B){
            //Put the first 2 campuses for player B down in the default places
            setStartingCampuses(g, UNI_B);
        } else if (currentUniversity == UNI_C){
            //Put the first 2 campuses for player C down in the default places
            setStartingCampuses(g, UNI_C);
        }
        currentUniversity++;
    }
    
    g->unis[0].studentCount[STUDENT_BQN] = 1;
    g->unis[0].studentCount[STUDENT_BPS] = 1;
    
    return g;
}


// after week 8 we will talk about implementing this. for now
// you can leave it to just do this free
void disposeGame (Game g) {
    assert (g != NULL);
    free (g);
}

//void throwDice
// advance the Game to the next turn,
// assuming that the dice has just been rolled and produced diceScore
// the Game starts in turn -1 (we call this state "Terra Nullis") and
// moves to turn 0 as soon as the first dice is thrown.
void throwDice (Game g, int diceScore) {
    int validRegions[NUM_REGIONS];
    int regions = 0;
    int i = 0;
    while (i < NUM_REGIONS) {
        if (g->regionDiceValue[i] == diceScore) {
            validRegions[regions] = i;
            regions++;
        }
        i++;
    }
    
    i = 0;
    while (i < MAX_CAMPUSES) {
        int player = 0;
        while (player < NUM_UNIS) {
            int j = 0;
            while (j < regions) {
                if (vertexSharesRegion(g->campuses[player][i],
                                       convertToRegion(g, validRegions[j]))) {
                    g->unis[player].studentCount[g->regionResource[validRegions[j]]]++;
                }
                j++;
            }
            player++;
        }
        i++;
    }
    
    if (diceScore == 7) {
        // all M$ and MTV students are discarded
        int player = 0;
        while (player < NUM_UNIS) {
            // add M$ and MTV to ThD
            g->unis[player].studentCount[STUDENT_THD] += g->unis[player].studentCount[STUDENT_MMONEY];
            g->unis[player].studentCount[STUDENT_THD] += g->unis[player].studentCount[STUDENT_MTV];
            
            // set M$ and MTV to 0
            g->unis[player].studentCount[STUDENT_MMONEY] = 0;
            g->unis[player].studentCount[STUDENT_MTV] = 0;
            
            player++;
        }
    }
    
    g->currentTurn++;
}


// make the specified action for the current player and update the
// Game state accordingly.
// The function may assume that the action requested is legal.
// START_SPINOFF is not a legal action here

void makeAction (Game g, action a) {
    // firstly, get details of current player
    // 1. if certain action code
    // 2. get a->targetVertex  or a->targetARC
    // 3. change g to reflect the action
    uni currentPlayer = getWhoseTurn(g);
	
	
    if (a.actionCode == PASS) {
        //I actually don't know what to put here
    } else if (a.actionCode == BUILD_CAMPUS) {
        // get the next campus address in uni'x'Campuses[]
        // build campus on a->targetVertex
        // increment g->numberOfCampuses[currentPlayer]
        // increment a player's numberOfCampuses
        
        // a player owns x many campuses
        // since arrays start from 0, the address to store a new(blank)
        // campus coordinate would be at index x in the array
        int nextCampusIndex = getCampuses(g, currentPlayer);
		
        g->campuses[currentPlayer][nextCampusIndex] = a.targetVertex;
        g->numberOfCampuses[currentPlayer]++;
        
        g->unis[currentPlayer].studentCount[STUDENT_BPS]--;
        g->unis[currentPlayer].studentCount[STUDENT_BQN]--;
        g->unis[currentPlayer].studentCount[STUDENT_MJ]--;
        g->unis[currentPlayer].studentCount[STUDENT_MTV]--;
        
    } else if (a.actionCode == BUILD_GO8) {
        int nextGO8Index = getGO8s(g, currentPlayer);
        
        g->GO8campuses[currentPlayer][nextGO8Index] = a.targetVertex;
        g->numberOfGO8[currentPlayer]++;
        
        g->unis[currentPlayer].studentCount[STUDENT_MJ] -= 2;
        g->unis[currentPlayer].studentCount[STUDENT_MMONEY] -= 3;
        
    } else if (a.actionCode == CREATE_ARC) {
        int nextARCIndex = getARCs(g, currentPlayer);
        
        g->arcGrants[currentPlayer][nextARCIndex] = a.targetARC;
        g->numberOfArcs[currentPlayer]++;
        
        g->unis[currentPlayer].studentCount[STUDENT_BQN]--;
        g->unis[currentPlayer].studentCount[STUDENT_BPS]--;
        
    } else if (a.actionCode == OBTAIN_PUBLICATION) {
        g->unis[currentPlayer].publications++;
        
        g->unis[currentPlayer].studentCount[STUDENT_MJ]--;
        g->unis[currentPlayer].studentCount[STUDENT_MMONEY]--;
        g->unis[currentPlayer].studentCount[STUDENT_MTV]--;
    } else if (a.actionCode == OBTAIN_IP_PATENT) {
        g->unis[currentPlayer].ipPatents++;
        
        g->unis[currentPlayer].studentCount[STUDENT_MJ]--;
        g->unis[currentPlayer].studentCount[STUDENT_MMONEY]--;
        g->unis[currentPlayer].studentCount[STUDENT_MTV]--;
        
    } else if (a.actionCode == RETRAIN_STUDENTS) {
        // use a->retrainTo and a->retrainFrom
        // use these value to subtract currentPlayer’s ‘retrainFrom’ degree
        // and add to the ‘retrainTo’
        int exchangeRate =  getExchangeRate (g, currentPlayer,
                                             a.retrainFrom, a.retrainTo);
        
        g->unis[currentPlayer].studentCount[a.retrainFrom] -= exchangeRate;
        g->unis[currentPlayer].studentCount[a.retrainTo]++;
    }
    
}


/* **** Functions which GET data about the Game aka GETTERS **** */
//isSea
// true if the region is not one of the land regions of knowledge island
int isSea (Game g, region r) {
    int sea;
    
    //The region will be sea if:
    //    -The x coord is equal to 3 or -3
    //    -The y coord is equal to 3 or -3
    //    -The region is (1,2), (2,1), (-1, -2) or (-2, -1)
    if ((r.x >= RIGHT_SEA_BOUNDRY) || (r.x <= LEFT_SEA_BOUNDRY)) {
        sea = TRUE;
    } else if ((r.y >= RIGHT_SEA_BOUNDRY) || (r.y <= LEFT_SEA_BOUNDRY)) {
        sea = TRUE;
    } else if (((r.x == 1) && (r.y == 2))  ||
               ((r.x == 2) && (r.y == 1))   ||
               ((r.x == -1) && (r.y == -2)) ||
               ((r.x == -2) && (r.y == -1))) {
        sea = TRUE;
    } else {
        sea = FALSE;
    }
    return sea;
}

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
//isLegalAction
int isLegalAction (Game g, action a) {
    //Legality booleans (maybe change these to chars if memory is tight):
    int legalAction;
    
    //Checks action code exists:
    assert(a.actionCode >= PASS && a.actionCode <= MAX_LEGAL_ACTION);
    
    //Makes sure player doesn't make any action before the game has
    //started:
    if (g->currentTurn == TERRA_NULLIUS && a.actionCode != PASS){
        legalAction = FALSE;
    } else if(a.actionCode == BUILD_CAMPUS){
        printf("----------NOW TESTING CAMPUS CONSTRUCTION----------\n\n");
        legalAction = isLegalCampus(g, a);
    } else if(a.actionCode == BUILD_GO8){
        printf("--------NOW TESTING GO8 CAMPUS CONSTRUCTION--------\n\n");
        legalAction = isLegalGO8(g, a);
    } else if(a.actionCode == CREATE_ARC){
        printf("---------NOW TESTING ARC GRANT CONSTRUCTION--------\n\n");
        legalAction = isLegalArcBuild(g, a);
    } else if(a.actionCode == START_SPINOFF){
        printf("----------NOW TESTING CREATION OF SPINOFF----------\n\n");
        legalAction = isLegalSpinoff(g, a);
    } else if(a.actionCode == RETRAIN_STUDENTS){
        printf("---------NOW TESTING RETRAINING OF STUDENTS--------\n\n");
        legalAction = isLegalRetrain(g, a);
    } else if (a.actionCode == PASS){
        legalAction = TRUE;
    } else {
        legalAction = FALSE;
    }
    
    return legalAction;
}

//===============CHECKS LEGALITY OF CAMPUS CONSTRUCTION===============//
//islegalcampus
static int isLegalCampus(Game g, action a){
    
    int player = getWhoseTurn(g);
    int legalCampusBuild;
    
    //Checks if the player has enough resources to buy a campus:
    int hasResources;
    //This can be copied into all of the other is legal tests where
    //certain resources are spent.
    if (g->unis[player].studentCount[STUDENT_BPS] >= 1 &&
        g->unis[player].studentCount[STUDENT_BQN] >= 1 &&
        g->unis[player].studentCount[STUDENT_MJ]  >= 1 &&
        g->unis[player].studentCount[STUDENT_MTV] >= 1){
        hasResources = TRUE;
        printf("PASSED: Player has sufficient resources for purchase\n");
    } else {
        hasResources = FALSE;
        printf("FAILED: Player has sufficient resources for purchase\n");
    }
    
    //Checks if the specified vertex is a land vertex:
    int campusOnLand;
    //If all three vertexes are in the sea this will return
    //a 0 for landVertex.
    campusOnLand = !(isSea(g, a.targetVertex.region0) &&
                     isSea(g, a.targetVertex.region1) &&
                     isSea(g, a.targetVertex.region2));
    
    if(campusOnLand){
        printf("PASSED: Desired location is on land\n");
    } else {
        printf("FAILED: Desired location is on land\n");
    }
    
    //Checks if the desired vertex is next to one of that
    //player's arc grants:
    int arcNextToVertex = FALSE;
    int arcCount  = 0;
    //Checks each of a player's arc grants to see if they neighbour
    //the desired vertex for building:
    while (arcCount < MAX_ARCS && arcNextToVertex == FALSE){
        region arcRegion0 = g->arcGrants[player][arcCount].region0;
        region arcRegion1 = g->arcGrants[player][arcCount].region1;
        
        if(vertexSharesRegion(a.targetVertex, arcRegion0) &&
           vertexSharesRegion(a.targetVertex, arcRegion1)){
            arcNextToVertex = TRUE;
        }
        arcCount++;
    }
    
    if(arcNextToVertex){
        printf("PASSED: Desired location is next to a player's ARC grant\n");
    } else {
        printf("FAILED: Desired location is next to a player's ARC grant\n");
    }
    
    //Checks if the desired vertex is vacant and there are no
    //campuses on neighbouring vertexes: (I'm not sure whether you are
    //able to build neighbouring campuses to your own so I have set it
    //up so you can't. This isn't hard to fix, D.P)
    int legalVertex = TRUE;
    int playerCount = 0;
    int campusCount = 0;
    //Checks legality of a vertex for every player's campuses.
    while(playerCount < NUM_UNIS){
        //Checks whether the target campus and all of the other campuses
        //on the board share two regions. If they do then legalCampus will
        //be false.
        while(campusCount < MAX_CAMPUSES && legalVertex == TRUE){
            region campusRegion0 = g->campuses[playerCount][campusCount].region0;
            region campusRegion1 = g->campuses[playerCount][campusCount].region1;
            region campusRegion2 = g->campuses[playerCount][campusCount].region1;
            
            if(vertexSharesRegion(a.targetVertex,campusRegion0)
               && vertexSharesRegion(a.targetVertex,campusRegion1)
               && vertexSharesRegion(a.targetVertex,campusRegion2)) {
                legalVertex = FALSE;
            }
            campusCount++;
        }
        playerCount++;
    }
    
    if(legalVertex){
        printf("PASSED: Desired location has no adjacent campuses\n");
    } else {
        printf("FAILED: Desired location has no adjacent campuses\n");
    }
    
    legalCampusBuild = hasResources && campusOnLand &&
    arcNextToVertex && legalVertex;
    
    if(legalCampusBuild){
        printf("\n---------------------TEST PASSED-------------------\n\n");
    } else {
        printf("\n---------------------TEST FAILED-------------------\n\n");
    }
    
    return legalCampusBuild;
}

//=============CHECKS LEGALITY OF GO8 CAMPUS CONSTRUCTION=============//
//This is very similar to isLegalCampus but simpler. Refer to the
//function above for more detailed comments.
static int isLegalGO8(Game g, action a){
    
    int player = getWhoseTurn(g);
    int legalGO8Build;
    
    //Checks if the player has enough resources to buy a GO8 campus:
    int hasResources;
    if (g->unis[player].studentCount[STUDENT_MJ]     >= 2 &&
        g->unis[player].studentCount[STUDENT_MMONEY] >= 3) {
        hasResources = TRUE;
        printf("PASSED: Player has sufficient resources for purchase\n");
    } else {
        hasResources = FALSE;
        printf("FAILED: Player has sufficient resources for purchase\n");
    }
    
    //Checks that there are less than 8 total GO8 campuses. There
    //can only be a maximum of 8:
    int availableGO8 = FALSE;
    if (sumArray(g->numberOfGO8, NUM_UNIS) < MAX_GO8S){
        availableGO8 = TRUE;
    }
    
    if(availableGO8){
        printf("PASSED: There are less than 8 GO8 campuses on the board\n");
    } else {
        printf("FAILED: There are less than 8 GO8 campuses on the board\n");
    }
    
    //Checks that there is a campus owned by that player on the
    //target vertex:
    int campusToUpgrade = FALSE;
    int campusCount = 0;
    while(campusCount < MAX_CAMPUSES && campusToUpgrade == FALSE){
        region campusRegion0 = g->campuses[player][campusCount].region0;
        region campusRegion1 = g->campuses[player][campusCount].region1;
        region campusRegion2 = g->campuses[player][campusCount].region1;
        
        if(vertexSharesRegion(a.targetVertex, campusRegion0)
           && vertexSharesRegion(a.targetVertex, campusRegion1)
           && vertexSharesRegion(a.targetVertex, campusRegion2)) {
            campusToUpgrade = TRUE;
        }
        campusCount++;
    }
    
    if(campusToUpgrade){
        printf("PASSED: Desired location has a campus owned by player\n");
    } else {
        printf("FAILED: Desired location has a campus owned by player\n");
    }
    
    legalGO8Build = hasResources && availableGO8 && campusToUpgrade;
    
    if(legalGO8Build){
        printf("\n---------------------TEST PASSED-------------------\n\n");
    } else {
        printf("\n---------------------TEST FAILED-------------------\n\n");
    }
    
    return legalGO8Build;
}

//==============CHECKS LEGALITY OF ARC GRANT CONSTRUCTION=============//
static int isLegalArcBuild(Game g, action a){
    
    int player = getWhoseTurn(g);
    int legalArcBuild;
    
    //Checks if the player has enough resources to buy an arc grant:
    int hasResources;
    if (g->unis[player].studentCount[STUDENT_BQN] > 0 &&
        g->unis[player].studentCount[STUDENT_BPS] > 0) {
        hasResources = TRUE;
        printf("PASSED: Player has sufficient resources for purchase\n");
    } else {
        hasResources = FALSE;
        printf("FAILED: Player has sufficient resources for purchase\n");
    }
    
    //Checks that the desired location isn't in the sea:
    int arcOnLand;
    //If all three vertexes are in the sea this will return
    //a 0 for landVertex.
    arcOnLand = !(isSea(g, a.targetARC.region0) &&
                  isSea(g, a.targetARC.region1));
    
    if(arcOnLand){
        printf("PASSED: Desired build location is on land\n");
    } else {
        printf("FAILED: Desired build location is on land\n");
    }
    
    //Checks that the desired location is free:
    int locationAvailable = TRUE;
    int playerCount = 0;
    int arcCount = 0;
    while(playerCount < NUM_UNIS){
        //Checks whether the target arc and all of the other arcs
        //on the board share two regions. If they do then locationAvailabe
        //will be false. This also covers a player's own arc grants.
        while(arcCount < MAX_ARCS && locationAvailable == TRUE){
            region arcRegion0 = g->arcGrants[playerCount][arcCount].region0;
            region arcRegion1 = g->arcGrants[playerCount][arcCount].region1;
            
            if(arcSharesRegion(a.targetARC, arcRegion0)
               && arcSharesRegion(a.targetARC, arcRegion1)) {
                locationAvailable = FALSE;
            }
            arcCount++;
        }
        playerCount++;
    }
    
    if(locationAvailable){
        printf("PASSED: Desired location is available for ARC construction\n");
    } else {
        printf("FAILED: Desired location is available for ARC construction\n");
    }
    
    //Checks that the desired arc connects to either a campus or other
    //arc grant:
    int nextToInfrastructure;
    
    int arcNextToCampus = FALSE;
    int campusCount  = 0;
    //Checks each of a player's campuses to see if they neighbour
    //the desired line for building:
    while (campusCount < MAX_CAMPUSES){
        region campusRegion0 = g->campuses[player][campusCount].region0;
        region campusRegion1 = g->campuses[player][campusCount].region1;
        region campusRegion2 = g->campuses[player][campusCount].region1;
        
        if(arcSharesRegion(a.targetARC, campusRegion0)
           && arcSharesRegion(a.targetARC, campusRegion1)) {
            arcNextToCampus = TRUE;
        } else if(arcSharesRegion(a.targetARC, campusRegion1)
                  && arcSharesRegion(a.targetARC, campusRegion2)){
            arcNextToCampus = TRUE;
        } else if(arcSharesRegion(a.targetARC, campusRegion0)
                  && arcSharesRegion(a.targetARC, campusRegion2)){
            arcNextToCampus = TRUE;
        }
        
        campusCount++;
    }
    
    //Check that an arc connects and arc:
    int arcNextToArc = FALSE;
    
    arcCount = 0;
    
    //Checks if two arcs share a vertex by testing whether the two
    //uncommon regions are or aren't adjacent.
    while(arcCount < MAX_ARCS && arcNextToArc == FALSE){
        region arcRegion0 = g->arcGrants[player][arcCount].region0;
        region arcRegion1 = g->campuses[player][arcCount].region1;
        
        if(regionsAreEqual(a.targetARC.region0, arcRegion0)
           && regionsAreAdjacent(a.targetARC.region1, arcRegion1)){
            arcNextToArc = TRUE;
        } else if(regionsAreEqual(a.targetARC.region1, arcRegion0)
                  && regionsAreAdjacent(a.targetARC.region0, arcRegion1)){
            arcNextToArc = TRUE;
        } else if(regionsAreEqual(a.targetARC.region0, arcRegion1)
                  && regionsAreAdjacent(a.targetARC.region1, arcRegion0)){
            arcNextToArc = TRUE;
        } else if(regionsAreEqual(a.targetARC.region1, arcRegion1)
                  && regionsAreAdjacent(a.targetARC.region0, arcRegion0)){
            arcNextToArc = TRUE;
        }
        arcCount++;
    }
    
    
    //If either of these, then nextToInfrastructure will be true.
    nextToInfrastructure = arcNextToArc || arcNextToCampus;
    
    if(nextToInfrastructure){
        printf("PASSED: Desired location has adjacent infrastructure owned by player\n");
    } else {
        printf("FAILED: Desired location has adjacent infrastructure owned by player\n");
    }
    
    legalArcBuild = hasResources && arcOnLand && locationAvailable
    && nextToInfrastructure;
    
    if(legalArcBuild){
        printf("\n---------------------TEST PASSED-------------------\n\n");
    } else {
        printf("\n---------------------TEST FAILED-------------------\n\n");
    }
    
    return legalArcBuild;
}

//================CHECKS LEGALITY OF CREATING A SPINOFF===============//
int isLegalSpinoff(Game g, action a){
    
    int player = getWhoseTurn(g);
    int legalSpinoff;
    
    //Checks if the player has enough resources to buy a spinoff:
    int hasResources;
    if (g->unis[player].studentCount[STUDENT_MJ]  > 0 &&
        g->unis[player].studentCount[STUDENT_MTV] > 0 &&
        g->unis[player].studentCount[STUDENT_MTV] > 0){
        hasResources = TRUE;
        printf("PASSED: Player has sufficient resources for purchase\n");
    } else {
        hasResources = FALSE;
        printf("FAILED: Player has sufficient resources for purchase\n");
    }
    
    legalSpinoff = hasResources;
    
    if(legalSpinoff){
        printf("\n---------------------TEST PASSED-------------------\n\n");
    } else {
        printf("\n---------------------TEST FAILED-------------------\n\n");
    }
    
    return legalSpinoff;
}

//===============CHECKS LEGALITY OF RETRAINING STUDENTS===============//
int isLegalRetrain(Game g, action a){
    
    int player = getWhoseTurn(g);
    int exchangeRate = getExchangeRate(g, player, a.retrainFrom, a.retrainTo);
    int legalRetrain;
    
    //Ensures that the number of students that a player wants to retrain
    //are in a 3 to 1 (2 to 1 if player has retrain centre) ratio:
    int retrainType = a.retrainFrom;
    int canTrade;
    
    
    if((g->unis[player].studentCount[retrainType] >= exchangeRate) &&
       (retrainType != STUDENT_THD)) {
        canTrade = TRUE;
        printf("PASSED: Player has enough students to trade\n");
    } else {
        canTrade = FALSE;
        printf("FAILED: Player has enough students to trade\n");
    }
    
    legalRetrain = canTrade;
    
    if(legalRetrain){
        printf("\n---------------------TEST PASSED-------------------\n\n");
    } else {
        printf("\n---------------------TEST FAILED-------------------\n\n");
    }
    
    return legalRetrain;
}

//getDegree
// what type of students are produced by the specified land region?
// see degree discipline codes above
degree getDegree (Game g, region r) {
    //Figure out what number the region is (convert 2d coordinates to 1d coordinates.)
    //Go to the regionResource array in game struct
    //Return whatever is in that
    
    int regionNo = 0;
    if (r.x == -2) {
        if (r.y == 0) {
            regionNo = g->regionResource[2];
        } else if (r.y == 1) {
            regionNo = g->regionResource[1];
        } else if (r.y == 2) {
            regionNo = g->regionResource[0];
        }
    } else if (r.x == -1) {
        if (r.y == -1) {
            regionNo = g->regionResource[6];
        } else if (r.y == 0) {
            regionNo = g->regionResource[5];
        } else if (r.y == 1) {
            regionNo = g->regionResource[4];
        } else if (r.y == 2) {
            regionNo = g->regionResource[3];
        }
    } else if (r.x == 0) {
        if (r.y == -2) {
            regionNo = g->regionResource[11];
        } else if (r.y == -1) {
            regionNo = g->regionResource[10];
        } else if (r.y == 0) {
            regionNo = g->regionResource[9];
            
        } else if (r.y == 1) {
            regionNo = g->regionResource[8];
        } else if (r.y == 2) {
            regionNo = g->regionResource[7];
        }
    } else if (r.x == 1) {
        if (r.y == -2) {
            regionNo = g->regionResource[15];
        } else if (r.y == -1) {
            regionNo = g->regionResource[14];
        } else if (r.y == 0) {
            regionNo = g->regionResource[13];
            
        } else if (r.y == 1) {
            regionNo = g->regionResource[12];
        }
    } else if (r.x == 2) {
        if (r.y == -2) {
            regionNo = g->regionResource[18];
        } else if (r.y == -1) {
            regionNo = g->regionResource[17];
        } else if (r.y == 0) {
            regionNo = g->regionResource[16];
        }
    }
    return regionNo;
}
//getDiceValue
// what dice value produces students in the specified land region?
// 2..12
//what number should be rolled to let the given region produce students
int getDiceValue (Game g, region r) {
    
    int arrayValue = convertRegionToArray(r);
    return g->regionDiceValue[arrayValue];
    
}
//getMostARCs
// which university currently has the prestige award for the most ARCs?
// this deemed to be UNI_C at the start of the Game.
uni getMostARCs (Game g) {
    int mostArcs;
    if (g->currentTurn == TERRA_NULLIUS ){
        mostArcs=UNI_C;
    } else if (g->numberOfArcs[UNI_A] > g->numberOfArcs[UNI_B] &&
               g->numberOfArcs[UNI_A] > g->numberOfArcs[UNI_C]) {
        mostArcs = UNI_A;
    } else if (g->numberOfArcs[UNI_B] > g->numberOfArcs[UNI_A] &&
               g->numberOfArcs[UNI_B] > g->numberOfArcs[UNI_C]) {
        mostArcs = UNI_B;
    } else if (g->numberOfArcs[UNI_C] > g->numberOfArcs[UNI_B] &&
               g->numberOfArcs[UNI_C] > g->numberOfArcs[UNI_A]) {
        mostArcs = UNI_C;
    } else{
        mostArcs = (g->previousMostArcs);    //Will only jump in if there is a draw in most arcs
        
    }
    (g->previousMostArcs) = mostArcs;
    return mostArcs;
}

// which university currently has the prestige award for the most pubs?
// this is deemed to be UNI_C at the start of the Game.

//Fixed this. Whoever wrote it before initialised mostPublications as UNI_C, then
//assigned (g->previousMostPublications) as mostPublications, which would result as
//previous holder always being UNI_C. Also changed the conditions because before,
//it was comparing elements which didn’t actually contain each player’s
//number of publications.
//Robert Kwan

//getMostPublications
uni getMostPublications (Game g) {
    int mostPublications;
    
    if (g->currentTurn == TERRA_NULLIUS ){
        mostPublications= UNI_C;
        
    } else if (g->unis[UNI_A].publications > g->unis[UNI_B].publications &&
               g->unis[UNI_A].publications > g->unis[UNI_C].publications) {
        mostPublications = UNI_A;
    } else if (g->unis[UNI_B].publications > g->unis[UNI_A].publications &&
               g->unis[UNI_B].publications > g->unis[UNI_C].publications) {
        mostPublications = UNI_B;
    } else if (g->unis[UNI_C].publications > g->unis[UNI_A].publications &&
               g->unis[UNI_C].publications > g->unis[UNI_B].publications) {
        mostPublications = UNI_C;
    } else {
        mostPublications = (g->previousMostPublications);   //Will only jump in if there is a draw in most arcs
    }
    
    (g->previousMostPublications) = mostPublications;
    return mostPublications;
}

//getTurnNumber
// return the current turn number of the Game -1,0,1, ..
int getTurnNumber (Game g) {
    return g->currentTurn;
}

//getWhoseTurn
// return the player id of the player whose turn it is
// the result of this function is UNI_C during Terra Nullis
uni getWhoseTurn (Game g) {
    int player;
    
    int playersTurnCalculation=((g->currentTurn+NUM_UNIS)%NUM_UNIS);
    //added 3 because the game starts at -1. thus prevents negative numbers
    
    if(playersTurnCalculation==UNI_A){
        player=UNI_A;
    }
    
    if(playersTurnCalculation==UNI_B){
        player=UNI_B;
    }
    
    if(playersTurnCalculation==UNI_C){
        player=UNI_C;
    }
    return player;
}

// return the contents of the given vertex (ie campus code or
// VACANT_VERTEX)

//getCampus
int getCampus (Game g, vertex v) {
    int output = VACANT_VERTEX;
    int currentUniversity = UNI_A;
    int currentVertex;
    
    while ((currentUniversity < NUM_UNIS) && (output == 0)) {
        currentVertex = 0;
        while((currentVertex < g->numberOfCampuses[currentUniversity]) &&
              (output == 0)){
            if(verticesAreEqual(v, g->campuses[currentUniversity][currentVertex])){
                output = currentUniversity + 1;
            }
            
            if(verticesAreEqual(v, g->GO8campuses[currentUniversity][currentVertex])){
                output = currentUniversity + 4;
            }
            currentVertex++;
        }
        currentUniversity++;
    }
    return output;
}


//getARC
// the contents of the given edge (ie ARC code or vacant ARC)
int getARC (Game g, arc a) {
    int output = VACANT_ARC;
    int currentUniversity = UNI_A;
    int currentEdge;
    
    
    while((currentUniversity < NUM_UNIS) && (output == 0)) {
        currentEdge = 0;
        while(currentEdge < g->numberOfArcs[currentUniversity]){
            if(arcsAreEqual(a, g->arcGrants[currentUniversity][currentEdge])) {
                output = currentUniversity+1;
            }
            currentEdge++;
        }
        currentUniversity++;
    }
    return output;
}


//getARCs
// return the number of ARC grants the specified player currently has
// This function gets given a specified player ie (UNI_A,UNI_B or UNI_C)
// gets number of arc grants that player has and returns this value
int getARCs (Game g, uni player) {
    return g->numberOfArcs[player];
}
//getG08s
// return the number of GO8 campuses the specified player currently has
int getGO8s (Game g, uni player) {
    return g->numberOfGO8[player];
}
//getCampuses
// return the number of normal Campuses the specified player currently has
int getCampuses (Game g, uni player) {
    return g->numberOfCampuses[player];
}


//getIPs
// return the number of IP Patents the specified player currently has
int getIPs (Game g, uni player) {
    return g->unis[player].ipPatents;
}

//getPublications
// return the number of Publications the specified player currently has
int getPublications (Game g, uni player) {
    return g->unis[player].publications;
}

//getStudents
// return the number of students of the specified discipline type
// the specified player currently has
int getStudents (Game g, uni player, degree discipline) {
    return g->unis[player].studentCount[discipline];
}

//getExchangeRate
// return  how many students of degree type tradeFrom
// the specified player would need to retrain in order to get one
// student of degree type trainTo.  This will depend
// on what retraining centers, if any, they have a campus at.
int getExchangeRate (Game g, uni player,
                     degree tradeFrom, degree tradeTo) {
    //    int exchangeRate;
    //
    //
    //    //#define NUMBER_OF_DEGREES 6
    //    // give means we give for example 2 MJ students to get 1 BPS student
    //    // The function asks
    //    int give = 0;
    //    int get = 0;
    //    int student = STUDENT_THD;
    //    while (student < NUMBER_OF_DEGREES) {
    //        if (action.retrainFrom == student) {
    //            if (campus == retrainingcenter) {
    //                give = 2;
    //            } else {
    //                give =3;
    //            }
    //        } if (action.retrainTo == student) {
    //            get = 1;
    //        } student++;
    //    }
    //    exchangeRate = give / get;
    //
    //    return exchangeRate;
    return 3;
}

//getKPIpoints
int getKPIpoints (Game g, uni player){
    int points = 0;
    //Buildings Points
    points = points + getCampuses(g, player) * 10;
    points = points + getGO8s(g, player) * 20;
    points = points + getARCs(g, player) * 2;
    points = points + getIPs (g, player) * 10;
    //Prestige Points
    
    if(player == getMostARCs (g) ){
        points = points + 10;
    }
    if(player == getMostPublications(g)){
        points = points + 10;
    }
    return points;
}




//HELPER FUNCTIONS





//Takes in 2 vertices, checks if they are equal.
static int verticesAreEqual(vertex a, vertex b){
    int stage0 = FALSE;
    int stage1 = FALSE;
    int stage2 = FALSE;
    int result = FALSE;
    
    if(regionsAreEqual(a.region0, b.region0) ||
       regionsAreEqual(a.region0, b.region1) ||
       regionsAreEqual(a.region0, b.region2)) {
        //Then one of the regions is equivalent to one of the others
        stage0 = TRUE;
    }
    if(regionsAreEqual(a.region1, b.region0) ||
       regionsAreEqual(a.region1, b.region1) ||
       regionsAreEqual(a.region1, b.region2)) {
        //Then one of the regions is equivalent to one of the others
        stage1 = TRUE;
    }
    if(regionsAreEqual(a.region2, b.region0) ||
       regionsAreEqual(a.region2, b.region1) ||
       regionsAreEqual(a.region2, b.region2)) {
        //Then one of the regions is equivalent to one of the others
        stage2 = TRUE;
    }
    
    //If all three regions are equivalent, then the vertex is equivalent
    if(stage0 == TRUE && stage1 == TRUE && stage2 == TRUE){
        result = TRUE;
    } else {
        result = FALSE;
    }
    
    return result;
}


//This takes in 2 regions and outputs whether or not they are equal
static int regionsAreEqual(region a, region b){
    int result;
    
    if ((a.x == b.x) && (a.y == b.y)){
        result = TRUE;
    } else {
        result = FALSE;
    }
    
    return result;
}

static int sumArray(int a[], int num_elements) {
    int i, sum=0;
    for (i=0; i<num_elements; i++) {
        sum = sum + a[i];
    }
    return sum;
}


//Converts a region number (from our array) to a region type.
//i.e. it converts to badly drawn co-ordinates
static region convertToRegion(Game g, int regionNumber){
    //If colN, then region.x = N-2
    //If rowM, then region.y = 2-M
    
    int colNum;
    int rowNum;
    int columnStart[6] = { COL_0_START, COL_1_START, COL_2_START, COL_3_START,COL_4_START, COL_END };
    region r;
    
    int currentColumn = 0;
    
    //changed since Tuesday’s Lab. Up to date as of 6:00 14-05-2013
    //Changed the if statement and the colNum and rowNum assignments.
    //Didn’t give the right values before, but now they do; I checked
    //Robert Kwan
    while(currentColumn < 6){
        
        if((regionNumber >= columnStart[currentColumn]) &&
           (regionNumber < columnStart[currentColumn+1])){
            colNum = currentColumn;
            rowNum = regionNumber - columnStart[currentColumn];
            
            if(colNum == 3){
                rowNum += 1;
            }else if(colNum == 4){
                rowNum += 2;
            }
        }
        currentColumn++;
        
    }
    
    r.x = colNum-2;
    r.y = 2 - rowNum;
    
    return r;
    
}

static int convertRegionToArray(region r){
    //col number is N = region.x + 2
    //row number is M = 2- region.y
    int colNum;
    int rowNum;
    int arrayValue;
    int columnStart[5] = { COL_0_START, COL_1_START, COL_2_START, COL_3_START,COL_4_START };
    
    colNum = r.x + 2;
    rowNum = 2 - r.y;
    
    arrayValue = columnStart[colNum] + rowNum;
    
    if(colNum == 3){
        arrayValue = arrayValue - 1;
    } else if(colNum == 4){
        arrayValue = arrayValue - 2;
    }
    
    return arrayValue;
}




static int arcSharesRegion(arc a, region b) {
    return (regionsAreEqual(a.region0, b) ||
            regionsAreEqual(a.region1, b) );
}

static int vertexSharesRegion(vertex a, region b) {
    return (regionsAreEqual(a.region0, b) ||
            regionsAreEqual(a.region1, b) ||
            regionsAreEqual(a.region2, b) );
}



static int arcsAreEqual(arc a, arc b){
    int result;
    
    if((regionsAreEqual(a.region0, b.region0) && regionsAreEqual(a.region1, b.region1))){
        
        result = TRUE;
    } else if(regionsAreEqual(a.region0, b.region1) && regionsAreEqual(a.region1, b.region0)){
        result = TRUE;
    } else {
        result = FALSE;
    }
    
    return result;
}

//This function checks whether two regions are adjacent by seeing if they
//a) Share either an x or y coordinate.
//b) Share an adjacent x or y coordinate.
//I built this using the coordinates system Richard provided us [D.P]
static int regionsAreAdjacent(region a, region b){
    int regionsAdjacent;
    int xCoordinatesAdjacent = 0;
    int yCoordinatesAdjacent = 0;
    
    if((a.x == b.x)||(a.x + 1 == b.x)||(a.x - 1 == b.x)){
        xCoordinatesAdjacent = TRUE;
    }
    
    if((a.y == b.y)||(a.y + 1 == b.y)||(a.y - 1 == b.y)){
        yCoordinatesAdjacent = TRUE;
    }
    
    if((xCoordinatesAdjacent == TRUE) && (yCoordinatesAdjacent == TRUE)){
        regionsAdjacent = TRUE;
    } else {
        regionsAdjacent = FALSE;
    }
    
    return regionsAdjacent;
}

static void setStartingCampuses(Game g, uni player) {
    vertex firstCampus;
    vertex secondCampus;
    
    if (player == UNI_A) {
        firstCampus.region0 = newRegion(0,2);
        firstCampus.region1 = newRegion(0,3);
        firstCampus.region2 = newRegion(-1,3);
        
        secondCampus.region0 = newRegion(0,-2);
        secondCampus.region1 = newRegion(0,-3);
        secondCampus.region2 = newRegion(1,-3);
        
    } else if (player == UNI_B) {
        firstCampus.region0 = newRegion(-3,3);
        firstCampus.region1 = newRegion(-2,2);
        firstCampus.region2 = newRegion(-3,2);
        
        secondCampus.region0 = newRegion(3,-3);
        secondCampus.region1 = newRegion(2,-2);
        secondCampus.region2 = newRegion(3,-2);
    } else if (player == UNI_C) {
        firstCampus.region0 = newRegion(-2,0);
        firstCampus.region1 = newRegion(-2,-1);
        firstCampus.region2 = newRegion(-3,0);
        
        secondCampus.region0 = newRegion(3,0);
        secondCampus.region1 = newRegion(2,0);
        secondCampus.region2 = newRegion(2,1);
    }
    
    g->campuses[player][0] = firstCampus;
    g->campuses[player][1] = secondCampus;
}

static region newRegion(int x, int y) {
    region r;
    r.x = x;
    r.y = y;
    return r;
}


// dont mind this, this is for me, but i couldnt save it somewhere else
/*boundry = 3;
 maxmax=30
 exchange = 2;
 coordinate (0) = 0;
 vertex (0) = 0;
 while (coordinate (0) <18 && maxmax <30) {
 if (trainingcentre == coordinate (0)){
 while (vertex (0) < boundry) {
 if (vertex (0) == campus){
 exchange = 2;
 }vertex(0) ++;
 }
 } coordinate(0) ++;
 boundry = boundry+2;
 }*/




