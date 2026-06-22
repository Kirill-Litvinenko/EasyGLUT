EasyGLUT — библиотеки для 2D и 3D графики

Две простые C-библиотеки для рисования графики через OpenGL и FreeGLUT.
EasyGLUT2D — для плоских графиков и фигур.
EasyGLUT3D — для трёхмерных сцен с управляемой камерой.
Что нужно установить

sudo apt install build-essential libgl-dev libglu1-mesa-dev freeglut3-dev

Как скомпилировать библиотеку

В папке с библиотекой лежит Makefile. Перед сборкой убедитесь, что в нём прописан нужный файл:

SRC = EasyGLUT2D.c        # или EasyGLUT3D.c
HDR = EasyGLUT2D.h        # или EasyGLUT3D.h
STATIC_LIB = libEasyGLUT2D.a
SHARED_LIB = libEasyGLUT2D.so

Затем запустите одну из команд:
Команда   Что делает
make  Собрать обе библиотеки (.a и .so)
make static   Только статическая библиотека (.a)
make shared   Только динамическая библиотека (.so)
make clean  Удалить скомпилированные файлы
Как скомпилировать свою программу

# Для 2D
gcc main.c -L. -lEasyGLUT2D -lGL -lGLU -lglut -lm -o my_app

# Для 3D
gcc main.c -L. -lEasyGLUT3D -lGL -lGLU -lglut -lm -o my_app

    Если используете динамическую библиотеку (.so), перед запуском выполните:

    export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

EasyGLUT2D
Минимальный пример

#include "EasyGLUT2D.h"

int main(void) {
    eg2d_window(1280, 720);              // размер окна
    eg2d_axis(-5.0, 5.0, -5.0, 5.0);   // диапазон координат, создаёт окно

    eg2d_set_point_color(255, 0, 0);
    eg2d_point(0.0, 0.0);               // красная точка в центре

    eg2d_show();                         // запустить окно
    return 0;
}

Настройка окна
Функция   Описание  По умолчанию
eg2d_window(w, h)   Размер окна в пикселях  1920 × 1080
eg2d_window_position(x, y)  Позиция окна на экране  0, 0
eg2d_axis(xmin, xmax, ymin, ymax)   Диапазон осей + инициализация   −1, 1, −1, 1
eg2d_set_background_color(r, g, b)  Цвет фона (0–255)   белый
eg2d_grid_visibility("ON"/"OFF")  Показать / скрыть сетку   ON
eg2d_grid_mode_set("CM"/"MM"/"BOTH")  Режим сетки: крупная / мелкая / обе   CM
eg2d_show()   Запустить окно (вызывать последним)   —
eg2d_cleanup_all()  Освободить память   —

    ⚠️ eg2d_axis нужно вызывать до добавления фигур. eg2d_window и eg2d_window_position — до eg2d_axis.

Фигуры и их функции
Точка

eg2d_point(x, y);

Настройка   Функция   По умолчанию
Цвет  eg2d_set_point_color(r, g, b)   чёрный
Размер  eg2d_set_point_size(size)   7.0
Линия

eg2d_line(x1, y1, x2, y2);

Настройка   Функция   По умолчанию
Цвет  eg2d_set_line_color(r, g, b)  чёрный
Толщина   eg2d_set_line_width(w)  5.0
Ломаная линия

double xs[] = {0, 1, 2, 3};
double ys[] = {0, 1, 0, 1};
eg2d_broken_line(xs, ys, 4);   // минимум 2 точки

Настройка   Функция   По умолчанию
Цвет  eg2d_set_broken_line_color(r, g, b)   чёрный
Толщина   eg2d_set_broken_line_width(w)   5.0
Стрелка

eg2d_arrow(x1, y1, x2, y2);   // стрелка от (x1,y1) к (x2,y2)

Настройка   Функция   По умолчанию
Цвет  eg2d_set_arrow_color(r, g, b)   чёрный
Толщина   eg2d_set_arrow_width(w)   5.0
Окружность

eg2d_circle(x, y, radius);

Настройка   Функция   По умолчанию
Цвет контура  eg2d_set_circle_color(r, g, b)  чёрный
Толщина   eg2d_set_circle_width(w)  5.0
Включить заливку  eg2d_enable_circle_fill(true/false)   выключена
Цвет заливки  eg2d_set_circle_fill(r, g, b, alpha)  синий, alpha=0.3

    alpha — прозрачность от 0.0 (невидимый) до 1.0 (непрозрачный).

