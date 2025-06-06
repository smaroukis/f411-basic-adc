/*
 * tm1637.c
 *
 *  Created on: May 23, 2025
 *      Author: s
 */


#include "tm1637.h"
#include "main.h" // for pin definitions
#include "stm32_nop_delay.h"
#include <assert.h>

#define CMD_DATA_AUTO_INC 	0x40u 			// 01xx 00xx
#define CMD_DATA_NO_AUTO 	0x44u 			// 01xx 01xx
#define CMD_ADDR         	0xC0u     		/* 11xx aaaa */
#define TM1637_ADDR(a)      (CMD_ADDR | ((a) & 0x3Fu))

#define   CMD_DISPLAY      	0x80u     		/* 10xx pbbb */
#define   DISP_ON         	(1u << 3) 		/* p = 1 → ON */
#define   DISP_OFF        	(0u << 3)
#define   BRIGHT(n)       	((n) & 0x07u)


// Forward Declarations
void CLK_HIGH(void);
void CLK_LOW(void);
void DATA_SET(uint8_t b);
void DATA_HIGH(void);
void DATA_LOW(void);

// TODO/later setup a hardware timer to pass to delay_us instead of using HAL_Delay
//static void delay_us(uint32_t us)
//{
//    uint32_t start = __HAL_TIM_GET_COUNTER(&htimX);  // Use a configured 1MHz timer
//    while ((__HAL_TIM_GET_COUNTER(&htimX) - start) < us);
//}

//
// ┌──────────────────────────┐
// │  __    __      __    __  │<- CLK
// │ |  |  |  | -  |  |  |  | │<- DIO
// │ |__|  |__| -  |__|  |__| │<- GND
// │  0     1        2     3  │<- VCC
// └──────────────────────────┘
//	    Digit index (0 = left)
//	Colon belongs to Digit 1 MSB

//
//      A
//     ---
//  F |   | B
//     -G-
//  E |   | C
//     ---
//      D    DP
const uint8_t digit_to_segment[10] = {
    0b00111111,  // 0: segments a,b,c,d,e,f 	0x3f
    0b00000110,  // 1: segments b,c				0x06
    0b01011011,  // 2: a,b,d,e,g				0x5b
    0b01001111,  // 3: a,b,c,d,g				0x4f
    0b01100110,  // 4: b,c,f,g					0x66
    0b01101101,  // 5: a,c,d,f,g				0x6D
    0b01111101,  // 6: a,c,d,e,f,g				0x7D
    0b00000111,  // 7: a,b,c					0x07
    0b01111111,  // 8: a,b,c,d,e,f,g			0x7F
    0b01101111   // 9: a,b,c,d,f,g				0x6F
};

static const uint8_t TM1637_DP_BIT = 0b10000000;

static const uint8_t DISPLAY_ERR[4] = { 0x0, 0b01111001, 0b01010000 , 0b01010000 };

/* Aim for ≈10 µs full bit => 5 µs half-bit */
/* Results: Measured at ~30us full bit */
static inline void tm_delay_halfbit(void)
{
    /* 5 µs * 84 MHz = 420 cycles ; 420 / 3 ≈ 140 loop-iters */
	// Above needs to be checked with clock tree //
    delay_cycles(140);
}

void tm1637_delay(void)
{
    tm_delay_halfbit();
}

void tm1637_init(void)
{
    TM_DIO_OUTPUT();
    // Idle HIGH, no pullups needed, handled by switching GPIO state
    CLK_HIGH();
    DATA_HIGH();
}


// Start condition: with CLK = HIGH, DIO HIGH->LOW signals START
void tm1637_start(void)
{
    TM_DIO_OUTPUT();
    CLK_HIGH();
    DATA_HIGH();
    tm1637_delay();
    DATA_LOW();
    tm1637_delay();
}

// Stop condition: With CLK = HIGH, DIO LOW->HIGH (After ACK/NACK_)
void tm1637_stop(void)
{
    TM_DIO_OUTPUT();
    CLK_LOW();
    DATA_LOW();
    tm1637_delay();
    CLK_HIGH();
    tm1637_delay();
    DATA_HIGH();
    tm1637_delay();
}


