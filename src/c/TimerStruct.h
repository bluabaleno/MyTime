#include <pebble.h>

typedef enum {
  TIMER_TYPE_Sleep = 0,
  TIMER_TYPE_Programming = 1,
} ActivityType;

typedef struct ActivityLog {
  AcitvityType type;
  uint32_t time_begin;
  uint32_t time_end;
} ActivityLog;
