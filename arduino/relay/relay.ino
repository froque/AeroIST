char cmd0,cmd1,arg0,arg1,arg2,arg3,etx, commandbuffer[20];

int alphaup = 12;
int alphadown = 11;
int betaleft = 10;
int betaright = 9;
int relay;
int led = 13;

int pin_addr_0 = 5;  // A  (pin 1) on SN74HCT138N
int pin_addr_1 = 6;  // B  (pin 2) on SN74HCT138N
int pin_addr_2 = 7;  // C  (pin 3) on SN74HCT138N
int pin_enable = 8;  // G1 (pin 6) on SN74HCT138N

// it easier this way. Table of values to 
boolean addr_0[8] = {LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW, HIGH};
boolean addr_1[8] = {LOW, LOW, HIGH, HIGH, LOW, LOW, HIGH, HIGH};
boolean addr_2[8] = {LOW, LOW, LOW, LOW, HIGH, HIGH, HIGH, HIGH};


void choose_channel(int channel){
  digitalWrite(pin_addr_0, addr_0[channel]);
  digitalWrite(pin_addr_1, addr_1[channel]);
  digitalWrite(pin_addr_2, addr_2[channel]);
}

void setup(){

//  analogReference(EXTERNAL);

  Serial.begin(57600);
  pinMode(alphaup,OUTPUT);  
  pinMode(alphadown,OUTPUT);  
  pinMode(betaleft,OUTPUT);  
  pinMode(betaright,OUTPUT);
  pinMode(led,OUTPUT);
  pinMode(pin_addr_0,OUTPUT);
  pinMode(pin_addr_1,OUTPUT);
  pinMode(pin_addr_2,OUTPUT);
  pinMode(pin_enable,OUTPUT);
  digitalWrite(alphaup, LOW);
  digitalWrite(alphadown, LOW);
  digitalWrite(betaleft, LOW);
  digitalWrite(betaright, LOW);
  digitalWrite(led, LOW);
  digitalWrite(pin_addr_0, LOW);
  digitalWrite(pin_addr_1, LOW);
  digitalWrite(pin_addr_2, LOW);
  digitalWrite(pin_enable, LOW);
}
  
    // communication form: $CIXXXX\n
    // $ is a start char
    // \n is a stop char
    
    // current commands
    // R - relay commands
    // A - read analog commands
    // C - channel enable/disable and selection commands
    
    // led 13 will be on during command processing

void loop() {
start:
    char c;
    c = Serial.read();
    if ( c == '$' ){   
        digitalWrite(led, HIGH);
        while(Serial.available() < 7){
            ;
        }
        
        cmd0 = Serial.read();
        cmd1 = Serial.read();
        arg0 = Serial.read();
        arg1 = Serial.read();
        arg2 = Serial.read();
        arg3 = Serial.read();
        etx  = Serial.read();
        if ( etx != '\n'){
            //Serial.println("bad ending");
            digitalWrite(led, LOW);
            goto start;
        } else {
          
            if (cmd0 == 'C'){
              if (cmd1 == 'E'){
                digitalWrite(pin_enable, HIGH);
                Serial.print("$CExxxx\n");
              }
              if (cmd1 == 'D'){
                digitalWrite(pin_enable, LOW);
                Serial.print("$CDxxxx\n");
              }
              if (cmd1 == 'S'){
                // only allow from 0 to 7
                if ('0' <= arg0 &&  arg0 <= '7') {
                  int i = arg0 - '0';
                  choose_channel(i);                  
                  sprintf(commandbuffer,"$CS%04d\n",i);
                  Serial.print(commandbuffer);
                } else {
                  // handle error
                  goto start;
                }
              }
            }
          
            // Commands to control relays
            if( cmd0=='R'){
                switch (cmd1){
                case '0': relay = alphaup;   break;
                case '1': relay = alphadown; break;
                case '2': relay = betaleft;  break;
                case '3': relay = betaright; break;
                default : goto start;        break;
                }
                if( arg0 =='o' && arg1=='n'){
                    digitalWrite(relay, HIGH);   // set the RELAY on
                }  
                if (arg0 =='o' && arg1=='f' && arg2 =='f'){
                    digitalWrite(relay, LOW);    // set the RELAY off 
                }
                Serial.print("$");
                Serial.print(cmd0);
                Serial.print(cmd1);
                Serial.print(arg0);
                Serial.print(arg1);
                Serial.print(arg2);
                Serial.print(arg3);
                Serial.print('\n'); // stop char
            }
            
            // commands to read analog values
            if( cmd0=='A' && cmd1=='0'){
                sprintf(commandbuffer,"$A0%04d\n",analogRead(0));
                Serial.print(commandbuffer);
            }
            if( cmd0=='A' && cmd1=='1'){
                sprintf(commandbuffer,"$A1%04d\n",analogRead(1));
                Serial.print(commandbuffer);
            }
            if( cmd0=='A' && cmd1=='2'){
                sprintf(commandbuffer,"$A2%04d\n",analogRead(2));
                Serial.print(commandbuffer);
            }
            if( cmd0=='A' && cmd1=='3'){
                sprintf(commandbuffer,"$A3%04d\n",analogRead(3));
                Serial.print(commandbuffer);
            }
            if( cmd0=='A' && cmd1=='4'){
                sprintf(commandbuffer,"$A4%04d\n",analogRead(4));
                Serial.print(commandbuffer);
            }
            if( cmd0=='A' && cmd1=='5'){
                sprintf(commandbuffer,"$A5%04d\n",analogRead(5));
                Serial.print(commandbuffer);
            }
            Serial.flush(); 
        }
        digitalWrite(led, LOW);
    } // end if c == '$'
} // end loop()
