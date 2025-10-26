// Библиотека для простой работы с FreeGLUT

// Подключение бибилиотек //
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

// Глобальные переменные для хранения данных //

// Переменные для Точки
typedef struct {
    float x;
    float y;
    float color[3];
    float size;
} Point;

static Point* PointS = NULL;
static int PointCount = 0;

// Переменные для Линии
typedef struct {
    float xStart, yStart;
    float xFinish, yFinish;
    float color[3];
    float width;
} Line;

static Line* LineS = NULL;
static int LineCount = 0;

// Переменные для ломанных линмй
typedef struct {
    float* x_coords;
    float* y_coords;
    int vertex_count;
    float color[3];
    float width;
} BrokenLine;

static BrokenLine* BrokenLineS = NULL;
static int BrokenLineCount = 0;

// Переменные для Векторов/Стрелок
typedef struct {
    float xStart, yStart;
    float xFinish, yFinish;
    float color[3];
    float width;
} Arrow;

static Arrow* ArrowS = NULL;
static int ArrowCount = 0;

// Переменные для Окружности
typedef struct {
    float x;      // Центр X
    float y;      // Центр Y
    float radius; // Радиус
    float color[3];
    float width;
} Circle;

static Circle* CircleS = NULL;
static int CircleCount = 0;

// Переменные для Еллипса
typedef struct {
    float x;
    float y;
    float a;
    float b;
    float color[3];
    float width;
} Ellipse;

static Ellipse* EllipseS = NULL;
static int EllipseCount = 0;

// Переменные для треугольника
typedef struct {
    float x0, y0;
    float x1, y1;
    float x2, y2;
    float color[3];
    float width;
} Triangle;

static Triangle* TriangleS = NULL;
static int TriangleCount = 0;

// Переменные для многоугольников
typedef struct {
    float* x_coords;
    float* y_coords;
    int vertex_count;
    float color[3];
    float width;
} Polygon;

static Polygon* PolygonS = NULL;
static int PolygonCount = 0;

// === === === === === === === === === === === === === === === === === === === ===

// Переменные для рабочего окна
static float axis_limits[4] = {-1.0f, 1.0f, -1.0f, 1.0f};
static int window_width = 1000;
static int window_height = 620;
static int win_pos_x = 500;
static int win_pos_y = 200;
static bool grid_enabled = false;

// Текущие настройки по умолчанию (используются при создании новых примитивов)
static float default_point_color[3] = {0.0f, 0.0f, 0.0f};
static float default_point_size = 7.0f;

static float default_line_color[3] = {0.0f, 0.0f, 0.0f};
static float default_line_width = 5.0f;

static float default_broken_line_color[3] = {0.0f, 0.0f, 0.0f};
static float default_broken_line_width = 5.0f;

static float default_arrow_color[3] = {0.0f, 0.0f, 0.0f};
static float default_arrow_width = 5.0f;

static float default_circle_color[3] = {0.0f, 0.0f, 0.0f};
static float default_circle_width = 5.0f;

static float default_ellipse_color[3] = {0.0f, 0.0f, 0.0f};
static float default_ellipse_width = 5.0f;

static float default_triangle_color[3] = {0.0f, 0.0f, 0.0f};
static float default_triangle_width = 4.0f;

static float default_polygon_color[3] = {0.0f, 0.0f, 0.0f};
static float default_polygon_width = 4.0f;

// === === === === === === === === === === === === === === === === === === === ===

// Функции рисующие объекты //

