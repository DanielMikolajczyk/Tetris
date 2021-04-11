/*
 * main.c
 *
 *  Created on: Feb 9, 2021
 *      Author: student
 */
#include "rand8.h"
#include <avr/io.h>
//	#include <avr/iom328p.h>  //zakomentowac przed kompilacja
#include <avr/interrupt.h>
#include <util/delay.h>
//deklaracje MAkr
#define PIN_SCK                   PB5
#define PIN_MOSI                  PB3
#define PIN_SS1                    PB2
#define PIN_SS0                    PB1

#define PC1_WC !(PINC & (1<<PC1))
#define PC0_WC !(PINC & (1<<PC0))
#define PC3_WC !(PINC & (1<<PC3))
#define PC2_WC !(PINC & (1<<PC2))
#define WYSWIETL_LEWY 2
#define WYSWIETL_PRAWY 4
#define WYSWIETL_OBA 0;
#define MAX7219_NOOP 0
#define MAX7219_DIG1 1
#define MAX7219_DIG2 2
#define MAX7219_DIG3 3
#define MAX7219_DIG4 4
#define MAX7219_DIG5 5
#define MAX7219_DIG6 6
#define MAX7219_DIG7 7
#define MAX7219_DIG8 8
#define MAX7219_MODE_DECODE       0x09
#define MAX7219_MODE_INTENSITY    0x0A
#define MAX7219_MODE_SCAN_LIMIT   0x0B
#define MAX7219_MODE_POWER        0x0C
#define MAX7219_MODE_TEST         0x0F

#define BLOK_I 0
#define BLOK_J 1
#define BLOK_L 2
#define BLOK_O 3
#define BLOK_S 4
#define BLOK_T 5
#define BLOK_Z 6
//zmienne globalne
uint16_t fb_main[32];
uint16_t fb_blocks[32];
uint16_t fb_floor[32];
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
uint8_t lvl;
volatile uint16_t tim_ms = 0;
volatile uint8_t lock=0;
//deklaracje funkcji
void init_adc();
//uint16_t read_adc(uint8_t adc_channel);
uint8_t rand8(void);

