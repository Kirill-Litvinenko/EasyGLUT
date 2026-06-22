#include "EasyGLUT3D.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEG_RAD(x) ((x) * (float)M_PI / 180.0f)
#define RAD_DEG(x) ((x) * 180.0f / (float)M_PI)

/* ───────── Массивы примитивов ───────── */
typedef struct {
    float x, y, z;
    float color[3];
    float size;
    int   group_id;
    bool  visible;
} Point;
static Point* PointS = NULL;
static int PointCount = 0;

typedef struct {
    float x0,y0,z0;
    float x1,y1,z1;
    float color[3];
    float width;
    int   group_id;
    bool  visible;
} Line;
static Line* LineS = NULL;
static int LineCount = 0;

typedef struct {
    float* x;
    float* y;
    float* z;
    int    vertex_count;
    float  color[3];
    float  width;
    int    group_id;
    bool   visible;
} PolyLine;
static PolyLine* PolyLineS = NULL;
static int PolyLineCount = 0;

typedef struct {
    float x0,y0,z0;
    float x1,y1,z1;
    float color[3];
    float width;
    int   group_id;
    bool  visible;
} Arrow;
static Arrow* ArrowS = NULL;
static int ArrowCount = 0;

typedef struct {
    float x,y,z;
    float r;
    float color[3];
    int   slices, stacks;
    bool  solid;
    float line_width;
    int   group_id;
    bool  visible;
} Sphere;
static Sphere* SphereS = NULL;
static int SphereCount = 0;

typedef struct {
    float x,y,z;
    float size;
    float color[3];
    bool  solid;
    float line_width;
    int   group_id;
    bool  visible;
} Cube;
static Cube* CubeS = NULL;
static int CubeCount = 0;

typedef struct {
    float x,y,z;
    float r;
    float h;
    float color[3];
    int   slices, stacks;
    bool  solid;
    bool  caps;
    float line_width;
    int   group_id;
    bool  visible;
} Cylinder;
static Cylinder* CylinderS = NULL;
static int CylinderCount = 0;

typedef struct {
    float x,y,z;
    float r;
    float h;
    float color[3];
    int   slices, stacks;
    bool  solid;
    bool  caps;
    float line_width;
    int   group_id;
    bool  visible;
} Cone;
static Cone* ConeS = NULL;
static int ConeCount = 0;

typedef struct {
    float x,y,z;
    float inner_r;
    float outer_r;
    float color[3];
    int   sides, rings;
    bool  solid;
    float line_width;
    int   group_id;
    bool  visible;
} Torus;
static Torus* TorusS = NULL;
static int TorusCount = 0;

typedef struct {
    float x, y, z;
    float color[3];
    char* text;
    int   group_id;
    bool  visible;
} OutText;
static OutText* OutTextS = NULL;
static int OutTextCount = 0;

/* 2D примитивы */
typedef struct {
    float x0,y0,z0, x1,y1,z1, x2,y2,z2;
    float color[3];
    float line_width;
    bool  filled;
    int   group_id;
    bool  visible;
} Triangle2D;
static Triangle2D* Triangle2DS = NULL;
static int Triangle2DCount = 0;

typedef struct {
    float* x;
    float* y;
    float* z;
    int    vertex_count;
    float  color[3];
    float  line_width;
    bool   filled;
    int    group_id;
    bool   visible;
} Polygon2D;
static Polygon2D* Polygon2DS = NULL;
static int Polygon2DCount = 0;

typedef struct {
    float cx, cy, cz;
    float r;
    int   segments;
    float color[3];
    float line_width;
    bool  filled;
    float nx, ny, nz;
    int   group_id;
    bool  visible;
} Circle2D;
static Circle2D* Circle2DS = NULL;
static int Circle2DCount = 0;

typedef struct {
    float cx, cy, cz;
    float rx, ry;
    float angle_deg;
    int   segments;
    float color[3];
    float line_width;
    bool  filled;
    float nx, ny, nz;
    int   group_id;
    bool  visible;
} Ellipse2D;
static Ellipse2D* Ellipse2DS = NULL;
static int Ellipse2DCount = 0;

/* ───────── Управление камерой ───────── */
static bool camera_control_enabled = true;

static float orbit_yaw_deg = 45.0f;
static float orbit_pitch_deg = -25.0f;
static float orbit_dist = 10.0f;

static float orbit_pitch_min = -89.0f;
static float orbit_pitch_max = 89.0f;

static float orbit_rot_sens = 0.25f;
static float orbit_pan_sens = 0.0025f;
static float orbit_zoom_mul = 1.10f;

static int  mouse_last_x = 0;
static int  mouse_last_y = 0;
static bool mouse_lmb_down = false;
static bool mouse_rmb_down = false;

/* ───────── Окно и сцена ───────── */
static int window3d_width = 1920;
static int window3d_height = 1080;
static int win3d_pos_x = 0;
static int win3d_pos_y = 0;

static float clear_color3d[4] = {1, 1, 1, 1};

static float axis3d_limits[6] = {-5, 5, -5, 5, -5, 5};
static bool  axes3d_enabled = true;
static float axes3d_width = 3.0f;
static float axis_color_x[3] = {1.0f, 0.0f, 0.0f};
static float axis_color_y[3] = {0.0f, 1.0f, 0.0f};
static float axis_color_z[3] = {0.0f, 0.0f, 1.0f};

static float cam_eye[3] = {6.0f, 5.0f, 6.0f};
static float cam_center[3] = {0.0f, 0.0f, 0.0f};
static float cam_up[3] = {0.0f, 1.0f, 0.0f};

static float proj_fov_y = 60.0f;
static float proj_near_ = 0.1f;
static float proj_far_ = 200.0f;

static float default_cam_eye[3] = {6.0f, 5.0f, 6.0f};
static float default_cam_center[3] = {0.0f, 0.0f, 0.0f};

static GLUquadric* g_quadric = NULL;
static bool grid_visibility = true;

/* ───────── Настройки по умолчанию (3D) ───────── */
static float default_point3d_color[3] = {0, 0, 0};
static float default_point3d_size = 7.0f;

static float default_Line_color[3] = {0, 0, 0};
static float default_Line_width = 3.0f;

static float default_polyLine_color[3] = {0, 0, 0};
static float default_polyLine_width = 3.0f;

static float default_arrow3d_color[3] = {0, 0, 0};
static float default_arrow3d_width = 3.0f;

static float default_sphere3d_color[3] = {0, 0, 0};
static bool  default_sphere3d_solid = false;
static int   default_sphere3d_slices = 24;
static int   default_sphere3d_stacks = 16;
static float default_sphere3d_line_width = 2.0f;

static float default_cube3d_color[3] = {0, 0, 0};
static bool  default_cube3d_solid = false;
static float default_cube3d_line_width = 2.0f;

static float default_cylinder3d_color[3] = {0, 0, 0};
static bool  default_cylinder3d_solid = false;
static bool  default_cylinder3d_caps = true;
static int   default_cylinder3d_slices = 24;
static int   default_cylinder3d_stacks = 4;
static float default_cylinder3d_line_width = 2.0f;

static float default_cone3d_color[3] = {0, 0, 0};
static bool  default_cone3d_solid = false;
static bool  default_cone3d_caps = true;
static int   default_cone3d_slices = 24;
static int   default_cone3d_stacks = 4;
static float default_cone3d_line_width = 2.0f;

static float default_torus3d_color[3] = {0, 0, 0};
static bool  default_torus3d_solid = false;
static int   default_torus3d_sides = 16;
static int   default_torus3d_rings = 24;
static float default_torus3d_line_width = 2.0f;

static float default_text_color[3] = {0, 0, 0};

/* ───────── Настройки по умолчанию (2D) ───────── */
static float default_triangle2d_color[3] = {0, 0, 0};
static bool  default_triangle2d_filled = false;
static float default_triangle2d_lw = 2.0f;

static float default_polygon2d_color[3] = {0, 0, 0};
static bool  default_polygon2d_filled = false;
static float default_polygon2d_lw = 2.0f;

static float default_circle2d_color[3] = {0, 0, 0};
static bool  default_circle2d_filled = false;
static float default_circle2d_lw = 2.0f;
static int   default_circle2d_segments = 64;
static float default_circle2d_normal[3] = {0, 1, 0};

static float default_ellipse2d_color[3] = {0, 0, 0};
static bool  default_ellipse2d_filled = false;
static float default_ellipse2d_lw = 2.0f;
static int   default_ellipse2d_segments = 64;
static float default_ellipse2d_normal[3] = {0, 1, 0};
static float default_ellipse2d_angle = 0.0f;

/* ───────── Управление группами ───────── */
static int current_group = 0;
static int groups_count = 0;
static int active_view_group = 0;

/* ════════════════════════════════════════════════════════════════
   ВСПОМОГАТЕЛЬНАЯ МАТЕМАТИКА (внутренняя, static)
   ════════════════════════════════════════════════════════════════ */

