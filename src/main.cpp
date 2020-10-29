#include "Pokitto.h"
#include "USBSerial.h"
#include "MCP23S17.h"
#include "SPISlave.h"

//#include "setup.c" // See defines, variables, constants, functions here


Pokitto::Core mygame;

USBSerial pc;
SPI spi(P1_22, P1_21, P1_20, P1_23);
MCP23S17 chip1 = MCP23S17(spi, P1_23, 0x40);
MCP23S17 chip2 = MCP23S17(spi, P1_23, 0x42);
// CLK      Chip2 GPB0
// WR       Chip2 GPB1
// RD       Chip2 GPB2
// CS/MREQ  Chip2 GPB3
// RESET    Chip2 GPB4

// D0-D7    Chip2 GPA0-GPA7
// A0-A15   Chip1 GPA0-GPB7
byte PortGB;
void wrPin_high () { PortGB |= (1<<1);chip2.write(PORT_B, PortGB);}
void wrPin_low  () { PortGB &= ~(1<<1);    chip2.write(PORT_B, PortGB);}
void clkPin_high () { PortGB |= (1<<0);chip2.write(PORT_B, PortGB);}
void clkPin_low  () { PortGB &= ~(1<<0);    chip2.write(PORT_B, PortGB);}
void resetPin_high () { PortGB |= (1<<4);chip2.write(PORT_B, PortGB);}
void resetPin_low  () { PortGB &= ~(1<<4);    chip2.write(PORT_B, PortGB);}

void mreqPin_high () { PortGB |= (1<<3);    chip2.write(PORT_B, PortGB);}
void mreqPin_low  () { PortGB &= ~(1<<3);    chip2.write(PORT_B, PortGB);}
void rdPin_high () { PortGB |= (1<<2);    chip2.write(PORT_B, PortGB);}
void rdPin_low  () { PortGB &= ~(1<<2);    chip2.write(PORT_B, PortGB);}

char gameTitle[17];


void rd_wr_mreq_reset () {
  rdPin_high(); // RD off
  wrPin_high(); // WR off
  mreqPin_high(); // MREQ off
}
uint8_t read_byte(uint16_t address) {
  //shiftout_address(address); // Shift out address
    chip1.write(PORT_B, address >> 8);
    chip1.write(PORT_A, address & 0xFF);
//  mreqPin_low();
  rdPin_low();
 wait_us(10);
  uint8_t bval = chip2.read(PORT_A); // Read data
  rdPin_high();
 // mreqPin_high();
  // wait_us(10);

  return bval;
}
void read_header () {
            pc.printf("GB reset\r\n");

    rd_wr_mreq_reset();
                pc.printf("GB Read\r\n");

    // Read cartridge title and check for non-printable text
    for (uint16_t romAddress = 0x0134; romAddress <= 0x143; romAddress++) {
      char headerChar = (char) read_byte(romAddress);
                      //mygame.display.print(headerChar);
      if ((headerChar >= 0x30 && headerChar <= 0x57) || // 0-9
          (headerChar >= 0x41 && headerChar <= 0x5A) || // A-Z
          (headerChar >= 0x61 && headerChar <= 0x7A)) { // a-z
            gameTitle[(romAddress-0x0134)] = headerChar;
      }
    }
    gameTitle[16] = '\0';
    pc.printf(gameTitle);
    mygame.display.print(gameTitle);

}

int main () {
    chip1.direction(PORT_A, 0x00);//OUTPUT
    chip1.direction(PORT_B, 0x00);
    chip2.direction(PORT_A, 0xFF);//INPUT
    chip2.direction(PORT_B, 0x00);
    PortGB= 0b00000000;
    chip2.write(PORT_B, PortGB);
    resetPin_high();
    clkPin_low();
    mygame.begin();
    while (mygame.isRunning()) {

        if (mygame.update()) {
            mygame.display.print("Hello World!");
            mygame.display.print(mygame.getTime());
            pc.printf("Pokitto ready\r\n");
           
           read_header();

wait(3);
           }
        }

}
