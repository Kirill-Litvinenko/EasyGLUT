// Заголовочный файл библиотеки EasyGLUT

// Защита от повторного включения заголовочного файла (include guard)
// Предотвращает ошибки компиляции при многократном включении этого файла
#ifndef EASYGLUT_H 
#define EASYGLUT_H 

// Обеспечение совместимости с C++ (позволяет использовать функции в C-коде)
// Если код компилируется как C++, оборачиваем объявления в extern "C"
#ifdef __cplusplus 
extern "C" { 
    #endif 

    // Функция для изменения положения окна
    void window_position(int x_position, int y_position);

    // Функция для изменения размера окна
    void window(int width, int height);

    // Инициализация окна с осями (аналог axis() в matplotlib)
    void axis(float xmin, float xmax, float ymin, float ymax);

    // Видимость сетки
    void grid_visibility(const char* choice);

    // ==========================================================================================

    // Смена размера точек
    void set_point_size(float width);
    // Смена цвета точек (R, G, B в диапазоне от 0 до 255)
    void set_point_color(int r, int g, int b);
    // Добавление точки на график
    void point(float x, float y);

    // ==========================================================================================

    // Смена размера линии
    void set_line_width(float width);
    // Смена цвета линии (R, G, B в диапазоне от 0 до 255)
    void set_line_color(int r, int g, int b);
    // Добавление линии на график
    void line(float xStart, float yStart, float xFinish, float yFinish);

    // ==========================================================================================

    // Добавление круга на график
    void circle(float x, float y, float radius);
    // Смена толщины круга
    void set_circle_width(float width);
    // Смена цвета линии (R, G, B в диапазоне от 0 до 255)
    void set_circle_color(int r, int g, int b);

    // ==========================================================================================

    // Добавление треугольника на график
    void triangle(float x0, float y0, float x1, float y1, float x2, float y2);
    // Смена толщины треугольника
    void set_triangle_width(float width);
    // Смена цвета линии (R, G, B в диапазоне от 0 до 255)
    void set_triangle_color(int r, int g, int b);

    // ==========================================================================================

    // Добавление многоугольника на график
    void polygon(float* x_coords, float* y_coords, int vertex_count);
    // Смена толщины многоугольника
    void set_polygon_width(float width);
    // Смена цвета линии (R, G, B в диапазоне от 0 до 255)
    void set_polygon_color(int r, int g, int b);

    // ==========================================================================================

    // Функция запуска обработчика событий (основного цикла GLUT)
    void show();

    // Завершение блока extern "C" для C++
    #ifdef __cplusplus 
} 
#endif 
// Конец include guard
#endif // EASYGLUT_H 