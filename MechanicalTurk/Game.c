/*
 *  Game.c
 *  1917 2013 ver 3.0 2nd June
 *  Submission for v3.0 milestone
 *
 *  As modified and implemented by Monday13Spoons
 *
 *  Created by Richard Buckland on 2/5/2013
 *  Licensed under Creative Commons SA-BY-NC 3.0. *
 *
 *  ==============================================
 *  If you are using this file for running your AI
 *  Please report any bugs to Genevieve Anne (our tute testing coordinator)
 *  And make sure you can replicate the bug!
 *  Thanks, Mon13Spoons
 *  ==============================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "Game.h"

#define NUM_VERTICES 54
#define NUM_ARCS     72
#define NUM_DEGREES  6
#define NUM_CENTERS  6

#define MAX_REGION_COORD 2
#define MAX_ARC_COORD (MAX_REGION_COORD*2+1)
#define VERTICES_PER_HEX 6
#define EDGES_PER_VERTEX 3
#define VERTICES_PER_EDGE 2

#define KPI_TO_WIN 150
#define START_KPI_COUNT 20
#define KPI_PER_CAMPUS  10
#define KPI_PER_GO8     20
#define KPI_PER_ARC     2
#define KPI_PER_PATENT  10
#define MOST_ARCS_KPI   10
#define MOST_PUBS_KPI   10

#define INVALID -1

#define DEFAULT_STUDENTS {0,3,3,1,1,1}
#define DEFAULT_EXCHANGE_RATE 3
#define TERRA_NULLIS -1

#define A_START_CAMPUS_1_VERTEX 26
#define A_START_CAMPUS_2_VERTEX 27
#define B_START_CAMPUS_1_VERTEX 5
#define B_START_CAMPUS_2_VERTEX 48
#define C_START_CAMPUS_1_VERTEX 0
#define C_START_CAMPUS_2_VERTEX 53

#define START_CAMPUS_COUNT    2
#define START_GO8_COUNT       0
#define START_ARC_COUNT       0
#define START_PATENT_COUNT    0
#define START_PUB_COUNT       0

#define CAMPUS_COST {0,1,1,1,1,0}
#define GO8_COST {0,0,0,2,0,3}
#define ARC_COST {0,1,1,0,0,0}
#define SPINOFF_COST {0,0,0,1,1,1}
#define MAX_GO8_CAMPUS 8

typedef struct _university {
    int studentsInDegree[NUM_DEGREES];
    int KPICount;
    int campusCount;
    int GO8Count;
    int arcGrantCount;
    int patentCount;
    int publicationsCount;
    int centers[NUM_DEGREES];
} university;

typedef struct  _hex {
    int hexValue;
    degree degreeType;
    region hexRegion;
} hex;

typedef struct _game {
    int currentTurn;
    hex map[NUM_REGIONS];
    university uniData[NUM_UNIS];
    int vertices[NUM_VERTICES];
    int arcs[NUM_ARCS];
    int mostArcsPlayer;
    int mostPubsPlayer;
} game;

//////////////////////////////////
// Helper Function Prototypes
//////////////////////////////////

typedef int regionIndex;
typedef int vertexIndex;
typedef int arcIndex;

static int isVertex (vertex v);
static int isArc (arc a);
static int isLand (region r);

static regionIndex regionToIndex (region r);
static arcIndex arcToIndex (arc a);
static vertexIndex vertexToIndex (vertex v);

static region indexToRegion (int index);

static vertexIndex adjVertexToRegion (region r, int i);
static vertexIndex adjVertexToArc(arc a,int i);
static arcIndex adjArcToArc(arc a,int i);
static vertexIndex adjVertexToVertex (vertex v, int i);
static arcIndex adjArcToVertex (vertex v, int i);

static int checkOrder (int a, int b, int c);
static int min (int a, int b);
static int max (int a, int b);
static int minYCoord (int xval, int limit);
static int maxYCoord (int xval, int limit);

/* **** Functions which change the Game aka SETTERS **** */
// given a Game this function makes it a new Game, resetting everything and
// setting the the type of degree produced by each
// region, and the value on the dice discs in each region.
//
Game newGame (int degree[], int dice[]) {
    
    // set memory aside for Game
    Game g = malloc(sizeof(struct _game));
    
    // check it worked
    assert (g != NULL);
    
    // initialise currentTurn
    g->currentTurn = TERRA_NULLIS;
    
    //Populate the map according to the input degree and dice arrays.
    int i = 0;
    while (i < NUM_REGIONS) {
        
        assert ((degree[i]>=STUDENT_THD) &&(degree[i]<=STUDENT_MMONEY) &&"Student types must be legal!");
        assert(dice[i]>=2 && dice[i]<=12 && "Dice values must be between 2 and 12");
        
        g->map[i].degreeType = degree[i];
        g->map[i].hexValue = dice[i];
        g->map[i].hexRegion = indexToRegion(i);
        i++;
    }
    
    //Set all vertices to vacant.
    i = 0;
    while (i < NUM_VERTICES) {
        g->vertices[i] = VACANT_VERTEX;
        i++;
    }
    
    // Set all arcs to vacant.
    i = 0;
    while (i < NUM_ARCS) {
        g->arcs[i] = VACANT_ARC;
        i++;
    }
    
    //Set university centers arrays to false
    i = UNI_A;
    int j;
    while (i < NUM_UNIS) {
        j = STUDENT_THD;
        while (j < NUM_DEGREES) {
            g->uniData[i].centers[j] = FALSE;
            j++;
        }
        i++;
    }
    
    //Populate the degrees in each university with the starting values.
    i = UNI_A;
    int startingStudents[NUM_DEGREES] = DEFAULT_STUDENTS;
    while (i < NUM_UNIS) {
        j = STUDENT_THD;
        while (j < NUM_DEGREES) {
            g->uniData[i].studentsInDegree[j] = startingStudents[j];
            j++;
        }
        i++;
    }
    
    // Assigning A's campus locations and centers
    g->vertices[A_START_CAMPUS_1_VERTEX] = CAMPUS_A;
    g->vertices[A_START_CAMPUS_2_VERTEX] = CAMPUS_A;
    
    // Assigning B's campus locations and centers
    g->vertices[B_START_CAMPUS_1_VERTEX] = CAMPUS_B;
    g->vertices[B_START_CAMPUS_2_VERTEX] = CAMPUS_B;
    
    // Assigning C's campus locations and centers
    g->vertices[C_START_CAMPUS_1_VERTEX] = CAMPUS_C;
    g->vertices[C_START_CAMPUS_2_VERTEX] = CAMPUS_C;
    
    //Setting all counts to initial values.
    i = UNI_A;
    while (i < NUM_UNIS) {
        g->uniData[i].KPICount = START_KPI_COUNT;
        g->uniData[i].campusCount = START_CAMPUS_COUNT;
        g->uniData[i].GO8Count = START_GO8_COUNT;
        g->uniData[i].arcGrantCount = START_ARC_COUNT;
        g->uniData[i].patentCount = START_PATENT_COUNT;
        g->uniData[i].publicationsCount = START_PUB_COUNT;
        i++;
    }
    
    //Updating the KPICount for the Player C prestige convention.
    g->uniData[UNI_C].KPICount += MOST_ARCS_KPI + MOST_PUBS_KPI;
    
    //Setting the Player C prestige convention.
    g->mostArcsPlayer = UNI_C;
    g->mostPubsPlayer = UNI_C;
    
    return g;
}