Эллипс

eg2d_ellipse(x, y, a, b);   // a — полуось X, b — полуось Y

Настройка   Функция   По умолчанию
Цвет контура  eg2d_set_ellipse_color(r, g, b)   чёрный
Толщина   eg2d_set_ellipse_width(w)   5.0
Включить заливку  eg2d_enable_ellipse_fill(true/false)  выключена
Цвет заливки  eg2d_set_ellipse_fill(r, g, b, alpha)   синий, alpha=0.3
Треугольник

eg2d_triangle(x0, y0, x1, y1, x2, y2);

Настройка   Функция   По умолчанию
Цвет контура  eg2d_set_triangle_color(r, g, b)  чёрный
Толщина   eg2d_set_triangle_width(w)  4.0
Включить заливку  eg2d_enable_triangle_fill(true/false)   выключена
Цвет заливки  eg2d_set_triangle_fill(r, g, b, alpha)  красный, alpha=0.3
Полигон (многоугольник)

double xs[] = {0, 2, 3, 1, -1};
double ys[] = {2, 2, 0, -1,  0};
eg2d_polygon(xs, ys, 5);   // минимум 3 точки

Настройка   Функция   По умолчанию
Цвет контура  eg2d_set_polygon_color(r, g, b)   чёрный
Толщина   eg2d_set_polygon_width(w)   4.0
Включить заливку  eg2d_enable_polygon_fill(true/false)  выключена
Цвет заливки  eg2d_set_polygon_fill(r, g, b, alpha)   зелёный, alpha=0.3
Текст

eg2d_outtext(x, y, "Привет!");

Настройка   Функция   По умолчанию
Цвет  eg2d_set_outtext_color(r, g, b)   чёрный
Группы

Группы позволяют объединить несколько фигур и управлять их видимостью вместе.
Первая группа (ID 0) создаётся автоматически при вызове eg2d_axis.

eg2d_axis(-5, 5, -5, 5);     // автоматически создаётся группа 0

eg2d_point(0, 0);             // попадает в группу 0

int g1 = eg2d_create_group(); // создать группу 1, она становится активной
eg2d_circle(1, 1, 2);         // попадает в группу 1

eg2d_select_group(0);         // вернуться к группе 0
eg2d_line(0, 0, 1, 1);        // попадает в группу 0

Функция   Описание
eg2d_create_group()   Создать новую группу, вернуть её ID
eg2d_select_group(id)   Сделать группу активной
eg2d_get_groups_count()   Получить количество групп
eg2d_show_only_group(id)  Скрыть все, показать только эту группу
eg2d_show_all_groups()  Показать все группы
eg2d_hide_all_groups()  Скрыть все группы
eg2d_set_group_visibility(id, true/false)   Задать видимость группы
eg2d_show_groups(ids[], count)  Показать несколько групп по списку ID
eg2d_clear_group(id)  Удалить группу и все её фигуры
Горячие клавиши (2D)
Клавиша   Действие
Пробел  Показывать группы по очереди
A   Показать все группы
H   Скрыть все группы
G   Включить / выключить сетку
M   Переключить режим сетки
C   Удалить текущую группу
ESC   Закрыть программу
EasyGLUT3D
Минимальный пример

#include "EasyGLUT3D.h"

int main(void) {
    eg3d_window3d(1280, 720);
    eg3d_axis3d(-5, 5, -5, 5, -5, 5);   // диапазон осей, создаёт окно

    eg3d_set_sphere3d_color(255, 0, 0);
    eg3d_set_sphere3d_mode("SOLID");
    eg3d_sphere3d(0, 0, 0, 1.5f);        // красная сфера в центре

    eg3d_show3d();
    return 0;
}

