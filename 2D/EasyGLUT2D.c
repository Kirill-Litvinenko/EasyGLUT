#include "EasyGLUT2D.h"
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================
//  Структуры примитивов
// ============================================================

typedef struct {
    double x, y;
    float  color[3];
    float  size;
    int    group_id;
    bool   visible;
} Point;

typedef struct {
    double xStart, yStart, xFinish, yFinish;
    float  color[3];
    float  width;
    int    group_id;
    bool   visible;
} Line;

typedef struct {
    double* x_coords;
    double* y_coords;
    int     vertex_count;
    float   color[3];
    float   width;
    int     group_id;
    bool    visible;
} BrokenLine;

typedef struct {
    double xStart, yStart, xFinish, yFinish;
    float  color[3];
    float  width;
    int    group_id;
    bool   visible;
} Arrow;

typedef struct {
    double x, y, radius;
    float  color[3];
    float  width;
    int    group_id;
    bool   visible;
    bool   fill_enabled;
    float  fill_color[4];
} Circle;

typedef struct {
    double x, y, a, b;
    float  color[3];
    float  width;
    int    group_id;
    bool   visible;
    bool   fill_enabled;
    float  fill_color[4];
} Ellipse;

typedef struct {
    double x0, y0, x1, y1, x2, y2;
    float color[3];
    float width;
    int   group_id;
    bool  visible;
    bool  fill_enabled;
    float fill_color[4];
} Triangle;

typedef struct {
    double* x_coords;
    double* y_coords;
    int     vertex_count;
    float   color[3];
    float   width;
    int     group_id;
    bool    visible;
    bool    fill_enabled;
    float   fill_color[4];
} Polygon;

typedef struct {
    double  x, y;
    float   color[3];
    char*   text;
    int     group_id;
    bool    visible;
} OutText;

// ============================================================
//  Глобальные массивы примитивов
// ============================================================

static Point*      Points      = NULL;  static int PointCount      = 0;
static Line*       Lines       = NULL;  static int LineCount       = 0;
static BrokenLine* BrokenLines = NULL;  static int BrokenLineCount = 0;
static Arrow*      Arrows      = NULL;  static int ArrowCount      = 0;
static Circle*     Circles     = NULL;  static int CircleCount     = 0;
static Ellipse*    Ellipses    = NULL;  static int EllipseCount    = 0;
static Triangle*   Triangles   = NULL;  static int TriangleCount   = 0;
static Polygon*    Polygons    = NULL;  static int PolygonCount    = 0;
static OutText*    OutTexts    = NULL;  static int OutTextCount    = 0;

// ============================================================
//  Глобальные настройки
// ============================================================

static double axis_limits[4]   = { -1.0, 1.0, -1.0, 1.0 };
static int   window_width     = 1920;
static int   window_height    = 1080;
static int   win_pos_x        = 0;
static int   win_pos_y        = 0;
static bool  grid_enabled     = true;
static bool  glut_initialized = false;
static float bg_color[3]      = { 1.0f, 1.0f, 1.0f };
static int   grid_mode        = 0;

static float default_point_color[3]       = { 0.0f, 0.0f, 0.0f };
static float default_point_size           = 7.0f;
static float default_line_color[3]        = { 0.0f, 0.0f, 0.0f };
static float default_line_width           = 5.0f;
static float default_broken_line_color[3] = { 0.0f, 0.0f, 0.0f };
static float default_broken_line_width    = 5.0f;
static float default_arrow_color[3]       = { 0.0f, 0.0f, 0.0f };
static float default_arrow_width          = 5.0f;
static float default_circle_color[3]      = { 0.0f, 0.0f, 0.0f };
static float default_circle_width         = 5.0f;
static float default_ellipse_color[3]     = { 0.0f, 0.0f, 0.0f };
static float default_ellipse_width        = 5.0f;
static float default_triangle_color[3]    = { 0.0f, 0.0f, 0.0f };
static float default_triangle_width       = 4.0f;
static float default_polygon_color[3]     = { 0.0f, 0.0f, 0.0f };
static float default_polygon_width        = 4.0f;
static float default_text_color[3]        = { 0.0f, 0.0f, 0.0f };

static float default_circle_fill_color[4]   = { 0.0f, 0.0f, 1.0f, 0.3f };
static float default_ellipse_fill_color[4]  = { 0.0f, 0.0f, 1.0f, 0.3f };
static float default_triangle_fill_color[4] = { 1.0f, 0.0f, 0.0f, 0.3f };
static float default_polygon_fill_color[4]  = { 0.0f, 1.0f, 0.0f, 0.3f };

static bool default_circle_fill_enabled   = false;
static bool default_ellipse_fill_enabled  = false;
static bool default_triangle_fill_enabled = false;
static bool default_polygon_fill_enabled  = false;

static int current_group     = 0;
static int groups_count      = 0;
static int active_view_group = 0;

// ============================================================
//  Предварительные заявления
// ============================================================
static void hide_all_groups(void);
static void show_all_groups(void);
static void cleanup_all_internal(void);
static void clear_group_internal(int group_id);
static void set_group_visibility_internal(int group_id, bool visible);

// ============================================================
//  АЛГОРИТМ ЛЯН–БАРСКИ: отсечение отрезков
// ============================================================

static bool clip_line_lb(double* x0, double* y0, double* x1, double* y1) {
    double ox = *x0, oy = *y0;
    double dx = *x1 - ox;
    double dy = *y1 - oy;
    double t0 = 0.0, t1 = 1.0;

    double p[4] = { -dx,  dx, -dy,  dy };
    double q[4] = {
        ox - axis_limits[0],
        axis_limits[1] - ox,
        oy - axis_limits[2],
        axis_limits[3] - oy
    };

    for (int i = 0; i < 4; i++) {
        if (fabs(p[i]) < 1e-12) {
            if (q[i] < 0.0) return false;
            continue;
        }
        double r = q[i] / p[i];
        if (p[i] < 0.0) { if (r > t1) return false; if (r > t0) t0 = r; }
        else            { if (r < t0) return false; if (r < t1) t1 = r; }
    }

    *x0 = ox + t0 * dx; *y0 = oy + t0 * dy;
    *x1 = ox + t1 * dx; *y1 = oy + t1 * dy;
    return true;
}

static bool point_inside(double x, double y) {
    return x >= axis_limits[0] && x <= axis_limits[1]
        && y >= axis_limits[2] && y <= axis_limits[3];
}

// ============================================================
//   АЛГОРИТМ Сазерленд-Ходжман: отсечение полигона
// ============================================================

