#include "rand8.h"  // tutaj trzeba podać ścieżkę do rand8 
#include <avr/io.h>		// IO
//#include <inttypes.h>	// uint
#include <stdlib.h>     // srand, rand

void init_adc(){
	ADMUX |= (1<<REFS0);
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADEN); // prescaler = 64,  8MHz/64 = 125kHz

}

uint16_t read_adc(uint8_t adc_channel){
	ADMUX = (1<<REFS0)|(adc_channel & 0x0F);	
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	return ADC;
}

uint8_t rand8(void){
	uint8_t seed_val = 0;
	uint8_t rand_val = 0;
	init_adc();

	for(uint8_t i=0; i<8; i++){
		seed_val = seed_val<<1 | (read_adc(0)&0b1);
	}

	srand(seed_val);
	rand_val = rand()%7;

	return rand_val;
}
