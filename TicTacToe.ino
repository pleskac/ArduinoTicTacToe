#define DEBUG_MESSAGE_MAX_LENGTH  80

#define LED_DELAY 3

#define WIN_FLASHES 5
#define FLASHES 2

//Input and Output pins
#define NUM_OF_LED_LINES 9
#define NUM_OF_BUTTON_INPUTS 5
int userLEDs[] = {11, 10, 9};
int aiLEDs[] = {6, 5, 3};
int annode[] = {8, 7, 4};

int pushButtons[] = {2, A5, A2, A3, A4 }; //3 are encoded, last one isn't

char message[DEBUG_MESSAGE_MAX_LENGTH];

int game[9]; 
int winningCombo[3];
int winner;

int turns;

// the setup routine runs once when you press reset:
void setup() {
    Serial.begin(9600);

    //Setup input and output pins
    for(int i = 0; i < NUM_OF_BUTTON_INPUTS; i++){
        pinMode(pushButtons[i], INPUT);
    }
}

// the loop routine runs over and over again forever:
void loop() {
    /**snprintf(message, DEBUG_MESSAGE_MAX_LENGTH, "A0 %i\n", digitalRead(pushButtons[0]));
        Serial.write(message);
        
        snprintf(message, DEBUG_MESSAGE_MAX_LENGTH, "A1 %i\n", digitalRead(pushButtons[1]));
        Serial.write(message);
        
        snprintf(message, DEBUG_MESSAGE_MAX_LENGTH, "A2 %i\n", digitalRead(pushButtons[2]));
        Serial.write(message);
        
    delay(2000);
   /**/
    
    boolean userTurn = true;
    boolean win = false;
    turns = 0;
    int input = -1;

    //signify the start of a new game
    flashBoard();

    do{
        input = getButtonInput(); //TODO

        
        if(userTurn && input != -1 && game[input] == 0){
          snprintf(message, DEBUG_MESSAGE_MAX_LENGTH, "Button input %i\n", input);
          Serial.write(message);
            //do user turn logic
            game[input] = 1;
            userTurn = false;
            refreshLEDs();

            win = testWin();
            turns ++;

            if(!win){
                int ai = aiMove();
                if(ai != -1 && game[ai] == 0){ //for testing
                  game[ai] = 2;
                  userTurn = true;
                }
                else{
                  snprintf(message, DEBUG_MESSAGE_MAX_LENGTH, "UH OH! AI wanted to play at %i but it was taken.\n", ai);
                  Serial.write(message);
                }
                win = testWin();
                turns++;
            }
            
            snprintf(message, DEBUG_MESSAGE_MAX_LENGTH, "GAME - 0:%i 1:%i 2:%i 3:%i 4:%i 5:%i 6:%i 7:%i 8:%i\n", game[0], game[1], game[2], game[3], game[4], game[5], game[6], game[7], game[8]);
            Serial.write(message);
        }

        refreshLEDs();
        
        //delay(1000);/////////////////////////////////////////////////////////////////////////////////
    }while(!win && turns < 9);

    if (win){
        displayWin();
    }
    else{
        //cat game. don't do anything. board will re-
        //flashBoard();
    }/**/
    
}

void flashBoard(){
    for(int count = 0; count < FLASHES; count ++){
          for(int i = 0; i < 3; i ++){
            pinMode(userLEDs[i], OUTPUT);
            pinMode(annode[i], OUTPUT);
            pinMode(aiLEDs[i], OUTPUT);
          }
          for(int i = 0; i < 3; i ++){
            digitalWrite(annode[i], HIGH);
            digitalWrite(userLEDs[i], LOW);
            digitalWrite(aiLEDs[i], HIGH);
          }
          delay(1000);
          for(int i = 0; i < 3; i ++){
            digitalWrite(annode[i], HIGH);
            digitalWrite(userLEDs[i], HIGH);
            digitalWrite(aiLEDs[i], LOW);
          }
          delay(1000);
          for(int i = 0; i < 3; i ++){
            digitalWrite(annode[i], HIGH);
            digitalWrite(userLEDs[i], HIGH);
            digitalWrite(aiLEDs[i], HIGH);
          }
            /*for (int z = 0; z < 3; z++)
            {
                game[z] = user;
            }
            for(int i = 0; i < 20; i++)
              refreshLEDs();*/
        
    }
    for (int z = 0; z < 9; z++)
    {
        game[z] = 0;
    }
    
}

void displayWin(){
    //reset board to 0
    for (int z = 0; z < 9; z++)
    {
        game[z] = 0;
    }

    for(int i = 0; i < WIN_FLASHES; i++){
        //turn win on, wait a sec
        for(int j = 0; j < 3; j++){
            game[winningCombo[j]] = winner;
        }
        for(int a = 0; a < 20; a ++)
          refreshLEDs();

        //turn all off, wait a sec
        for(int j = 0; j < 3; j++){
            game[winningCombo[j]] = 0;
        }
        for(int a = 0; a < 20; a ++)
          refreshLEDs();
    }
}

