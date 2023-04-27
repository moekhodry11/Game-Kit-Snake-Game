
#include <U8glib.h>   // u8glib  fron library mannager
// display constructor for u8g library - this one depends on your type of display

/*
PIN-1 : Ground connect to ground terminal of Arduino board
PIN-2 : +5V supply connect to +5V
PIN-3 : V0c is connect to 10k Variable Potentiometer for contrast control
PIN-4 : DI – Data Input pin is connect to Analog pin of Arduino (A3)
PIN-5 : R/W – Read-write pin is connect to Analog pin of Arduino (A2)
PIN-6-: E – Enable pin is connect to Analog pin of Arduino  (A4)
PIN-7-14 : DB0-DB7 are connect to Digital pin of Arduino like- 8,9,10,11,4,5,6,7
PIN15 -16  : CS1-CS2 serial communication pins are connect to Analog pin of Arduino( A0-A1))
PIN17 – RESET
PIN18- : pot
PIN19- : A + for 5V Supply
PIN20- : K for Ground
 */
U8GLIB_KS0108_128 u8g(8, 9, 10, 11, 4, 5, 6, 7, A4, A0, A1, A3, A2);     // 8Bit Com: D0..D7: 8,9,10,11,4,5,6,7 en=18, cs1=14, cs2=15,di=17,rw=16


// pull down resetor
int left = 2, right = 3, down = 12, up = 13;
int leftV = 0, rightV = 0, downV = 0, upV = 0;

//snakedirection:
//0: left
//1: right
//2: down
//3: up
int snakeDir = 1;

struct gameItem {
  int X; // x position
  int Y;  //y position
};

//diameter of the gameItemSize
int gameItemSize = 4;

//default snake size
unsigned int snakeSize = 3;

unsigned int score = 0;


//array to store all snake body part positions
gameItem snake[100];

//snake food item
gameItem snakeFood;

//GameStates
enum gameState {
  setupGame,
  pausedGame,
  inGame,
  gameOver,
};

gameState state = setupGame;

void draw() {
  drawSnake();
  drawFood();
}
void draw2() {

  // (horizontal,virtical)
  // (128,64)
  drawText("Game Over", 44, 25);
  drawText("Score= ", 44, 35);
  String myStr;
myStr = String(score);   //Converts integer to string
  drawText(myStr, 74, 35);
  
}
void drawText(String text, int x, int y) {
  //char array to print
  char charBuff[10];
  //convert the string to char array
  text.toCharArray(charBuff, 10);
  //prepare the font
  u8g.setFont(u8g_font_5x8);
  //print the text
  u8g.drawStr(x, y, charBuff);
}

void drawSnake() {
  for (int i = 0; i < snakeSize; i++) {
    u8g.drawFrame(snake[i].X, snake[i].Y, gameItemSize, gameItemSize);
  }

}

void drawFood() {
  u8g.drawBox(snakeFood.X, snakeFood.Y, gameItemSize, gameItemSize);
}

void handleColisions() {

  //check if snake eats food
  if (snake[0].X == snakeFood.X && snake[0].Y == snakeFood.Y) {
    //increase snakeSize
    snakeSize+=2;

    score++;
    //regen food
    spawnSnakeFood();
  }

  //check if snake collides with itself
  else {
    for (int i = 1; i < snakeSize; i++) {
      if (snake[0].X == snake[i].X && snake[0].Y == snake[i].Y) {
        state = gameOver;
        
      }
    }
  }
  //check for wall collisions
  if ((snake[0].X < 1) || (snake[0].Y < 1) || (snake[0].X > 127) || (snake[0].Y > 63)) {
    state = gameOver;
  }
}


void spawnSnakeFood() {
  
  //generate snake Food position
  do {
    snakeFood.X = random(2, 126);
  } while (snakeFood.X % 4 != 0);
  do {
    snakeFood.Y = random(2, 62);
  } while (snakeFood.Y % 4 != 0);
}

