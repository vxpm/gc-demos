#include <gccore.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_FIFO_SIZE (256 * 1024)

static GXRModeObj* rmode = NULL;
static void* frameBuffer[2] = {NULL, NULL};

void draw_textured_cube() {
    GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    GX_SetTevColorIn(
        GX_TEVSTAGE0, GX_CC_RASC, GX_CC_TEXC, GX_CC_HALF, GX_CC_ZERO
    );
    GX_SetTevColorOp(
        GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV
    );
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

    GX_Begin(GX_QUADS, GX_VTXFMT0, 24);

    GX_Position3f32(-1.0f, 1.0f, 1.0f); // Top Left of the quad (top)
    GX_Color3f32(1.0f, 0.0f, 0.0f);     // Set The Color To Red
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(1.0f, 1.0f, 1.0f); // Top Right of the quad (top)
    GX_Color3f32(1.0f, 0.0f, 0.0f);    // Set The Color To Red
    GX_TexCoord2f32(1.0, 0.0);
    GX_Position3f32(1.0f, 1.0f, -1.0f); // Bottom Right of the quad (top)
    GX_Color3f32(1.0f, 0.0f, 0.0f);     // Set The Color To Red
    GX_TexCoord2f32(1.0, 1.0);
    GX_Position3f32(-1.0f, 1.0f, -1.0f); // Bottom Left of the quad (top)
    GX_Color3f32(1.0f, 0.0f, 0.0f);      // Set The Color To Red
    GX_TexCoord2f32(0.0, 1.0);

    GX_Position3f32(-1.0f, -1.0f, 1.0f); // Top Left of the quad (bottom)
    GX_Color3f32(0.0f, 1.0f, 0.0f);      // Set The Color To Green
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(1.0f, -1.0f, 1.0f); // Top Right of the quad (bottom)
    GX_Color3f32(0.0f, 1.0f, 0.0f);     // Set The Color To Green
    GX_TexCoord2f32(1.0, 0.0);
    GX_Position3f32(1.0f, -1.0f, -1.0f); // Bottom Right of the quad (bottom)
    GX_Color3f32(0.0f, 1.0f, 0.0f);      // Set The Color To Green
    GX_TexCoord2f32(1.0, 1.0);
    GX_Position3f32(-1.0f, -1.0f, -1.0f); // Bottom Left of the quad (bottom)
    GX_Color3f32(0.0f, 1.0f, 0.0f);       // Set The Color To Green
    GX_TexCoord2f32(0.0, 1.0);

    GX_Position3f32(-1.0f, 1.0f, 1.0f); // Top Left of the quad (front)
    GX_Color3f32(0.0f, 0.0f, 1.0f);     // Set The Color To Blue
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(-1.0f, -1.0f, 1.0f); // Top Right of the quad (front)
    GX_Color3f32(0.0f, 0.0f, 1.0f);      // Set The Color To Blue
    GX_TexCoord2f32(1.0, 0.0);
    GX_Position3f32(1.0f, -1.0f, 1.0f); // Bottom Right of the quad (front)
    GX_Color3f32(0.0f, 0.0f, 1.0f);     // Set The Color To Blue
    GX_TexCoord2f32(1.0, 1.0);
    GX_Position3f32(1.0f, 1.0f, 1.0f); // Bottom Left of the quad (front)
    GX_Color3f32(0.0f, 0.0f, 1.0f);    // Set The Color To Blue
    GX_TexCoord2f32(0.0, 1.0);

    GX_Position3f32(-1.0f, 1.0f, -1.0f); // Top Left of the quad (back)
    GX_Color3f32(1.0f, 1.0f, 0.0f);      // Set The Color To Yellow
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(-1.0f, -1.0f, -1.0f); // Top Right of the quad (back)
    GX_Color3f32(1.0f, 1.0f, 0.0f);       // Set The Color To Yellow
    GX_TexCoord2f32(1.0, 0.0);
    GX_Position3f32(1.0f, -1.0f, -1.0f); // Bottom Right of the quad (back)
    GX_Color3f32(1.0f, 1.0f, 0.0f);      // Set The Color To Yellow
    GX_TexCoord2f32(1.0, 1.0);
    GX_Position3f32(1.0f, 1.0f, -1.0f); // Bottom Left of the quad (back)
    GX_Color3f32(1.0f, 1.0f, 0.0f);     // Set The Color To Yellow
    GX_TexCoord2f32(0.0, 1.0);

    GX_Position3f32(-1.0f, 1.0f, 1.0f); // Top Left of the quad (left)
    GX_Color3f32(1.0f, 0.0f, 1.0f);     // Set The Color To Pink
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(-1.0f, 1.0f, -1.0f); // Top Right of the quad (back)
    GX_Color3f32(1.0f, 0.0f, 1.0f);      // Set The Color To Pink
    GX_TexCoord2f32(1.0, 0.0);
    GX_Position3f32(-1.0f, -1.0f, -1.0f); // Bottom Right of the quad (back)
    GX_Color3f32(1.0f, 0.0f, 1.0f);       // Set The Color To Pink
    GX_TexCoord2f32(1.0, 1.0);
    GX_Position3f32(-1.0f, -1.0f, 1.0f); // Bottom Left of the quad (back)
    GX_Color3f32(1.0f, 0.0f, 1.0f);      // Set The Color To Pink
    GX_TexCoord2f32(0.0, 1.0);

    GX_Position3f32(1.0f, 1.0f, 1.0f); // Top Left of the quad (right)
    GX_Color3f32(0.0f, 1.0f, 1.0f);    // Set The Color To Cyan
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(1.0f, 1.0f, -1.0f); // Top Right of the quad (right)
    GX_Color3f32(0.0f, 1.0f, 1.0f);     // Set The Color To Cyan
    GX_TexCoord2f32(1.0, 0.0);
    GX_Position3f32(1.0f, -1.0f, -1.0f); // Bottom Right of the quad (right)
    GX_Color3f32(0.0f, 1.0f, 1.0f);      // Set The Color To Cyan
    GX_TexCoord2f32(1.0, 1.0);
    GX_Position3f32(1.0f, -1.0f, 1.0f); // Bottom Left of the quad (right)
    GX_Color3f32(0.0f, 1.0f, 1.0f);     // Set The Color To Cyan
    GX_TexCoord2f32(0.0, 1.0);

    GX_End();
}

