#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h> // Include math.h for sin() and exp()
#include <time.h>
#include <stdlib.h>

typedef struct Coordinate
{
    int x;
    int y;
} coord_t;

typedef struct Wave_t
{
    int nx;
    int ny;
    float c;
    int h;
    int dt;
    int user_mur_abc;
} wave_t;

float sigmoid(float x)
{
    return 1.0f / (1.0f + exp(-x));
}

void print_matrix(int **matrix, int n_cells)
{
    int max_display = 6; // Show first 3 and last 3 rows/cols if matrix is large
    int truncate = n_cells > max_display;

    printf("tensor([");

    for (int i = 0; i < n_cells; i++)
    {
        // Skip middle rows if truncating
        if (truncate && i == 3 && n_cells > max_display)
        {
            printf("        ...,\n");
            i = n_cells - 3; // Jump to last 3 rows
            if (i < 3)
                i = 3; // Safety check
        }

        if (i == 0)
            printf("[");
        else
            printf("        [");

        for (int j = 0; j < n_cells; j++)
        {
            // Skip middle columns if truncating
            if (truncate && j == 3 && n_cells > max_display)
            {
                printf(" ...,");
                j = n_cells - 3; // Jump to last 3 columns
                if (j < 3)
                    j = 3; // Safety check
            }

            printf("%4d", matrix[i][j]);
            if (j < n_cells - 1)
                printf(",");
        }

        if (i < n_cells - 1)
            printf("],\n");
        else
            printf("]])\n");
    }
    printf("\n");
}

void copy_matrix(int **src, int **dest, int n_cells)
{
    for (int i = 0; i < n_cells; i++)
    {
        for (int j = 0; j < n_cells; j++)
        {
            dest[i][j] = src[i][j];
        }
    }
}

void drawGrid(int n_cells, int screen_size)
{
    Color lineColor = {0, 0, 0, 0};
    int cell_size = screen_size / n_cells;
    for (int i = 0; i < n_cells; i++)
    {
        DrawLine(0, i * cell_size, screen_size, i * cell_size, lineColor);
        DrawLine(i * cell_size, 0, i * cell_size, screen_size, lineColor);
    }
}

void showFPS(int show_fps, int fontSize, int screenSize)
{
    if (show_fps == 1)
    {
        int fps = GetFPS();
        const char *fpsText = TextFormat("FPS: %d", fps);
        int fpsTextWidth = MeasureText(fpsText, fontSize);
        DrawText(fpsText, screenSize - fpsTextWidth - 10, 10, fontSize, GREEN);
    }
}

int **init_2d_matrix(int n_cells)
{
    int **m = (int **)malloc(n_cells * sizeof(int *));
    if (m == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < n_cells; i++)
    {
        m[i] = (int *)malloc(n_cells * sizeof(int));
        if (m[i] == NULL)
        {
            for (int j = 0; j < i; i++)
            {
                free(m[j]);
            }
            free(m);
            return NULL;
        }

        for (int j = 0; j < n_cells; j++)
        {
            m[i][j] = 0;
        }
    }

    return m;
}

coord_t getCoordinates(int mouse_x, int mouse_y, int cell_size)
{
    coord_t mouse_pos = {0, 0};
    mouse_pos.x = mouse_x / cell_size;
    mouse_pos.y = mouse_y / cell_size;
    return mouse_pos;
}

int normalize(float value, float min_val, float max_val)
{
    if (max_val == min_val)
        return 128; // Return middle value if range is zero
    // printf("%f\n", value);
    float normalized = (value - min_val) / (max_val - min_val) * 255.0f;

    // Clamp to 0-255 range
    if (normalized < 0)
        return 0;
    if (normalized > 255)
        return 255;

    return (int)normalized;
}

void drawCells(int **m, int cell_num, int cell_size)
{

    for (int i = 0; i < cell_num; i++)
    {
        for (int j = 0; j < cell_num; j++)
        {
            int norm_color = normalize(m[i][j], 0, 255);
            // printf("%d\n", norm_color);
            Color color = {0, 0, 0, norm_color};
            DrawRectangle(i * cell_size, j * cell_size, cell_size, cell_size, color);
        }
    }
}

int **calculate_next(wave_t wave, int **prev, int **curr, int **next, int n_cells)
{
    float alpha = (wave.c * wave.c) / (wave.h * wave.h);
    for (int i = 1; i < n_cells - 1; i++)
    {
        for (int j = 1; j < n_cells - 1; j++)
        {
            next[i][j] = (alpha * alpha) * (curr[i + 1][j] + curr[i - 1][j] + curr[i][j + 1] + curr[i][j - 1] - 4 * curr[i][j]) + 2 * curr[i][j] - prev[i][j];
        }
    }

    return next;
}

void update(wave_t wave, int **prev, int **curr, int **next, int n_cells)
{
    copy_matrix(curr, prev, n_cells);
    copy_matrix(next, curr, n_cells);
    copy_matrix(calculate_next(wave, prev, curr, next, n_cells), next, n_cells);
}

int main(void)
{
    srand(time(NULL)); // Initialization, should only be called once.

    const int screen_size = 500;
    const char *text = "Wave simulation by stun100";
    const int fontSize = 20;
    const int cell_num = 250;
    int **m = init_2d_matrix(cell_num);
    int **prev = init_2d_matrix(cell_num);
    int **curr = init_2d_matrix(cell_num);

    for (int i = 0; i < cell_num; i++)
    {
        for (int j = 0; j < cell_num; j++)
        {
            int r = rand();
            curr[i][j] = (10 * j / r);
        }
    }
    int **next = init_2d_matrix(cell_num);

    // Initialize with sinusoidal wave at center
    // init_sinusoidal_wave(curr, cell_num, 1.0f, 0.5f);
    // copy_matrix(curr, prev, cell_num); // Set previous state same as current

    const int cell_size = screen_size / cell_num;
    wave_t wave = {cell_num, cell_num, 0.2, 1, 1, 0};
    InitWindow(screen_size, screen_size, "Wave simulation");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        int textWidth = MeasureText(text, fontSize);
        int textX = (screen_size - textWidth) / 2;
        int textY = (screen_size - fontSize) / 2;

        // Color textColor = {0, 0, 0, 100};
        // DrawText(text, textX, textY, fontSize, textColor);

        // coord_t mouse_pos = getCoordinates(GetMouseX(), GetMouseY(), screen_size / cell_num);
        // // DrawRectangle(mouse_pos.x * cell_size, mouse_pos.y * cell_size, cell_size, cell_size, RED);
        // if (mouse_pos.x < cell_num && mouse_pos.y < cell_num)
        // {
        //     m[mouse_pos.x][mouse_pos.y] = 1;
        // }

        drawCells(curr, cell_num, cell_size);
        drawGrid(cell_num, screen_size);
        update(wave, prev, curr, next, cell_num);
        // print_matrix(curr, cell_num);
        // Draw FPS in upper right corner
        showFPS(1, 25, screen_size);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
