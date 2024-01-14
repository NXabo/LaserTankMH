#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <mutex>

using namespace std;


std::mutex mutexOnBest;
std::thread** tab_t = NULL;
int NumberThreads = NULL;
int*** scoreTab = NULL;
int** levelTab = NULL;
int levelSize = NULL;
char levelName[50];
int maxIt = 180000;
int best = maxIt;
int movedRECO = 1;
int wonRECO = 10;
int deadPUNI = -4;
int hardPUNI = -2;
int softPUNI = -1;
int* posTu = new int[2];
int explo = 8;
int followRate = 2;
int followTime = 8;
int nonFollowTime = 3;


void writeOutput(char* seq, int th) {
    FILE* outfile;
    char fileName[] = "out.txt";
    outfile = fopen(fileName, "w+");
    fprintf(outfile, "%s", levelName);
    fprintf(outfile, "%s%d%s", "Solver : Ant number ", th, " from the HEH colony\n");
    fprintf(outfile, "%s%d%s", "Score : ", best, "\n");
    fprintf(outfile, "%s%s", "\n", seq);
    fclose(outfile);
}


void writeBest(int newBest, int th, char* seq, int* posTank, int* start) {
    mutexOnBest.lock();
    if (newBest < best) {
        best = newBest;
        std::cout << "start " << start[0] << start[1] << std::endl;
        std::cout << "end " << posTank[0] << posTank[1] << std::endl;
        std::cout << "Found : " << best << std::endl;
        writeOutput(seq, th);
    }
    mutexOnBest.unlock();
}


bool isFiredAt(int* posTank, char* direction, int** levelThread) {
    int verti = 0;
    int hori = 0;
    int iterator = 0;
    int blocNext = 0;
    bool dirA = false;
    bool dirB = false;

    if (strcmp(direction, "U") == 0) {
        verti = -1;
    }
    else if (strcmp(direction, "D") == 0) {
        verti = 1;
    }
    else if (strcmp(direction, "R") == 0) {
        hori = 1;
    }
    else if (strcmp(direction, "L") == 0) {
        hori = -1;
    }
    
    if (verti != 0) {
        iterator = posTank[1] - 1;
        for (int i = iterator; i >= 0; i--) {
            blocNext = levelThread[posTank[0]][i];
            if (blocNext == 9) {
                dirA = true;
                break;
            }
            else if ((blocNext >= 4 && blocNext <= 8) ||
                (blocNext >= 10 && blocNext <= 14) ||
                (blocNext >= 19 && blocNext <= 23)) {
                dirA = false;
                break;
            }
        }
        iterator = posTank[1] + 1;
        if (dirA != true) {
            for (int i = iterator; i <= levelSize - 1; i++) {
                blocNext = levelThread[posTank[0]][i];
                if (blocNext == 10) {
                    dirB = true;
                    break;
                }
                else if ((blocNext >= 4 && blocNext <= 9) ||
                    (blocNext >= 11 && blocNext <= 14) ||
                    (blocNext >= 19 && blocNext <= 23)) {
                    dirB = false;
                    break;
                }
            }
        }
    }
    else if (hori != 0) {
        iterator = posTank[0] - 1;
        for (int i = iterator; i >= 0; i--) {
            blocNext = levelThread[i][posTank[1]];
            if (blocNext == 8) {
                dirA = true;
                break;
            }
            else if ((blocNext >= 4 && blocNext <= 7) ||
                (blocNext >= 9 && blocNext <= 14) ||
                (blocNext >= 19 && blocNext <= 23)) {
                dirA = false;
                break;
            }
        }
        iterator = posTank[0] + 1;
        if (dirA != true) {
            for (int i = iterator; i <= levelSize - 1; i++) {
                blocNext = levelThread[i][posTank[1]];
                if (blocNext == 7) {
                    dirB = true;
                    break;
                }
                else if ((blocNext >= 4 && blocNext <= 6) ||
                    (blocNext >= 8 && blocNext <= 14) ||
                    (blocNext >= 19 && blocNext <= 23)) {
                    dirB = false;
                    break;
                }
            }
        }
    }
    return dirA + dirB;
}


