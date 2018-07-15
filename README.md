# Timezones
Timezone related functions to get the correct settings of the tzData used by Mongoose-OS.
Inspired and based on a blog posting from Pavel Gurenko ["Getting POSIX TZ strings from Olson tzdata"](http://www.pavelgurenko.com/2017/05/getting-posix-tz-strings-from-olson.html)

This is mostly an MJS based library.

# Usage:

- Example:

```JAVASCRIPT
load('api_timezones.js');

// if you want to move the timezone data to another file system
TZINFO.moveData('/mnt');

// Convert your wished timezone to the POSIX format used by Mongoose-OS
// and write it directly to the system configuration
let tzSpec = TZINFO.convertOlsonToPosix('Europe/Dublin', true);

** input 'Europe/Berlin' will result in 'CET-1CEST,M3.5.0,M10.5.0/3' **
```

## Description:

- this is the object you may use
```javascript
TZINFO = {...};
```
### Internal variables:

* _dataPath contains the filesystem where the timezone data is stored, if it si an empty string, the defsault filesystem ist used, if not, it contains the additional filesystem to use
```javascript
TZINFO._dataPath: Cfg.get('timezone.data_path')
```
* _dataGroups shows the names of the grouping of the data, mostly th names of the continents. The data is splitted into multiple files to save time when loading them. As the wished timezone is knoen before loading, it is clear in whicth group to look and so only the needed data is loaded.
```javascript
TZINFO._dataGroups: ["Africa", "America", "Asia", "Europe", "Others"],
```
* _dataFiles is an object which holds the filenames of the stored timezone data
```javascript
TZINFO._dataFiles: {
  Africa: 'africa-tz.min.json',
  America: 'america-tz.min.json',
  Asia: 'asia-tz.min.json',
  Europe: 'europe-tz.min.json',
  Others: 'others-tz.min.json'
},
```
### The methods:
```javascript
TZINFO.convertOlsonToPosix(name, doConf, doForce)
```	
* Main method: gets the POSIX timezone code from an Olson named timezone (e.g. 'Europe/Dublin')
1. Parameter:
	* string name - the name of the timezone (e.g. 'Europe/Dublin')
	* boolean doCOnf - if true, teh successful loaded POSIX code will be written to the system configuration, key: "sys.tz_spec"
	* boolean doForce - overwrite the POSIX timezone code even if it already exists (not yet implemented)
2. Return value
	* the successfully retrieved POSIX timecode, 'UTC0' on failure
```javascript
TZINFO.getFileName(path, name)
```	
* Helper method: which gets the full filename set together from the filesystem and the group name.
1. Parameter:
	* string path - the path (aka filesystem) where the data is/will be located
	* string name - the group name describing the first level (mostly continent) of the timezone
2. Return value
	* the filename as string
```javascript
TZINFO.moveData(target, doForce)
```	
* Moves all datafiles defined in the _dataFiles object (which are coming along with the library) to another filesystem.
1. Parameter:
	* string target - name of the filesystem holding the data after the call (must be mounted)
	* boolean doForce - if true, the data will be moved even if it exists already in the target filesystem (not yet implemented)
2. Return value
	* if successfully moved, the result is boolean true, false if not
```javascript
TZINFO.moveFile(srcName, target)
```	
* Helper method: moves one data file to the target
1. Parameter:
	* string srcName - the group of the data file to move (mostly continent name)
	* string target - name of the filesystem holding the data after the call (must be mounted)
2. Return value
	* the target filename as string id successful, else an empty string
```javascript
TZINFO.loadData(group)
```	
* Helper method: set the converted timezone in POSIX format in the system configuration
1. Parameter: String tzSpecStr - the result of the conversion (POSIX timezone code) called before
2. Return value
	* none
```javascript
TZINFO.setTimeZoneInConf(tzSpecStr)
```	
### ToDo:
* implement some functions in C
* implement a function which will retrieve the timezone from coordinates (longitude, latitude)
* implement a function which will get the timezone from a full address (e.g. via GoogleAPI)
