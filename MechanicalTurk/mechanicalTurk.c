//
//  mechanicalTurk.c
//  KnowledgeIsland
//
//  Created by Owen Cassidy on 21/05/13.
//  Copyright (c) 2013 Owen Cassidy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
#include "mechanicalTurk.h"

#define MAX_ARCS 100
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

static action chooseAction(Game g);
static vertex chooseCampus(Game g);
static vertex chooseGO8(Game g);
static arc chooseArc(Game g);
static int chooseSpinoff(Game g);

static arcs arcsAroundVertex(vertex v);
static arcs arcsAroundArc(arc a);

static vertices verticesAroundVertex(vertex v);
static vertices verticesAroundArc(arc a);

static int verticesAreEqual(vertex a, vertex b);
static int arcsAreEqual(arc a, arc b);
static int regionsAreEqual(region a, region b);

static vertex illegalVertex(void);
static arc illegalArc(void);

static int isLegalVertex(vertex v);
static int isLegalArc(arc a);

static arcs ownedArcs(Game g, uni me);
static vertices ownedCampuses(Game g, uni me);

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
    } else if(verticesAreEqual(chooseCampus(g), illegalVertex())){
        legalAction.actionCode = BUILD_CAMPUS;
        legalAction.targetVertex = chooseCampus(g);
    } else if(chooseSpinoff(g)){
        legalAction.actionCode = START_SPINOFF;
    } else if(arcsAreEqual(chooseArc(g), illegalArc())){
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
    if (getStudents(g, me, STUDENT_BPS) >= 1 &&
        getStudents(g, me, STUDENT_BQN) >= 1 &&
        getStudents(g, me, STUDENT_MJ)  >= 1 &&
        getStudents(g, me, STUDENT_MTV) >= 1){
        
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
    if (getStudents(g, me, STUDENT_BPS) >= 1 &&
        getStudents(g, me, STUDENT_BQN) >= 1 &&
        getStudents(g, me, STUDENT_MJ)  >= 1 &&
        getStudents(g, me, STUDENT_MTV) >= 1){
        
        arcs mArcs = ownedArcs(g, me);
        int arcCount = 0;
        
        while (arcCount < mArcs.amountOfArcs && verticesAreEqual(legalVertex, illegalVertex())){
            testVertices = verticesAroundArc(mArcs.arcs[arcCount]);
            
            if (isLegalVertex(testVertices.vertices[0])){
                legalVertex = testVertices.vertices[0];
            } else if (isLegalVertex(testVertices.vertices[1])){
                legalVertex = testVertices.vertices[1];
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
    if (getStudents(g, me, STUDENT_BPS) >= 1 &&
        getStudents(g, me, STUDENT_BQN) >= 1){
        
        int arcCount = 0;
        
        while (arcCount < mArcs.amountOfArcs &&
               arcsAreEqual(legalArc, illegalArc())) {
            
            testArcs = arcsAroundArc(mArcs.arcs[arcCount]);
            
            if (isLegalArc(testArcs.arcs[0])){
                legalArc = testArcs.arcs[0];
            } else if (isLegalArc(testArcs.arcs[1])){
                legalArc = testArcs.arcs[0];
            } else if (isLegalArc(testArcs.arcs[2])){
                legalArc = testArcs.arcs[0];
            } else if (isLegalArc(testArcs.arcs[3])){
                legalArc = testArcs.arcs[0];
            }
            arcCount++;
        }
        
        return legalArc;
    } else {
        return illegalArc();
    }
}

static int chooseSpinoff(Game g) {
    int legalSpinoff = FALSE;
    uni me = getTurnNumber(g);
    
    if (getStudents(g, me, STUDENT_MJ)  >= 1 &&
        getStudents(g, me, STUDENT_MTV) >= 1 &&
        getStudents(g, me, STUDENT_MMONEY) >= 1){
        legalSpinoff = TRUE;
    }
    
    return legalSpinoff;
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

static int regionsAreEqual(region a, region b) {
    return TRUE;
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

static int isLegalVertex(vertex v) {
    return TRUE;
}

static int isLegalArc(arc a) {
    return TRUE;
}

static arcs ownedArcs(Game g, uni me) {
    arcs result;
    
    return result;
}

static vertices ownedCampuses(Game g, uni me) {
    vertices result;
    
    return result;
}
                    
