char cmd0,cmd1,arg0,arg1,arg2,arg3, commandbuffer[20];
int i,j,k;
int alphaup = 7;
int alphadown = 6;
int betaleft = 5;
int betaright = 4;
int relay;

void setup(){

//  analogReference(EXTERNAL);

  Serial.begin(9600);
  pinMode(alphaup,OUTPUT);  
  pinMode(alphadown,OUTPUT);  
  pinMode(betaleft,OUTPUT);  
  pinMode(betaright,OUTPUT);
  digitalWrite(alphaup, LOW);
  digitalWrite(alphadown, LOW);
  digitalWrite(betaleft, LOW);
  digitalWrite(betaright, LOW);
  Serial.flush();
}

void loop() {
start: 
  delay(100);
//  k = Serial.available();
//  Serial.println(k);
//  if(k>=6){    
  if(Serial.available()==6){  
      cmd0 = Serial.read();
      cmd1 = Serial.read();
      arg0 = Serial.read();
      arg1 = Serial.read();
      arg2 = Serial.read();
      arg3 = Serial.read();
      Serial.flush();
//      while(Serial.available()){
//          Serial.read();
//      }



    if( cmd0=='R'){
      switch (cmd1){
      case '0': 
        relay = alphaup; 
        break;
      case '1': 
        relay = alphadown; 
        break;
      case '2': 
        relay = betaleft; 
        break;
      case '3': 
        relay = betaright; 
        break;          
      default: goto start;
        break;
      }
      if( arg0 =='o' && arg1=='n'){
        digitalWrite(relay, HIGH);   // set the LED on
//        Serial.println("Relay high");
      }  
      if (arg0 =='o' && arg1=='f' && arg2 =='f'){
        digitalWrite(relay, LOW);    // set the LED off
//        Serial.println("Relay low");  
      }
      Serial.print("\"");
      Serial.print(cmd0);
      Serial.print(cmd1);
      Serial.print(arg0);
      Serial.print(arg1);
      Serial.print(arg2);
      Serial.print(arg3);
      Serial.println("\"");
    }
    
    if( cmd0=='T' && cmd1=='0'){
/*      Serial.println(analogRead(0));*/
        sprintf(commandbuffer,"\"T0%04d\"",analogRead(0));
// The is tha ideia behind the cntents of the arduino, but not the true file.
// the true file was lost in a stupid rsync
// THIS IS NOT TESTED. 
        Serial.println(commandbuffer);
    }
    if( cmd0=='A' && cmd1=='0'){
      sprintf(commandbuffer,"\"A0%04d\"",analogRead(0));
      Serial.println(commandbuffer);
    }
    if( cmd0=='A' && cmd1=='1'){
      sprintf(commandbuffer,"\"A1%04d\"",analogRead(1));
      Serial.println(commandbuffer);
    }
    if( cmd0=='A' && cmd1=='2'){
      sprintf(commandbuffer,"\"A2%04d\"",analogRead(2));
      Serial.println(commandbuffer);
    }
    if( cmd0=='A' && cmd1=='3'){
      sprintf(commandbuffer,"\"A3%04d\"",analogRead(3));
      Serial.println(commandbuffer);
    }
    
    if( cmd0=='A' && cmd1=='4'){
      sprintf(commandbuffer,"\"A4%04d\"",analogRead(4));
      Serial.println(commandbuffer);
    }
    if( cmd0=='A' && cmd1=='5'){
      sprintf(commandbuffer,"\"A5%04d\"",analogRead(5));
      Serial.println(commandbuffer);
    }
          
  } else {
//     Serial.println("\"Flush \"");
//     Serial.print(Serial.available());
     Serial.flush(); 
     delay(10);
  }
}

