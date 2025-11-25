
#ifndef CONFIG_H
#define CONFIG_H

#if defined __unix__
#   define DEFAULT_CONFIG_DIR  "%HOME%/.config/wleafcron"
#elif defined _WIN32
#   define DEFAULT_CONFIG_DIR  "%APPDATA%\\wleafcron"
#endif

#define CONFIG_FILENAME     "crontab"

#endif /* CONFIG_H */
/* end of file */