void disposeGame (Game g) {
    assert (g != NULL);
    free (g);
}

// advance the Game to the next turn,
// assuming that the dice has just been rolled and produced diceScore
// the Game starts in turn -1 (we call this state "Terra Nullis") and
// moves to turn 0 as soon as the first dice is thrown.
void throwDice (Game g, int diceScore) {
    assert(g != NULL&&"A test gave us a null game!");
    assert(diceScore>=2 && diceScore<=12 && "Dice values must be between 2 and 12");
    g->currentTurn++;
    int i = 0;
    int contents;
    while (i < NUM_REGIONS) {
        if (g->map[i].hexValue == diceScore) {
            int j = 0;
            region activeRegion = g->map[i].hexRegion;
            degree studentType = g->map[i].degreeType;
            while (j < VERTICES_PER_HEX) {
                vertexIndex index = adjVertexToRegion(activeRegion, j);
                if (index > INVALID) {
                    contents = g->vertices[index];
                    if (contents > CAMPUS_C) {
                        g->uniData[contents - GO8_CAMPUS_A].studentsInDegree[studentType]+=2;
                    } else if (contents>VACANT_VERTEX) {
                        g->uniData[contents - CAMPUS_A].studentsInDegree[studentType]++;
                    }
                }
                j++;
            }
        }
        i++;
    }
    if (diceScore==7) {
        i = 0;
        int thdCount;
        while (i < NUM_UNIS) {
            thdCount = 0;
            thdCount += g->uniData[i].studentsInDegree[STUDENT_MTV];
            thdCount += g->uniData[i].studentsInDegree[STUDENT_MMONEY];
            g->uniData[i].studentsInDegree[STUDENT_MTV] = 0;
            g->uniData[i].studentsInDegree[STUDENT_MMONEY] = 0;
            g->uniData[i].studentsInDegree[STUDENT_THD] +=thdCount;
            i++;
        }
    }
    return;
}

