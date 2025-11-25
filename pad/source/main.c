#include <gccore.h>
#include <malloc.h>
#include <ogcsys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void* xfb = NULL;
static GXRModeObj* rmode = NULL;

void* Initialise() {
    void* framebuffer;

    VIDEO_Init();
    PAD_Init();

    rmode = VIDEO_GetPreferredMode(NULL);

    framebuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    console_init(
        framebuffer, 20, 20, rmode->fbWidth, rmode->xfbHeight,
        rmode->fbWidth * VI_DISPLAY_PIX_SZ
    );

    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(framebuffer);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

    return framebuffer;
}

int main(int argc, char** argv) {
    int buttons[] = {
        PAD_BUTTON_A,    PAD_BUTTON_B,     PAD_BUTTON_X,    PAD_BUTTON_Y,
        PAD_BUTTON_DOWN, PAD_BUTTON_UP,    PAD_BUTTON_LEFT, PAD_BUTTON_RIGHT,
        PAD_BUTTON_MENU, PAD_BUTTON_START,
    };

    char* names[] = {
        "A", "B", "X", "Y", "Down", "Up", "Left", "Right", "Menu", "Start",
    };

    xfb = Initialise();
    printf("\nHello Pad!\n");

    u64 last_pressed = SYS_Time();
    u64 last_printed = SYS_Time();
    while (1) {
        VIDEO_WaitVSync();
        PAD_ScanPads();

        int buttonsDown = PAD_ButtonsDown(PAD_CHAN0);
        for (int i = 0; i < 10; i++) {
            if (buttonsDown & buttons[i]) {
                printf("Button %s pressed.\n", names[i]);
                PAD_ControlMotor(PAD_CHAN0, PAD_MOTOR_RUMBLE);
                last_pressed = SYS_Time();
            }
        }

        if (SYS_Time() >= last_pressed + 2000000) {
            PAD_ControlMotor(PAD_CHAN0, PAD_MOTOR_STOP);
        }

        if (SYS_Time() >= last_printed + 4000000) {
            s8 stick_x = PAD_StickX(PAD_CHAN0);
            s8 stick_y = PAD_StickY(PAD_CHAN0);
            s8 substick_x = PAD_SubStickX(PAD_CHAN0);
            s8 substick_y = PAD_SubStickY(PAD_CHAN0);
            u8 trigger_l = PAD_TriggerL(PAD_CHAN0);
            u8 trigger_r = PAD_TriggerR(PAD_CHAN0);

            printf(
                "X: %d  Y: %d  SUBX: %d  SUBY: %d  L: %d  R:  %d\n", stick_x,
                stick_y, substick_x, substick_y, trigger_l, trigger_r
            );

            last_printed = SYS_Time();
        }
    }

    return 0;
}
