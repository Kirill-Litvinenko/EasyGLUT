#ifndef EASYGLUT2D_H
#define EASYGLUT2D_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Создать окно и задать логическую систему координат.
   Должна вызываться первой, до любых примитивов.
   Автоматически создаёт группу 0. */
void eg2d_axis(double xmin, double xmax, double ymin, double ymax);

/* Задать размер окна в пикселях (до вызова axis()). */
void eg2d_window(int w, int h);

/* Задать позицию окна на экране (до вызова axis()). */
void eg2d_window_position(int x, int y);

/* Цвет фона окна. r, g, b — целые 0..255.
   Вызывать после axis() */
void eg2d_set_background_color(int r, int g, int b);

/* Видимость координатной сетки.
   c: "ON" | "on" | "1" | "OFF" | "off" | "0" */
void eg2d_grid_visibility(const char* c);

/* Режим сетки:
 * "CM"   — сантиметровая (шаг 1)
 * "MM"   — миллиметровая (шаг 0.1)
 * "BOTH" — обе сразу */
void eg2d_grid_mode_set(const char* m);

/* Запустить главный цикл GLUT (блокирующий вызов). */
void eg2d_show(void);

/* Освободить всю динамическую память библиотеки. */
void eg2d_cleanup_all(void);

/*
 *      Группы
 */

/* Создать новую группу. Она становится активной.
   Возвращает числовой id группы (0, 1, 2, ...). */
int  eg2d_create_group(void);

/* Сделать группу gid активной (новые примитивы идут в неё). */
void eg2d_select_group(int gid);

/* Скрыть все группы, затем показать только gid.
   gid < 0 — показать все. */
void eg2d_show_only_group(int gid);

/* Показать сразу несколько групп по массиву id. */
void eg2d_show_groups(int* ids, int count);

/* Задать видимость конкретной группы (не влияет на другие). */
void eg2d_set_group_visibility(int gid, bool visible);

/* Скрыть все примитивы всех групп. */
void eg2d_hide_all_groups(void);

/* Показать все примитивы всех групп. */
void eg2d_show_all_groups(void);

/* Удалить все примитивы группы gid и саму группу.
   Id остальных групп сдвигаются вниз. */
void eg2d_clear_group(int gid);

/* Вернуть текущее число групп. */
int  eg2d_get_groups_count(void);

/*
 *      Примитивы
 */

/* Нарисовать точку в (x, y). */
void eg2d_point(double x, double y);

/* Цвет точек. r, g, b — целые 0..255. */
void eg2d_set_point_color(int r, int g, int b);

/* Размер точки в пикселях (glPointSize). По умолчанию: 7. */
void eg2d_set_point_size(float s);


/* Провести отрезок из (x1,y1) в (x2,y2). */
void eg2d_line(double x1, double y1, double x2, double y2);

/* Цвет отрезков. r, g, b — целые 0..255. */
void eg2d_set_line_color(int r, int g, int b);

/* Толщина отрезков (glLineWidth). По умолчанию: 5. */
void eg2d_set_line_width(float w);


/* Нарисовать ломаную из vc вершин.
   xc[] и yc[] — координаты вершин, vc >= 2. */
void eg2d_broken_line(double* xc, double* yc, int vc);

/* Цвет ломаных. r, g, b — целые 0..255. */
void eg2d_set_broken_line_color(int r, int g, int b);

/* Толщина ломаных. По умолчанию: 5. */
void eg2d_set_broken_line_width(float w);


/* Нарисовать стрелку из (x1,y1) в (x2,y2).
   Наконечник — в точке (x2, y2). */
void eg2d_arrow(double x1, double y1, double x2, double y2);

/* Цвет стрелок. r, g, b — целые 0..255. */
void eg2d_set_arrow_color(int r, int g, int b);

/* Толщина стрелок. По умолчанию: 5. */
void eg2d_set_arrow_width(float w);


/* Нарисовать окружность с центром (x,y) и радиусом r. */
void eg2d_circle(double x, double y, double r);

/* Цвет контура окружностей. r, g, b — целые 0..255. */
void eg2d_set_circle_color(int r, int g, int b);

/* Толщина контура окружностей. По умолчанию: 5. */
void eg2d_set_circle_width(float w);

/* Задать цвет заливки окружностей.
   r, g, b : целые 0..255
   alpha   : вещественное 0.0 (прозрачный) .. 1.0 (непрозр.) */
void eg2d_set_circle_fill(int r, int g, int b, float alpha);

/* Включить (true) или выключить (false) заливку окружностей.
   Ретроактивно применяется ко всем окружностям
   текущей активной группы. */
void eg2d_enable_circle_fill(bool enable);


/* Нарисовать эллипс с центром (x,y),
   полуосью a по X и полуосью b по Y. */
void eg2d_ellipse(double x, double y, double a, double b);

/* Цвет контура эллипсов. r, g, b — целые 0..255. */
void eg2d_set_ellipse_color(int r, int g, int b);

/* Толщина контура эллипсов. По умолчанию: 5. */
void eg2d_set_ellipse_width(float w);

/* Задать цвет заливки эллипсов.
   r, g, b : целые 0..255
   alpha   : вещественное 0.0 .. 1.0
   Дефолт  : синий (0, 0, 255), alpha = 0.3 */
void eg2d_set_ellipse_fill(int r, int g, int b, float alpha);

/* Включить/выключить заливку эллипсов.
   Ретроактивно обновляет текущую группу. */
void eg2d_enable_ellipse_fill(bool enable);


/* Нарисовать треугольник по трём вершинам. */
void eg2d_triangle(double x0, double y0,
                   double x1, double y1,
                   double x2, double y2);

/* Цвет контура треугольников. r, g, b — целые 0..255. */
void eg2d_set_triangle_color(int r, int g, int b);

/* Толщина контура треугольников. По умолчанию: 4. */
void eg2d_set_triangle_width(float w);

/* Задать цвет заливки треугольников.
   r, g, b : целые 0..255
   alpha   : вещественное 0.0 .. 1.0
   Дефолт  : красный (255, 0, 0), alpha = 0.3 */
void eg2d_set_triangle_fill(int r, int g, int b, float alpha);

/* Включить/выключить заливку треугольников.
   Ретроактивно обновляет текущую группу. */
void eg2d_enable_triangle_fill(bool enable);


/* Нарисовать замкнутый многоугольник из vc >= 3 вершин.
   xc[] и yc[] — координаты вершин. */
void eg2d_polygon(double* xc, double* yc, int vc);

/* Цвет контура многоугольников. r, g, b — целые 0..255. */
void eg2d_set_polygon_color(int r, int g, int b);

/* Толщина контура многоугольников. По умолчанию: 4. */
void eg2d_set_polygon_width(float w);

/* Задать цвет заливки многоугольников.
   r, g, b : целые 0..255
   alpha   : вещественное 0.0 .. 1.0
   Дефолт  : зелёный (0, 255, 0), alpha = 0.3 */
void eg2d_set_polygon_fill(int r, int g, int b, float alpha);

/* Включить/выключить заливку многоугольников.
   Ретроактивно обновляет текущую группу. */
void eg2d_enable_polygon_fill(bool enable);


/* Вывести строку text с левым нижним углом в (x, y). */
void eg2d_outtext(double x, double y, const char* text);

/* Цвет текста. r, g, b — целые 0..255. */
void eg2d_set_outtext_color(int r, int g, int b);


#ifdef __cplusplus
}
#endif

#endif /* EASYGLUT2D_H */