static bool clip_polygon_sh(double* in_x, double* in_y, int in_n,
                            double** out_x, double** out_y, int* out_n) {
    *out_n = 0;
    *out_x = NULL;
    *out_y = NULL;

    if (in_n < 3) return false;

    int max_v = in_n * 4;
    double* cur_x  = malloc((size_t)max_v * sizeof(double));
    double* cur_y  = malloc((size_t)max_v * sizeof(double));
    double* next_x = malloc((size_t)max_v * sizeof(double));
    double* next_y = malloc((size_t)max_v * sizeof(double));
    if (!cur_x || !cur_y || !next_x || !next_y) {
        free(cur_x); free(cur_y); free(next_x); free(next_y);
        return false;
    }

    memcpy(cur_x, in_x, (size_t)in_n * sizeof(double));
    memcpy(cur_y, in_y, (size_t)in_n * sizeof(double));
    int cur_n = in_n;

    double bounds[4][4] = {
        { 1, 0, -axis_limits[0], 0 },
        { -1, 0,  axis_limits[1], 0 },
        { 0, 1, -axis_limits[2], 0 },
        { 0, -1,  axis_limits[3], 0 }
    };

    for (int edge = 0; edge < 4; edge++) {
        if (cur_n == 0) break;
        double A = bounds[edge][0], B = bounds[edge][1], C = bounds[edge][2];
        int next_idx = 0;

        for (int i = 0; i < cur_n; i++) {
            int j = (i + 1) % cur_n;
            double x1 = cur_x[i], y1 = cur_y[i];
            double x2 = cur_x[j], y2 = cur_y[j];
            double d1 = A * x1 + B * y1 + C;
            double d2 = A * x2 + B * y2 + C;

            bool inside1 = (d1 >= -1e-12);
            bool inside2 = (d2 >= -1e-12);

            if (inside1 && inside2) {
                next_x[next_idx] = x2;
                next_y[next_idx] = y2;
                next_idx++;
            }
            else if (inside1 && !inside2) {
                double t = d1 / (d1 - d2);
                if (fabs(d1 - d2) > 1e-12) {
                    double ix = x1 + t * (x2 - x1);
                    double iy = y1 + t * (y2 - y1);
                    next_x[next_idx] = ix;
                    next_y[next_idx] = iy;
                    next_idx++;
                }
            }
            else if (!inside1 && inside2) {
                double t = d1 / (d1 - d2);
                if (fabs(d1 - d2) > 1e-12) {
                    double ix = x1 + t * (x2 - x1);
                    double iy = y1 + t * (y2 - y1);
                    next_x[next_idx] = ix;
                    next_y[next_idx] = iy;
                    next_idx++;
                }
                next_x[next_idx] = x2;
                next_y[next_idx] = y2;
                next_idx++;
            }
        }
        double* tmp_x = cur_x; cur_x = next_x; next_x = tmp_x;
        double* tmp_y = cur_y; cur_y = next_y; next_y = tmp_y;
        cur_n = next_idx;
    }

    if (cur_n >= 3) {
        *out_x = malloc((size_t)cur_n * sizeof(double));
        *out_y = malloc((size_t)cur_n * sizeof(double));
        if (*out_x && *out_y) {
            memcpy(*out_x, cur_x, (size_t)cur_n * sizeof(double));
            memcpy(*out_y, cur_y, (size_t)cur_n * sizeof(double));
            *out_n = cur_n;
        }
    }

    free(cur_x); free(cur_y); free(next_x); free(next_y);
    return (*out_n >= 3);
}

// ============================================================
//  EAR-CLIPPING ТРИАНГУЛЯЦИЯ
// ============================================================

static double cross2d(double ax, double ay, double bx, double by, double cx, double cy) {
    return (bx - ax) * (cy - ay) - (by - ay) * (cx - ax);
}

static bool point_in_tri(double px, double py, double ax, double ay, double bx, double by, double cx, double cy) {
    double d1 = cross2d(px, py, ax, ay, bx, by);
    double d2 = cross2d(px, py, bx, by, cx, cy);
    double d3 = cross2d(px, py, cx, cy, ax, ay);
    bool hn = (d1 < 0.0) || (d2 < 0.0) || (d3 < 0.0);
    bool hp = (d1 > 0.0) || (d2 > 0.0) || (d3 > 0.0);
    return !(hn && hp);
}

static double* triangulate_polygon(double* xs, double* ys, int n, int* out_count) {
    *out_count = 0;
    if (n < 3) return NULL;

    int* idx = malloc((size_t)n * sizeof(int));
    if (!idx) return NULL;
    for (int i = 0; i < n; i++) idx[i] = i;

    double* tris = malloc((size_t)(n - 2) * 6 * sizeof(double));
    if (!tris) { free(idx); return NULL; }
    int tc = 0, sz = n;

    double area = 0.0;
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        area += xs[i] * ys[j] - xs[j] * ys[i];
    }
    if (area < 0.0) {
        for (int i = 0, j = n - 1; i < j; i++, j--) {
            int tmp = idx[i]; idx[i] = idx[j]; idx[j] = tmp;
        }
    }

    while (sz > 3) {
        bool found = false;
        for (int i = 0; i < sz; i++) {
            int ia = idx[(i - 1 + sz) % sz];
            int ib = idx[i];
            int ic = idx[(i + 1) % sz];
            double cp = cross2d(xs[ia], ys[ia], xs[ib], ys[ib], xs[ic], ys[ic]);
            if (cp <= 0.0) continue;
            bool ear = true;
            for (int k = 0; k < sz && ear; k++) {
                int p = idx[k];
                if (p == ia || p == ib || p == ic) continue;
                if (point_in_tri(xs[p], ys[p], xs[ia], ys[ia], xs[ib], ys[ib], xs[ic], ys[ic])) ear = false;
            }
            if (ear) {
                tris[tc++] = xs[ia]; tris[tc++] = ys[ia];
                tris[tc++] = xs[ib]; tris[tc++] = ys[ib];
                tris[tc++] = xs[ic]; tris[tc++] = ys[ic];
                for (int k = i; k < sz - 1; k++) idx[k] = idx[k + 1];
                sz--;
                found = true;
                break;
            }
        }
        if (!found) break;
    }
    if (sz == 3) {
        tris[tc++] = xs[idx[0]]; tris[tc++] = ys[idx[0]];
        tris[tc++] = xs[idx[1]]; tris[tc++] = ys[idx[1]];
        tris[tc++] = xs[idx[2]]; tris[tc++] = ys[idx[2]];
    }
    free(idx);
    *out_count = tc / 6;
    return tris;
}

// ============================================================
//  Управление группами
// ============================================================

static void _update_group_visibility(int gid, bool v) {
    for (int i = 0; i < PointCount;      i++) if (Points[i].group_id      == gid) Points[i].visible      = v;
    for (int i = 0; i < LineCount;       i++) if (Lines[i].group_id       == gid) Lines[i].visible       = v;
    for (int i = 0; i < BrokenLineCount; i++) if (BrokenLines[i].group_id == gid) BrokenLines[i].visible = v;
    for (int i = 0; i < ArrowCount;      i++) if (Arrows[i].group_id      == gid) Arrows[i].visible      = v;
    for (int i = 0; i < CircleCount;     i++) if (Circles[i].group_id     == gid) Circles[i].visible     = v;
    for (int i = 0; i < EllipseCount;    i++) if (Ellipses[i].group_id    == gid) Ellipses[i].visible    = v;
    for (int i = 0; i < TriangleCount;   i++) if (Triangles[i].group_id   == gid) Triangles[i].visible   = v;
    for (int i = 0; i < PolygonCount;    i++) if (Polygons[i].group_id    == gid) Polygons[i].visible    = v;
    for (int i = 0; i < OutTextCount;    i++) if (OutTexts[i].group_id    == gid) OutTexts[i].visible    = v;
}

