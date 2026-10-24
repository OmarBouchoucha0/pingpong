#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>

static int VELOCITY = 800;
static const int INITIAL_BALL_VELOCITY = 500;
static const int MAX_BALL_VELOCITY = 2000;
static int BALL_VELOCITY = INITIAL_BALL_VELOCITY;
static int WINDOW_WIDTH = 1600;
static int WINDOW_HEIGHT = 900;
static int PLAYER1_GOALS = 0;
static int PLAYER2_GOALS = 0;
static bool GOAL_SCORED = false;
static int GOALS_TO_WIN = 1;

typedef enum BallDirectionX BallDirectionX;
enum BallDirectionX {
  RIGHT,
  LEFT,
};

static BallDirectionX BALL_DIRECTION_X = RIGHT;

typedef enum BallDirectionY BallDirectionY;
enum BallDirectionY {
  UP,
  DOWN,
};
static BallDirectionY BALL_DIRECTION_Y = UP;

typedef enum GameState GameState;
enum GameState {
  MENU,
  GAME,
  GAME_OVER,
};
static GameState GAME_STATE = MENU;

typedef enum MenuOptions MenuOptions;
enum MenuOptions {
  START,
  QUIT,
};

void resetGameState(Rectangle *player1, Rectangle *player2, Rectangle *ball) {
  GAME_STATE = GAME;
  int player_width = WINDOW_WIDTH / 100;
  int player_height = WINDOW_HEIGHT / 5;
  int player_offsetx = 10;
  int player_offsety = WINDOW_HEIGHT / 2 - player_height / 2;

  player1->x = player_offsetx;
  player1->y = player_offsety;

  player2->x = WINDOW_WIDTH - player_offsetx - player_width;
  player2->y = player_offsety;

  ball->x = WINDOW_WIDTH / 2 - ball->width / 2;
  ball->y = WINDOW_HEIGHT / 2 - ball->height / 2;

  PLAYER1_GOALS = 0;
  PLAYER2_GOALS = 0;
  BALL_VELOCITY = INITIAL_BALL_VELOCITY;
}

void input(float dt, Rectangle *player1, Rectangle *player2, Rectangle *ball) {
  switch (GAME_STATE) {
  case MENU: {
    if (IsKeyDown(KEY_R)) {
      GAME_STATE = GAME;
    }
  } break;
  case GAME: {

    if (IsKeyDown(KEY_UP) && player2->y > 0) {
      player2->y -= VELOCITY * dt;
    }
    if (IsKeyDown(KEY_DOWN) && player2->y + player2->height < WINDOW_HEIGHT) {
      player2->y += VELOCITY * dt;
    }
    if (IsKeyDown(KEY_W) && player1->y > 0) {
      player1->y -= VELOCITY * dt;
    }
    if (IsKeyDown(KEY_S) && player1->y + player1->height < WINDOW_HEIGHT) {
      player1->y += VELOCITY * dt;
    }
  } break;
  case GAME_OVER: {
    if (IsKeyDown(KEY_R)) {
      resetGameState(player1, player2, ball);
    }
  } break;
  }
}

void physics(float dt, Rectangle *ball, Rectangle player1, Rectangle player2) {
  Vector2 dp = {0};
  if (CheckCollisionRecs(*ball, player1)) {
    BALL_DIRECTION_X = RIGHT;
  }
  if (CheckCollisionRecs(*ball, player2)) {
    BALL_DIRECTION_X = LEFT;
  }

  if (ball->x + ball->width > WINDOW_WIDTH) {
    PLAYER1_GOALS += 1;
    ball->x = WINDOW_WIDTH / 2 - ball->width / 2;
    ball->y = WINDOW_HEIGHT / 2 - ball->height / 2;
    GOAL_SCORED = true;
    BALL_VELOCITY = INITIAL_BALL_VELOCITY;
    if (PLAYER1_GOALS >= GOALS_TO_WIN || PLAYER2_GOALS >= GOALS_TO_WIN) {
      GAME_STATE = GAME_OVER;
    }
  }
  if (ball->x < 0) {
    PLAYER2_GOALS += 1;
    ball->x = WINDOW_WIDTH / 2 - ball->width / 2;
    ball->y = WINDOW_HEIGHT / 2 - ball->height / 2;
    GOAL_SCORED = true;
    BALL_VELOCITY = INITIAL_BALL_VELOCITY;
    if (PLAYER1_GOALS >= GOALS_TO_WIN || PLAYER2_GOALS >= GOALS_TO_WIN) {
      GAME_STATE = GAME_OVER;
    }
  }

  if (BALL_DIRECTION_X == RIGHT) {
    dp.x += 1;
  }
  if (BALL_DIRECTION_X == LEFT) {
    dp.x -= 1;
  }

  if (ball->y + ball->height > WINDOW_HEIGHT) {
    BALL_DIRECTION_Y = UP;
  }
  if (ball->y < 0) {
    BALL_DIRECTION_Y = DOWN;
  }
  if (BALL_DIRECTION_Y == DOWN) {
    dp.y += 1;
  }
  if (BALL_DIRECTION_Y == UP) {
    dp.y -= 1;
  }
  dp = Vector2Normalize(dp);
  ball->x += dp.x * dt * BALL_VELOCITY;
  ball->y += dp.y * dt * BALL_VELOCITY;

  if (BALL_VELOCITY <= MAX_BALL_VELOCITY) {
    BALL_VELOCITY += 2000 * dt;
  }
}

