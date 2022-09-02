
#include <DYIRDaikin.h>

DYIRDaikin irdaikin;
int isOn;
//This indicates the digit of the temperature set
int checkNumber=0;//0 false, 1 -> first number, 2 -> second number
char checking = '.';

#define DEBUG true

boolean commandSent=false;
boolean attention=false;
int i=0; 
char response[200];
boolean isAvailable=false;

int temp=-1;
int swing=-1;//0 not swing, 1 swing
int acMode=-1;//Diferent modes
int fan=-1;//until 4(max speed)
int power=-1;//0 off 1 on
//For reseting the module every half an hour
unsigned long timesReseted= 0;
//For programmed tasks
unsigned long currentMillis = millis();

unsigned long previousMillisOff = millis();//Millis when the 'timer' for turning it off was set
unsigned long previousMillisOn = millis();//Millis when the 'timer' for turning it on was set
unsigned long previousMillisGeneral = millis();

unsigned long millisToWaitOn = 0;//Millis to wait for until turning it On
unsigned long millisToWaitOff = 0;//Millis to wait for until turning it Off
unsigned long secondsToWaitGeneral = 0;//The time to wait is set here and when the power is known,
//the value of this variable is asigned to millisToWaitOn or millisToWaitOff, and there we multiply it by 1000
//for it to be millis
boolean checkingC = false;
boolean checkingForTime = false;
int attributesForOn[4]; //Atributtes that will be used for turning it on when it's time
//In this order: temp, mode, fan, swing
int numberOfTimeCharacters = -1;//Number of characters the number has
int actualTimeCharacter = -1;


/*IDENTIFYING ORDERS TO TURNING IT ON/OFF IN X TIME
  Letter c (clock) -> _cyx (being y the number of characters the number has and x the seconds to be wait)
  
  EXAMPLE OF ORDER TO TURNING IT ON IN HALF AN HOUR:
  http://.../_c41800_p0_t20_m1_f1_s0
  4 -> Number of character the seconds has (1800 has 4 characters)
  1800 -> Number of seconds to be wait
*/
//--------------------


int proveItsReady=0; //Character number of the word ready
boolean itCanBeReady=false; //It says the esp has said r of ready
String lastCharacter = "";
void setup()
  {
    //For internet:  
    Serial.begin(115200);
     Serial1.begin(115200);
    //For IR:

  #ifdef DYIRDAIKIN_SOFT_IR
  irdaikin.begin(3);
  #else
  irdaikin.begin();
  #endif
//If all data isnt set, the A/C doesnt do anything
irdaikin.on();
  irdaikin.setSwing_off();
  irdaikin.setMode(1);
  irdaikin.setFan(5);//FAN speed to MAX
  irdaikin.setTemp(22);
  
  
  /*EXAMPLE:
  irdaikin.on();
  irdaikin.setSwing_off();
  irdaikin.setMode(1);
  irdaikin.setFan(4);//FAN speed to MAX
  irdaikin.setTemp(25);
  //----everything is ok and to execute send command-----
  irdaikin.sendCommand();
  //isOn = 0;
  */
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
 
  //sendData("AT+CWMODE=3\r\n",1000,DEBUG); // configure as access point
   //sendData("AT+CWJAP=\"MOVISTAR_25B8\",\"XoH7Nn9fPffCil9Elsll\"\r\n",1000,DEBUG); // turn on server on port 80
  //sendData("AT+CIPSTA?\r\n",1000,DEBUG); // get ip address
  //delay(2500);
  //sendData("AT+CIPSTA=\"192.168.1.160\"\r\n",1000,DEBUG); // define static ip
  //delay(2500);
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  delay(2000);
  sendData("AT+CIPSERVER=1,450\r\n",1000,DEBUG); // turn on server on port 80
  
  }

