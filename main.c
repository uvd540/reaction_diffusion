#include "raylib.h"
#include "raymath.h"

#define SCREEN_SIZE 800
#define GRID_SIZE 200

// convolution weights
float weight_center = -1.0f;
float weight_cardinal = 0.2f;
float weight_diagonal = 0.05f;

// diffusion coefficients
float diff_a = 1.0f;
float diff_b = 0.5f;

float f = 0.055f; // feed rate
float k = 0.062f; // kill rate

float dt = 1.0f; // time step

float get_value(float *component, int x, int y) {
  return component[x * GRID_SIZE + y];
}

void set_value(float *component, int x, int y, float value) {
  component[x * GRID_SIZE + y] = value;
}

float laplacian(float *component, int x, int y) {
  return weight_center * get_value(component, x, y) +
         weight_cardinal * get_value(component, x - 1, y) +
         weight_cardinal * get_value(component, x + 1, y) +
         weight_cardinal * get_value(component, x, y - 1) +
         weight_cardinal * get_value(component, x, y + 1) +
         weight_diagonal * get_value(component, x - 1, y - 1) +
         weight_diagonal * get_value(component, x - 1, y + 1) +
         weight_diagonal * get_value(component, x + 1, y - 1) +
         weight_diagonal * get_value(component, x + 1, y + 1);
}

int main() {
  Camera2D camera = {0};
  camera.zoom = 4; // scale simulation world to window
  float a[GRID_SIZE * GRID_SIZE] = {0};
  float b[GRID_SIZE * GRID_SIZE] = {0};
  float a_temp[GRID_SIZE * GRID_SIZE] = {0};
  float b_temp[GRID_SIZE * GRID_SIZE] = {0};
  for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
    a[i] = 1.0f;
    a_temp[i] = 1.0f;
  }
  for (int x = 95; x < 105; x++) {
    for (int y = 95; y < 105; y++) {
      set_value(b, x, y, 1.0f);
    }
  }
  InitWindow(SCREEN_SIZE, SCREEN_SIZE, "reaction_diffusion");
  while (!WindowShouldClose()) {
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
      int x = i / GRID_SIZE;
      int y = i % GRID_SIZE;
      if (x == 0 || x == GRID_SIZE - 1)
        continue;
      if (y == 0 || y == GRID_SIZE - 1)
        continue;
      a_temp[i] = Clamp(a[i] + ((diff_a * laplacian(a, x, y)) -
                                (a[i] * b[i] * b[i]) + (f * (1 - a[i]))) *
                                   dt,
                        0.0f, 1.0f);
      b_temp[i] = Clamp(b[i] + ((diff_b * laplacian(b, x, y)) +
                                (a[i] * b[i] * b[i]) - ((k + f) * b[i])) *
                                   dt,
                        0.0f, 1.0f);
    }
    BeginDrawing();
    BeginMode2D(camera);
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
      a[i] = a_temp[i];
      b[i] = b_temp[i];
      int colorValue = Clamp(floor((a[i] - b[i]) * 255), 0, 255);
      Color color = (Color){colorValue, colorValue, colorValue, 255};
      int x = i / GRID_SIZE;
      int y = i % GRID_SIZE;
      DrawPixel(x, y, color);
    }
    EndMode2D();
    DrawFPS(10, 10);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