int eg2d_create_group(void) { int id = groups_count++; current_group = id; return id; }
void eg2d_select_group(int gid) {
    if (gid < 0 || gid >= groups_count) { fprintf(stderr, "select_group: неверный id %d\n", gid); return; }
    current_group = gid;
}
void eg2d_show_only_group(int gid) {
    eg2d_hide_all_groups();
    if (gid < 0) { eg2d_show_all_groups(); return; }
    if (gid >= groups_count) { fprintf(stderr, "show_only_group: неверный id %d\n", gid); return; }
    _update_group_visibility(gid, true);
    active_view_group = gid;
    if (glut_initialized) glutPostRedisplay();
}
void eg2d_set_group_visibility(int gid, bool v) {
    if (gid < 0 || gid >= groups_count) { fprintf(stderr, "set_group_visibility: неверный id %d\n", gid); return; }
    _update_group_visibility(gid, v);
    if (glut_initialized) glutPostRedisplay();
}
void eg2d_show_groups(int* ids, int cnt) {
    eg2d_hide_all_groups();
    for (int i = 0; i < cnt; i++) if (ids[i] >= 0 && ids[i] < groups_count) _update_group_visibility(ids[i], true);
    if (glut_initialized) glutPostRedisplay();
}
void eg2d_hide_all_groups(void) {
    for (int i = 0; i < PointCount;      i++) Points[i].visible      = false;
    for (int i = 0; i < LineCount;       i++) Lines[i].visible       = false;
    for (int i = 0; i < BrokenLineCount; i++) BrokenLines[i].visible = false;
    for (int i = 0; i < ArrowCount;      i++) Arrows[i].visible      = false;
    for (int i = 0; i < CircleCount;     i++) Circles[i].visible     = false;
    for (int i = 0; i < EllipseCount;    i++) Ellipses[i].visible    = false;
    for (int i = 0; i < TriangleCount;   i++) Triangles[i].visible   = false;
    for (int i = 0; i < PolygonCount;    i++) Polygons[i].visible    = false;
    for (int i = 0; i < OutTextCount;    i++) OutTexts[i].visible    = false;
    if (glut_initialized) glutPostRedisplay();
}
void eg2d_show_all_groups(void) {
    for (int i = 0; i < PointCount;      i++) Points[i].visible      = true;
    for (int i = 0; i < LineCount;       i++) Lines[i].visible       = true;
    for (int i = 0; i < BrokenLineCount; i++) BrokenLines[i].visible = true;
    for (int i = 0; i < ArrowCount;      i++) Arrows[i].visible      = true;
    for (int i = 0; i < CircleCount;     i++) Circles[i].visible     = true;
    for (int i = 0; i < EllipseCount;    i++) Ellipses[i].visible    = true;
    for (int i = 0; i < TriangleCount;   i++) Triangles[i].visible   = true;
    for (int i = 0; i < PolygonCount;    i++) Polygons[i].visible    = true;
    for (int i = 0; i < OutTextCount;    i++) OutTexts[i].visible    = true;
    if (glut_initialized) glutPostRedisplay();
}
int eg2d_get_groups_count(void) { return groups_count; }

#define EG_SHRINK(arr, cnt, type) do { \
    if ((cnt) == 0) { free(arr); (arr) = NULL; } \
    else { type* _t = realloc((arr), (size_t)(cnt) * sizeof(type)); if (_t) (arr) = _t; } \
} while(0)

#define CLEAR_ARR(arr, cnt, type, free_func) do { \
    int w = 0; \
    for (int i = 0; i < cnt; i++) { \
        if (arr[i].group_id != group_id) { if (w != i) arr[w] = arr[i]; w++; } \
        else { free_func } \
    } \
    cnt = w; EG_SHRINK(arr, cnt, type); \
} while(0)

void eg2d_clear_group(int group_id) {
    if (group_id < 0 || group_id >= groups_count) { fprintf(stderr, "clear_group: неверный id %d\n", group_id); return; }
    CLEAR_ARR(Points,      PointCount,      Point,      {});
    CLEAR_ARR(Lines,       LineCount,       Line,       {});
    CLEAR_ARR(Arrows,      ArrowCount,      Arrow,      {});
    CLEAR_ARR(Circles,     CircleCount,     Circle,     {});
    CLEAR_ARR(Ellipses,    EllipseCount,    Ellipse,    {});
    CLEAR_ARR(Triangles,   TriangleCount,   Triangle,   {});
    CLEAR_ARR(BrokenLines, BrokenLineCount, BrokenLine, free(BrokenLines[i].x_coords); free(BrokenLines[i].y_coords););
    CLEAR_ARR(Polygons,    PolygonCount,    Polygon,    free(Polygons[i].x_coords); free(Polygons[i].y_coords););
    CLEAR_ARR(OutTexts,    OutTextCount,    OutText,    free(OutTexts[i].text););
    #undef CLEAR_ARR

    for (int i = 0; i < PointCount;      i++) if (Points[i].group_id      > group_id) Points[i].group_id--;
    for (int i = 0; i < LineCount;       i++) if (Lines[i].group_id       > group_id) Lines[i].group_id--;
    for (int i = 0; i < BrokenLineCount; i++) if (BrokenLines[i].group_id > group_id) BrokenLines[i].group_id--;
    for (int i = 0; i < ArrowCount;      i++) if (Arrows[i].group_id      > group_id) Arrows[i].group_id--;
    for (int i = 0; i < CircleCount;     i++) if (Circles[i].group_id     > group_id) Circles[i].group_id--;
    for (int i = 0; i < EllipseCount;    i++) if (Ellipses[i].group_id    > group_id) Ellipses[i].group_id--;
    for (int i = 0; i < TriangleCount;   i++) if (Triangles[i].group_id   > group_id) Triangles[i].group_id--;
    for (int i = 0; i < PolygonCount;    i++) if (Polygons[i].group_id    > group_id) Polygons[i].group_id--;
    for (int i = 0; i < OutTextCount;    i++) if (OutTexts[i].group_id    > group_id) OutTexts[i].group_id--;

    groups_count--;
    if (current_group > group_id) current_group--;
    else if (current_group == group_id && groups_count > 0)
        if (current_group >= groups_count) current_group = groups_count - 1;

    if (groups_count > 0) eg2d_show_all_groups(); else eg2d_hide_all_groups();
    active_view_group = 0;
}

// ============================================================
//  Вспомогательные функции отрисовки
// ============================================================