int* tankMoving(int* posTank, char* direction, int** levelThread, int mv, bool onIce = false, int posInitV = 0, int posInitH = 0) {
    int* retour;
    retour = new int[6];
    bool error = false;
    bool moved = false;
    bool won = false;
    bool died = false;
    bool portalFound = false;
    int blocNext = NULL;
    int verti = 0;
    int hori = 0;
    int* posInit = new int[2];
    if (onIce) {
        posInit[0] = posInitV;
        posInit[1] = posInitH;
    }
    else {
        posInit[0] = posTank[0];
        posInit[1] = posTank[1];
    }
    
    
    if (strcmp(direction, "U") == 0) {
        verti = -1;
    }
    else if (strcmp(direction, "D") == 0) {
        verti = 1;
    }
    else if (strcmp(direction, "R") == 0) {
        hori = 1;
    }
    else if (strcmp(direction, "L") == 0) {
        hori = -1;
    }
    if (posTank[0] + verti >= 0 &&
        posTank[0] + verti <= levelSize - 1 &&
        posTank[1] + hori >= 0 &&
        posTank[1] + hori <= levelSize - 1) {

        blocNext = levelThread[posTank[0] + verti][posTank[1] + hori];
        
        if (blocNext == 0) {
            posTank[0] = posTank[0] + verti;
            posTank[1] = posTank[1] + hori;
            moved = true;
        }
        else if (blocNext == 2) {
            posTank[0] = posTank[0] + verti;
            posTank[1] = posTank[1] + hori;
            moved = true;
            won = true;
        }
        else if (blocNext == 3 || blocNext == 25) {
            posTank[0] = posTank[0] + verti;
            posTank[1] = posTank[1] + hori;
            moved = true;
            died = true;
        }
        else if ((blocNext >= 4 && blocNext <= 14) ||
            (blocNext >= 19 && blocNext <= 23)) {
        }
        else if (blocNext >= 26 && blocNext <= 33) {
            for (int k = 0; k < levelSize; k++) {
                for (int l = 0; l < levelSize; l++) {
                    if (levelThread[k][l] == blocNext && (posTank[0] + verti != k || posTank[1] + hori != l)) {
                        posTank[0] = k;
                        posTank[1] = l;
                        portalFound = true;
                        break;
                    }
                }
                if (portalFound) {
                    break;
                }
            }
            if (!portalFound) {
                posTank[0] = posTank[0] + verti;
                posTank[1] = posTank[1] + hori;
            }
            moved = true;
        }
        else if (blocNext == 24) {
            posTank[0] += verti;
            posTank[1] += hori;
            moved = true;
            return tankMoving(posTank, direction, levelThread, mv, true, posInit[0], posInit[1]);
        }
        else if (blocNext == 15) {
            posTank[0] += verti;
            posTank[1] += hori;
            moved = true;
            char dir[1][2] = { "U" };
            return tankMoving(posTank, dir[0], levelThread, mv);
        }
        else if (blocNext == 16) {
            posTank[0] += verti;
            posTank[1] += hori;
            moved = true;
            char dir[1][2] = { "D" };
            return tankMoving(posTank, dir[0], levelThread, mv);
        }
        else if (blocNext == 17) {
            posTank[0] += verti;
            posTank[1] += hori;
            moved = true;
            char dir[1][2] = { "R" };
            return tankMoving(posTank, dir[0], levelThread, mv);
        }
        else if (blocNext == 18) {
            posTank[0] += verti;
            posTank[1] += hori;
            moved = true;
            char dir[1][2] = { "L" };
            return tankMoving(posTank, dir[0], levelThread, mv);
        }
        if (isFiredAt(posTank, direction, levelThread) == true &&
            blocNext != 24 && 
            (blocNext < 15 || blocNext > 18)) {
            died = true;
            moved = true;
            std::cout << "Killed" << std::endl;
        }
    }
    else {
        //std::cout << "Trying a move going above the limits of the map." << std::endl;
    }
    retour[0] = error;
    retour[1] = moved;
    retour[2] = won;
    retour[3] = died;
    retour[4] = posTank[0];
    retour[5] = posTank[1];
    int index = 0;
    if (strcmp(direction, "U") == 0) {
        index = 0;
    }
    else if (strcmp(direction, "D") == 0) {
        index = 1;
    }
    else if (strcmp(direction, "R") == 0) {
        index = 2;
    }
    else if (strcmp(direction, "L") == 0) {
        index = 3;
    }
    else {
        index = 4;
    }
    
    if (moved && !died) {
        scoreTab[posInit[0]][posInit[1]][index] += movedRECO;
    }
    else if (moved && died) {
        scoreTab[posInit[0]][posInit[1]][index] += deadPUNI;
    }
    else if (!moved) {
        scoreTab[posInit[0]][posInit[1]][index] += hardPUNI;
    }
    else if (moved && won) {
        scoreTab[posInit[0]][posInit[1]][index] += wonRECO;
    }
    
    return retour;
}


