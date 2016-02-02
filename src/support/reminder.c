#include <pebble.h>
#include "../support/config.h"
#include "../support/utils.h"
#include "../support/parameters.h"
#include "../support/reminder.h"
#include "../support/data.h"
#include "../support/pin.h"

const WeekDay INTERNAL_DAYNAMES[NUM_OF_WEEK_DAYS] = { MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY, SUNDAY };

static int s_has_reminders = 0;
static Reminder s_reminder;
static WakeupId s_wakeup_id = -1;

static char s_hour_str[3];
static char s_min_str[3];
// static char s_period_str[3];
static char has_reminder_str[4];
static char s_reminder_str[21];
static char s_days_str[11];

void check_wakeup(void) {
  if (s_wakeup_id != -1 && !wakeup_query(s_wakeup_id, NULL)) {
    s_wakeup_id = -1;
  }
}

void cancell_all_reminders(void) {
  LOG("Cancel all reminders");
  wakeup_cancel_all();
  s_wakeup_id = -1;
  if (get_current_pin_id(REMINDER) > 0) {
    LOG("Delete pin");
    delete_pin(REMINDER, get_current_pin_id(REMINDER));
  }
}

void schedule_reminder(void) {
  LOG("Schedule a new reminder");
  ReminderTimeStamp rts;
  time_t wakeup_time;

  check_wakeup();
  LOG("Wakeup id: %d", (int)s_wakeup_id);

  if (s_wakeup_id != -1) {
    return;
  }

  rts = get_next_reminder_timestamp();

  if (rts.valid) {
    wakeup_time = clock_to_timestamp(rts.weekday, rts.hour, rts.minute);
    s_wakeup_id = wakeup_schedule(wakeup_time, 0, true);

    struct tm* ti = localtime(&wakeup_time);
    uint32_t wdt = (uint32_t)(ti->tm_year + 1900) * 10000 + (uint32_t)(ti->tm_mon + 1) * 100 + (uint32_t)ti->tm_mday;
    uint32_t wmt = (uint32_t)ti->tm_hour * 100 + (uint32_t)ti->tm_min;

    add_pin(REMINDER, get_next_pin_id(REMINDER), wdt, wmt, "");

    if (s_wakeup_id <= 0) {
      // error - we could not schedule a wake up
      s_wakeup_id = -1;
      return;
    }
  }
}

char* get_reminder_str(void) {
  if (get_has_reminders()) {
    zero_padded_int_str(s_reminder.hour, s_hour_str, 3);
    zero_padded_int_str(s_reminder.minute, s_min_str, 3);

    /*if (s_reminder.before_noon) {
      strcpy(s_period_str, "AM");
    } else {
      strcpy(s_period_str, "PM");
    }*/

    switch (s_reminder.days) {
      case WEEKDAYS:
        strcpy(s_days_str, "weekdays");
        break;

      case WEEKENDS:
        strcpy(s_days_str, "weekends");
        break;

      default:
        strcpy(s_days_str, "every day");
        break;
    }

    snprintf(s_reminder_str, sizeof(s_reminder_str), "%s:%s [%s]", s_hour_str, s_min_str, s_days_str);
  } else {
    strcpy(s_reminder_str, "not set");
  }

  return s_reminder_str;
}

void init_reminders(uint32_t has_rem, WakeupId wuid) {
  s_has_reminders = (int)has_rem;

  if (!s_has_reminders) {
      s_reminder.hour = 7;
      s_reminder.minute = 0;
      //s_reminder.before_noon = true;
      s_reminder.days = EVERYDAY;
  }

  s_wakeup_id = wuid;

  check_wakeup();

  if (s_wakeup_id == -1 && s_has_reminders) {
    cancell_all_reminders();
    schedule_reminder();
  } else if (s_wakeup_id != -1 && !s_has_reminders) {
    cancell_all_reminders();
  }
}

int get_has_reminders(void) {
  return s_has_reminders;
}

void set_has_reminders(int hr) {
  s_has_reminders = hr;

  if (s_has_reminders) {
    cancell_all_reminders();
    schedule_reminder();
  } else {
    cancell_all_reminders();
  }
}

int32_t get_military_time(void) {
  return (int32_t)(s_reminder.hour * 100 + s_reminder.minute);
}

ReminderTimeStamp get_next_reminder_timestamp(void) {
  ReminderTimeStamp ts;
  int32_t cmt;
  int32_t rmt;
  int weekday, nrwday;

  weekday = get_week_day();
  cmt = get_current_military_time();
  rmt = get_military_time();
  nrwday = -1;

  switch (s_reminder.days) {
    case WEEKDAYS:
      if (cmt < rmt) {
        nrwday = (weekday < 5) ? weekday : 0;
      } else {
        nrwday = weekday + 1;
        if (nrwday > 4) {
          nrwday = 0;
        }
      }

      break;

    case WEEKENDS:
      if (cmt < rmt) {
        nrwday = (weekday > 4) ? weekday : 5;
      } else {
        nrwday = (weekday == 5) ? 6 : 5;
      }

      break;

    case EVERYDAY:
      if (cmt < rmt) {
        nrwday = weekday;
      } else {
        nrwday = (weekday == 6) ? 0 : weekday + 1;
      }

      break;
  }

  if (nrwday == -1) {
    ts.valid = 0;
  } else if (nrwday == weekday && cmt < rmt) {
    ts.valid = 1;
    ts.weekday = TODAY;
    ts.hour = s_reminder.hour;
    ts.minute = s_reminder.minute;
  } else {
    ts.valid = 1;
    ts.weekday = INTERNAL_DAYNAMES[nrwday];
    ts.hour = s_reminder.hour;
    ts.minute = s_reminder.minute;
  }

  return ts;
}

WakeupId get_wakeup_id(void) {
  return s_wakeup_id;
}

char* get_has_reminder_str(void) {
  if (get_has_reminders()) {
    strcpy(has_reminder_str, "yes");
  } else {
    strcpy(has_reminder_str, "no");
  }

  return has_reminder_str;
}

Reminder* get_reminder_ptr(void) {
  return &s_reminder;
}