//DON'T WAIT FOR USER INPUT
//NEED TO REFRESH THE LEDs
int getButtonInput(){
    int buttonState[NUM_OF_BUTTON_INPUTS];

    boolean buttonPushed = false;
    //Loop through all input pins
    //Since we are using an encoder, multiple pins may be high
    
        delay(1);
    for(int i = 0; i < NUM_OF_BUTTON_INPUTS; i++){
        buttonState[i] = digitalRead(pushButtons[i]);
        
        if(buttonState[i] == LOW){
            buttonPushed = true;
        }
    }
    
     snprintf(message, DEBUG_MESSAGE_MAX_LENGTH, "0: %i  1: %i 2: %i 3: %i 4: %i\n", buttonState[0], buttonState[1], buttonState[2], buttonState[3], buttonState[4]);
    //Serial.write(message);
    
    if(!buttonPushed){
        return -1;
    }
    
    
    
    if(buttonState[3] == LOW){
      return 7;
    }
    
    if(buttonState[4]  == LOW){
      return 8;
    }
    
    if(buttonState[0]  == LOW){
      //1??
      if(buttonState[1]  == LOW){
        //11?
        if(buttonState[2]  == LOW){
          //111
          return 6;
        }else{
          //110
          return 5;
        }
      }else{
        //10?
        if(buttonState[2] == LOW){
          //101
          return 4;
        }else{
          //100
          return 3;
        }
      }
    } else if(buttonState[1]  == LOW){
      //01?
      if(buttonState[2] == LOW){
        //011
        return 2;
      }
      //010
      return 1;
    }else if(buttonState[2] == LOW){
      return 0;
    }
    //if no buttons pressed, return -1
    return -1;
}

boolean testWin(){
    //test user then AI
    for(int i = 1; i <= 2; i++){
        winner = i;

        //ROWS
        //0,1,2 DONE
        //3,4,5 DONE
        //6,7,8 DONE

        //COLS
        //0,3,6 DONE
        //1,4,7 DONE
        //2,5,8 DONE
        
        //DIAGS
        //0,4,8 DONE
        //2,4,6 DONE

        if(game[4] == i){
            //0,4,8
            //2,4,6
            //3,4,5
            //1,4,7
            if(game[0] == i && game[8] == i){
                winningCombo[0] = 0;
                winningCombo[1] = 4;
                winningCombo[2] = 8;
                return true;
            }
            else if (game[2] == i && game[6] == i){
                winningCombo[0] = 2;
                winningCombo[1] = 4;
                winningCombo[2] = 6;
                return true;
            }
            else if(game[3] == i && game[5] == i){
                winningCombo[0] = 3;
                winningCombo[1] = 4;
                winningCombo[2] = 5;
                return true;
            }
            else if(game[1] == i && game[7] == i){
                winningCombo[0] = 1;
                winningCombo[1] = 4;
                winningCombo[2] = 7;
                return true;
            }
        }

        if(game[0] == i){
            //0,1,2
            //0,3,6
            if(game[1] == i && game[2] == i){
                winningCombo[0] = 0;
                winningCombo[1] = 1;
                winningCombo[2] = 2;
                return true;
            }
            else if(game[3] == i && game[6] == i){
                winningCombo[0] = 0;
                winningCombo[1] = 3;
                winningCombo[2] = 6;
                return true;
            }
        }

        if(game[8] == i){
            //6,7,8
            //2,5,8
            if(game[6] == i && game[7] == i){
                winningCombo[0] = 6;
                winningCombo[1] = 7;
                winningCombo[2] = 8;
                return true;
            }
            else if(game[2] == i && game[5] == i){
                winningCombo[0] = 2;
                winningCombo[1] = 5;
                winningCombo[2] = 8;
                return true;
            }
        }
    }

    return false;
}