void draw_coord_label(float x, float y, const char* text) {
    glRasterPos2f(x + 0.03f, y + 0.03f);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
    }
}

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
        for (float x = axis_limits[0]; x <= axis_limits[1]; x += 1.0f) {
            glVertex2f(x, axis_limits[2]-0.2);
            glVertex2f(x, axis_limits[3]);
        }
        for (float y = axis_limits[2]; y <= axis_limits[3]; y += 1.0f) {
            glVertex2f(axis_limits[0]-0.2, y);
            glVertex2f(axis_limits[1], y);
        }
        glEnd();

        glColor3f(0.0f, 0.0f, 0.0f);
        for (float x = axis_limits[0]; x <= axis_limits[1]; x += 1.0f) {
            char label[10];
            snprintf(label, sizeof(label), "%.1f", x);
            draw_coord_label(x, axis_limits[2]-0.2f, label);
        }
        for (float y = axis_limits[2]; y <= axis_limits[3]; y += 1.0f) {
            char label[10];
            snprintf(label, sizeof(label), "%.1f", y);
            draw_coord_label(axis_limits[0]-0.4f, y+0.1f, label);
        }
    }

    // Оси координат
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(5.0f);
    glBegin(GL_LINES);
        // Стрелка Ось X
        glVertex2f(axis_limits[0]-0.3, axis_limits[2]);
        glVertex2f(axis_limits[1], axis_limits[2]);
        glVertex2f(axis_limits[1], axis_limits[2]);
        glVertex2f(axis_limits[1]-0.2, axis_limits[2]+0.1);
        glVertex2f(axis_limits[1], axis_limits[2]);
        glVertex2f(axis_limits[1]-0.2, axis_limits[2]-0.1);
        // Стрелка Ось Y
        glVertex2f(axis_limits[0], axis_limits[2]-0.3);
        glVertex2f(axis_limits[0], axis_limits[3]);
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

    // Примитивы //

    // Многоугольники
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    for (int i = 0; i < PolygonCount; i++) {
        bool all_vertices_visible = true;
        for (int j = 0; j < PolygonS[i].vertex_count; j++) {
            if ((PolygonS[i].x_coords[j] < axis_limits[0]) || (PolygonS[i].x_coords[j] > axis_limits[1]) ||
                (PolygonS[i].y_coords[j] < axis_limits[2]) || (PolygonS[i].y_coords[j] > axis_limits[3])) {
                all_vertices_visible = false;
                break;
            }
        }

        if (all_vertices_visible) {
            glColor3fv(PolygonS[i].color);
            glLineWidth(PolygonS[i].width);
            glBegin(GL_LINE_LOOP);

            for (int j = 0; j < PolygonS[i].vertex_count; j++) {
                glVertex2f(PolygonS[i].x_coords[j], PolygonS[i].y_coords[j]);
            }
            glEnd();
        }
    }

    // Треугольники
    for (int i = 0; i < TriangleCount; i++) {
        if ((TriangleS[i].x0 >= axis_limits[0]) && (TriangleS[i].x0 <= axis_limits[1]) &&
            (TriangleS[i].y0 >= axis_limits[2]) && (TriangleS[i].y0 <= axis_limits[3]) &&
            (TriangleS[i].x1 >= axis_limits[0]) && (TriangleS[i].x1 <= axis_limits[1]) &&
            (TriangleS[i].y1 >= axis_limits[2]) && (TriangleS[i].y1 <= axis_limits[3]) &&
            (TriangleS[i].x2 >= axis_limits[0]) && (TriangleS[i].x2 <= axis_limits[1]) &&
            (TriangleS[i].y2 >= axis_limits[2]) && (TriangleS[i].y2 <= axis_limits[3])) {

                glColor3fv(TriangleS[i].color);
                glLineWidth(TriangleS[i].width);
                glBegin(GL_LINES);
                    glVertex2f(TriangleS[i].x0, TriangleS[i].y0);
                    glVertex2f(TriangleS[i].x1, TriangleS[i].y1);
                    glVertex2f(TriangleS[i].x1, TriangleS[i].y1);
                    glVertex2f(TriangleS[i].x2, TriangleS[i].y2);
                    glVertex2f(TriangleS[i].x2, TriangleS[i].y2);
                    glVertex2f(TriangleS[i].x0, TriangleS[i].y0);
                glEnd();
        }
    }

    // Эллипсы
    for (int i = 0; i < EllipseCount; i++) {
        if ((EllipseS[i].x - EllipseS[i].a >= axis_limits[0]) &&
            (EllipseS[i].x + EllipseS[i].a <= axis_limits[1]) &&
            (EllipseS[i].y - EllipseS[i].b >= axis_limits[2]) &&
            (EllipseS[i].y + EllipseS[i].b <= axis_limits[3])) {

                glColor3fv(EllipseS[i].color);
                glLineWidth(EllipseS[i].width);
                 const float PI = 3.14159265358979323846f;
                const int segments = 360;

                glBegin(GL_LINE_LOOP);
                    for (int j = 0; j < segments; j++) {
                        float angle = 2.0f * PI * j / segments;
                        float dx = EllipseS[i].a * cosf(angle);
                        float dy = EllipseS[i].b * sinf(angle);
                        glVertex2f(EllipseS[i].x + dx, EllipseS[i].y + dy);
                    }
                glEnd();
        }
    }

    // Окружности
    for (int i = 0; i < CircleCount; i++) {
        if ((CircleS[i].x - CircleS[i].radius >= axis_limits[0]) &&
            (CircleS[i].x + CircleS[i].radius <= axis_limits[1]) &&
            (CircleS[i].y - CircleS[i].radius >= axis_limits[2]) &&
            (CircleS[i].y + CircleS[i].radius <= axis_limits[3])) {

                glColor3fv(CircleS[i].color);
                glLineWidth(CircleS[i].width);
                const float PI = 3.14159265358979323846f;
                const int segments = 360;

                glBegin(GL_LINE_LOOP);
                    for (int j = 0; j < segments; j++) {
                        float angle = 2.0f * PI * j / segments;
                        float dx = CircleS[i].radius * cosf(angle);
                        float dy = CircleS[i].radius * sinf(angle);
                        glVertex2f(CircleS[i].x + dx, CircleS[i].y + dy);
                    }
                glEnd();
        }
    }

    // Стрелки/Вектора
    for (int i = 0; i < ArrowCount; i++) {
        if ((ArrowS[i].xStart >= axis_limits[0]) && (ArrowS[i].xStart <= axis_limits[1]) &&
            (ArrowS[i].xFinish >= axis_limits[0]) && (ArrowS[i].xFinish <= axis_limits[1]) &&
            (ArrowS[i].yStart >= axis_limits[2]) && (ArrowS[i].yStart <= axis_limits[3]) &&
            (ArrowS[i].yFinish >= axis_limits[2]) && (ArrowS[i].yFinish <= axis_limits[3])) {

                glColor3fv(ArrowS[i].color);
                glLineWidth(ArrowS[i].width);

                // Рисуем основную линию стрелки
                glBegin(GL_LINES);
                    glVertex2f(ArrowS[i].xStart, ArrowS[i].yStart);
                    glVertex2f(ArrowS[i].xFinish, ArrowS[i].yFinish);
                glEnd();

                // Вычисляем параметры для стрелки
                float dx = ArrowS[i].xFinish - ArrowS[i].xStart;
                float dy = ArrowS[i].yFinish - ArrowS[i].yStart;
                float length = sqrtf(dx * dx + dy * dy);

                if (length > 0.0f) {
                    // Нормализуем направление
                    dx /= length;
                    dy /= length;

                    // Размер наконечника стрелки
                    float arrow_size = 0.1f * length;
                    if (arrow_size > 0.3f) arrow_size = 0.3f;

                    // Перпендикулярные векторы для наконечника
                    float perp_x = -dy;
                    float perp_y = dx;

                    // Вершины наконечника стрелки
                    float tip_x = ArrowS[i].xFinish;
                    float tip_y = ArrowS[i].yFinish;

                    float left_x = tip_x - dx * arrow_size + perp_x * arrow_size * 0.5f;
                    float left_y = tip_y - dy * arrow_size + perp_y * arrow_size * 0.5f;

                    float right_x = tip_x - dx * arrow_size - perp_x * arrow_size * 0.5f;
                    float right_y = tip_y - dy * arrow_size - perp_y * arrow_size * 0.5f;

                    // Рисуем наконечник стрелки
                    glBegin(GL_LINES);
                        glVertex2f(tip_x, tip_y);
                        glVertex2f(left_x, left_y);
                        glVertex2f(tip_x, tip_y);
                        glVertex2f(right_x, right_y);
                    glEnd();
                }
        }
    }

    // Ломанные линии
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    for (int i = 0; i < BrokenLineCount; i++) {
        bool all_vertices_visible = true;
        for (int j = 0; j < BrokenLineS[i].vertex_count; j++) {
            if ((BrokenLineS[i].x_coords[j] < axis_limits[0]) || (BrokenLineS[i].x_coords[j] > axis_limits[1]) ||
                (BrokenLineS[i].y_coords[j] < axis_limits[2]) || (BrokenLineS[i].y_coords[j] > axis_limits[3])) {
                all_vertices_visible = false;
                break;
            }
        }

        if (all_vertices_visible) {
            glColor3fv(BrokenLineS[i].color);
            glLineWidth(BrokenLineS[i].width);
            glBegin(GL_LINES);
            for (int j = 1; j < BrokenLineS[i].vertex_count; j++) {
                glVertex2f(BrokenLineS[i].x_coords[j-1], BrokenLineS[i].y_coords[j-1]);
                glVertex2f(BrokenLineS[i].x_coords[j], BrokenLineS[i].y_coords[j]);
            }
            glEnd();
        }
    }

    // Линии
    for (int i = 0; i < LineCount; i++) {
        if ((LineS[i].xStart >= axis_limits[0]) && (LineS[i].xStart <= axis_limits[1]) &&
            (LineS[i].xFinish >= axis_limits[0]) && (LineS[i].xFinish <= axis_limits[1]) &&
            (LineS[i].yStart >= axis_limits[2]) && (LineS[i].yStart <= axis_limits[3]) &&
            (LineS[i].yFinish >= axis_limits[2]) && (LineS[i].yFinish <= axis_limits[3])) {

                glColor3fv(LineS[i].color);
                glLineWidth(LineS[i].width);
                glBegin(GL_LINES);
                    glVertex2f(LineS[i].xStart, LineS[i].yStart);
                    glVertex2f(LineS[i].xFinish, LineS[i].yFinish);
                glEnd();
        }
    }

    // Точки
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    for (int i = 0; i < PointCount; i++) {
        if ((PointS[i].x >= axis_limits[0]) && (PointS[i].x <= axis_limits[1]) &&
            (PointS[i].y >= axis_limits[2]) && (PointS[i].y <= axis_limits[3])) {

                glColor3fv(PointS[i].color);
                glPointSize(PointS[i].size);
                glBegin(GL_POINTS);
                    glVertex2f(PointS[i].x, PointS[i].y);
                glEnd();
        }
    }
    glutSwapBuffers();
}

