
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
 
  sendData("AT+CWMODE=3\r\n",1000,DEBUG); // configure as access point
   //sendData("AT+CWJAP=\"MOVISTAR_25B8\",\"XoH7Nn9fPffCil9Elsll\"\r\n",1000,DEBUG); // turn on server on port 80
  //sendData("AT+CIPSTA?\r\n",1000,DEBUG); // get ip address
  delay(2500);
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  delay(2000);
  sendData("AT+CIPSERVER=1,450\r\n",1000,DEBUG); // turn on server on port 80
  
  }

void loop()
  {  
      /*for(int o=0; Serial1.available()>0 || Serial.available()>0;o++){
         if (Serial1.available()>0){Serial.write(Serial1.read());}
          if (Serial.available()>0){Serial1.write(Serial.read());}
      }*/
     //if (Serial1.available()>0){Serial.write(Serial1.read());}
  
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
       
          
       //if(Serial.available()>0){
         //Serial.print("----");
        //response[i] = Serial.read();  
       //serial.print(response[i]);
       //}
        /*if(Serial1.available()>0){
         Serial1.print(Serial1.read());
       }*/
      
   
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
           
        }
        
      }
      if(response[o]=='_'){
       
        attention=true;
        
        
      }
      /*possibleReadyChar=possibleReadyChar[1] + possibleReadyChar[2] + possibleReadyChar[3] + possibleReadyChar[4] + response[o]
      if(possibleReadyChar*/
      /*String last5Chars = String(response).substring(187);
      
      if(last5Chars.indexOf("CONNECT FAIL")){//last5Chars=="ready"
        
        sendData("AT+CWMODE=3\r\n",1000,DEBUG); // configure as access point
  
          delay(2500);
          sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
          delay(2000);
          sendData("AT+CIPSERVER=1,450\r\n",1000,DEBUG); // turn on server on port 80
          attention=false;
          proveItsReady=0;
          itCanBeReady=false;
          delay(2000);
      }*/
      
      
      if(response[o]==' '){
          //Serial.write("isR");
          proveItsReady=1;
          itCanBeReady=true;
          lastCharacter="r";
        }else if(response[o]=='F' && itCanBeReady==true && proveItsReady==1 && lastCharacter=="r"){
          proveItsReady=2;
              //Serial.write("isE");
          attention=true;
          lastCharacter="e";
        }else if(response[o]=='A' && itCanBeReady==true && proveItsReady==2 && lastCharacter=="e"){
          proveItsReady=3;
          //Serial.write("isA");
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

      
      
  if(power>-1){
     Serial.println("pow2er->" +  power);
  }
    //Serial.println("temp->" +  temp);
  if(temp!=-1 || (swing==0 || swing==1)  || acMode>0 || fan>-1 || power>-1){
    //Serial.println("------>" + power);
    //Something has changed
    Serial.println("temp->" +  temp);
    Serial.println("swing->" +  swing);
    Serial.println("acMode->" +  acMode);
    Serial.println("fan->" +  fan);
    Serial.println("power->" +  power);
      
    if(power == 0){
          Serial.println("dentro?");

        irdaikin.off();
    }else if(power==1){
        irdaikin.on();
            Serial.println("dentro sip");

    }
    if(temp>-1){
      Serial.print("temp-> ");
      Serial.println(int(temp));
      irdaikin.setTemp(temp);
    }
    if(swing==0){
       Serial.print("swing-> ");
      Serial.println(swing);
      irdaikin.setSwing_off();
    }else if(swing==1){
       Serial.print("swing-> ");
      Serial.println(swing);
      irdaikin.setSwing_on();
    }
    if(acMode>0){
       Serial.print("mode-> ");
      Serial.println(acMode);
      irdaikin.setMode(acMode);
    }
    if(fan>-1){
     
       Serial.print("fan-> ");
      Serial.println(fan);
       irdaikin.setFan(fan);
    }

  irdaikin.sendCommand();
  power=-1;
  temp=-1;
  swing=-1;
  acMode=-1;
  fan=-1;
    
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

    
