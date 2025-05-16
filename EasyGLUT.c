// Библиотека для простой работы с FreeGLUT

// Подключение бибилиотек 
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

// Глобальные переменные для хранения данных

// Переменные для точки
typedef struct {
    float x; 
    float y; 
} Point;

static Point* points = NULL;
static int point_count = 0; // Счётчик количества точек
static float point_size = 7.0f; // Размер по умолчанию
static float point_color[3] = {0.0f, 0.0f, 0.0f}; // Чёрный по умолчанию

// Переменные для линии
typedef struct {
    float xStart, yStart;
    float xFinish, yFinish;
} Line;

static Line* lines = NULL;
static int line_count = 0; // Счётчик количества линей
static float line_width = 5.0f; // Размер по умолчанию
static float line_color[3] = {0.0f, 0.0f, 0.0f}; // Чёрный по умолчанию

// Переменные для окружности
typedef struct {
    float x;      // Центр X
    float y;      // Центр Y
    float radius; // Радиус
} Circle;

static Circle* circles = NULL;
static int circle_count = 0; // Счётчик окружностей
static float circle_width = 5.0f; // Толщина линии по умолчанию
static float circle_color[3] = {0.0f, 0.0f, 0.0f}; // Чёрный по умолчанию

// Переменные для треугольника
typedef struct {
    float x0, y0;
    float x1, y1;
    float x2, y2;
} Triangle;

static Triangle* triangles = NULL;
static int triangle_count = 0;
static float triangle_width = 4.0f;
static float triangle_color[3] = {0.0f, 0.0f, 0.0f};

// Переменные для многоугольников
typedef struct {
    float* x_coords; // Массив X-координат вершин
    float* y_coords; // Массив Y-координат вершин
    int vertex_count; // Количество вершин
} Polygon;

static Polygon* polygons = NULL;
static int polygon_count = 0;
static float polygon_width = 4.0f;
static float polygon_color[3] = {0.0f, 0.0f, 0.0f};

// Переменные для рабочего окна
static float axis_limits[4] = {-1.0f, 1.0f, -1.0f, 1.0f}; // Размеры по умолчанию
static int window_width = 1000; // Размер по умолчанию
static int window_height = 620; // Размер по умолчанию
static int win_pos_x = 500; // Положение окна по умолчанию
static int win_pos_y = 200; // Положение окна по умолчанию

static bool grid_enabled = false; // По умолчанию сетка выключена

// Функции рисующие объекты

// Функция для вывода координат
void draw_coord_label(float x, float y, const char* text) {
    glRasterPos2f(x + 0.03f, y + 0.03f); // Смещение, чтобы текст не накладывался на другие объекты
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
    }
}

