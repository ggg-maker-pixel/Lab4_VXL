#include "scheduler.h"

typedef struct {
  SCH_TaskFunction_t functionPointer;
  uint8_t id;
  uint32_t delay;
  uint32_t period;
  uint8_t flag;
} SCH_Task;

static SCH_Task tasks[SCH_TASKNUMBER];
static volatile uint32_t schedulerTimeMs;

void SCH_Init(void)
{
  uint8_t i;

  schedulerTimeMs = 0;

  for (i = 0; i < SCH_TASKNUMBER; i++) {
    tasks[i].functionPointer = 0;
    tasks[i].id = SCH_TASKNUMBER - i - 1;
    tasks[i].delay = 0;
    tasks[i].period = 0;
    tasks[i].flag = 0;
  }
}

void SCH_Update(void)
{
  schedulerTimeMs += SCH_TIMERTICK;

  if (tasks[0].functionPointer == 0) {
    return;
  }

  if (tasks[0].delay > SCH_TIMERTICK) {
    tasks[0].delay -= SCH_TIMERTICK;
  } else {
    tasks[0].delay = 0;
  }

  if (tasks[0].delay == 0) {
    tasks[0].flag = 1;
  }
}

uint32_t SCH_GetTime(void)
{
  return schedulerTimeMs;
}

uint8_t SCH_AddTask(SCH_TaskFunction_t functionPointer, uint32_t delay,
                    uint32_t period)
{
  uint8_t i;
  uint8_t j;
  uint8_t currentID;
  uint32_t currentDelay = 0;

  if ((functionPointer == 0) ||
      (tasks[SCH_TASKNUMBER - 1].functionPointer != 0)) {
    return SCH_TASKNUMBER;
  }

  currentID = tasks[SCH_TASKNUMBER - 1].id;
  for (i = 0; i < SCH_TASKNUMBER; i++) {
    currentDelay += tasks[i].delay;
    if ((currentDelay > delay) || (tasks[i].functionPointer == 0)) {
      for (j = SCH_TASKNUMBER - 1; j > i; j--) {
        tasks[j] = tasks[j - 1];
      }

      tasks[i].functionPointer = functionPointer;
      tasks[i].id = currentID;
      tasks[i].period = period;
      tasks[i].flag = 0;

      if (currentDelay > delay) {
        uint32_t newDelay = currentDelay - delay;
        tasks[i].delay = tasks[i + 1].delay - newDelay;
        if (tasks[i].delay == 0) {
          tasks[i].flag = 1;
        }
        tasks[i + 1].delay = newDelay;
        if (tasks[i + 1].delay == 0) {
          tasks[i + 1].flag = 1;
        }
      } else {
        tasks[i].delay = delay - currentDelay;
        if (tasks[i].delay == 0) {
          tasks[i].flag = 1;
        }
      }

      return tasks[i].id;
    }
  }

  return SCH_TASKNUMBER;
}

uint8_t SCH_DeleteTask(uint8_t taskID)
{
  uint8_t i;
  uint8_t j;
  uint8_t currentID;

  for (i = 0; i < SCH_TASKNUMBER; i++) {
    if (tasks[i].id == taskID) {
      currentID = tasks[i].id;

      if ((i + 1 < SCH_TASKNUMBER) &&
          (tasks[i + 1].functionPointer != 0)) {
        tasks[i + 1].delay += tasks[i].delay;
      }

      for (j = i; j < SCH_TASKNUMBER - 1; j++) {
        tasks[j] = tasks[j + 1];
      }

      tasks[SCH_TASKNUMBER - 1].functionPointer = 0;
      tasks[SCH_TASKNUMBER - 1].id = currentID;
      tasks[SCH_TASKNUMBER - 1].delay = 0;
      tasks[SCH_TASKNUMBER - 1].period = 0;
      tasks[SCH_TASKNUMBER - 1].flag = 0;
      return 1;
    }
  }

  return 0;
}

void SCH_Dispatch(void)
{
  SCH_Task newTask;

  if ((tasks[0].functionPointer == 0) || (tasks[0].flag == 0)) {
    return;
  }

  tasks[0].functionPointer();
  tasks[0].flag = 0;
  newTask = tasks[0];
  (void)SCH_DeleteTask(tasks[0].id);

  if (newTask.period > 0) {
    (void)SCH_AddTask(newTask.functionPointer, newTask.period,
                      newTask.period);
  }
}
