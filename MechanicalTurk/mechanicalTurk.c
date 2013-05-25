//
//  mechanicalTurk.c
//  KnowledgeIsland
//
//  Created by Owen Cassidy and Daniel Parker on 21/05/13 - 25/05/13
//  Copyright (c) 2013 Owen Cassidy. All rights reserved.
//

/* TODO
 **** ownedCampuses
 **** ownedArcs
 **** arcsAroundArc
 **** verticesAroundArc
 **** verticesAroundVertex
 */

#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
#include "mechanicalTurk.h"

#define MAX_ARCS 100
#define MAX_CAMPUSES 100
#define MAX_VERTICES 100

#define TRUE 1
#define FALSE 0

typedef struct _arcs {
    arc arcs[MAX_ARCS];
    int amountOfArcs;
} arcs;

typedef struct _vertices {
    vertex vertices[MAX_VERTICES];
    int amountOfvertices;
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


// Returns an impossible vertex
static vertex illegalVertex(void);

// Returns an impossible ARC
static arc illegalArc(void);

// Checks if a vertex is on the board.
static int isLegalVertex(Game g, vertex v);

// Checks if a vertex is on the board.
static int isLegalArc(Game g, arc a);


// Returns an array of all ARCs owned by a given player.
static arcs ownedArcs(Game g, uni me);

// Returns an array of all campuses owned by a given player.
static vertices ownedCampuses(Game g, uni me);

// Checks if two regions are adjacent.
static int regionsAreAdjacent(region a, region b);

// Checks if player has resources to perform an action
static int canAfford(Game g, int actionCode);

action bestMove(Game g) {
    action bestMove;
    
    bestMove = chooseAction(g);
    
    return bestMove;
}

static action chooseAction(Game g){
    
    action legalAction;
    
    if(verticesAreEqual(chooseGO8(g), illegalVertex())){
        
        legalAction.actionCode = BUILD_GO8;
        legalAction.targetVertex = chooseGO8(g);
        
    } else if(!verticesAreEqual(chooseCampus(g), illegalVertex())){
        
        legalAction.actionCode = BUILD_CAMPUS;
        legalAction.targetVertex = chooseCampus(g);
        
    } else if(canAfford(g, START_SPINOFF)){
        
        legalAction.actionCode = START_SPINOFF;
        
    } else if(!arcsAreEqual(chooseArc(g), illegalArc())){
        
        legalAction.actionCode = CREATE_ARC;
        legalAction.targetARC = chooseArc(g);
        
    } else {
        
        legalAction.actionCode = PASS;
        
    }
    return legalAction;
}

static vertex chooseGO8(Game g){
    vertex legalVertex = illegalVertex();
    vertex testVertex;
    uni me = getTurnNumber(g);
    
    //Region Coordinates lie between -2 and 2.
    if (canAfford(g, BUILD_GO8)){
        
        if (ownedCampuses(g, me).amountOfvertices != 0) {
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
    arcs mArcs = ownedArcs(g, me);
    
    //Region Coordinates lie between -2 and 2.
    if (canAfford(g, CREATE_ARC)){
        
        int arcCount = 0;
        
        while (arcCount < mArcs.amountOfArcs &&
               arcsAreEqual(legalArc, illegalArc())) {
            
            testArcs = arcsAroundArc(mArcs.arcs[arcCount]);
            
            if (isLegalArc(g, testArcs.arcs[0])){
                legalArc = testArcs.arcs[0];

            } else if (isLegalArc(g, testArcs.arcs[1])){
                legalArc = testArcs.arcs[1];
            } else if (isLegalArc(g, testArcs.arcs[2])){
                legalArc = testArcs.arcs[2];
            } else if (isLegalArc(g, testArcs.arcs[3])){
                legalArc = testArcs.arcs[3];
            }
            arcCount++;
        }
        
        return legalArc;
    } else {
        return illegalArc();
    }
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
    
    
    
    arr.amountOfArcs = 4;
    
    return arr;
}

static vertices verticesAroundVertex(vertex v) {
    vertices arr;
    
    
    
    arr.amountOfvertices = 3;
    
    return arr;
}

static vertices verticesAroundArc(arc a) {
    vertices arr;
    
    
    
    arr.amountOfvertices = 2;
    
    return arr;
}
    
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
    
    int isOnBoard;
    int exists;
    int hasAdjacentCampus;
    int hasAdjacentArc;
    
    vertices adjacentVertices;
    arcs adjacentArcs;
    
    int i;
    
    if (!isSea(g, v.region0) &&
        !isSea(g, v.region1) &&
        !isSea(g, v.region2)) {
        isOnBoard = TRUE;
    } else {
        isOnBoard = FALSE;
    }
    
    if (regionsAreAdjacent(v.region0, v.region1) &&
        regionsAreAdjacent(v.region0, v.region2) &&
        regionsAreAdjacent(v.region1, v.region2)) {
        exists = TRUE;
    } else {
        exists = FALSE;
    }
    
    adjacentVertices = verticesAroundVertex(v);
    
    hasAdjacentCampus = FALSE;
    
    i = 0;
    while (i < adjacentVertices.amountOfvertices) {
        
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
    
    isLegal = exists && isOnBoard && hasAdjacentArc && !hasAdjacentCampus;
    
    return isLegal;
}

static int isLegalArc(Game g, arc a) {
    int isLegal;
    
    int isOnBoard;
    int exists;
    int hasAdjacentArc;
    
    arcs adjacentArcs;
    
    int i;
    
    if (!isSea(g, a.region0) &&
        !isSea(g, a.region1)) {
        isOnBoard = TRUE;
    } else {
        isOnBoard = FALSE;
    }
    
    if (regionsAreAdjacent(a.region0, a.region1)) {
        exists = TRUE;
    } else {
        exists = FALSE;
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
    
    isLegal = isOnBoard && exists && hasAdjacentArc;
    
    return isLegal;
}

static arcs ownedArcs(Game g, uni me) {
    arcs result;
    
    return result;
}

static vertices ownedCampuses(Game g, uni me) {
    vertices result;
    
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

<<<<<<< HEAD
static int canAfford(Game g, action a) {
    
    return TRUE;
}
=======
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
    
    return canAfford;
}
>>>>>>> a70126b1a02d239bfbcdd511b0d776b3e7846338
