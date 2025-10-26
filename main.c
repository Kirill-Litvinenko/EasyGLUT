#include "EasyGLUT.h"

int main() {

    window(1000, 620); // Изменение размера окна
    axis(-5, 5, -3, 3); // Задание границ

    grid_visibility("ON");

    // Смена цвета точек
    set_point_color(200, 0, 0);
    set_point_size(10);

    // Добавление точек
    point(1, 1.);
    point(-2, 0.5);

    set_point_color(0, 200, 0);
    set_point_size(15);
    point(-4,-2);
    point(5.3, -1); // Не отображается из-за выхода за границы
    point(3, 3);

    // Смена цвета линий;
    set_line_color(0, 0, 255);

    // Добавление линии
    line(-1, 0, -1, 2);
    line(-7, -1, 2, 2); // Не отобразится из-за выхода за границы
    line(3, -3, 5, 0);

    // Добавление окружности
    set_circle_color(150, 0, 150);
    circle(1, 0, 2);

    // Добавление треугольника
    set_triangle_color(0, 200, 0);
    triangle(-1, -1, 0, 0, 1, 0);

    // Добавление многоугольника
    set_polygon_color(200, 100, 0);
    float x[4] = {-4, -2, -2, -5};
    float y[4] = {-2, -2, 2, 2};
    polygon(x, y, 4);

    // Добавление ломаной линии
    set_broken_line_color(255, 255, 0);
    float a[3] = {1, 0, -2};
    float b[3] = {1, 0, 3};
    broken_line(a, b, 3);

    // Добавление вектора
    set_arrow_width(1);
    arrow(0,0,4,1);

    // Добавление эллипсиса
    ellipse(1, 2, 3, 1);

    // Вывод на экран
    show();

    return 0;
}
