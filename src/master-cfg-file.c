#include <pwd.h>
#include <unistd.h>
#include <string.h>
#include "master-cfg-file.h"

/* The location to place the main configuration directory */
void get_config_dir_loc(char *s) {
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    strncpy(s, homedir, 501);
    strcat(s, "/");
}