static void draw_coord_label(double x, double y, const char* text) {
    glRasterPos2d(x, y);
    for (const char* c = text; *c; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
}

static void draw_grid_layer(double step, float r, float g, float b, float lw, bool labels) {
    // Вычисляем длину осей для динамических отступов
    double dx = axis_limits[1] - axis_limits[0];
    double dy = axis_limits[3] - axis_limits[2];
    if (dx <= 0.0) dx = 1.0;
    if (dy <= 0.0) dy = 1.0;

    glColor3f(r, g, b); glLineWidth(lw); glBegin(GL_LINES);
    double xs = ceil(axis_limits[0] / step) * step;
    for (double x = xs; x <= axis_limits[1] + step * 0.5; x += step) {
        // Динамическая длина засечки (1% от оси Y)
        glVertex2d(x, axis_limits[2] - dy * 0.01);
        glVertex2d(x, axis_limits[3]);
    }
    double ys = ceil(axis_limits[2] / step) * step;
    for (double y = ys; y <= axis_limits[3] + step * 0.5; y += step) {
        // Динамическая длина засечки (1% от оси X)
        glVertex2d(axis_limits[0] - dx * 0.01, y);
        glVertex2d(axis_limits[1], y);
    }
    glEnd();

    if (labels) {
        glColor3f(0.0f, 0.0f, 0.0f);
        for (double x = xs; x <= axis_limits[1] + step * 0.5; x += step) {
            char l[32]; snprintf(l, 32, "%.2f", x);
            // Отступ надписи: центрирование и сдвиг вниз (2.5% от оси Y)
            draw_coord_label(x - dx * 0.015, axis_limits[2] - dy * 0.025, l);
        }
        for (double y = ys; y <= axis_limits[3] + step * 0.5; y += step) {
            char l[32]; snprintf(l, 32, "%.2f", y);
            // Отступ надписи: сдвиг влево (3.5% от оси X)
            draw_coord_label(axis_limits[0] - dx * 0.035, y - dy * 0.005, l);
        }
    }
}

// ============================================================
//  display — главная функция отрисовки
// ============================================================

static void display(void) {

    glClear(GL_COLOR_BUFFER_BIT);

    // --- ДИНАМИЧЕСКИЕ ОТСТУПЫ ---
    double dx = axis_limits[1] - axis_limits[0];
    double dy = axis_limits[3] - axis_limits[2];
    if (dx <= 0.0) dx = 0.05; // Защита от деления на ноль / нулевой длины
    if (dy <= 0.0) dy = 0.05;

    // Отступ от края экрана = 4% от длины осей
    double mx = dx * 0.04;
    double my = dy * 0.04;

    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    // Используем динамические отступы mx и my вместо жестких
    gluOrtho2D(axis_limits[0] - mx, axis_limits[1] + mx,
               axis_limits[2] - my, axis_limits[3] + my);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    if (grid_enabled) {
        switch (grid_mode) {
            case 0: draw_grid_layer(1.0, 0.75f, 0.75f, 0.75f, 0.3f, true);  break;
            case 1: draw_grid_layer(0.1, 0.88f, 0.88f, 0.88f, 0.3f, false); break;
            default:
                draw_grid_layer(0.1, 0.88f, 0.88f, 0.88f, 0.3f, false);
                draw_grid_layer(1.0, 0.62f, 0.62f, 0.62f, 0.3f, true);
                break;
        }
    }

    // Вывод осей со стрелками
    glColor3f(0.0f, 0.0f, 0.0f); glLineWidth(3.0f); glBegin(GL_LINES);
        // Ось X (отрисовываем немного не доходя до края окна, на mx * 0.8)
        glVertex2d(axis_limits[0] - mx * 0.8, axis_limits[2]);
        glVertex2d(axis_limits[1] + mx * 0.8, axis_limits[2]);
        // Стрелка оси X
        glVertex2d(axis_limits[1] + mx * 0.8, axis_limits[2]);
        glVertex2d(axis_limits[1] + mx * 0.8 - dx * 0.02, axis_limits[2] + dy * 0.008);
        glVertex2d(axis_limits[1] + mx * 0.8, axis_limits[2]);
        glVertex2d(axis_limits[1] + mx * 0.8 - dx * 0.02, axis_limits[2] - dy * 0.008);

        // Ось Y
        glVertex2d(axis_limits[0], axis_limits[2] - my * 0.8);
        glVertex2d(axis_limits[0], axis_limits[3] + my * 0.8);
        // Стрелка оси Y
        glVertex2d(axis_limits[0], axis_limits[3] + my * 0.8);
        glVertex2d(axis_limits[0] - dx * 0.008, axis_limits[3] + my * 0.8 - dy * 0.02);
        glVertex2d(axis_limits[0], axis_limits[3] + my * 0.8);
        glVertex2d(axis_limits[0] + dx * 0.008, axis_limits[3] + my * 0.8 - dy * 0.02);
    glEnd();

    // Динамическое позиционирование подписей осей
    // X над стрелкой
    glRasterPos2d(axis_limits[1] + mx * 0.8 - dx * 0.015, axis_limits[2] + dy * 0.015);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'X');
    // Y слева от стрелки
    glRasterPos2d(axis_limits[0] - dx * 0.02, axis_limits[3] + my * 0.8 - dy * 0.015);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'Y');

    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);  glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);
    glEnable(GL_POINT_SMOOTH); glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    const double PI  = 3.14159265358979323846;
    const int    SEG = 360;

    // ════════════════════════════════════════════════════════
    //  ФАЗА 1: ВСЕ ЗАЛИВКИ
    // ════════════════════════════════════════════════════════

    // ── Заливка полигонов ──
    for (int i = 0; i < PolygonCount; i++) {
        if (!Polygons[i].visible || !Polygons[i].fill_enabled) continue;
        double* clip_x = NULL, *clip_y = NULL; int clip_n = 0;
        if (!clip_polygon_sh(Polygons[i].x_coords, Polygons[i].y_coords,
                             Polygons[i].vertex_count, &clip_x, &clip_y, &clip_n)) continue;
        int tc = 0;
        double* tris = triangulate_polygon(clip_x, clip_y, clip_n, &tc);
        free(clip_x); free(clip_y);
        if (!tris) continue;
        float* fc = Polygons[i].fill_color;
        glColor4f(fc[0], fc[1], fc[2], fc[3]);
        glBegin(GL_TRIANGLES);
        for (int t = 0; t < tc; t++) {
            glVertex2d(tris[t * 6 + 0], tris[t * 6 + 1]);
            glVertex2d(tris[t * 6 + 2], tris[t * 6 + 3]);
            glVertex2d(tris[t * 6 + 4], tris[t * 6 + 5]);
        }
        glEnd();
        free(tris);
    }

    // ── Заливка треугольников ──
    for (int i = 0; i < TriangleCount; i++) {
        if (!Triangles[i].visible || !Triangles[i].fill_enabled) continue;
        double px[3] = { Triangles[i].x0, Triangles[i].x1, Triangles[i].x2 };
        double py[3] = { Triangles[i].y0, Triangles[i].y1, Triangles[i].y2 };
        double* clip_x = NULL, *clip_y = NULL; int clip_n = 0;
        if (!clip_polygon_sh(px, py, 3, &clip_x, &clip_y, &clip_n)) continue;
        int tc = 0;
        double* tris = triangulate_polygon(clip_x, clip_y, clip_n, &tc);
        free(clip_x); free(clip_y);
        if (!tris) continue;
        float* fc = Triangles[i].fill_color;
        glColor4f(fc[0], fc[1], fc[2], fc[3]);
        glBegin(GL_TRIANGLES);
        for (int t = 0; t < tc; t++) {
            glVertex2d(tris[t * 6 + 0], tris[t * 6 + 1]);
            glVertex2d(tris[t * 6 + 2], tris[t * 6 + 3]);
            glVertex2d(tris[t * 6 + 4], tris[t * 6 + 5]);
        }
        glEnd();
        free(tris);
    }

    // ── Заливка эллипсов ──
    for (int i = 0; i < EllipseCount; i++) {
        if (!Ellipses[i].visible || !Ellipses[i].fill_enabled) continue;
        float* fc = Ellipses[i].fill_color;
        glColor4f(fc[0], fc[1], fc[2], fc[3]);
        double cx = Ellipses[i].x, cy = Ellipses[i].y, a = Ellipses[i].a, b = Ellipses[i].b;
        glBegin(GL_TRIANGLES);
        for (int j = 0; j < SEG; j++) {
            double a0 = 2.0 * PI * j / SEG;
            double a1 = 2.0 * PI * (j + 1) / SEG;
            double vx[3] = { cx, cx + a * cos(a0), cx + a * cos(a1) };
            double vy[3] = { cy, cy + b * sin(a0), cy + b * sin(a1) };
            double* clip_x = NULL, *clip_y = NULL; int clip_n = 0;
            if (clip_polygon_sh(vx, vy, 3, &clip_x, &clip_y, &clip_n)) {
                for (int k = 1; k < clip_n - 1; k++) {
                    glVertex2d(clip_x[0], clip_y[0]);
                    glVertex2d(clip_x[k], clip_y[k]);
                    glVertex2d(clip_x[k+1], clip_y[k+1]);
                }
            }
            free(clip_x); free(clip_y);
        }
        glEnd();
    }

    // ── Заливка окружностей ──
    for (int i = 0; i < CircleCount; i++) {
        if (!Circles[i].visible || !Circles[i].fill_enabled) continue;
        float* fc = Circles[i].fill_color;
        glColor4f(fc[0], fc[1], fc[2], fc[3]);
        double cx = Circles[i].x, cy = Circles[i].y, r = Circles[i].radius;
        glBegin(GL_TRIANGLES);
        for (int j = 0; j < SEG; j++) {
            double a0 = 2.0 * PI * j / SEG;
            double a1 = 2.0 * PI * (j + 1) / SEG;
            double vx[3] = { cx, cx + r * cos(a0), cx + r * cos(a1) };
            double vy[3] = { cy, cy + r * sin(a0), cy + r * sin(a1) };
            double* clip_x = NULL, *clip_y = NULL; int clip_n = 0;
            if (clip_polygon_sh(vx, vy, 3, &clip_x, &clip_y, &clip_n)) {
                for (int k = 1; k < clip_n - 1; k++) {
                    glVertex2d(clip_x[0], clip_y[0]);
                    glVertex2d(clip_x[k], clip_y[k]);
                    glVertex2d(clip_x[k+1], clip_y[k+1]);
                }
            }
            free(clip_x); free(clip_y);
        }
        glEnd();
    }

    // ════════════════════════════════════════════════════════
    //  ФАЗА 2: ВСЕ КОНТУРЫ
    // ════════════════════════════════════════════════════════

    // ── Контуры полигонов ──
    for (int i = 0; i < PolygonCount; i++) {
        if (!Polygons[i].visible) continue;
        glColor3fv(Polygons[i].color); glLineWidth(Polygons[i].width);
        int vc = Polygons[i].vertex_count;
        glBegin(GL_LINES);
        for (int j = 0; j < vc; j++) {
            double ax = Polygons[i].x_coords[j], ay = Polygons[i].y_coords[j];
            double bx = Polygons[i].x_coords[(j + 1) % vc], by = Polygons[i].y_coords[(j + 1) % vc];
            if (clip_line_lb(&ax, &ay, &bx, &by)) { glVertex2d(ax, ay); glVertex2d(bx, by); }
        }
        glEnd();
    }

    // ── Контуры треугольников ──
    for (int i = 0; i < TriangleCount; i++) {
        if (!Triangles[i].visible) continue;
        glColor3fv(Triangles[i].color); glLineWidth(Triangles[i].width);
        double px[3] = { Triangles[i].x0, Triangles[i].x1, Triangles[i].x2 };
        double py[3] = { Triangles[i].y0, Triangles[i].y1, Triangles[i].y2 };
        glBegin(GL_LINES);
        for (int e = 0; e < 3; e++) {
            double ax = px[e], ay = py[e];
            double bx = px[(e + 1) % 3], by = py[(e + 1) % 3];
            if (clip_line_lb(&ax, &ay, &bx, &by)) { glVertex2d(ax, ay); glVertex2d(bx, by); }
        }
        glEnd();
    }

    // ── Контуры эллипсов ──
    for (int i = 0; i < EllipseCount; i++) {
        if (!Ellipses[i].visible) continue;
        glColor3fv(Ellipses[i].color); glLineWidth(Ellipses[i].width);
        glBegin(GL_LINES);
        for (int j = 0; j < SEG; j++) {
            double a0 = 2.0 * PI * j / SEG, a1 = 2.0 * PI * (j + 1) / SEG;
            double ax = Ellipses[i].x + Ellipses[i].a * cos(a0);
            double ay = Ellipses[i].y + Ellipses[i].b * sin(a0);
            double bx = Ellipses[i].x + Ellipses[i].a * cos(a1);
            double by = Ellipses[i].y + Ellipses[i].b * sin(a1);
            if (clip_line_lb(&ax, &ay, &bx, &by)) { glVertex2d(ax, ay); glVertex2d(bx, by); }
        }
        glEnd();
    }

    // ── Контуры окружностей ──
    for (int i = 0; i < CircleCount; i++) {
        if (!Circles[i].visible) continue;
        glColor3fv(Circles[i].color); glLineWidth(Circles[i].width);
        glBegin(GL_LINES);
        for (int j = 0; j < SEG; j++) {
            double a0 = 2.0 * PI * j / SEG, a1 = 2.0 * PI * (j + 1) / SEG;
            double ax = Circles[i].x + Circles[i].radius * cos(a0);
            double ay = Circles[i].y + Circles[i].radius * sin(a0);
            double bx = Circles[i].x + Circles[i].radius * cos(a1);
            double by = Circles[i].y + Circles[i].radius * sin(a1);
            if (clip_line_lb(&ax, &ay, &bx, &by)) { glVertex2d(ax, ay); glVertex2d(bx, by); }
        }
        glEnd();
    }

    // ── Стрелки ──
    for (int i = 0; i < ArrowCount; i++) {
        if (!Arrows[i].visible) continue;
        glColor3fv(Arrows[i].color); glLineWidth(Arrows[i].width);
        double ox2 = Arrows[i].xFinish, oy2 = Arrows[i].yFinish;
        double ax = Arrows[i].xStart, ay = Arrows[i].yStart;
        double bx = ox2, by = oy2;
        if (clip_line_lb(&ax, &ay, &bx, &by)) {
            glBegin(GL_LINES); glVertex2d(ax, ay); glVertex2d(bx, by); glEnd();
            if (point_inside(ox2, oy2)) {
                double dx = ox2 - Arrows[i].xStart, dy = oy2 - Arrows[i].yStart;
                double len = sqrt(dx * dx + dy * dy);
                if (len > 0.0) {
                    dx /= len; dy /= len;
                    double as = 0.1 * len; if (as > 0.3) as = 0.3;
                    double px = -dy, py = dx;
                    double fx = ox2, fy = oy2;
                    double lx1 = fx - dx * as + px * as * 0.5;
                    double ly1 = fy - dy * as + py * as * 0.5;
                    double lx2 = fx - dx * as - px * as * 0.5;
                    double ly2 = fy - dy * as - py * as * 0.5;
                    double f2x = fx, f2y = fy;
                    glBegin(GL_LINES);
                    if (clip_line_lb(&fx, &fy, &lx1, &ly1)) { glVertex2d(fx, fy); glVertex2d(lx1, ly1); }
                    if (clip_line_lb(&f2x, &f2y, &lx2, &ly2)) { glVertex2d(f2x, f2y); glVertex2d(lx2, ly2); }
                    glEnd();
                }
            }
        }
    }

    // ── Ломаные ──
    for (int i = 0; i < BrokenLineCount; i++) {
        if (!BrokenLines[i].visible) continue;
        glColor3fv(BrokenLines[i].color); glLineWidth(BrokenLines[i].width);
        glBegin(GL_LINES);
        for (int j = 0; j < BrokenLines[i].vertex_count - 1; j++) {
            double ax = BrokenLines[i].x_coords[j], ay = BrokenLines[i].y_coords[j];
            double bx = BrokenLines[i].x_coords[j + 1], by = BrokenLines[i].y_coords[j + 1];
            if (clip_line_lb(&ax, &ay, &bx, &by)) { glVertex2d(ax, ay); glVertex2d(bx, by); }
        }
        glEnd();
    }

    // ── Линии ──
    for (int i = 0; i < LineCount; i++) {
        if (!Lines[i].visible) continue;
        glColor3fv(Lines[i].color); glLineWidth(Lines[i].width);
        double ax = Lines[i].xStart, ay = Lines[i].yStart;
        double bx = Lines[i].xFinish, by = Lines[i].yFinish;
        if (clip_line_lb(&ax, &ay, &bx, &by)) {
            glBegin(GL_LINES); glVertex2d(ax, ay); glVertex2d(bx, by); glEnd();
        }
    }

    // ── Точки ──
    for (int i = 0; i < PointCount; i++) {
        if (!Points[i].visible || !point_inside(Points[i].x, Points[i].y)) continue;
        glColor3fv(Points[i].color); glPointSize(Points[i].size);
        glBegin(GL_POINTS); glVertex2d(Points[i].x, Points[i].y); glEnd();
    }

    // ── Текст ──
    for (int i = 0; i < OutTextCount; i++) {
        if (!OutTexts[i].visible || !point_inside(OutTexts[i].x, OutTexts[i].y)) continue;
        glColor3fv(OutTexts[i].color);
        draw_coord_label(OutTexts[i].x, OutTexts[i].y, OutTexts[i].text);
    }

    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_BLEND);
    glutSwapBuffers();
}

