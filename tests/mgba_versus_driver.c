// One emulated GBA. Raw host key events pass through upstream mGBA's actual
// InputContext and the shipped Lua adapter. Only the adapter's input mailbox is
// writable. Combat state is observed through read-only telemetry.
#include <mgba/core/core.h>
#include <mgba/core/config.h>
#include <mgba/core/log.h>
#include <mgba/core/scripting.h>
#include <mgba/core/version.h>
#include <mgba/script.h>
#include <mgba-util/audio-buffer.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static struct mScriptContext scripting;
static bool attached = false;
static unsigned held[2];
static struct mCore* core;
static const char* adapter;
static mColor video[240 * 160];

static const unsigned host_keys[2][10] = {
    {'j', 'k', 'o', mSCRIPT_KEY_ENTER, mSCRIPT_KEY_RIGHT, mSCRIPT_KEY_LEFT,
     mSCRIPT_KEY_UP, mSCRIPT_KEY_DOWN, 'i', 'u'},
    {'f', 'g', 0, 't', 'd', 'a', 'w', 's', 'e', 'q'}
};

static void quiet(struct mLogger* logger, int category, enum mLogLevel level,
                  const char* format, va_list arguments) {
    (void) logger;
    (void) category;
    if (level == mLOG_ERROR || level == mLOG_FATAL) {
        vfprintf(stderr, format, arguments);
        fputc('\n', stderr);
    }
}

static void detach_adapter(void) {
    if (!attached) return;
    mScriptContextDetachCore(&scripting);
    mScriptContextDeinit(&scripting);
    attached = false;
    held[0] = held[1] = 0;
}

static void attach_adapter(void) {
    detach_adapter();
    mScriptContextInit(&scripting);
    mScriptContextRegisterEngines(&scripting);
    mScriptContextAttachStdlib(&scripting);
    mScriptContextAttachLogger(&scripting, NULL);
    mScriptContextAttachInput(&scripting);
    mScriptContextAttachCore(&scripting, core);
    attached = true;
    if (!mScriptContextLoadFile(&scripting, adapter)) {
        fprintf(stderr, "Unable to load Local Versus Lua adapter\n");
        exit(5);
    }
}

static void send_keys(int player, unsigned mask) {
    if (!attached) return;
    for (unsigned bit = 0; bit < 10; ++bit) {
        if (!host_keys[player][bit] || !((held[player] ^ mask) & (1u << bit))) continue;
        struct mScriptKeyEvent event = {
            .d = {.type = mSCRIPT_EV_TYPE_KEY},
            .key = host_keys[player][bit],
            .state = mask & (1u << bit) ? mSCRIPT_INPUT_STATE_DOWN : mSCRIPT_INPUT_STATE_UP
        };
        mScriptContextFireEvent(&scripting, &event.d);
    }
    held[player] = mask;
}

static void run_frame(void) {
    core->runFrame(core);
    mAudioBufferClear(core->getAudioBuffer(core));
}

static void capture(const char* path) {
    FILE* file = fopen(path, "wb");
    if (!file) exit(6);
    fprintf(file, "P6\n240 160\n255\n");
    for (unsigned pixel = 0; pixel < 240 * 160; ++pixel) {
        uint32_t color = video[pixel];
        fputc(color & 255, file);
        fputc((color >> 8) & 255, file);
        fputc((color >> 16) & 255, file);
    }
    fclose(file);
}

int main(int argc, char** argv) {
    if (argc < 3) return 2;
    struct mLogger logger = {.log = quiet};
    mLogSetDefaultLogger(&logger);
    core = mCoreFind(argv[1]);
    if (!core || !core->init(core)) return 3;
    mCoreConfigInit(&core->config, "pkmn-local-versus-test");
    mCoreConfigSetDefaultIntValue(&core->config, "skipBios", 1);
    mCoreConfigSetDefaultIntValue(&core->config, "useBios", 0);
    mCoreConfigSetDefaultIntValue(&core->config, "volume", 256);
    mCoreConfigSetDefaultIntValue(&core->config, "sampleRate", 32768);
    mCoreLoadConfig(core);
    core->setVideoBuffer(core, video, 240);
    core->setAudioBufferSize(core, 1024);
    if (!mCoreLoadFile(core, argv[1])) return 4;
    if (argc > 3 && !mCoreLoadSaveFile(core, argv[3], true)) return 4;
    core->reset(core);
    adapter = argv[2];
    attach_adapter();

    char operation[16], path[512];
    unsigned frames, player1, player2, address;
    while (scanf("%15s", operation) == 1) {
        if (operation[0] == 'i') {
            printf("mgba %s %s\n", gitCommit, projectVersion);
        } else if (operation[0] == 'f') {
            if (scanf("%u %u %u", &frames, &player1, &player2) != 3) return 7;
            core->setKeys(core, 0);
            send_keys(0, player1);
            send_keys(1, player2);
            for (unsigned frame = 0; frame < frames; ++frame) run_frame();
            printf("frames %u\n", core->frameCounter(core));
        } else if (operation[0] == 'h') {
            if (scanf("%u %u", &frames, &player1) != 2) return 7;
            core->setKeys(core, player1);
            for (unsigned frame = 0; frame < frames; ++frame) run_frame();
            puts("hardware");
        } else if (operation[0] == 's') {
            if (scanf("%x", &address) != 1) return 7;
            printf("state");
            for (unsigned word = 0; word < 32; ++word) {
                printf(" %u", core->busRead32(core, address + word * 4));
            }
            putchar('\n');
        } else if (operation[0] == 'd') {
            if (scanf("%511s", path) != 1) return 7;
            capture(path);
            puts("captured");
        } else if (operation[0] == 'x') {
            detach_adapter();
            puts("detached");
        } else if (operation[0] == 'c') {
            attach_adapter();
            puts("connected");
        } else if (operation[0] == 'l') {
            if (!mScriptContextLoadFile(&scripting, adapter)) return 5;
            puts("reloaded");
        } else if (operation[0] == 'q') {
            break;
        } else {
            return 7;
        }
        fflush(stdout);
    }
    detach_adapter();
    unsigned char sram[32768];
    if (argc > 3) {
        for (unsigned offset = 0; offset < sizeof(sram); ++offset) {
            sram[offset] = core->busRead8(core, 0x0e000000 + offset);
        }
    }
    core->deinit(core);
    if (argc > 3) {
        FILE* file = fopen(argv[3], "wb");
        if (!file) return 8;
        fwrite(sram, 1, sizeof(sram), file);
        fclose(file);
    }
    return 0;
}