// make the specified action for the current player and update the
// Game state accordingly.
// The function may assume that the action requested is legal.
// START_SPINOFF is not a legal action here
void makeAction (Game g, action a) {
    assert(g != NULL && "A test gave us a null game!");
    assert(a.actionCode >= PASS && a.actionCode <= RETRAIN_STUDENTS && "Action does not exist!");
    assert(a.actionCode != START_SPINOFF && "Make action was asked to make a spinoff - impossible!");
    uni player = getWhoseTurn(g);
    assert(g->currentTurn !=-1 && "Make action was asked to make an action before the game had started!");
    if (a.actionCode == PASS) {
        //do nothing. playGame function will call rollDice to progress the game to the next turn
    } else if (a.actionCode == BUILD_CAMPUS) {
        assert(isLegalAction(g,a)&&"Tried to build an illegal campus!");
        //Updating the map
        vertex targetVertex = a.targetVertex;
        vertexIndex index = vertexToIndex(targetVertex);
        //Stores value of uni who owns the campus in index
        //Eg, if it is player A's turn, vertex will contain 1
        g->vertices[index] = (player + 1);
        
        //Updating the player data
        g->uniData[player].campusCount++;
        
        //Checking and updating if player owns a new port
        if (index == 15) {
            g->uniData[player].centers[STUDENT_MTV] = TRUE;
        } else if ((index == 36) || (index == 46)) {
            g->uniData[player].centers[STUDENT_MMONEY] = TRUE;
        } else if (index == 50) {
            g->uniData[player].centers[STUDENT_BQN] = TRUE;
        } else if ((index == 38) || (index == 39)) {
            g->uniData[player].centers[STUDENT_MJ] = TRUE;
        } else if (index == 7) {
            g->uniData[player].centers[STUDENT_BPS] = TRUE;
        }
        
        //Updating KPI
        g->uniData[player].KPICount += KPI_PER_CAMPUS;
        
        //Updating resources
        g->uniData[player].studentsInDegree[STUDENT_BPS] -= 1;
        g->uniData[player].studentsInDegree[STUDENT_BQN] -= 1;
        g->uniData[player].studentsInDegree[STUDENT_MJ] -= 1;
        g->uniData[player].studentsInDegree[STUDENT_MTV] -= 1;
        
    } else if (a.actionCode == BUILD_GO8) {
        assert(isLegalAction(g,a)&&"Tried to build an illegal GO8!");
        //Updating the map
        vertex targetVertex = a.targetVertex;
        vertexIndex index = vertexToIndex(targetVertex);
        //Increase value in index to indicate upgrade from campus to GO8
        g->vertices[index] += (GO8_CAMPUS_A-CAMPUS_A);
        
        //Updating player data
        g->uniData[player].campusCount--;
        g->uniData[player].GO8Count++;
        
        //Updating KPI
        g->uniData[player].KPICount -= KPI_PER_CAMPUS;
        g->uniData[player].KPICount += KPI_PER_GO8;
        //Updating resources
        g->uniData[player].studentsInDegree[STUDENT_MJ] -= 2;
        g->uniData[player].studentsInDegree[STUDENT_MMONEY] -=3;
        
        
    } else if (a.actionCode == CREATE_ARC) {
        assert(isLegalAction(g,a)&&"Tried to build an illegal arc!");
        //Updating the map
        arc targetARC = a.targetARC;
        arcIndex index = arcToIndex(targetARC);
        //Stores value of uni who owns the ARC in index
        g->arcs[index] = (player + 1);
        
        //Updating player data -- test if prestige changes hands
        g->uniData[player].arcGrantCount++;
        if (g->uniData[player].arcGrantCount >
            g->uniData[getMostARCs(g)].arcGrantCount) {
            
            g->uniData[getMostARCs(g)].KPICount -= MOST_ARCS_KPI;
            g->mostArcsPlayer = player;
            g->uniData[player].KPICount += MOST_ARCS_KPI;
        }
        
        //Updating KPI
        g->uniData[player].KPICount += KPI_PER_ARC;
        //Updating resources
        g->uniData[player].studentsInDegree[STUDENT_BPS] -= 1;
        g->uniData[player].studentsInDegree[STUDENT_BQN] -= 1;
        
    } else if (a.actionCode == OBTAIN_PUBLICATION) {
        action b;
        b.actionCode = START_SPINOFF;
        assert(isLegalAction(g,b)&&"Tried to obtain an illegal publication!");
        //Updating player data -- test if prestige changes hands
        g->uniData[player].publicationsCount++;
        if (g->uniData[player].publicationsCount >
            g->uniData[getMostPublications(g)].publicationsCount) {
            
            g->uniData[getMostPublications(g)].KPICount -= MOST_PUBS_KPI;
            g->mostPubsPlayer = player;
            g->uniData[player].KPICount += MOST_PUBS_KPI;
        }
        
        //Updating resources
        g->uniData[player].studentsInDegree[STUDENT_MJ] -= 1;
        g->uniData[player].studentsInDegree[STUDENT_MTV] -= 1;
        g->uniData[player].studentsInDegree[STUDENT_MMONEY] -= 1;
        
        
    } else if (a.actionCode == OBTAIN_IP_PATENT) {
        action b;
        b.actionCode = START_SPINOFF;
        assert(isLegalAction(g,b) && "Tried to obtain an illegal patent!");
        //Updating player data
        g->uniData[player].patentCount++;
        
        //Updating KPI
        g->uniData[player].KPICount += KPI_PER_PATENT;
        //Updating resources
        g->uniData[player].studentsInDegree[STUDENT_MJ] -= 1;
        g->uniData[player].studentsInDegree[STUDENT_MTV] -= 1;
        g->uniData[player].studentsInDegree[STUDENT_MMONEY] -= 1;
        
    } else if (a.actionCode == RETRAIN_STUDENTS) {
        assert(a.retrainFrom > STUDENT_THD && a.retrainFrom <= STUDENT_MMONEY && "Student types to retrain must be legal!");
        assert(a.retrainTo >= STUDENT_THD && a.retrainTo <= STUDENT_MMONEY && "Student types to retrain must be legal!");
        assert(isLegalAction(g,a)&&"Tried to retrain students illegally!");
        //Calculating exchange rate
        degree tradeFrom = a.retrainFrom;
        degree tradeTo = a.retrainTo;
        
        //Cost is how many students of degree tradeFrom are needed for one of degree tradeTo
        int cost = getExchangeRate(g, player, tradeFrom, tradeTo);
        
        //Subtracting number of students traded
        g->uniData[player].studentsInDegree[tradeFrom] -= cost;
        
        //Adding one student of degree tradeTo
        g->uniData[player].studentsInDegree[tradeTo]++;
        
    }
    //not checking if victory conditions have been met, as this should be done by playGame function
}

