struct Point {
    int x;
    int y;
};

enum Direction {
    UP,
    DOWN,
    RIGHT,
    LEFT
};

class Snake {
    public:
        Snake(int maxBoardX, int maxBoardY, int startX, int startY){
            int initialX = startX;
            int initialY = startY;
            
            position[0].x = initialX;
            position[0].y = initialY;
            
            position[1].x = initialX + 1;
            position[1].y = initialY;
            
            position[2].x = initialX + 2;
            position[2].y = initialY;
            
            size = 3;
            direction = LEFT;
            maxX = maxBoardX;
            maxY = maxBoardY;
            
            for (int i = 3; i < maxSize; i++) {
                position[i].x = 0;
                position[i].y = 0;
            }
        }

        int getSize() {
          return size;
        }

        Point getPosition(int index) {
            return position[index];
        }

        int getMaxX() {
            return maxX;
        }

        int getMaxY() {
            return maxY;
        }

        Direction getDirection() {
            return direction;
        }

        void setDirection(Direction newDirection) {
            if (direction == UP && newDirection == DOWN ||
                direction == DOWN && newDirection == UP ||
                direction == RIGHT && newDirection == LEFT ||
                direction == LEFT && newDirection == RIGHT)
            {
                return;
            }
            direction = newDirection;
        }

        void update() {
            for (int i = size - 1; i > 0; i--) {
                position[i] = position[i - 1];
            }

          switch(direction) {
            case UP:
                position[0].y = (position[0].y + 1) % maxY;
                break;
            case DOWN:
                position[0].y = (position[0].y - 1 + maxY) % maxY;
                break;
            case RIGHT:
                position[0].x = (position[0].x + 1) % maxX;
                break;
            case LEFT:
                position[0].x = (position[0].x - 1 + maxX) % maxX;
                break;
          }
        }

        void grow() {
            if (size < maxSize) {
                size++;
            }
        }
        
        bool containsPoint(Point &point) {
            for (int i = 0; i < size; i++) {
                if (point.x == position[i].x && point.y == position[i].y) {
                    return true;
                }
            }
            return false;
        }

        bool selfIntersects() {
            Point head = position[0];
            for (int i = 1; i < size; i++) {
                if (head.x == position[i].x && head.y == position[i].y) {
                    return true;
                }
            }
            return false;
        }

        bool intersectsWith(Snake otherSnake){
          Point head = position[0];
            for (int i = 0; i < size; i++) {
                if (head.x == otherSnake.position[i].x && head.y == otherSnake.position[i].y) {
                    return true;
                }
            }
            return false;
        }
        
        static const int maxSize = 32;
        
        Point position[maxSize];
        
    private:
        Direction direction;
        int size;
        int maxX;
        int maxY;
};

enum GameState {
    RUNNING,
    FINISHED,
    FINISHED_TIE
};

class Game {
    public:
        Game(int maxBoardX, int maxBoardY): 
        snake(maxBoardX, maxBoardY, 0, 0),
        first_snake(maxBoardX, maxBoardY, 6, 6),
        second_snake(maxBoardX, maxBoardY, 4, 4)
        {
            maxX = maxBoardX;
            maxY = maxBoardY;
            food = generateFood();
            state = RUNNING;
            winnerNumber=0;
        }
        
        void update() {
            if (isRunning()) {
                if (first_snake.containsPoint(food)) {
                    first_snake.grow();
                    food = generateFood();
                }
                if (second_snake.containsPoint(food)) {
                    second_snake.grow();
                    food = generateFood();
                }
                first_snake.update();
                second_snake.update();
                if (first_snake.selfIntersects()||first_snake.intersectsWith(second_snake)) {
                    winnerNumber=2;
                    state = FINISHED;
                }
                if (second_snake.selfIntersects()||second_snake.intersectsWith(first_snake)) {
                    if (winnerNumber!=0)
                    {
                      winnerNumber=0;
                      state=FINISHED_TIE;
                    }
                    else
                    {
                      winnerNumber=1;
                      state = FINISHED;
                    }
                }
            }
        }
        
        Snake &getSnake() {
            return snake;
        }

        Snake &getSnake1() {
            return first_snake;
        }

        Snake &getSnake2() {
            return second_snake;
        }

        int getWinner(){
          return winnerNumber;
        }
        
        void setSnakeDirection(Direction direction) {
            snake.setDirection(direction);
        }

        Point &getFood() {
            return food;
        }
        
        bool isRunning() {
            return state == RUNNING;
        }

        bool isTie() {
            return state == FINISHED_TIE;
        }

        void setSnakeDirection(Direction direction, int snakeIndex) {
          if (snakeIndex==1)
          {
            first_snake.setDirection(direction);
          }
          else
          {
            second_snake.setDirection(direction);
          }
        }

    private:
        Point generateFood() {
            Point result = food;
            result.x = random(0, maxX);
            result.y = random(0, maxY);
            
            while (snake.containsPoint(result)) {
                result.x = (result.x + 1) % maxX;
                result.y = (result.y + 1) % maxY;
            }
            return result;
        }
  
        Snake snake;
        Snake first_snake;
        Snake second_snake;
        Point food;
        GameState state;
        int winnerNumber;
        int maxX;
        int maxY;
};
