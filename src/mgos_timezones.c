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

char *mgos_get_zipped_tz_data(const char *archFile, const char *groupFile, bool doConf) {

	char *result;

	result = mgos_get_zipped_data(archFile, groupFile, NULL, 0);
	if (result) {
		LOG(LL_DEBUG, ("ZIP file <%s> uncompressed", groupFile));
		if (doConf) {
      mgos_set_tzspec(result);
		}
    mz_free(result);
	}
	return result;
}

bool mgos_set_tzspec(char *tzdata) {
    bool result = true;
    const char *key = mgos_sys_config_get_timezone_olson();
    int   len = strlen(key);

    if (strstr(key, tzdata)) {
      char *buff;
      char *fmt = malloc(len + 7);
    
      sprintf(fmt, "{%s: %%Q}", key);
      LOG(LL_DEBUG, ("Format string for json_scanf: <%s> ...", fmt));
      if (json_scanf(tzdata, strlen(tzdata), fmt, &buff) == 1) {
        LOG(LL_INFO, ("Timezone result from JSON data: <%s> ...", buff));
        mgos_sys_config_set_sys_tz_spec(buff);
        free(buff);
      } else {
        LOG(LL_ERROR, ("Timezone <%s> not found in database!", fmt));
      }
      free(fmt);
    } else {
      LOG(LL_ERROR, ("Timezone <%s> not found in database!", key));
    }

    return result;
}

bool mgos_timezones_init(void) {

  if (!mgos_sys_config_get_timezone_enable()) {
    return true;
  }
  
  struct mbuf arch;
  const char *dataPath = mgos_sys_config_get_timezone_data_path();
  dataPath = (dataPath == NULL) ? "" : dataPath;
  LOG(LL_DEBUG, ("Timezone init - dataPath: <%s>", (char *) dataPath));
  const char *archFile = mgos_sys_config_get_timezone_arch_file();
  LOG(LL_DEBUG, ("Timezone init - archFile: <%s>", (char *) archFile));
  const char *olson = mgos_sys_config_get_timezone_olson();
  LOG(LL_DEBUG, ("Timezone init - olson: <%s>", (char *) olson));
  const char *groupFile = mgos_get_data_filename(olson);
  LOG(LL_DEBUG, ("Timezone init - groupFile: <%s>", (char *) groupFile));
  
  if (mgos_file_exists((char *) archFile) && strlen(dataPath) > 0) {
    LOG(LL_DEBUG, ("Moving <%s> to <%s> ...", (char *) archFile, (char *) dataPath));
    mgos_file_move((char *) archFile, "", (char *) dataPath);
  }
  mgos_create_filepath(&arch, (char *) dataPath, (char *) archFile);
  LOG(LL_DEBUG, ("Try to open ZIP file <%s>", (char *) arch.buf));
  mgos_get_zipped_tz_data(arch.buf, groupFile, true);
  LOG(LL_DEBUG, ("Timezone is set, free archive buffer ..."));
  mbuf_free(&arch);
  
  return true;
}
