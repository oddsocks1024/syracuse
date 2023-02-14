/*
 * Arculator 2.1 by Sarah Walker
 * Syracuse 2.2 modified from Arculator by Ian Chapman
 * Main Function
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
}

extern char cmosdir[PATH_MAX];

int main(int argc, char **argv) {
    char configdir_loc[PATH_MAX];
    struct stat st = {0};
    XInitThreads();
    al_init_main(0, NULL);
    get_config_dir_loc(configdir_loc);
    snprintf(configdir, sizeof(configdir), "%s%s", configdir_loc, MACHINE_CFG_DIRNAME);
    snprintf(cmosdir, sizeof(cmosdir), "%s%scmos/", configdir_loc, MACHINE_CFG_DIRNAME);

    if (stat(configdir, &st) == DOES_NOT_EXIST) {
        mkdir(configdir, 0755);
    }

    if (stat(cmosdir, &st) == DOES_NOT_EXIST) {
        mkdir(cmosdir, 0755);
    }

    podule_build_list();
    opendlls();
    SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");
    wxApp::SetInstance(new App());
    wxEntry(argc, argv);
    return 0;
}
