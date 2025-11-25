#include <gccore.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Checkerboard pattern texture
#include "checkerboard_tpl.h"

#define DEFAULT_FIFO_SIZE (256 * 1024)

static void* frameBuffer[2] = {NULL, NULL};
GXRModeObj* rmode;

GXTexObj checkerboard_texture;

void load_tpl(GXTexObj* tex, const u8* data, u32 len) {
    TPLFile file;
    TPL_OpenTPLFromMemory(&file, (void*)data, len);
    TPL_GetTexture(&file, 0, tex);
}

void draw_quad(Mtx view, guVector position) {
    Mtx model, modelview;

    guMtxIdentity(model);
    guMtxTransApply(model, model, position.x, position.y, position.z);
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

    GX_Position3f32(-1.0f, 1.0f, 0.0f);
    GX_TexCoord2f32(0.0, 0.0);

    GX_Position3f32(1.0f, 1.0f, 0.0f);
    GX_TexCoord2f32(1.0, 0.0);

    GX_Position3f32(1.0f, -1.0f, 0.0f);
    GX_TexCoord2f32(1.0, 1.0);

    GX_Position3f32(-1.0f, -1.0f, 0.0f);
    GX_TexCoord2f32(0.0, 1.0);

    GX_End();
}

int main(int argc, char** argv) {
    f32 yscale;

    u32 xfbHeight;

    Mtx view;
    Mtx44 perspective;

    u32 fb = 0; // initial framebuffer index

    // init the vi.
    VIDEO_Init();

    rmode = VIDEO_GetPreferredMode(NULL);
    PAD_Init();

    // allocate 2 framebuffers for double buffering
    frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(frameBuffer[fb]);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

    // setup the fifo and then init the flipper
    void* gp_fifo = NULL;
    gp_fifo = memalign(32, DEFAULT_FIFO_SIZE);
    memset(gp_fifo, 0, DEFAULT_FIFO_SIZE);

    GX_Init(gp_fifo, DEFAULT_FIFO_SIZE);

    // other gx setup
    GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
    yscale = GX_GetYScaleFactor(rmode->efbHeight, rmode->xfbHeight);
    xfbHeight = GX_SetDispCopyYScale(yscale);
    GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
    GX_SetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
    GX_SetDispCopyDst(rmode->fbWidth, xfbHeight);
    GX_SetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
    GX_SetFieldMode(
        rmode->field_rendering,
        ((rmode->viHeight == 2 * rmode->xfbHeight) ? GX_ENABLE : GX_DISABLE)
    );

    GXColor background = {0, 0, 0, 0xFF};
    GX_SetCopyClear(background, 0x00ffffff);

    GX_SetCullMode(GX_CULL_NONE);
    GX_CopyDisp(frameBuffer[fb], GX_FALSE);
    GX_SetDispCopyGamma(GX_GM_1_0);

    // setup the vertex descriptor
    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

    // setup the vertex attribute table
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    // load textures
    load_tpl(&checkerboard_texture, checkerboard_tpl, checkerboard_tpl_size);
    GX_LoadTexObj(&checkerboard_texture, GX_TEXMAP0);

    // setup texgen
    GX_SetTexCoordGen2(
        GX_TEXCOORD0, // tex coord 0
        GX_TG_MTX2x4, // will be computed from a 2x4 matrix mul
        GX_TG_TEX0,   // with source tex coord 0
        GX_IDENTITY,  // matrix will be identity
        GX_FALSE,     // no normalization will happen
        GX_DTTMTX0    // and post transformed with dual matrix 0
    );

    // setup tevs
    GX_SetNumChans(1);
    GX_SetNumTexGens(1);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);

    // set efb fmt
    GX_SetPixelFmt(GX_PF_RGBA6_Z24, GX_ZC_LINEAR);

    // setup our camera at the origin
    // looking down the -z axis with y up
    guVector cam = {0.0F, 0.0F, 0.0F}, up = {0.0F, 1.0F, 0.0F},
             look = {0.0F, 0.0F, -1.0F};
    guLookAt(view, &cam, &up, &look);

    // setup our projection matrix
    // this creates a perspective matrix with a view angle of 90,
    // and aspect ratio based on the display resolution
    f32 w = rmode->viWidth;
    f32 h = rmode->viHeight;
    guPerspective(perspective, 45, (f32)w / h, 0.1F, 300.0F);
    GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);

    SYS_STDIO_Report(true);

    f32 counter = 0.0;
    while (1) {
        // draw
        GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);

        // load post transform matrix
        // hardware cannot store 1.0, use 0.5 and set scale to 1 instead
        f32 matrix[3][4] = {
            {(cos(counter) + 1) / 2, (sin(counter) + 1) / 2, 0.0, 0.0},
            {(sin(counter) + 1) / 2, (cos(counter) + 1) / 2, 0.0, 0.0},
            {0.0, 0.0, 1.0, 0.0},
        };
        GX_LoadTexMtxImm(matrix, GX_DTTMTX0, GX_MTX3x4);

        guVector position;
        position.x = 0.0;
        position.y = 0.0;
        position.z = -3;
        draw_quad(view, position);

        GX_DrawDone();

        // finish
        fb ^= 1;
        GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
        GX_SetColorUpdate(GX_TRUE);
        GX_CopyDisp(frameBuffer[fb], GX_TRUE);

        VIDEO_SetNextFramebuffer(frameBuffer[fb]);
        VIDEO_Flush();
        VIDEO_WaitVSync();

        counter += 0.05;
    }

    return 0;
}
