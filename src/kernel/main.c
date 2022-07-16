#include "main.h"

#include "font.h"
#include "keyboard.h"
#include "math.h"
#include "mouse.h"
#include "print.h"
#include "screen.h"
#include "teapot.h"
#include "timer.h"
#include "types.h"


typedef struct {
    f32 x[3];
} v3f;

typedef struct {
    v3f p[3];
} tri;

typedef struct {
    const tri* tris;
    u32 cnt;
} mesh;


typedef struct {
    f32 m[4][4];
} mat44;


static v3f vec_scalar(f32 k) {
    return (v3f){{k, k, k}};
}
static void vec_iadd(v3f* a, const v3f* b) {
    a->x[0] += b->x[0];
    a->x[1] += b->x[1];
    a->x[2] += b->x[2];
}
static v3f vec_add(const v3f* a, const v3f* b) {
    v3f c = *a;
    vec_iadd(&c, b);
    return c;
}

static void vec_isub(v3f* a, const v3f* b) {
    a->x[0] -= b->x[0];
    a->x[1] -= b->x[1];
    a->x[2] -= b->x[2];
}
static v3f vec_sub(const v3f* a, const v3f* b) {
    v3f c = *a;
    vec_isub(&c, b);
    return c;
}

static void vec_imul(v3f* a, const v3f* b) {
    a->x[0] *= b->x[0];
    a->x[1] *= b->x[1];
    a->x[2] *= b->x[2];
}
static v3f vec_mul(const v3f* a, const v3f* b) {
    v3f c = *a;
    vec_imul(&c, b);
    return c;
}

static f32 vec_dot(const v3f* a, const v3f* b) {
    return a->x[0] * b->x[0] + a->x[1] * b->x[1] + a->x[2] * b->x[2];
}

static f32 vec_mag(const v3f* v) {
    return sqrt(vec_dot(v, v));
}

static void vec_inorm(v3f* v) {
    v3f vv = vec_scalar(1 / vec_mag(v));
    vec_imul(v, &vv);
}

static v3f vec_cross(const v3f* a, const v3f* b) {
    return (v3f){{
        a->x[1] * b->x[2] - a->x[2] * b->x[1],
        a->x[2] * b->x[0] - a->x[0] * b->x[2],
        a->x[0] * b->x[1] - a->x[1] * b->x[0],
    }};
}

static mat44 mat_ident() {
    mat44 mat = {};
    mat.m[0][0] = 1.0f;
    mat.m[1][1] = 1.0f;
    mat.m[2][2] = 1.0f;
    mat.m[3][3] = 1.0f;
    return mat;
}

static mat44 mat_rotx(f32 angle) {
    mat44 mat = {};
    mat.m[0][0] = 1.0f;
    mat.m[1][1] = cos(angle);
    mat.m[1][2] = sin(angle);
    mat.m[2][1] = -sin(angle);
    mat.m[2][2] = cos(angle);
    mat.m[3][3] = 1.0f;
    return mat;
}

static mat44 mat_roty(f32 angle) {
    mat44 mat = {};
    mat.m[0][0] = cos(angle);
    mat.m[0][2] = sin(angle);
    mat.m[2][0] = -sin(angle);
    mat.m[1][1] = 1.0f;
    mat.m[2][2] = cos(angle);
    mat.m[3][3] = 1.0f;
    return mat;
}

static mat44 mat_rotz(f32 angle) {
    mat44 mat = {};
    mat.m[0][0] = cos(angle);
    mat.m[0][1] = sin(angle);
    mat.m[1][0] = -sin(angle);
    mat.m[1][1] = cos(angle);
    mat.m[2][2] = 1.0f;
    mat.m[3][3] = 1.0f;
    return mat;
}

static mat44 mat_trans(f32 x, f32 y, f32 z) {
    mat44 mat = {};
    mat.m[0][0] = 1.0f;
    mat.m[1][1] = 1.0f;
    mat.m[2][2] = 1.0f;
    mat.m[3][3] = 1.0f;
    mat.m[3][0] = x;
    mat.m[3][1] = y;
    mat.m[3][2] = z;
    return mat;
}

static mat44 mat_proj(f32 fovDeg, f32 aspectRatio, f32 near, f32 far) {
    mat44 mat = {};
    f32 fovRad = 1.0f / tan(fovDeg * 0.5f / 180.0f * PI);
    mat.m[0][0] = aspectRatio * fovRad;
    mat.m[1][1] = fovRad;
    mat.m[2][2] = far / (far - near);
    mat.m[3][2] = (-far * near) / (far - near);
    mat.m[2][3] = 1.0f;
    mat.m[3][3] = 0.0f;
    return mat;
}