// === === === === === === === === === === === === === === === === === === === ===

void window_position(int x_position, int y_position) {
    win_pos_x = x_position;
    win_pos_y = y_position;
}

void window(int width, int height) {
    window_width = width;
    window_height = height;
}

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

void point(float x, float y) {
    Point* temp = (Point*)realloc(PointS, (PointCount + 1) * sizeof(Point));
    if (temp == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для точки.\n");
        return;
    }
    PointS = temp;
    PointS[PointCount].x = x;
    PointS[PointCount].y = y;
    memcpy(PointS[PointCount].color, default_point_color, 3 * sizeof(float));
    PointS[PointCount].size = default_point_size;
    PointCount++;
    glutPostRedisplay();
}

void set_point_size(float size) {
    default_point_size = size;
}

void set_point_color(int r, int g, int b) {
    default_point_color[0] = r/255.0f;
    default_point_color[1] = g/255.0f;
    default_point_color[2] = b/255.0f;
}

void line(float xStart, float yStart, float xFinish, float yFinish) {
    Line* temp = (Line*)realloc(LineS, (LineCount + 1) * sizeof(Line));
    if (temp == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для линии.\n");
        return;
    }
    LineS = temp;
    LineS[LineCount].xStart = xStart;
    LineS[LineCount].yStart = yStart;
    LineS[LineCount].xFinish = xFinish;
    LineS[LineCount].yFinish = yFinish;
    memcpy(LineS[LineCount].color, default_line_color, 3 * sizeof(float));
    LineS[LineCount].width = default_line_width;
    LineCount++;
    glutPostRedisplay();
}

