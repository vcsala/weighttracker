#pragma once
#include <pebble.h>

#define NUM_OF_WEEK_DAYS 7

typedef enum ReminderDays { EVERYDAY = 0, WEEKDAYS = 1, WEEKENDS = 2 } ReminderDays;

typedef struct Reminder {
  int hour;
  int minute;
  // bool before_noon;
  ReminderDays days;
} Reminder;

typedef struct ReminderTimeStamp {
  int valid;
  WeekDay weekday;
  int hour;
  int minute;
} ReminderTimeStamp;

void show_reminder(void);
void hide_reminder(void);
char* get_reminder_str(void);
void init_reminders(uint32_t has_rem, WakeupId wuid);
int get_has_reminders(void);
void set_has_reminders(int hr);
int32_t get_military_time(void);
ReminderTimeStamp get_next_reminder_timestamp(void);
void cancell_all_reminders(void);
void schedule_reminder(void);
WakeupId get_wakeup_id(void);
char* get_has_reminder_str(void);
Reminder* get_reminder_ptr(void);