void led_init();
void led_send();
void spiSendByte(char databyte);
void TIM0_Init();
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
void rotateRight();
void fb_init();
void gameover(); 
//kody funkcji
int main(void) {
//init
	//PINY
	DDRB |= (1 << PIN_SCK) | (1 << PIN_MOSI) | (1 << PIN_SS1) | (1 << PIN_SS0)
			| (1 << PB0);
	PORTB = 15;
	DDRC = 0;	//ustwienie przyciskow
	PORTC = 255;
	// SPI
	SPCR |= (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);

	//timer1
	TIM0_Init();
	//adc
	init_adc();

	//wyswietlacze
	led_init();
	for (uint8_t i = 0; i < 4; i++) {	//skan
			PORTB = WYSWIETL_OBA
			;	//ustawieneie SS
			spiSendByte(MAX7219_MODE_SCAN_LIMIT);
			spiSendByte(7);
			PORTB = (((1 << PIN_SS1)) | ((1 << PIN_SS0)));	//ustawieneie SS
		}


	//display "PLAY" untill any button == 1
	fb_floor[14] = 0xe8ea;                                    
	fb_floor[15] = 0xa8aa;                                    //
	fb_floor[16] = 0xe8e4;                                    //
	fb_floor[17] = 0x88a4;                                    //
	fb_floor[18] = 0x8ea4;                                    //
	updateframebuffer();                                      //
	while (!PC2_WC & !PC3_WC & !PC0_WC & !PC1_WC) {
	};           
	fb_init();
	//points = 0;
	nextblock = rand8();
	newblock();
	sei();
	while (~lock) {
		lock=1;
		if (tim_ms > ((500 - lvl)<<1)) {

			movedown();
			tim_ms = 0;
		}

		if (PC2_WC) {
			moveleft();
//			_delay_ms(100);
		}
		if (PC3_WC) {
			movedown();
//			_delay_ms(100);
		}
		if (PC0_WC) {
			moveright();
//			_delay_ms(100);
		}
		if (PC1_WC) {
			rotateRight();
//			_delay_ms(100);
		}

	}
	return (0);
}
void led_init() {
	//wlacz
	//jasnosc
	//wyczysc
	//lewy
	for (uint8_t i = 0; i < 4; i++) {	//wlacz
		PORTB = WYSWIETL_OBA
		;	//ustawieneie SS
		spiSendByte(MAX7219_MODE_POWER);
		spiSendByte(1);
		PORTB = (((1 << PIN_SS1)) | ((1 << PIN_SS0)));	//ustawieneie SS
	}
	_delay_us(300);
	for (uint8_t i = 0; i < 4; i++) {	//jasnosc
		PORTB = WYSWIETL_OBA
		;	//ustawieneie SS
		spiSendByte(MAX7219_MODE_INTENSITY);
		spiSendByte(1);
		PORTB = (((1 << PIN_SS1)) | ((1 << PIN_SS0)));	//ustawieneie SS
	}
	_delay_us(300);
	for (uint8_t i = 0; i < 4; i++) {	//dekoder
		PORTB = WYSWIETL_OBA
		;	//ustawieneie SS
		spiSendByte(MAX7219_MODE_DECODE);
		spiSendByte(0);
		PORTB = WYSWIETL_OBA
		;	//ustawieneie SS
	}
	_delay_us(300);
	for (uint8_t i = 0; i < 4; i++) {	//skan
		PORTB = WYSWIETL_OBA
		;	//ustawieneie SS
		spiSendByte(MAX7219_MODE_SCAN_LIMIT);
		spiSendByte(7);
		PORTB = (((1 << PIN_SS1)) | ((1 << PIN_SS0)));	//ustawieneie SS
	}
	for (uint8_t i = 0; i < 32; i++) {
		if (i % 2) {
			fb_main[i] = 0xffff;
		} else {
			fb_main[i] = 0;
		}
	}
	_delay_us(300);
	for (uint8_t i = 1; i < 9; ++i) {	//wyczysc
		PORTB = WYSWIETL_OBA
		;	//ustawieneie SS
		spiSendByte(i);
		spiSendByte(0);
		spiSendByte(i);
		spiSendByte(0);
		spiSendByte(i);
		spiSendByte(0);
		spiSendByte(i);
		spiSendByte(0);
		PORTB = (1 << PIN_SS1) | (1 << PIN_SS0);
	}

}
void led_send() {
	//lewe potem prawe
	//wyslij 4razy ten sam adres
	//wyslij 0,fb[24+i],0,fb[16+i],0,fb[8+i],0,fb[0+i]
	//lewy

	for (uint8_t i = 1; i < 9; ++i) {
		PORTB = WYSWIETL_LEWY;	//ustawieneie SS
		spiSendByte(i);
		spiSendByte(fb_main[(0 + i) - 1] >> 8);
		spiSendByte(i);
		spiSendByte(fb_main[7 + i] >> 8);
		spiSendByte(i);
		spiSendByte(fb_main[15 + i] >> 8);
		spiSendByte(i);
		spiSendByte(fb_main[23 + i] >> 8);
		PORTB = (1 << PIN_SS1) | (1 << PIN_SS0);
	}
	//prawy
	for (uint8_t i = 1; i < 9; ++i) {
		PORTB = WYSWIETL_PRAWY;	//ustawieneie SS
		spiSendByte(i);
		spiSendByte(fb_main[(0 + i) - 1]);
		spiSendByte(i);
		spiSendByte(fb_main[7 + i]);
		spiSendByte(i);
		spiSendByte(fb_main[15 + i]);
		spiSendByte(i);
		spiSendByte(fb_main[23 + i]);
		PORTB = (1 << PIN_SS1) | (1 << PIN_SS0);
	}

}
void spiSendByte(char databyte) {
	// Copy data into the SPI data register
	SPDR = databyte;
	// Wait until transfer is complete
	while (!(SPSR & (1 << SPIF)))
		;
}
void TIM0_Init() { //przerwanie co 1/2ms
	TCCR0A = (1 << WGM01);  //ustawiam tryb CTC
	TCCR0B = (1 << CS00) | (1 << CS02); //ustawiam preskaler na 1024
	OCR0A = 3;
	TIMSK0 = (1 << OCIE0A);
}
void updatepoints() {
	points++;

	if (points < 100)
		lvl = points * 3;

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
		fb_points001[1] = 0x00a0;
		fb_points001[2] = 0x00a0;
		fb_points001[3] = 0x00a0;
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
	for (int i = -2; i <= 2; ++i) {          //
		fb_blocks[kordy + i] = 0x0000;     
	}                                      //
	kordx = 8;
	kordy = 8;
	possition = 0;
	currentblock = nextblock;
	nextblock = rand8();

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
	for (uint8_t i = 9; i >= 8; i--) {
		if (fb_blocks[i] & fb_floor[i])
			gameover();
	}
}

