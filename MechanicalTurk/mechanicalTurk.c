//
//  mechanicalTurk.c
//  KnowledgeIsland
//
//  Created by Owen Cassidy and Daniel Parker on 21/05/13 - 25/05/13
//  Copyright (c) 2013 Owen Cassidy. All rights reserved.
//

/* TODO
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

#define TRUE 1
#define FALSE 0

#define HORIZONTAL 0
#define FORWARDS 1
#define BACKWARDS 2

#define LEFT 0
#define RIGHT 1

#define NULL_STUDENT 7

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

typedef struct retrainValues {
    int retrainFrom;
    int retrainTo;
} retrainValues;

static spatialInfo retriveInfo(Game g);

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
static arcs arcsAroundVertex(Game g, vertex v);

// Returns all the ARCs adjacent to a given ARC.
static arcs arcsAroundArc(Game g, arc a);


// Returns all vertices adjacent to a given vertex.
static vertices verticesAroundVertex(Game g, vertex v);

// Returns all vertices adjacent to a given ARC.
static vertices verticesAroundArc(Game g, arc a);


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

// Checks if a player can retrain to afford an action.
static int canRetrain(Game g, int actionCode);

static retrainValues retrainFor(Game g, int actionCode);


// Returns which way an arc is facing.
// / - FORWARDS
// \ - BACKWARDS
// - - HORIZONTAL
static int whichWayArc(arc a);

// Returns which way a vertex is facing.
// =- - LEFT
// -= - RIGHT
static int whichWayVertex(vertex v);

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


// Returns an arc based on a region:
//   ____
//  /    \
// /  r0  \
// \  in  /
//  \____/
//   ----
static arc bottomArcFromRegion(region r);

// Returns an arc based on a region:
//   ____
//  /    \\
// /  r0  \\
// \  in  /
//  \____/
static arc rightTopArcFromRegion(region r);

// Returns an arc based on a region:
//   ____
//  /    \
// /  r0  \
// \  in  //
//  \____//
static arc rightBottomArcFromRegion(region r);

// Returns an array of all legal arcs.
static arcs getAllArcs(Game g);


// Sorts a vertex's regions
//         ____
//        /    \
//   ____/  r1  \
//  /    \      /
// /  r0  \____/   - right
// \      /    \
//  \____/  r2  \
//       \      /
//        \____/
//    ____
//   /    \
//  /  r0  \____
//  \      /    \
//   \____/  r2  \ - left
//   /    \      /
//  /  r1  \____/
//  \      /
//   \____/
static vertex sortVertex(vertex v, int facing);


action bestMove(Game g) {
    action legalAction;
    
    vertex chosenCampus;
    arc chosenArc;
    vertex chosenGO8;
    
    uni me = getWhoseTurn(g);
    chosenCampus = chooseCampus(g);
    
    printf("I chose campus (%d, %d), (%d, %d), (%d, %d)\n", chosenCampus.region0.x, chosenCampus.region0.y, chosenCampus.region1.x, chosenCampus.region1.y, chosenCampus.region2.x, chosenCampus.region2.y);
    
    if(!verticesAreEqual(chosenCampus, illegalVertex())) {
       if (canAfford(g, BUILD_CAMPUS)){
            
            legalAction.actionCode = BUILD_CAMPUS;
            legalAction.targetVertex = chosenCampus;
        
        } else if (canRetrain(g, BUILD_CAMPUS)) {
            
            legalAction.actionCode = RETRAIN_STUDENTS;
            legalAction.retrainFrom = retrainFor(g, BUILD_CAMPUS).retrainFrom;
            legalAction.retrainTo = retrainFor(g, BUILD_CAMPUS).retrainTo;
        }
        
    } else {
        chosenArc = chooseArc(g);
        
        printf("I chose arc (%d, %d), (%d, %d)\n", chosenArc.region0.x, chosenArc.region0.y, chosenArc.region1.x, chosenArc.region1.y);
        

        if(!arcsAreEqual(chosenArc, illegalArc())) {
            if (canAfford(g, CREATE_ARC)) {
                legalAction.actionCode = CREATE_ARC;
                legalAction.targetARC = chosenArc;
            } else if (canRetrain(g, CREATE_ARC)) {
                
                legalAction.actionCode = RETRAIN_STUDENTS;
                legalAction.retrainFrom = retrainFor(g, CREATE_ARC).retrainFrom;
                legalAction.retrainTo = retrainFor(g, CREATE_ARC).retrainTo;
            }
        } else {
            
            if(canAfford(g, START_SPINOFF)){
        
                legalAction.actionCode = START_SPINOFF;
                
            } else if (canRetrain(g, START_SPINOFF)) {
                
                legalAction.actionCode = RETRAIN_STUDENTS;
                legalAction.retrainFrom = retrainFor(g, START_SPINOFF).retrainFrom;
                legalAction.retrainTo = retrainFor(g, START_SPINOFF).retrainTo;
                
            } else {
                chosenGO8 = chooseGO8(g);
                
                printf("I chose GO8 (%d, %d), (%d, %d), (%d, %d)\n", chosenGO8.region0.x, chosenGO8.region0.y, chosenGO8.region1.x, chosenGO8.region1.y, chosenGO8.region2.x, chosenGO8.region2.y);
                
                if(!verticesAreEqual(illegalVertex(), chosenGO8) &&
                   canAfford(g, BUILD_GO8)){
                    
                    legalAction.actionCode = BUILD_GO8;
                    legalAction.targetVertex = chosenGO8;
                    
                } else {
                
                    legalAction.actionCode = PASS;
                
                }
            }
        }
    }
    
    printf("I'm playing %d\n", legalAction.actionCode);
    
    printf("I can afford it! I have: BPS:%d B?:%d MJ:%d M$:%d MTV:%d THD:%d\n", getStudents(g, me, STUDENT_BPS), getStudents(g, me, STUDENT_BQN), getStudents(g, me, STUDENT_MJ), getStudents(g, me, STUDENT_MMONEY), getStudents(g, me, STUDENT_MTV), getStudents(g, me, STUDENT_THD));
    
    
    assert(isLegalAction(g, legalAction));
    
    return legalAction;
}

static vertex chooseGO8(Game g) {
    vertex legalVertex = illegalVertex();
    uni me = getWhoseTurn(g);
    vertices owned;
    int i;
    
    owned = ownedCampuses(g, me);
    
    i = 0;
    while (i < owned.amountOfVertices) {
        if (getCampus(g, owned.vertices[i]) == me+1) {
            legalVertex = owned.vertices[i];
        }
        i++;
    }
    
    return legalVertex;
}

static vertex chooseCampus(Game g){
    vertex legalVertex = illegalVertex();
    vertices testVertices;
    uni me = getWhoseTurn(g);
        
    arcs mArcs = ownedArcs(g, me);
    int arcCount = 0;
    int vertexCount = 0;
    
    while (arcCount < mArcs.amountOfArcs && verticesAreEqual(legalVertex, illegalVertex())){
        testVertices = verticesAroundArc(g, mArcs.arcs[arcCount]);
        
        while (vertexCount < testVertices.amountOfVertices) {
            if (isLegalVertex(g, testVertices.vertices[vertexCount])){
                legalVertex = testVertices.vertices[vertexCount];
            }
            vertexCount++;
        }
        arcCount++;
    }
    
    return legalVertex;
}

static arc chooseArc(Game g) {
    arc legalArc = illegalArc();
    uni me = getWhoseTurn(g);
    arcs testArcs;
    arcs mArcs;
    vertices mCampuses;
    int arcCount;
    int campusCount;
    int i;
    int j;
    vertices aroundArc;
    int campusAroundArc;
    
    mArcs = ownedArcs(g, me);
    if (mArcs.amountOfArcs ==  0) {
        mCampuses = ownedCampuses(g, me);
        
        campusCount = 0;
        
        while (campusCount < mCampuses.amountOfVertices &&
               arcsAreEqual(legalArc, illegalArc())) {
            
            testArcs = arcsAroundVertex(g, mCampuses.vertices[campusCount]);

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
            
            testArcs = arcsAroundArc(g, mArcs.arcs[arcCount]);

            i = 0;
            while (i < testArcs.amountOfArcs) {
                aroundArc = verticesAroundArc(g, testArcs.arcs[i]);
                campusAroundArc = FALSE;
                j = 0;
                while (j < aroundArc.amountOfVertices) {
                    if (getCampus(g, aroundArc.vertices[i]) == me+1 ||
                        getCampus(g, aroundArc.vertices[i]) == me+4) {
                        campusAroundArc = TRUE;
                    }
                    j++;
                }
                if (isLegalArc(g, testArcs.arcs[i]) &&
                    arcsAreEqual(legalArc, illegalArc()) &&
                    !campusAroundArc) {
                    legalArc = testArcs.arcs[i];
                }
                i++;
            }
            
            arcCount++;
        }
    }
    
    return legalArc;
}

static arcs arcsAroundVertex(Game g, vertex v) {
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
    
    arr.amountOfArcs = 0;
    
    if (isRealArc(g, a)) {
        arr.arcs[arr.amountOfArcs] = a;
        arr.amountOfArcs++;
    }
    if (isRealArc(g, b)) {
        arr.arcs[arr.amountOfArcs] = b;
        arr.amountOfArcs++;
    }
    if (isRealArc(g, c)) {
        arr.arcs[2] = c;
        arr.amountOfArcs++;
    }
    
    return arr;
}

static arcs arcsAroundArc(Game g, arc a) {
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
    
    
    facing = whichWayArc(a);
    
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
    
    arr.amountOfArcs = 0;
    
    if (isRealArc(g, a1)) {
        arr.arcs[arr.amountOfArcs] = a1;
        arr.amountOfArcs++;
    }
    if (isRealArc(g, a2)) {
        arr.arcs[arr.amountOfArcs] = a2;
        arr.amountOfArcs++;
    }
    if (isRealArc(g, a3)) {
        arr.arcs[arr.amountOfArcs] = a3;
        arr.amountOfArcs++;
    }
    if (isRealArc(g, a4)) {
        arr.arcs[arr.amountOfArcs] = a4;
        arr.amountOfArcs++;
    }
    
    return arr;
}

static vertices verticesAroundVertex(Game g, vertex v) {
    vertices arr;
    vertex v0;
    vertex v1;
    vertex v2;
    
    region r0;
    region r1;
    region r2;
    region r3;
    region r4;
    region r5;
    
    if (whichWayVertex(v) == LEFT) {
        v = sortVertex(v, LEFT);
        r1 = v.region0;
        r3 = v.region1;
        r4 = v.region2;
        
        r0.y = r1.y;
        r0.x = r1.x - 1;
        
        r2.y = r1.y;
        r2.x = r1.x + 1;
        
        r5.x = r4.x;
        r4.y = r4.y - 1;
        
        v0.region0 = r0;
        v0.region1 = r1;
        v0.region2 = r3;
        
        v1.region0 = r1;
        v1.region1 = r2;
        v1.region2 = r4;
        
        v2.region0 = r3;
        v2.region1 = r4;
        v2.region2 = r5;
        
    } else {
        v = sortVertex(v, RIGHT);
        r1 = v.region0;
        r2 = v.region1;
        r4 = v.region2;
        
        r0.x = r1.x;
        r0.y = r1.y + 1;
        
        r3.x = r1.x;
        r3.y = r1.y - 1;
        
        r5.y = r4.y;
        r5.x = r4.x + 1;
        
        v0.region0 = r0;
        v0.region1 = r1;
        v0.region2 = r2;
        
        v1.region0 = r1;
        v1.region1 = r3;
        v1.region2 = r4;
        
        v2.region0 = r2;
        v2.region1 = r4;
        v2.region2 = r5;
    }
    
    arr.amountOfVertices = 0;
    
    if (isRealVertex(g, v0)) {
        arr.vertices[arr.amountOfVertices] = v0;
        arr.amountOfVertices++;
    }
    if (isRealVertex(g, v1)) {
        arr.vertices[arr.amountOfVertices] = v1;
        arr.amountOfVertices++;
    }
    if (isRealVertex(g, v2)) {
        arr.vertices[arr.amountOfVertices] = v2;
        arr.amountOfVertices++;
    }
    
    return arr;
}

static vertices verticesAroundArc(Game g, arc a) {
    vertices arr;
    
    region r0;
    region r1;
    region r2;
    region r3;
    
    vertex v0;
    vertex v1;
    
    int facing;
    
    facing = whichWayArc(a);
    
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
        
    } else if (facing == BACKWARDS) {
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
        
    } else if (facing == FORWARDS) {
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
    
    arr.amountOfVertices = 0;
    
    if (isRealVertex(g, v0)) {
        arr.vertices[arr.amountOfVertices] = v0;
        arr.amountOfVertices++;
    }
    if (isRealVertex(g, v1)) {
        arr.vertices[arr.amountOfVertices] = v1;
        arr.amountOfVertices++;
    }
    
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
    
    int isCampus;
    
    vertices adjacentVertices;
    arcs adjacentArcs;
    
    int i;
    
    adjacentVertices = verticesAroundVertex(g, v);
    
    hasAdjacentCampus = FALSE;
    
    i = 0;
    while (i < adjacentVertices.amountOfVertices) {
        
        if (isRealVertex(g, adjacentVertices.vertices[i])) {
            if ((getCampus(g, adjacentVertices.vertices[i]) != VACANT_VERTEX)) {
                    hasAdjacentCampus = TRUE;
            }
        }
        
        i++;
    }
    
    adjacentArcs = arcsAroundVertex(g, v);
    
    hasAdjacentArc = FALSE;
    
    i = 0;
    while (i < adjacentArcs.amountOfArcs) {
        
        if (isLegalArc(g, adjacentArcs.arcs[i])) {
            if ((getARC(g, adjacentArcs.arcs[i]) == (getWhoseTurn(g)) + 1)) {
                hasAdjacentArc = TRUE;
            }
        }
        
        i++;
    }
    
    isCampus = FALSE;
    if (isRealVertex(g, v)) {
        if (getCampus(g, v) != VACANT_VERTEX) {
            isCampus = TRUE;
        }
    }
    
    isLegal = isRealVertex(g, v) && hasAdjacentArc && !hasAdjacentCampus && !isCampus;
    
    return isLegal;
}

static int isLegalArc(Game g, arc a) {
    int isLegal;
    
    int hasAdjacentArc;
    int hasAdjacentCampus;
    
    int isArc;
    
    arcs adjacentArcs;
    vertices adjacentVertices;
    
    uni me = getWhoseTurn(g);
    
    int i;
    
    hasAdjacentCampus = FALSE;
    adjacentVertices = verticesAroundArc(g, a);
    
    i = 0;
    while (i < adjacentVertices.amountOfVertices) {
        if (isRealVertex(g, adjacentVertices.vertices[i])) {
            if (getCampus(g, adjacentVertices.vertices[i]) == me+1 ||
                getCampus(g, adjacentVertices.vertices[i]) == me+4) {
                
                hasAdjacentCampus = TRUE;
            }
        }
        
        i++;
    }

    
    adjacentArcs = arcsAroundArc(g, a);
    hasAdjacentArc = FALSE;
    
    i = 0;
    while (i < adjacentArcs.amountOfArcs) {
        if (isRealArc(g, adjacentArcs.arcs[i])) {
            if ((getARC(g, adjacentArcs.arcs[i]) == (getWhoseTurn(g)) + 1)) {
                hasAdjacentArc = TRUE;
            }
        }
        
        i++;
    }
    
    isArc = FALSE;
    
    if (isRealArc(g, a)) {
        if (getARC(g, a) != VACANT_ARC) {
            isArc = TRUE;
        }
    }
    
    isLegal = isRealArc(g, a) && (hasAdjacentArc || hasAdjacentCampus) && !isArc;
    
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
    arcs allArcs;
    int i;
    int j;
    int alreadyCounted;
    
    result.amountOfArcs = 0;
    allArcs = getAllArcs(g);
    
    i = 0;
    while (i < allArcs.amountOfArcs) {
        if (isRealArc(g, allArcs.arcs[i])) {
            if (getARC(g, allArcs.arcs[i]) == me+1) {
                
                j = 0;
                alreadyCounted = FALSE;
                while (j < result.amountOfArcs) {
                    if (arcsAreEqual(result.arcs[j], allArcs.arcs[i])) {
                        alreadyCounted = TRUE;
                    }
                    j++;
                }
                
                if (!alreadyCounted) {
                    result.arcs[result.amountOfArcs] = allArcs.arcs[i];
                    result.amountOfArcs++;
                }
                
            }
        }
        
        i++;
    }
    
    printf("got %d arcs\n", getARCs(g, me));
    
    assert(getARCs(g, me) == result.amountOfArcs);
    
    return result;
}

static vertices ownedCampuses(Game g, uni me) {
    vertices result;
    vertices allVertices;
    int i;
    int j;
    int alreadyCounted;
    
    result.amountOfVertices = 0;
    allVertices = getAllVertices(g);

    i = 0;
    while (i < allVertices.amountOfVertices) {
        if ((getCampus(g, allVertices.vertices[i]) == me+1 ||
            getCampus(g, allVertices.vertices[i]) == me+4) &&
            isRealVertex(g, allVertices.vertices[i])) {
            
            printf("campus (%d, %d), (%d, %d), (%d, %d) owned by player %d\n", allVertices.vertices[i].region0.x, allVertices.vertices[i].region0.y, allVertices.vertices[i].region1.x, allVertices.vertices[i].region1.y, allVertices.vertices[i].region2.x, allVertices.vertices[i].region2.y, getCampus(g, allVertices.vertices[i]));
            
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
    //tiles and stores them in the spatialInfo struct. This achieved by
    //scanning in all of the regions using the Game.c getters.
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
                regionCount++;
            }
            testRegion.y++;
        }
    }
    
    //Tests all of the verticies on the board and stores campus locations
    //into spatialInfo. The Arcs are also found and stored in a similar
    //way.
    testRegion.x = -3;
    testRegion.y = 3;
    
    arc testArc;
    region adjacentTestRegion;
    //Counters so we can keep track of the index of each campus or arc.
    int arcCountA = 0;
    int arcCountB = 0;
    int arcCountC = 0;
    int vertexCountA = 0;
    int vertexCountB = 0;
    int vertexCountC = 0;
    
    //Scans through the regions in pairs to test for arc grants on these
    //locations and then uses these adjacent regions with
    //verticesAroundArc to test all of the vertices on the map for campuses.
    //All of this data is save into the spatialInfo struct.
    while(testRegion.x <= 3){
        testRegion.y = 3;
        while(testRegion.y >= -3){
            //Initialising the adjacentTestRegion to be directly below
            //the test region.
            adjacentTestRegion.y = testRegion.y -1;
            adjacentTestRegion.x = testRegion.x;
            testArc.region0 = testRegion;
            testArc.region1 = adjacentTestRegion;
            
            //This is only testing the hortizontal arcs at the moment and
            //needs to be adjusted to test every arc. I will use
            //arcsAroundArc to do this [D.P].
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
            
            vertices testVertices = verticesAroundArc(g, testArc);
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
    
    me = getWhoseTurn(g);
    
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

static int canRetrain(Game g, int actionCode) {
    return TRUE;
}

static retrainValues retrainFor(Game g, int actionCode){
    uni me;
    int numBPS;
    int numBQN;
    int numMJ;
    int numMTV;
    int numMMONEY;
    int testStudentTo;
    int testStudentFrom;
    degree surplusStudent0 = NULL_STUDENT;
    degree surplusStudent1 = NULL_STUDENT;
    degree surplusStudent2 = NULL_STUDENT;
    degree retrainFrom = NULL_STUDENT;
    retrainValues result;
    
    me = getWhoseTurn(g);
    
    numBPS = getStudents(g, me, STUDENT_BPS);
    numBQN = getStudents(g, me, STUDENT_BQN);
    numMJ = getStudents(g, me, STUDENT_MJ);
    numMTV = getStudents(g, me, STUDENT_MTV);
    numMMONEY = getStudents(g, me, STUDENT_MMONEY);
    
    if(actionCode == BUILD_CAMPUS){
        surplusStudent0 = STUDENT_MMONEY;
    } else if (actionCode == CREATE_ARC){
        surplusStudent0 = STUDENT_MJ;
        surplusStudent1 = STUDENT_MTV;
        surplusStudent2 = STUDENT_MMONEY;
    } else if (actionCode == START_SPINOFF){
        surplusStudent0 = STUDENT_BPS;
        surplusStudent1 = STUDENT_BQN;
    }
    
    testStudentTo = 0;
    while(testStudentTo < 5 && retrainFrom != NULL_STUDENT){
        if(getStudents(g, me, testStudentTo) == 0){
            testStudentFrom = 0;
            while (testStudentFrom < 5 != retrainFrom != NULL_STUDENT){
                if(surplusStudent0 == testStudentFrom ||
                   surplusStudent1 == testStudentFrom ||
                   surplusStudent2 == testStudentFrom){
                    if(getExchangeRate(g, me, testStudentFrom, testStudentTo)
                       <= testStudentTo && testStudentFrom != testStudentTo){
                        retrainFrom = testStudentFrom;
                    } 
                } else {
                    if(getExchangeRate(g, me, testStudentFrom, testStudentTo)
                       < testStudentTo && testStudentFrom != testStudentTo){
                        retrainFrom = testStudentFrom;
                    } 
                }
                testStudentFrom++;
            }
        }
        testStudentTo++;
    }
    
    result.retrainFrom = retrainFrom;
    result.retrainTo = testStudentTo;
    
    return result;
}

static int whichWayArc(arc a) {
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

static int whichWayVertex(vertex v) {
    int facing;
    int sharedY;
    int otherY;
    
    if (v.region0.y == v.region1.y) {
        sharedY = v.region0.y;
        otherY = v.region2.y;
    } else if (v.region1.y == v.region2.y) {
        sharedY = v.region1.y;
        otherY = v.region0.y;
    } else if (v.region2.y == v.region0.y) {
        sharedY = v.region2.y;
        otherY = v.region1.y;
    } else {
        sharedY = 0;
        otherY = 0;
        assert(FALSE);
    }
    
    if (sharedY > otherY) {
        facing = RIGHT;
    } else {
        facing = LEFT;
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
    
    result.amountOfVertices = 0;
    
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

static arc bottomArcFromRegion(region r) {
    arc a;
    region r1;
    
    r1.x = r.x;
    r1.y = r.y - 1;
    
    a.region0 = r;
    a.region1 = r1;
    
    return a;
}

static arc rightTopArcFromRegion(region r) {
    arc a;
    region r1;
    
    r1.x = r.x + 1;
    r1.y = r.y;
    
    a.region0 = r;
    a.region1 = r1;
    
    return a;
}

static arc rightBottomArcFromRegion(region r) {
    arc a;
    region r1;
    
    r1.x = r.x + 1;
    r1.y = r.y - 1;
    
    a.region0 = r;
    a.region1 = r1;
    
    return a;
}

static arcs getAllArcs(Game g) {
    int x;
    int y;
    region r;
    arcs result;
    arc tempArc;
    
    result.amountOfArcs = 0;
    
    x = MIN_COORD;
    while (x <= MAX_COORD) {
        y = MIN_COORD;
        while (y <= MAX_COORD) {
            r.x = x;
            r.y = y;
            
            tempArc = bottomArcFromRegion(r);
            if (isRealArc(g, tempArc)) {
                result.arcs[result.amountOfArcs] = tempArc;
                result.amountOfArcs++;
            }
            
            tempArc = rightTopArcFromRegion(r);
            if (isRealArc(g, tempArc)) {
                result.arcs[result.amountOfArcs] = tempArc;
                result.amountOfArcs++;
            }
            
            tempArc = rightBottomArcFromRegion(r);
            if (isRealArc(g, tempArc)) {
                result.arcs[result.amountOfArcs] = tempArc;
                result.amountOfArcs++;
            }
            y++;
        }
        x++;
    }
    return result;
}


// Sorts a vertex's regions
//         ____
//        /    \
//   ____/  r1  \
//  /    \x1,y0 /
// /  r0  \____/   - right
// \x0,y0 /    \
//  \____/  r2  \
//       \x1,y1 /
//        \____/
//    ____
//   /    \
//  /  r0  \____
//  \x0,y1 /    \
//   \____/  r2  \ - left
//   /    \x1,y0 /
//  /  r1  \____/
//  \x0,y0 /
//   \____/
static vertex sortVertex(vertex v, int facing) {
    region r0;
    region r1;
    region r2;
    
    if (facing == RIGHT) {
        if (v.region0.y == v.region1.y) {
            if (v.region0.x < v.region1.x) {
                r0 = v.region0;
                r1 = v.region1;
            } else {
                r0 = v.region1;
                r1 = v.region0;
            }
            r2 = v.region2;
        } else if (v.region1.y == v.region2.y) {
            if (v.region1.x < v.region2.x) {
                r0 = v.region1;
                r1 = v.region2;
            } else {
                r0 = v.region2;
                r1 = v.region1;
            }
            r2 = v.region0;
        } else {
            if (v.region2.x < v.region0.x) {
                r0 = v.region2;
                r1 = v.region0;
            } else {
                r0 = v.region0;
                r1 = v.region2;
            }
            r2 = v.region1;
        }
    } else {
        if (v.region0.y == v.region1.y) {
            if (v.region0.x < v.region1.x) {
                r1 = v.region0;
                r2 = v.region1;
            } else {
                r1 = v.region1;
                r2 = v.region0;
            }
            r0 = v.region2;
        } else if (v.region1.y == v.region2.y) {
            if (v.region1.x < v.region2.x) {
                r1 = v.region1;
                r2 = v.region2;
            } else {
                r1 = v.region2;
                r2 = v.region1;
            }
            r0 = v.region0;
        } else {
            if (v.region2.x < v.region0.x) {
                r1 = v.region2;
                r2 = v.region0;
            } else {
                r1 = v.region0;
                r2 = v.region2;
            }
            r0 = v.region1;
        }
    }
    
    v.region0 = r0;
    v.region1 = r1;
    v.region2 = r2;
    
    return v;
}