// ============================================================
//  Клавиатура
// ============================================================

static void keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    switch (key) {
        case ' ':
            if (!groups_count) break;
            eg2d_show_only_group(active_view_group);
            if (++active_view_group >= groups_count) active_view_group = 0;
            break;
        case 'a': case 'A': eg2d_show_all_groups(); break;
        case 'h': case 'H': eg2d_hide_all_groups(); break;
        case 'g': case 'G': grid_enabled = !grid_enabled; glutPostRedisplay(); break;
        case 'm': case 'M': grid_mode = (grid_mode + 1) % 3; glutPostRedisplay(); break;
        case 'c': case 'C': if (groups_count > 0) eg2d_clear_group(active_view_group); break;
        case 27: eg2d_cleanup_all(); exit(0); break;
    }
}

// ============================================================
//  Настройка окна и осей
// ============================================================

void eg2d_window_position(int x, int y) { win_pos_x = x; win_pos_y = y; }
void eg2d_window(int w, int h)           { window_width = w; window_height = h; }

void eg2d_axis(double xmin, double xmax, double ymin, double ymax) {
    axis_limits[0] = xmin; axis_limits[1] = xmax;
    axis_limits[2] = ymin; axis_limits[3] = ymax;
    int argc = 1; char* argv[1] = { (char*)"" };
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(win_pos_x, win_pos_y);
    glutCreateWindow("EasyGLUT2D");
    glClearColor(bg_color[0], bg_color[1], bg_color[2], 1.0f);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glut_initialized = true;
    eg2d_create_group();
}

