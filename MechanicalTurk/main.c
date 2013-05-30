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

int main(int argc, const char * argv[]) {

    // test city!
    // population 1...
    
    int degree[] = TEST_DISCIPLINES;
    int dice[] = TEST_DICE;
    
    Game g = newGame(degree, dice);
    
    throwDice(g, 1);
    printf("Player %d, turn %d\n", getTurnNumber(g) % 3, getTurnNumber(g));
    
    action a = bestMove(g);
    
    makeAction(g, a);
    
    throwDice(g, 2);
    printf("\nPlayer %d, turn %d\n", getTurnNumber(g) % 3, getTurnNumber(g));
    
    a = bestMove(g);
    
    makeAction(g, a);
    
    
    throwDice(g, 3);
    printf("\nPlayer %d, turn %d\n", getTurnNumber(g) % 3, getTurnNumber(g));
    
    a = bestMove(g);
    
    makeAction(g, a);
    
    
    throwDice(g, 4);
    printf("\nPlayer %d, turn %d\n", getTurnNumber(g) % 3, getTurnNumber(g));
    
    a = bestMove(g);
    
    makeAction(g, a);
    
    throwDice(g, 5);
    printf("\nPlayer %d, turn %d\n", getTurnNumber(g) % 3, getTurnNumber(g));
    
    a = bestMove(g);
    
    makeAction(g, a);
    
    
    throwDice(g, 6);
    printf("\nPlayer %d, turn %d\n", getTurnNumber(g) % 3, getTurnNumber(g));
    
    a = bestMove(g);
    
    makeAction(g, a);
    
    throwDice(g, 7);
    printf("\nPlayer %d, turn %d\n", getTurnNumber(g) % 3, getTurnNumber(g));
    
    a = bestMove(g);
    
    makeAction(g, a);
    
    throwDice(g, 8);
    printf("\nPlayer %d, turn %d\n", getTurnNumber(g) % 3, getTurnNumber(g));
    
    a = bestMove(g);
    
    makeAction(g, a);
    
    
    throwDice(g, 9);
    printf("\nPlayer %d, turn %d\n", getTurnNumber(g) % 3, getTurnNumber(g));
    
    a = bestMove(g);
    
    makeAction(g, a);
    
    
    throwDice(g, 10);
    printf("\nPlayer %d, turn %d\n", getTurnNumber(g) % 3, getTurnNumber(g));
    
    a = bestMove(g);
    
    makeAction(g, a);
    
    throwDice(g, 11);
    printf("\nPlayer %d, turn %d\n", getTurnNumber(g) % 3, getTurnNumber(g));
    
    a = bestMove(g);
    
    makeAction(g, a);
    
    
    throwDice(g, 12);
    printf("\nPlayer %d, turn %d\n", getTurnNumber(g) % 3, getTurnNumber(g));
    
    a = bestMove(g);
    
    makeAction(g, a);
    
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