// 3 - Send One Byte with ACK Handling
// TODO change to static once done debugging
// Note we write LSb first so 0b1111 0000 looks like 0000_1111 in time
void tm1637_write_byte(uint8_t b)
{
    TM_DIO_OUTPUT();

    for (int i = 0; i < 8; i++) {
        CLK_LOW();
        DATA_SET(b & 0x01);
        tm1637_delay();
        CLK_HIGH();
        tm1637_delay();
        b >>= 1;
    }

    // Wait for ACK
    // Assume: Master handles clock while releasing DIO line for Secondary to respond
    CLK_LOW();
    //DATA_HIGH(); // release bus, see if we get an ACK
    TM_DIO_INPUT();
    // Datasheet says delay 5us after falling edge of clock (vs 2us typ.)
    tm1637_delay();
    // heres where we would read the ACK
    CLK_HIGH(); //  data valid during HIGH, so we can read
    tm1637_delay();

    tm1637_delay();
}

// Wrapper for CLK = 1
void CLK_HIGH(void) {
	HAL_GPIO_WritePin(TM_CLK_GPIO_Port, TM_CLK_Pin, GPIO_PIN_SET);
}

void CLK_LOW(void) {
	HAL_GPIO_WritePin(TM_CLK_GPIO_Port, TM_CLK_Pin, GPIO_PIN_RESET);
}

