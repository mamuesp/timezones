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
#ifndef __MGOS_TIMEZONES_H
#define __MGOS_TIMEZONES_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mongoose.h"
#include "mgos.h"
#include "mgos_app.h"
#include "mgos_config.h"
#include "mgos_zip_tools.h"
#include "mgos_common_tools.h"

#define MGOS_TIMEZONES_GROUP_AFRICA "africa-tz.min.json"
#define MGOS_TIMEZONES_GROUP_AMERICA "america-tz.min.json"
#define MGOS_TIMEZONES_GROUP_ASIA "asia-tz.min.json"
#define MGOS_TIMEZONES_GROUP_EUROPE "europe-tz.min.json"
#define MGOS_TIMEZONES_GROUP_OTHERS "others-tz.min.json"

char *mgos_get_data_filename(const char *olson);

uint16_t mgos_generate_api_url(char *buffer, uint16_t len);

char *mgos_get_zipped_tz_data(const char *archFile, const char *groupFile, bool doConf);

bool mgos_set_tzspec(char *tzdata, size_t zipSize);

bool mgos_timezones_init(void);

#endif
