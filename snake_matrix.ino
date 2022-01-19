#include <stdio.h> //We need NULL for the snake's head segment
#include <Adafruit_IS31FL3731.h> //LED matrix library

//a segment of our linked-list snake
struct Segment{
  int x;
  int y;
  struct Segment *next;  //pointer to next segment
};

//just a point but it will represent food
struct Food{
  int x;
  int y;
};

//input pins to control direction
const int RIGHT_PIN = 6;
const int LEFT_PIN = 7;

//our LED matrix
Adafruit_IS31FL3731 matrix = Adafruit_IS31FL3731();

//game variables
int snakeLength;
Segment head;
Segment tail;
Food currFood = {0,0};
bool gameOver = false;

void setup() {
  //initialize our matrix. We assume this won't fail. 
  matrix.begin();
  matrix.setRotation(0); //this may need to be changed

  /*
    Game setup
  */
  //initial snake
  head = {6, 5, NULL};
  Segment mid = {5, 5, &head};
  tail = {4, 5, &mid};
  lightSnake();
  
  createFood();

}

void loop() {
  if(gameOver){
    gameOverDisp();
  } else {
  
    bool left = digitalRead(LEFT_PIN) == HIGH;
    bool right = digitalRead(RIGHT_PIN) == HIGH;

    Segment newhead;
  
    //new head in the new direction
    if(left and not right){
      newhead = {head.x-1, head.y, &head};
    } else if(right and not left){
      newhead = {head.x+1, head.y, &head};
    } else if(right and left){
      newhead = {head.x, head.y-1, &head};
    } else {
      newhead = {head.x, head.y+1, &head};
    }

    head = newhead;
   
    if isCollision(){
      gameOver = true;
      continue;
    }
    
    //if the new head is on food, don't decrease length
    if (not(head.x == currFood.x and head.y == currFood.y)){
      tail = *tail.next;
    } else {
      snakeLength += 1;
      createFood();
    }
  
    lightSnake();
    delay(500);
  }
}

void gameOverDisp(){
  matrix.clear();
  delay(10000);
}

void lightSnake(){
  
  //use full brightness for now until we're sure it works
  Segment currSeg = tail;
  for(int i=0; i<snakeLength; i++){
    matrix.drawPixel(currSeg.x, currSeg.y, 255);
    currSeg = *currSeg.next;
  }
  
}

//randomly choose a pixel that isn't in the snake
//asign that to food and light it. No blinking yet. 
void createFood(){
  bool goodPoint = false;
  int x;
  int y;
  while(not goodPoint){
    x = random(0, 9);
    y = random(0, 16);
    goodPoint = true;
    Segment currSegment = tail;
    for(int i=0; i<snakeLength; i++){  
      if(currSegment.x == x and currSegment.y == y){
        goodPoint = false;
      }
    currSegment = *currSegment.next; 
    }
  }     
  
  matrix.drawPixel(currFood.x, currFood.y, 0);
  currFood = {x,y};
  matrix.drawPixel(currFood.x, currFood.y, 255);

}

//detect if the snake's head collided with a wall or its body
void isCollision(){
  if(head.x < 0 or head.x > 8 or head.y <  0 or head.y > 15){
    return true;
  }
 
  Segment currSeg = tail; 

  for(int i=0; i<snakeLength; i++){
    if(currSeg.x == head.x and currSeg.y == head.y){
       return true;
    }
    currSeg = *currSeg.next;
  }

  return false;
}

