//
//  main.c
//  MechanicalTurk
//
//  Created by Owen Cassidy on 22/05/13.
//  Copyright (c) 2013 Owen Cassidy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mechanicalTurk.h"

#define CYAN STUDENT_BQN
#define PURP STUDENT_MMONEY
#define YELL STUDENT_MJ
#define RED STUDENT_BPS
#define GREE STUDENT_MTV
#define BLUE STUDENT_THD

#define TEST_DISCIPLINES {CYAN,PURP,YELL,PURP,YELL,RED ,GREE,GREE, RED,GREE,CYAN,YELL,CYAN,BLUE,YELL,PURP,GREE,CYAN,RED }
#define TEST_DICE {9,10,8,12,6,5,3,11,3,11,4,6,4,9,9,2,8,10,5}

#define CRAZY_DISCIPLINES {RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED,RED}
#define CRAZY_DICE {6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6}

#define AMOUNT_OF_CRAZY 1917

void retrainFor(Game g, int retrainTo, uni player);
Game newGameWithCrazyResources (void);

int main(int argc, const char * argv[]) {
    
    Game g = newGameWithCrazyResources();
    action a;
    
    printf("\nPlayer %d, turn %d\n", getWhoseTurn(g), getTurnNumber(g));
    
    a = bestMove(g);
    
    assert(isLegalAction(g, a));
    makeAction(g, a);
    
    throwDice(g, 2);
    
    
    throwDice(g, 2);
    
    
    throwDice(g, 3);
    printf("\nPlayer %d, turn %d\n", getWhoseTurn(g), getTurnNumber(g));
    
    a = bestMove(g);
    
    assert(isLegalAction(g, a));
    makeAction(g, a);
    
    throwDice(g, 4);
    
    
    throwDice(g, 5);
    
    throwDice(g, 6);
    printf("\nPlayer %d, turn %d\n", getWhoseTurn(g), getTurnNumber(g));
    
    a = bestMove(g);
    
    assert(isLegalAction(g, a));
    makeAction(g, a);
    
    throwDice(g, 8);
    
    throwDice(g, 9);
    
    throwDice(g, 10);
    printf("\nPlayer %d, turn %d\n", getWhoseTurn(g), getTurnNumber(g));
    
    a = bestMove(g);
    
    assert(isLegalAction(g, a));
    makeAction(g, a);
    
    throwDice(g, 11);
    
    int i=0;
    while (i < 3) {
        printf("\n==PLAYER %d SCORE==\n", i);
        printf("     kpi: %d\n", getKPIpoints(g, i));
        printf("    arcs: %d\n", getARCs(g, i));
        printf("campuses: %d\n", getCampuses(g, i));
        printf("    go8s: %d\n", getGO8s(g, i));
        printf("    pubs: %d\n", getPublications(g, i));
        printf("students: BPS:%d B?:%d MJ:%d M$:%d MTV:%d THD:%d\n", getStudents(g, i, STUDENT_BPS), getStudents(g, i, STUDENT_BQN), getStudents(g, i, STUDENT_MJ), getStudents(g, i, STUDENT_MMONEY), getStudents(g, i, STUDENT_MTV), getStudents(g, i, STUDENT_THD));
        i++;
    }
    
    printf("all tests passed!");
    return EXIT_SUCCESS;
}

Game newGameWithCrazyResources (void) {
    int dice[] = CRAZY_DICE;
    int degree[] = CRAZY_DISCIPLINES;
    
    int j;
    int i;
    
    Game g = newGame(degree, dice);
    
    i = 0;
    while (i < AMOUNT_OF_CRAZY * 7.5) {
        throwDice(g, 6);
        i++;
    }
    
    while (getWhoseTurn(g) != 0) {
        throwDice(g, 4);
    }
    
    i = 0;
    while (i < 3) {
        j = 0;
        while (j < AMOUNT_OF_CRAZY * 5) {
            retrainFor(g, j % 5 + 1, i);
            j++;
        }
        throwDice(g, 4);
        
        i++;
    }
    i--;
    printf("students: BPS:%d B?:%d MJ:%d M$:%d MTV:%d THD:%d\n", getStudents(g, i, STUDENT_BPS), getStudents(g, i, STUDENT_BQN), getStudents(g, i, STUDENT_MJ), getStudents(g, i, STUDENT_MMONEY), getStudents(g, i, STUDENT_MTV), getStudents(g, i, STUDENT_THD));
    
    
    return g;
}

void retrainFor(Game g, int retrainTo, uni player) {
    action a;
    
    a.actionCode = RETRAIN_STUDENTS;
    a.retrainFrom = RED;
    a.retrainTo = retrainTo;
    
    makeAction(g, a);
}