void gameover() {                             
	tim_ms = 0;
	while (tim_ms < 500)
		;
	for (int j = 0; j < 5; j++) {
		for (int i = 0; i < 32; ++i) {
			fb_main[i] = 0xffff;
		}
		led_send();
		tim_ms = 0;
		while (tim_ms < 500)
			;
		for (int i = 0; i < 32; ++i) {
			fb_main[i] = 0x0000;
		}
		led_send();
		tim_ms = 0;
		while (tim_ms < 500)
			;
	}
	points--;
	for (int i = 0; i < 32; ++i) {
		fb_floor[i] = 0x0000;
		fb_blocks[i] = 0x0000;

	}
	fb_nextblock[0] = 0x0000;
	fb_nextblock[1] = 0x0000;

	updatepoints();
	for (;;) {
	}                            //wyświetlaj punkty do momentu kolejnego resetu
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
	for (int i = -2; i <= 2; ++i) {
		if (fb_blocks[kordy + i] & fb_floor[kordy + i] >> 1)
			return 0;
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
}
int spaceright() {
	for (int i = -2; i <= 2; ++i) {
		if (fb_blocks[kordy + i] & fb_floor[kordy + i] << 1)
			return 0;
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
}
void rotateRight() {

	if (currentblock) {
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
		c = 1;
		int g;
		for (int8_t i = -1; i < 2; i++) {
			g = fb_floor[kordy + i];
			for (int8_t j = -1; j < 2; j++) {
				if (newTable[i * 3 + j + 4]) {
					if (g & (1 << (kordx + j))) {
						c = 0;
					}
				}
			}
		}

		//Jeśli przejdzie to zamiana
		if (c) {
			for (int8_t i = -1; i < 2; i++) {
				g = 0;
				for (int8_t j = -1; j < 2; j++) {
					if (newTable[i * 3 + j + 4]) {
						g = g ^ (1 << (kordx + j));
					}
				}
				fb_blocks[kordy + i] = g;
			}
			updateframebuffer();
		}
	} else {
		//Sprawdź pozycję
		if (fb_blocks[kordy - 1] == (1 << kordx)) { //Obrócony pionowo
			//Walidacja pion -> poziom
			if (!((15 << (kordx - 2)) & fb_floor[kordy])) {
				fb_blocks[kordy - 1] = 0;
				fb_blocks[kordy] = 15 << (kordx - 2);
				fb_blocks[kordy + 1] = 0;
				fb_blocks[kordy + 2] = 0;
				updateframebuffer();
			}
		} else {  //Obrócony poziomo
			int8_t c = 1;
			//Walidacja poziom -> pion
			for (int8_t i = -1; i < 3; i++) {
				if (fb_floor[kordy + i] & (1 << kordx)) {
					c = 0;
				}
			}
			//FLaga ustawiona
			if (c) {
				for (int8_t i = -1; i < 3; i++) {
					fb_blocks[kordy + i] = 1 << kordx;
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
ISR(TIMER0_COMPA_vect) {
	tim_ms++;
	lock=0;
}