void eg2d_set_background_color(int r, int g, int b) {
    bg_color[0] = r / 255.0f; bg_color[1] = g / 255.0f; bg_color[2] = b / 255.0f;
    if (glut_initialized) { glClearColor(bg_color[0], bg_color[1], bg_color[2], 1.0f); glutPostRedisplay(); }
}

static bool _streq_ci(const char* a, const char* b) {
    while (*a && *b) {
        char ca = (*a >= 'a' && *a <= 'z') ? (*a - 32) : *a;
        char cb = (*b >= 'a' && *b <= 'z') ? (*b - 32) : *b;
        if (ca != cb) return false; a++; b++;
    }
    return *a == *b;
}

void eg2d_grid_visibility(const char* c) {
    if      (_streq_ci(c, "ON") || _streq_ci(c, "On") || _streq_ci(c, "on") || !strcmp(c, "1")) grid_enabled = true;
    else if (_streq_ci(c, "OFF") || _streq_ci(c, "Off") || _streq_ci(c, "off") || !strcmp(c, "0")) grid_enabled = false;
    else { fprintf(stderr, "grid_visibility: неверное значение '%s'\n", c); return; }
    if (glut_initialized) glutPostRedisplay();
}

void eg2d_grid_mode_set(const char* m) {
    if      (_streq_ci(m, "CM") || _streq_ci(m, "Cm") || _streq_ci(m, "cm"))   grid_mode = 0;
    else if (_streq_ci(m, "MM") || _streq_ci(m, "Mm") || _streq_ci(m, "mm"))   grid_mode = 1;
    else if (_streq_ci(m, "BOTH")) grid_mode = 2;
    else { fprintf(stderr, "grid_mode_set: неверное значение '%s'\n", m); return; }
    if (glut_initialized) glutPostRedisplay();
}

// ============================================================
//  Функции создания примитивов
// ============================================================

