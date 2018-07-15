# Timezones
Timezone related functions to get the correct settings of the tzData used by Mongoose-OS.
Inspred and based on a Blog posting from Pavel Gurenko ["Getting POSIX TZ strings from Olson tzdata"](http://www.pavelgurenko.com/2017/05/getting-posix-tz-strings-from-olson.html)

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
TZINFO.getFileName(path, name)
```	
* Helper function which gets the full filename build from the filesystem and the group.
1. Parameter:
	* string path - the path (aka filesystem) where the data is/will be located
	* string name - the group name describing the first level (mostly continent) of the timezone
2. Return value
	* the filename as string
```javascript
TZINFO.moveData(target, doForce)
```	
* Moves all datafiles defined in the _dataFiles object, whic are coming with the library to another filesystem.
1. Parameter:
	* string target - name of the filesystem holding the data after the call (must be mounted)
	* boolean doForce - if true, the data will be moved even if it exists already in the target filesystem
2. Return value
	* if successfully moved, the result is boolean true, false if not
```javascript
TZINFO.moveFile(srcName, target)
```	
* Helper function moves one single file to the target.
1. Parameter:
	* string srcName - the group of teh data file to move (mostly continent name)
	* string target - name of the filesystem holding the data after the call (must be mounted)
2. Return value
	* the target filename as string id successfull, else an empty string
		
/**
 * This will set the converted timezone in POSIX format in the system configuration
 * internal function
 * Parameter: String tzSpecStr - the result of the conversion called before
 **/
TZINFO.loadData(group)

/**
 * This will set the converted timezone in POSIX format in the system configuration
 * internal function
 * Parameter: String tzSpecStr - the result of the conversion called before
 **/
TZINFO.convertOlsonToPosix(name, doConf, doForce)

/**
 * This will set the converted timezone in POSIX format in the system configuration
 * internal function
 * Parameter: String tzSpecStr - the result of the conversion called before
 **/
TZINFO.setTimeZoneInConf(tzSpecStr)
```
