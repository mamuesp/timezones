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
    
bool mgos_file_exists(char *file) {
  FILE *test = fopen(file, "rb");
  bool isExisting = (test != NULL);
  
  if (isExisting) {
    fclose(test);
  }

  return isExisting;
}

size_t mgos_create_filepath(struct mbuf *res, char *path, char *file) {
  uint8_t end = 0;

  mbuf_init(res, strlen(path) + strlen(file) + 2);
  if (strlen(path) > 0) {
    mbuf_append(res, path, strlen(path));
    mbuf_append(res, "/", 1);
  }
  mbuf_append(res, file, strlen(file));
  mbuf_append(res, &end, 1);
  mbuf_trim(res);
  return res->len;
}

bool mgos_file_move(char *file, char *source, char* target) {
  struct mbuf src;
  struct mbuf tgt;
  size_t pos = 0;
  uint8_t ch;

  mgos_create_filepath(&src, source, file);
  mgos_create_filepath(&tgt, target, file);

  if (strcmp(src.buf, tgt.buf) == 0) {
    return true;
  }
  
  FILE* in = fopen(src.buf, "rb");
  FILE* out = fopen(tgt.buf, "wb");
  if(in == NULL || out == NULL) {
    LOG(LL_ERROR, ("mgos_file_move: error opening files! <%s> - <%s>", src.buf, tgt.buf));
    in = out = 0;
    return false;
  }

  fseek(in, 0L, SEEK_END); // file pointer at end of file
  pos = ftell(in);
  fseek(in, 0L, SEEK_SET); // file pointer set at start
  while (pos--) {
    ch = fgetc(in);  // copying file character by character
    fputc(ch, out);
  }    

  fclose(out);
  fclose(in);
  
  if(remove(src.buf) < 0) {
    LOG(LL_ERROR, ("mgos_file_move: error deleting file! <%s>", src.buf));
    return false;
  }

  mbuf_free(&src);
  mbuf_free(&tgt);

  return true;
}

char *mgos_get_data_filename(const char *olson) {
  if (strstr(olson, "Africa") != NULL) {
    return MGOS_TIMEZONES_GROUP_AFRICA;
  } else if (strstr(olson, "America") != NULL) {
    return MGOS_TIMEZONES_GROUP_AMERICA;
  } else if (strstr(olson, "Asia") != NULL) {
    return MGOS_TIMEZONES_GROUP_ASIA;
  } else if (strstr(olson, "Europe") != NULL) {
    return MGOS_TIMEZONES_GROUP_EUROPE;
  } else {
    return MGOS_TIMEZONES_GROUP_OTHERS;
  }
}

uint16_t mgos_generate_api_url(char *buffer, uint16_t len) {
  double lat = mgos_sys_config_get_timezone_latitude();
  double lon = mgos_sys_config_get_timezone_longitude();
  const char *base = mgos_sys_config_get_timezone_api_url();
  const char *key = mgos_sys_config_get_timezone_api_key();
  const char *acc = mgos_sys_config_get_timezone_api_account();
  char *result;

  uint16_t urlLen = 400; //(uint16_t) strlen(base) + (uint16_t) strlen(key) + (uint16_t) strlen(acc);
  if (len < urlLen) {
    LOG(LL_ERROR, ("Provided buffer is too small: <%ld> vs. <%ld>!", (long) len, (long) urlLen));
    return 0;
  }

  result = malloc(urlLen);
  sprintf(result, base, acc, key, lon, lat);
  memset(buffer, 0, len);
  strncpy(buffer, result, urlLen);

  free(result);

  return strlen(buffer);
}

bool mgos_init_archive(const char *archFile, mz_zip_archive *zipArch) {
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

char *mgos_get_zipped_tz_data(const char *archFile, const char *groupFile, bool doConf) {

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
			LOG(LL_DEBUG, ("ZIP file <%s> uncompressed (%ld bytes)", groupFile, (long) zipSize));
			if (doConf) {
	      mgos_set_tzspec(result);
			}
      mz_free(result);
		}
	}

	mgos_exit_archive(&zipArch);
	return result;
}

char *mgos_set_tzspec(char *tzdata) {
    const char *key = mgos_sys_config_get_timezone_olson();
    int   len = strlen(key);
    char *result;
    char *fmt = malloc(len + 7);

    sprintf(fmt, "{%s: %%Q}", key);
    LOG(LL_ERROR, ("Format string for json_scanf: <%s> ...", fmt));
    json_scanf(tzdata, strlen(tzdata), fmt, &result);
    LOG(LL_ERROR, ("Result from json_scanf: <%s> ...", result));
    mgos_sys_config_set_sys_tz_spec(result);

    free(fmt);
    free(result);

    return result;
}

bool mgos_free_zipped_tz_data(void *buffer) {
	mz_free(buffer);
	return true;
}

bool mgos_timezones_init(void) {
  struct mbuf arch;
  const char *dataPath = mgos_sys_config_get_timezone_data_path();
  const char *archFile = mgos_sys_config_get_timezone_arch_file();
  const char *olson = mgos_sys_config_get_timezone_olson();
  const char *groupFile = mgos_get_data_filename(olson);
  bool doFree = false;
  
  if (mgos_file_exists((char *) archFile) && strlen(dataPath) > 0) {
    mgos_file_move((char *) archFile, "", (char *) dataPath);
  }
  mgos_create_filepath(&arch, (char *) dataPath, (char *) archFile);
  mgos_get_zipped_tz_data(arch.buf, groupFile, true);
  mbuf_free(&arch);
  
  return true;
}