static mat44 mat_mul(const mat44* A, const mat44* B) {
    mat44 mat;
    for (u32 c = 0; c < 4; ++c)
        for (u32 r = 0; r < 4; ++r)
            mat.m[r][c] =
                A->m[r][0] * B->m[0][c] + A->m[r][1] * B->m[1][c] + A->m[r][2] * B->m[2][c] + A->m[r][3] * B->m[3][c];
    return mat;
}

static v3f vec_mat_mul(const v3f* in, const mat44* mat) {
    v3f out = {{
        in->x[0] * mat->m[0][0] + in->x[1] * mat->m[1][0] + in->x[2] * mat->m[2][0] + mat->m[3][0],
        in->x[0] * mat->m[0][1] + in->x[1] * mat->m[1][1] + in->x[2] * mat->m[2][1] + mat->m[3][1],
        in->x[0] * mat->m[0][2] + in->x[1] * mat->m[1][2] + in->x[2] * mat->m[2][2] + mat->m[3][2],
    }};
    f32 w = in->x[0] * mat->m[0][3] + in->x[1] * mat->m[1][3] + in->x[2] * mat->m[2][3] + mat->m[3][3];
    if (w > 0.01f) {
        out.x[0] /= w;
        out.x[1] /= w;
        out.x[2] /= w;
    }
    return out;
}

static mat44 mat_pointat(const v3f* pos, const v3f* target, const v3f* up) {
    v3f newFwd = vec_sub(target, pos);
    vec_inorm(&newFwd);

    v3f x = vec_scalar(vec_dot(up, &newFwd));
    v3f a = vec_mul(&newFwd, &x);
    v3f newUp = vec_sub(up, &a);
    vec_inorm(&newUp);

    v3f newRight = vec_cross(&newUp, &newFwd);

    mat44 mat = {{
        {newRight.x[0], newRight.x[1], newRight.x[2], 0.0f},
        {newUp.x[0], newUp.x[1], newUp.x[2], 0.0f},
        {newFwd.x[0], newFwd.x[1], newFwd.x[2], 0.0f},
        {pos->x[0], pos->x[1], pos->x[2], 1.0f},
    }};
    return mat;
}
static mat44 mat_quick_inv(const mat44* mat) {
    return (mat44){
        {{mat->m[0][0], mat->m[1][0], mat->m[2][0], 0.0f},
         {mat->m[0][1], mat->m[1][1], mat->m[2][1], 0.0f},
         {mat->m[0][2], mat->m[1][2], mat->m[2][2], 0.0f},
         {-(mat->m[3][0] * mat->m[0][0] + mat->m[3][1] * mat->m[0][1] + mat->m[3][2] * mat->m[0][2]),
          -(mat->m[3][0] * mat->m[1][0] + mat->m[3][1] * mat->m[1][1] + mat->m[3][2] * mat->m[1][2]),
          -(mat->m[3][0] * mat->m[2][0] + mat->m[3][1] * mat->m[2][1] + mat->m[3][2] * mat->m[2][2]), 1.0f}}};
}

mesh meshCube;
mat44 matProj;

v3f cameraPos;
f32 cameraYaw = 0.0f;
f32 cameraPitch = 0.0f;

#define NUM_TRIS (TEAPOT_COUNT / 3)
static void update(u32 dt);
void kmain(void) {
    tri tris[NUM_TRIS];
    for (u32 i = 0; i < NUM_TRIS; i += 3) {
        v3f a = {{teapot[(i + 0) * 3 + 0], teapot[(i + 0) * 3 + 1], teapot[(i + 0) * 3 + 2]}};
        v3f b = {{teapot[(i + 1) * 3 + 0], teapot[(i + 1) * 3 + 1], teapot[(i + 1) * 3 + 2]}};
        v3f c = {{teapot[(i + 2) * 3 + 0], teapot[(i + 2) * 3 + 1], teapot[(i + 2) * 3 + 2]}};
        tris[i] = (tri){{a, b, c}};
    }
    meshCube.cnt = NUM_TRIS;
    meshCube.tris = tris;

    matProj = mat_proj(90.0f, (f32) SCREEN_HEIGHT / SCREEN_WIDTH, 0.1f, 1000.0f);

    u32 lastTick = 0;
    while (1) {
        // TODO: throttling
        u32 currTick = g_tick;
        update(currTick - lastTick);
        lastTick = currTick;
    }
}