void draw_colored_cube() {
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);

    GX_Begin(GX_QUADS, GX_VTXFMT0, 24);

    GX_Position3f32(-1.0f, 1.0f, 1.0f); // Top Left of the quad (top)
    GX_Color3f32(1.0f, 0.0f, 0.0f);     // Set The Color To Red
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(1.0f, 1.0f, 1.0f); // Top Right of the quad (top)
    GX_Color3f32(1.0f, 0.0f, 0.0f);    // Set The Color To Red
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(1.0f, 1.0f, -1.0f); // Bottom Right of the quad (top)
    GX_Color3f32(1.0f, 0.0f, 0.0f);     // Set The Color To Red
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(-1.0f, 1.0f, -1.0f); // Bottom Left of the quad (top)
    GX_Color3f32(1.0f, 0.0f, 0.0f);      // Set The Color To Red
    GX_TexCoord2f32(0.0, 0.0);

    GX_Position3f32(-1.0f, -1.0f, 1.0f); // Top Left of the quad (bottom)
    GX_Color3f32(0.0f, 1.0f, 0.0f);      // Set The Color To Green
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(1.0f, -1.0f, 1.0f); // Top Right of the quad (bottom)
    GX_Color3f32(0.0f, 1.0f, 0.0f);     // Set The Color To Green
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(1.0f, -1.0f, -1.0f); // Bottom Right of the quad (bottom)
    GX_Color3f32(0.0f, 1.0f, 0.0f);      // Set The Color To Green
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(-1.0f, -1.0f, -1.0f); // Bottom Left of the quad (bottom)
    GX_Color3f32(0.0f, 1.0f, 0.0f);       // Set The Color To Green
    GX_TexCoord2f32(0.0, 0.0);

    GX_Position3f32(-1.0f, 1.0f, 1.0f); // Top Left of the quad (front)
    GX_Color3f32(0.0f, 0.0f, 1.0f);     // Set The Color To Blue
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(-1.0f, -1.0f, 1.0f); // Top Right of the quad (front)
    GX_Color3f32(0.0f, 0.0f, 1.0f);      // Set The Color To Blue
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(1.0f, -1.0f, 1.0f); // Bottom Right of the quad (front)
    GX_Color3f32(0.0f, 0.0f, 1.0f);     // Set The Color To Blue
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(1.0f, 1.0f, 1.0f); // Bottom Left of the quad (front)
    GX_Color3f32(0.0f, 0.0f, 1.0f);    // Set The Color To Blue
    GX_TexCoord2f32(0.0, 0.0);

    GX_Position3f32(-1.0f, 1.0f, -1.0f); // Top Left of the quad (back)
    GX_Color3f32(1.0f, 1.0f, 0.0f);      // Set The Color To Yellow
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(-1.0f, -1.0f, -1.0f); // Top Right of the quad (back)
    GX_Color3f32(1.0f, 1.0f, 0.0f);       // Set The Color To Yellow
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(1.0f, -1.0f, -1.0f); // Bottom Right of the quad (back)
    GX_Color3f32(1.0f, 1.0f, 0.0f);      // Set The Color To Yellow
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(1.0f, 1.0f, -1.0f); // Bottom Left of the quad (back)
    GX_Color3f32(1.0f, 1.0f, 0.0f);     // Set The Color To Yellow
    GX_TexCoord2f32(0.0, 0.0);

    GX_Position3f32(-1.0f, 1.0f, 1.0f); // Top Left of the quad (left)
    GX_Color3f32(1.0f, 0.0f, 1.0f);     // Set The Color To Pink
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(-1.0f, 1.0f, -1.0f); // Top Right of the quad (back)
    GX_Color3f32(1.0f, 0.0f, 1.0f);      // Set The Color To Pink
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(-1.0f, -1.0f, -1.0f); // Bottom Right of the quad (back)
    GX_Color3f32(1.0f, 0.0f, 1.0f);       // Set The Color To Pink
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(-1.0f, -1.0f, 1.0f); // Bottom Left of the quad (back)
    GX_Color3f32(1.0f, 0.0f, 1.0f);      // Set The Color To Pink
    GX_TexCoord2f32(0.0, 0.0);

    GX_Position3f32(1.0f, 1.0f, 1.0f); // Top Left of the quad (right)
    GX_Color3f32(0.0f, 1.0f, 1.0f);    // Set The Color To Cyan
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(1.0f, 1.0f, -1.0f); // Top Right of the quad (right)
    GX_Color3f32(0.0f, 1.0f, 1.0f);     // Set The Color To Cyan
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(1.0f, -1.0f, -1.0f); // Bottom Right of the quad (right)
    GX_Color3f32(0.0f, 1.0f, 1.0f);      // Set The Color To Cyan
    GX_TexCoord2f32(0.0, 0.0);
    GX_Position3f32(1.0f, -1.0f, 1.0f); // Bottom Left of the quad (right)
    GX_Color3f32(0.0f, 1.0f, 1.0f);     // Set The Color To Cyan
    GX_TexCoord2f32(0.0, 0.0);

    GX_End();
}

