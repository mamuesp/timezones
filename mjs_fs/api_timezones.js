/**
 *
 * Partially Copyright (c) 2017 by Pavel Gurenko
 * Copyright (c) 2018 Manfred Mueller-Spaeth <fms1961@gmail.com>
 *
 *  Based on a tool from Pavel Gurenko: http://www.pavelgurenko.com/2017/05/getting-posix-tz-strings-from-olson.html
 *  
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
 * A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 **/ 
load('api_config.js');

let logHead = 'Timezones: ';

let TZINFO = {

	_getZippedData: ffi('char *mgos_get_zipped_tz_data(char *)'),
	_freeZippedData: ffi('void mgos_free_zipped_tz_data(char *)'),
	_dataPath: Cfg.get('timezone.data_path'),
	_dataGroups: ["Africa", "America", "Asia", "Europe", "Others"],
	_dataFiles: {
		Africa: 'africa-tz.min.json',
		America: 'america-tz.min.json',
		Asia: 'asia-tz.min.json',
		Europe: 'europe-tz.min.json',
		Others: 'others-tz.min.json'
	},
	_splitString: function(inTxt, sepChr) {
		let buff = '';
		let out = [];
		for (let i = 0; i < inTxt.length; i++) {
			if (inTxt[i] !== sepChr) {
				buff = buff + inTxt[i];
			} else {
				if (buff.length > 0) {
					out.push(buff);
					buff = '';
				}
			}
		}
		if (buff.length > 0) {
			out.push(buff);
		}
		return out;
	},
  
	getFileName: function(path, name) {
		if (path.length === 0) {
			return name;			
		} else {
			return path + '/' + name;
		}
	},
	
	moveData: function(target, doForce) {
		let result = false;

		if (target !== this._dataPath) {
			let copied = [];
			if (target && target.length > 0) {
				for (let i = 0; i < this._dataGroups.length; i++) {
					let group = this._dataGroups[i];
					let tgtFile = this.moveFile(this._dataFiles[group], target);
					result = (tgtFile.length > 0);
					if (result) {
						copied.push(tgtFile);
					}
				}
			}
		
			if (result) {
				// if all went well, we remove the source files
				for (let i = 0; i < this._dataGroups.length; i++) {
					let group = this._dataGroups[i];
					let file = this.getFileName(this._dataPath, this._dataFiles[group]);
					result = (File.remove(file) === 0);
				}
			} else {
				// on error we remove already copied files
				for (let i = 0; i < copied.length; i++) {
					let file = copied[i];
					(File.remove(file) === 0);
				}
			}
			this._dataPath = result ? target : this._dataPath; 

			let oSet = { timezone: { data_path: this._dataPath } };
			Cfg.set(oSet);
		} else {
			// already moved ...
			result = true;
		}
		
		return result;
	},

	moveFile: function(srcName, target) {
		let result = '';
		if (target && target.length > 0) {
			let srcFile = this.getFileName(this._dataPath, srcName);
			let tgtFile = this.getFileName(target, srcName); 
			let strJSON = File.read(srcFile);
			if (strJSON && strJSON.length > 0) {
				let len = 0;
				len = File.write(strJSON, tgtFile);
				if (len === strJSON.length) {
					result = tgtFile;
				}
			}
		}
		return result;
	},
	
	loadData: function(group) {
		let doZip =	Cfg.get('timezone.zipped_data');
		let strJSON = '';
		if (doZip) {
			let file = this._dataFiles[group];
			let data = this._getZippedData(file);
			strJSON = mkstr(data, 0, data.length, true);
			this._freeZippedData(data);
		} else {
			let file = this.getFileName(this._dataPath, this._dataFiles[group]); 
			strJSON = File.read(file);
		}
		let oJSON = {};
		Log.debug(logHead + 'Timezone JSON file read!');
		if (strJSON !== null && strJSON.length > 0) {
			oJSON = JSON.parse(strJSON);
			Log.debug(logHead + 'JSON Status data read successfully!');
		}
		
		if (doZip) {
			this._freeZippedData(strJSON);
		}
		
		return oJSON;
	},

	getGroup: function(name) {
		let parts = this._splitString(name, '/');
		let group = 'Others';
		if (parts.length > 0) {
			// check if group is listed in _dataFiles
			if (this._dataFiles[parts[0]]) {
				group = parts[0];
			}
		}
		return group;
	},

	getTzSpec: function(tzData, name, doConf, doForce) {
		let tzSpec = tzData[name];
		// don't beed the data anymore ...
		tzData = null;
		if (doConf) {
			this.setTimeZoneInConf(tzSpec);
		}
		gc(true);
		Log.info('Timezone settings for <' + name + '>: ' + tzSpec);
		return tzSpec;
	},
	
	convertOlsonToPosix: function (name, doConf, doForce) {
		let group = this.getGroup(name);
		let tzData = this.loadData(group);
		if (tzData) {
			this.getTzSpec(tzData, name, doConf, doForce);
		} else {
			Log.error('Timezone group <' + name + '> not found in database!');
			return 'UTC0';
		}
	},
	
	setTimeZoneInConf: function(tzSpecStr) {
		let oSet = { sys: { tz_spec: tzSpecStr } };
		Cfg.set(oSet);
	}

}