Настройка окна и сцены
Функция   Описание  По умолчанию
eg3d_window3d(w, h)   Размер окна   1920 × 1080
eg3d_window_position3d(x, y)  Позиция окна  0, 0
eg3d_axis3d(xmin,xmax, ymin,ymax, zmin,zmax)  Диапазон осей + инициализация   —
eg3d_background_color3d(r, g, b)  Цвет фона   белый
eg3d_axes_visibility3d("ON"/"OFF")  Показать / скрыть оси X, Y, Z   ON
eg3d_axes_colored3d("ON"/"OFF")   Цветные оси (X-красный, Y-зелёный, Z-синий)   ON
eg3d_grid_visibility_enable("ON"/"OFF")   Показать / скрыть сетку   ON
eg3d_show3d()   Запустить окно  —
eg3d_cleanup_all()  Освободить память   —
Фигуры и их функции
Точка

eg3d_point3d(x, y, z);

Настройка   Функция   По умолчанию
Цвет  eg3d_set_point3d_color(r, g, b)   чёрный
Размер  eg3d_set_point3d_size(size)   7.0
Линия

eg3d_line3d(x0, y0, z0, x1, y1, z1);

Настройка   Функция   По умолчанию
Цвет  eg3d_set_line3d_color(r, g, b)  чёрный
Толщина   eg3d_set_line3d_width(w)  3.0
Ломаная линия

float xs[] = {0, 1, 2};
float ys[] = {0, 1, 0};
float zs[] = {0, 0, 1};
eg3d_polyline3d(xs, ys, zs, 3);   // минимум 2 точки

Настройка   Функция   По умолчанию
Цвет  eg3d_set_polyline3d_color(r, g, b)  чёрный
Толщина   eg3d_set_polyline3d_width(w)  3.0
Стрелка

eg3d_arrow3d(x0, y0, z0, x1, y1, z1);

Настройка   Функция   По умолчанию
Цвет  eg3d_set_arrow3d_color(r, g, b)   чёрный
Толщина   eg3d_set_arrow3d_width(w)   3.0
Сфера

eg3d_sphere3d(x, y, z, radius);

Настройка   Функция   По умолчанию
Цвет  eg3d_set_sphere3d_color(r, g, b)  чёрный
Режим   eg3d_set_sphere3d_mode("SOLID"/"WIRE")  WIRE
Качество  eg3d_set_sphere3d_quality(slices, stacks)   24, 16
Толщина сетки   eg3d_set_sphere3d_line_width(w)   2.0
Куб

eg3d_cube3d(x, y, z, size);   // x,y,z — центр куба, size — длина стороны

Настройка   Функция   По умолчанию
Цвет  eg3d_set_cube3d_color(r, g, b)  чёрный
Режим   eg3d_set_cube3d_mode("SOLID"/"WIRE")  WIRE
Толщина сетки   eg3d_set_cube3d_line_width(w)   2.0
Цилиндр

eg3d_cylinder3d(x, y, z, radius, height);

Настройка   Функция   По умолчанию
Цвет  eg3d_set_cylinder3d_color(r, g, b)  чёрный
Режим   eg3d_set_cylinder3d_mode("SOLID"/"WIRE")  WIRE
Крышки  eg3d_set_cylinder3d_caps("ON"/"OFF")  ON
Качество  eg3d_set_cylinder3d_quality(slices, stacks)   24, 4
Толщина   eg3d_set_cylinder3d_line_width(w)   2.0
Конус

eg3d_cone3d(x, y, z, radius, height);

Настройка   Функция   По умолчанию
Цвет  eg3d_set_cone3d_color(r, g, b)  чёрный
Режим   eg3d_set_cone3d_mode("SOLID"/"WIRE")  WIRE
Крышка  eg3d_set_cone3d_caps("ON"/"OFF")  ON
Качество  eg3d_set_cone3d_quality(slices, stacks)   24, 4
Толщина   eg3d_set_cone3d_line_width(w)   2.0
Тор (бублик)

eg3d_torus3d(x, y, z, inner_radius, outer_radius);

inner_radius — радиус трубки, outer_radius — радиус от центра до середины трубки.
Настройка   Функция   По умолчанию
Цвет  eg3d_set_torus3d_color(r, g, b)   чёрный
Режим   eg3d_set_torus3d_mode("SOLID"/"WIRE")   WIRE
Качество  eg3d_set_torus3d_quality(sides, rings)  16, 24
Толщина   eg3d_set_torus3d_line_width(w)  2.0
Текст

eg3d_outtext(x, y, z, "Текст");

Настройка   Функция   По умолчанию
Цвет  eg3d_set_outtext_color(r, g, b)   чёрный
Плоские фигуры в 3D пространстве

