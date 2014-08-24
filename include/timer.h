/*
 * timer.h
 *
 * Type and function declarations for timer handling.
 *
 * (c) 2014 Hadi Moshayedi <hadi@moshayedi.net>
 */

#ifndef TIMER_H
#define TIMER_H

#include <types.h>

void timer_set_interval(uint32_t interval_microsecond);
void timer_clear_interrupt(void);

#endif