int aiMove(){
  //Start in the middle if possible.
  if(game[4] == 0){
    return 4;
  }
  
  //User played center first, go for a corner. The side will lose.
  if(turns == 1){
    if(game[0] == 0)
      return 0;
    else if(game[2] == 0)
      return 2;
  }
  
  //See if AI has two in a row
  //See if user has two in a row
  int i = 2;
  for(i = 2; i > 0; i--){
    //row1
    if(((game[0] + game[1] + game[2]) == 2*i) && (game[0] == i || game[1] == i) ){
      for(int j = 0; j < 3; j++){
        if(game[j] == 0){
          return j;
        }
      }
    }
    
    //row2
    if(((game[3] + game[4] + game[5]) == 2*i) && (game[3] == i || game[4] == i) ){
      for(int j = 3; j < 6; j++){
        if(game[j] == 0){
          return j;
        }
      }
    }
    
    //row3
    if(((game[6] + game[7] + game[8]) == 2*i) && (game[6] == i || game[7] == i) ){
      for(int j = 6; j < 9; j++){
        if(game[j] == 0){
          return j;
        }
      }
    }

    //col1
    if(((game[0] + game[3] + game[6]) == 2*i) && (game[0] == i || game[3] == i) ){
      if(game[0] == 0)
        return 0;
      else if(game[3] == 0)
        return 3;
      else if (game[6] == 0)
        return 6;
    }
    
    //col2
    if(((game[1] + game[4] + game[7]) == 2*i) && (game[1] == i || game[4] == i) ){
      if(game[1] == 0)
        return 1;
      else if(game[4] == 0)
        return 4;
      else if (game[7] == 0)
        return 7;
    }
    
    //col3
    if(((game[2] + game[5] + game[8]) == 2*i) && (game[2] == i || game[5] == i) ){
      if(game[2] == 0)
        return 2;
      else if(game[5] == 0)
        return 5;
      else if (game[8] == 0)
        return 8;
    }

    //diag /
    if(((game[2] + game[4] + game[6]) == 2*i) && (game[2] == i || game[4] == i) ){
      if(game[2] == 0)
        return 2;
      else if(game[4] == 0)
        return 4;
      else if (game[6] == 0)
        return 6;
    }

    //diag \
    //for some reason, compiler errors if this is on more than one line
    if(((game[0] + game[4] + game[8]) == 2*i) && (game[0] == i || game[4] == i) ){ if(game[0] == 0) return 0; else if(game[4] == 0) return 4; else if (game[8] == 0) return 8; }
  }//end for
  
  //MORE STRATEGY
  
  
  //protect against the "L"
  if(game[0] == 1){
    if(game[5] == 1 && game[2] == 0){
      return 2;
    }
    if(game[7] == 1 && game[6] == 0){
      return 6;
    }
  }else if(game[2] == 1){
    if(game[7] == 1 && game[8] == 0){
      return 8;
    }
    if(game[3] == 1 && game[0] == 0){
      return 0;
    }
  }else if(game[6] == 1){
    if(game[1] == 1 && game[0] == 0){
      return 0;
    }
    if(game[5] == 1 && game[8] == 0){
      return 8;
    }
  }else if(game[8] == 1){
    if(game[4] == 1 && game[6] == 0){
      return 6;
    }
    if(game[1] == 1 && game[2] == 0){
      return 2;
    }
  }
  
  //protect against symmetry
  if(game[0] == 1 && game[8] == 1){
    //don't go in a corner
    if(game[1] == 0) return 1;
    else if(game[3] == 0) return 3;
    else if(game[5] == 0) return 5;
   else if(game[7] == 0) return 7;
  }
  if(game[2] == 1 && game[6] == 1){
   //don't go in a corner 
   if(game[1] == 0) return 1;
   else if(game[3] == 0) return 3;
   else if(game[5] == 0) return 5;
   else if(game[7] == 0) return 7;
  }
  if(game[1] == 1 && game[3] == 1){
   //go between them
   if(game[0] == 0) return 0;
  }
  if(game[1] == 1 && game[5] == 1){
    //go between them
    if(game[2] == 0) return 2;
  }
  if(game[3] == 1 && game[7] == 1){
   //go between them
   if(game[6] == 0) return 6;
  }
  if(game[7] == 1 && game[5] == 1){
    //go between them
    if(game[8] == 0) return 8;
  }
  
  for(int a = 0; a < 9; a++){
    if(game[a] == 0){
      return a;
    }
  }
  
  return -1;
}

void lightLED()
{
 // shift the bits to the right, turning on the LEDs whenever
  // there is a 1, turning off whenever there is a 0 in LEDOnOff
  // If the LED is lit, LEDColour determines which LED is lit
  // 1 is red, 0 is green
  
  for (int j=0;j<3;j++)
  {
    pinMode(userLEDs[j], INPUT);
    digitalWrite(userLEDs[j], HIGH);
    pinMode(aiLEDs[j], INPUT);
    digitalWrite(aiLEDs[j], HIGH);
    pinMode(annode[j], INPUT);
    digitalWrite(annode[j], HIGH);
  }
  
  for (int i=0;i<9;i++)
  {  
      if (game[i] == 1)
      {
         pinMode(userLEDs[i%3], OUTPUT);
         pinMode(annode[i/3], OUTPUT);
         digitalWrite(userLEDs[i%3], LOW);
         digitalWrite(annode[i/3], HIGH);
         
         delay(LED_DELAY);
         
         digitalWrite(userLEDs[i%3], HIGH);
         pinMode(userLEDs[i%3], INPUT);
         pinMode(annode[i/3], INPUT);
      } else if (game[i] == 2)
      {
         pinMode(aiLEDs[i%3], OUTPUT);
         pinMode(annode[i/3], OUTPUT);
         digitalWrite(aiLEDs[i%3], LOW);
         digitalWrite(annode[i/3], HIGH);
         
         delay(LED_DELAY);
         
         digitalWrite(aiLEDs[i%3], HIGH);
         pinMode(aiLEDs[i%3], INPUT);
         pinMode(annode[i/3], INPUT);        
      }
      
    }
 
  
}

void refreshLEDs(){
  lightLED();
  return;
}



