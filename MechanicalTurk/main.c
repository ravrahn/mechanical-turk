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
    
    throwDice(g, 6);
    
    action a = bestMove(g);
    
    printf("actionCode: %d\n", a.actionCode);
    
    printf("all tests passed!");
    return EXIT_SUCCESS;
}