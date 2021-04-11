/*
* main.c
 *
 *  Created on: Feb 9, 2021
 *      Author: student
 *
 */
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>

#define BLOK_I 0
#define BLOK_J 1
#define BLOK_L 2
#define BLOK_O 3
#define BLOK_S 4
#define BLOK_T 5
#define BLOK_Z 6
//zmienne globalne
uint16_t fb_main[33];
uint16_t fb_blocks[32];
uint16_t fb_floor[33];
uint16_t fb_nextblock[2];

uint16_t fb_points100[5];
uint16_t fb_points010[5];
uint16_t fb_points001[5];

uint8_t kordx = 8;
uint8_t kordy = 8; 

uint16_t points = 0;
uint8_t nextblock;
uint8_t currentblock;
uint8_t possition;

//deklaracje funkcji
void led_send();
void updatepoints();
void deletelevel();
void nextblockdisplay();
void newblock();
int spacedown();
void movedown();
int spaceleft();
void moveleft();
int spaceright();
void moveright();
void updateframebuffer();
void rotateRight(int x);
void heh();
void fb_init();
void gameover(); //Sołtys
//kody funkcji
void printfb(uint16_t n){
    while (n) {
        if (n & 1)
            printf("O");
        else
            printf("-");

        n >>= 1;
    }
    printf("\n");
}
int main(void) { //init
    //display "PLAY" untill any button == 1
    fb_floor[14] = 0xe8ea;                                    //
    fb_floor[15] = 0xa8aa;                                    //
    fb_floor[16] = 0xe8e4;                                    //
    fb_floor[17] = 0x88a4;                                    //
    fb_floor[18] = 0x8ea4;                                    //
    updateframebuffer();                                      //
    sleep(2);
    fb_init();
    nextblock = BLOK_I;
    newblock();
    char input;
    printf("done, kordx %d, kordy %d, nextblock %d", kordx, kordy, nextblock);
    for (;;) {	//tymczasowe test wyswietlacza
        scanf("%c",&input);
        if(input=='w') rotateRight(nextblock);
        else if (input=='c') {
            if(spacedown()){
                movedown();
            }
        }
        else if (input=='s') {
            printf("\n");
            while(spacedown()) {
                movedown();
            }
            movedown();
            printf("done, kordx %d, kordy %d", kordx, kordy);
        }
        else if (input=='a') {printf("\n"); moveleft(); printf("done, kordx %d, kordy %d", kordx, kordy);}
        else if (input=='x') {printf("\n"); gameover(); printf("done, kordx %d, kordy %d", kordx, kordy);}
        else if (input=='d') {printf("\n"); moveright(); printf("done, kordx %d, kordy %d", kordx, kordy);}
        else if (input=='m') {printf("\n"); led_send(); printf("done, kordx %d, kordy %d", kordx, kordy);}
        else if (input=='f') {
            printf("\nfb_floor\n");
            for(int i=0;i<32;++i){
                printfb(fb_floor[i]);
            }
            printf("done, kordx %d, kordy %d", kordx, kordy);
        }

        else if (input=='b'){
            printf("\nfb_blocks\n");
            for(int i=0;i<32;++i){
                printfb(fb_blocks[i]);
            }
            printf("done, kordx %d, kordy %d", kordx, kordy);
        }
        sleep(1);

    }
    return (0);

}
void led_send() {
    printf("\nfb_main\n");
    for(int i=0;i<32;++i){
        printfb(fb_main[i]);
    }

}