// Функция отрисовки
static void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Установка проекции
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(axis_limits[0]-0.5, axis_limits[1]+0.5, axis_limits[2]-0.5, axis_limits[3]+0.5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Сетка (рисуется только если включена)
    if (grid_enabled) {
        glColor3f(0.8f, 0.8f, 0.8f);
        glLineWidth(0.5f);
        glBegin(GL_LINES);
        // Вертикальные линии сетки
        for (float x = axis_limits[0]; x <= axis_limits[1]; x += 1.0f) {
            glVertex2f(x, axis_limits[2]-0.2);
            glVertex2f(x, axis_limits[3]);
        }
        // Горизонтальные линии сетки
        for (float y = axis_limits[2]; y <= axis_limits[3]; y += 1.0f) {
            glVertex2f(axis_limits[0]-0.2, y);
            glVertex2f(axis_limits[1], y);
        }
        glEnd();

        // Координаты осей
        glColor3f(0.0f, 0.0f, 0.0f);
        for (float x = axis_limits[0]; x <= axis_limits[1]; x += 1.0f) {
            char label[10];
            snprintf(label, sizeof(label), "%.1f", x); // Перевод из строки в число
            draw_coord_label(x, axis_limits[2]-0.2f, label);
        }
        for (float y = axis_limits[2]; y <= axis_limits[3]; y += 1.0f) {
            char label[10];
            snprintf(label, sizeof(label), "%.1f", y); // Перевод из строки в число
            draw_coord_label(axis_limits[0]-0.4f, y+0.1f, label);
        } 
    }

    // Оси координат
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(5.0f);
    glBegin(GL_LINES);
        // Ось X
        glVertex2f(axis_limits[0]-0.3, axis_limits[2]);
        glVertex2f(axis_limits[1], axis_limits[2]);

        // Стрелка для оси X
        glVertex2f(axis_limits[1], axis_limits[2]);
        glVertex2f(axis_limits[1]-0.2, axis_limits[2]+0.1);
        glVertex2f(axis_limits[1], axis_limits[2]);
        glVertex2f(axis_limits[1]-0.2, axis_limits[2]-0.1);

        // Ось Y
        glVertex2f(axis_limits[0], axis_limits[2]-0.3);
        glVertex2f(axis_limits[0], axis_limits[3]);

        // Стрелка для оси Y
        glVertex2f(axis_limits[0], axis_limits[3]);
        glVertex2f(axis_limits[0]-0.1, axis_limits[3]-0.2);
        glVertex2f(axis_limits[0], axis_limits[3]);
        glVertex2f(axis_limits[0]+0.1, axis_limits[3]-0.2);
    glEnd();

    // Подпись осей
    glRasterPos2f(axis_limits[1]+0.2, axis_limits[2]);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'X');
    glRasterPos2f(axis_limits[0], axis_limits[3]+0.2);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'Y');

    // Примитивы (приоритет отображения точка, линия, окружность, треугольник, многоугольник)

    // Многоугольники
    glColor3fv(polygon_color);
    glLineWidth(polygon_width);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    for (int i = 0; i < polygon_count; i++) {
        bool all_vertices_visible = true;
        // Проверяем, все ли вершины многоугольника видны
        for (int j = 0; j < polygons[i].vertex_count; j++) {
            if ((polygons[i].x_coords[j] < axis_limits[0]) || (polygons[i].x_coords[j] > axis_limits[1]) ||
                (polygons[i].y_coords[j] < axis_limits[2]) || (polygons[i].y_coords[j] > axis_limits[3])) {
                all_vertices_visible = false;
                break;
            }
        }
        
        if (all_vertices_visible) {
            glBegin(GL_LINE_LOOP);
            for (int j = 0; j < polygons[i].vertex_count; j++) {
                glVertex2f(polygons[i].x_coords[j], polygons[i].y_coords[j]);
            }
            glEnd();
        }
    }

    // Треугольники
    glColor3fv(triangle_color);
    glLineWidth(triangle_width);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    for (int i = 0; i < triangle_count; i++) {
        if ((triangles[i].x0 >= axis_limits[0]) && (triangles[i].x0 <= axis_limits[1]) &&
            (triangles[i].y0 >= axis_limits[2]) && (triangles[i].y0 <= axis_limits[3]) &&
            (triangles[i].x1 >= axis_limits[0]) && (triangles[i].x1 <= axis_limits[1]) &&
            (triangles[i].y1 >= axis_limits[2]) && (triangles[i].y1 <= axis_limits[3]) &&
            (triangles[i].x2 >= axis_limits[0]) && (triangles[i].x2 <= axis_limits[1]) &&
            (triangles[i].y2 >= axis_limits[2]) && (triangles[i].y2 <= axis_limits[3])) {

            glBegin(GL_LINES);
                glVertex2f(triangles[i].x0, triangles[i].y0);
                glVertex2f(triangles[i].x1, triangles[i].y1);
                glVertex2f(triangles[i].x1, triangles[i].y1);
                glVertex2f(triangles[i].x2, triangles[i].y2);
                glVertex2f(triangles[i].x2, triangles[i].y2);
                glVertex2f(triangles[i].x0, triangles[i].y0);
            glEnd();
        }
    }

    // Окружности
    glColor3fv(circle_color);
    glLineWidth(circle_width);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    for (int i = 0; i < circle_count; i++) {
        if ((circles[i].x - circles[i].radius >= axis_limits[0]) && 
            (circles[i].x + circles[i].radius <= axis_limits[1]) &&
            (circles[i].y - circles[i].radius >= axis_limits[2]) && 
            (circles[i].y + circles[i].radius <= axis_limits[3])) {
            
            const float PI = 3.14159265358979323846f;
            const int segments = 360;
            
            glBegin(GL_LINE_LOOP);
                for (int j = 0; j < segments; j++) {
                    float angle = 2.0f * PI * j / segments;  
                    float dx = circles[i].radius * cosf(angle);  
                    float dy = circles[i].radius * sinf(angle);
                    glVertex2f(circles[i].x + dx, circles[i].y + dy);
                }
            glEnd();
        }
    }

    // Линии
    glColor3fv(line_color);
    glLineWidth(line_width);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    for (int i = 0; i < line_count; i++) {
        if ((lines[i].xStart >= axis_limits[0]) && (lines[i].xStart <= axis_limits[1]) &&
            (lines[i].xFinish >= axis_limits[0]) && (lines[i].xFinish <= axis_limits[1]) &&
            (lines[i].yStart >= axis_limits[2]) && (lines[i].yStart <= axis_limits[3]) &&
            (lines[i].yFinish >= axis_limits[2]) && (lines[i].yFinish <= axis_limits[3])) {
            glBegin(GL_LINES);
                glVertex2f(lines[i].xStart, lines[i].yStart);
                glVertex2f(lines[i].xFinish, lines[i].yFinish);
            glEnd();
        }
    }

    // Точки
    glColor3fv(point_color);
    glPointSize(point_size);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    for (int i = 0; i < point_count; i++) {
        if ((points[i].x >= axis_limits[0]) && (points[i].x <= axis_limits[1]) &&
            (points[i].y >= axis_limits[2]) && (points[i].y <= axis_limits[3])) {
            glBegin(GL_POINTS);
                glVertex2f(points[i].x, points[i].y);
            glEnd();
        }
    }

    glutSwapBuffers();
}

