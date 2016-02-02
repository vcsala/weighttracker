#include <pebble.h>
#include "../support/config.h"
#include "../support/utils.h"
#include "../support/parameters.h"

static const double CONVERSION_MULTIPLIER = 10000.0;
#ifndef PBL_PLATFORM_APLITE
static bool s_animating = false;
#endif

long days(uint32_t start, uint32_t end) {
  int sm, sd, sy, em, ed, ey;
  long fac1, fac2, diff;

  sy = start / 10000;
  sm = (start - sy * 10000) / 100;
  sd = (start - sy * 10000 - sm * 100);

  ey = end / 10000;
  em = (end - ey * 10000) / 100;
  ed = (end - ey * 10000 - em * 100);

  fac1 = factor(sm, sd, sy);
  fac2 = factor(em, ed, ey);

  diff = fac2 - fac1;

  return (diff);
}

long factor(int month, int day, int year)
{
  long f;

  f = 365 * year + day + 31 * (month - 1);

  if (month < 3) {
    f = f + ((year -1)/4) - (.75 * ((year - 1)/100)+1);
  } else {
    f = f - (.4 * month + 2.3) + (year / 4) - (((.75 * (year / 100) + 1)));
  }

  return (f);
}

int days_in_month(int year, int month) {
  int numberOfDays;

  if (month == 4 || month == 6 || month == 9 || month == 11) {
    numberOfDays = 30;
  } else if (month == 2) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
      numberOfDays = 29;
    } else {
      numberOfDays = 28;
    }
  } else {
    numberOfDays = 31;
  }

  return numberOfDays;
}

void date_str(uint32_t dt, char* buffer, int len) {
  uint32_t m, d, y;

  y = dt / (uint32_t)10000;
  m = (dt - y * (uint32_t)10000) / (uint32_t)100;
  d = (dt - y * (uint32_t)10000 - m * (uint32_t)100);

  switch (get_date_format()) {
    case YMD:
      snprintf(buffer, len, "%lu.%lu.%lu", y, m, d);
      break;

    case MDY:
      snprintf(buffer, len, "%lu.%lu.%lu", m, d, y);
      break;

    case DMY:
      snprintf(buffer, len, "%lu.%lu.%lu", d, m, y);
      break;
  }
}

void double_str(double d, char* buffer, int len) {
  int iWhole, iFract;
  double am = get_accuracy_multiplier();

  d = (double)((int)(d * am + 0.5)) / am;
  iWhole = (int)d;
  iFract = (int)((d - (double)iWhole) * am + 0.5);
  snprintf(buffer, len, "%d.%d", iWhole, iFract);
}

void zero_padded_int_str(int i, char* buffer, int len) {
  int t = ((i < 100) ? ((i < 0) ? 0 : i) : (i % 100));

  if (t < 10) {
    snprintf(buffer, len, "0%d", t);
  } else {
    snprintf(buffer, len, "%d", t);
  }
}

uint32_t decrease_date(uint32_t dt) {
  uint32_t m, d, y;
  uint32_t ndt;

  y = dt / (uint32_t)10000;
  m = (dt - y * (uint32_t)10000) / (uint32_t)100;
  d = (dt - y * (uint32_t)10000 - m * (uint32_t)100);

  d--;

  if (d < 1) {
    m--;

    if (m < 1) {
      m = 12;
      y--;
    }

    d = days_in_month(y, m);
  }

  ndt = ((uint32_t)y) * (uint32_t)10000 + ((uint32_t)m) * (uint32_t)100 + (uint32_t)d;

  return ndt;
}

uint32_t get_current_date(void) {
  time_t rawtime;
  struct tm* timeinfo;
  uint32_t cdate;

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  cdate = (uint32_t)(timeinfo->tm_year + 1900) * (uint32_t)10000 + (uint32_t)(timeinfo->tm_mon + 1) * (uint32_t)100 + (uint32_t)timeinfo->tm_mday;

  return cdate;
}

int get_year_day(void) {
  time_t rawtime;
  struct tm* timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  return timeinfo->tm_yday;
}

double floor(double fp) {
  int i;

  i = (int)fp;

  if (i > fp) {
    i--;
  }

  return (double)i;
}

double ceil(double fp) {
  int i;

  i = (int)fp;

  if (i < fp) {
    i++;
  }

  return (double)i;
}

status_t persist_write_double(const uint32_t key, const double d) {
  int32_t v;

  v = (int32_t)(d * CONVERSION_MULTIPLIER);
  return (persist_write_int(key, v));
}

double persist_read_double(const uint32_t key) {
  int32_t v;

  v = persist_read_int(key);
  return (((double)v) / CONVERSION_MULTIPLIER);
}

void persist_safe_delete(const uint32_t key) {
  if (persist_exists(key)) {
    persist_delete(key);
  }
}

int32_t get_current_military_time(void) {
  time_t rawtime;
  struct tm* timeinfo;
  int32_t mt;

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  mt = (int32_t)timeinfo->tm_hour * 100 + (int32_t)timeinfo->tm_min;

  return mt;
}

int get_week_day(void) {
  time_t rawtime;
  struct tm* timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  return (timeinfo->tm_wday + 6) % 7;
}

/*************************** AnimationImplementation **************************/
#ifndef PBL_PLATFORM_APLITE
void animation_started(Animation *anim, void *context) {
  s_animating = true;
}

void animation_stopped(Animation *anim, bool stopped, void *context) {
  s_animating = false;
}

void animate(int duration, int delay, AnimationImplementation *implementation, bool handlers) {
  Animation *anim = animation_create();
  animation_set_duration(anim, duration);
  animation_set_delay(anim, delay);
  animation_set_curve(anim, AnimationCurveEaseInOut);
  animation_set_implementation(anim, implementation);
  if(handlers) {
    animation_set_handlers(anim, (AnimationHandlers) {
      .started = animation_started,
      .stopped = animation_stopped
    }, NULL);
  }
  animation_schedule(anim);
}
#endif
