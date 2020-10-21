#include "Pokitto.h"
#include "USBSerial.h"
#include "MCP23S17.h"
#include "SPISlave.h"

//#include "setup.c" // See defines, variables, constants, functions here


Pokitto::Core mygame;

USBSerial pc;
SPI spi(P1_22, P1_21, P1_20, P1_23);
MCP23S17 chip = MCP23S17(spi, P1_23, 0x40);
MCP23S17 chip2 = MCP23S17(spi, P1_23, 0x42);


int main () {
    chip2.direction(PORT_A, 0xFF);

    uint32_t address = 0;
	uint8_t eepromSize = 1;
	uint8_t resetCommonLines = 1;
    mygame.begin();
    while (mygame.isRunning()) {

        if (mygame.update()) {
            mygame.display.print("Hello World!");
            pc.printf("Pokitto ready\r\n");
            char a=chip2.read(PORT_A);
            pc.printf("%d",a);

           }
        }

}
