/*
This code is only intended as a sketch that can be used to control 
the LCD driver ref: uPD16432
It's not clean, nor optimized, the idea being to leave it as basic 
as possible for an easy interpretation of what it sends to the driver!
*/

#define LCD_test 11
#define LCD_inh 12  //Can force it a High value with a resisitor of 4,7K to +5VDC
#define LCD_in 8
#define LCD_clk 9
#define LCD_stb 10

byte data[14];  

void cmd_with_ce(unsigned char a){
// send with stb 
//Note: Wait time tWAIT (1 µs) is necessary from the rise of the 8th shift 
//clock of command 1until the fall of the 1st shift clock of data 1
//Note which to the uPD16432 I'm send the data at reverse order!!! Going first the MSB
  unsigned char data = 0x00; //value to transmit
  unsigned char mask = 1; //our bitmask
  data=a;
  digitalWrite(LCD_stb, LOW);
  delayMicroseconds(2);
  //This send the strobe signal
  //Note: The first byte input at DataIN after the STB go HIGH is interpreted as a command!!!
  digitalWrite(LCD_stb, HIGH);
  delayMicroseconds(2);
         for (mask = 10000000; mask>0; mask >>= 1) { //iterate through bit mask
           digitalWrite(LCD_clk, LOW);
           delayMicroseconds(1);
                   if (data & mask){ // if bitwise AND resolves to true
                      digitalWrite(LCD_in, HIGH);
                   }
                   else{ //if bitwise and resolves to false
                     digitalWrite(LCD_in, LOW);
                   }
          digitalWrite(LCD_clk, HIGH);
          delayMicroseconds(2);
         }
}
void cmd_without_ce(unsigned char a){
// send without stb
//Note: Wait time tWAIT (1 µs) is necessary from the rise of the 8th shift 
//clock of command 1until the fall of the 1st shift clock of data 1
  //Note which to the uPD16432 I'm send the data at reverse order!!! Going first the MSB
  unsigned char data = 0x00; //value to transmit
  unsigned char mask = 1; //our bitmask
  data=a;
  //This don't send the strobe signal, to be used in burst data send
     for (mask = 10000000; mask>0; mask >>= 1) { //iterate through bit mask
       digitalWrite(LCD_clk, LOW);
           if (data & mask){ // if bitwise AND resolves to true
              digitalWrite(LCD_in, HIGH);
           }
           else{ //if bitwise and resolves to false
             digitalWrite(LCD_in, LOW);
           }
      digitalWrite(LCD_clk, HIGH);
      delayMicroseconds(2); 
     }
}
void clear_LCD_ADRAM(void){
  cmd_with_ce(0b01001000); //Sets the display RAM & Increment or Fixed address by bit 4
  delayMicroseconds(4);
      for (uint8_t n=0x00; n < 0x19; n++){  // 
           cmd_with_ce((0b10000000) | n); 
           cmd_without_ce(0b00100000); // Character you want, on this sample I sent the SPACE(20)to clear the LCD
      }
  cmd_with_ce(0b11000110); //Status command   
}
void uPD16432_print(unsigned char address, unsigned char *text){
  unsigned char c;
  cmd_with_ce(0b01001000); //Sets the display RAM & Increment or Fixed address by bit 4
  delayMicroseconds(4);
   while ( c = (*text++) ){
          cmd_with_ce((0b10000000) | (address & 0x1F)); //((0b00010000)) + (address & 0x0F) );
          cmd_without_ce(c); //
          address++;
        }   
  delayMicroseconds(2);
  digitalWrite(LCD_stb, LOW); // This line finalize the DATA.
}
void led(uint16_t num, uint8_t gap){
  for(uint16_t cyc=0; cyc < num; cyc++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(gap);
    digitalWrite(LED_BUILTIN, LOW);
    delay(gap);
  }
}
void setup() {
// put your setup code here, to run once:
// initialize digital pin LED_BUILTIN as an output.
 pinMode(LED_BUILTIN, OUTPUT); // This is the Arduino LED and is only to use as HELP to DEBUG!
 pinMode(8, OUTPUT); //DATA IN
 pinMode(9, OUTPUT); //CLOCK
 pinMode(10, OUTPUT);//Chip Enable(ce) or Strobe(stb)
 pinMode(11, OUTPUT); //This pin is only to change the color of Panel to GREEN or AMBAR.
 Serial.begin(115200);
 
delayMicroseconds(300); //Delay to estabilize the IC

}
void loop(){ 
//cmd_with_ce(0b00000101); //cmd1 This command initializes the µPD16432B, and sets the duty, number of segments, number of commons, master/slave operation, and the drive voltage supply method.
//                                                                 Drive voltage supply method selection     Master/slave setting    Duty setting
//                                                                 0: External                               0: Master               0: 1/8 duty (SEG61/COM14 to SEG65/COM10 → segment outputs)
//                                                                 1: Interna                                1: Slave                1: 1/15 duty (SEG61/COM14 to SEG65/COM10 → common outputs
  
delayMicroseconds(2);
cmd_with_ce(0b11000111); //cmd2 Sets the data write mode, read mode, and address increment mode.
//Test mode setting       //Standby mode setting      //Key scan control              //LED control              //     LCD cotrol
//0: Normal operation     //0: Normal operation       //0: Key scanning stopped       //0: LED forced off        //     00: LCD forced off (SEGn, COMn = VLC5)
//1: Test mode            //1: Standby mode           //1: Key scan operation         //1: Normal operation      //     01: LCD forced off (SEGn, COMn = unselected waveform)
//                                                                                                               //     10: Normal operation
//                                                                                                               //     11: Normal operation
delayMicroseconds(2); 
clear_LCD_ADRAM(); //Here I clean all LCD
led(0x08, 0x64); //This is only to DEBUG, you can eleminate this functions and all the calls of it! number of times it blink; 100 = 0x64, 200=0xC8 to interval blinking of led;

//cmd_with_ce(0b01000000); //Write to display data RAM, follwed by byte of address: 10Xaaaaa
//                                                                                      //     000: Write to display data RAM
//                                                                                      //     001: Write to character display RAM
//                                                                                      //     010: Write to CGRAM
//                                                                                      //     011: Write to LED output latch
//                                                                                      //     100: Read key data
//****************************LED ZONE************************************//
cmd_with_ce(0b01000011); // Write LED's.   (Data write mode/read mode setting at last 3 bits
delayMicroseconds(2);                                                                                     
cmd_without_ce(0b00000001); // Write LED nº 1. LED stay ON!
delayMicroseconds(2);
digitalWrite(LCD_stb, LOW); // This line finalize the DATA.
delay(2000);
cmd_with_ce(0b01000011); // Write LED's.   (Data write mode/read mode setting at last 3 bits
delayMicroseconds(2);                                                                                     
cmd_without_ce(0b00000000); // Write LED nº 1. LED stay OFF!
delayMicroseconds(2);
digitalWrite(LCD_stb, LOW); // This line finalize the DATA.
//***************************END LED ZONE********************************//
//


//****************************ASCII ZONE********************************//
cmd_with_ce(0b11000110);
/*
byte seq[5]={0x20,0x20,0x20,0x20, 0x20}; // only to clean the display near of...
    for (unsigned int symb = 0x20; symb < 0xE0; symb++){
      seq[0] =(char(symb)); //Insert ASCII value of table in the array at position "0".
      uPD16432_print(0x06, seq);  //Present at column 6 the char of ASCII table begin at 0x20(sapce) and the last char present at table of uPD16432.
      delay(100);
    }
 */
//*************************MSG's and Color ZONE*************************//
strcpy(data, "HI FOLKS");
//strrevert1(data);
cmd_with_ce(0b11000111);
cmd_with_ce(0b01000000);
uPD16432_print(3,data);//Write the msg "Hi Folks", starting at position 3. Note you have 3 small digits at up left corner!
delay(2000);
strcpy(data, "I'm Alive!!!");
uPD16432_print(0x3, data);
delay(2000);
digitalWrite(11, HIGH);
strcpy(data, "  GREEN   "); //I add the space to fill the 10 digits and at same time clean it!
uPD16432_print(0x3, data);
delay(2000); 
digitalWrite(11, LOW);
strcpy(data, "  YELLOW  "); //I add the space to fill the 10 digits and at same time clean it!
uPD16432_print(0x3, data);
delay(2000); 
}