static void update(u32 dt) {
    static u32 o = 0;
    static f32 angle = 0.0f;
    o += dt;

    // screen_fill(0x00);
    for (u32 i = 0; i < SCREEN_HEIGHT; ++i) {
        u32 y = (i + o / 10) % SCREEN_HEIGHT;
        for (u32 j = 0; j < SCREEN_WIDTH; ++j) {
            screen_set_px(j, y, i & 0xFF);
        }
    }
    u16 x = 0, y = 0;
    print_num(&x, &y, dt);
    print_cstr(&x, &y, "ms");
    y += 16, x = 0;

    f32 timestep = dt * 0.01f;


    i32 mdx = g_mx - SCREEN_WIDTH / 2;
    i32 mdy = g_my - SCREEN_HEIGHT / 2;
    g_mx = SCREEN_WIDTH / 2;
    g_my = SCREEN_HEIGHT / 2;
    u8 kw = g_kbchars['w'];
    u8 ks = g_kbchars['s'];
    u8 ka = g_kbchars['a'];
    u8 kd = g_kbchars['d'];
    u8 kspc = g_kbchars[' '];
    u8 kv = g_kbchars['v'];
    cameraYaw -= mdx * 0.002f;
    cameraPitch -= mdy * 0.002f;


    angle += timestep;
    mat_ident();
    mat44 matObjRotX = mat_rotx(angle * 0.1f);
    mat44 matObjRotY = mat_roty(angle * 0.5f);
    mat44 matObjRotZ = mat_rotz(angle * 0.2f);
    mat44 matObjTrans = mat_trans(0, 0, 5.0f);
    mat44 matWorld;
    matWorld = mat_mul(&matObjRotX, &matObjRotY);
    matWorld = mat_mul(&matWorld, &matObjRotZ);
    matWorld = mat_mul(&matWorld, &matObjTrans);

    v3f up = {{0, 1, 0}};     // +y
    v3f target = {{0, 0, 1}}; // face +z
    mat44 matCamRotY = mat_roty(cameraYaw);
    mat44 matCamRotX = mat_rotx(cameraPitch);
    v3f cameraDir = vec_mat_mul(&target, &matCamRotX);
    cameraDir = vec_mat_mul(&cameraDir, &matCamRotY);
    target = vec_add(&cameraPos, &cameraDir);
    mat44 matCamera = mat_pointat(&cameraPos, &target, &up);
    mat44 matView = mat_quick_inv(&matCamera);

    v3f velocity = vec_scalar(timestep * 1.0f);
    v3f forward = vec_mul(&cameraDir, &velocity);
    v3f horizontal = vec_cross(&cameraDir, &up);
    vec_imul(&horizontal, &velocity);

    if (kw != ks) {
        if (kw)
            vec_iadd(&cameraPos, &forward);
        else
            vec_isub(&cameraPos, &forward);
    }
    if (kd != ka) {
        if (ka)
            vec_iadd(&cameraPos, &horizontal);
        else
            vec_isub(&cameraPos, &horizontal);
    }
    cameraPos.x[1] += ((i32) kv - (i32) kspc) * timestep;


    for (u32 i = 0; i < NUM_TRIS; ++i) {
        tri proj = meshCube.tris[i];

        proj = (tri){{
            vec_mat_mul(&proj.p[0], &matWorld),
            vec_mat_mul(&proj.p[1], &matWorld),
            vec_mat_mul(&proj.p[2], &matWorld),
        }};
        proj = (tri){{
            vec_mat_mul(&proj.p[0], &matView),
            vec_mat_mul(&proj.p[1], &matView),
            vec_mat_mul(&proj.p[2], &matView),
        }};
        proj = (tri){{
            vec_mat_mul(&proj.p[0], &matProj),
            vec_mat_mul(&proj.p[1], &matProj),
            vec_mat_mul(&proj.p[2], &matProj),
        }};
        v3f off = {{1.0f, 1.0f, 0.0f}};
        vec_iadd(&proj.p[0], &off);
        vec_iadd(&proj.p[1], &off);
        vec_iadd(&proj.p[2], &off);
        v3f scl = {{0.5f * SCREEN_WIDTH, 0.5f * SCREEN_HEIGHT, 1.0f}};
        vec_imul(&proj.p[0], &scl);
        vec_imul(&proj.p[1], &scl);
        vec_imul(&proj.p[2], &scl);

        screen_draw_line(proj.p[0].x[0], proj.p[0].x[1], proj.p[1].x[0], proj.p[1].x[1], 0xFF);
        screen_draw_line(proj.p[1].x[0], proj.p[1].x[1], proj.p[2].x[0], proj.p[2].x[1], 0xFF);
        screen_draw_line(proj.p[2].x[0], proj.p[2].x[1], proj.p[0].x[0], proj.p[0].x[1], 0xFF);
    }

    screen_swap_buffers();
}
