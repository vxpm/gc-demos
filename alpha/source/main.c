#include <gccore.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_FIFO_SIZE (256 * 1024)

static void* frameBuffer[2] = {NULL, NULL};
GXRModeObj* rmode;

void draw_quad(Mtx view, guVector position, GXColor color) {
    Mtx model, modelview;

    guMtxIdentity(model);
    guMtxTransApply(model, model, position.x, position.y, position.z);
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

    GX_Position3f32(-1.0f, 1.0f, 0.0f);
    GX_Color4u8(color.r, color.g, color.b, color.a);

    GX_Position3f32(1.0f, 1.0f, 0.0f);
    GX_Color4u8(color.r, color.g, color.b, color.a);

    GX_Position3f32(1.0f, -1.0f, 0.0f);
    GX_Color4u8(color.r, color.g, color.b, color.a);

    GX_Position3f32(-1.0f, -1.0f, 0.0f);
    GX_Color4u8(color.r, color.g, color.b, color.a);

    GX_End();
}

int main(int argc, char** argv) {
    f32 yscale;

    u32 xfbHeight;

    Mtx view;
    Mtx44 perspective;

    u32 fb = 0; // initial framebuffer index

    u8 blend_modes[] = {GX_BM_BLEND, GX_BM_SUBTRACT};
    u8 blend_factors[] = {GX_BL_ZERO,      GX_BL_ONE,        GX_BL_SRCCLR,
                          GX_BL_INVSRCCLR, GX_BL_SRCALPHA,   GX_BL_INVSRCALPHA,
                          GX_BL_DSTALPHA,  GX_BL_INVDSTALPHA};
    GXColor bg_colors[] = {
        {10, 255, 255, 255}, {10, 255, 255, 64}, {10, 30, 255, 255},
        {10, 30, 255, 64},   {255, 30, 10, 255}, {255, 30, 10, 64},
    };

    u8 blend_modes_count = sizeof(blend_modes) / sizeof(blend_modes[0]);
    u8 blend_factors_count = sizeof(blend_factors) / sizeof(blend_factors[0]);
    u8 bg_colors_count = sizeof(bg_colors) / sizeof(bg_colors[0]);

    u8 current_blend_mode = 0;
    u8 current_src_factor = 0;
    u8 current_dst_factor = 0;
    u8 current_bg_color = 0;

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

    GX_SetCullMode(GX_CULL_NONE);
    GX_CopyDisp(frameBuffer[fb], GX_FALSE);
    GX_SetDispCopyGamma(GX_GM_1_0);

    // setup the vertex descriptor
    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

    // setup the vertex attribute table
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

    // setup tevs
    GX_SetNumChans(1);
    GX_SetNumTexGens(0);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

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
        PAD_ScanPads();

        u16 buttons = PAD_ButtonsDown(0);
        if (buttons & PAD_BUTTON_X) {
            current_bg_color = (current_bg_color + 1) % bg_colors_count;
        }
        if (buttons & PAD_BUTTON_Y) {
            current_blend_mode = (current_blend_mode + 1) % blend_modes_count;
        }
        if (buttons & PAD_BUTTON_A) {
            current_src_factor = (current_src_factor + 1) % blend_factors_count;
        }
        if (buttons & PAD_BUTTON_B) {
            current_dst_factor = (current_dst_factor + 1) % blend_factors_count;
        }

        printf(
            "bg_color: %d, src_factor: %d, dst_factor: %d, blend_mode: %d\n",
            current_bg_color, current_src_factor, current_dst_factor,
            current_blend_mode
        );

        // draw
        GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
        guVector position;
        GXColor color;

        position.x = cos(counter);
        position.y = sin(counter);
        position.z = -6.0;
        color = (GXColor){.r = 255, .g = 0, .b = 0, .a = 128};
        draw_quad(view, position, color);

        position.x = cos(2 * counter);
        position.y = sin(0.5 * counter);
        position.z = -6.0;
        color = (GXColor){.r = 0, .g = 255, .b = 0, .a = 128};
        draw_quad(view, position, color);

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

        GX_SetCopyClear(bg_colors[current_bg_color], 0x00ffffff);
        GX_SetBlendMode(
            blend_modes[current_blend_mode], blend_factors[current_src_factor],
            blend_factors[current_dst_factor], GX_LO_NOOP
        );
    }

    return 0;
}