void updatepoints() {
    points++;

    uint8_t hundreds;
    uint8_t tens;
    uint8_t unity;

//	hundreds = points % 1000;
    hundreds = points / 100;
    tens = points % 100;
    tens = tens / 10;
    unity = points % 10;

    switch (hundreds) {
        case 0:
            fb_points100[0] = 0xe000;
            fb_points100[1] = 0xa000;
            fb_points100[2] = 0xa000;
            fb_points100[3] = 0xa000;
            fb_points100[4] = 0xe000;
            break;
        case 1:
            fb_points100[0] = 0x4000;
            fb_points100[1] = 0xc000;
            fb_points100[2] = 0x4000;
            fb_points100[3] = 0x4000;
            fb_points100[4] = 0xe000;
            break;
        case 2:
            fb_points100[0] = 0xe000;
            fb_points100[1] = 0x2000;
            fb_points100[2] = 0xe000;
            fb_points100[3] = 0x8000;
            fb_points100[4] = 0xe000;
            break;
        case 3:
            fb_points100[0] = 0xe000;
            fb_points100[1] = 0x2000;
            fb_points100[2] = 0xe000;
            fb_points100[3] = 0x2000;
            fb_points100[4] = 0xe000;
            break;
        case 4:
            fb_points100[0] = 0xa000;
            fb_points100[1] = 0xa000;
            fb_points100[2] = 0xe000;
            fb_points100[3] = 0x2000;
            fb_points100[4] = 0x2000;
            break;
        case 5:
            fb_points100[0] = 0xe000;
            fb_points100[1] = 0x8000;
            fb_points100[2] = 0xe000;
            fb_points100[3] = 0x2000;
            fb_points100[4] = 0xe000;
            break;
        case 6:
            fb_points100[0] = 0xe000;
            fb_points100[1] = 0x8000;
            fb_points100[2] = 0xe000;
            fb_points100[3] = 0xa000;
            fb_points100[4] = 0xe000;
            break;
        case 7:
            fb_points100[0] = 0xe000;
            fb_points100[1] = 0xa000;
            fb_points100[2] = 0x2000;
            fb_points100[3] = 0x2000;
            fb_points100[4] = 0x2000;
            break;
        case 8:
            fb_points100[0] = 0xe000;
            fb_points100[1] = 0xa000;
            fb_points100[2] = 0xe000;
            fb_points100[3] = 0xa000;
            fb_points100[4] = 0xe000;
            break;
        case 9:
            fb_points100[0] = 0xe000;
            fb_points100[1] = 0xa000;
            fb_points100[2] = 0xe000;
            fb_points100[3] = 0x2000;
            fb_points100[4] = 0xe000;
            break;
    }

    switch (tens) {
        case 0:
            fb_points010[0] = 0x0e00;
            fb_points010[1] = 0x0a00;
            fb_points010[2] = 0x0a00;
            fb_points010[3] = 0x0a00;
            fb_points010[4] = 0x0e00;
            break;
        case 1:
            fb_points010[0] = 0x0400;
            fb_points010[1] = 0x0c00;
            fb_points010[2] = 0x0400;
            fb_points010[3] = 0x0400;
            fb_points010[4] = 0x0e00;
            break;
        case 2:
            fb_points010[0] = 0x0e00;
            fb_points010[1] = 0x0200;
            fb_points010[2] = 0x0e00;
            fb_points010[3] = 0x0800;
            fb_points010[4] = 0x0e00;
            break;
        case 3:
            fb_points010[0] = 0x0e00;
            fb_points010[1] = 0x0200;
            fb_points010[2] = 0x0e00;
            fb_points010[3] = 0x0200;
            fb_points010[4] = 0x0e00;
            break;
        case 4:
            fb_points010[0] = 0x0a00;
            fb_points010[1] = 0x0a00;
            fb_points010[2] = 0x0e00;
            fb_points010[3] = 0x0200;
            fb_points010[4] = 0x0200;
            break;
        case 5:
            fb_points010[0] = 0x0e00;
            fb_points010[1] = 0x0800;
            fb_points010[2] = 0x0e00;
            fb_points010[3] = 0x0200;
            fb_points010[4] = 0x0e00;
            break;
        case 6:
            fb_points010[0] = 0x0e00;
            fb_points010[1] = 0x0800;
            fb_points010[2] = 0x0e00;
            fb_points010[3] = 0x0a00;
            fb_points010[4] = 0x0e00;
            break;
        case 7:
            fb_points010[0] = 0x0e00;
            fb_points010[1] = 0x0a00;
            fb_points010[2] = 0x0200;
            fb_points010[3] = 0x0200;
            fb_points010[4] = 0x0200;
            break;
        case 8:
            fb_points010[0] = 0x0e00;
            fb_points010[1] = 0x0a00;
            fb_points010[2] = 0x0e00;
            fb_points010[3] = 0x0a00;
            fb_points010[4] = 0x0e00;
            break;
        case 9:
            fb_points010[0] = 0x0e00;
            fb_points010[1] = 0x0a00;
            fb_points010[2] = 0x0e00;
            fb_points010[3] = 0x0200;
            fb_points010[4] = 0x0e00;
            break;
    }

    switch (unity) {
        case 0:
            fb_points001[0] = 0x00e0;
            fb_points001[1] = 0x0090;
            fb_points001[2] = 0x0090;
            fb_points001[3] = 0x0090;
            fb_points001[4] = 0x00e0;
            break;
        case 1:
            fb_points001[0] = 0x0040;
            fb_points001[1] = 0x00c0;
            fb_points001[2] = 0x0040;
            fb_points001[3] = 0x0040;
            fb_points001[4] = 0x00e0;
            break;
        case 2:
            fb_points001[0] = 0x00e0;
            fb_points001[1] = 0x0020;
            fb_points001[2] = 0x00e0;
            fb_points001[3] = 0x0080;
            fb_points001[4] = 0x00e0;
            break;
        case 3:
            fb_points001[0] = 0x00e0;
            fb_points001[1] = 0x0020;
            fb_points001[2] = 0x00e0;
            fb_points001[3] = 0x0020;
            fb_points001[4] = 0x00e0;
            break;
        case 4:
            fb_points001[0] = 0x00a0;
            fb_points001[1] = 0x00a0;
            fb_points001[2] = 0x00e0;
            fb_points001[3] = 0x0020;
            fb_points001[4] = 0x0020;
            break;
        case 5:
            fb_points001[0] = 0x00e0;
            fb_points001[1] = 0x0080;
            fb_points001[2] = 0x00e0;
            fb_points001[3] = 0x0020;
            fb_points001[4] = 0x00e0;
            break;
        case 6:
            fb_points001[0] = 0x00e0;
            fb_points001[1] = 0x0080;
            fb_points001[2] = 0x00e0;
            fb_points001[3] = 0x00a0;
            fb_points001[4] = 0x00e0;
            break;
        case 7:
            fb_points001[0] = 0x00e0;
            fb_points001[1] = 0x00a0;
            fb_points001[2] = 0x0020;
            fb_points001[3] = 0x0020;
            fb_points001[4] = 0x0020;
            break;
        case 8:
            fb_points001[0] = 0x00e0;
            fb_points001[1] = 0x00a0;
            fb_points001[2] = 0x00e0;
            fb_points001[3] = 0x00a0;
            fb_points001[4] = 0x00e0;
            break;
        case 9:
            fb_points001[0] = 0x00e0;
            fb_points001[1] = 0x00a0;
            fb_points001[2] = 0x00e0;
            fb_points001[3] = 0x0020;
            fb_points001[4] = 0x00e0;
            break;
    }
    updateframebuffer();
}
void deletelevel() {
    int i = 31;
    while (i > 7) {
        if (fb_floor[i] == 0xffff) {
            for (uint8_t j = i; j > 8; j--) {
                fb_floor[j] = fb_floor[j - 1];
            }
            fb_floor[8] = 0xc003;
            updatepoints();
            continue;
        }
        i--;
    }
}
void nextblockdisplay() {
    switch (nextblock) {
        case BLOK_I:
            fb_nextblock[0] = 0x000f;
            fb_nextblock[1] = 0x0000;
            break;
        case BLOK_J:
            fb_nextblock[0] = 0x000e;
            fb_nextblock[1] = 0x0002;
            break;
        case BLOK_L:
            fb_nextblock[0] = 0x000e;
            fb_nextblock[1] = 0x0008;
            break;
        case BLOK_O:
            fb_nextblock[0] = 0x0006;
            fb_nextblock[1] = 0x0006;
            break;
        case BLOK_S:
            fb_nextblock[0] = 0x0006;
            fb_nextblock[1] = 0x000c;
            break;
        case BLOK_T:
            fb_nextblock[0] = 0x000e;
            fb_nextblock[1] = 0x0004;
            break;
        case BLOK_Z:
            fb_nextblock[0] = 0x000c;
            fb_nextblock[1] = 0x0006;
            break;
    }
    updateframebuffer();
}
void newblock() {
    for (int i = -2; i<=2; ++i) {          //
        fb_blocks[kordy + i] = 0x0000;     //
    }                                      //
    kordx = 8;
    kordy = 8;
    possition = 0;
    currentblock = nextblock;
    //nextblock = random(); //TODO

    switch (currentblock) {
        case BLOK_I:
            fb_blocks[8] = 0x03c0;
            fb_blocks[9] = 0x0000;
            break;
        case BLOK_J:
            fb_blocks[8] = 0x0380;
            fb_blocks[9] = 0x0080;
            break;
        case BLOK_L:
            fb_blocks[8] = 0x0380;
            fb_blocks[9] = 0x0200;
            break;
        case BLOK_O:
            fb_blocks[8] = 0x0180;
            fb_blocks[9] = 0x0180;
            break;
        case BLOK_S:
            fb_blocks[8] = 0x0180;
            fb_blocks[9] = 0x0300;
            break;
        case BLOK_T:
            fb_blocks[8] = 0x0380;
            fb_blocks[9] = 0x0100;
            break;
        case BLOK_Z:
            fb_blocks[8] = 0x0300;
            fb_blocks[9] = 0x0180;
            break;
    }
    nextblockdisplay();
    updateframebuffer();
    for (uint8_t i=9; i>=8; i--) {
        if (fb_blocks[i] & fb_floor[i]) gameover();
    }
}
void gameover() {                             //
    sleep(1);
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 32; ++i) {
            fb_main[i] = 0xffff;
        }
        led_send();
        sleep(1);
        for (int i = 0; i < 32; ++i) {
            fb_main[i] = 0x0000;
        }
        led_send();
        sleep(1);
    }
    points--;
    for (int i = 0; i < 32; ++i) {
        fb_floor[i] = 0x0000;
        fb_blocks[i] = 0x0000;
        fb_nextblock[i] = 0x0000;
    }
    updatepoints();
    for (;;) {}                              //wyświetlaj punkty do momentu kolejnego resetu
}
int spacedown() {
    if (fb_blocks[31]) {
        return 0;
    }
    for (int8_t i = 30; i >= 7; i--) {
        if (fb_blocks[i] & fb_floor[i + 1])
            return 0;
    }
    return 1;
}
void movedown() {
    if (spacedown()) {
        for (int8_t i = 31; i > 7; i--) {
            fb_blocks[i] = fb_blocks[i - 1];
        }
        kordy++;
        updateframebuffer();
    } else {
        for (int8_t i = 31; i > 7; i--) {
            fb_floor[i] += fb_blocks[i];
        }
        deletelevel();
        newblock();
    }
}
int spaceleft() {
    for (int i = -2; i<=2; ++i){
            if(fb_blocks[kordy+i] & fb_floor[kordy+i]>>1) return 0;
        }
    return 1;
}
void moveleft() {
    if (spaceleft()) {
        for (int8_t y = -1; y < 3; y++) {
            fb_blocks[kordy + y] = (fb_blocks[kordy + y] << 1);
        }
        kordx++;
        updateframebuffer();
    }
    updateframebuffer(); //TODO usunąć
}
int spaceright() {
    for (int i = -2; i<=2; ++i) {
        if (fb_blocks[kordy + i] & fb_floor[kordy + i] << 1) return 0;
    }
    return 1;
}
void moveright() {
    if (spaceright()) {
        for (int8_t y = -1; y < 3; y++) {
            fb_blocks[kordy + y] = (fb_blocks[kordy + y] >> 1);
        }
        kordx--;
        updateframebuffer();
    }
    updateframebuffer();
}
void rotateRight(int nextblock) { 

    if(nextblock){
        int8_t z = 0;
        int8_t crossArr[5] = { };
        int8_t diamondArr[4] = { };
        int8_t c = 0;
        int8_t d = 0;
    
        for (int8_t i = -1; i < 2; i++) {
            for (int8_t j = -1; j < 2; j++) {
                if (fb_blocks[kordy + i] & (1 << (kordx + j))) {
                    if ((i + j) % 2) {
                        diamondArr[c] = 1;
                        c++;
                    } else {
                        crossArr[d] = 1;
                        d++;
                    }
                } else {
                    if ((i + j) % 2) {
                        diamondArr[c] = 0;
                        c++;
                    } else {
                        crossArr[d] = 0;
                        d++;
                    }
                }
            }
        }
    
        int8_t newTable[9] = { };
    
        newTable[0] = crossArr[1];
        newTable[1] = diamondArr[2];
        newTable[2] = crossArr[4];
        newTable[3] = diamondArr[0];
        newTable[4] = crossArr[2];
        newTable[5] = diamondArr[3];
        newTable[6] = crossArr[0];
        newTable[7] = diamondArr[1];
        newTable[8] = crossArr[3];
    
    
        //Walidacja
        c=1;
        int g;
        for (int8_t i = -1; i < 2; i++) {
            g = fb_floor[kordy + i];
            for (int8_t j = -1; j < 2; j++) {
                if(newTable[i*3+j+4]){
                    if(g & (1 << (kordx + j))){
                        c=0;
                    }
                }
            }
        }

        if (c) {
            for (int8_t i = -1; i < 2; i++) {
                g=0;
                for (int8_t j = -1; j < 2; j++) {
                    if(newTable[i*3+j+4]){
                        g = g ^ (1 << (kordx + j));
                    }
                }
                fb_blocks[kordy + i] = g;
            }
            updateframebuffer();
        }
    }else{
        //Sprawdź pozycję
        if(fb_blocks[kordy-1] == (1<<kordx)){
            //Walidacja pion -> poziom
            if(!((15<<(kordx-2)) & fb_floor[kordy])){
                fb_blocks[kordy-1] = 0;
                fb_blocks[kordy] = 15<<(kordx-2);
                fb_blocks[kordy+1] = 0;
                fb_blocks[kordy+2] = 0;
                updateframebuffer();
            }
        }else{
            int8_t c = 1;
            //Walidacja poziom -> pion
            for(int8_t i = -1; i<3; i++){
                if(fb_floor[kordy+i] & (1<<kordx)){
                    c=0;
                }
            }
            if(c){
                for(int8_t i = -1; i<3; i++){
                    fb_blocks[kordy+i] = 1<<kordx;
                }
                updateframebuffer();
            }
        }
    }
}