bool tankFiring(int* posFire, char* direction, int** levelThread, int* posInit) {
    bool error = false;
    int* retour = new int[2];
    int* firePosition = new int[2];
    int bloc = 0;
    int verti = 0;
    int hori = 0;

    if (strcmp(direction, "U") == 0) {
        verti = -1;
    }
    else if (strcmp(direction, "D") == 0) {
        verti = 1;
    }
    else if (strcmp(direction, "R") == 0) {
        hori = 1;
    }
    else if (strcmp(direction, "L") == 0) {
        hori = -1;
    }
    else {
        return true;
    }

    firePosition[0] = posFire[0] + verti;
    firePosition[1] = posFire[1] + hori;

    bloc = levelThread[firePosition[0]][firePosition[1]];

    if (firePosition[0] < 0 ||
        firePosition[0] > levelSize - 1 ||
        firePosition[1] < 0 ||
        firePosition[1] > levelSize - 1) {
        scoreTab[posInit[0]][posInit[1]][4] += softPUNI;
        error = false;
    }
    else if (bloc == 0 ||
        bloc == 2 ||
        bloc == 3 ||
        (bloc >= 15 && bloc <= 19) ||
        (bloc >= 24 && bloc <= 33)) {
        error = tankFiring(firePosition, direction, levelThread, posInit);
    }
    else if (bloc == 6) {
        levelThread[firePosition[0]][firePosition[1]] = 0;
        scoreTab[posInit[0]][posInit[1]][4] += movedRECO;
        error = false;
    }
    else if (bloc == 4) {
        scoreTab[posInit[0]][posInit[1]][4] += softPUNI;
        error = false;
    }
    else if (bloc == 5) {
        int afterBloc = levelThread[firePosition[0]+verti][firePosition[1]+hori];
        if (afterBloc == 0) {
            levelThread[firePosition[0] + verti][firePosition[1] + hori] = 5;
            if ((levelTab[firePosition[0]][firePosition[1]] >= 26 && levelTab[firePosition[0]][firePosition[1]] <= 33)
                || (levelTab[firePosition[0]][firePosition[1]] >= 15 && levelTab[firePosition[0]][firePosition[1]] <= 18)) {
                levelThread[firePosition[0]][firePosition[1]] = levelTab[firePosition[0]][firePosition[1]];
            }
            else {
                levelThread[firePosition[0]][firePosition[1]] = 0;
            }
            error = false;
        }
        else if (afterBloc == 3) {
            levelThread[firePosition[0] + verti][firePosition[1] + hori] = 0;
            if ((levelTab[firePosition[0]][firePosition[1]] >= 26 && levelTab[firePosition[0]][firePosition[1]] <= 33)
                || (levelTab[firePosition[0]][firePosition[1]] >= 15 && levelTab[firePosition[0]][firePosition[1]] <= 18)) {
                levelThread[firePosition[0]][firePosition[1]] = levelTab[firePosition[0]][firePosition[1]];
            }
            else {
                levelThread[firePosition[0]][firePosition[1]] = 0;
            }
            error = false;
        }
        else if ((afterBloc >= 4 && afterBloc <= 14) ||
            afterBloc >= 19 && afterBloc <= 23) {
            error = false;
        }
    }
    else if (bloc == 7) {
        if (strcmp(direction, "D") == 0) {
            levelThread[firePosition[0]][firePosition[1]] = 4;
            scoreTab[posInit[0]][posInit[1]][4] += movedRECO;
        }
        error = false;
    }
    else if (bloc == 8) {
        if (strcmp(direction, "U") == 0) {
            levelThread[firePosition[0]][firePosition[1]] = 4;
            scoreTab[posInit[0]][posInit[1]][4] += movedRECO;
        }
        error = false;
    }
    else if (bloc == 9) {
        if (strcmp(direction, "L") == 0) {
            levelThread[firePosition[0]][firePosition[1]] = 4;
            scoreTab[posInit[0]][posInit[1]][4] += movedRECO;
        }
        error = false;
    }
    else if (bloc == 10) {
        if (strcmp(direction, "R") == 0) {
            levelThread[firePosition[0]][firePosition[1]] = 4;
            scoreTab[posInit[0]][posInit[1]][4] += movedRECO;
        }
        error = false;
    }
    else if (bloc == 11) {
        if (strcmp(direction, "D") == 0) {
            char toSend[2] = { "R" };
            error = tankFiring(firePosition, toSend, levelThread, posInit);
        }
        if (strcmp(direction, "L") == 0) {
            char toSend[2] = { "U" };
            error = tankFiring(firePosition, toSend, levelThread, posInit);
        }
        else {
            scoreTab[posInit[0]][posInit[1]][4] += softPUNI;
            error = false;
        }
    }
    else if (bloc == 12) {
        if (strcmp(direction, "D") == 0) {
            char toSend[2] = { "L" };
            error = tankFiring(firePosition, toSend, levelThread, posInit);
        }
        if (strcmp(direction, "R") == 0) {
            char toSend[2] = { "U" };
            error = tankFiring(firePosition, toSend, levelThread, posInit);
        }
        else {
            scoreTab[posInit[0]][posInit[1]][4] += softPUNI;
            error = false;
        }
    }
    else if (bloc == 13) {
        if (strcmp(direction, "U") == 0) {
            char toSend[2] = { "R" };
            error = tankFiring(firePosition, toSend, levelThread, posInit);
        }
        if (strcmp(direction, "L") == 0) {
            char toSend[2] = { "D" };
            error = tankFiring(firePosition, toSend, levelThread, posInit);
        }
        else {
            scoreTab[posInit[0]][posInit[1]][4] += softPUNI;
            error = false;
        }
    }
    else if (bloc == 14) {
        if (strcmp(direction, "U") == 0) {
            char toSend[2] = { "L" };
            error = tankFiring(firePosition, toSend, levelThread, posInit);
        }
        if (strcmp(direction, "R") == 0) {
            char toSend[2] = { "D" };
            error = tankFiring(firePosition, toSend, levelThread, posInit);
        }
        else {
            scoreTab[posInit[0]][posInit[1]][4] += softPUNI;
            error = false;
        }
    }
    else if (bloc == 20) {
        if (strcmp(direction, "D") == 0) {
            char toSend[2] = { "R" };
            error = tankFiring(firePosition, toSend, levelThread, posInit);
        }
        if (strcmp(direction, "L") == 0) {
            char toSend[2] = { "U" };
            error = tankFiring(firePosition, toSend, levelThread, posInit);
        }
        else {
            levelThread[firePosition[0]][firePosition[1]] = 22;
            error = false;
        }
    }
    else if (bloc == 21) {
        if (strcmp(direction, "D") == 0) {
            char toSend[2] = { "L" };
            error = tankFiring(firePosition, toSend, levelThread, posInit);
        }
        if (strcmp(direction, "R") == 0) {
            char toSend[2] = { "U" };
            error = tankFiring(firePosition, toSend, levelThread, posInit);
        }
        else {
            levelThread[firePosition[0]][firePosition[1]] = 20;
            error = false;
        }
    }
    else if (bloc == 22) {
        if (strcmp(direction, "U") == 0) {
            char toSend[2] = { "R" };
            error = tankFiring(firePosition, toSend, levelThread, posInit);
        }
        if (strcmp(direction, "L") == 0) {
            char toSend[2] = { "D" };
            error = tankFiring(firePosition, toSend, levelThread, posInit);
        }
        else {
            levelThread[firePosition[0]][firePosition[1]] = 23;
            error = false;
        }
    }
    else if (bloc == 23) {
        if (strcmp(direction, "U") == 0) {
            char toSend[2] = { "L" };
            error = tankFiring(firePosition, toSend, levelThread, posInit);
        }
        if (strcmp(direction, "R") == 0) {
            char toSend[2] = { "D" };
            error = tankFiring(firePosition, toSend, levelThread, posInit);
        }
        else {
            levelThread[firePosition[0]][firePosition[1]] = 21;
            error = false;
        }
    }

    return error;
}