int main(int argc, char** argv) {
    f32 yscale, zt = 0;
    u32 xfbHeight;
    u32 fb = 0;
    u32 first_frame = 1;
    Mtx model, view, modelview;
    Mtx44 perspective;
    void* gpfifo = NULL;

    GXColor white_background = {0xff, 0xff, 0xff, 0xff};
    u32 zclear = 0x00ffffff;

    VIDEO_Init();
    PAD_Init();

    rmode = VIDEO_GetPreferredMode(NULL);

    // allocate the fifo buffer
    gpfifo = memalign(32, DEFAULT_FIFO_SIZE);
    memset(gpfifo, 0, DEFAULT_FIFO_SIZE);

    // allocate 2 framebuffers for double buffering
    frameBuffer[0] = SYS_AllocateFramebuffer(rmode);
    frameBuffer[1] = SYS_AllocateFramebuffer(rmode);

    // configure video
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(frameBuffer[fb]);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

    fb ^= 1;

    // init the flipper
    GX_Init(gpfifo, DEFAULT_FIFO_SIZE);

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

    if (rmode->aa) {
        GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
    } else {
        GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
    }

    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GX_SetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_SET);
    GX_SetColorUpdate(GX_TRUE);
    GX_SetAlphaUpdate(GX_TRUE);

    GX_SetCullMode(GX_CULL_NONE);
    GX_CopyDisp(frameBuffer[fb], GX_TRUE);
    GX_SetDispCopyGamma(GX_GM_1_0);

    // setup the vertex attribute table
    GX_InvVtxCache();
    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    // setup color channels
    GX_SetNumChans(1);
    GX_SetChanCtrl(
        GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, GX_LIGHTNULL,
        GX_DF_NONE, GX_AF_NONE
    );

    // setup texgen
    GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);

    GX_InvalidateTexAll();

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

    guVector axis_a = {1, 1, 1};
    guVector axis_b = {-1, 1, -1};

    u8* rendered_texture = aligned_alloc(32, 2 * 256 * 256);
    f32 rquad = 0.0f;
    while (1) {
        // 01. draw colored cube
        GX_SetViewport(0, 0, 256, 256, 0, 1);

        guMtxIdentity(model);
        guMtxRotAxisDeg(model, &axis_a, 10 * rquad);
        guMtxRotAxisDeg(model, &axis_b, 5 * rquad);
        guMtxTransApply(model, model, 0.0f, 0.0f, zt - 5.0f);
        guMtxConcat(view, model, modelview);
        GX_LoadPosMtxImm(modelview, GX_PNMTX0);

        draw_colored_cube();

        // 02. copy it as a texture to main memory
        GX_SetCopyClear(white_background, zclear);
        GX_SetTexCopySrc(0, 0, 256, 256);
        GX_SetTexCopyDst(256, 256, GX_TF_RGB565, GX_FALSE);
        GX_CopyTex(rendered_texture, GX_TRUE);

        // 03. load it as a texture
        GXTexObj rendered_texture_obj;
        GX_InitTexObj(
            &rendered_texture_obj, rendered_texture, 256, 256, GX_TF_RGB565,
            GX_REPEAT, GX_REPEAT, GX_FALSE
        );
        GX_LoadTexObj(&rendered_texture_obj, GX_TEXMAP0);

        // 04. draw the textured cube
        GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);

        guMtxIdentity(model);
        guMtxRotAxisDeg(model, &axis_a, rquad);
        guMtxTransApply(model, model, 0.0f, 0.0f, zt - 6.0f);
        guMtxConcat(view, model, modelview);
        GX_LoadPosMtxImm(modelview, GX_PNMTX0);
        draw_textured_cube();

        GX_SetCopyClear(white_background, zclear);
        GX_CopyDisp(frameBuffer[fb], GX_TRUE);

        GX_DrawDone();

        VIDEO_SetNextFramebuffer(frameBuffer[fb]);
        if (first_frame) {
            first_frame = 0;
            VIDEO_SetBlack(FALSE);
        }
        VIDEO_Flush();
        VIDEO_WaitVSync();
        fb ^= 1;

        rquad -= 0.25f;
    }
}