// Изменение размера окна
void window_position(int x_position, int y_position) {
    win_pos_x = x_position;
    win_pos_y = y_position;
}

void window(int width, int height) {
    window_width = width;
    window_height = height;
}

// Инициализация системы
void axis(float xmin, float xmax, float ymin, float ymax) {
    axis_limits[0] = xmin;
    axis_limits[1] = xmax;
    axis_limits[2] = ymin;
    axis_limits[3] = ymax;

    int argc = 1;
    char* argv[1] = {(char*)"easy"};
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(win_pos_x, win_pos_y);
    glutCreateWindow("EasyGLUT");
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glutDisplayFunc(display);
}

// Видимость сетки
void grid_visibility(const char* choice) {
    if (strcmp(choice, "ON") == 0 || strcmp(choice, "on") == 0 || strcmp(choice, "1") == 0) {
        grid_enabled = true;
    } else if (strcmp(choice, "OFF") == 0 || strcmp(choice, "off") == 0 || strcmp(choice, "0") == 0) {
        grid_enabled = false;
    } else {
        fprintf(stderr, "Ошибка: некорректный аргумент grid_visibility(). Ожидается 'ON' или 'OFF'.\n");
        return;
    }
    glutPostRedisplay();
}

// Добавление точки
void point(float x, float y) {
    Point* temp = (Point*)realloc(points, (point_count + 1) * sizeof(Point));
    if (temp == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для точки.\n");
        return;
    }
    points = temp;
    points[point_count].x = x;
    points[point_count].y = y;
    point_count++;
    glutPostRedisplay();
}

// Установка размера точки
void set_point_size(float size) {
    point_size = size;
}

// Установка цвета точек
void set_point_color(int r, int g, int b) {
    point_color[0] = r/255.0f;
    point_color[1] = g/255.0f;
    point_color[2] = b/255.0f;
}

