# timezones
Timezone related functions to get the correct settings of the tzData used by Mongoose-OS.
Base on a Blog posting from Pavel Gurenko ["Getting POSIX TZ strings from Olson tzdata"](http://www.pavelgurenko.com/2017/05/getting-posix-tz-strings-from-olson.html)

This is mostly an MJS based library.

#Usage:

```JAVASCRIPT
load('api_timezones.js');

// if you want to move the timezone data to another file system
TZINFO.moveData('/mnt');

// Convert your wished timezone to the POSIX format used by Mongoose-OS
// and write it directly to the system configuration
let tzSpec = TZINFO.convertOlsonToPosix('Europe/Dublin', true);

TZINFO = {...};

TZINFO._dataPath: Cfg.get('timezone.data_path'),
TZINFO._dataGroups: ["Africa", "America", "Asia", "Europe", "Others"],

	// 
TZINFO._dataFiles: {
		Africa: 'africa-tz.min.json',
		America: 'america-tz.min.json',
		Asia: 'asia-tz.min.json',
		Europe: 'europe-tz.min.json',
		Others: 'others-tz.min.json'
	},
	
TZINFO.getFileName(path, name)
	
TZINFO.moveData(target, doForce)

TZINFO.moveFile(srcName, target)
	
TZINFO.loadData(group)

TZINFO.convertOlsonToPosix(name, doConf, doForce)

/**
 * This will set the converted timezone in POSIX format in the system configuration
 * internal function
 * Parameter: String tzSpecStr - the result of the conversion called before
 **/
TZINFO.setTimeZoneInConf(tzSpecStr)
```
