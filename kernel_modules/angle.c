#include <asm/io.h>
#include "angle.h"

void init_bta(int isaaddress){
  outb(9+128,isaaddress + 3);  /* A,B, output, C input */
  outb(27+128,isaaddress + 7); /* A,B,C, input */
}

int read_bta(int isaaddress){
  unsigned short dig1, dig2, dig3, dig4;
  int dig;

  dig1 = inb(isaaddress + 2);
  dig2 = inb(isaaddress + 4);
  dig3 = inb(isaaddress + 5);
  dig4 = inb(isaaddress + 6);
  dig4 = dig4 & 1; 

  dig = dig4 * 256*256*256 + dig3 *256*256 + dig2*256 + dig1;

 return dig;
}

void write_bta(int isaaddress, int value){
  /* unsigned int dig1, dig2, dig3, dig4, dig; */
  int dig1, dig2, dig3, dig4, dig;

  /* display only goes to 1999 */
  if (abs(value) >= 2000){
     return;
  }

  dig1 = abs(value) % 10;
  dig2 = ((abs(value) % 100) - dig1)/10;
  dig3 = ((abs(value) % 1000) - dig2*10 - dig1)/100;

  if (value >=0){
    if (value >=1000){
       dig4 = 0;
    } else {
       dig4 = 4;
    }
  } else {
    if (value <=-1000){
       dig4 = 3;
    } else {
       dig4 = 1;
    }
  }

  dig  = dig1 + (dig2<<4);
  outb(dig,isaaddress + 0);
  dig  = dig3 + (dig4<<4);
  outb(dig,isaaddress + 1);
  return;
}
