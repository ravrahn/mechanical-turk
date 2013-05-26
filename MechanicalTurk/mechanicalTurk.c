//
//  mechanicalTurk.c
//  KnowledgeIsland
//
//  Created by Owen Cassidy and Daniel Parker on 21/05/13 - 25/05/13
//  Copyright (c) 2013 Owen Cassidy. All rights reserved.
//

/* TODO
 **** verticesAroundArc
 **** verticesAroundVertex
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mechanicalTurk.h"

#define MAX_ARCS 100
#define MAX_CAMPUSES 100
#define MAX_VERTICES 100

#define MIN_COORD -3
#define MAX_COORD 3

#define TOTAL_ARCS 62

#define TRUE 1
#define FALSE 0

#define HORIZONTAL 0
#define FORWARDS 1
#define BACKWARDS 2

typedef struct _arcs {
    arc arcs[MAX_ARCS];
    int amountOfArcs;
} arcs;

typedef struct _vertices {
    vertex vertices[MAX_VERTICES];
    int amountOfVertices;
} vertices;


typedef struct spatialInfo{
    int regionDiceValues[NUM_REGIONS];
    int regionDegreeTypes[NUM_REGIONS];
    vertex campusLocations[NUM_UNIS][MAX_CAMPUSES];
    arc arcLocations[NUM_UNIS][MAX_ARCS];
} spatialInfo;

static spatialInfo retriveInfo(Game g);

// Returns the best action to make in this turn.
static action chooseAction(Game g);

// Returns the best vertex on which to build a campus
// or illegalVertex() if there is none.
static vertex chooseCampus(Game g);

// Returns the best normal campus to convert to a Go8 campus
// or illegalVertex() if there is none.
static vertex chooseGO8(Game g);

// Returns the best place to build an ARC
// or illegalArc() if there is none.
static arc chooseArc(Game g);

// Returns all the ARCs adjacent a given vertex.
static arcs arcsAroundVertex(vertex v);

// Returns all the ARCs adjacent to a given ARC.
static arcs arcsAroundArc(arc a);


// Returns all vertices adjacent to a given vertex.
static vertices verticesAroundVertex(vertex v);

// Returns all vertices adjacent to a given ARC.
static vertices verticesAroundArc(arc a);


// Checks if two vertices are equal.
static int verticesAreEqual(vertex a, vertex b);

// Checks if two ARCs are equal.
static int arcsAreEqual(arc a, arc b);

// Checks if two regions are equal.
static int regionsAreEqual(region a, region b);

// Checks if two regions are adjacent.
static int regionsAreAdjacent(region a, region b);

// Returns an impossible vertex
static vertex illegalVertex(void);

// Returns an impossible ARC
static arc illegalArc(void);

// Checks if a vertex can be built on.
static int isLegalVertex(Game g, vertex v);

// Checks if an arc can be built on.
static int isLegalArc(Game g, arc a);

// Checks if a vertex is on the board.
static int isRealVertex(Game g, vertex v);

// Checks if an arc is on the board.
static int isRealArc(Game g, arc a);


// Returns an array of all ARCs owned by a given player.
static arcs ownedArcs(Game g, uni me);

// Returns an array of all campuses owned by a given player.
static vertices ownedCampuses(Game g, uni me);


// Checks if player has resources to perform an action.
static int canAfford(Game g, int actionCode);


// Returns which way an arc is facing.
// / - FORWARDS
// \ - BACKWARDS
// - - HORIZONTAL
static int whichWay(arc a);

// Returns a vertex based on a region:
//         ____
//        /    \
//   ____/  r1  \
//  /    \      /
// /  r0  \____/
// \  in  /    \
//  \____/  r2  \
//       \      /
//        \____/
static vertex rightVertexFromRegion(region r);

// Returns a vertex based on a region:
//    ____
//   /    \
//  /  r1  \____
//  \      /    \
//   \____/  r0  \
//   /    \  in  /
//  /  r2  \____/
//  \      /
//   \____/
static vertex leftVertexFromRegion(region r);

// Returns an array of all legal vertices.
static vertices getAllVertices(Game g);


action bestMove(Game g) {
    action bestMove;
    
    bestMove = chooseAction(g);
    
    return bestMove;
}

static action chooseAction(Game g){
    
    action legalAction;
    
    vertex chosenCampus;
    arc chosenArc;
    vertex chosenGO8;
    
    chosenGO8 = chooseGO8(g);
    
    if(!verticesAreEqual(illegalVertex(), chosenGO8) &&
       canAfford(g, BUILD_GO8)){
        
        legalAction.actionCode = BUILD_GO8;
        legalAction.targetVertex = chosenGO8;
        
    } else {
        chosenCampus = chooseCampus(g);
        
        printf("I chose (%d, %d), (%d, %d), (%d, %d)\n", chosenCampus.region0.x, chosenCampus.region0.y, chosenCampus.region1.x, chosenCampus.region1.y, chosenCampus.region2.x, chosenCampus.region2.y);
        
        if(!verticesAreEqual(chosenCampus, illegalVertex()) &&
           canAfford(g, BUILD_CAMPUS)){
        
            legalAction.actionCode = BUILD_CAMPUS;
            legalAction.targetVertex = chosenCampus;
            
        } else {
            if(canAfford(g, START_SPINOFF)){
        
                legalAction.actionCode = START_SPINOFF;
                
            } else {
                chosenArc = chooseArc(g);
                
                printf("I chose (%d, %d), (%d, %d)\n", chosenArc.region0.x, chosenArc.region0.y, chosenArc.region1.x, chosenArc.region1.y);
                
                if(canAfford(g, CREATE_ARC) &&
                   !arcsAreEqual(chosenArc, illegalArc())){
                
                    
                    
                    legalAction.actionCode = CREATE_ARC;
                    legalAction.targetARC = chooseArc(g);
                    
                } else {
                    
                    legalAction.actionCode = PASS;
        
                }
            }
        }
    }
    
    return legalAction;
}

static vertex chooseGO8(Game g){
    vertex legalVertex = illegalVertex();
    vertex testVertex;
    uni me = getTurnNumber(g);
    
    //Region Coordinates lie between -2 and 2.
    if (canAfford(g, BUILD_GO8)){
        
        if (ownedCampuses(g, me).amountOfVertices != 0) {
            testVertex = ownedCampuses(g, me).vertices[0];
        }
    }
    return legalVertex;
}

static vertex chooseCampus(Game g){
    vertex legalVertex = illegalVertex();
    vertices testVertices;
    uni me = getTurnNumber(g);
    
    //Region Coordinates lie between -2 and 2.
    if (canAfford(g, BUILD_CAMPUS)){
        
        arcs mArcs = ownedArcs(g, me);
        int arcCount = 0;
        
        while (arcCount < mArcs.amountOfArcs && verticesAreEqual(legalVertex, illegalVertex())){
            testVertices = verticesAroundArc(mArcs.arcs[arcCount]);
            
            if (isLegalVertex(g, testVertices.vertices[0])){
                legalVertex = testVertices.vertices[0];
            } else if (isLegalVertex(g, testVertices.vertices[1])){
                legalVertex = testVertices.vertices[1];
            } else if (isLegalVertex(g, testVertices.vertices[2])){
                legalVertex = testVertices.vertices[2];
            }
            arcCount++;
        }
    }
    return legalVertex;
}

static arc chooseArc(Game g) {
    arc legalArc = illegalArc();
    uni me = getTurnNumber(g);
    arcs testArcs;
    arcs mArcs;
    vertices mCampuses;
    int arcCount;
    int campusCount;
    int i;
    
    mArcs = ownedArcs(g, me);
    if (mArcs.amountOfArcs ==  0) {
        mCampuses = ownedCampuses(g, me);
        
        campusCount = 0;
        
        while (campusCount < mCampuses.amountOfVertices &&
               arcsAreEqual(legalArc, illegalArc())) {
            
            testArcs = arcsAroundVertex(mCampuses.vertices[campusCount]);

            i = 0;
            while (i < testArcs.amountOfArcs) {
                if (isLegalArc(g, testArcs.arcs[i]) &&
                    arcsAreEqual(legalArc, illegalArc())) {
                    legalArc = testArcs.arcs[i];
                }
                i++;
            }
            
            campusCount++;
            
        }
        
    } else {
        arcCount = 0;
        
        while (arcCount < mArcs.amountOfArcs &&
               arcsAreEqual(legalArc, illegalArc())) {
            
            testArcs = arcsAroundArc(mArcs.arcs[arcCount]);

            i = 0;
            while (i < testArcs.amountOfArcs) {
                if (isLegalArc(g, testArcs.arcs[i]) &&
                    arcsAreEqual(legalArc, illegalArc())) {
                    legalArc = testArcs.arcs[i];
                }
                i++;
            }
            
            arcCount++;
        }
    }
    
    return legalArc;
}

static arcs arcsAroundVertex(vertex v) {
    arc a;
    arc b;
    arc c;
    arcs arr;
    
    a.region0 = v.region0;
    a.region1 = v.region1;
    
    b.region0 = v.region1;
    b.region1 = v.region2;
    
    c.region0 = v.region2;
    c.region0 = v.region0;
    
    arr.arcs[0] = a;
    arr.arcs[1] = b;
    arr.arcs[2] = c;
    
    arr.amountOfArcs = 3;
    
    return arr;
}

static arcs arcsAroundArc(arc a) {
    arcs arr;
    
    region r0;
    region r1;
    region r2;
    region r3;
    
    arc a1;
    arc a2;
    arc a3;
    arc a4;
    
    int facing;
    
    
    facing = whichWay(a);
    
    if (facing == HORIZONTAL) {
        if (a.region0.x > a.region1.x) {
            r1 = a.region0;
            r2 = a.region1;
        } else {
            r1 = a.region1;
            r2 = a.region0;
        }
        
        r0 = r1;
        r0.y -= 1;
        
        r3 = r2;
        r3.y += 1;
        
        a1.region0 = r0;
        a1.region1 = r1;
        
        a2.region0 = r0;
        a2.region1 = r2;
        
        a3.region0 = r1;
        a3.region0 = r3;
        
        a4.region0 = r2;
        a4.region1 = r3;
        
    } else if (facing == FORWARDS) {
        if (a.region0.x > a.region1.x) {
            r0 = a.region0;
            r3 = a.region1;
        } else {
            r0 = a.region1;
            r3 = a.region0;
        }
        
        r1 = r0;
        r1.y += 1;
        
        r2 = r3;
        r2.y -= 1;
        
        a1.region0 = r0;
        a1.region1 = r2;
        
        a2.region0 = r3;
        a2.region1 = r2;
        
        a3.region0 = r0;
        a3.region1 = r1;
        
        a4.region0 = r1;
        a4.region0 = r3;
        
    } else if (facing == BACKWARDS) {
        if (a.region0.y < a.region1.y) {
            r1 = a.region0;
            r2 = a.region1;
        } else {
            r1 = a.region1;
            r2 = a.region0;
        }
        r0 = r1;
        r0.x += 1;
        
        r3 = r2;
        r3.x -= 1;
        
        a1.region0 = r0;
        a1.region1 = r1;
        
        a2.region0 = r1;
        a2.region1 = r3;
        
        a3.region0 = r2;
        a3.region1 = r3;
        
        a4.region0 = r0;
        a4.region1 = r2;
    }
    
    arr.arcs[0] = a1;
    arr.arcs[1] = a2;
    arr.arcs[2] = a3;
    arr.arcs[3] = a4;
    
    arr.amountOfArcs = 4;
    
    return arr;
}

static vertices verticesAroundVertex(vertex v) {
    vertices arr;
    
    
    
    arr.amountOfVertices = 3;
    
    return arr;
}

static vertices verticesAroundArc(arc a) {
    vertices arr;
    
    region r0;
    region r1;
    region r2;
    region r3;
    
    vertex v0;
    vertex v1;
    
    int facing;
    
    
    facing = whichWay(a);
    
    if (facing == HORIZONTAL) {
        if (a.region0.x > a.region1.x) {
            r1 = a.region0;
            r2 = a.region1;
        } else {
            r1 = a.region1;
            r2 = a.region0;
        }
        
        r0 = r1;
        r0.y -= 1;
        
        r3 = r2;
        r3.y += 1;
        
        v0.region0 = r0;
        v0.region1 = r1;
        v0.region2 = r2;
        
        v1.region0 = r1;
        v1.region1 = r2;
        v1.region2 = r3;
        
    } else if (facing == FORWARDS) {
        if (a.region0.x > a.region1.x) {
            r0 = a.region0;
            r3 = a.region1;
        } else {
            r0 = a.region1;
            r3 = a.region0;
        }
        
        r1 = r0;
        r1.y += 1;
        
        r2 = r3;
        r2.y -= 1;
        
        v0.region0 = r0;
        v0.region1 = r1;
        v0.region2 = r2;
        
        v1.region0 = r1;
        v1.region1 = r2;
        v1.region2 = r3;
        
    } else if (facing == BACKWARDS) {
        if (a.region0.y < a.region1.y) {
            r1 = a.region0;
            r2 = a.region1;
        } else {
            r1 = a.region1;
            r2 = a.region0;
        }
        r0 = r1;
        r0.x += 1;
        
        r3 = r2;
        r3.x -= 1;
        
        v0.region0 = r0;
        v0.region1 = r1;
        v0.region2 = r3;
        
        v1.region0 = r0;
        v1.region1 = r2;
        v1.region2 = r3;
    }

    
    arr.amountOfVertices = 2;
    
    return arr;
}
    
//Takes in 2 vertices, checks if they are equal.
static int verticesAreEqual(vertex a, vertex b){
    int result = FALSE;
    
    if (regionsAreEqual(a.region0, b.region0)) {
        if (regionsAreEqual(a.region1, b.region1)) {
            if (regionsAreEqual(a.region2, b.region2)) { // 0=0, 1=1, 2=2
                result = TRUE;
            } else {
                result = FALSE;
            }
        } else if (regionsAreEqual(a.region1, b.region2)) {
            if (regionsAreEqual(a.region2, b.region1)) { // 0=0, 1=2, 2=1
                result = TRUE;
            } else {
                result = FALSE;
            }
        } else {
            result = FALSE;
        }
    }  else if (regionsAreEqual(a.region0, b.region1)) {
        if (regionsAreEqual(a.region1, b.region0)) {
            if (regionsAreEqual(a.region2, b.region2)) { // 0=1, 1=0, 2=2
                result = TRUE;
            } else {
                result = FALSE;
            }
        } else if (regionsAreEqual(a.region1, b.region2)) {
            if (regionsAreEqual(a.region2, b.region0)) { // 0=1, 1=2, 2=0
                result = TRUE;
            } else {
                result = FALSE;
            }
        } else {
            result = FALSE;
        }
    } else if (regionsAreEqual(a.region0, b.region2)) {
        if (regionsAreEqual(a.region1, b.region0)) {
            if (regionsAreEqual(a.region2, b.region1)) { // 0=2, 1=0, 2=1
                result = TRUE;
            } else {
                result = FALSE;
            }
        } else if (regionsAreEqual(a.region1, b.region1)) {
            if (regionsAreEqual(a.region2, b.region0)) { // 0=2, 1=1, 2=0
                result = TRUE;
            } else {
                result = FALSE;
            }
        } else {
            result = FALSE;
        }
    } else {
        result = FALSE;
    }
    
    return result;
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

static int regionsAreEqual(region a, region b){
    int result;
    
    if ((a.x == b.x) && (a.y == b.y)){
        result = TRUE;
    } else {
        result = FALSE;
    }
    
    return result;
}

static vertex illegalVertex(void) {
    vertex v;
    v.region0.x = 4;
    v.region0.y = 4;
    v.region1.x = 4;
    v.region1.y = 4;
    v.region2.x = 4;
    v.region2.y = 4;
    
    return v;
}

static arc illegalArc(void) {
    arc a;
    a.region0.x = 4;
    a.region0.y = 4;
    a.region1.x = 4;
    a.region1.y = 4;
    
    return a;
}

static int isLegalVertex(Game g, vertex v) {
    int isLegal;
    
    int hasAdjacentCampus;
    int hasAdjacentArc;
    
    vertices adjacentVertices;
    arcs adjacentArcs;
    
    int i;
    
    adjacentVertices = verticesAroundVertex(v);
    
    hasAdjacentCampus = FALSE;
    
    i = 0;
    while (i < adjacentVertices.amountOfVertices) {
        
        if ((getCampus(g, adjacentVertices.vertices[i]) != VACANT_VERTEX)) {
            
                hasAdjacentCampus = TRUE;
        }
        
        i++;
    }
    
    adjacentArcs = arcsAroundVertex(v);
    
    hasAdjacentArc = FALSE;
    
    i = 0;
    while (i < adjacentArcs.amountOfArcs) {
        
        if ((getARC(g, adjacentArcs.arcs[i]) == getTurnNumber(g) + 1)) {
                hasAdjacentArc = TRUE;
            }
        
        i++;
    }
    
    isLegal = isRealVertex(g, v) && hasAdjacentArc && !hasAdjacentCampus;
    
    return isLegal;
}

static int isLegalArc(Game g, arc a) {
    int isLegal;
    
    int hasAdjacentArc;
    int hasAdjacentCampus;
    
    arcs adjacentArcs;
    vertices adjacentVertices;
    
    uni me = getTurnNumber(g);
    
    int i;
    
    hasAdjacentCampus = FALSE;
    adjacentVertices = verticesAroundArc(a);
    
    i = 0;
    while (i < adjacentVertices.amountOfVertices) {
        
        if (getCampus(g, adjacentVertices.vertices[i]) == me+1 ||
            getCampus(g, adjacentVertices.vertices[i]) == me+4) {
            
            hasAdjacentCampus = TRUE;
        }
        
        i++;
    }

    
    adjacentArcs = arcsAroundArc(a);
    hasAdjacentArc = FALSE;
    
    i = 0;
    while (i < adjacentArcs.amountOfArcs) {
        
        if ((getARC(g, adjacentArcs.arcs[i]) == getTurnNumber(g) + 1)) {
            hasAdjacentArc = TRUE;
        }
        
        i++;
    }
    
    isLegal = isRealArc(g, a) && (hasAdjacentArc || hasAdjacentCampus);
    
    return isLegal;
}

static int isRealVertex(Game g, vertex v) {
    int isReal;
    
    int isOnBoard;
    int exists;
    int hasDistinctRegions;
    
    if (isSea(g, v.region0) &&
        isSea(g, v.region1) &&
        isSea(g, v.region2)) {
        isOnBoard = FALSE;
    } else {
        isOnBoard = TRUE;
    }
    
    if (regionsAreAdjacent(v.region0, v.region1) &&
        regionsAreAdjacent(v.region0, v.region2) &&
        regionsAreAdjacent(v.region1, v.region2)) {
        exists = TRUE;
    } else {
        exists = FALSE;
    }
    
    if (!regionsAreEqual(v.region0, v.region1) &&
        !regionsAreEqual(v.region1, v.region2) &&
        !regionsAreEqual(v.region2, v.region0)) {
        hasDistinctRegions = TRUE;
    } else {
        hasDistinctRegions = FALSE;
    }
    
    isReal = exists && isOnBoard && hasDistinctRegions;
    
    return isReal;
}

static int isRealArc(Game g, arc a) {
    int isReal;
    
    int isOnBoard;
    int exists;
    int hasDistinctRegions;
    
    if (isSea(g, a.region0) &&
        isSea(g, a.region1)) {
        isOnBoard = FALSE;
    } else {
        isOnBoard = TRUE;
    }
    
    if (regionsAreAdjacent(a.region0, a.region1)) {
        exists = TRUE;
    } else {
        exists = FALSE;
    }
    
    hasDistinctRegions = !regionsAreEqual(a.region0, a.region1);
    
    isReal = isOnBoard && exists && hasDistinctRegions;
    
    return isReal;
}

static arcs ownedArcs(Game g, uni me) {
    arcs result;
    arc a;
    int j;
    int alreadyCounted;
    
    a.region0.x = MIN_COORD;
    a.region0.y = MIN_COORD;
    a.region1.x = MIN_COORD;
    a.region1.y = MIN_COORD;
    
    result.amountOfArcs = 0;
    while (a.region0.x <= MAX_COORD) {
        while (a.region0.y <= MAX_COORD) {
            while (a.region1.x <= MAX_COORD) {
                while (a.region1.y <= MAX_COORD) {
                    
                    j = 0;
                    alreadyCounted = FALSE;
                    while (j < result.amountOfArcs) {
                        if (arcsAreEqual(result.arcs[j], a)) {
                            alreadyCounted = TRUE;
                        }
                        j++;
                    }
                    
                    if (getARC(g, a) == me+1 &&
                        isRealArc(g, a) &&
                        !alreadyCounted) {
                        printf("arc found: (%d, %d), (%d, %d)\n", a.region0.x, a.region0.y, a.region1.x, a.region1.y);
                        result.arcs[result.amountOfArcs] = a;
                        result.amountOfArcs++;
                    }
                    
                    a.region1.y++;
                }
                a.region1.y = MIN_COORD;
                a.region1.x++;
            }
            a.region1.x = MIN_COORD;
            a.region0.y++;
        }
        a.region0.y = MIN_COORD;
        a.region0.x++;
    }
    
    assert(getARCs(g, me) == result.amountOfArcs);
    
    return result;
}

static vertices ownedCampuses(Game g, uni me) {
    vertices result;
    vertices allVertices;
    int i;
    int j;
    int alreadyCounted;
    
    allVertices = getAllVertices(g);
    
    i = 0;
    while (i < allVertices.amountOfVertices) {
        if ((getCampus(g, allVertices.vertices[i]) == me+1 ||
            getCampus(g, allVertices.vertices[i]) == me+4) &&
            isRealVertex(g, allVertices.vertices[i])) {
            
            j = 0;
            alreadyCounted = FALSE;
            while (j < result.amountOfVertices) {
                if (verticesAreEqual(result.vertices[j], allVertices.vertices[i])) {
                    alreadyCounted = TRUE;
                }
                j++;
            }
            
            if (!alreadyCounted) {
                result.vertices[result.amountOfVertices] = allVertices.vertices[i];
                result.amountOfVertices++;
            }
        }
        i++;
    }
    
    assert(getCampuses(g, me) == result.amountOfVertices);
    
    return result;
}

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

static spatialInfo retriveInfo(Game g){
    spatialInfo gameInfo;
    
    //Retrieves all of the dice values and degree types for each of the
    //tiles and stores them in the spatialInfo struct.
    region testRegion;
    testRegion.y = -3;
    
    int regionCount = 0;
    while(regionCount < NUM_REGIONS){
        while (testRegion.y <= 3){
            testRegion.x = -3;
            while (testRegion.x <= 3){
                gameInfo.regionDiceValues[regionCount] = getDiceValue(g, testRegion);
                gameInfo.regionDegreeTypes[regionCount] = getDegree(g, testRegion);
                testRegion.x++;
            }
            testRegion.y++;
        }
        regionCount++;
    }
    
    //Tests all of the verticies on the board and stores campus locations
    //into spatialInfo. The Arcs are also found and stored in a similar
    //way.
    testRegion.x = -3;
    testRegion.y = 3;
    
    arc testArc;
    region adjacentTestRegion;
    int arcCountA = 0;
    int arcCountB = 0;
    int arcCountC = 0;
    int vertexCountA = 0;
    int vertexCountB = 0;
    int vertexCountC = 0;
    
    while(testRegion.x <= 3){
        testRegion.y = 3;
        while(testRegion.y >= -3){
            //Initialising the adjacentTestRegion to be directly below
            //the test region.
            adjacentTestRegion.y = testRegion.y -1;
            adjacentTestRegion.x = testRegion.x;
            testArc.region0 = testRegion;
            testArc.region1 = adjacentTestRegion;
            
            int playersArc = getARC(g, testArc);
            if(playersArc == UNI_A){
                gameInfo.arcLocations[UNI_A][arcCountA] = testArc;
                arcCountA++;
            } else if(playersArc == UNI_B){
                gameInfo.arcLocations[UNI_B][arcCountB] = testArc;
                arcCountB++;
            } else if(playersArc == UNI_C){
                gameInfo.arcLocations[UNI_C][arcCountC] = testArc;
                arcCountC++;
            }
            
            vertices testVertices = verticesAroundArc(testArc);
            int playersVertex0 = getCampus(g, testVertices.vertices[0]);
            int playersVertex1 = getCampus(g, testVertices.vertices[1]);
            
            if(playersVertex0 == UNI_A){
                gameInfo.campusLocations[UNI_A][vertexCountA] = testVertices.vertices[0];
                vertexCountA++;
            } else if(playersVertex1 == UNI_A){
                gameInfo.campusLocations[UNI_A][vertexCountA] = testVertices.vertices[1];
                vertexCountA++;
            } else if(playersVertex0 == UNI_B){
                gameInfo.campusLocations[UNI_B][vertexCountB] = testVertices.vertices[0];
                vertexCountB++;
            } else if(playersVertex1 == UNI_B){
                gameInfo.campusLocations[UNI_B][vertexCountB] = testVertices.vertices[1];
                vertexCountB++;
            } else if(playersVertex0 == UNI_B){
                gameInfo.campusLocations[UNI_C][vertexCountC] = testVertices.vertices[0];
                vertexCountC++;
            } else if(playersVertex1 == UNI_C){
                gameInfo.campusLocations[UNI_C][vertexCountC] = testVertices.vertices[1];
                vertexCountC++;
            }
            
            testRegion.y--;
        }
        testRegion.x++;
    }
    
    return gameInfo;
}

static int canAfford(Game g, int actionCode) {
    int canAfford;
    uni me;
    
    me = getTurnNumber(g);
    
    if (actionCode == BUILD_CAMPUS) {
        
        if (getStudents(g, me, STUDENT_BPS) >= 1 &&
            getStudents(g, me, STUDENT_BQN) >= 1 &&
            getStudents(g, me, STUDENT_MJ)  >= 1 &&
            getStudents(g, me, STUDENT_MTV) >= 1){
            canAfford = TRUE;
        } else {
            canAfford = FALSE;
        }
        
    } else if (actionCode == BUILD_GO8) {
        
        if (getStudents(g, me, STUDENT_MJ)  >= 2 &&
            getStudents(g, me, STUDENT_MMONEY) >= 3){
            canAfford = TRUE;
        } else {
            canAfford = FALSE;
        }
        
    } else if (actionCode == CREATE_ARC) {
        
        if (getStudents(g, me, STUDENT_BPS) >= 1 &&
            getStudents(g, me, STUDENT_BQN) >= 1){
            canAfford = TRUE;
        } else {
            canAfford = FALSE;
        }
        
    } else if (actionCode == START_SPINOFF) {
        
        if (getStudents(g, me, STUDENT_MJ)  >= 1 &&
            getStudents(g, me, STUDENT_MTV) >= 1 &&
            getStudents(g, me, STUDENT_MMONEY) >= 1){
            canAfford = TRUE;
        } else {
            canAfford = FALSE;
        }
        
    } else {
        canAfford = FALSE;
    }
    
    printf("code: %d, can afford? %d\n", actionCode, canAfford);
    
    return canAfford;
}

static int whichWay(arc a) {
    int facing;
    
    if (a.region0.y == a.region1.y) {
        facing = HORIZONTAL;
    } else if (a.region0.x == a.region1.x) {
        facing = BACKWARDS;
    } else {
        facing = FORWARDS;
    }
    
    return facing;
}

static vertex rightVertexFromRegion(region r) {
    vertex v;
    
    v.region0 = r;
    
    v.region1.x = r.x;
    v.region1.y = r.y+1;
    
    v.region2.x = r.x-1;
    v.region2.y = r.y+1;
    
    return v;
}

static vertex leftVertexFromRegion(region r) {
    vertex v;
    
    v.region0 = r;
    
    v.region1.x = r.x;
    v.region1.y = r.y-1;
    
    v.region2.x = r.x+1;
    v.region2.y = r.y-1;
    
    return v;
}

static vertices getAllVertices(Game g) {
    int x;
    int y;
    region r;
    vertices result;
    vertex tempVertex;
    
    x = MIN_COORD;
    while (x <= MAX_COORD) {
        y = MIN_COORD;
        while (y <= MAX_COORD) {
            r.x = x;
            r.y = y;
            
            tempVertex = rightVertexFromRegion(r);
            if (isRealVertex(g, tempVertex)) {
                result.vertices[result.amountOfVertices] = tempVertex;
                result.amountOfVertices++;
            }
            
            tempVertex = leftVertexFromRegion(r);
            if (isRealVertex(g, tempVertex)) {
                result.vertices[result.amountOfVertices] = tempVertex;
                result.amountOfVertices++;
            }
            y++;
        }
        x++;
    }
    return result;
}
