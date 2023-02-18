/*
    Syracuse 2.2 forked from Arculator which was written by Sarah Walker
    Main Function
*/
#include "wx-app.h"
#include <SDL2/SDL.h>
#include <wx/filename.h>
#include "wx-config_sel.h"
#include <sys/stat.h>

extern "C" {
    #include <X11/Xlib.h>
    #include "arc.h"
    #include "config.h"
    #include "podules.h"
    #include "soundopenal.h"
#include "master-cfg-file.h"

}

extern char cmosdir[PATH_MAX];
char configdir[PATH_MAX];

int main(int argc, char **argv) {
    char configdir_loc[PATH_MAX];
    char logdir[PATH_MAX];
    struct stat st = {0};
    XInitThreads();
    al_init_main(0, NULL);
    get_config_dir_loc(configdir_loc);
    snprintf(configdir, sizeof(configdir), "%s%s", configdir_loc, CFGDIR); // Ignore truncation warnings
    snprintf(cmosdir, sizeof(cmosdir), "%s%s", configdir_loc, CMOSDIR); // Ignore truncation warnings
    snprintf(logdir, sizeof(logdir), "%s%s", configdir_loc, LOGDIR); // Ignore truncation warnings

    if (stat(configdir, &st) == DOES_NOT_EXIST) {
        mkdir(configdir, 0755);
    }

    if (stat(cmosdir, &st) == DOES_NOT_EXIST) {
        mkdir(cmosdir, 0755);
    }

    if (stat(logdir, &st) == DOES_NOT_EXIST) {
        mkdir(logdir, 0755);
    }

#ifdef DEBUG_LOG
    printf("DEBUG MODE ENABLED\n");
#else
    printf("DEBUG MODE NOT ENABLED\n");
#endif

    podule_build_list();
    opendlls();
    SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");
    wxApp::SetInstance(new App());
    wxEntry(argc, argv);
    return 0;
}