void set_line_width(float width) {
    default_line_width = width;
}

void set_line_color(int r, int g, int b) {
    default_line_color[0] = r/255.0f;
    default_line_color[1] = g/255.0f;
    default_line_color[2] = b/255.0f;
}

void broken_line(float* x_coords, float* y_coords, int vertex_count) {
    if (vertex_count < 2) {
        fprintf(stderr, "Ошибка: линия должна иметь минимум 2 вершины.\n");
        return;
    }
    BrokenLine* temp = (BrokenLine*)realloc(BrokenLineS, (BrokenLineCount + 1) * sizeof(BrokenLine));
    if (temp == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для ломаной.\n");
        return;
    }
    BrokenLineS = temp;

    BrokenLineS[BrokenLineCount].x_coords = (float*)malloc(vertex_count * sizeof(float));
    BrokenLineS[BrokenLineCount].y_coords = (float*)malloc(vertex_count * sizeof(float));

    if (BrokenLineS[BrokenLineCount].x_coords == NULL || BrokenLineS[BrokenLineCount].y_coords == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для координат ломаной линии.\n");
        return;
    }

    memcpy(BrokenLineS[BrokenLineCount].x_coords, x_coords, vertex_count * sizeof(float));
    memcpy(BrokenLineS[BrokenLineCount].y_coords, y_coords, vertex_count * sizeof(float));
    BrokenLineS[BrokenLineCount].vertex_count = vertex_count;
    memcpy(BrokenLineS[BrokenLineCount].color, default_broken_line_color, 3 * sizeof(float));
    BrokenLineS[BrokenLineCount].width = default_broken_line_width;
    BrokenLineCount++;
    glutPostRedisplay();
}