void updateframebuffer() {
    for (int i = 0; i < 32; i++) {
        fb_main[i] = fb_blocks[i] + fb_floor[i];
    }
    fb_main[3] = fb_nextblock[0];
    fb_main[4] = fb_nextblock[1];
    for (int i = 1; i < 6; i++) {
        fb_main[i] += fb_points100[i - 1] + fb_points010[i - 1]
                      + fb_points001[i - 1];
    }
    led_send();
}
void fb_init() {
    for (uint8_t i = 0; i < 8; ++i) {
        fb_blocks[i] = 0;
        fb_floor[i] = 0x0;
    }
    for (uint8_t i = 7; i < 32; ++i) {
        fb_blocks[i] = 0;
        fb_floor[i] = 0xc003;
    }
    fb_floor[7] = 0xffff;                     
    fb_points100[0] = 0xe000;
    fb_points100[1] = 0xa000;
    fb_points100[2] = 0xa000;
    fb_points100[3] = 0xa000;
    fb_points100[4] = 0xe000;
    fb_points010[0] = 0x0e00;
    fb_points010[1] = 0x0a00;
    fb_points010[2] = 0x0a00;
    fb_points010[3] = 0x0a00;
    fb_points010[4] = 0x0e00;
    fb_points001[0] = 0x00e0;
    fb_points001[1] = 0x00a0;
    fb_points001[2] = 0x00a0;
    fb_points001[3] = 0x00a0;
    fb_points001[4] = 0x00e0;                
}
