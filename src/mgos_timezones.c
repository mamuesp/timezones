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
#include <stdbool.h>
#include "mgos_timezones.h"

bool mgos_init_archive(char *archFile, mz_zip_archive *zipArch) {
	mz_bool status;
	// init the structure
	mz_zip_zero_struct(zipArch);
	status = mz_zip_reader_init_file(zipArch, archFile, 0);
	if (!status) {
		LOG(LL_ERROR, ("ZIP file <%s> appears invalid/not loadable!", archFile));
		return false;
	}
	return true;
}

bool mgos_exit_archive(mz_zip_archive *zipArch) {
  LOG(LL_DEBUG, ("ZIP reader end."));
	mz_zip_reader_end(zipArch);
  return true;
}

uint16_t mgos_get_zipped_buffer_size(char *archFile, char *groupFile) {

  mz_zip_archive zipArch;
	mz_zip_archive_file_stat zipFileStat;
	mz_uint32 zipIdx;
	mz_bool zipFile;
	uint16_t result;
	char *comment = NULL;
	long archSize = 0;

	result = 0;

	if (!mgos_init_archive(archFile, &zipArch)) {
		return result;
	}

	LOG(LL_DEBUG, ("Check ZIP file <%s> ...", groupFile));
	archSize = zipArch.m_archive_size;
	if (archSize > 0) {
		LOG(LL_DEBUG, ("ZIP file checked, archive size <%ld> ...", (long) archSize));
		zipFile = mz_zip_reader_locate_file_v2(&zipArch, groupFile, comment, MZ_ZIP_FLAG_IGNORE_PATH, &zipIdx);
		if (zipFile == MZ_TRUE) {
			LOG(LL_DEBUG, ("ZIP file found, index is <%d> ...", zipIdx));
			if (mz_zip_reader_file_stat(&zipArch, zipIdx, &zipFileStat)) {
				result = (uint16_t) zipFileStat.m_uncomp_size;
			}
		}
	}

  LOG(LL_DEBUG, ("ZIP reader end."));
	mz_zip_reader_end(&zipArch);

	return result;
}

char *mgos_get_zipped_tz_data(char *archFile, char *groupFile, char *buffer) {

	mz_zip_archive zipArch;
	mz_zip_archive_file_stat zipFileStat;
	mz_uint32 zipIdx;
	mz_bool zipFile;
	char *result;
	char *comment = NULL;
	size_t zipSize = 0;
	long archSize = 0;

	result = NULL;
	if (!mgos_init_archive(archFile, &zipArch)) {
		return result;
	}

	LOG(LL_DEBUG, ("Check ZIP file <%s> ...", groupFile));
	archSize = zipArch.m_archive_size;
	if (archSize <= 0) {
		return result;
	}

	LOG(LL_DEBUG, ("ZIP file checked, archive size <%ld> ...", (long) archSize));
	zipFile = mz_zip_reader_locate_file_v2(&zipArch, groupFile, comment, MZ_ZIP_FLAG_IGNORE_PATH, &zipIdx);
	if (zipFile == MZ_FALSE) {
		return result;
	}

	LOG(LL_DEBUG, ("ZIP file found, index is <%d> ...", zipIdx));
	if (mz_zip_reader_file_stat(&zipArch, zipIdx, &zipFileStat)) {
		zipSize = zipFileStat.m_uncomp_size;
		LOG(LL_DEBUG, ("ZIP file <%s> checked, unzipped size <%ld> ...", groupFile, (long) zipSize));
		result = mz_zip_reader_extract_to_heap(&zipArch, zipIdx, &zipSize, 0);
		if (result) {
			if (buffer != NULL) {
				strncpy(buffer, result, zipSize);
				mz_free(result);
				result = NULL;
			}
			LOG(LL_DEBUG, ("ZIP file <%s> uncompressed (%ld bytes)", groupFile, (long) zipSize));
		}
	}

	mgos_exit_archive(&zipArch);
	return result;
}

bool mgos_free_zipped_tz_data(void *buffer) {
	mz_free(buffer);
	return true;
}

bool mgos_timezones_init(void) {
  return true;
}