void eg2d_outtext(double x, double y, const char* text) {
    if (!text) return;
    OutText* t = realloc(OutTexts, (size_t)(OutTextCount + 1) * sizeof(OutText));
    if (!t) return; OutTexts = t;
    OutTexts[OutTextCount].text = malloc(strlen(text) + 1);
    if (!OutTexts[OutTextCount].text) return;
    strcpy(OutTexts[OutTextCount].text, text);
    OutTexts[OutTextCount].x = x; OutTexts[OutTextCount].y = y;
    memcpy(OutTexts[OutTextCount].color, default_text_color, 3 * sizeof(float));
    OutTexts[OutTextCount].group_id = current_group;
    OutTexts[OutTextCount].visible  = true;
    OutTextCount++;
    if (glut_initialized) glutPostRedisplay();
}
void eg2d_set_outtext_color(int r, int g, int b) {
    default_text_color[0] = r / 255.0f; default_text_color[1] = g / 255.0f; default_text_color[2] = b / 255.0f;
}

void eg2d_point(double x, double y) {
    Point* t = realloc(Points, (size_t)(PointCount + 1) * sizeof(Point));
    if (!t) return; Points = t;
    Points[PointCount].x = x; Points[PointCount].y = y;
    memcpy(Points[PointCount].color, default_point_color, 3 * sizeof(float));
    Points[PointCount].size = default_point_size;
    Points[PointCount].group_id = current_group; Points[PointCount].visible = true;
    PointCount++; if (glut_initialized) glutPostRedisplay();
}
void eg2d_set_point_size(float s) { default_point_size = s; }
void eg2d_set_point_color(int r, int g, int b) {
    default_point_color[0] = r / 255.0f; default_point_color[1] = g / 255.0f; default_point_color[2] = b / 255.0f;
}

void eg2d_line(double x1, double y1, double x2, double y2) {
    Line* t = realloc(Lines, (size_t)(LineCount + 1) * sizeof(Line));
    if (!t) return; Lines = t;
    Lines[LineCount].xStart = x1; Lines[LineCount].yStart = y1;
    Lines[LineCount].xFinish = x2; Lines[LineCount].yFinish = y2;
    memcpy(Lines[LineCount].color, default_line_color, 3 * sizeof(float));
    Lines[LineCount].width = default_line_width;
    Lines[LineCount].group_id = current_group; Lines[LineCount].visible = true;
    LineCount++; if (glut_initialized) glutPostRedisplay();
}
void eg2d_set_line_width(float w) { default_line_width = w; }
void eg2d_set_line_color(int r, int g, int b) {
    default_line_color[0] = r / 255.0f; default_line_color[1] = g / 255.0f; default_line_color[2] = b / 255.0f;
}

void eg2d_broken_line(double* xc, double* yc, int vc) {
    if (vc < 2) { fprintf(stderr, "broken_line: минимум 2 вершины\n"); return; }
    BrokenLine* t = realloc(BrokenLines, (size_t)(BrokenLineCount + 1) * sizeof(BrokenLine));
    if (!t) return; BrokenLines = t;
    BrokenLine* bl = &BrokenLines[BrokenLineCount++];
    bl->x_coords = malloc((size_t)vc * sizeof(double));
    bl->y_coords = malloc((size_t)vc * sizeof(double));
    if (!bl->x_coords || !bl->y_coords) { free(bl->x_coords); free(bl->y_coords); BrokenLineCount--; return; }
    memcpy(bl->x_coords, xc, (size_t)vc * sizeof(double));
    memcpy(bl->y_coords, yc, (size_t)vc * sizeof(double));
    bl->vertex_count = vc;
    memcpy(bl->color, default_broken_line_color, 3 * sizeof(float));
    bl->width = default_broken_line_width;
    bl->group_id = current_group; bl->visible = true;
    if (glut_initialized) glutPostRedisplay();
}
void eg2d_set_broken_line_width(float w) { default_broken_line_width = w; }
void eg2d_set_broken_line_color(int r, int g, int b) {
    default_broken_line_color[0] = r / 255.0f; default_broken_line_color[1] = g / 255.0f; default_broken_line_color[2] = b / 255.0f;
}

void eg2d_arrow(double x1, double y1, double x2, double y2) {
    Arrow* t = realloc(Arrows, (size_t)(ArrowCount + 1) * sizeof(Arrow));
    if (!t) return; Arrows = t;
    Arrows[ArrowCount].xStart = x1; Arrows[ArrowCount].yStart = y1;
    Arrows[ArrowCount].xFinish = x2; Arrows[ArrowCount].yFinish = y2;
    memcpy(Arrows[ArrowCount].color, default_arrow_color, 3 * sizeof(float));
    Arrows[ArrowCount].width = default_arrow_width;
    Arrows[ArrowCount].group_id = current_group; Arrows[ArrowCount].visible = true;
    ArrowCount++; if (glut_initialized) glutPostRedisplay();
}
void eg2d_set_arrow_width(float w) { default_arrow_width = w; }
void eg2d_set_arrow_color(int r, int g, int b) {
    default_arrow_color[0] = r / 255.0f; default_arrow_color[1] = g / 255.0f; default_arrow_color[2] = b / 255.0f;
}

void eg2d_circle(double x, double y, double r) {
    Circle* t = realloc(Circles, (size_t)(CircleCount + 1) * sizeof(Circle));
    if (!t) return; Circles = t;
    Circles[CircleCount].x = x; Circles[CircleCount].y = y; Circles[CircleCount].radius = r;
    memcpy(Circles[CircleCount].color, default_circle_color, 3 * sizeof(float));
    Circles[CircleCount].width = default_circle_width;
    Circles[CircleCount].fill_enabled = default_circle_fill_enabled;
    memcpy(Circles[CircleCount].fill_color, default_circle_fill_color, 4 * sizeof(float));
    Circles[CircleCount].group_id = current_group; Circles[CircleCount].visible = true;
    CircleCount++; if (glut_initialized) glutPostRedisplay();
}
void eg2d_set_circle_width(float w) { default_circle_width = w; }
void eg2d_set_circle_color(int r, int g, int b) {
    default_circle_color[0] = r / 255.0f; default_circle_color[1] = g / 255.0f; default_circle_color[2] = b / 255.0f;
}
void eg2d_set_circle_fill(int r, int g, int b, float alpha) {
    default_circle_fill_color[0] = r / 255.0f; default_circle_fill_color[1] = g / 255.0f;
    default_circle_fill_color[2] = b / 255.0f; default_circle_fill_color[3] = fmaxf(0.0f, fminf(1.0f, alpha));
}
void eg2d_enable_circle_fill(bool enable) {
    default_circle_fill_enabled = enable;
    for (int i = 0; i < CircleCount; i++) if (Circles[i].group_id == current_group) Circles[i].fill_enabled = enable;
    if (glut_initialized) glutPostRedisplay();
}

