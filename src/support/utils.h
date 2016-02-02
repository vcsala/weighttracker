#pragma once
#include <pebble.h>

long days(uint32_t start, uint32_t end);
long factor(int month, int day, int year);
uint32_t decrease_date(uint32_t dt);
uint32_t get_current_date(void);
double floor(double fp);
double ceil(double fp);
status_t persist_write_double(const uint32_t key, const double d);
double persist_read_double(const uint32_t key);
void date_str(uint32_t dt, char* buffer, int len);
void double_str(double d, char* buffer, int len);
void zero_padded_int_str(int i, char* buffer, int len);
int get_year_day(void);
int32_t get_current_military_time(void);
int get_week_day(void);
void persist_safe_delete(const uint32_t key);
#ifndef PBL_PLATFORM_APLITE
void animation_started(Animation *anim, void *context);
void animation_stopped(Animation *anim, bool stopped, void *context);
void animate(int duration, int delay, AnimationImplementation *implementation, bool handlers);
#endif