int main() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ping pong");
  ToggleFullscreen();
  WINDOW_WIDTH = GetScreenWidth();
  WINDOW_HEIGHT = GetScreenHeight();
  int player_width = WINDOW_WIDTH / 100;
  int player_height = WINDOW_HEIGHT / 5;
  int player_offsetx = 10;
  int player_offsety = WINDOW_HEIGHT / 2 - player_height / 2;

  VELOCITY = WINDOW_HEIGHT / 2;

  Rectangle player1 = {0};
  player1.width = player_width;
  player1.height = player_height;
  player1.x = player_offsetx;
  player1.y = player_offsety;

  Rectangle player2 = {0};
  player2.width = player_width;
  player2.height = player_height;
  player2.x = WINDOW_WIDTH - player_offsetx - player_width;
  player2.y = player_offsety;

  Rectangle ball = {0};
  ball.height = player_width;
  ball.width = player_width;
  ball.x = WINDOW_WIDTH / 2 - ball.width / 2;
  ball.y = WINDOW_HEIGHT / 2 - ball.height / 2;

  Rectangle GoalCelebration = {0};
  GoalCelebration.x = 0;
  GoalCelebration.y = 0;
  GoalCelebration.width = WINDOW_WIDTH;
  GoalCelebration.height = WINDOW_HEIGHT;

  float dt;
  char info_buffer[100];
  float celebration_time = 0;
  float text_flashing_interval = 0;
  while (!WindowShouldClose()) {
    dt = GetFrameTime();

    input(dt, &player1, &player2, &ball);
    if (!GOAL_SCORED) {
      physics(dt, &ball, player1, player2);
    }
    BeginDrawing();
    ClearBackground(BLACK);

    switch (GAME_STATE) {
    case MENU: {
      DrawRectangleLinesEx(GoalCelebration, 5, RAYWHITE);
      snprintf(info_buffer, sizeof(info_buffer), "PING PONG");
      DrawText(info_buffer, (WINDOW_WIDTH - MeasureText(info_buffer, 128)) / 2,
               WINDOW_HEIGHT / 5, 128, RAYWHITE);

      snprintf(info_buffer, sizeof(info_buffer), "PRESS \"R\" TO START");
      DrawText(info_buffer, (WINDOW_WIDTH - MeasureText(info_buffer, 32)) / 2,
               WINDOW_HEIGHT / 2 - 16, 32, RAYWHITE);
      snprintf(info_buffer, sizeof(info_buffer), "PRESS \"ESC\" TO QUIT");
      DrawText(info_buffer, (WINDOW_WIDTH - MeasureText(info_buffer, 32)) / 2,
               WINDOW_HEIGHT / 2 + 16, 32, RAYWHITE);
    } break;
    case GAME: {
      DrawRectangle(player1.x, player1.y, player1.width, player1.height,
                    RAYWHITE);
      DrawRectangle(player2.x, player2.y, player2.width, player2.height,
                    RAYWHITE);
      DrawRectangle(ball.x, ball.y, ball.width, ball.height, RAYWHITE);
      if (GOAL_SCORED) {
        celebration_time += dt;
        DrawRectangleLinesEx(GoalCelebration, 5, RAYWHITE);
        if (celebration_time >= 0.5) {
          celebration_time = 0;
          GOAL_SCORED = false;
        }
      }

      DrawLine(WINDOW_WIDTH / 2, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT, RAYWHITE);
      snprintf(info_buffer, sizeof(info_buffer), "%d", PLAYER1_GOALS);
      DrawText(info_buffer, WINDOW_WIDTH / 4, 30, 32, RAYWHITE);
      snprintf(info_buffer, sizeof(info_buffer), "%d", PLAYER2_GOALS);
      DrawText(info_buffer, WINDOW_WIDTH * 3 / 4, 30, 32, RAYWHITE);
    } break;
    case GAME_OVER: {
      DrawRectangleLinesEx(GoalCelebration, 5, RAYWHITE);
      snprintf(info_buffer, sizeof(info_buffer), "GAME OVER");
      DrawText(info_buffer, (WINDOW_WIDTH - MeasureText(info_buffer, 128)) / 2,
               WINDOW_HEIGHT / 5, 128, RAYWHITE);
      text_flashing_interval += dt;
      if (text_flashing_interval > 0.5f) {
        if (PLAYER1_GOALS >= GOALS_TO_WIN) {
          snprintf(info_buffer, sizeof(info_buffer), "PLAYER1 WINS");

          DrawText(info_buffer,
                   (WINDOW_WIDTH - MeasureText(info_buffer, 48)) / 2,
                   WINDOW_HEIGHT / 2, 48, WHITE);
        }
        if (PLAYER2_GOALS >= GOALS_TO_WIN) {
          snprintf(info_buffer, sizeof(info_buffer), "PLAYER2 WINS");
          DrawText(info_buffer,
                   (WINDOW_WIDTH - MeasureText(info_buffer, 48)) / 2,
                   WINDOW_HEIGHT / 2, 48, WHITE);
        }
      }
      if (text_flashing_interval > 1.0f) {
        text_flashing_interval = 0;
      }

      snprintf(info_buffer, sizeof(info_buffer), "PRESS \"R\" TO RESET");
      DrawText(info_buffer, (WINDOW_WIDTH - MeasureText(info_buffer, 32)) / 2,
               WINDOW_HEIGHT * 3 / 4, 32, RAYWHITE);
      snprintf(info_buffer, sizeof(info_buffer), "PRESS \"ESC\" TO RESET");
      DrawText(info_buffer, (WINDOW_WIDTH - MeasureText(info_buffer, 32)) / 2,
               WINDOW_HEIGHT * 3 / 4 + 32, 32, RAYWHITE);

    } break;
    }
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