void loop()
  {  
    if(millis() - currentMillis >=1000){
      currentMillis=millis();
    }     
  
   for(int o=0; Serial1.available()>0 || Serial.available()>0;o++){
       // if(o>200){o=0;}
      
      if (Serial1.available()>0){
       response[o] = Serial1.read();  
        Serial.write(response[o]);
       
      }
      
       if (Serial.available()>0){
       //  
        response[o] = Serial.read();
        Serial1.write(response[o]);
        
         
       }
       
          

      
      if(checking=='c'){
        if(actualTimeCharacter<0){
          previousMillisGeneral = millis();
          numberOfTimeCharacters = String(response[o]).toInt();
          Serial.println(".number" + String(numberOfTimeCharacters) + "number.");
          actualTimeCharacter = 0;//It points to the next character
          secondsToWaitGeneral = 0;
        }else{
          String numberToMultiply = "1";
          for(int i=1; i<=(numberOfTimeCharacters - actualTimeCharacter - 1); i++){
            numberToMultiply += "0";
          }
          
          secondsToWaitGeneral += long(long(String(response[o]).toInt()) * long(numberToMultiply.toInt()));
          actualTimeCharacter+=1;
        }
      checkingForTime=true;


        
      }
      if(checking=='t'){
        //Checking temperature
        Serial.println("in temp");
        if(checkNumber==1){
          Serial.println("1 temp");
          temp=String(response[o]).toInt()*10;
          checkNumber=2;
        }else if(checkNumber==2){
           Serial.println("2 temp");
          temp=int(temp) + int(String(response[o]).toInt());
          checking='.';
        }
        
      }else if(checking=='s'){//swing
        swing=String(response[o]).toInt();
        checking='.';
      }else if(checking=='m'){//mode
        acMode=String(response[o]).toInt();
        checking='.';
      }else if(checking=='f'){//fan
        fan = String(response[o]).toInt();
        checking='.';
      }else if(checking=='p'){//power
       

        power=String(response[o]).toInt();
        checking='.';
         Serial.print("->");
        Serial.print(power);
        Serial.println("<2-");
        Serial.print(String(response[o]));
        Serial.println("<23-");
      }
          if(attention==true){
        attention=false;
        
        if(response[o]=='t'){
          //We're about to check temp
          checkNumber=1;
          checking='t';
        }else if(response[o]=='s' || response[o]=='m' || response[o]=='f' || response[o]=='p'){
          checking=response[o];
           Serial.print("the check is " + String(response[o]));
        }else if(response[o]=='c'){
          Serial.println("c checking");
          checking='c';
          checkingForTime=true;
          checkingC=true;
        }
        
      }
      if(response[o]=='_'){
        attention=true;
        checkingC = false;
        checking='.';
        actualTimeCharacter = -1;
      }
       
      
      if(response[o]==' '){
          //Serial.write("isR");
          proveItsReady=1;
          itCanBeReady=true;
          lastCharacter="r";
        }else if(response[o]=='F' && itCanBeReady==true && proveItsReady==1 && lastCharacter=="r"){
          proveItsReady=2;
          attention=true;
          lastCharacter="e";
        }else if(response[o]=='A' && itCanBeReady==true && proveItsReady==2 && lastCharacter=="e"){
          proveItsReady=3;
          attention=true;
          lastCharacter="a";
        }else if(response[o]=='I' && itCanBeReady==true && proveItsReady==3 && lastCharacter=="a"){
          proveItsReady=4;
          //Serial.write("isD");
          attention=true;
          lastCharacter="d";
        }else if(response[o]=='L' && itCanBeReady==true && proveItsReady==4 && lastCharacter=="d"){
          proveItsReady=5;
          //Serial.write("isY");
          attention=true;
          //It has said ready
          delay(2000);
          sendData("AT+RST\r\n",1000,DEBUG); // configure for multiple connections
          delay(2000);
          sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
          delay(2000);
          sendData("AT+CIPSERVER=1,450\r\n",1000,DEBUG); // turn on server on port 80
          attention=false;
          proveItsReady=0;
          itCanBeReady=false;
        }else{
          lastCharacter="";
          itCanBeReady=false;
          proveItsReady=0;
        }

  
    
   

  }
  checking='.';
  if(checkingForTime && checkingC==false ){
      

      if(power==1){
        attributesForOn[0]=temp;//temp
        attributesForOn[1]=acMode;//mode
        attributesForOn[2]=fan;//fan
        attributesForOn[3]=swing;//Swing
      
        millisToWaitOn = secondsToWaitGeneral * 1000;  
        previousMillisOn = previousMillisGeneral;
      }else if(power==0){
        millisToWaitOff = secondsToWaitGeneral * 1000;
        previousMillisOff = previousMillisGeneral;
      }
     // Serial.println("timeoff" + String(millisToWaitOff) + "timeon" + String(millisToWaitOn) + "power" + String(power));
      //Serial.println("_->>- actualTimeCharacter " + String(actualTimeCharacter) + ",,numberOfTimeCharacters" + String(numberOfTimeCharacters) + "time" + String(secondsToWaitGeneral) + "-<<-_");
      checkingForTime=false;
      actualTimeCharacter = -1;

      power=-1;
      temp=-1;
      acMode=-1;
      fan=-1;
      swing=-1;
  }
  if(power>-1){
    // Serial.println("pow2er->" +  power);
  }
    //Serial.println("temp->" +  temp);
  if(temp!=-1 || (swing==0 || swing==1)  || acMode>0 || fan>-1 || power>-1){
    //Serial.println("------>" + power);
    //Something has changed
   /* Serial.println("temp->" +  temp);
    Serial.println("swing->" +  swing);
    Serial.println("acMode->" +  acMode);
    Serial.println("fan->" +  fan);
    Serial.println("power->" +  power);*/
      
    
setACAttributes(power, temp, acMode, fan, swing);
    //Serial.println("power2->" +  power);
  irdaikin.sendCommand();
  power=-1;
  temp=-1;
  swing=-1;
  acMode=-1;
  fan=-1;
    
  }

//For timers:
if(millisToWaitOn>0){
 
  if ((currentMillis - previousMillisOn) >= millisToWaitOn && (long(currentMillis) - long(previousMillisOn)) > 0) {
     Serial.print(String(currentMillis) + ",prevOn" + String(previousMillisOn) + ",waitOn" + String(millisToWaitOn) + ",quit" + String(currentMillis - previousMillisOn));
    //Its time for turning it on
    setACAttributes(1,attributesForOn[0],attributesForOn[1],attributesForOn[2],attributesForOn[3]);
     /* 1,//power
      attributesForOn[0],//temp
      attributesForOn[1],//mode
      attributesForOn[2],//fan
      attributesForOn[3]//Swing
   );*/
   /* setACAttributes(
      1,//power
      attributesForOn[0],//temp
      attributesForOn[1],//mode
      attributesForOn[2],//fan
      attributesForOn[3]//Swing
   );*/
     irdaikin.sendCommand();

    millisToWaitOn=0;
  }
}
if(millisToWaitOff>0){
  if ((currentMillis - previousMillisOff) >= millisToWaitOff && (long(currentMillis) - long(previousMillisOff))>0) {
    //Its time for turning it off
    irdaikin.off();
    irdaikin.sendCommand();
    millisToWaitOff=0;

  }
}

//resseting the module every 30 minutes
//30 minutes -> 1 800 000 millis
if(currentMillis/(timesReseted + 1) >= 1800000){
  //time to reset it
  timesReseted+=1;
  
  sendData("AT+RST\r\n",1000,DEBUG); // configure for multiple connections
  delay(4500);
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  delay(2000);
  sendData("AT+CIPSERVER=1,450\r\n",1000,DEBUG); // turn on server on port 80
  
}
    
  
}
void setACAttributes(int power, int temp, int acMode, int fan, int swing){
       //Serial.println("setACAttributes");
  if(power == 0){
       //Serial.println("dentro?");
        irdaikin.off();
    }else if(power==1){
        irdaikin.on();
    }
    
    if(temp>-1){
      //Serial.print("temp-> ");
      //Serial.println(int(temp));
      irdaikin.setTemp(temp);
    }
    if(swing==0){
       //Serial.print("swing-> ");
      //Serial.println(swing);
      irdaikin.setSwing_off();
    }else if(swing==1){
      //Serial.print("swing-> ");
      //Serial.println(swing);
      irdaikin.setSwing_on();
    }
    if(acMode>0){
      //Serial.print("mode-> ");
      //Serial.println(acMode);
      irdaikin.setMode(acMode);
    }
    if(fan>-1){
      //Serial.print("fan-> ");
      //Serial.println(fan);
       irdaikin.setFan(fan);
    }
}
String sendData(String command, const int timeout, boolean debug)
            {
                String response = "";
                Serial1.print(command);
                long int time = millis();
                while( (time+timeout) > millis())
                {
                   while(Serial1.available())
                      {
                         char c = Serial1.read(); // read the next character.
                         response+=c;
                      }  
                }
                
                if(debug)
                     {
                     Serial.print(response); //displays the esp response messages in arduino Serial monitor
                     }
                return response;
            }

    
