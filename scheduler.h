#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>

/* Scheduler time values are expressed in milliseconds. */
#define SCH_TASKNUMBER 8
#define SCH_TIMERTICK 10

typedef void (*SCH_TaskFunction_t)(void);

void SCH_Init(void);
void SCH_Update(void);
void SCH_Dispatch(void);
uint32_t SCH_GetTime(void);
uint8_t SCH_AddTask(SCH_TaskFunction_t functionPointer, uint32_t delay,
                    uint32_t period);
uint8_t SCH_DeleteTask(uint8_t taskID);

#endif /* INC_SCHEDULER_H_ */
