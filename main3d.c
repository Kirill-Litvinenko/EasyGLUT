#include "EasyGLUT3D.h"

int main(void) {
    window3d(1920, 1080);
    window_position3d(0, 0);

    axis3d(-5, 5, -5, 5, -5, 5);

    axes_colored3d("ON");

    camera_control3d("ON");

    grid_visibility_enable("OFF");

    arrow3d(1, 0, 1, 1, 4, 5);

    set_line3d_color(165,42,42);
    set_line3d_width(3);
    line3d(-4,0,0, 4,3,2);

    set_point3d_color(255,0,0);
    set_point3d_size(10);
    point3d(0,0,0);
    point3d(3, 2, -3);

    set_sphere3d_mode("WIRE");
    set_sphere3d_color(255,0,0);
    sphere3d(0,1.2f,0, 1.0f);

    set_cube3d_mode("SOLID");
    set_cube3d_color(0, 255, 0);
    cube3d(2,1,1, 1.5f);

    set_cylinder3d_color(0, 0, 255);
    set_cylinder3d_mode("WIRE");
    cylinder3d(-2,1,0, 0.6f, 2.0f);

    set_cone3d_color(40, 140, 30);
    cone3d(2, -1, 2, 1, 2);

    set_outtext_color(255, 100, 0);
    outtext(3, 3, 3, "Hello World! * / . | @ % & ? < > - _ 11");

    torus3d(3, -3, 3, 0.5, 1);
    show3d();
    return 0;
}