// Добавление линии
void line(float xStart, float yStart, float xFinish, float yFinish) {
    Line* temp = (Line*)realloc(lines, (line_count + 1) * sizeof(Line));
    if (temp == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для линии.\n");
        return;
    }
    lines = temp;
    lines[line_count].xStart = xStart;
    lines[line_count].yStart = yStart;
    lines[line_count].xFinish = xFinish;
    lines[line_count].yFinish = yFinish;
    line_count++;
    glutPostRedisplay();
}

// Установка размера линии
void set_line_width(float width) {
    line_width = width;
}

// Установка цвета линий
void set_line_color(int r, int g, int b) {
    line_color[0] = r/255.0f;
    line_color[1] = g/255.0f;
    line_color[2] = b/255.0f;
}

// Добавление окружности
void circle(float x, float y, float radius) {
    Circle* temp = (Circle*)realloc(circles, (circle_count + 1) * sizeof(Circle));
    if (temp == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для окружности.\n");
        return;
    }
    circles = temp;
    circles[circle_count].x = x;
    circles[circle_count].y = y;
    circles[circle_count].radius = radius;
    circle_count++;
    glutPostRedisplay();
}

// Установка толщины линии окружности
void set_circle_width(float width) {
    circle_width = width;
}

// Установка цвета окружности
void set_circle_color(int r, int g, int b) {
    circle_color[0] = r / 255.0f;
    circle_color[1] = g / 255.0f;
    circle_color[2] = b / 255.0f;
}

// Добавление треугольника
void triangle(float x0, float y0, float x1, float y1, float x2, float y2) {
    Triangle* temp = (Triangle*)realloc(triangles, (triangle_count + 1) * sizeof(Triangle));
    if (temp == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для треугольника.\n");
        return;
    }
    triangles = temp;
    triangles[triangle_count].x0 = x0;
    triangles[triangle_count].y0 = y0;
    triangles[triangle_count].x1 = x1;
    triangles[triangle_count].y1 = y1;
    triangles[triangle_count].x2 = x2;
    triangles[triangle_count].y2 = y2;
    triangle_count++;
    glutPostRedisplay();
}

// Установка толщины линии треугольника
void set_triangle_width(float width) {
    triangle_width = width;
}

// Установка цвета треугольника
void set_triangle_color(int r, int g, int b) {
    triangle_color[0] = r / 255.0f;
    triangle_color[1] = g / 255.0f;
    triangle_color[2] = b / 255.0f;
}

// Добавление многоугольника
void polygon(float* x_coords, float* y_coords, int vertex_count) {
    if (vertex_count < 3) {
        fprintf(stderr, "Ошибка: многоугольник должен иметь минимум 3 вершины.\n");
        return;
    }

    Polygon* temp = (Polygon*)realloc(polygons, (polygon_count + 1) * sizeof(Polygon));
    if (temp == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для многоугольника.\n");
        return;
    }
    polygons = temp;

    // Выделяем память для координат этого многоугольника
    polygons[polygon_count].x_coords = (float*)malloc(vertex_count * sizeof(float));
    polygons[polygon_count].y_coords = (float*)malloc(vertex_count * sizeof(float));
    
    if (polygons[polygon_count].x_coords == NULL || polygons[polygon_count].y_coords == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для координат многоугольника.\n");
        return;
    }

    // Копируем координаты
    memcpy(polygons[polygon_count].x_coords, x_coords, vertex_count * sizeof(float));
    memcpy(polygons[polygon_count].y_coords, y_coords, vertex_count * sizeof(float));
    polygons[polygon_count].vertex_count = vertex_count;
    polygon_count++;
    glutPostRedisplay();
}

// Установка толщины линии многоугольника
void set_polygon_width(float width) {
    polygon_width = width;
}

// Установка цвета многоугольника
void set_polygon_color(int r, int g, int b) {
    polygon_color[0] = r / 255.0f;
    polygon_color[1] = g / 255.0f;
    polygon_color[2] = b / 255.0f;
}

// Вывод графиков (запуск обработчика событий)
void show() {
    glutMainLoop();
}