void eg2d_ellipse(double x, double y, double a, double b) {
    Ellipse* t = realloc(Ellipses, (size_t)(EllipseCount + 1) * sizeof(Ellipse));
    if (!t) return; Ellipses = t;
    Ellipses[EllipseCount].x = x; Ellipses[EllipseCount].y = y;
    Ellipses[EllipseCount].a = a; Ellipses[EllipseCount].b = b;
    memcpy(Ellipses[EllipseCount].color, default_ellipse_color, 3 * sizeof(float));
    Ellipses[EllipseCount].width = default_ellipse_width;
    Ellipses[EllipseCount].fill_enabled = default_ellipse_fill_enabled;
    memcpy(Ellipses[EllipseCount].fill_color, default_ellipse_fill_color, 4 * sizeof(float));
    Ellipses[EllipseCount].group_id = current_group; Ellipses[EllipseCount].visible = true;
    EllipseCount++; if (glut_initialized) glutPostRedisplay();
}
void eg2d_set_ellipse_width(float w) { default_ellipse_width = w; }
void eg2d_set_ellipse_color(int r, int g, int b) {
    default_ellipse_color[0] = r / 255.0f; default_ellipse_color[1] = g / 255.0f; default_ellipse_color[2] = b / 255.0f;
}
void eg2d_set_ellipse_fill(int r, int g, int b, float alpha) {
    default_ellipse_fill_color[0] = r / 255.0f; default_ellipse_fill_color[1] = g / 255.0f;
    default_ellipse_fill_color[2] = b / 255.0f; default_ellipse_fill_color[3] = fmaxf(0.0f, fminf(1.0f, alpha));
}
void eg2d_enable_ellipse_fill(bool enable) {
    default_ellipse_fill_enabled = enable;
    for (int i = 0; i < EllipseCount; i++) if (Ellipses[i].group_id == current_group) Ellipses[i].fill_enabled = enable;
    if (glut_initialized) glutPostRedisplay();
}

void eg2d_triangle(double x0, double y0, double x1, double y1, double x2, double y2) {
    Triangle* t = realloc(Triangles, (size_t)(TriangleCount + 1) * sizeof(Triangle));
    if (!t) return; Triangles = t;
    Triangles[TriangleCount].x0 = x0; Triangles[TriangleCount].y0 = y0;
    Triangles[TriangleCount].x1 = x1; Triangles[TriangleCount].y1 = y1;
    Triangles[TriangleCount].x2 = x2; Triangles[TriangleCount].y2 = y2;
    memcpy(Triangles[TriangleCount].color, default_triangle_color, 3 * sizeof(float));
    Triangles[TriangleCount].width = default_triangle_width;
    Triangles[TriangleCount].fill_enabled = default_triangle_fill_enabled;
    memcpy(Triangles[TriangleCount].fill_color, default_triangle_fill_color, 4 * sizeof(float));
    Triangles[TriangleCount].group_id = current_group; Triangles[TriangleCount].visible = true;
    TriangleCount++; if (glut_initialized) glutPostRedisplay();
}
void eg2d_set_triangle_width(float w) { default_triangle_width = w; }
void eg2d_set_triangle_color(int r, int g, int b) {
    default_triangle_color[0] = r / 255.0f; default_triangle_color[1] = g / 255.0f; default_triangle_color[2] = b / 255.0f;
}
void eg2d_set_triangle_fill(int r, int g, int b, float alpha) {
    default_triangle_fill_color[0] = r / 255.0f; default_triangle_fill_color[1] = g / 255.0f;
    default_triangle_fill_color[2] = b / 255.0f; default_triangle_fill_color[3] = fmaxf(0.0f, fminf(1.0f, alpha));
}
void eg2d_enable_triangle_fill(bool enable) {
    default_triangle_fill_enabled = enable;
    for (int i = 0; i < TriangleCount; i++) if (Triangles[i].group_id == current_group) Triangles[i].fill_enabled = enable;
    if (glut_initialized) glutPostRedisplay();
}

void eg2d_polygon(double* xc, double* yc, int vc) {
    if (vc < 3) { fprintf(stderr, "polygon: минимум 3 вершины\n"); return; }
    Polygon* t = realloc(Polygons, (size_t)(PolygonCount + 1) * sizeof(Polygon));
    if (!t) return; Polygons = t;
    Polygon* pg = &Polygons[PolygonCount++];
    pg->x_coords = malloc((size_t)vc * sizeof(double));
    pg->y_coords = malloc((size_t)vc * sizeof(double));
    if (!pg->x_coords || !pg->y_coords) { free(pg->x_coords); free(pg->y_coords); PolygonCount--; return; }
    memcpy(pg->x_coords, xc, (size_t)vc * sizeof(double));
    memcpy(pg->y_coords, yc, (size_t)vc * sizeof(double));
    pg->vertex_count = vc;
    memcpy(pg->color, default_polygon_color, 3 * sizeof(float));
    pg->width = default_polygon_width;
    pg->fill_enabled = default_polygon_fill_enabled;
    memcpy(pg->fill_color, default_polygon_fill_color, 4 * sizeof(float));
    pg->group_id = current_group; pg->visible = true;
    if (glut_initialized) glutPostRedisplay();
}
void eg2d_set_polygon_width(float w) { default_polygon_width = w; }
void eg2d_set_polygon_color(int r, int g, int b) {
    default_polygon_color[0] = r / 255.0f; default_polygon_color[1] = g / 255.0f; default_polygon_color[2] = b / 255.0f;
}
void eg2d_set_polygon_fill(int r, int g, int b, float alpha) {
    default_polygon_fill_color[0] = r / 255.0f; default_polygon_fill_color[1] = g / 255.0f;
    default_polygon_fill_color[2] = b / 255.0f; default_polygon_fill_color[3] = fmaxf(0.0f, fminf(1.0f, alpha));
}
void eg2d_enable_polygon_fill(bool enable) {
    default_polygon_fill_enabled = enable;
    for (int i = 0; i < PolygonCount; i++) if (Polygons[i].group_id == current_group) Polygons[i].fill_enabled = enable;
    if (glut_initialized) glutPostRedisplay();
}

// ============================================================
//  Очистка памяти
// ============================================================

void eg2d_cleanup_all(void) {
    free(Points);    Points    = NULL; PointCount    = 0;
    free(Lines);     Lines     = NULL; LineCount     = 0;
    free(Arrows);    Arrows    = NULL; ArrowCount    = 0;
    free(Circles);   Circles   = NULL; CircleCount   = 0;
    free(Ellipses);  Ellipses  = NULL; EllipseCount  = 0;
    free(Triangles); Triangles = NULL; TriangleCount = 0;
    for (int i = 0; i < BrokenLineCount; i++) { free(BrokenLines[i].x_coords); free(BrokenLines[i].y_coords); }
    free(BrokenLines); BrokenLines = NULL; BrokenLineCount = 0;
    for (int i = 0; i < PolygonCount; i++) { free(Polygons[i].x_coords); free(Polygons[i].y_coords); }
    free(Polygons); Polygons = NULL; PolygonCount = 0;
    for (int i = 0; i < OutTextCount; i++) free(OutTexts[i].text);
    free(OutTexts); OutTexts = NULL; OutTextCount = 0;
    groups_count = 0; current_group = 0; active_view_group = 0; glut_initialized = false;
}

void eg2d_show(void) { glutMainLoop(); }