int game(int th) {
    int* posInterTank = new int[2];
    posInterTank[0] = posTu[0];
    posInterTank[1] = posTu[1];

    int* start = new int[2];
    start[0] = posTu[0];
    start[1] = posTu[1];

    int* posTank = new int[2];
    posTank[0] = posTu[0];
    posTank[1] = posTu[1];

    int** seqMvg = new int*[maxIt];
    for (int i = 0; i < maxIt; i++) {
        seqMvg[i] = new int[3];
    }
    for (int i = 0; i < maxIt; i++) {
        for (int j = 0; j < 3; j++) {
            seqMvg[i][j] = NULL;
        }
    }
    char posFaced[2] = "U";
    char seqMvgChoice[5][2] = { "U", "D", "R", "L", "F"};
    int* retour;
    retour = new int[6];
    int choice = 0;

    int** levelTabThread = new int* [levelSize];
    for (int i = 0; i < levelSize; i++) {
        levelTabThread[i] = new int[levelSize];
    }

    for (int i = 0; i < levelSize; i++) {
        for (int j = 0; j < levelSize; j++) {
            levelTabThread[i][j] = levelTab[i][j];
        }
    }
    bool facedDir = false;
    int lastMove = 0;

    int threadFollowing = rand() % 10;
    threadFollowing = threadFollowing < followRate;

    for (int i = 0; i < maxIt; i++) {
        posInterTank[0] = posTank[0];
        posInterTank[1] = posTank[1];
        int max_element = scoreTab[posTank[0]][posTank[1]][0];
        int max_index = 0;

        if (threadFollowing) {
            if (i% followTime >= 0 && i % followTime <= nonFollowTime) {
                choice = rand() % 5;
                if (facedDir) {
                    if (choice % 2 == 0) {
                        choice = lastMove;
                    }
                    else {
                        choice = 4;
                    }
                }
            }
            else {
                for (int j = 0; j < 5; j++) {
                    if (scoreTab[posTank[0]][posTank[1]][j] > max_element) {
                        max_index = j;
                        max_element = scoreTab[posTank[0]][posTank[1]][j];
                    }
                }
                choice = max_index;
            } 
        }
        else {
            choice = rand() % 10;
            if (choice <= explo) {
                choice = rand() % 5;
                if (facedDir) {
                    if (choice % 2 == 0) {
                        choice = lastMove;
                    }
                    else {
                        choice = 4;
                    }
                }
            }
            else {
                for (int j = 0; j < 5; j++) {
                    if (scoreTab[posTank[0]][posTank[1]][j] > max_element) {
                        max_index = j;
                        max_element = scoreTab[posTank[0]][posTank[1]][j];
                    }
                }
                choice = max_index;
            }
        }

        facedDir = false;
        lastMove = choice;
        if (strcmp(seqMvgChoice[choice], "U") != 0 &&
            strcmp(seqMvgChoice[choice], "D") != 0 &&
            strcmp(seqMvgChoice[choice], "R") != 0 &&
            strcmp(seqMvgChoice[choice], "L") != 0 &&
            strcmp(seqMvgChoice[choice], "F") != 0) {
            std::cout << "Error in seqMvgChoice, not a move. Should have been U D R L or F." << std::endl;
            break;
        }
        else if (strcmp(seqMvgChoice[choice], posFaced) != 0) {
            strcpy(posFaced, seqMvgChoice[choice]);
            facedDir = true;
            seqMvg[i][0] = choice;
            seqMvg[i][1] = posInterTank[0];
            seqMvg[i][2] = posInterTank[1];
        }
        else {
            if (strcmp(seqMvgChoice[choice], "F") == 0) {
                int errFire = tankFiring(posTank, posFaced, levelTabThread, posTank);
                seqMvg[i][0] = choice;
                seqMvg[i][1] = posInterTank[0];
                seqMvg[i][2] = posInterTank[1];
            }
            else {
                retour = tankMoving(posTank, seqMvgChoice[choice], levelTabThread, i);
                posTank[0] = retour[4];
                posTank[1] = retour[5];
                seqMvg[i][0] = choice;
                seqMvg[i][1] = posInterTank[0];
                seqMvg[i][2] = posInterTank[1];
                if (retour[0]) {
                    std::cout << "An error has occured." << std::endl;
                    break;
                }
                if (retour[1]) {
                }
                if (retour[2]) {
                    char way2win[10000];
                    strcpy(way2win, "");
                    if (i < best) {
                        for (int j = 0; j < i; j++) {
                            strcat(way2win, seqMvgChoice[seqMvg[j][0]]);
                            scoreTab[seqMvg[j][1]][seqMvg[j][2]][seqMvg[j][0]] += wonRECO;
                        }
                        writeBest(i, th, way2win, posTank, start);
                    }
                }
                if (retour[3]) {
                    for (int j = 0; j < i; j++) {
                        scoreTab[seqMvg[j][1]][seqMvg[j][2]][seqMvg[j][0]] += deadPUNI;
                    }
                }
            }
        }
    }
    for (int i = 0; i < levelSize; i++) {
        delete[] levelTabThread[i];
    }
    delete[] levelTabThread;
    for (int i = 0; i < maxIt; i++) {
        delete[] seqMvg[i];
    }
    delete[] seqMvg;
    delete[] posTank;
    delete[] posInterTank;
    delete[] start;
    return 1;
}


