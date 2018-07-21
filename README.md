# Timezones
Timezone related functions to get the correct settings of the tzData used by Mongoose-OS.
Inspired and based on a blog posting from Pavel Gurenko ["Getting POSIX TZ strings from Olson tzdata"](http://www.pavelgurenko.com/2017/05/getting-posix-tz-strings-from-olson.html). The library will only use the current timezone settings and is not able to track changes in timezone data.

This is a library written in C which works in the background. Just add the lib, set your personal values in `mos.yml` and build the app.

**the setting 'Europe/Berlin' will result in 'CET-1CEST,M3.5.0,M10.5.0/3'**

# Usage:

## 1. Include library in `mos.yml`

## 2. Adapt you settings:
```YAML
  - ["timezone.enable", true]
```
You may enable/disable the library though it is loaded.

```YAML
  - ["timezone.data_path", "/mnt"]
```
If you have mounted an additional file system with e.g. the name `/mnt`, you may move the ZIP file (~5KB) to the file system you entered here. It must by mounted before. This will only be executed after flashing a new version of the firmware. After this action the moved file will be used. Be aware that after moving the archive file you should not change this setting without a new build and flash, because this will lead to an error finding the file. 
```YAML
  - ["timezone.arch_file", "tz-archive.zip"]
```
This is the filename of the timezone data archive, **should not be touched**.
```YAML
  - ["timezone.olson", "UTC0"]

```
Here you may enter the wished timezone in Olson notation you wish to be configured for your device.
For example: if you enter `America/New_York`, the `sys.tz_spec` will be set to `EST5EDT,M3.2.0,M11.1.0`
	
## 3. ToDo:
* comment the source code
* add update possibilities, because the timezone data changes from time to time
* implement some functions in MJS for information purposes
* implement a function which will retrieve the timezone from coordinates (longitude, latitude)
* implement a function which will get the timezone from a full address (e.g. via GoogleAPI)
