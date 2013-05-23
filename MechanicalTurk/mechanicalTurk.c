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

typedef struct _arcs {
    arc arcs[MAX_ARCS];
    int amountOfArcs;
} arcs;

typedef struct _vertices {
    vertex vertices[MAX_vertices];
    int amountOfvertices;
} vertices;

int chooseAction(void);
vertex chooseVertex(void);

arcs arcsAroundVertex(vertex v);
arcs arcsAroundArc(arc a);

vertices verticesAroundVertex(vertex v);
vertices verticesAroundArc(arc a);

action bestMove(Game g) {
    action bestMove;
    
    bestMove.actionCode = chooseAction();
    return bestMove;
}

int chooseAction(void){
    return PASS;
}

vertex chooseVertex(void) {
    vertex chosen;
    return chosen;
}

arc chooseArc(void) {
    arc chosen;
    return chosen;
}

arcs arcsAroundVertex(vertex v) {
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

arcs arcsAroundArc(arc a) {
    arcs arr;
    
    arr.amountOfArcs = 4;
    
    return arr;
}

vertices verticesAroundVertex(vertex v) {
    vertices arr;
    
    arr.amountOfvertices = 3;
    
    return arr;
}

vertices verticesAroundArc(arc a) {
    vertices arr;
    
    
    
    arr.amountOfvertices = 2;
    
    return arr;
}