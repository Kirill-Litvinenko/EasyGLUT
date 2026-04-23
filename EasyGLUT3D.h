// Библиотека для упрощённой работы с 3D графикой

// Защита от повторного включения заголовочного файла (include guard)
// Предотвращает ошибки компиляции при многократном включении этого файла
#ifndef EASYGLUT3D_H
#define EASYGLUT3D_H

// Обеспечение совместимости с C++ (позволяет использовать функции в C-коде)
#include <GL/freeglut_std.h>
#ifdef __cplusplus
extern "C" {
    #endif

    #include <GL/freeglut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>

    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <math.h>
    #include <stdbool.h>

    /* ============================ Вспомогательная математика ============================ */

    typedef struct { float x, y, z; } Vec3;

    static Vec3 v3(float x, float y, float z) { Vec3 v = {x,y,z}; return v; }

    static Vec3 v3_add(Vec3 a, Vec3 b)  { return v3(a.x+b.x, a.y+b.y, a.z+b.z); }
    static Vec3 v3_sub(Vec3 a, Vec3 b)  { return v3(a.x-b.x, a.y-b.y, a.z-b.z); }
    static Vec3 v3_mul(Vec3 a, float s) { return v3(a.x*s, a.y*s, a.z*s); }

    static float v3_len(Vec3 a) { return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z); }

    static float v3_dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }

    static Vec3 v3_norm(Vec3 a) {
        float L = v3_len(a);
        if (L <= 1e-8f) return v3(0,0,0);
        return v3(a.x/L, a.y/L, a.z/L);
    }

    static Vec3 v3_cross(Vec3 a, Vec3 b) {
        return v3(
            a.y*b.z - a.z*b.y,
            a.z*b.x - a.x*b.z,
            a.x*b.y - a.y*b.x
        );
    }

    /* ============================ Управление камерой (ORBIT + FREECAM) ============================ */

    #ifndef M_PI
    #define M_PI 3.14159265358979323846
    #endif

    #define DEG_RAD(x) ((x) * (float)M_PI / 180.0f)
    #define RAD_DEG(x) ((x) * 180.0f / (float)M_PI)

    static bool camera_control_enabled = true;

    // orbit-параметры (углы в градусах)
    static float orbit_yaw_deg   = 45.0f;   // вращение вокруг Y
    static float orbit_pitch_deg = -25.0f;  // вверх/вниз
    static float orbit_dist      = 10.0f;   // расстояние eye->center

    // ограничения pitch чтобы не “переворачиваться”
    static float orbit_pitch_min = -89.0f;
    static float orbit_pitch_max =  89.0f;

    // чувствительность
    static float orbit_rot_sens = 0.25f;   // градусов на пиксель
    static float orbit_pan_sens = 0.0025f; // мировых единиц * dist на пиксель
    static float orbit_zoom_mul = 1.08f;   // множитель на один шаг колеса

    // мышь
    static int  mouse_last_x = 0;
    static int  mouse_last_y = 0;
    static bool mouse_lmb_down = false;
    static bool mouse_rmb_down = false;

    /* ============================ Примитивы и хранилища ============================ */

    // Точка 3D
    typedef struct {
        float x, y, z;
        float color[3];
        float size;
    } Point3D;
    static Point3D* Point3DS = NULL;
    static int Point3DCount = 0;

    // Линия 3D
    typedef struct {
        float x0,y0,z0;
        float x1,y1,z1;
        float color[3];
        float width;
    } Line3D;
    static Line3D* Line3DS = NULL;
    static int Line3DCount = 0;

    // Ломаная 3D
    typedef struct {
        float* x;
        float* y;
        float* z;
        int vertex_count;
        float color[3];
        float width;
    } Polyline3D;
    static Polyline3D* Polyline3DS = NULL;
    static int Polyline3DCount = 0;

    // Стрелка/вектор 3D
    typedef struct {
        float x0,y0,z0;
        float x1,y1,z1;
        float color[3];
        float width;
    } Arrow3D;
    static Arrow3D* Arrow3DS = NULL;
    static int Arrow3DCount = 0;

    // Сфера
    typedef struct {
        float x,y,z;
        float r;
        float color[3];
        int slices, stacks;
        bool solid;
        float line_width;
    } Sphere3D;
    static Sphere3D* Sphere3DS = NULL;
    static int Sphere3DCount = 0;

    // Куб
    typedef struct {
        float x,y,z;
        float size;
        float color[3];
        bool solid;
        float line_width;
    } Cube3D;
    static Cube3D* Cube3DS = NULL;
    static int Cube3DCount = 0;

    // Цилиндр
    typedef struct {
        float x,y,z;
        float r;
        float h;
        float color[3];
        int slices, stacks;
        bool solid;
        bool caps;
        float line_width;
    } Cylinder3D;
    static Cylinder3D* Cylinder3DS = NULL;
    static int Cylinder3DCount = 0;

    // Конус
    typedef struct {
        float x,y,z;
        float r;
        float h;
        float color[3];
        int slices, stacks;
        bool solid;
        bool caps;
        float line_width;
    } Cone3D;
    static Cone3D* Cone3DS = NULL;
    static int Cone3DCount = 0;

    // Тор
    typedef struct {
        float x,y,z;
        float inner_r;
        float outer_r;
        float color[3];
        int sides, rings;
        bool solid;
        float line_width;
    } Torus3D;
    static Torus3D* Torus3DS = NULL;
    static int Torus3DCount = 0;

    // Текст
    typedef struct {
        float x, y, z;
        float color[3];
        char* text;
    } Text;
    static Text* TextS = NULL;
    static int TextCount = 0;

    /* ============================ Окно / сцена / камера ============================ */

    static int window3d_width  = 1920;
    static int window3d_height = 1080;
    static int win3d_pos_x = 0;
    static int win3d_pos_y = 0;

    static float clear_color3d[4] = {1,1,1,1};

    static float axis3d_limits[6] = {-5, 5,  -5, 5,  -5, 5}; // xmin,xmax,ymin,ymax,zmin,zmax

    static bool  axes3d_enabled = true;
    static float axes3d_width = 3.0f;
    static float axis_color_x[3] = {1.0f, 0.0f, 0.0f};
    static float axis_color_y[3] = {0.0f, 1.0f, 0.0f};
    static float axis_color_z[3] = {0.0f, 0.0f, 1.0f};

    // Камера и проекция
    static float cam_eye[3]    = {6.0f, 5.0f, 6.0f};
    static float cam_center[3] = {0.0f, 0.0f, 0.0f};
    static float cam_up[3]     = {0.0f, 1.0f, 0.0f};

    static float proj_fov_y = 60.0f;
    static float proj_near_ = 0.1f;
    static float proj_far_  = 200.0f;


    static float default_cam_eye[3] = {6.0f, 5.0f, 6.0f};
    static float default_cam_center[3] = {0.0f, 0.0f, 0.0f};

    // GLU quadric для цилиндров/конусов
    static GLUquadric* g_quadric = NULL; // Библиотека поверхностей второго порядка

    /* ============================ Настройки по умолчанию ============================ */

    static float default_point3d_color[3] = {0,0,0};
    static float default_point3d_size = 7.0f;

    static float default_line3d_color[3] = {0,0,0};
    static float default_line3d_width = 3.0f;

    static float default_polyline3d_color[3] = {0,0,0};
    static float default_polyline3d_width = 3.0f;

    static float default_arrow3d_color[3] = {0,0,0};
    static float default_arrow3d_width = 3.0f;

    static float default_sphere3d_color[3] = {0,0,0};
    static bool  default_sphere3d_solid = false;
    static int   default_sphere3d_slices = 24;
    static int   default_sphere3d_stacks = 16;
    static float default_sphere3d_line_width = 2.0f;

    static float default_cube3d_color[3] = {0,0,0};
    static bool  default_cube3d_solid = false;
    static float default_cube3d_line_width = 2.0f;

    static float default_cylinder3d_color[3] = {0,0,0};
    static bool  default_cylinder3d_solid = false;
    static bool  default_cylinder3d_caps = true;
    static int   default_cylinder3d_slices = 24;
    static int   default_cylinder3d_stacks = 4;
    static float default_cylinder3d_line_width = 2.0f;

    static float default_cone3d_color[3] = {0,0,0};
    static bool  default_cone3d_solid = false;
    static bool  default_cone3d_caps = true;
    static int   default_cone3d_slices = 24;
    static int   default_cone3d_stacks = 4;
    static float default_cone3d_line_width = 2.0f;

    static float default_torus3d_color[3] = {0,0,0};
    static bool  default_torus3d_solid = false;
    static int   default_torus3d_sides = 16;
    static int   default_torus3d_rings = 24;
    static float default_torus3d_line_width = 2.0f;

    static float default_text_color[3] = {0,0,0};
    static bool  grid_visibility = true;

    /* ============================ Внутренние функции ============================ */

    static void draw_text_3d(float x, float y, float z, const char* text) {
        glRasterPos3f(x, y, z);
        for (const char* c = text; *c; ++c) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    // Отрисовка конуса, выровненного вдоль вектора направления
    static void draw_cone_aligned_3d(Vec3 p0, Vec3 p1, float radius, float height, bool solid) {
        Vec3 dir = v3_sub(p1, p0);
        float L = v3_len(dir);
        if (L <= 1e-6f) return; // Защита от деления на ноль
        dir = v3_mul(dir, 1.0f / L);

        // Сдвигаем базу конуса так, чтобы его вершина оказалась точно в точке p1
        Vec3 base = v3_sub(p1, v3_mul(dir, height));

        glPushMatrix();
        glTranslatef(base.x, base.y, base.z);

        // Вычисляем ось и угол вращения (из вектора Z в вектор dir)
        Vec3 z_axis = v3(0, 0, 1);
        float dot = v3_dot(z_axis, dir);

        if (dot > 0.9999f) {
            // Уже смотрит вдоль Z, вращать не нужно
        } else if (dot < -0.9999f) {
            // Смотрит в обратную сторону Z, разворачиваем на 180
            glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        } else {
            // Находим ось через векторное произведение и угол через скалярное
            Vec3 cross = v3_cross(z_axis, dir);
            float angle_deg = acosf(dot) * 180.0f / (float)M_PI;
            glRotatef(angle_deg, cross.x, cross.y, cross.z);
        }

        if (!g_quadric) g_quadric = gluNewQuadric();
        gluQuadricDrawStyle(g_quadric, solid ? GLU_FILL : GLU_LINE);
        gluCylinder(g_quadric, radius, 0.0f, height, 16, 1);

        // Закрываем основание конуса
        if (solid) {
            glPushMatrix();
            glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
            gluDisk(g_quadric, 0.0, radius, 16, 1);
            glPopMatrix();
        }

        glPopMatrix();
    }

    static void draw_axes_3d(void) {
        if (!axes3d_enabled) return;

        float xmin = axis3d_limits[0], xmax = axis3d_limits[1];
        float ymin = axis3d_limits[2], ymax = axis3d_limits[3];
        float zmin = axis3d_limits[4], zmax = axis3d_limits[5];

        glLineWidth(axes3d_width);

        // 1. Отрисовка линий осей
        glBegin(GL_LINES);
            // X
            glColor3fv(axis_color_x);
            glVertex3f(xmin, ymin, zmin); glVertex3f(xmax, ymin, zmin);
            // Y
            glColor3fv(axis_color_y);
            glVertex3f(xmin, ymin, zmin); glVertex3f(xmin, ymax, zmin);
            // Z
            glColor3fv(axis_color_z);
            glVertex3f(xmin, ymin, zmin); glVertex3f(xmin, ymin, zmax);
        glEnd();

        // 2. Отрисовка наконечников осей
        float lenX = xmax - xmin; float hX = lenX * 0.05f; if(hX < 0.2f) hX = 0.2f; float rX = hX * 0.3f;
        glColor3fv(axis_color_x);
        draw_cone_aligned_3d(v3(xmin, ymin, zmin), v3(xmax, ymin, zmin), rX, hX, true);

        float lenY = ymax - ymin; float hY = lenY * 0.05f; if(hY < 0.2f) hY = 0.2f; float rY = hY * 0.3f;
        glColor3fv(axis_color_y);
        draw_cone_aligned_3d(v3(xmin, ymin, zmin), v3(xmin, ymax, zmin), rY, hY, true);

        float lenZ = zmax - zmin; float hZ = lenZ * 0.05f; if(hZ < 0.2f) hZ = 0.2f; float rZ = hZ * 0.3f;
        glColor3fv(axis_color_z);
        draw_cone_aligned_3d(v3(xmin, ymin, zmin), v3(xmin, ymin, zmax), rZ, hZ, true);

        // 3. Отрисовка сетки
        if (grid_visibility) {
            // X сетка
            glBegin(GL_LINES);
            glColor3fv(axis_color_x);
            for (float x = xmin; x <= xmax; x += 1.0f) {
                glVertex3f(x, ymin, zmin);
                glVertex3f(x, ymax, zmin);

                glVertex3f(x, ymin, zmin);
                glVertex3f(x, ymin, zmax);
            }
            // Y сетка
            glColor3fv(axis_color_y);
            for (float y = ymin; y <= ymax; y += 1.0f) {
                glVertex3f(xmin, y, zmin);
                glVertex3f(xmax, y, zmin);

                glVertex3f(xmin, y, zmin);
                glVertex3f(xmin, y, zmax);
            }
            // Z сетка
            glColor3fv(axis_color_z);
            for (float z = zmin; z <= zmax; z += 1.0f) {
                glVertex3f(xmin, ymin, z);
                glVertex3f(xmax, ymin, z);

                glVertex3f(xmin, ymin, z);
                glVertex3f(xmin, ymax, z);
            }
            glEnd();
        }

        // 4. Подписи осей
        // X
        glColor3fv(axis_color_x);
        draw_text_3d(xmax+0.2f, ymin, zmin, "X");
        for (float gridX = axis3d_limits[0]; gridX <= axis3d_limits[1]; gridX += 1.0f) {
            char label[10]; snprintf(label, sizeof(label), "%.1f", gridX);
            draw_text_3d(gridX, ymin - 0.4f, zmin, label);
        }
        // Y
        glColor3fv(axis_color_y);
        draw_text_3d(xmin, ymax+0.2f, zmin, "Y");
        for (float gridY = axis3d_limits[2]; gridY <= axis3d_limits[3]; gridY += 1.0f) {
            char label[10]; snprintf(label, sizeof(label), "%.1f", gridY);
            draw_text_3d(xmin - 0.4f, gridY, zmin, label);
        }
        // Z
        glColor3fv(axis_color_z);
        draw_text_3d(xmin, ymin, zmax+0.2f, "Z");
        for (float gridZ = axis3d_limits[4]; gridZ <= axis3d_limits[5]; gridZ += 1.0f) {
            char label[10]; snprintf(label, sizeof(label), "%.1f", gridZ);
            draw_text_3d(xmin, ymin - 0.4f, gridZ, label);
        }
    }

    static void reshape3d(int w, int h) {
        if (h <= 0) h = 1;
        window3d_width = w;
        window3d_height = h;
        glViewport(0, 0, w, h);
        glutPostRedisplay();
    }

    static void camera_orbit_update_eye(void) {
        float yaw   = DEG_RAD(orbit_yaw_deg);
        float pitch = DEG_RAD(orbit_pitch_deg);
        Vec3 center = v3(cam_center[0], cam_center[1], cam_center[2]);

        Vec3 offset = v3(
            orbit_dist * cosf(pitch) * sinf(yaw),
            orbit_dist * sinf(pitch),
            orbit_dist * cosf(pitch) * cosf(yaw)
        );

        Vec3 eye = v3_add(center, offset);
        cam_eye[0] = eye.x; cam_eye[1] = eye.y; cam_eye[2] = eye.z;
    }

    static void camera_orbit_sync_from_lookat(void) {
        Vec3 eye    = v3(cam_eye[0], cam_eye[1], cam_eye[2]);
        Vec3 center = v3(cam_center[0], cam_center[1], cam_center[2]);

        Vec3 off = v3_sub(eye, center);
        float d = v3_len(off);
        if (d <= 1e-6f) d = 1.0f;
        orbit_dist = d;

        orbit_yaw_deg = RAD_DEG(atan2f(off.x, off.z));

        float s = off.y / d;
        if (s >  1.0f) s =  1.0f;
        if (s < -1.0f) s = -1.0f;
        orbit_pitch_deg = RAD_DEG(asinf(s));

        if (orbit_pitch_deg < orbit_pitch_min) orbit_pitch_deg = orbit_pitch_min;
        if (orbit_pitch_deg > orbit_pitch_max) orbit_pitch_deg = orbit_pitch_max;
    }

    // Получение базисных векторов для относительного движения (WASD / Pan)
    static void camera_get_basis(Vec3* out_forward, Vec3* out_right, Vec3* out_up2) {
        Vec3 eye    = v3(cam_eye[0], cam_eye[1], cam_eye[2]);
        Vec3 center = v3(cam_center[0], cam_center[1], cam_center[2]);
        Vec3 upv    = v3_norm(v3(cam_up[0], cam_up[1], cam_up[2]));

        Vec3 forward = v3_norm(v3_sub(center, eye));
        Vec3 right   = v3_norm(v3_cross(forward, upv));
        Vec3 up2     = v3_norm(v3_cross(right, forward));

        *out_forward = forward;
        *out_right   = right;
        *out_up2     = up2;
    }

    static void display3d(void) {
        if (!g_quadric) g_quadric = gluNewQuadric();

        glClearColor(clear_color3d[0], clear_color3d[1], clear_color3d[2], clear_color3d[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_POINT_SMOOTH);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float aspect = (float)window3d_width / (float)window3d_height;
        gluPerspective(proj_fov_y, aspect, proj_near_, proj_far_);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(
            cam_eye[0], cam_eye[1], cam_eye[2],
            cam_center[0], cam_center[1], cam_center[2],
            cam_up[0], cam_up[1], cam_up[2]
        );

        draw_axes_3d();

        // Торы
        for (int i = 0; i < Torus3DCount; ++i) {
            glPushMatrix();
            glTranslatef(Torus3DS[i].x, Torus3DS[i].y, Torus3DS[i].z);
            glColor3fv(Torus3DS[i].color);
            glLineWidth(Torus3DS[i].line_width);
            if (Torus3DS[i].solid) glutSolidTorus(Torus3DS[i].inner_r, Torus3DS[i].outer_r, Torus3DS[i].sides, Torus3DS[i].rings);
            else                   glutWireTorus (Torus3DS[i].inner_r, Torus3DS[i].outer_r, Torus3DS[i].sides, Torus3DS[i].rings);
            glPopMatrix();
        }

        // Сферы
        for (int i = 0; i < Sphere3DCount; ++i) {
            glPushMatrix();
            glTranslatef(Sphere3DS[i].x, Sphere3DS[i].y, Sphere3DS[i].z);
            glColor3fv(Sphere3DS[i].color);
            glLineWidth(Sphere3DS[i].line_width);
            if (Sphere3DS[i].solid) glutSolidSphere(Sphere3DS[i].r, Sphere3DS[i].slices, Sphere3DS[i].stacks);
            else                    glutWireSphere (Sphere3DS[i].r, Sphere3DS[i].slices, Sphere3DS[i].stacks);
            glPopMatrix();
        }

        // Кубы
        for (int i = 0; i < Cube3DCount; ++i) {
            glPushMatrix();
            glTranslatef(Cube3DS[i].x, Cube3DS[i].y, Cube3DS[i].z);
            glColor3fv(Cube3DS[i].color);
            glLineWidth(Cube3DS[i].line_width);
            if (Cube3DS[i].solid) glutSolidCube(Cube3DS[i].size);
            else                  glutWireCube (Cube3DS[i].size);
            glPopMatrix();
        }

        // Цилиндры
        for (int i = 0; i < Cylinder3DCount; ++i) {
            glPushMatrix();
            glTranslatef(Cylinder3DS[i].x, Cylinder3DS[i].y, Cylinder3DS[i].z - Cylinder3DS[i].h * 0.5f);
            glColor3fv(Cylinder3DS[i].color);
            glLineWidth(Cylinder3DS[i].line_width);

            gluQuadricDrawStyle(g_quadric, Cylinder3DS[i].solid ? GLU_FILL : GLU_LINE);
            gluCylinder(g_quadric, Cylinder3DS[i].r, Cylinder3DS[i].r, Cylinder3DS[i].h, Cylinder3DS[i].slices, Cylinder3DS[i].stacks);

            if (Cylinder3DS[i].caps) {
                glPushMatrix();
                gluQuadricDrawStyle(g_quadric, Cylinder3DS[i].solid ? GLU_FILL : GLU_LINE);
                gluDisk(g_quadric, 0.0, Cylinder3DS[i].r, Cylinder3DS[i].slices, 1);
                glPopMatrix();

                glPushMatrix();
                glTranslatef(0,0,Cylinder3DS[i].h);
                gluQuadricDrawStyle(g_quadric, Cylinder3DS[i].solid ? GLU_FILL : GLU_LINE);
                gluDisk(g_quadric, 0.0, Cylinder3DS[i].r, Cylinder3DS[i].slices, 1);
                glPopMatrix();
            }
            glPopMatrix();
        }

        // Конусы
        for (int i = 0; i < Cone3DCount; ++i) {
            glPushMatrix();
            glTranslatef(Cone3DS[i].x, Cone3DS[i].y, Cone3DS[i].z - Cone3DS[i].h * 0.5f);
            glColor3fv(Cone3DS[i].color);
            glLineWidth(Cone3DS[i].line_width);

            gluQuadricDrawStyle(g_quadric, Cone3DS[i].solid ? GLU_FILL : GLU_LINE);
            gluCylinder(g_quadric, Cone3DS[i].r, 0.0, Cone3DS[i].h, Cone3DS[i].slices, Cone3DS[i].stacks);

            if (Cone3DS[i].caps) {
                gluQuadricDrawStyle(g_quadric, Cone3DS[i].solid ? GLU_FILL : GLU_LINE);
                gluDisk(g_quadric, 0.0, Cone3DS[i].r, Cone3DS[i].slices, 1);
            }
            glPopMatrix();
        }

        // Стрелки
        for (int i = 0; i < Arrow3DCount; ++i) {
            glColor3fv(Arrow3DS[i].color);
            glLineWidth(Arrow3DS[i].width);
            Vec3 p0 = v3(Arrow3DS[i].x0, Arrow3DS[i].y0, Arrow3DS[i].z0);
            Vec3 p1 = v3(Arrow3DS[i].x1, Arrow3DS[i].y1, Arrow3DS[i].z1);

            glBegin(GL_LINES);
                glVertex3f(p0.x,p0.y,p0.z);
                glVertex3f(p1.x,p1.y,p1.z);
            glEnd();

            float L = v3_len(v3_sub(p1,p0));
            float head = 0.12f * L;
            if (head > 0.5f) head = 0.5f;
            if (head < 0.08f) head = 0.08f;
            draw_cone_aligned_3d(p0, p1, head * 0.3f, head, true);
        }

        // Ломаные
        for (int i = 0; i < Polyline3DCount; ++i) {
            glColor3fv(Polyline3DS[i].color);
            glLineWidth(Polyline3DS[i].width);
            glBegin(GL_LINES);
                for (int j = 1; j < Polyline3DS[i].vertex_count; ++j) {
                    glVertex3f(Polyline3DS[i].x[j-1], Polyline3DS[i].y[j-1], Polyline3DS[i].z[j-1]);
                    glVertex3f(Polyline3DS[i].x[j],   Polyline3DS[i].y[j],   Polyline3DS[i].z[j]);
                }
            glEnd();
        }

        // Линии
        for (int i = 0; i < Line3DCount; ++i) {
            glColor3fv(Line3DS[i].color);
            glLineWidth(Line3DS[i].width);
            glBegin(GL_LINES);
                glVertex3f(Line3DS[i].x0, Line3DS[i].y0, Line3DS[i].z0);
                glVertex3f(Line3DS[i].x1, Line3DS[i].y1, Line3DS[i].z1);
            glEnd();
        }

        // Точки
        for (int i = 0; i < Point3DCount; ++i) {
            glColor3fv(Point3DS[i].color);
            glPointSize(Point3DS[i].size);
            glBegin(GL_POINTS);
                glVertex3f(Point3DS[i].x, Point3DS[i].y, Point3DS[i].z);
            glEnd();
        }

        // Текст
        for (int i = 0; i < TextCount; ++i) {
            glColor3fv(TextS[i].color);
            draw_text_3d(TextS[i].x, TextS[i].y, TextS[i].z, TextS[i].text);
        }

        glutSwapBuffers();
    }

    /* ============================ Публичный API ============================ */

    static void window_position3d(int x_position, int y_position) {
        win3d_pos_x = x_position;
        win3d_pos_y = y_position;
    }

    static void window3d(int width, int height) {
        window3d_width = width;
        window3d_height = height;
    }

    static void background_color3d(int r, int g, int b) {
        clear_color3d[0] = r / 255.0f; clear_color3d[1] = g / 255.0f; clear_color3d[2] = b / 255.0f; clear_color3d[3] = 1.0f;
        glutPostRedisplay();
    }

    static void camera3d(float ex,float ey,float ez, float cx,float cy,float cz, float ux,float uy,float uz) {
        cam_eye[0]=ex; cam_eye[1]=ey; cam_eye[2]=ez;
        cam_center[0]=cx; cam_center[1]=cy; cam_center[2]=cz;
        cam_up[0]=ux; cam_up[1]=uy; cam_up[2]=uz;
        camera_orbit_sync_from_lookat();
        glutPostRedisplay();
    }

    static void camera_control3d(const char* choice) {
        if (!choice) return;
        if (strcmp(choice, "ON")==0 || strcmp(choice, "on")==0 || strcmp(choice, "1")==0) camera_control_enabled = true;
        else if (strcmp(choice, "OFF")==0 || strcmp(choice, "off")==0 || strcmp(choice, "0")==0) camera_control_enabled = false;
    }

    static void set_camera_control_sensitivity3d(float rot_sens, float pan_sens, float zoom_mul) {
        if (rot_sens > 0.0f) orbit_rot_sens = rot_sens;
        if (pan_sens > 0.0f) orbit_pan_sens = pan_sens;
        if (zoom_mul > 1.001f) orbit_zoom_mul = zoom_mul;
    }

    static void axes_visibility3d(const char* choice) {
        if (!choice) return;
        if (strcmp(choice, "ON") == 0 || strcmp(choice, "on") == 0 || strcmp(choice, "1") == 0) axes3d_enabled = true;
        else if (strcmp(choice, "OFF") == 0 || strcmp(choice, "off") == 0 || strcmp(choice, "0") == 0) axes3d_enabled = false;
        glutPostRedisplay();
    }

    static void axes_colored3d(const char* choice) {
        if (!choice) return;
        float c = 0.0f;
        if (strcmp(choice, "ON") == 0 || strcmp(choice, "on") == 0 || strcmp(choice, "1") == 0) c = 1.0f;
        else if (strcmp(choice, "OFF") == 0 || strcmp(choice, "off") == 0 || strcmp(choice, "0") == 0) c = 0.0f;
        axis_color_x[0] = c;
        axis_color_y[1] = c;
        axis_color_z[2] = c;
        glutPostRedisplay();
    }

    static void grid_visibility_enable(const char* choice) {
        if (!choice) return;
        if (strcmp(choice, "ON") == 0 || strcmp(choice, "on") == 0 || strcmp(choice, "1") == 0) grid_visibility = true;
        else if (strcmp(choice, "OFF") == 0 || strcmp(choice, "off") == 0 || strcmp(choice, "0") == 0) grid_visibility = false;
        glutPostRedisplay();
    }

    /* ============================ УПРАВЛЕНИЕ: Мышь ============================ */

    static void mouse3d(int button, int state, int x, int y) {
        if (!camera_control_enabled) return;
        mouse_last_x = x;
        mouse_last_y = y;

        if (button == GLUT_LEFT_BUTTON)  mouse_lmb_down = (state == GLUT_DOWN);
        if (button == GLUT_RIGHT_BUTTON) mouse_rmb_down = (state == GLUT_DOWN);
    }

    static void motion3d(int x, int y) {
        if (!camera_control_enabled) return;

        int dx = x - mouse_last_x;
        int dy = y - mouse_last_y;
        mouse_last_x = x;
        mouse_last_y = y;

        // Вращение (Орбита)
        if (mouse_lmb_down) {
            orbit_yaw_deg   += dx * orbit_rot_sens;
            orbit_pitch_deg += dy * orbit_rot_sens;

            if (orbit_pitch_deg < orbit_pitch_min) orbit_pitch_deg = orbit_pitch_min;
            if (orbit_pitch_deg > orbit_pitch_max) orbit_pitch_deg = orbit_pitch_max;

            camera_orbit_update_eye();
            glutPostRedisplay();
        }
        // Панорамирование (Перемещение центра)
        else if (mouse_rmb_down) {
            Vec3 forward, right, up;
            camera_get_basis(&forward, &right, &up);

            // Скорость зависит от отдаления от объекта (интуитивность)
            float pan_speed = orbit_dist * orbit_pan_sens;

            Vec3 move_x = v3_mul(right, -(float)dx * pan_speed);
            Vec3 move_y = v3_mul(up, (float)dy * pan_speed);

            cam_center[0] += move_x.x + move_y.x;
            cam_center[1] += move_x.y + move_y.y;
            cam_center[2] += move_x.z + move_y.z;

            camera_orbit_update_eye();
            glutPostRedisplay();
        }
    }

    static void mousewheel3d(int wheel, int direction, int x, int y) {
        (void)wheel; (void)x; (void)y;
        if (!camera_control_enabled) return;

        if (direction > 0) orbit_dist /= orbit_zoom_mul;
        else               orbit_dist *= orbit_zoom_mul;

        if (orbit_dist < 0.05f) orbit_dist = 0.05f;

        camera_orbit_update_eye();
        glutPostRedisplay();
    }

    /* ============================ УПРАВЛЕНИЕ: Клавиатура ============================ */

    static void keyboard3d(unsigned char key, int x, int y) {
        (void)x; (void)y;
        if (!camera_control_enabled) return;

        Vec3 forward, right, up;
        camera_get_basis(&forward, &right, &up);

        // Скорость также пропорциональна дистанции
        float speed = orbit_dist * 0.05f;
        if (speed < 0.1f) speed = 0.1f;

        Vec3 move = v3(0,0,0);

        switch(key) {
            case 'w': case 'W': move = v3_add(move, v3_mul(forward, speed)); break;
            case 's': case 'S': move = v3_sub(move, v3_mul(forward, speed)); break;
            case 'a': case 'A': move = v3_sub(move, v3_mul(right, speed)); break;
            case 'd': case 'D': move = v3_add(move, v3_mul(right, speed)); break;
            case 'e': case 'E': move = v3_add(move, v3(0, speed, 0)); break; // Глобальный UP
            case 'q': case 'Q': move = v3_sub(move, v3(0, speed, 0)); break; // Глобальный DOWN

            // Сброс камеры
            case 'r': case 'R':
                cam_center[0] = default_cam_center[0]; cam_center[1] = default_cam_center[1]; cam_center[2] = default_cam_center[2];
                orbit_yaw_deg = 45.0f; orbit_pitch_deg = 45.0f; orbit_dist = 45.0f;
                camera_orbit_update_eye();
                glutPostRedisplay();
            return;
        }

        if (move.x != 0 || move.y != 0 || move.z != 0) {
            cam_center[0] += move.x;
            cam_center[1] += move.y;
            cam_center[2] += move.z;
            camera_orbit_update_eye();
            glutPostRedisplay();
        }
    }

    /* ============================ Инициализация ============================ */

    static void axis3d(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax) {
        axis3d_limits[0]=xmin; axis3d_limits[1]=xmax;
        axis3d_limits[2]=ymin; axis3d_limits[3]=ymax;
        axis3d_limits[4]=zmin; axis3d_limits[5]=zmax;

        cam_eye[0]=xmax+3; cam_eye[1]=ymax+3; cam_eye[2]=zmax+3;
        cam_center[0]=xmin; cam_center[1]=ymin; cam_center[2]=zmin;

        default_cam_eye[0] = cam_eye[0];
        default_cam_eye[1] = cam_eye[1];
        default_cam_eye[2] = cam_eye[2];

        default_cam_center[0] = cam_center[0];
        default_cam_center[1] = cam_center[1];
        default_cam_center[2] = cam_center[2];


        int argc = 1;
        char* argv[1] = {(char*)"EasyGLUT3D"};
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowSize(window3d_width, window3d_height);
        glutInitWindowPosition(win3d_pos_x, win3d_pos_y);
        glutCreateWindow("EasyGLUT3D");

        glutDisplayFunc(display3d);
        glutReshapeFunc(reshape3d);

        glutKeyboardFunc(keyboard3d); // Включение управления клавиатурой
        glutMouseFunc(mouse3d);       // Включение управления мышью
        glutMotionFunc(motion3d);
        glutMouseWheelFunc(mousewheel3d);

        camera_orbit_sync_from_lookat();
        camera_orbit_update_eye();

        glutPostRedisplay();
        glClearColor(clear_color3d[0], clear_color3d[1], clear_color3d[2], clear_color3d[3]);
    }

    /* ============================ Примитивы ============================ */

    static void set_outtext_color(int r, int g, int b) {
        default_text_color[0] = r/255.0f;
        default_text_color[1] = g/255.0f;
        default_text_color[2] = b/255.0f;
    }
    static void outtext(float x, float y, float z, const char* text) {
        if (!text) return;
        Text* temp = (Text*)realloc(TextS, (TextCount+1)*sizeof(Text));
        if (!temp) return;
        TextS = temp;
        TextS[TextCount].text = (char*)malloc(strlen(text) + 1);
        strcpy(TextS[TextCount].text, text);
        TextS[TextCount].x=x; TextS[TextCount].y=y; TextS[TextCount].z=z;
        memcpy(TextS[TextCount].color, default_text_color, 3*sizeof(float));
        TextCount++;
        glutPostRedisplay();
    }

    static void set_point3d_size(float size) { default_point3d_size = size; }
    static void set_point3d_color(int r,int g,int b) {
        default_point3d_color[0]=r/255.0f;
        default_point3d_color[1]=g/255.0f;
        default_point3d_color[2]=b/255.0f;
    }
    static void point3d(float x,float y,float z) {
        Point3D* temp = (Point3D*)realloc(Point3DS, (Point3DCount+1)*sizeof(Point3D));
        if (!temp) return;
        Point3DS = temp;
        Point3DS[Point3DCount].x=x; Point3DS[Point3DCount].y=y; Point3DS[Point3DCount].z=z;
        memcpy(Point3DS[Point3DCount].color, default_point3d_color, 3*sizeof(float));
        Point3DS[Point3DCount].size = default_point3d_size;
        Point3DCount++;
        glutPostRedisplay();
    }

    static void set_line3d_width(float width) { default_line3d_width = width; }
    static void set_line3d_color(int r,int g,int b) {
        default_line3d_color[0]=r/255.0f; default_line3d_color[1]=g/255.0f; default_line3d_color[2]=b/255.0f;
    }
    static void line3d(float x0,float y0,float z0, float x1,float y1,float z1) {
        Line3D* temp = (Line3D*)realloc(Line3DS, (Line3DCount+1)*sizeof(Line3D));
        if (!temp) return;
        Line3DS = temp;
        Line3DS[Line3DCount].x0=x0; Line3DS[Line3DCount].y0=y0; Line3DS[Line3DCount].z0=z0;
        Line3DS[Line3DCount].x1=x1; Line3DS[Line3DCount].y1=y1; Line3DS[Line3DCount].z1=z1;
        memcpy(Line3DS[Line3DCount].color, default_line3d_color, 3*sizeof(float));
        Line3DS[Line3DCount].width = default_line3d_width;
        Line3DCount++;
        glutPostRedisplay();
    }

    static void set_polyline3d_width(float width) { default_polyline3d_width = width; }
    static void set_polyline3d_color(int r,int g,int b) {
        default_polyline3d_color[0]=r/255.0f;
        default_polyline3d_color[1]=g/255.0f;
        default_polyline3d_color[2]=b/255.0f;
    }
    static void polyline3d(float* x, float* y, float* z, int vertex_count) {
        if (vertex_count < 2) return;
        Polyline3D* temp = (Polyline3D*)realloc(Polyline3DS, (Polyline3DCount+1)*sizeof(Polyline3D));
        if (!temp) return;
        Polyline3DS = temp;
        Polyline3DS[Polyline3DCount].x = (float*)malloc(vertex_count*sizeof(float));
        Polyline3DS[Polyline3DCount].y = (float*)malloc(vertex_count*sizeof(float));
        Polyline3DS[Polyline3DCount].z = (float*)malloc(vertex_count*sizeof(float));
        memcpy(Polyline3DS[Polyline3DCount].x, x, vertex_count*sizeof(float));
        memcpy(Polyline3DS[Polyline3DCount].y, y, vertex_count*sizeof(float));
        memcpy(Polyline3DS[Polyline3DCount].z, z, vertex_count*sizeof(float));
        Polyline3DS[Polyline3DCount].vertex_count = vertex_count;
        memcpy(Polyline3DS[Polyline3DCount].color, default_polyline3d_color, 3*sizeof(float));
        Polyline3DS[Polyline3DCount].width = default_polyline3d_width;
        Polyline3DCount++;
        glutPostRedisplay();
    }

    static void set_arrow3d_width(float width) { default_arrow3d_width = width; }
    static void set_arrow3d_color(int r,int g,int b) {
        default_arrow3d_color[0]=r/255.0f;
        default_arrow3d_color[1]=g/255.0f;
        default_arrow3d_color[2]=b/255.0f;
    }
    static void arrow3d(float x0,float y0,float z0, float x1,float y1,float z1) {
        Arrow3D* temp = (Arrow3D*)realloc(Arrow3DS, (Arrow3DCount+1)*sizeof(Arrow3D));
        if (!temp) return;
        Arrow3DS = temp;
        Arrow3DS[Arrow3DCount].x0=x0; Arrow3DS[Arrow3DCount].y0=y0; Arrow3DS[Arrow3DCount].z0=z0;
        Arrow3DS[Arrow3DCount].x1=x1; Arrow3DS[Arrow3DCount].y1=y1; Arrow3DS[Arrow3DCount].z1=z1;
        memcpy(Arrow3DS[Arrow3DCount].color, default_arrow3d_color, 3*sizeof(float));
        Arrow3DS[Arrow3DCount].width = default_arrow3d_width;
        Arrow3DCount++;
        glutPostRedisplay();
    }

    static void set_sphere3d_color(int r,int g,int b) {
        default_sphere3d_color[0]=r/255.0f;
        default_sphere3d_color[1]=g/255.0f;
        default_sphere3d_color[2]=b/255.0f; }
    static void set_sphere3d_mode(const char* mode) {
        if (!mode) return;
        if (strcmp(mode,"SOLID")==0 || strcmp(mode,"S")==0) default_sphere3d_solid = true;
        else if (strcmp(mode,"WIRE")==0 || strcmp(mode,"W")==0) default_sphere3d_solid = false;
    }
    static void set_sphere3d_quality(int slices, int stacks) {
        if(slices>2) default_sphere3d_slices=slices;
        if(stacks>2) default_sphere3d_stacks=stacks; }
    static void set_sphere3d_line_width(float w) { default_sphere3d_line_width = w; }
    static void sphere3d(float x,float y,float z, float r) {
        Sphere3D* temp = (Sphere3D*)realloc(Sphere3DS, (Sphere3DCount+1)*sizeof(Sphere3D));
        if (!temp) return;
        Sphere3DS = temp;
        Sphere3DS[Sphere3DCount].x=x; Sphere3DS[Sphere3DCount].y=y; Sphere3DS[Sphere3DCount].z=z; Sphere3DS[Sphere3DCount].r=r;
        memcpy(Sphere3DS[Sphere3DCount].color, default_sphere3d_color, 3*sizeof(float));
        Sphere3DS[Sphere3DCount].slices = default_sphere3d_slices; Sphere3DS[Sphere3DCount].stacks = default_sphere3d_stacks;
        Sphere3DS[Sphere3DCount].solid = default_sphere3d_solid; Sphere3DS[Sphere3DCount].line_width = default_sphere3d_line_width;
        Sphere3DCount++; glutPostRedisplay();
    }

    static void set_cube3d_color(int r,int g,int b) {
        default_cube3d_color[0]=r/255.0f;
        default_cube3d_color[1]=g/255.0f;
        default_cube3d_color[2]=b/255.0f; }
    static void set_cube3d_mode(const char* mode) {
        if (!mode) return;
        if (strcmp(mode,"SOLID")==0 || strcmp(mode,"S")==0) default_cube3d_solid = true;
        else if (strcmp(mode,"WIRE")==0 || strcmp(mode,"W")==0) default_cube3d_solid = false;
    }
    static void set_cube3d_line_width(float w) { default_cube3d_line_width = w; }
    static void cube3d(float x,float y,float z, float size) {
        Cube3D* temp = (Cube3D*)realloc(Cube3DS, (Cube3DCount+1)*sizeof(Cube3D));
        if (!temp) return;
        Cube3DS = temp;
        Cube3DS[Cube3DCount].x=x; Cube3DS[Cube3DCount].y=y; Cube3DS[Cube3DCount].z=z; Cube3DS[Cube3DCount].size=size;
        memcpy(Cube3DS[Cube3DCount].color, default_cube3d_color, 3*sizeof(float));
        Cube3DS[Cube3DCount].solid = default_cube3d_solid; Cube3DS[Cube3DCount].line_width = default_cube3d_line_width;
        Cube3DCount++; glutPostRedisplay();
    }

    static void set_cylinder3d_color(int r,int g,int b) {
        default_cylinder3d_color[0]=r/255.0f;
        default_cylinder3d_color[1]=g/255.0f;
        default_cylinder3d_color[2]=b/255.0f;
    }
    static void set_cylinder3d_mode(const char* mode) {
        if (!mode) return;
        if (strcmp(mode,"SOLID")==0 || strcmp(mode,"S")==0) default_cylinder3d_solid = true;
        else if (strcmp(mode,"WIRE")==0 || strcmp(mode,"W")==0) default_cylinder3d_solid = false;
    }
    static void set_cylinder3d_caps(const char* choice) {
        if (!choice) return;
        if (strcmp(choice, "ON")==0 || strcmp(choice, "1")==0) default_cylinder3d_caps = true;
        else if (strcmp(choice,"OFF")==0 || strcmp(choice,"0")==0) default_cylinder3d_caps = false;
    }
    static void set_cylinder3d_quality(int slices, int stacks) {
        if(slices>2) default_cylinder3d_slices=slices;
        if(stacks>0) default_cylinder3d_stacks=stacks;
    }
    static void set_cylinder3d_line_width(float w) { default_cylinder3d_line_width = w; }
    static void cylinder3d(float x,float y,float z, float r, float h) {
        Cylinder3D* temp = (Cylinder3D*)realloc(Cylinder3DS, (Cylinder3DCount+1)*sizeof(Cylinder3D));
        if (!temp) return;
        Cylinder3DS = temp;
        Cylinder3DS[Cylinder3DCount].x=x; Cylinder3DS[Cylinder3DCount].y=y; Cylinder3DS[Cylinder3DCount].z=z;
        Cylinder3DS[Cylinder3DCount].r=r; Cylinder3DS[Cylinder3DCount].h=h;
        memcpy(Cylinder3DS[Cylinder3DCount].color, default_cylinder3d_color, 3*sizeof(float));
        Cylinder3DS[Cylinder3DCount].solid = default_cylinder3d_solid; Cylinder3DS[Cylinder3DCount].caps = default_cylinder3d_caps;
        Cylinder3DS[Cylinder3DCount].slices = default_cylinder3d_slices; Cylinder3DS[Cylinder3DCount].stacks = default_cylinder3d_stacks;
        Cylinder3DS[Cylinder3DCount].line_width = default_cylinder3d_line_width;
        Cylinder3DCount++; glutPostRedisplay();
    }

    static void set_cone3d_color(int r,int g,int b) {
        default_cone3d_color[0]=r/255.0f;
        default_cone3d_color[1]=g/255.0f;
        default_cone3d_color[2]=b/255.0f;
    }
    static void set_cone3d_mode(const char* mode) {
        if (!mode) return;
        if (strcmp(mode,"SOLID")==0 || strcmp(mode,"S")==0) default_cone3d_solid = true;
        else if (strcmp(mode,"WIRE")==0 || strcmp(mode,"W")==0) default_cone3d_solid = false;
    }
    static void set_cone3d_caps(const char* choice) {
        if (!choice) return;
        if (strcmp(choice, "ON")==0 || strcmp(choice, "1")==0) default_cone3d_caps = true;
        else if (strcmp(choice,"OFF")==0 || strcmp(choice,"0")==0) default_cone3d_caps = false;
    }
    static void set_cone3d_quality(int slices, int stacks) { if(slices>2) default_cone3d_slices=slices; if(stacks>0) default_cone3d_stacks=stacks; }
    static void set_cone3d_line_width(float w) { default_cone3d_line_width = w; }
    static void cone3d(float x,float y,float z, float r, float h) {
        Cone3D* temp = (Cone3D*)realloc(Cone3DS, (Cone3DCount+1)*sizeof(Cone3D));
        if (!temp) return;
        Cone3DS = temp;
        Cone3DS[Cone3DCount].x=x; Cone3DS[Cone3DCount].y=y; Cone3DS[Cone3DCount].z=z;
        Cone3DS[Cone3DCount].r=r; Cone3DS[Cone3DCount].h=h;
        memcpy(Cone3DS[Cone3DCount].color, default_cone3d_color, 3*sizeof(float));
        Cone3DS[Cone3DCount].solid = default_cone3d_solid; Cone3DS[Cone3DCount].caps = default_cone3d_caps;
        Cone3DS[Cone3DCount].slices = default_cone3d_slices; Cone3DS[Cone3DCount].stacks = default_cone3d_stacks;
        Cone3DS[Cone3DCount].line_width = default_cone3d_line_width;
        Cone3DCount++; glutPostRedisplay();
    }

    static void set_torus3d_color(int r,int g,int b) {
        default_torus3d_color[0]=r/255.0f;
        default_torus3d_color[1]=g/255.0f;
        default_torus3d_color[2]=b/255.0f;
    }
    static void set_torus3d_mode(const char* mode) {
        if (!mode) return;
        if (strcmp(mode,"SOLID")==0 || strcmp(mode,"S")==0) default_torus3d_solid = true;
        else if (strcmp(mode,"WIRE")==0 || strcmp(mode,"W")==0) default_torus3d_solid = false;
    }
    static void set_torus3d_quality(int sides, int rings) { if(sides>2) default_torus3d_sides=sides; if(rings>2) default_torus3d_rings=rings; }
    static void set_torus3d_line_width(float w) { default_torus3d_line_width = w; }
    static void torus3d(float x,float y,float z, float inner_r, float outer_r) {
        Torus3D* temp = (Torus3D*)realloc(Torus3DS, (Torus3DCount+1)*sizeof(Torus3D));
        if (!temp) return;
        Torus3DS = temp;
        Torus3DS[Torus3DCount].x=x; Torus3DS[Torus3DCount].y=y; Torus3DS[Torus3DCount].z=z;
        Torus3DS[Torus3DCount].inner_r = inner_r; Torus3DS[Torus3DCount].outer_r = outer_r;
        memcpy(Torus3DS[Torus3DCount].color, default_torus3d_color, 3*sizeof(float));
        Torus3DS[Torus3DCount].solid = default_torus3d_solid; Torus3DS[Torus3DCount].sides = default_torus3d_sides;
        Torus3DS[Torus3DCount].rings = default_torus3d_rings; Torus3DS[Torus3DCount].line_width = default_torus3d_line_width;
        Torus3DCount++; glutPostRedisplay();
    }

    static void show3d(void) {
        glutMainLoop();
    }

    #ifdef __cplusplus
}
#endif
#endif // EASYGLUT3D_H
