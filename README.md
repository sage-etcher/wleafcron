
# wleafcron

A cross-compatible cron daemon for Linux/BSD/Mac/Windows.

## Build

Dependencies:

- meson (build-time)
- c99 compiler+linker (build-time)

    meson setup build
    ninja -C build


## Usage

Configure the `crontab` file, by default `%APPDATA%\wleafcron\crontab` on 
windows and `%HOME%/.config/wleafcron/crontab` on other platforms.

crontab format:

    #+------------------ minute 0-59
    #|  +--------------- hour 0-23
    #|  |  +------------ day of month 1-31
    #|  |  |  +--------- month 1-12
    #|  |  |  |  +------ day of week 0-6 (sun,mon,tue,wed,thur,fri,sat)
    #|  |  |  |  |  +--- command
    #|  |  |  |  |  |
    09 15  *  *  *  echo "alert every day at 3:09pm"
    00 00  *  *  0  echo "alert every sundy at midnight"
     *  *  *  *  *  echo "alert on start and crontab update"

