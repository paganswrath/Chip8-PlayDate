#include <stdbool.h>

#include "pd_api.h"

PlaydateAPI* pd = NULL;

#include <sys/time.h>

int _gettimeofday( struct timeval *tv, void *tzvp )
{
    uint64_t t = time(NULL);  // get uptime in nanoseconds
    tv->tv_sec = t / 1000000000;  // convert to seconds
    tv->tv_usec = ( t % 1000000000 ) / 1000;  // get remaining microseconds
    return 0;  // return non-zero for error
}

#define F 15

bool RomLoaded = false;

static unsigned char ScreenMemory[64][32];

static unsigned char SystemMemory[4096];
static unsigned char SystemRegisters[16];
static unsigned short SystemStack[16];
static unsigned short SystemIRegister = 0;
static unsigned short SystemProgramCounter = 0x200;
static unsigned short SystemStackPointer = 0;

static unsigned char SystemDelayTimer = 0 ;
static unsigned char SystemSoundTimer = 0;
static unsigned char SystemKey = 0x0006;



void RunCycle(){

	if (SystemDelayTimer > 0) {
		--(SystemDelayTimer);
		
	}
	if (SystemSoundTimer > 0) {
		--(SystemSoundTimer);
		if (SystemSoundTimer == 0) {
            //printf("BEEP!\n");
        }
	}

    bool JumpFlag = false;
    int i, j;
	unsigned short Opcode;
    unsigned short JumpAddress;
	unsigned char X, Y, NN;
	Opcode = SystemMemory[SystemProgramCounter] << 8 | SystemMemory[SystemProgramCounter + 1];

	JumpAddress = Opcode & 0x0FFF;
	X = (Opcode & 0x0F00) >> 8;
	Y = (Opcode & 0x00F0) >> 4;
	NN = Opcode & 0x00FF;

    switch (Opcode & 0xF000) {
		case 0x0000:
			if (Opcode == 0x00E0) {
				for (int x = 0 ; x <= 64; x ++){
					for (int y = 0 ; y <= 32; y ++){
						ScreenMemory[x][y] = 0;
					}
				}
			}
			else if (Opcode == 0x00EE) {
				SystemProgramCounter = SystemStack[SystemStackPointer];
				SystemStack[SystemStackPointer] = 0;
				if (SystemStackPointer > 0) {
					--(SystemStackPointer);
				}
			}
			break;
		case 0x1000:
			SystemProgramCounter = JumpAddress;
			JumpFlag = true;
			break;
		case 0x2000:
			if (SystemStack[SystemStackPointer] != 0) {
				++(SystemStackPointer);
			}
			SystemStack[SystemStackPointer] = SystemProgramCounter;
			SystemProgramCounter = JumpAddress;
			JumpFlag =true;
			break;
		case 0x3000:
			if (SystemStack[X] == NN) { SystemProgramCounter += 2; }
			break;
		case 0x4000:
			if (SystemStack[X] != NN) { SystemProgramCounter += 2; }
			break;
		case 0x5000:
			if (SystemStack[X] == SystemStack[Y]) { SystemProgramCounter += 2; }
			break;
		case 0x6000:
			SystemStack[X] = NN;
			break;
		case 0x7000:
			SystemStack[X] += NN;
			break;
		case 0x8000:
			switch (Opcode & 0x000F) {
				case 0x0000:
					SystemStack[X] = SystemStack[Y];
					break;
				case 0x0001:
					SystemStack[X] = SystemStack[X] | SystemStack[Y];
					break;
				case 0x0002:
					SystemStack[X] = SystemStack[X] & SystemStack[Y];
					break;
				case 0x0003:
					SystemStack[X] = SystemStack[X] ^ SystemStack[Y];
					break;
				case 0x0004:
					if (SystemStack[X] + SystemStack[Y] > 255) {
						SystemStack[F] = 1;
					} else {
						SystemStack[F] = 0;
					}
					SystemStack[X] += SystemStack[Y];
					break;
				case 0x0005:
					if (SystemStack[X] < SystemStack[Y]) {
						SystemStack[F] = 0;
					} else {
						SystemStack[F] = 1;
					}
					SystemStack[X] -= SystemStack[Y];
					break;
				case 0x0006:
					if (SystemStack[X] % 2 == 0) {
						SystemStack[F] = 0;
					} else {
						SystemStack[F] = 1;
					}
					SystemStack[X] = SystemStack[X] >> 1;
					break;
				case 0x0007:
					if (SystemStack[Y] < SystemStack[X]) {
						SystemStack[F] = 0;
					}
					else {
						SystemStack[F] = 1;
					}
					SystemStack[X] = SystemStack[Y] - SystemStack[X];
					break;
				case 0x000E:
					if (SystemStack[X] < 128) {
						SystemStack[F] = 0;
					} else {
						SystemStack[F] = 1;
					}
					SystemStack[X] = SystemStack[X] << 1;
					break;
			}
			break;
		case 0x9000:
			if (SystemStack[X] != SystemStack[Y]) { SystemProgramCounter += 2; }
			break;
		case 0xA000:
			SystemIRegister = Opcode & 0x0FFF;
			break;
		case 0xB000:
			SystemProgramCounter = JumpAddress + SystemStack[0];
			JumpFlag =true;
			break;
		case 0xC000:
			SystemStack[X] = (rand() % 255) & (Opcode & 0x00FF);
			break;
		case 0xD000: {
			int height = Opcode & 0x000F;
			int x_coord = SystemStack[X];
			int y_coord = SystemStack[Y];
			int ands[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };

			SystemStack[F] = 0;
			for (i = 0; i < height; i++) {
				for (j = 0; j < 8; j++) {
					if (x_coord + j == 64) {
						x_coord = -j;
					}
					if (y_coord + i == 32) {
						y_coord = -i;
					}
					if (ScreenMemory[x_coord + j][y_coord + i] == 1 &&
						((SystemMemory[SystemIRegister + i] & ands[j]) >> (8 - j - 1)) == 1) {
						SystemStack[F] = 1;
					}
					ScreenMemory[x_coord + j][y_coord + i] = ScreenMemory[x_coord + j][y_coord + i] ^ 
						((SystemMemory[SystemIRegister + i] & ands[j]) >> (8 - j - 1));
				}
				x_coord = SystemStack[X];
				y_coord = SystemStack[Y];
			}
			break;
		}
		case 0xE000:
			if (NN == 0x009E) {
				if (SystemKey == SystemStack[X]) {
					SystemProgramCounter += 2;
				}
			} else if (NN == 0x00A1) {
				if (SystemKey != SystemStack[X]) {
					SystemProgramCounter += 2;
				}
			} 
			break;
		case 0xF000:
			switch (Opcode & 0x00FF) {
				case 0x0007:
					SystemStack[X] = SystemDelayTimer;
					break;
				case 0x000A:
				if (SystemKey == 0) {
						return;
					} else {
						SystemStack[X] = SystemKey;
					}
					break;
				case 0x0015:
					SystemDelayTimer = SystemStack[X];
					break;
				case 0x0018:
					SystemSoundTimer = SystemStack[X];
					break;
				case 0x001E:
					SystemIRegister += SystemStack[X];
					break;
				case 0x0029:
					SystemIRegister = SystemStack[X] * 5;
					break;
				case 0x0033:
					SystemMemory[SystemIRegister] = SystemStack[X] / 100;
					SystemMemory[SystemIRegister + 1] = (SystemStack[X] / 10) % 10;
					SystemMemory[SystemIRegister + 2] = SystemStack[X] % 10;
					break;
				case 0x0055:
					for (i = 0; i < X + 1; i++) {
						SystemMemory[SystemIRegister + i] = SystemStack[i];
					}
					SystemIRegister = SystemIRegister + X + 1;
					break;
				case 0x0065:
					for (i = 0; i < X + 1; i++) {
						SystemStack[i] = SystemMemory[SystemIRegister + i];
					}
					SystemIRegister = SystemIRegister + X + 1;
					break;
			}
		break;
    }
    if (!JumpFlag) { SystemProgramCounter += 2; }

}