/* **** Functions which GET data about the Game aka GETTERS **** */

// true if the region is not one of the land regions of knowledge island
int isSea (Game g, region r) {
    assert(g != NULL&&"A test gave us a null game!");
    return !isLand(r);
}

// what type of students are produced by the specified land region?
// see degree discipline codes above
degree getDegree (Game g, region r) {
    assert(g != NULL&&"A test gave us a null game!");
    assert(isLand(r) && "A test asked for the degree type of a nonexistant tile");
    return g->map[regionToIndex(r)].degreeType;
}

// what dice value produces students in the specified land region?
// 2..12
int getDiceValue (Game g, region r) {
    assert(g != NULL&&"A test gave us a null game!");
    assert(isLand(r) && "A test asked for the dice value of a nonexistant tile");
    return g->map[regionToIndex(r)].hexValue;
}

// which university currently has the prestige award for the most ARCs?
// this deemed to be UNI_C at the start of the Game.
uni getMostARCs (Game g) {
    assert(g != NULL&&"A test gave us a null game!");
    return g->mostArcsPlayer;
}

// which university currently has the prestige award for the most pubs?
// this is deemed to be UNI_C at the start of the Game.
uni getMostPublications (Game g) {
    assert(g != NULL&&"A test gave us a null game!");
    return g->mostPubsPlayer;
}

// return the current turn number of the Game -1,0,1, ..
int getTurnNumber (Game g) {
    assert(g != NULL&&"A test gave us a null game!");
    return g->currentTurn;
}

// return the player id of the player whose turn it is
// the result of this function is UNI_C during Terra Nullis
uni getWhoseTurn (Game g) {
    assert(g != NULL&&"A test gave us a null game!");
    int whoseTurn;
    if (g->currentTurn == TERRA_NULLIS) {
        whoseTurn = UNI_C;
    } else {
        whoseTurn = (g->currentTurn)%NUM_UNIS;
    }
    return whoseTurn;
}

// return the contents of the given vertex (ie campus code or
// VACANT_VERTEX)
int getCampus (Game g, vertex v) {
    assert(g != NULL&&"A test gave us a null game!");
    assert(isVertex(v)&&"A test asked us to locate a nonexistant vertex");
    return g->vertices[vertexToIndex(v)];
}