Эти фигуры плоские, но размещаются в 3D сцене. Их ориентацию задаёт нормаль — вектор, перпендикулярный плоскости фигуры.
Треугольник

eg3d_triangle2d(x0,y0,z0, x1,y1,z1, x2,y2,z2);

Настройка   Функция   По умолчанию
Цвет  eg3d_set_triangle2d_color(r, g, b)  чёрный
Режим   eg3d_set_triangle2d_mode("SOLID"/"WIRE")  WIRE
Толщина   eg3d_set_triangle2d_line_width(w)   2.0
Полигон

float xs[] = {0,1,1,0};
float ys[] = {0,0,1,1};
float zs[] = {0,0,0,0};
eg3d_polygon2d(xs, ys, zs, 4);   // минимум 3 точки

Настройка   Функция   По умолчанию
Цвет  eg3d_set_polygon2d_color(r, g, b)   чёрный
Режим   eg3d_set_polygon2d_mode("SOLID"/"WIRE")   WIRE
Толщина   eg3d_set_polygon2d_line_width(w)  2.0
Окружность

eg3d_set_circle2d_normal(0, 1, 0);   // лежит в горизонтальной плоскости
eg3d_circle2d(x, y, z, radius);

Настройка   Функция   По умолчанию
Цвет  eg3d_set_circle2d_color(r, g, b)  чёрный
Режим   eg3d_set_circle2d_mode("SOLID"/"WIRE")  WIRE
Нормаль   eg3d_set_circle2d_normal(nx, ny, nz)  0, 1, 0
Сегменты  eg3d_set_circle2d_segments(n)   64
Толщина   eg3d_set_circle2d_line_width(w)   2.0
Эллипс

eg3d_set_ellipse2d_normal(0, 1, 0);
eg3d_ellipse2d(x, y, z, rx, ry);   // rx, ry — полуоси

Настройка   Функция   По умолчанию
Цвет  eg3d_set_ellipse2d_color(r, g, b)   чёрный
Режим   eg3d_set_ellipse2d_mode("SOLID"/"WIRE")   WIRE
Нормаль   eg3d_set_ellipse2d_normal(nx, ny, nz)   0, 1, 0
Поворот   eg3d_set_ellipse2d_angle(degrees)   0°
Сегменты  eg3d_set_ellipse2d_segments(n)  64
Толщина   eg3d_set_ellipse2d_line_width(w)  2.0
Камера

Камера орбитальная — она всегда смотрит в одну точку и вращается вокруг неё.
Программная настройка

eg3d_camera3d(
    ex, ey, ez,   // откуда смотрит камера
    cx, cy, cz,   // на что смотрит
    ux, uy, uz    // где верх (обычно 0, 1, 0)
);

Управление мышью
Действие  Результат
Зажать ЛКМ + двигать  Вращение вокруг сцены
Зажать ПКМ + двигать  Панорамирование
Колёсико  Приближение / удаление
Чувствительность

eg3d_set_camera_control_sensitivity3d(rot_sens, pan_sens, zoom_mul);
// По умолчанию: 0.25, 0.0025, 1.10

Включить / выключить управление

eg3d_camera_control3d("ON");    // по умолчанию включено
eg3d_camera_control3d("OFF");

Группы (3D)

Работают так же, как в 2D, только с префиксом eg3d_.
Функция   Описание
eg3d_create_group()   Создать новую группу
eg3d_select_group(id)   Сделать группу активной
eg3d_show_only_group(id)  Показать только эту группу
eg3d_show_all_groups()  Показать все
eg3d_hide_all_groups()  Скрыть все
eg3d_set_group_visibility(id, true/false)   Задать видимость
eg3d_show_groups(ids[], count)  Показать несколько групп
eg3d_clear_group(id)  Удалить группу и её фигуры
Горячие клавиши (3D)
Клавиша   Действие
W / S / A / D   Двигать точку обзора вперёд / назад / влево / вправо
E / Q   Двигать точку обзора вверх / вниз
R   Сбросить камеру в начальное положение
Пробел  Показывать группы по очереди
O   Показать все группы
H   Скрыть все группы
C   Удалить текущую группу
L   Показать в консоли количество групп
ESC   Закрыть программу