void set_broken_line_width(float width) {
    default_broken_line_width = width;
}

void set_broken_line_color(int r, int g, int b) {
    default_broken_line_color[0] = r / 255.0f;
    default_broken_line_color[1] = g / 255.0f;
    default_broken_line_color[2] = b / 255.0f;
}

void arrow(float xStart, float yStart, float xFinish, float yFinish) {
    Arrow* temp = (Arrow*)realloc(ArrowS, (ArrowCount + 1) * sizeof(Arrow));
    if (temp == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для стрелки.\n");
        return;
    }
    ArrowS = temp;
    ArrowS[ArrowCount].xStart = xStart;
    ArrowS[ArrowCount].yStart = yStart;
    ArrowS[ArrowCount].xFinish = xFinish;
    ArrowS[ArrowCount].yFinish = yFinish;
    memcpy(ArrowS[ArrowCount].color, default_arrow_color, 3 * sizeof(float));
    ArrowS[ArrowCount].width = default_arrow_width;
    ArrowCount++;
    glutPostRedisplay();
}

void set_arrow_width(float width) {
    default_arrow_width = width;
}

void set_arrow_color(int r, int g, int b) {
    default_arrow_color[0] = r / 255.0f;
    default_arrow_color[1] = g / 255.0f;
    default_arrow_color[2] = b / 255.0f;
}

void circle(float x, float y, float radius) {
    Circle* temp = (Circle*)realloc(CircleS, (CircleCount + 1) * sizeof(Circle));
    if (temp == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для окружности.\n");
        return;
    }
    CircleS = temp;
    CircleS[CircleCount].x = x;
    CircleS[CircleCount].y = y;
    CircleS[CircleCount].radius = radius;
    memcpy(CircleS[CircleCount].color, default_circle_color, 3 * sizeof(float));
    CircleS[CircleCount].width = default_circle_width;
    CircleCount++;
    glutPostRedisplay();
}

void set_circle_width(float width) {
    default_circle_width = width;
}

