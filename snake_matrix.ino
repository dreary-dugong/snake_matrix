#include <stdio.h> //We need NULL for the snake's head segment
#include <Adafruit_IS31FL3731.h>

//our LED matrix
Adafruit_IS31FL3731 matrix = Adafruit_IS31FL3731();

//game variables
int snakeLength;
struct Segment head;
struct Segment tail;
struct Food currFood = {0,0};

void setup() {
  //initialize our matrix. We assume this won't fail. 
  matrix.begin();
  matrix.setRotation(0); //this may need to be changed

  /*
    Game setup
  */
  //initial snake
  head = {6, 5, NULL};
  struct Segment mid = {5, 5, &head};
  tail = {4, 5, &mid};
  lightSnake();
  
  createFood();

}

void loop() {
   
}

void lightSnake(){
  
  //use full brightness for now until we're sure it works
  struct Segment currSeg = tail;
  for(int i=0; i<snakeLength; i++){
    matrix.drawPixel(currSeg.x, currSeg.y, 255);
    currSeg = &currSeg.next;
  }
  
}

//randomly choose a pixel that isn't in the snake
//asign that to food and light it. No blinking yet. 
void createFood(){

}

//detect if the snake's head collided with a wall or its body
void isCollision(){
  if(head.x < 0 or head.x > 8 or head.y <  0 or head.y > 15){
    return true;
  }
 
  struct Segment currSeg = tail; 

  for(int i=0; i<snakeLength; i++){
    if(currSeg.x == head.x and currSeg.y == head.y){
       return true;
    }
    currSeg = &currSeg.next;
  }

  return false;
}

//a segment of our linked-list snake
struct Segment{
  int x;
  int y;
  *struct Segment next;  //pointer to next segment
}

//just a point but it will represent food
struct Food{
  int x;
  int y;
}
