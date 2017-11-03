#include <interpow/interpow.h>
#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>


#define NUM_OF_ARRAYS 4
#define SIZE          10

uint16_t a[SIZE] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
uint16_t b[SIZE] = {19, 18, 17, 16, 15, 14, 13, 12, 11, 10};
uint16_t c[SIZE] = {29, 28, 27, 26, 25, 24, 23, 22, 21, 20};
uint16_t d[SIZE] = {39, 38, 37, 36, 35, 34, 33, 32, 31, 30};

/*
 *******************************************************************************
 * Task functions declaration
 *******************************************************************************
 */

// Find minimum of given arrays
void task_find_min_f(void);

// Subtract minimum from given arrays
void task_sub_min_f(void);

/*
 *******************************************************************************
 * Definition of tasks, preceded by the PERSISTENT declaration
 *******************************************************************************
 */

#pragma PERSISTENT(TASK_FIND_MIN)
NewTask(TASK_FIND_MIN, task_find_min_f, 1) // with self-field

#pragma PERSISTENT(TASK_SUB_MIN)
NewTask(TASK_SUB_MIN, task_sub_min_f, 1) // with self-field

/*
 *******************************************************************************
 * Inform the program about the task to execute on the first start of the
 * system, preceded by the PERSISTENT declaration
 *******************************************************************************
 */

#pragma PERSISTENT(PersState)
InitialTask(TASK_FIND_MIN)

/*
 *******************************************************************************
 * Definition of fields, preceded by the PERSISTENT declaration
 *******************************************************************************
 */

// This self-field helps TASK_FIND_MIN keep track of the array whose minimum
// has to be found (array a to d)
#pragma PERSISTENT(PersSField0(TASK_FIND_MIN, sf_array_index))
#pragma PERSISTENT(PersSField1(TASK_FIND_MIN, sf_array_index))
NewSelfField(TASK_FIND_MIN, sf_array_index, UINT8, 1, SELF_FIELD_CODE_1)

// This field connects the two tasks, informing TASK_SUB_MIN about the position
// of each array's minimum
#pragma PERSISTENT(PersField(TASK_FIND_MIN, TASK_SUB_MIN, f_min_index))
NewField(TASK_FIND_MIN, TASK_SUB_MIN, f_min_index, UINT8, NUM_OF_ARRAYS)

// This self-field helps TASK_SUB_MIN keep track of the array whose average
// has to be computed
#pragma PERSISTENT(PersSField0(TASK_FIND_MIN, sf_array_index))
#pragma PERSISTENT(PersSField1(TASK_FIND_MIN, sf_array_index))
NewSelfField(TASK_SUB_MIN, sf_array_index, UINT8, 1, SELF_FIELD_CODE_1)

// This self-field helps TASK_SUB_MIN keep track of each array's average,
// after an array has been subtracted its minimum
#pragma PERSISTENT(PersSField0(TASK_SUB_MIN, sf_avg))
#pragma PERSISTENT(PersSField1(TASK_SUB_MIN, sf_avg))
NewSelfField(TASK_SUB_MIN, sf_avg, FLOAT32, NUM_OF_ARRAYS, SELF_FIELD_CODE_2)

/*
 *******************************************************************************
 * main
 *******************************************************************************
 */
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    while(1) {
        Resume();
    }
}


uint8_t find_min_pos(uint16_t* array)
{
    uint8_t i, min_pos;
    uint16_t min = array[0];

    min_pos = 0;
    for (i = 1; i < SIZE; i++) {
        if (array[i] < min) {
            min = array[i];
            min_pos = i;
        }
    }

    return min_pos;
}


float sub_min_and_avg(uint16_t* array, uint8_t min_pos)
{
    uint8_t i;
    float avg = 0;

    for (i = 0; i < SIZE; i++) {
        avg += array[i] - array[min_pos];
    }

    return avg / SIZE;
}


void task_find_min_f()
{
    uint8_t array_index;
    uint8_t min_pos;

    ReadSelfField_U8(TASK_FIND_MIN, sf_array_index, &array_index); // initially sf_array_index = 0

    switch (array_index) {
    case 0:
        min_pos = find_min_pos(a);
        break;
    case 1:
        min_pos = find_min_pos(b);
        break;
    case 2:
        min_pos = find_min_pos(c);
        break;
    case 3:
        min_pos = find_min_pos(d);
        break;
    }

    WriteFieldElement_U8(TASK_FIND_MIN, TASK_SUB_MIN, f_min_index, &min_pos, array_index);

    if (++array_index == NUM_OF_ARRAYS) {
        array_index = 0;
        WriteSelfField_U8(TASK_FIND_MIN, sf_array_index, &array_index);
        StartTask(TASK_SUB_MIN);
    }
    else {
        WriteSelfField_U8(TASK_FIND_MIN, sf_array_index, &array_index);
        StartTask(TASK_FIND_MIN);
    }
}


void task_sub_min_f()
{
    uint8_t array_index;
    uint8_t min_pos[NUM_OF_ARRAYS];
    float avg[NUM_OF_ARRAYS];

    ReadSelfField_U8(TASK_SUB_MIN, sf_array_index, &array_index); // initially sf_array_index = 0
    ReadField_U8(TASK_FIND_MIN, TASK_SUB_MIN, f_min_index, min_pos);
    ReadSelfField_F32(TASK_SUB_MIN, sf_avg, avg);

    switch (array_index) {
    case 0:
        avg[array_index] = sub_min_and_avg(a, min_pos[array_index]);
        break;
    case 1:
        avg[array_index] = sub_min_and_avg(b, min_pos[array_index]);
        break;
    case 2:
        avg[array_index] = sub_min_and_avg(c, min_pos[array_index]);
        break;
    case 3:
        avg[array_index] = sub_min_and_avg(d, min_pos[array_index]);
        break;
    }

    WriteSelfField_F32(TASK_SUB_MIN, sf_avg, avg);

    if (++array_index == NUM_OF_ARRAYS) {
        array_index = 0;
        WriteSelfField_U8(TASK_SUB_MIN, sf_array_index, &array_index);
        while(1); // END OF PROGRAM, breakpoint here to check result
    }
    else {
        WriteSelfField_U8(TASK_SUB_MIN, sf_array_index, &array_index);
        StartTask(TASK_SUB_MIN);
    }
}
