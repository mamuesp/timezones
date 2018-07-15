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

- Description:

```JAVASCRIPT
// this is the object you may use
TZINFO = {...};

- some internal variable:

// _dataPath contains the filesystem where the timezone data is stored,
// if it si an empty string, the defsault filesystem ist used, if not,
// it contains the additional filesystem to use
TZINFO._dataPath: Cfg.get('timezone.data_path'),

// _dataGroups shows the names of the grouping of the data, mostly the
// The data is splitted into multiple files to save time when loading them
// because of the wished timezone its clear, where to look and so only the
// needed data is loaded.
TZINFO._dataGroups: ["Africa", "America", "Asia", "Europe", "Others"],

// _dataFiles holds the filenames of the stored timezone data
TZINFO._dataFiles: {
  Africa: 'africa-tz.min.json',
  America: 'america-tz.min.json',
  Asia: 'asia-tz.min.json',
  Europe: 'europe-tz.min.json',
  Others: 'others-tz.min.json'
},
	
/**
 * This will set the converted timezone in POSIX format in the system configuration
 * internal function
 * Parameter: String tzSpecStr - the result of the conversion called before
 **/
TZINFO.getFileName(path, name)
	
/**
 * This will set the converted timezone in POSIX format in the system configuration
 * internal function
 * Parameter: String tzSpecStr - the result of the conversion called before
 **/
TZINFO.moveData(target, doForce)

/**
 * This will set the converted timezone in POSIX format in the system configuration
 * internal function
 * Parameter: String tzSpecStr - the result of the conversion called before
 **/
TZINFO.moveFile(srcName, target)
	
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
