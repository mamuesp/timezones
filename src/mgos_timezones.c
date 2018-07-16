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
#include "mgos_timezones.h"

char *mgos_get_zipped_tz_data(char *groupFile) {

	mz_zip_archive zipArch;
	mz_zip_archive_file_stat zipArchStat;
	mz_zip_archive_file_stat zipFileStat;
	mz_bool status;
	mz_uint32 zipIdx;
	mz_bool zipFile;
	char *archFile = "tz-archive.zip";
	char *result;
	char *comment = NULL;
	size_t zipSize;
	size_t archSize;

	result = NULL;
	
	// init the structure
	mz_zip_zero_struct(&zipArch);
	status = mz_zip_reader_init_file(&zipArch, archFile, 0);
	if (!status) {
		LOG(LL_ERROR, ("ZIP file <%s> appears invalid/not loadable!", archFile));
	} else {
		LOG(LL_INFO, ("Check ZIP file <%s> ...", groupFile));
		archSize = mz_zip_get_central_dir_size(&zipArch);
		if (archSize > 0) {
			LOG(LL_INFO, ("ZIP file checked, unzipped size <%ld> ...", (long) archSize));
			zipFile = mz_zip_reader_locate_file_v2(&zipArch, groupFile, comment, MZ_ZIP_FLAG_IGNORE_PATH, &zipIdx);
			if (zipFile == MZ_TRUE) {
				LOG(LL_INFO, ("ZIP file found, index is <%d> ...", zipIdx));
				if (mz_zip_reader_file_stat(&zipArch, zipIdx, &zipFileStat)) {
					LOG(LL_INFO, ("ZIP file <%s> checked, unzipped size <%ld> ...", groupFile, (long) zipFileStat.m_uncomp_size));
					result = mz_zip_reader_extract_to_heap(&zipArch, zipIdx, &zipSize, 0);					
					if (result) {
						LOG(LL_INFO, ("ZIP file uncompressed, size is <%ld> ...", (long) zipSize));
					}
				}
			}
		}
	}
  LOG(LL_INFO, ("ZIP reader end."));
	mz_zip_reader_end(&zipArch);
	return result;
}

bool mgos_free_zipped_tz_data(void *buffer) {
	mz_free(buffer);
	return true;
}

bool mgos_timezones_init(void) {
  return true;
}