static Vec3 v3(float x, float y, float z) {
    Vec3 v = {x,y,z};
    return v;
}

static Vec3 v3_add(Vec3 a, Vec3 b) {
    return v3(a.x+b.x, a.y+b.y, a.z+b.z);
}

static Vec3 v3_sub(Vec3 a, Vec3 b) {
    return v3(a.x-b.x, a.y-b.y, a.z-b.z);
}

static Vec3 v3_mul(Vec3 a, float s) {
    return v3(a.x*s, a.y*s, a.z*s);
}

static float v3_len(Vec3 a) {
    return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);
}

static float v3_dot(Vec3 a, Vec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

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

/* ════════════════════════════════════════════════════════════════
   ВНУТРЕННИЕ ФУНКЦИИ ОТРИСОВКИ И УПРАВЛЕНИЯ (static)
   ════════════════════════════════════════════════════════════════ */

static void _update_group_visibility(int group_id, bool visible) {
    for (int i = 0; i < PointCount; i++)
        if (PointS[i].group_id == group_id) PointS[i].visible = visible;
    for (int i = 0; i < LineCount; i++)
        if (LineS[i].group_id == group_id) LineS[i].visible = visible;
    for (int i = 0; i < PolyLineCount; i++)
        if (PolyLineS[i].group_id == group_id) PolyLineS[i].visible = visible;
    for (int i = 0; i < ArrowCount; i++)
        if (ArrowS[i].group_id == group_id) ArrowS[i].visible = visible;
    for (int i = 0; i < SphereCount; i++)
        if (SphereS[i].group_id == group_id) SphereS[i].visible = visible;
    for (int i = 0; i < CubeCount; i++)
        if (CubeS[i].group_id == group_id) CubeS[i].visible = visible;
    for (int i = 0; i < CylinderCount; i++)
        if (CylinderS[i].group_id == group_id) CylinderS[i].visible = visible;
    for (int i = 0; i < ConeCount; i++)
        if (ConeS[i].group_id == group_id) ConeS[i].visible = visible;
    for (int i = 0; i < TorusCount; i++)
        if (TorusS[i].group_id == group_id) TorusS[i].visible = visible;
    for (int i = 0; i < OutTextCount; i++)
        if (OutTextS[i].group_id == group_id) OutTextS[i].visible = visible;
    for (int i = 0; i < Triangle2DCount; i++)
        if (Triangle2DS[i].group_id == group_id) Triangle2DS[i].visible = visible;
    for (int i = 0; i < Polygon2DCount; i++)
        if (Polygon2DS[i].group_id == group_id) Polygon2DS[i].visible = visible;
    for (int i = 0; i < Circle2DCount; i++)
        if (Circle2DS[i].group_id == group_id) Circle2DS[i].visible = visible;
    for (int i = 0; i < Ellipse2DCount; i++)
        if (Ellipse2DS[i].group_id == group_id) Ellipse2DS[i].visible = visible;
}

static void draw_text_3d(float x, float y, float z, const char* text) {
    glRasterPos3f(x, y, z);
    for (const char* c = text; *c; ++c)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
}

static void draw_cone_aligned_3d(Vec3 p0, Vec3 p1, float radius, float height, bool solid) {
    Vec3 dir = v3_sub(p1, p0);
    float L = v3_len(dir);
    if (L <= 1e-6f) return;

    dir = v3_mul(dir, 1.0f / L);
    Vec3 base = v3_sub(p1, v3_mul(dir, height));

    glPushMatrix();
    glTranslatef(base.x, base.y, base.z);

    Vec3 z_axis = v3(0, 0, 1);
    float dot = v3_dot(z_axis, dir);

    if (dot > 0.9999f) {
        // Уже смотрит вдоль Z
    } else if (dot < -0.9999f) {
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    } else {
        Vec3 cross = v3_cross(z_axis, dir);
        float angle_deg = acosf(dot) * 180.0f / (float)M_PI;
        glRotatef(angle_deg, cross.x, cross.y, cross.z);
    }

    if (!g_quadric) g_quadric = gluNewQuadric();
    gluQuadricDrawStyle(g_quadric, solid ? GLU_FILL : GLU_LINE);
    gluCylinder(g_quadric, radius, 0.0f, height, 16, 1);

    if (solid) {
        glPushMatrix();
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        gluDisk(g_quadric, 0.0, radius, 16, 1);
        glPopMatrix();
    }
    glPopMatrix();
}

static void _build_plane_basis(Vec3 n, Vec3* out_u, Vec3* out_v) {
    n = v3_norm(n);
    Vec3 tmp = (fabsf(n.y) < 0.9f) ? v3(0,1,0) : v3(1,0,0);
    *out_u = v3_norm(v3_cross(tmp, n));
    *out_v = v3_cross(n, *out_u);
}

static void _draw_circle_or_ellipse(
    float cx, float cy, float cz,
    float rx, float ry,
    float angle_deg,
    int segments,
    float nx, float ny, float nz,
    bool filled)
{
    Vec3 n = v3_norm(v3(nx, ny, nz));
    Vec3 u, v;
    _build_plane_basis(n, &u, &v);

    float cos_a = cosf(DEG_RAD(angle_deg));
    float sin_a = sinf(DEG_RAD(angle_deg));

    if (filled) glBegin(GL_TRIANGLE_FAN);
    else        glBegin(GL_LINE_LOOP);

    if (filled) {
        glVertex3f(cx, cy, cz);
    }

    for (int i = 0; i <= segments; i++) {
        float t = 2.0f * (float)M_PI * i / segments;
        float lx = rx * cosf(t);
        float ly = ry * sinf(t);
        float ux = lx * cos_a - ly * sin_a;
        float uy = lx * sin_a + ly * cos_a;
        Vec3 p = v3(
            cx + ux * u.x + uy * v.x,
            cy + ux * u.y + uy * v.y,
            cz + ux * u.z + uy * v.z
        );
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
}

static void _draw_triangles_2d(void) {
    for (int i = 0; i < Triangle2DCount; i++) {
        if (!Triangle2DS[i].visible) continue;
        glColor3fv(Triangle2DS[i].color);
        if (Triangle2DS[i].filled) {
            glBegin(GL_TRIANGLES);
        } else {
            glLineWidth(Triangle2DS[i].line_width);
            glBegin(GL_LINE_LOOP);
        }
        glVertex3f(Triangle2DS[i].x0, Triangle2DS[i].y0, Triangle2DS[i].z0);
        glVertex3f(Triangle2DS[i].x1, Triangle2DS[i].y1, Triangle2DS[i].z1);
        glVertex3f(Triangle2DS[i].x2, Triangle2DS[i].y2, Triangle2DS[i].z2);
        glEnd();
    }
}

static void _draw_polygons_2d(void) {
    for (int i = 0; i < Polygon2DCount; i++) {
        if (!Polygon2DS[i].visible) continue;
        glColor3fv(Polygon2DS[i].color);
        if (Polygon2DS[i].filled) {
            glBegin(GL_POLYGON);
        } else {
            glLineWidth(Polygon2DS[i].line_width);
            glBegin(GL_LINE_LOOP);
        }
        for (int j = 0; j < Polygon2DS[i].vertex_count; j++)
            glVertex3f(Polygon2DS[i].x[j], Polygon2DS[i].y[j], Polygon2DS[i].z[j]);
        glEnd();
    }
}

static void _draw_circles_2d(void) {
    for (int i = 0; i < Circle2DCount; i++) {
        if (!Circle2DS[i].visible) continue;
        glColor3fv(Circle2DS[i].color);
        if (!Circle2DS[i].filled) glLineWidth(Circle2DS[i].line_width);
        _draw_circle_or_ellipse(
            Circle2DS[i].cx, Circle2DS[i].cy, Circle2DS[i].cz,
            Circle2DS[i].r, Circle2DS[i].r,
            0.0f,
            Circle2DS[i].segments,
            Circle2DS[i].nx, Circle2DS[i].ny, Circle2DS[i].nz,
            Circle2DS[i].filled
        );
    }
}

static void _draw_ellipses_2d(void) {
    for (int i = 0; i < Ellipse2DCount; i++) {
        if (!Ellipse2DS[i].visible) continue;
        glColor3fv(Ellipse2DS[i].color);
        if (!Ellipse2DS[i].filled) glLineWidth(Ellipse2DS[i].line_width);
        _draw_circle_or_ellipse(
            Ellipse2DS[i].cx, Ellipse2DS[i].cy, Ellipse2DS[i].cz,
            Ellipse2DS[i].rx, Ellipse2DS[i].ry,
            Ellipse2DS[i].angle_deg,
            Ellipse2DS[i].segments,
            Ellipse2DS[i].nx, Ellipse2DS[i].ny, Ellipse2DS[i].nz,
            Ellipse2DS[i].filled
        );
    }
}

static void draw_axes_3d(void) {
    if (!axes3d_enabled) return;

    float xmin = axis3d_limits[0], xmax = axis3d_limits[1];
    float ymin = axis3d_limits[2], ymax = axis3d_limits[3];
    float zmin = axis3d_limits[4], zmax = axis3d_limits[5];

    glLineWidth(axes3d_width);
    glBegin(GL_LINES);
        glColor3fv(axis_color_x);
        glVertex3f(xmin, ymin, zmin); glVertex3f(xmax, ymin, zmin);
        glColor3fv(axis_color_y);
        glVertex3f(xmin, ymin, zmin); glVertex3f(xmin, ymax, zmin);
        glColor3fv(axis_color_z);
        glVertex3f(xmin, ymin, zmin); glVertex3f(xmin, ymin, zmax);
    glEnd();

    // Наконечники осей
    float lenX = xmax - xmin; float hX = fmaxf(lenX * 0.05f, 0.2f); float rX = hX * 0.3f;
    glColor3fv(axis_color_x);
    draw_cone_aligned_3d(v3(xmin, ymin, zmin), v3(xmax, ymin, zmin), rX, hX, true);

    float lenY = ymax - ymin; float hY = fmaxf(lenY * 0.05f, 0.2f); float rY = hY * 0.3f;
    glColor3fv(axis_color_y);
    draw_cone_aligned_3d(v3(xmin, ymin, zmin), v3(xmin, ymax, zmin), rY, hY, true);

    float lenZ = zmax - zmin; float hZ = fmaxf(lenZ * 0.05f, 0.2f); float rZ = hZ * 0.3f;
    glColor3fv(axis_color_z);
    draw_cone_aligned_3d(v3(xmin, ymin, zmin), v3(xmin, ymin, zmax), rZ, hZ, true);

    // Сетка
    if (grid_visibility) {
        glLineWidth(1.0f);
        glColor3f(0.75f, 0.75f, 0.75f);
        glBegin(GL_LINES);
        for (float x = xmin; x <= xmax + 1e-4f; x += 1.0f) {
            glVertex3f(x, ymin, zmin); glVertex3f(x, ymin, zmax);
        }
        for (float z = zmin; z <= zmax + 1e-4f; z += 1.0f) {
            glVertex3f(xmin, ymin, z); glVertex3f(xmax, ymin, z);
        }
        glEnd();
    }

    // Подписи осей
    glColor3fv(axis_color_x);
    draw_text_3d(xmax+0.2f, ymin, zmin, "X");
    glColor3fv(axis_color_y);
    draw_text_3d(xmin, ymax+0.2f, zmin, "Y");
    glColor3fv(axis_color_z);
    draw_text_3d(xmin, ymin, zmax+0.2f, "Z");
}

static void camera_get_basis(Vec3* out_forward, Vec3* out_right, Vec3* out_up2) {
    Vec3 eye = v3(cam_eye[0], cam_eye[1], cam_eye[2]);
    Vec3 center = v3(cam_center[0], cam_center[1], cam_center[2]);
    Vec3 upv = v3_norm(v3(cam_up[0], cam_up[1], cam_up[2]));
    Vec3 forward = v3_norm(v3_sub(center, eye));

    float dot_test = fabsf(v3_dot(upv, forward));
    if (dot_test > 0.99f) {
        upv = (fabsf(forward.z) < 0.9f) ? v3(0,0,1) : v3(1,0,0);
    }

    Vec3 right = v3_norm(v3_cross(forward, upv));
    Vec3 up2 = v3_norm(v3_cross(right, forward));
    *out_forward = forward;
    *out_right = right;
    *out_up2 = up2;
}

static void reshape3d(int w, int h) {
    if (h <= 0) h = 1;
    window3d_width = w;
    window3d_height = h;
    glViewport(0, 0, w, h);
    glutPostRedisplay();
}

static void camera_orbit_update_eye(void) {
    float yaw = DEG_RAD(orbit_yaw_deg);
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
    Vec3 eye = v3(cam_eye[0], cam_eye[1], cam_eye[2]);
    Vec3 center = v3(cam_center[0], cam_center[1], cam_center[2]);
    Vec3 off = v3_sub(eye, center);
    float d = v3_len(off);
    if (d <= 1e-6f) d = 1.0f;
    orbit_dist = d;
    orbit_yaw_deg = RAD_DEG(atan2f(off.x, off.z));
    float s = off.y / d;
    if (s > 1.0f) s = 1.0f;
    if (s < -1.0f) s = -1.0f;
    orbit_pitch_deg = RAD_DEG(asinf(s));
    if (orbit_pitch_deg < orbit_pitch_min) orbit_pitch_deg = orbit_pitch_min;
    if (orbit_pitch_deg > orbit_pitch_max) orbit_pitch_deg = orbit_pitch_max;
}

static void display3d(void) {
    if (!g_quadric) g_quadric = gluNewQuadric();

    glClearColor(clear_color3d[0], clear_color3d[1], clear_color3d[2], clear_color3d[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_LINE_SMOOTH); glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SMOOTH); glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    float aspect = (float)window3d_width / (float)window3d_height;
    gluPerspective(proj_fov_y, aspect, proj_near_, proj_far_);

    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    gluLookAt(cam_eye[0], cam_eye[1], cam_eye[2],
              cam_center[0], cam_center[1], cam_center[2],
              cam_up[0], cam_up[1], cam_up[2]);

    draw_axes_3d();

    // === 3D ПРИМИТИВЫ ===
    for (int i = 0; i < TorusCount; ++i) {
        if (!TorusS[i].visible) continue;
        glPushMatrix();
        glTranslatef(TorusS[i].x, TorusS[i].y, TorusS[i].z);
        glColor3fv(TorusS[i].color);
        glLineWidth(TorusS[i].line_width);
        if (TorusS[i].solid) glutSolidTorus(TorusS[i].inner_r, TorusS[i].outer_r, TorusS[i].sides, TorusS[i].rings);
        else glutWireTorus(TorusS[i].inner_r, TorusS[i].outer_r, TorusS[i].sides, TorusS[i].rings);
        glPopMatrix();
    }

    for (int i = 0; i < SphereCount; ++i) {
        if (!SphereS[i].visible) continue;
        glPushMatrix();
        glTranslatef(SphereS[i].x, SphereS[i].y, SphereS[i].z);
        glColor3fv(SphereS[i].color);
        glLineWidth(SphereS[i].line_width);
        if (SphereS[i].solid) glutSolidSphere(SphereS[i].r, SphereS[i].slices, SphereS[i].stacks);
        else glutWireSphere(SphereS[i].r, SphereS[i].slices, SphereS[i].stacks);
        glPopMatrix();
    }

    for (int i = 0; i < CubeCount; ++i) {
        if (!CubeS[i].visible) continue;
        glPushMatrix();
        glTranslatef(CubeS[i].x, CubeS[i].y, CubeS[i].z);
        glColor3fv(CubeS[i].color);
        glLineWidth(CubeS[i].line_width);
        if (CubeS[i].solid) glutSolidCube(CubeS[i].size);
        else glutWireCube(CubeS[i].size);
        glPopMatrix();
    }

    for (int i = 0; i < CylinderCount; ++i) {
        if (!CylinderS[i].visible) continue;
        glPushMatrix();
        glTranslatef(CylinderS[i].x, CylinderS[i].y, CylinderS[i].z - CylinderS[i].h * 0.5f);
        glColor3fv(CylinderS[i].color);
        glLineWidth(CylinderS[i].line_width);
        gluQuadricDrawStyle(g_quadric, CylinderS[i].solid ? GLU_FILL : GLU_LINE);
        gluCylinder(g_quadric, CylinderS[i].r, CylinderS[i].r, CylinderS[i].h, CylinderS[i].slices, CylinderS[i].stacks);
        if (CylinderS[i].caps) {
            glPushMatrix();
            gluQuadricDrawStyle(g_quadric, CylinderS[i].solid ? GLU_FILL : GLU_LINE);
            gluDisk(g_quadric, 0.0, CylinderS[i].r, CylinderS[i].slices, 1);
            glPopMatrix();
            glPushMatrix();
            glTranslatef(0,0,CylinderS[i].h);
            gluQuadricDrawStyle(g_quadric, CylinderS[i].solid ? GLU_FILL : GLU_LINE);
            gluDisk(g_quadric, 0.0, CylinderS[i].r, CylinderS[i].slices, 1);
            glPopMatrix();
        }
        glPopMatrix();
    }

    for (int i = 0; i < ConeCount; ++i) {
        if (!ConeS[i].visible) continue;
        glPushMatrix();
        glTranslatef(ConeS[i].x, ConeS[i].y, ConeS[i].z - ConeS[i].h * 0.5f);
        glColor3fv(ConeS[i].color);
        glLineWidth(ConeS[i].line_width);
        gluQuadricDrawStyle(g_quadric, ConeS[i].solid ? GLU_FILL : GLU_LINE);
        gluCylinder(g_quadric, ConeS[i].r, 0.0, ConeS[i].h, ConeS[i].slices, ConeS[i].stacks);
        if (ConeS[i].caps) {
            gluQuadricDrawStyle(g_quadric, ConeS[i].solid ? GLU_FILL : GLU_LINE);
            gluDisk(g_quadric, 0.0, ConeS[i].r, ConeS[i].slices, 1);
        }
        glPopMatrix();
    }

    for (int i = 0; i < ArrowCount; ++i) {
        if (!ArrowS[i].visible) continue;
        glColor3fv(ArrowS[i].color);
        glLineWidth(ArrowS[i].width);
        Vec3 p0 = v3(ArrowS[i].x0, ArrowS[i].y0, ArrowS[i].z0);
        Vec3 p1 = v3(ArrowS[i].x1, ArrowS[i].y1, ArrowS[i].z1);
        glBegin(GL_LINES);
            glVertex3f(p0.x,p0.y,p0.z); glVertex3f(p1.x,p1.y,p1.z);
        glEnd();
        float L = v3_len(v3_sub(p1,p0));
        float head = 0.12f * L;
        if (head > 0.5f) head = 0.5f;
        if (head < 0.08f) head = 0.08f;
        draw_cone_aligned_3d(p0, p1, head * 0.3f, head, true);
    }

    for (int i = 0; i < PolyLineCount; ++i) {
        if (!PolyLineS[i].visible) continue;
        glColor3fv(PolyLineS[i].color);
        glLineWidth(PolyLineS[i].width);
        glBegin(GL_LINES);
        for (int j = 1; j < PolyLineS[i].vertex_count; ++j) {
            glVertex3f(PolyLineS[i].x[j-1], PolyLineS[i].y[j-1], PolyLineS[i].z[j-1]);
            glVertex3f(PolyLineS[i].x[j],   PolyLineS[i].y[j],   PolyLineS[i].z[j]);
        }
        glEnd();
    }

    for (int i = 0; i < LineCount; ++i) {
        if (!LineS[i].visible) continue;
        glColor3fv(LineS[i].color);
        glLineWidth(LineS[i].width);
        glBegin(GL_LINES);
            glVertex3f(LineS[i].x0, LineS[i].y0, LineS[i].z0);
            glVertex3f(LineS[i].x1, LineS[i].y1, LineS[i].z1);
        glEnd();
    }

    for (int i = 0; i < PointCount; ++i) {
        if (!PointS[i].visible) continue;
        glColor3fv(PointS[i].color);
        glPointSize(PointS[i].size);
        glBegin(GL_POINTS);
            glVertex3f(PointS[i].x, PointS[i].y, PointS[i].z);
        glEnd();
    }

    for (int i = 0; i < OutTextCount; ++i) {
        if (!OutTextS[i].visible) continue;
        glColor3fv(OutTextS[i].color);
        draw_text_3d(OutTextS[i].x, OutTextS[i].y, OutTextS[i].z, OutTextS[i].text);
    }

    // === 2D ПРИМИТИВЫ ===
    _draw_triangles_2d();
    _draw_polygons_2d();
    _draw_circles_2d();
    _draw_ellipses_2d();

    glutSwapBuffers();
}

static void mouse3d(int button, int state, int x, int y) {
    if (!camera_control_enabled) return;
    mouse_last_x = x; mouse_last_y = y;
    if (button == GLUT_LEFT_BUTTON) mouse_lmb_down = (state == GLUT_DOWN);
    if (button == GLUT_RIGHT_BUTTON) mouse_rmb_down = (state == GLUT_DOWN);
}

static void motion3d(int x, int y) {
    if (!camera_control_enabled) return;
    int dx = x - mouse_last_x;
    int dy = y - mouse_last_y;
    mouse_last_x = x; mouse_last_y = y;

    if (mouse_lmb_down) {
        orbit_yaw_deg   += dx * orbit_rot_sens;
        orbit_pitch_deg += dy * orbit_rot_sens;
        if (orbit_pitch_deg < orbit_pitch_min) orbit_pitch_deg = orbit_pitch_min;
        if (orbit_pitch_deg > orbit_pitch_max) orbit_pitch_deg = orbit_pitch_max;
        camera_orbit_update_eye();
        glutPostRedisplay();
    }
    else if (mouse_rmb_down) {
        Vec3 forward, right, up;
        camera_get_basis(&forward, &right, &up);
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
    else orbit_dist *= orbit_zoom_mul;
    if (orbit_dist < 0.05f) orbit_dist = 0.05f;
    camera_orbit_update_eye();
    glutPostRedisplay();
}

static void keyboard3d(unsigned char key, int x, int y) {
    (void)x; (void)y;
    if (!camera_control_enabled) return;

    Vec3 forward, right, up;
    camera_get_basis(&forward, &right, &up);
    float speed = orbit_dist * 0.05f;
    if (speed < 0.1f) speed = 0.1f;
    Vec3 move = v3(0,0,0);
    static int ui_cycle = 0;

    switch(key) {
        case 'w': case 'W': move = v3_add(move, v3_mul(forward, speed)); break;
        case 's': case 'S': move = v3_sub(move, v3_mul(forward, speed)); break;
        case 'a': case 'A': move = v3_sub(move, v3_mul(right, speed)); break;
        case 'd': case 'D': move = v3_add(move, v3_mul(right, speed)); break;
        case 'e': case 'E': move = v3_add(move, v3(0, speed, 0)); break;
        case 'q': case 'Q': move = v3_sub(move, v3(0, speed, 0)); break;
        case 'r': case 'R':
            cam_center[0] = default_cam_center[0]; cam_center[1] = default_cam_center[1]; cam_center[2] = default_cam_center[2];
            orbit_yaw_deg = 45.0f; orbit_pitch_deg = -25.0f; orbit_dist = 10.0f;
            camera_orbit_update_eye();
            glutPostRedisplay();
            return;
        case ' ':
            if (groups_count == 0) break;
            eg3d_show_only_group(active_view_group);
            ui_cycle = active_view_group;
            active_view_group++;
            if (active_view_group >= groups_count) active_view_group = 0;
            break;
        case 'c': case 'C':
            if(groups_count > 0) {
                eg3d_clear_group(ui_cycle);
                printf("Группа %d удалена.\n", ui_cycle);
            }
            break;
        case 'o': case 'O': eg3d_show_all_groups(); break;
        case 'h': case 'H': eg3d_hide_all_groups(); break;
        case 'l': case 'L':
            printf("Всего групп: %d, Отображаемая: %d\n", groups_count, active_view_group);
            break;
        case '+':
            if(groups_count > 0) {
                eg3d_set_group_visibility(active_view_group, true);
                ui_cycle++;
                if(ui_cycle >= groups_count) ui_cycle = 0;
            } break;
        case '-':
            if(groups_count > 0) {
                ui_cycle--;
                if(ui_cycle < 0) ui_cycle = groups_count - 1;
                eg3d_set_group_visibility(active_view_group, false);
            } break;
        case 27: eg3d_cleanup_all(); exit(0); break;
    }

    if (move.x != 0 || move.y != 0 || move.z != 0) {
        cam_center[0] += move.x;
        cam_center[1] += move.y;
        cam_center[2] += move.z;
        camera_orbit_update_eye();
        glutPostRedisplay();
    }
}

/* ════════════════════════════════════════════════════════════════
   ПУБЛИЧНЫЙ API: РЕАЛИЗАЦИЯ
   ════════════════════════════════════════════════════════════════ */

/* ───────── Группы ───────── */
int eg3d_create_group(void) {
    int new_group_id = groups_count;
    groups_count++;
    current_group = new_group_id;
    return new_group_id;
}

void eg3d_select_group(int group_id) {
    if (group_id < 0 || group_id >= groups_count) {
        fprintf(stderr, "Ошибка: группа %d не существует. Доступно групп: %d\n", group_id, groups_count);
        return;
    }
    current_group = group_id;
}

void eg3d_show_only_group(int group_id) {
    eg3d_hide_all_groups();
    if (group_id < 0) {
        eg3d_show_all_groups();
        return;
    }
    if (group_id >= groups_count) {
        fprintf(stderr, "Предупреждение: группа %d не существует (всего групп: %d)\n", group_id, groups_count);
        return;
    }
    _update_group_visibility(group_id, true);
    active_view_group = group_id;
    glutPostRedisplay();
}

void eg3d_set_group_visibility(int group_id, bool visible) {
    if (group_id < 0 || group_id >= groups_count) {
        fprintf(stderr, "Ошибка: группа %d не существует\n", group_id);
        return;
    }
    _update_group_visibility(group_id, visible);
    glutPostRedisplay();
}

void eg3d_show_groups(int* group_ids, int count) {
    eg3d_hide_all_groups();
    for (int i = 0; i < count; i++) {
        if (group_ids[i] >= 0 && group_ids[i] < groups_count) {
            _update_group_visibility(group_ids[i], true);
        }
    }
    glutPostRedisplay();
}

void eg3d_hide_all_groups(void) {
    for (int i = 0; i < PointCount; i++) PointS[i].visible = false;
    for (int i = 0; i < LineCount; i++) LineS[i].visible = false;
    for (int i = 0; i < PolyLineCount; i++) PolyLineS[i].visible = false;
    for (int i = 0; i < ArrowCount; i++) ArrowS[i].visible = false;
    for (int i = 0; i < SphereCount; i++) SphereS[i].visible = false;
    for (int i = 0; i < CubeCount; i++) CubeS[i].visible = false;
    for (int i = 0; i < CylinderCount; i++) CylinderS[i].visible = false;
    for (int i = 0; i < ConeCount; i++) ConeS[i].visible = false;
    for (int i = 0; i < TorusCount; i++) TorusS[i].visible = false;
    for (int i = 0; i < OutTextCount; i++) OutTextS[i].visible = false;
    for (int i = 0; i < Triangle2DCount; i++) Triangle2DS[i].visible = false;
    for (int i = 0; i < Polygon2DCount; i++) Polygon2DS[i].visible = false;
    for (int i = 0; i < Circle2DCount; i++) Circle2DS[i].visible = false;
    for (int i = 0; i < Ellipse2DCount; i++) Ellipse2DS[i].visible = false;
    glutPostRedisplay();
}

void eg3d_show_all_groups(void) {
    for (int i = 0; i < PointCount; i++) PointS[i].visible = true;
    for (int i = 0; i < LineCount; i++) LineS[i].visible = true;
    for (int i = 0; i < PolyLineCount; i++) PolyLineS[i].visible = true;
    for (int i = 0; i < ArrowCount; i++) ArrowS[i].visible = true;
    for (int i = 0; i < SphereCount; i++) SphereS[i].visible = true;
    for (int i = 0; i < CubeCount; i++) CubeS[i].visible = true;
    for (int i = 0; i < CylinderCount; i++) CylinderS[i].visible = true;
    for (int i = 0; i < ConeCount; i++) ConeS[i].visible = true;
    for (int i = 0; i < TorusCount; i++) TorusS[i].visible = true;
    for (int i = 0; i < OutTextCount; i++) OutTextS[i].visible = true;
    for (int i = 0; i < Triangle2DCount; i++) Triangle2DS[i].visible = true;
    for (int i = 0; i < Polygon2DCount; i++) Polygon2DS[i].visible = true;
    for (int i = 0; i < Circle2DCount; i++) Circle2DS[i].visible = true;
    for (int i = 0; i < Ellipse2DCount; i++) Ellipse2DS[i].visible = true;
    glutPostRedisplay();
}

void eg3d_clear_group(int group_id) {
    if (group_id < 0 || group_id >= groups_count) {
        fprintf(stderr, "Ошибка: группа %d не существует\n", group_id);
        return;
    }

    #define CLEAR_SIMPLE(Type, Arr, Cnt) \
        { int w=0; for(int i=0;i<Cnt;i++) if(Arr[i].group_id!=group_id) { if(w!=i) Arr[w]=Arr[i]; w++; } Cnt=w; Arr=(Type*)realloc(Arr, Cnt*sizeof(Type)); }

    CLEAR_SIMPLE(Point, PointS, PointCount);
    CLEAR_SIMPLE(Line, LineS, LineCount);
    CLEAR_SIMPLE(Arrow, ArrowS, ArrowCount);
    CLEAR_SIMPLE(Sphere, SphereS, SphereCount);
    CLEAR_SIMPLE(Cube, CubeS, CubeCount);
    CLEAR_SIMPLE(Cylinder, CylinderS, CylinderCount);
    CLEAR_SIMPLE(Cone, ConeS, ConeCount);
    CLEAR_SIMPLE(Torus, TorusS, TorusCount);
    CLEAR_SIMPLE(Triangle2D, Triangle2DS, Triangle2DCount);
    CLEAR_SIMPLE(Circle2D, Circle2DS, Circle2DCount);
    CLEAR_SIMPLE(Ellipse2D, Ellipse2DS, Ellipse2DCount);

    {
        int w=0;
        for(int i=0;i<PolyLineCount;i++) {
            if(PolyLineS[i].group_id != group_id) {
                if(w != i) PolyLineS[w] = PolyLineS[i];
                w++;
            } else {
                if(PolyLineS[i].x) free(PolyLineS[i].x);
                if(PolyLineS[i].y) free(PolyLineS[i].y);
                if(PolyLineS[i].z) free(PolyLineS[i].z);
            }
        }
        PolyLineCount = w;
        PolyLineS = (PolyLine*)realloc(PolyLineS, PolyLineCount * sizeof(PolyLine));
    }

    {
        int w=0;
        for(int i=0;i<Polygon2DCount;i++) {
            if(Polygon2DS[i].group_id != group_id) {
                if(w != i) Polygon2DS[w] = Polygon2DS[i];
                w++;
            } else {
                if(Polygon2DS[i].x) free(Polygon2DS[i].x);
                if(Polygon2DS[i].y) free(Polygon2DS[i].y);
                if(Polygon2DS[i].z) free(Polygon2DS[i].z);
            }
        }
        Polygon2DCount = w;
        Polygon2DS = (Polygon2D*)realloc(Polygon2DS, Polygon2DCount * sizeof(Polygon2D));
    }

    {
        int w=0;
        for(int i=0;i<OutTextCount;i++) {
            if(OutTextS[i].group_id != group_id) {
                if(w != i) OutTextS[w] = OutTextS[i];
                w++;
            } else {
                if(OutTextS[i].text) free(OutTextS[i].text);
            }
        }
        OutTextCount = w;
        OutTextS = (OutText*)realloc(OutTextS, OutTextCount * sizeof(OutText));
    }

    #define REINDEX(Arr, Cnt) for(int i=0;i<Cnt;i++) if(Arr[i].group_id > group_id) Arr[i].group_id--

    REINDEX(PointS, PointCount);           REINDEX(LineS, LineCount);
    REINDEX(PolyLineS, PolyLineCount);     REINDEX(ArrowS, ArrowCount);
    REINDEX(SphereS, SphereCount);         REINDEX(CubeS, CubeCount);
    REINDEX(CylinderS, CylinderCount);     REINDEX(ConeS, ConeCount);
    REINDEX(TorusS, TorusCount);           REINDEX(OutTextS, OutTextCount);
    REINDEX(Triangle2DS, Triangle2DCount); REINDEX(Polygon2DS, Polygon2DCount);
    REINDEX(Circle2DS, Circle2DCount);     REINDEX(Ellipse2DS, Ellipse2DCount);

    groups_count--;
    if (current_group > group_id) current_group--;
    else if (current_group == group_id && groups_count > 0)
        if (current_group >= groups_count) current_group = groups_count - 1;

    if (groups_count > 0) eg3d_show_all_groups();
    else eg3d_hide_all_groups();
    active_view_group = 0;
}

/* ───────── Настройка окна и сцены ───────── */
void eg3d_window_position3d(int x_position, int y_position) {
    win3d_pos_x = x_position; win3d_pos_y = y_position;
}

void eg3d_window3d(int width, int height) {
    window3d_width = width; window3d_height = height;
}

void eg3d_background_color3d(int r, int g, int b) {
    clear_color3d[0] = r / 255.0f; clear_color3d[1] = g / 255.0f; clear_color3d[2] = b / 255.0f; clear_color3d[3] = 1.0f;
    glutPostRedisplay();
}

void eg3d_camera3d(float ex,float ey,float ez, float cx,float cy,float cz, float ux,float uy,float uz) {
    cam_eye[0]=ex; cam_eye[1]=ey; cam_eye[2]=ez;
    cam_center[0]=cx; cam_center[1]=cy; cam_center[2]=cz;
    cam_up[0]=ux; cam_up[1]=uy; cam_up[2]=uz;

    default_cam_eye[0] = ex; default_cam_eye[1] = ey; default_cam_eye[2] = ez;
    default_cam_center[0] = cx; default_cam_center[1] = cy; default_cam_center[2] = cz;

    camera_orbit_sync_from_lookat();
    glutPostRedisplay();
}

void eg3d_camera_control3d(const char* choice) {
    if (!choice) return;
    if (strcmp(choice, "ON")==0 || strcmp(choice, "on")==0 || strcmp(choice, "1")==0) camera_control_enabled = true;
    else if (strcmp(choice, "OFF")==0 || strcmp(choice, "off")==0 || strcmp(choice, "0")==0) camera_control_enabled = false;
}

void eg3d_set_camera_control_sensitivity3d(float rot_sens, float pan_sens, float zoom_mul) {
    if (rot_sens > 0.0f) orbit_rot_sens = rot_sens;
    if (pan_sens > 0.0f) orbit_pan_sens = pan_sens;
    if (zoom_mul > 1.001f) orbit_zoom_mul = zoom_mul;
}

void eg3d_axes_visibility3d(const char* choice) {
    if (!choice) return;
    if (strcmp(choice, "ON") == 0 || strcmp(choice, "on") == 0 || strcmp(choice, "1") == 0) axes3d_enabled = true;
    else if (strcmp(choice, "OFF") == 0 || strcmp(choice, "off") == 0 || strcmp(choice, "0") == 0) axes3d_enabled = false;
    glutPostRedisplay();
}

void eg3d_axes_colored3d(const char* choice) {
    if (!choice) return;
    float c = 0.0f;
    if (strcmp(choice, "ON") == 0 || strcmp(choice, "on") == 0 || strcmp(choice, "1") == 0) c = 1.0f;
    else if (strcmp(choice, "OFF") == 0 || strcmp(choice, "off") == 0 || strcmp(choice, "0") == 0) c = 0.0f;
    axis_color_x[0] = c; axis_color_y[1] = c; axis_color_z[2] = c;
    glutPostRedisplay();
}

void eg3d_grid_visibility_enable(const char* choice) {
    if (!choice) return;
    if (strcmp(choice, "ON") == 0 || strcmp(choice, "on") == 0 || strcmp(choice, "1") == 0) grid_visibility = true;
    else if (strcmp(choice, "OFF") == 0 || strcmp(choice, "off") == 0 || strcmp(choice, "0") == 0) grid_visibility = false;
    glutPostRedisplay();
}

void eg3d_axis3d(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax) {
    axis3d_limits[0]=xmin; axis3d_limits[1]=xmax;
    axis3d_limits[2]=ymin; axis3d_limits[3]=ymax;
    axis3d_limits[4]=zmin; axis3d_limits[5]=zmax;

    cam_eye[0]=xmax+3; cam_eye[1]=ymax+3; cam_eye[2]=zmax+3;
    cam_center[0]=xmin; cam_center[1]=ymin; cam_center[2]=zmin;
    default_cam_eye[0] = cam_eye[0]; default_cam_eye[1] = cam_eye[1]; default_cam_eye[2] = cam_eye[2];
    default_cam_center[0] = cam_center[0]; default_cam_center[1] = cam_center[1]; default_cam_center[2] = cam_center[2];

    int argc = 1;
    char* argv[1] = {(char*)"EasyGLUT3D"};
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(window3d_width, window3d_height);
    glutInitWindowPosition(win3d_pos_x, win3d_pos_y);
    glutCreateWindow("EasyGLUT3D");

    glutDisplayFunc(display3d);
    glutReshapeFunc(reshape3d);
    glutKeyboardFunc(keyboard3d);
    glutMouseFunc(mouse3d);
    glutMotionFunc(motion3d);
    glutMouseWheelFunc(mousewheel3d);

    camera_orbit_sync_from_lookat();
    camera_orbit_update_eye();
    glutPostRedisplay();
    glClearColor(clear_color3d[0], clear_color3d[1], clear_color3d[2], clear_color3d[3]);

    eg3d_create_group();
}

/* ───────── 3D примитивы ───────── */
void eg3d_set_outtext_color(int r, int g, int b) {
    default_text_color[0] = r/255.0f; default_text_color[1] = g/255.0f; default_text_color[2] = b/255.0f;
}

void eg3d_outtext(float x, float y, float z, const char* text) {
    if (!text) return;
    OutText* temp = (OutText*)realloc(OutTextS, (OutTextCount+1)*sizeof(OutText));
    if (!temp) return;
    OutTextS = temp;
    OutTextS[OutTextCount].text = (char*)malloc(strlen(text) + 1);
    strcpy(OutTextS[OutTextCount].text, text);
    OutTextS[OutTextCount].x=x; OutTextS[OutTextCount].y=y; OutTextS[OutTextCount].z=z;
    memcpy(OutTextS[OutTextCount].color, default_text_color, 3*sizeof(float));
    OutTextS[OutTextCount].group_id = current_group;
    OutTextS[OutTextCount].visible = true;
    OutTextCount++;
    glutPostRedisplay();
}

void eg3d_set_point3d_size(float size) { default_point3d_size = size; }
void eg3d_set_point3d_color(int r,int g,int b) {
    default_point3d_color[0]=r/255.0f; default_point3d_color[1]=g/255.0f; default_point3d_color[2]=b/255.0f;
}
void eg3d_point3d(float x,float y,float z) {
    Point* temp = (Point*)realloc(PointS, (PointCount+1)*sizeof(Point));
    if (!temp) return;
    PointS = temp;
    PointS[PointCount].x=x; PointS[PointCount].y=y; PointS[PointCount].z=z;
    memcpy(PointS[PointCount].color, default_point3d_color, 3*sizeof(float));
    PointS[PointCount].size = default_point3d_size;
    PointS[PointCount].group_id = current_group;
    PointS[PointCount].visible = true;
    PointCount++;
    glutPostRedisplay();
}

void eg3d_set_line3d_width(float width) { default_Line_width = width; }
void eg3d_set_line3d_color(int r,int g,int b) {
    default_Line_color[0]=r/255.0f; default_Line_color[1]=g/255.0f; default_Line_color[2]=b/255.0f;
}
void eg3d_line3d(float x0,float y0,float z0, float x1,float y1,float z1) {
    Line* temp = (Line*)realloc(LineS, (LineCount+1)*sizeof(Line));
    if (!temp) return;
    LineS = temp;
    LineS[LineCount].x0=x0; LineS[LineCount].y0=y0; LineS[LineCount].z0=z0;
    LineS[LineCount].x1=x1; LineS[LineCount].y1=y1; LineS[LineCount].z1=z1;
    memcpy(LineS[LineCount].color, default_Line_color, 3*sizeof(float));
    LineS[LineCount].width = default_Line_width;
    LineS[LineCount].group_id = current_group;
    LineS[LineCount].visible = true;
    LineCount++;
    glutPostRedisplay();
}

void eg3d_set_polyline3d_width(float width) { default_polyLine_width = width; }
void eg3d_set_polyline3d_color(int r,int g,int b) {
    default_polyLine_color[0]=r/255.0f; default_polyLine_color[1]=g/255.0f; default_polyLine_color[2]=b/255.0f;
}
void eg3d_polyline3d(float* x, float* y, float* z, int vertex_count) {
    if (vertex_count < 2) return;
    PolyLine* temp = (PolyLine*)realloc(PolyLineS, (PolyLineCount+1)*sizeof(PolyLine));
    if (!temp) return;
    PolyLineS = temp;
    PolyLineS[PolyLineCount].x = (float*)malloc(vertex_count*sizeof(float));
    PolyLineS[PolyLineCount].y = (float*)malloc(vertex_count*sizeof(float));
    PolyLineS[PolyLineCount].z = (float*)malloc(vertex_count*sizeof(float));
    memcpy(PolyLineS[PolyLineCount].x, x, vertex_count*sizeof(float));
    memcpy(PolyLineS[PolyLineCount].y, y, vertex_count*sizeof(float));
    memcpy(PolyLineS[PolyLineCount].z, z, vertex_count*sizeof(float));
    PolyLineS[PolyLineCount].vertex_count = vertex_count;
    memcpy(PolyLineS[PolyLineCount].color, default_polyLine_color, 3*sizeof(float));
    PolyLineS[PolyLineCount].width = default_polyLine_width;
    PolyLineS[PolyLineCount].group_id = current_group;
    PolyLineS[PolyLineCount].visible = true;
    PolyLineCount++;
    glutPostRedisplay();
}

void eg3d_set_arrow3d_width(float width) { default_arrow3d_width = width; }
void eg3d_set_arrow3d_color(int r,int g,int b) {
    default_arrow3d_color[0]=r/255.0f; default_arrow3d_color[1]=g/255.0f; default_arrow3d_color[2]=b/255.0f;
}
void eg3d_arrow3d(float x0,float y0,float z0, float x1,float y1,float z1) {
    Arrow* temp = (Arrow*)realloc(ArrowS, (ArrowCount+1)*sizeof(Arrow));
    if (!temp) return;
    ArrowS = temp;
    ArrowS[ArrowCount].x0=x0; ArrowS[ArrowCount].y0=y0; ArrowS[ArrowCount].z0=z0;
    ArrowS[ArrowCount].x1=x1; ArrowS[ArrowCount].y1=y1; ArrowS[ArrowCount].z1=z1;
    memcpy(ArrowS[ArrowCount].color, default_arrow3d_color, 3*sizeof(float));
    ArrowS[ArrowCount].width = default_arrow3d_width;
    ArrowS[ArrowCount].group_id = current_group;
    ArrowS[ArrowCount].visible = true;
    ArrowCount++;
    glutPostRedisplay();
}

void eg3d_set_sphere3d_color(int r,int g,int b) {
    default_sphere3d_color[0]=r/255.0f; default_sphere3d_color[1]=g/255.0f; default_sphere3d_color[2]=b/255.0f;
}
void eg3d_set_sphere3d_mode(const char* mode) {
    if (!mode) return;
    if (strcmp(mode,"SOLID")==0 || strcmp(mode,"S")==0) default_sphere3d_solid = true;
    else if (strcmp(mode,"WIRE")==0 || strcmp(mode,"W")==0) default_sphere3d_solid = false;
}
void eg3d_set_sphere3d_quality(int slices, int stacks) {
    if(slices>2) default_sphere3d_slices=slices;
    if(stacks>2) default_sphere3d_stacks=stacks;
}
void eg3d_set_sphere3d_line_width(float w) { default_sphere3d_line_width = w; }
void eg3d_sphere3d(float x,float y,float z, float r) {
    Sphere* temp = (Sphere*)realloc(SphereS, (SphereCount+1)*sizeof(Sphere));
    if (!temp) return;
    SphereS = temp;
    SphereS[SphereCount].x=x; SphereS[SphereCount].y=y; SphereS[SphereCount].z=z; SphereS[SphereCount].r=r;
    memcpy(SphereS[SphereCount].color, default_sphere3d_color, 3*sizeof(float));
    SphereS[SphereCount].slices = default_sphere3d_slices; SphereS[SphereCount].stacks = default_sphere3d_stacks;
    SphereS[SphereCount].solid = default_sphere3d_solid; SphereS[SphereCount].line_width = default_sphere3d_line_width;
    SphereS[SphereCount].group_id = current_group;
    SphereS[SphereCount].visible = true;
    SphereCount++; glutPostRedisplay();
}

void eg3d_set_cube3d_color(int r,int g,int b) {
    default_cube3d_color[0]=r/255.0f; default_cube3d_color[1]=g/255.0f; default_cube3d_color[2]=b/255.0f;
}
void eg3d_set_cube3d_mode(const char* mode) {
    if (!mode) return;
    if (strcmp(mode,"SOLID")==0 || strcmp(mode,"S")==0) default_cube3d_solid = true;
    else if (strcmp(mode,"WIRE")==0 || strcmp(mode,"W")==0) default_cube3d_solid = false;
}
void eg3d_set_cube3d_line_width(float w) { default_cube3d_line_width = w; }
void eg3d_cube3d(float x,float y,float z, float size) {
    Cube* temp = (Cube*)realloc(CubeS, (CubeCount+1)*sizeof(Cube));
    if (!temp) return;
    CubeS = temp;
    CubeS[CubeCount].x=x; CubeS[CubeCount].y=y; CubeS[CubeCount].z=z; CubeS[CubeCount].size=size;
    memcpy(CubeS[CubeCount].color, default_cube3d_color, 3*sizeof(float));
    CubeS[CubeCount].solid = default_cube3d_solid; CubeS[CubeCount].line_width = default_cube3d_line_width;
    CubeS[CubeCount].group_id = current_group;
    CubeS[CubeCount].visible = true;
    CubeCount++; glutPostRedisplay();
}

void eg3d_set_cylinder3d_color(int r,int g,int b) {
    default_cylinder3d_color[0]=r/255.0f; default_cylinder3d_color[1]=g/255.0f; default_cylinder3d_color[2]=b/255.0f;
}
void eg3d_set_cylinder3d_mode(const char* mode) {
    if (!mode) return;
    if (strcmp(mode,"SOLID")==0 || strcmp(mode,"S")==0) default_cylinder3d_solid = true;
    else if (strcmp(mode,"WIRE")==0 || strcmp(mode,"W")==0) default_cylinder3d_solid = false;
}
void eg3d_set_cylinder3d_caps(const char* choice) {
    if (!choice) return;
    if (strcmp(choice, "ON")==0 || strcmp(choice, "1")==0) default_cylinder3d_caps = true;
    else if (strcmp(choice,"OFF")==0 || strcmp(choice,"0")==0) default_cylinder3d_caps = false;
}
void eg3d_set_cylinder3d_quality(int slices, int stacks) {
    if(slices>2) default_cylinder3d_slices=slices;
    if(stacks>0) default_cylinder3d_stacks=stacks;
}
void eg3d_set_cylinder3d_line_width(float w) { default_cylinder3d_line_width = w; }
void eg3d_cylinder3d(float x,float y,float z, float r, float h) {
    Cylinder* temp = (Cylinder*)realloc(CylinderS, (CylinderCount+1)*sizeof(Cylinder));
    if (!temp) return;
    CylinderS = temp;
    CylinderS[CylinderCount].x=x; CylinderS[CylinderCount].y=y; CylinderS[CylinderCount].z=z;
    CylinderS[CylinderCount].r=r; CylinderS[CylinderCount].h=h;
    memcpy(CylinderS[CylinderCount].color, default_cylinder3d_color, 3*sizeof(float));
    CylinderS[CylinderCount].solid = default_cylinder3d_solid; CylinderS[CylinderCount].caps = default_cylinder3d_caps;
    CylinderS[CylinderCount].slices = default_cylinder3d_slices; CylinderS[CylinderCount].stacks = default_cylinder3d_stacks;
    CylinderS[CylinderCount].line_width = default_cylinder3d_line_width;
    CylinderS[CylinderCount].group_id = current_group;
    CylinderS[CylinderCount].visible = true;
    CylinderCount++; glutPostRedisplay();
}

void eg3d_set_cone3d_color(int r,int g,int b) {
    default_cone3d_color[0]=r/255.0f; default_cone3d_color[1]=g/255.0f; default_cone3d_color[2]=b/255.0f;
}
void eg3d_set_cone3d_mode(const char* mode) {
    if (!mode) return;
    if (strcmp(mode,"SOLID")==0 || strcmp(mode,"S")==0) default_cone3d_solid = true;
    else if (strcmp(mode,"WIRE")==0 || strcmp(mode,"W")==0) default_cone3d_solid = false;
}
void eg3d_set_cone3d_caps(const char* choice) {
    if (!choice) return;
    if (strcmp(choice, "ON")==0 || strcmp(choice, "1")==0) default_cone3d_caps = true;
    else if (strcmp(choice,"OFF")==0 || strcmp(choice,"0")==0) default_cone3d_caps = false;
}
void eg3d_set_cone3d_quality(int slices, int stacks) {
    if(slices>2) default_cone3d_slices=slices;
    if(stacks>0) default_cone3d_stacks=stacks;
}
void eg3d_set_cone3d_line_width(float w) { default_cone3d_line_width = w; }
void eg3d_cone3d(float x,float y,float z, float r, float h) {
    Cone* temp = (Cone*)realloc(ConeS, (ConeCount+1)*sizeof(Cone));
    if (!temp) return;
    ConeS = temp;
    ConeS[ConeCount].x=x; ConeS[ConeCount].y=y; ConeS[ConeCount].z=z;
    ConeS[ConeCount].r=r; ConeS[ConeCount].h=h;
    memcpy(ConeS[ConeCount].color, default_cone3d_color, 3*sizeof(float));
    ConeS[ConeCount].solid = default_cone3d_solid; ConeS[ConeCount].caps = default_cone3d_caps;
    ConeS[ConeCount].slices = default_cone3d_slices; ConeS[ConeCount].stacks = default_cone3d_stacks;
    ConeS[ConeCount].line_width = default_cone3d_line_width;
    ConeS[ConeCount].group_id = current_group;
    ConeS[ConeCount].visible = true;
    ConeCount++; glutPostRedisplay();
}

void eg3d_set_torus3d_color(int r,int g,int b) {
    default_torus3d_color[0]=r/255.0f; default_torus3d_color[1]=g/255.0f; default_torus3d_color[2]=b/255.0f;
}
void eg3d_set_torus3d_mode(const char* mode) {
    if (!mode) return;
    if (strcmp(mode,"SOLID")==0 || strcmp(mode,"S")==0) default_torus3d_solid = true;
    else if (strcmp(mode,"WIRE")==0 || strcmp(mode,"W")==0) default_torus3d_solid = false;
}
void eg3d_set_torus3d_quality(int sides, int rings) {
    if(sides>2) default_torus3d_sides=sides;
    if(rings>2) default_torus3d_rings=rings;
}
void eg3d_set_torus3d_line_width(float w) { default_torus3d_line_width = w; }
void eg3d_torus3d(float x,float y,float z, float inner_r, float outer_r) {
    Torus* temp = (Torus*)realloc(TorusS, (TorusCount+1)*sizeof(Torus));
    if (!temp) return;
    TorusS = temp;
    TorusS[TorusCount].x=x; TorusS[TorusCount].y=y; TorusS[TorusCount].z=z;
    TorusS[TorusCount].inner_r = inner_r; TorusS[TorusCount].outer_r = outer_r;
    memcpy(TorusS[TorusCount].color, default_torus3d_color, 3*sizeof(float));
    TorusS[TorusCount].solid = default_torus3d_solid; TorusS[TorusCount].sides = default_torus3d_sides;
    TorusS[TorusCount].rings = default_torus3d_rings; TorusS[TorusCount].line_width = default_torus3d_line_width;
    TorusS[TorusCount].group_id = current_group;
    TorusS[TorusCount].visible = true;
    TorusCount++; glutPostRedisplay();
}

/* ───────── 2D примитивы ───────── */
void eg3d_set_triangle2d_color(int r, int g, int b) {
    default_triangle2d_color[0]=r/255.0f;
    default_triangle2d_color[1]=g/255.0f;
    default_triangle2d_color[2]=b/255.0f;
}
void eg3d_set_triangle2d_mode(const char* mode) {
    if (!mode) return;
    default_triangle2d_filled = (strcmp(mode,"SOLID")==0 || strcmp(mode,"S")==0);
}
void eg3d_set_triangle2d_line_width(float w) { default_triangle2d_lw = w; }
void eg3d_triangle2d(float x0,float y0,float z0,
                     float x1,float y1,float z1,
                     float x2,float y2,float z2) {
    Triangle2D* tmp = (Triangle2D*)realloc(Triangle2DS, (Triangle2DCount+1)*sizeof(Triangle2D));
    if (!tmp) return;
    Triangle2DS = tmp;
    Triangle2DS[Triangle2DCount] = (Triangle2D){
        x0,y0,z0, x1,y1,z1, x2,y2,z2,
        {default_triangle2d_color[0], default_triangle2d_color[1], default_triangle2d_color[2]},
        default_triangle2d_lw,
        default_triangle2d_filled,
        current_group, true
    };
    Triangle2DCount++;
    glutPostRedisplay();
}

void eg3d_set_polygon2d_color(int r, int g, int b) {
    default_polygon2d_color[0]=r/255.0f;
    default_polygon2d_color[1]=g/255.0f;
    default_polygon2d_color[2]=b/255.0f;
}
void eg3d_set_polygon2d_mode(const char* mode) {
    if (!mode) return;
    default_polygon2d_filled = (strcmp(mode,"SOLID")==0 || strcmp(mode,"S")==0);
}
void eg3d_set_polygon2d_line_width(float w) { default_polygon2d_lw = w; }
void eg3d_polygon2d(float* x, float* y, float* z, int n) {
    if (n < 3) return;
    Polygon2D* tmp = (Polygon2D*)realloc(Polygon2DS, (Polygon2DCount+1)*sizeof(Polygon2D));
    if (!tmp) return;
    Polygon2DS = tmp;
    Polygon2D* p = &Polygon2DS[Polygon2DCount];
    p->x = (float*)malloc(n*sizeof(float));
    p->y = (float*)malloc(n*sizeof(float));
    p->z = (float*)malloc(n*sizeof(float));
    memcpy(p->x, x, n*sizeof(float));
    memcpy(p->y, y, n*sizeof(float));
    memcpy(p->z, z, n*sizeof(float));
    p->vertex_count = n;
    memcpy(p->color, default_polygon2d_color, 3*sizeof(float));
    p->line_width = default_polygon2d_lw;
    p->filled     = default_polygon2d_filled;
    p->group_id   = current_group;
    p->visible    = true;
    Polygon2DCount++;
    glutPostRedisplay();
}

void eg3d_set_circle2d_color(int r, int g, int b) {
    default_circle2d_color[0]=r/255.0f;
    default_circle2d_color[1]=g/255.0f;
    default_circle2d_color[2]=b/255.0f;
}
void eg3d_set_circle2d_mode(const char* mode) {
    if (!mode) return;
    default_circle2d_filled = (strcmp(mode,"SOLID")==0 || strcmp(mode,"S")==0);
}
void eg3d_set_circle2d_segments(int s) { if(s>4) default_circle2d_segments=s; }
void eg3d_set_circle2d_line_width(float w) { default_circle2d_lw = w; }
void eg3d_set_circle2d_normal(float nx, float ny, float nz) {
    default_circle2d_normal[0]=nx;
    default_circle2d_normal[1]=ny;
    default_circle2d_normal[2]=nz;
}
void eg3d_circle2d(float cx, float cy, float cz, float r) {
    Circle2D* tmp = (Circle2D*)realloc(Circle2DS, (Circle2DCount+1)*sizeof(Circle2D));
    if (!tmp) return;
    Circle2DS = tmp;
    Circle2DS[Circle2DCount] = (Circle2D){
        cx, cy, cz, r,
        default_circle2d_segments,
        {default_circle2d_color[0], default_circle2d_color[1], default_circle2d_color[2]},
        default_circle2d_lw,
        default_circle2d_filled,
        default_circle2d_normal[0], default_circle2d_normal[1], default_circle2d_normal[2],
        current_group, true
    };
    Circle2DCount++;
    glutPostRedisplay();
}

void eg3d_set_ellipse2d_color(int r, int g, int b) {
    default_ellipse2d_color[0]=r/255.0f;
    default_ellipse2d_color[1]=g/255.0f;
    default_ellipse2d_color[2]=b/255.0f;
}
void eg3d_set_ellipse2d_mode(const char* mode) {
    if (!mode) return;
    default_ellipse2d_filled = (strcmp(mode,"SOLID")==0 || strcmp(mode,"S")==0);
}
void eg3d_set_ellipse2d_segments(int s) { if(s>4) default_ellipse2d_segments=s; }
void eg3d_set_ellipse2d_line_width(float w) { default_ellipse2d_lw = w; }
void eg3d_set_ellipse2d_normal(float nx, float ny, float nz) {
    default_ellipse2d_normal[0]=nx;
    default_ellipse2d_normal[1]=ny;
    default_ellipse2d_normal[2]=nz;
}
void eg3d_set_ellipse2d_angle(float deg) { default_ellipse2d_angle = deg; }
void eg3d_ellipse2d(float cx, float cy, float cz, float rx, float ry) {
    Ellipse2D* tmp = (Ellipse2D*)realloc(Ellipse2DS, (Ellipse2DCount+1)*sizeof(Ellipse2D));
    if (!tmp) return;
    Ellipse2DS = tmp;
    Ellipse2DS[Ellipse2DCount] = (Ellipse2D){
        cx, cy, cz, rx, ry,
        default_ellipse2d_angle,
        default_ellipse2d_segments,
        {default_ellipse2d_color[0], default_ellipse2d_color[1], default_ellipse2d_color[2]},
        default_ellipse2d_lw,
        default_ellipse2d_filled,
        default_ellipse2d_normal[0], default_ellipse2d_normal[1], default_ellipse2d_normal[2],
        current_group, true
    };
    Ellipse2DCount++;
    glutPostRedisplay();
}

/* ───────── Завершение ───────── */
void eg3d_cleanup_all(void) {
    if(PointS) { free(PointS); PointS=NULL; PointCount=0; }
    if(LineS) { free(LineS); LineS=NULL; LineCount=0; }
    for(int i=0;i<PolyLineCount;i++) {
        if(PolyLineS[i].x) free(PolyLineS[i].x);
        if(PolyLineS[i].y) free(PolyLineS[i].y);
        if(PolyLineS[i].z) free(PolyLineS[i].z);
    }
    if(PolyLineS) { free(PolyLineS); PolyLineS=NULL; PolyLineCount=0; }
    if(ArrowS) { free(ArrowS); ArrowS=NULL; ArrowCount=0; }
    if(SphereS) { free(SphereS); SphereS=NULL; SphereCount=0; }
    if(CubeS) { free(CubeS); CubeS=NULL; CubeCount=0; }
    if(CylinderS) { free(CylinderS); CylinderS=NULL; CylinderCount=0; }
    if(ConeS) { free(ConeS); ConeS=NULL; ConeCount=0; }
    if(TorusS) { free(TorusS); TorusS=NULL; TorusCount=0; }
    for(int i=0;i<OutTextCount;i++) { if(OutTextS[i].text) free(OutTextS[i].text); }
    if(OutTextS) { free(OutTextS); OutTextS=NULL; OutTextCount=0; }

    if(Triangle2DS) { free(Triangle2DS); Triangle2DS=NULL; Triangle2DCount=0; }
    for(int i=0;i<Polygon2DCount;i++) {
        if(Polygon2DS[i].x) free(Polygon2DS[i].x);
        if(Polygon2DS[i].y) free(Polygon2DS[i].y);
        if(Polygon2DS[i].z) free(Polygon2DS[i].z);
    }
    if(Polygon2DS) { free(Polygon2DS); Polygon2DS=NULL; Polygon2DCount=0; }
    if(Circle2DS) { free(Circle2DS); Circle2DS=NULL; Circle2DCount=0; }
    if(Ellipse2DS) { free(Ellipse2DS); Ellipse2DS=NULL; Ellipse2DCount=0; }

    if(g_quadric) { gluDeleteQuadric(g_quadric); g_quadric = NULL; }
    groups_count = 0; current_group = 0; active_view_group = 0;
}

void eg3d_show3d(void) {
    glutMainLoop();
}
