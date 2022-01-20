#include <stdio.h> //We need NULL for the snake's head segment
#include <stdlib.h> //memory allocation.
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

//used for choosing where the snake moves
enum direction{up, down, left, right};

//input pins to control direction
const int JOYSTICK_X = A2;
const int JOYSTICK_Y = A3;

//our LED matrix
Adafruit_IS31FL3731 matrix = Adafruit_IS31FL3731();

//game variables
int snakeLength; //length of the snake
struct Segment *head_ptr; //head of the snake
struct Segment *tail_ptr; //tail of the snake
struct Food currFood = {0,0}; //current food
enum direction currDirection = up; //snake direction
bool gameOver = false; //has the game ended
int moveDelay = 2000; //time between snake moves

void setup() {

  //pinmodes
  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_Y, INPUT);

  //set seed
  randomSeed(analogRead(A5)); //A5 must be unused
  
  //serial for debugging
  Serial.begin(9600);

  //initialize our matrix. We assume this won't fail. 
  matrix.begin();

  //matrix settings
  matrix.setRotation(3); 
  matrix.setTextSize(2);
  matrix.setTextWrap(false);
  matrix.setTextColor(25);

  /*
    Game setup
  */
  //initial snake
  head_ptr = (Segment*) malloc(sizeof(struct Segment));
  head_ptr->x = 6;
  head_ptr->y = 5;
  head_ptr->next = NULL;
  Serial.println("Head created and pointer stored.");

  struct Segment *mid_ptr = (Segment*) malloc(sizeof(struct Segment));
  mid_ptr->x = 5;
  mid_ptr->y = 5;
  mid_ptr->next = head_ptr; 
  Serial.println("Mid created.");

  tail_ptr = (Segment*) malloc(sizeof(struct Segment));
  tail_ptr->x = 4;
  tail_ptr->y = 5;
  tail_ptr->next = mid_ptr;
  Serial.println("Tail created and pointer stored.");

  snakeLength = 3;

  lightSnake();
  Serial.println("Snake lit.");

  createFood();
  Serial.println("Food created and lit.");

}

void loop() {
  if(gameOver){
    delay(10000);
  } else {
  
    int newX;
    int newY;

 
    //new head in the new direction
    if(currDirection == left){
      newX = head_ptr->x-1;
      newY = head_ptr->y;
      Serial.println("\nSnake moving left!");
    } else if(currDirection == right){
      newX = head_ptr->x+1;
      newY = head_ptr->y;
      Serial.println("\nSnake moving right!");
    } else if(currDirection==up){
      newX = head_ptr->x;
      newY = head_ptr->y-1;
      Serial.println("\nSnake moving up!");
    } else if(currDirection==down){
      newX = head_ptr->x;
      newY = head_ptr->y+1;
      Serial.println("\nSnake moving down!");
    }

    struct Segment *newHead_ptr = (Segment*) malloc(sizeof(struct Segment));
    newHead_ptr->x = newX;
    newHead_ptr->y = newY;
    newHead_ptr->next = NULL;

    head_ptr->next = newHead_ptr;
    head_ptr = newHead_ptr;

    Serial.println("Snake has moved. New head at: ");
    Serial.println(head_ptr->x);
    Serial.println(head_ptr->y);
    
   
    if(isCollision()){
      gameOver = true;
      gameOverDisp();
      Serial.println("Collision! Game over!");
    }
    
    //if the new head is on food, don't decrease length
    if (head_ptr->x == currFood.x and head_ptr->y == currFood.y){
      snakeLength += 1;
      moveDelay -= 250;
      moveDelay = max(moveDelay, 400);
      createFood();
    } else {
      matrix.drawPixel(tail_ptr->x, tail_ptr->y, 0);
      struct Segment *newTail_ptr = tail_ptr->next;
      free(tail_ptr);
      tail_ptr = newTail_ptr;
    }
  
    lightSnake();
    gameDelay(moveDelay);
  }
}

void gameOverDisp(){
  for(int x=0; x>=-255; x--){
    matrix.clear();
    matrix.setCursor(x, 0); 
    matrix.print("GAMEOVER");
    delay(50);
  }
  delay(10000);
}

//make the food blink while we're waiting
void gameDelay(int ms){
  while(ms > 0){
    matrix.drawPixel(currFood.x, currFood.y, 0);
    pollDelay(200);
    ms -= 200;
    matrix.drawPixel(currFood.x, currFood.y, 255);
    pollDelay(300);
    ms -= 300;
  }
}

//poll controller input during the wait
void pollDelay(int ms){
  while(ms > 0){
    updateDirection();
    delay(10);
    ms -= 10;
  }
}

//take control input and change direction
void updateDirection(){

  if(analogRead(JOYSTICK_Y) < 400){
    currDirection = left;
  } else if(analogRead(JOYSTICK_Y) > 600){
    currDirection = right;
  } else if(analogRead(JOYSTICK_X) < 400){
    currDirection = down;
  } else if(analogRead(JOYSTICK_X) > 600){
    currDirection = up;
  }

}

//light the tail dimly, the head brightly
void lightSnake(){
  
  struct Segment *currSeg = tail_ptr;
  for(int i=0; i<snakeLength; i++){
    matrix.drawPixel(currSeg->x, currSeg->y, (i+1)*(255/snakeLength));
    currSeg = currSeg->next;
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
    struct Segment *currSegment = tail_ptr;
    for(int i=0; i<snakeLength; i++){  
      if(currSegment->x == x and currSegment->y == y){
        goodPoint = false;
      }
    currSegment = currSegment->next; 
    }

  Serial.println("New food created! Location: ");
  Serial.print(x);
  Serial.print(y);
  }     
  
  matrix.drawPixel(currFood.x, currFood.y, 0);
  currFood = {x,y};
  matrix.drawPixel(currFood.x, currFood.y, 255);

}

//detect if the snake's head collided with a wall or its body
bool isCollision(){
  if(head_ptr->x < 0 or head_ptr->x > 8 or head_ptr->y < 0 or head_ptr->y > 15){
    return true;
  }
 
  struct Segment *currSeg = tail_ptr; 

  for(int i=0; i<snakeLength-1; i++){
    if(currSeg->x == head_ptr->x and currSeg->y == head_ptr->y){
       return true;
    }
    currSeg = currSeg->next;
  }

  return false;
}