void DATA_SET(uint8_t b) {
	HAL_GPIO_WritePin(TM_SDA_GPIO_Port, TM_SDA_Pin, b ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void DATA_HIGH(void) {
	HAL_GPIO_WritePin(TM_SDA_GPIO_Port, TM_SDA_Pin, GPIO_PIN_SET);
}

void DATA_LOW(void) {
	HAL_GPIO_WritePin(TM_SDA_GPIO_Port, TM_SDA_Pin, GPIO_PIN_RESET);
}

// ================== Public Functions ===============================

// Write a multi byte packet to multiple Digits
// buff[0] will be written to digit 0
// e.g. {0xAA, 0xBB, 0xCC, 0xDD} -> means 0xAA will be written to digit[0] etc
// N.B.: Dumb and fast, doesn't blank other digits outside of start_addr to len-1
// TODO debug for non zero start_addr
void tm1637_write_packet(uint8_t *buff, size_t len, uint8_t start_addr) {
	uint8_t *data = buff;

	if (start_addr >= TM1637_DIGIT_COUNT) {
		// TODO handle better
		assert(0);
	}

	if (len == 0) {
		return;
	}

	if (start_addr + len > TM1637_DIGIT_COUNT) {
		// clamp to remaining digits
		len = TM1637_DIGIT_COUNT - start_addr;
	}

	tm1637_start();
	tm1637_write_byte(CMD_DATA_AUTO_INC);
	tm1637_stop();

	// Address Command
	tm1637_start();
	tm1637_write_byte(TM1637_ADDR(start_addr)); // Byte addressed, one byte wide. E.g. digit 0 at 0x0, digit 1 at 0x01, etc

	// Address is auto incrementing
	for (int i = 0; i < len; i++) {
		tm1637_write_byte(*data++);
	}

	tm1637_stop();


	tm1637_delay();
	tm1637_delay();

	// Flush/Display Control
	tm1637_start();
	tm1637_write_byte(CMD_DISPLAY | DISP_ON | BRIGHT(7) ); //Display Control +  max brightness from manual 1000 1111
	tm1637_stop();
}

// Given a buffer and its length and a starting index (0-indexed) create the packet of encoded bytes
// while filling all positions before start_pos with zeros
// ** Note this WONT reverse the array, so e.g. { 1, 2, 3} start_pos = 1 will look like { 3 2 1 x} on the display
// Example:
// Input: data = {1, 2, 3} | start_pos = 1
// Outputs: full = {x, 1, 2, 3} where x is 0x0 and others are encoded vals
void tm1637_fill_with_blanks(const uint8_t *data, uint8_t len, uint8_t start_pos) {
	uint8_t full[TM1637_DIGIT_COUNT] = { 0 };

	for (uint8_t i = 0; i < len; i++) {
		if (start_pos + i >= TM1637_DIGIT_COUNT) {
			assert(0); // TODO-handle this
		}
		// populate the rest with the data
		full[start_pos + i] = data[i];
	}
	tm1637_write_packet(full, sizeof(full) / sizeof(full[0]), 0); // start at 0 to fill with blanks as necessary
}


// Display an integer between 0-9999 starting at 0-indexed digit position
// See header for digit indices descriptions
void tm1637_displayNumber(uint16_t number, uint8_t pos) {
	// Out of bounds flashes the screen
	if (number > 9999 || number < 0) {
		tm1637_displayError();
		HAL_Delay(200);
		tm1637_unset_all();
		HAL_Delay(200);
		tm1637_displayError();
		HAL_Delay(200);
		tm1637_unset_all();
		return;
	}


	uint8_t i = 0;
	uint8_t encodedDigits[TM1637_DIGIT_COUNT] = {0};

	if (number == 0) {
		// Handle singular case
		// Override position
		tm1637_fill_with_blanks((uint8_t[]){ 0 }, 1, 0);
		return;
	}

	// Create encoded buffer and fill with blanks to write
	// NOTE we need to reverse the array since we are using modulo for LSD
	do {
		encodedDigits[i++] = digit_to_segment[number % 10];
		number /= 10;
	} while (number > 0 && i < TM1637_DIGIT_COUNT);

	// Reverse the array
	if (i > 1) {
		for (uint8_t head = 0; head < i/2; head++) {
			uint8_t tail = i - 1 - head;
			uint8_t tmp = encodedDigits[tail];
			encodedDigits[tail] = encodedDigits[head];
			encodedDigits[head] = tmp;
		}
	}

	tm1637_fill_with_blanks(encodedDigits, i, pos);
}

// Displays floating point with decimal
// HACK -- using colon (digit 1 MSB) as decimal
// TODO still not working for <1.00 e.g. 0.34
void tm1637_displayDecimal(float number, uint8_t pos) {

	// Check bounds, since decimal is in the middle, we can only go to 99.99
	if (number > 99.99 || number < 0) {
		tm1637_displayError();
        return;
	}

	// Seperate Ones, Tens, Hundred, and Tenths, Hundredths
	uint32_t intPart = (uint32_t)number;
	float fracPart  = number - (float)intPart;

	// Get number of non-zero integer digits e.g. 99.99 -> 2, 9.99 -> 1, 0.99 -> 0
    uint8_t numIntDigits = 0;
    if (intPart == 0) {
        // still show "0" on the display
        numIntDigits = 1;
    }
    if (intPart > 0) {
        uint32_t tmp = intPart;
        numIntDigits = 1;
            while ( ( (tmp % 10 )  > 0 ) && ( ( tmp / 10 ) > 0) ) {
                tmp /= 10;
                numIntDigits++;
            }
    }

	// Build non encoded array handling decimal
    // Note this will take off digits starting from the least significant / rightmost
    // So we need to reverse afterwards
	uint8_t i = 0;
	uint8_t encodedDigits[TM1637_DIGIT_COUNT] = {0};
    uint32_t scaled = (uint32_t)(number * 100.0f + 0.5f); // two decimal places max for this hardware, bump up before cast for rounding
	do {
		encodedDigits[i++] = digit_to_segment[scaled % 10];
		scaled /= 10;

	} while (scaled > 0 && i < TM1637_DIGIT_COUNT);
    if (intPart == 0) { // if <1.0f need to put a 0 at the end before reversal
        encodedDigits[i] = digit_to_segment[0];
        i++;
    }

	// Reverse the array, note will left align to [0], leaving blanks on the right
    // Don't forget to offset when writing the bytes
	if (i > 1) {
		for (uint8_t head = 0; head < i/2; head++) {
			uint8_t tail = i - 1 - head;
			uint8_t tmp = encodedDigits[tail];
			encodedDigits[tail] = encodedDigits[head];
			encodedDigits[head] = tmp;
		}
	}

    // Shift by the offset if integer digits < 2
    uint8_t offset = 0;
    if (numIntDigits < 2){
        offset = TM1637_DIGIT_COUNT - i; // i should be number of digits total
        for (int pos = i - 1; pos >= 0; pos--) {
            encodedDigits[offset + pos] = encodedDigits[pos]; // e.g. copy nth to nth+offset
            encodedDigits[pos] = 0; // replace the copy from source
        }
    }

	// Put back in the decimal, accounting for offset
    if (numIntDigits > 0 && numIntDigits <= TM1637_DIGIT_COUNT) {
        encodedDigits[offset + numIntDigits - 1] |= TM1637_DP_BIT;
    }


	// Send Encoded array for writing
	tm1637_fill_with_blanks(encodedDigits, TM1637_DIGIT_COUNT, pos);

}

// 4 digit set all function
// LSB first
// Address is 0x00 to 0x05, 8-bits wide
// Segments are Columns (Left=SEG1->Right=SEG8), Grids/Digits are rows (top=GRID1->bottom=GRID6)
void tm1637_set_all(void){

	// Data Command
	tm1637_start();
	tm1637_write_byte(CMD_DATA_AUTO_INC);
	tm1637_stop();

	// Address Command
	tm1637_start();
	tm1637_write_byte(TM1637_ADDR(0)); // Write Address From 0x0; 0b1100 0000
	for (int i = 0; i < TM1637_DIGIT_COUNT; i++) {
		tm1637_write_byte(0xff);
	}
	tm1637_stop();


	tm1637_delay();
	tm1637_delay();

	// Flush/Display Control
	tm1637_start();
	tm1637_write_byte(CMD_DISPLAY | DISP_ON | BRIGHT(7) ); //Display Control +  max brightness from manual 1000 1111
	tm1637_stop();

}

void tm1637_unset_all(void) {
	// Data Command
	tm1637_start();
	tm1637_write_byte(CMD_DATA_AUTO_INC);
	tm1637_stop();

	// Address Command
	tm1637_start();
	tm1637_write_byte(TM1637_ADDR(0x0)); // Write Address From 0x0; 0b1100 0000
	for (int i = 0; i < TM1637_DIGIT_COUNT; i++) {
		tm1637_write_byte(0x00);
	}
	tm1637_stop();

	tm1637_delay();
	tm1637_delay();
}

void tm1637_displayError(void) {
	tm1637_write_packet(DISPLAY_ERR, TM1637_DIGIT_COUNT, 0);
	HAL_Delay(500);
	tm1637_unset_all();
	HAL_Delay(500);
	tm1637_write_packet(DISPLAY_ERR, TM1637_DIGIT_COUNT, 0);
	tm1637_unset_all();
}


// =============== Decoder Helpers ==========================

// Core encoder: take 0-9 plus "dot" boolean flag
static inline uint8_t encode_raw(uint8_t digit, uint8_t dot) {
	if (digit > 9) {
		return 0;
	}

	uint8_t seg = digit_to_segment[digit];

	return dot ? seg | TM1637_DP_BIT : seg;
}

static inline uint8_t encode_digit(uint8_t d) {
	return encode_raw(d, 0);
}

static inline uint8_t encode_digit_dp(uint8_t d) {
	return encode_raw(d, 1);
}


// Build an encoded  buffer from raw integers
void encoded_buf_from_int_buf(const uint8_t *digits, const uint8_t *use_dp, uint8_t *buf, uint8_t len) {

	for (uint8_t i = 0; i < len; i++) {
		buf[i] = use_dp[i] ? encode_digit_dp(digits[i]) : encode_digit(digits[i]);
	}

}



// =============== Looping Helpers ==========================
void tm1637_loopAllSegs(void) {
	// Assert Brightness
	tm1637_start();
	tm1637_write_byte(CMD_DISPLAY | DISP_ON | BRIGHT(7) );
	tm1637_stop();

	// Light up each segment one by one to check where e.g. address of dots
	tm1637_start();
	tm1637_write_byte(CMD_DATA_NO_AUTO);
	tm1637_stop();

	for (uint8_t a = 0; a < 4; a++) {

		for (uint8_t i = 0; i < 8; i++) {

			tm1637_start();
			tm1637_write_byte(TM1637_ADDR(a));

			tm1637_write_byte(1U << i);

			tm1637_stop();

			HAL_Delay(100);

			if (i == 7) {
				tm1637_start();
				tm1637_write_byte(TM1637_ADDR(a));
				tm1637_write_byte(0x00);
				tm1637_stop();
			}
		}

	}

}

void tm1637_loopDigNSegs(uint8_t n) {
	// Assert Brightness
	tm1637_start();
	tm1637_write_byte(CMD_DISPLAY | DISP_ON | BRIGHT(7) );
	tm1637_stop();

	// Light up each segment one by one to check where e.g. address of dots
	tm1637_start();
	tm1637_write_byte(CMD_DATA_NO_AUTO);
	tm1637_stop();

	for (uint8_t i = 0; i < 8; i++) {

		tm1637_start();
		tm1637_write_byte(TM1637_ADDR(n));

		tm1637_write_byte(1U << i);

		tm1637_stop();
		HAL_Delay(200);

		if (i == 7) {
			tm1637_start();
			tm1637_write_byte(TM1637_ADDR(n));
			tm1637_write_byte(0x00);
			tm1637_stop();
		}
	}

}


void tm1637_loop9999(void) {

	uint16_t i = 0;
	do {
	 tm1637_displayNumber(i, 0);
	 HAL_Delay(25);
	 i++;
	} while(i < 10000u);

}