void gameSetup() {
  //Reset snake size
  snakeSize = 3;

  //snakeHead initial position

  int sHeadX;
  int sHeadY;
  do {
    sHeadX = random(2, 126);
    
  } while (sHeadX % gameItemSize != 0);
  do {
    sHeadY = random(2, 62);
  } while (sHeadY % gameItemSize != 0);

  //set first part of the snake
  snake[0].X = sHeadX;
  snake[0].Y = sHeadY;
  //generate random positions for the rest of the snake
  for (int i = 1; i < snakeSize; i++) {
    //choose random direction to build snake
    int dir = random(0, 3);

    if (dir == 0)//means left
    {
      snake[i].X = snake[i-1].X - gameItemSize;
      snake[i].Y = snake[i - 1].Y;
    }

    else if (dir == 1)//means right
    {
      snake[i].X = snake[i - 1].X + gameItemSize;
      snake[i].Y = snake[i - 1].Y;
    }

    else if (dir == 2)//means up
    {
      snake[i].X = snake[i - 1].X;
      snake[i].Y = snake[i - 1].Y - gameItemSize;
    }

    else if (dir == 3)//means down
    {
      snake[i].X = snake[i - 1].X;
      snake[i].Y = snake[i - 1].Y + gameItemSize;
    }
  }

  //generate snake Food position
  spawnSnakeFood();

  //change state to inGame
  state = pausedGame;
}

void handleInput() {
  //read left button
  leftV = digitalRead(left);
  //read right button
  rightV = digitalRead(right);
  //read up button
  upV = digitalRead(up);
  //read down button
  downV = digitalRead(down);


  if (leftV != 0 && snakeDir != 1) {
    snakeDir = 0;
  }

  if (rightV != 0 && snakeDir != 0) {
    snakeDir = 1;
  }

  if (downV != 0 && snakeDir != 3) {
    snakeDir = 2;
  }

  if (upV != 0 && snakeDir != 2) {
    snakeDir = 3;
  }
}

void updateValues() {
  //update all body parts of the snake excpet the head
  for (int i = snakeSize - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }

  //Now update the head
  //move left
  if (snakeDir == 0) {

    snake[0].X -= gameItemSize;
  }
  //move right
  else if (snakeDir == 1) {

    snake[0].X += gameItemSize;
  }

  //move down
  else if (snakeDir == 2) {

    snake[0].Y += gameItemSize;
  }

  //move up
  else if (snakeDir == 3) {

    snake[0].Y -= gameItemSize;
  }

}

void playGame() {
  handleColisions();
  handleInput();
  updateValues();
  
  u8g.firstPage();
  do {
    
    draw();
    delay(5);
  } while (u8g.nextPage());

}

void gamePaused() {
  //read left button
  leftV = digitalRead(left);
  //read right button
  rightV = digitalRead(right);
  //read up button
  upV = digitalRead(up);
  //read down button
  downV = digitalRead(down);


  if (leftV != 0) {
    state = inGame;
  }

  if (rightV != 0) {
    state = inGame;
  }

  if (downV != 0) {
    state = inGame;
  }

  if (upV != 0) {
    state = inGame;
  }

  u8g.firstPage();
  do {

    draw();
    delay(5);
  } while (u8g.nextPage());
}
void gameLost() {
  u8g.firstPage();
  do {

    draw2();
    delay(5);
  } while (u8g.nextPage());
  delay(2000);
  state = setupGame;
}

void setup() {
  //Set the pinmodes
  pinMode(left, INPUT);
  pinMode(right, INPUT);
  pinMode(down, INPUT);
  pinMode(up, INPUT);
  // random seed for the random function
  randomSeed(analogRead(0));

  if (u8g.getMode() == U8G_MODE_R3G3B2)
    u8g.setColorIndex(255); // white
  else if (u8g.getMode() == U8G_MODE_GRAY2BIT)
    u8g.setColorIndex(3); // max intensity
  else if (u8g.getMode() == U8G_MODE_BW)
    u8g.setColorIndex(1); // pixel on
}

void loop() {
  switch (state)
  {
  case setupGame: gameSetup();
    break;

  case pausedGame: gamePaused();
    break;
  case inGame: playGame();
    break;
  case gameOver: gameLost();
  score=0;
    break;
  }

}