int main()
{
    srand(time(NULL));
    char bloc[3]; // Contiendra un à un les blocs du tableau de jeu
    char codedBloc[34][10]; // Contiendra un nombre représentant le bloc dans le parseur
    char line[256];
    FILE* laserTab; // Fichier .txt du tableau de jeu
    laserTab = fopen("doc4.txt", "rt"); // Le fichier est ouvert en lecture de .txt
    printf("Level size : ");
    scanf("%d", &levelSize);
    printf("Number of threads : ");
    scanf("%d", &NumberThreads);


    strcpy(codedBloc[0], "D");
    strcpy(codedBloc[1], "Tu");
    strcpy(codedBloc[2], "b");
    strcpy(codedBloc[3], "w");
    strcpy(codedBloc[4], "Bs");
    strcpy(codedBloc[5], "Bm");
    strcpy(codedBloc[6], "B");
    strcpy(codedBloc[7], "Au");
    strcpy(codedBloc[8], "Ad");
    strcpy(codedBloc[9], "Ar");
    strcpy(codedBloc[10], "Al");
    strcpy(codedBloc[11], "Mur");
    strcpy(codedBloc[12], "Mul");
    strcpy(codedBloc[13], "Mdr");
    strcpy(codedBloc[14], "Mdl");
    strcpy(codedBloc[15], "Wu");
    strcpy(codedBloc[16], "Wd");
    strcpy(codedBloc[17], "Wr");
    strcpy(codedBloc[18], "Wl");
    strcpy(codedBloc[19], "C");
    strcpy(codedBloc[20], "Rur");
    strcpy(codedBloc[21], "Rul");
    strcpy(codedBloc[22], "Rdr");
    strcpy(codedBloc[23], "Rdl");
    strcpy(codedBloc[24], "I");
    strcpy(codedBloc[25], "i");
    strcpy(codedBloc[26], "Tr");
    strcpy(codedBloc[27], "Tg");
    strcpy(codedBloc[28], "Tb");
    strcpy(codedBloc[29], "Tc");
    strcpy(codedBloc[30], "Ty");
    strcpy(codedBloc[31], "Tp");
    strcpy(codedBloc[32], "Tw");
    strcpy(codedBloc[33], "Td");


    // Le tableau de jeu commence après une ligne vide dans le .txt
    // La boucle ci-dessous lis ligne par ligne le .txt et est brisée
    // une fois la ligne vide trouvée.
    int countLines = 0;
    while (fgets(line, sizeof(line), laserTab)) {
        countLines += 1;
        if (countLines == 3) {
            strcpy(levelName, line);
        }
        if (strspn(line, " \t\n") == strlen(line)) {
            break;
        }
    }


    // Puisque le .txt n'est pas fermé entre temps, la fonction 
    // fscanf reprend la lecture là où s'est arrêté la fonction précédente.
    // Dès lors, le tableau de jeu est lu.
    levelTab = new int* [levelSize];
    for (int i = 0; i < levelSize; i++) {
        levelTab[i] = new int[levelSize];
    }
    for (int i = 0; i < levelSize; i++) {
        for (int j = 0; j < levelSize; j++) {
            fscanf(laserTab, "%s", bloc);
            for (int k = 0; k < 34; k++) {
                if (strcmp(bloc, codedBloc[k]) == 0) {
                    levelTab[i][j] = k;
                    if (k == 1) {
                        posTu[0] = i;
                        posTu[1] = j;
                        levelTab[i][j] = 0;
                    }
                    break;
                }
            }
        }
    }
    fclose(laserTab);


    scoreTab = new int** [levelSize];
    for (int i = 0; i < levelSize; i++) {
        scoreTab[i] = new int*[levelSize];
    }
    for (int i = 0; i < levelSize; i++) {
        for (int j = 0; j < levelSize; j++) {
            scoreTab[i][j] = new int[5];
        }
    }
    for (int i = 0; i < levelSize; i++) {
        for (int j = 0; j < levelSize; j++) {
            for (int k = 0; k < 5; k++)
            scoreTab[i][j][k] = 0;
        }
    }

    auto start = std::chrono::system_clock::now();
    tab_t = new std::thread * [NumberThreads];
    for (int k = 0; k < 1000; k++) {
        for (int i = 0; i < NumberThreads; i++) {
            tab_t[i] = new std::thread(game, i);
        }
        for (int i = 0; i < NumberThreads; i++) {
            tab_t[i]->join();
        }
    }

    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << "computation time : " << elapsed.count() << " [s]" << std::endl;

    std::cout << "Best is : " << best << std::endl;
    for (int i = 0; i < levelSize; i++) {
        for (int j = 0; j < levelSize; j++) {
            for (int k = 0; k < 5; k++) {
                std::cout << scoreTab[i][j][k] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    return 0;
}