void set_circle_color(int r, int g, int b) {
    default_circle_color[0] = r / 255.0f;
    default_circle_color[1] = g / 255.0f;
    default_circle_color[2] = b / 255.0f;
}

void ellipse(float x, float y, float a, float b) {
    Ellipse* temp = (Ellipse*)realloc(EllipseS, (EllipseCount + 1) * sizeof(Ellipse));
    if (temp == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для эллипса.\n");
        return;
    }
    EllipseS = temp;
    EllipseS[EllipseCount].x = x;
    EllipseS[EllipseCount].y = y;
    EllipseS[EllipseCount].a = a;  // большая полуось
    EllipseS[EllipseCount].b = b;  // малая полуось
    memcpy(EllipseS[EllipseCount].color, default_ellipse_color, 3 * sizeof(float));
    EllipseS[EllipseCount].width = default_ellipse_width;
    EllipseCount++;
    glutPostRedisplay();
}

void set_ellipse_width(float width) {
    default_ellipse_width = width;
}

void set_ellipse_color(int r, int g, int b) {
    default_ellipse_color[0] = r / 255.0f;
    default_ellipse_color[1] = g / 255.0f;
    default_ellipse_color[2] = b / 255.0f;
}

void triangle(float x0, float y0, float x1, float y1, float x2, float y2) {
    Triangle* temp = (Triangle*)realloc(TriangleS, (TriangleCount + 1) * sizeof(Triangle));
    if (temp == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для треугольника.\n");
        return;
    }
    TriangleS = temp;
    TriangleS[TriangleCount].x0 = x0;
    TriangleS[TriangleCount].y0 = y0;
    TriangleS[TriangleCount].x1 = x1;
    TriangleS[TriangleCount].y1 = y1;
    TriangleS[TriangleCount].x2 = x2;
    TriangleS[TriangleCount].y2 = y2;
    memcpy(TriangleS[TriangleCount].color, default_triangle_color, 3 * sizeof(float));
    TriangleS[TriangleCount].width = default_triangle_width;
    TriangleCount++;
    glutPostRedisplay();
}

void set_triangle_width(float width) {
    default_triangle_width = width;
}

void set_triangle_color(int r, int g, int b) {
    default_triangle_color[0] = r / 255.0f;
    default_triangle_color[1] = g / 255.0f;
    default_triangle_color[2] = b / 255.0f;
}

void polygon(float* x_coords, float* y_coords, int vertex_count) {
    if (vertex_count < 3) {
        fprintf(stderr, "Ошибка: многоугольник должен иметь минимум 3 вершины.\n");
        return;
    }

    Polygon* temp = (Polygon*)realloc(PolygonS, (PolygonCount + 1) * sizeof(Polygon));
    if (temp == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для многоугольника.\n");
        return;
    }
    PolygonS = temp;

    PolygonS[PolygonCount].x_coords = (float*)malloc(vertex_count * sizeof(float));
    PolygonS[PolygonCount].y_coords = (float*)malloc(vertex_count * sizeof(float));

    if (PolygonS[PolygonCount].x_coords == NULL || PolygonS[PolygonCount].y_coords == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для координат многоугольника.\n");
        return;
    }

    memcpy(PolygonS[PolygonCount].x_coords, x_coords, vertex_count * sizeof(float));
    memcpy(PolygonS[PolygonCount].y_coords, y_coords, vertex_count * sizeof(float));
    PolygonS[PolygonCount].vertex_count = vertex_count;
    memcpy(PolygonS[PolygonCount].color, default_polygon_color, 3 * sizeof(float));
    PolygonS[PolygonCount].width = default_polygon_width;
    PolygonCount++;
    glutPostRedisplay();
}

void set_polygon_width(float width) {
    default_polygon_width = width;
}

void set_polygon_color(int r, int g, int b) {
    default_polygon_color[0] = r / 255.0f;
    default_polygon_color[1] = g / 255.0f;
    default_polygon_color[2] = b / 255.0f;
}

void show() {
    glutMainLoop();
}