// the contents of the given edge (ie ARC code or vacent ARC)
int getARC (Game g, arc a) {
    assert(g != NULL&&"A test gave us a null game!");
    assert(isArc(a)&&"A test asked us to locate a nonexistant arc");
    return g->arcs[arcToIndex(a)];
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
//

int isLegalAction (Game g, action a) {
    assert(g != NULL&&"A test gave us a null game!");
    if(!(a.actionCode >=PASS && a.actionCode <= RETRAIN_STUDENTS)) {
        printf("Action code does not exist!");
    }
    int isAllowed = FALSE;
    int player = getWhoseTurn(g);
    //This gets the amount of resources the player has
    int i = 0;
    int cardsInHand[NUM_DEGREES];
    while (i< NUM_DEGREES) {
        cardsInHand[i] = getStudents(g, player, i);
        i++;
    }
    if (a.actionCode == PASS) {
        isAllowed = TRUE;
    } else if (a.actionCode == BUILD_CAMPUS) {
        vertex buildHere = a.targetVertex;
        //checks if they have enough resources
        int resource = TRUE;
        i = 0;
        int campusCost[NUM_DEGREES] = CAMPUS_COST;
        while (i< NUM_DEGREES) {
            resource =  resource && (cardsInHand[i] >= campusCost[i]);
            i++;
        }
        int buildable = FALSE;
        int arcFound = FALSE;
        //checks if vertex is valid
        if (isVertex(buildHere)==TRUE) {
            //checks if the vertex is vacant
            buildable = (g->vertices[vertexToIndex(buildHere)] == VACANT_VERTEX);
            i = 0;
            int adj;
            //checks for adjacent vertices being vacant
            while (i < EDGES_PER_VERTEX) {
                adj = adjVertexToVertex(buildHere,i);
                if (adj != INVALID) {
                    buildable = buildable && (g->vertices[adj]==VACANT_VERTEX);
                }
                i++;
            }
            //checks for adjacent arcs having at least one owned
            i = 0;
            while (i < EDGES_PER_VERTEX && !arcFound) {
                adj = adjArcToVertex(buildHere,i);
                if (adj != INVALID) {
                    arcFound = arcFound || (g->arcs[adj]==player+ARC_A);
                }
                i++;
            }
        }
        if ((buildable == TRUE) && (resource == TRUE) && (arcFound == TRUE)) {
            isAllowed = TRUE;
        }
    } else if (a.actionCode== CREATE_ARC) {
        arc buildHere = a.targetARC;
        int resource = TRUE;
        i =  0;
        int arcCost[NUM_DEGREES] = ARC_COST;
        while (i< NUM_DEGREES) {
            resource =  resource && (cardsInHand[i] >= arcCost[i]);
            i++;
        }
        int buildable = FALSE;
        int adjFound = FALSE;
        //checks if arc is valid
        if (isArc(buildHere)==TRUE) {
            //checks if the arc is vacant
            buildable = (g->arcs[arcToIndex(buildHere)] == VACANT_ARC);
            i = 0;
            int adj;
            while (i < VERTICES_PER_EDGE) {
                adj = adjVertexToArc(buildHere,i);
                if (adj != INVALID) {
                    adjFound = adjFound || (g->vertices[adj]==player+CAMPUS_A) || (g->vertices[adj]==player+GO8_CAMPUS_A);
                }
                i++;
            }
            i = 0;
            while (i < VERTICES_PER_EDGE*VERTICES_PER_EDGE && !adjFound) {
                adj = adjArcToArc(buildHere,i);
                if (adj != INVALID) {
                    adjFound = adjFound || (g->arcs[adj]==player+ARC_A);
                }
                i++;
            }
        }
        if ((buildable == TRUE) && (resource == TRUE) && (adjFound == TRUE)) {
            isAllowed = TRUE;
        } else {
            //printf("Resources %d, Adjacent %d, Vacant %d \n",resource, adjFound, buildable);
        }
    } else if (a.actionCode == BUILD_GO8) {
        int resource = TRUE;
        int existingCampus = FALSE;
        vertex buildHere = a.targetVertex;
        i = 0;
        int GO8Cost[NUM_DEGREES] = GO8_COST;
        while (i< NUM_DEGREES) {
            resource =  resource && (cardsInHand[i] >= GO8Cost[i]);
            i++;
        }
        if (isVertex(buildHere)==TRUE) {
            //checks if the vertex is a campus belonging to the player
            existingCampus = (g->vertices[vertexToIndex(buildHere)] == CAMPUS_A+player);
        }
        i = 0;
        //counts the number of GO8 campuses
        int GO8count = 0;
        while (i < NUM_UNIS) {
            GO8count += getGO8s(g,i);
            i++;
        }
        if ( (GO8count < MAX_GO8_CAMPUS) && (resource ==TRUE) && (existingCampus == TRUE)) {
            isAllowed = TRUE;
        }
    } else if (a.actionCode == START_SPINOFF) {
        int resource = TRUE;
        int spinCost[NUM_DEGREES] = SPINOFF_COST;
        i = 0;
        while (i< NUM_DEGREES) {
            resource =  resource && (cardsInHand[i] >= spinCost[i]);
            i++;
        }
        isAllowed = resource;
    } else if (a.actionCode == RETRAIN_STUDENTS) {
        degree tradeFrom = a.retrainFrom;
        degree tradeTo = a.retrainTo;
        if ((tradeFrom > STUDENT_THD) && (tradeFrom <= STUDENT_MMONEY) &&
            (tradeTo >=STUDENT_THD) && (tradeTo<=STUDENT_MMONEY)) {
            int cost = getExchangeRate(g, player, tradeFrom, tradeTo);
            if (cardsInHand[tradeFrom] >= cost ) {
                // Has more than or equal to the amount of cards that is needed to trade
                isAllowed = TRUE;
            }
        } else {
            isAllowed = FALSE;
        }
        
    } else if (a.actionCode == OBTAIN_PUBLICATION || a.actionCode==OBTAIN_IP_PATENT) {
        //printf("The current university tried to obtain a publication or IP patent directly \n");
        isAllowed = FALSE;
    }
    // checks if the current turn is valid, turn != -1. (as turn = -1 at the very start)
    if (getTurnNumber(g) < 0) {
        isAllowed = FALSE;
    }
    return isAllowed;
}

// --- get data about a specified player ---

// return the number of KPI points the specified player currently has
int getKPIpoints (Game g, uni player) {
    assert(g != NULL&&"A test gave us a null game!");
    assert(player >= UNI_A && player <= UNI_C && "Player was an illegal player");
    return g->uniData[player].KPICount;
}

// return the number of ARC grants the specified player currently has
int getARCs (Game g, uni player) {
    assert(g != NULL&&"A test gave us a null game!");
    assert(player >= UNI_A && player <= UNI_C && "Player was an illegal player");
    return g->uniData[player].arcGrantCount;
}

// return the number of GO8 campuses the specified player currently has
int getGO8s (Game g, uni player) {
    assert(g != NULL&&"A test gave us a null game!");
    assert(player >= UNI_A && player <= UNI_C && "Player was an illegal player");
    return g->uniData[player].GO8Count;
}

// return the number of normal Campuses the specified player currently has
int getCampuses (Game g, uni player) {
    assert(g != NULL&&"A test gave us a null game!");
    assert(player >= UNI_A && player <= UNI_C && "Player was an illegal player");
    return g->uniData[player].campusCount;
}

// return the number of IP Patents the specified player currently has
int getIPs (Game g, uni player) {
    assert(g != NULL&&"A test gave us a null game!");
    assert(player >= UNI_A && player <= UNI_C && "Player was an illegal player");
    return g->uniData[player].patentCount;
}

// return the number of Publications the specified player currently has
int getPublications (Game g, uni player) {
    assert(g != NULL&&"A test gave us a null game!");
    assert(player >= UNI_A && player <= UNI_C && "Player was an illegal player");
    return g->uniData[player].publicationsCount;
}

// return the number of students of the specified discipline type
// the specified player currently has
int getStudents (Game g, uni player, degree discipline) {
    assert(g != NULL&&"A test gave us a null game!");
    assert(player >= UNI_A && player <= UNI_C && "Player was an illegal player");
    assert(discipline >= STUDENT_THD && discipline <= STUDENT_MMONEY && "Test gave us illegal student type!");
    return g-> uniData[player].studentsInDegree[discipline];
}

// return how many students of degree type tradeFrom
// the specified player would need to retrain in order to get one
// student of degree type trainTo.  This will depend
// on what retraining centers, if any, they have a campus at.
int getExchangeRate (Game g, uni player, degree tradeFrom, degree tradeTo) {
    assert(g != NULL&&"A test gave us a null game!");
    assert(player >= UNI_A && player <= UNI_C && "Player was an illegal player");
    assert(tradeFrom >= STUDENT_THD && tradeFrom <= STUDENT_MMONEY && "Trading from nonexistant student type!");
    assert(tradeTo >= STUDENT_THD && tradeTo <= STUDENT_MMONEY && "Trading to nonexistant student type!");
    int rate = DEFAULT_EXCHANGE_RATE - g->uniData[player].centers[tradeFrom];
    return rate;
}

// Helper Functions

static int isLand (region r) {
    return (abs(r.x)<=MAX_REGION_COORD&&abs(r.y)<=MAX_REGION_COORD&&abs(r.x+r.y)<=MAX_REGION_COORD);
}

static int isArc (arc a) {
    int valid;
    if (!isLand(a.region0)&&!isLand(a.region1)) {
        valid = FALSE;
    } else {
        int xDiff = a.region0.x - a.region1.x;
        int yDiff = a.region0.y - a.region1.y;
        if ((xDiff == 0 && abs(yDiff)==1)||
            (yDiff == 0 && abs(xDiff)==1)||
            (xDiff * yDiff == -1)) {
            valid = TRUE;
        } else {
            valid = FALSE;
        }
    }
    return valid;
}

static int isVertex (vertex v) {
    int valid;
    if (!(isLand(v.region0)||isLand(v.region1)||isLand(v.region2))) {
        valid = FALSE;
    } else {
        valid = TRUE;
        int x0, x1, x2;
        int y0, y1, y2;
        x0 = v.region0.x;
        x1 = v.region1.x;
        x2 = v.region2.x;
        y0 = v.region0.y;
        y1 = v.region1.y;
        y2 = v.region2.y;
        if (checkOrder(x0,x1,x2)) {
            if (checkOrder(y0,y2,y1) ||checkOrder (y1,y2,y0) ) {
                valid = TRUE;
            } else {
                valid = FALSE;
            }
        } else if (checkOrder(x0,x2,x1)) {
            if (checkOrder(y0,y1,y2) ||checkOrder (y2,y1,y0) ) {
                valid = TRUE;
            } else {
                valid = FALSE;
            }
        } else if (checkOrder(x2,x1,x0)) {
            if (checkOrder(y2,y0,y1) ||checkOrder (y1,y0,y2) ) {
                valid = TRUE;
            } else {
                valid = FALSE;
            }
        } else {
            valid = FALSE;
        }
        if (abs(x0-x1+y0-y1)>1||abs(x0-x2+y0-y2)>1||abs(x2-x1+y2-y1)>1) {
            valid = FALSE;
        }
    }
    return valid;
}

static int checkOrder (int a, int b, int c) {
    return (a==b && abs(a-c)==1 && abs(b-c)==1);
}

//INDEX finding functions

static int regionToIndex (region r) {
    assert(isLand(r)==TRUE);
    int index = 0;
    int x = -MAX_REGION_COORD;
    while (x < r.x) {
        //adds the maximum coordinate in column minus minimum coord in column
        index += (maxYCoord(x,MAX_REGION_COORD)-minYCoord(x, MAX_REGION_COORD)+1);
        x++;
    }
    int y = minYCoord(x, MAX_REGION_COORD);
    while (y < r.y) {
        index++;
        y++;
    }
    return index;
}

static int arcToIndex (arc a) {
    assert(isArc(a)==TRUE);
    int index = 0;
    int ax = a.region0.x+a.region1.x;
    int ay = a.region0.y+a.region1.y;
    int x = -MAX_ARC_COORD;
    while (x < ax) {
        //adds the maximum coordinate in column minus minimum coord in column
        if (x%2==0) {
            index += (maxYCoord(x,MAX_ARC_COORD)-minYCoord(x, MAX_ARC_COORD))/2+1;
        } else {
            index += (maxYCoord(x,MAX_ARC_COORD)-minYCoord(x, MAX_ARC_COORD)+1);
        }
        x++;
    }
    int y = minYCoord(x, MAX_ARC_COORD);
    while (y < ay) {
        index++;
        if (x%2==0) {
            y+=2;
        } else {
            y++;
        }
    }
    return index;
}

static int vertexToIndex (vertex v) {
    int index = 0;
    int x = -MAX_REGION_COORD;
    int vx = max(v.region0.x,max(v.region1.x,v.region2.x));
    int vy = max(v.region0.y+v.region1.y,max(v.region0.y+v.region2.y,v.region2.y+v.region1.y));
    while (x < vx) {
        //adds the maximum coordinate in column minus minimum coord in column
        index += (maxYCoord(2*x-1,6)-minYCoord(2*x-1, 5)+1);
        x++;
    }
    int y = minYCoord(2*x-1, 5);
    assert(isVertex(v)==TRUE);
    while (y < vy) {
        index++;
        y++;
    }
    return index;
}

static region indexToRegion (int index) {
    int pointIndex = 0;
    int x = -MAX_REGION_COORD;
    int yRange = maxYCoord(x,MAX_REGION_COORD)-minYCoord(x, MAX_REGION_COORD)+1;
    while (pointIndex + yRange <= index ) {
        x++;
        pointIndex += yRange;
        //adds the maximum coordinate in column minus minimum coord in column
        yRange = (maxYCoord(x,MAX_REGION_COORD)-minYCoord(x, MAX_REGION_COORD)+1);
    }
    int y = minYCoord(x, MAX_REGION_COORD);
    while (pointIndex < index) {
        pointIndex++;
        y++;
    }
    region r;
    assert(isLand(r)==TRUE);
    r.x = x;
    r.y = y;
    return r;
}

static vertexIndex adjVertexToRegion (region r, int i) {
    assert(isLand(r)==TRUE);
    int vx;
    int vy;
    if (i>=3) {
        vx = r.x+1;
        vy = r.y*2 + (i%3) - 1;
    } else {
        vy = r.y*2 + (i%3);
        vx = r.x;
    }
    int x = -MAX_REGION_COORD;
    int index = 0;
    while (x < vx) {
        //adds the maximum coordinate in column minus minimum coord in column
        index += (maxYCoord(2*x-1,6)-minYCoord(2*x-1, 5)+1);
        x++;
    }
    int y = minYCoord(2*x-1, 5);
    while (y < vy) {
        index++;
        y++;
    }
    if ((x<-MAX_REGION_COORD)||(x>(MAX_REGION_COORD+1))|| (y>maxYCoord(2*x-1,6))||(y<minYCoord(2*x-1, 5))) {
        index = INVALID;
    }
    return index;
}

static vertexIndex adjVertexToVertex (vertex v, int i) {
    assert(isVertex(v)==TRUE);
    int vx = max(v.region0.x,max(v.region1.x,v.region2.x));
    int vy = max(v.region0.y+v.region1.y,max(v.region0.y+v.region2.y,v.region2.y+v.region1.y));
    if (i==0) {
        vy--;
    } else if (i==1) {
        vy++;
    } else if (i==2) {
        if (vy%2==0) {
            vy--;
            vx++;
        } else {
            vy++;
            vx--;
        }
    }
    
    region r0,r1,r2;
    if (vy %2 ==0 ) {
        r0.y = vy/2;
        r1.y = vy/2;
        r2.y = vy/2 - 1;
        r0.x = vx;
        r1.x = vx-1;
        r2.x = vx;
    } else {
        r0.y = (vy+1)/2;
        r1.y = (vy-1)/2;
        r2.y = (vy-1)/2;
        r0.x = vx-1;
        r1.x = vx-1;
        r2.x = vx;
    }
    vertex adjV = {.region0 = r0, .region1 = r1, .region2 = r2};
    int index;
    if (isVertex(adjV)) {
        index = vertexToIndex(adjV);
    } else {
        index = INVALID;
    }
    return index;
}

static arcIndex adjArcToVertex (vertex v, int i) {
    assert(isVertex(v)==TRUE);
    arc a;
    int index;
    //this function is TBC
    if (i==0) {
        a.region0 = v.region0;
        a.region1 = v.region1;
    } else if (i == 1) {
        a.region0 = v.region0;
        a.region1 = v.region2;
    } else if (i==2) {
        a.region0 = v.region1;
        a.region1 = v.region2;
    }
    if (isArc (a)) {
        index = arcToIndex(a);
    } else {
        index = INVALID;
    }
    return index;
}


static arcIndex adjArcToArc(arc a,int i) {
    assert(isArc(a)==TRUE);
    region r;
    arc a2;
    int index;
    if (a.region0.x==a.region1.x) {
        if (i>=2) {
            r.x = a.region0.x+1;
            r.y = min(a.region0.y,a.region1.y);
        } else {
            r.x = a.region0.x-1;
            r.y = max(a.region0.y,a.region1.y);
        }
    } else if (a.region0.y==a.region1.y) {
        if (i>=2) {
            r.y = a.region0.y+1;
            r.x = min(a.region0.x,a.region1.x);
        } else {
            r.y = a.region0.y-1;
            r.x = max(a.region0.x,a.region1.x);
        }
    } else {
        if (i>=2) {
            r.y = max(a.region0.y,a.region1.y);
            r.x = max(a.region0.x,a.region1.x);
        } else {
            r.y = min(a.region0.y,a.region1.y);
            r.x = min(a.region0.x,a.region1.x);
        }
    }
    a2.region0 = r;
    if (i%2==0) {
        a2.region1 = a.region0;
    } else {
        a2.region1 = a.region1;
    }
    if (isArc (a2)) {
        index = arcToIndex(a2);
    } else {
        index = INVALID;
    }
    return index;
}

static vertexIndex adjVertexToArc(arc a,int i) {
    assert(isArc(a)==TRUE);
    region r;
    vertex v;
    if (a.region0.x==a.region1.x) {
        if (i==1) {
            r.x = a.region0.x+1;
            r.y = min(a.region0.y,a.region1.y);
        } else {
            r.x = a.region0.x-1;
            r.y = max(a.region0.y,a.region1.y);
        }
    } else if (a.region0.y==a.region1.y) {
        if (i==1) {
            r.y = a.region0.y+1;
            r.x = min(a.region0.x,a.region1.x);
        } else {
            r.y = a.region0.y-1;
            r.x = max(a.region0.x,a.region1.x);
        }
    } else {
        if (i==1) {
            r.y = max(a.region0.y,a.region1.y);
            r.x = max(a.region0.x,a.region1.x);
        } else {
            r.y = min(a.region0.y,a.region1.y);
            r.x = min(a.region0.x,a.region1.x);
        }
    }
    v.region0 = r;
    v.region1 = a.region0;
    v.region2 = a.region1;
    int index;
    if (isVertex(v)) {
        index = vertexToIndex(v);
    } else {
        index = INVALID;
    }
    return index;
}


static int min (int a, int b) {
    int min;
    if (a<b) {
        min=a;
    } else {
        min=b;
    }
    return min;
}

static int max (int a, int b) {
    int max;
    if (a>b) {
        max=a;
    } else {
        max=b;
    }
    return max;
}

static int minYCoord (int xval, int limit) {
    return max(-limit-xval,-limit);
}

static int maxYCoord (int xval, int limit) {
    return min(limit,limit-xval);
}
