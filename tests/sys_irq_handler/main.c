/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Interrupt handler test application
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * This application demonstrates how to use the interrupt handler thread
 * module `irq_handler`. For that purpose the main thread simulates interrupts
 * from 2 interrupt sources with different priorities with a period
 * of a half second. One interrupt source triggers a second interrupt before
 * the interrupts can be handled by the interrupt handler.
 *
 * To be able to simulate the interrupts by the main thread, the interrupt
 * handler thread has to have a lower priority as the main thread for this
 * example. Otherwise, the interrupts would be handled immediately when they
 * are generated by the main thread. Therefore the interrupt handler thread
 * priority is set to THREAD_PRIORITY_MAIN+1 for this example.
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "irq_handler.h"
#include "mutex.h"
#include "thread.h"
#include "xtimer.h"

#define TEST_TIME                   (100 * US_PER_MS)
#define TEST_REPETITIONS            (10)

static void _int1_service (void *arg);
static void _int2_service (void *arg);

static char some_stack[THREAD_STACKSIZE_MAIN];
static int i = 0;
static int n_int1_handled = 0;
static int n_int2_handled = 0;
static int n_already_pending = 0;
static mutex_t mutex = MUTEX_INIT;
xtimer_t timer1a = {
    .callback = _int1_service,
    .arg = &timer1a
};
xtimer_t timer1b = {
    .callback = _int1_service,
    .arg = &timer1b
};
xtimer_t timer2 = {
    .callback = _int2_service,
    .arg = &timer2
};

/* preallocated and initialized interrupt event objects */
static irq_event_t _int1_event = IRQ_EVENT_INIT;
static irq_event_t _int2_event = IRQ_EVENT_INIT;

/* interrupt service routine for a simulated interrupt from source 1 */
static void _int1_service (void *arg)
{
    (void)arg;
    puts("int1 triggered");
    /* registers just the interrupt event and returns */
    if (irq_event_add(&_int1_event) == -EALREADY) {
        puts("int1 is already pending");
        n_already_pending++;
    }
}

/* interrupt handler for interrupts from source 1 */
static void _int1_handler(void *ctx)
{
    xtimer_t *timer = ctx;
    xtimer_set(timer, TEST_TIME);
    puts("int1 handled");
    mutex_lock(&mutex);
    n_int1_handled++;
    mutex_unlock(&mutex);
}

/* interrupt service routine for a simulated interrupt from source 2 */
static void _int2_service (void *arg)
{
    (void)arg;
    puts("int2 triggered");
    /* registers just the interrupt event and returns */
    if (irq_event_add(&_int2_event) == -EALREADY) {
        puts("int2 is already pending");
        n_already_pending++;
    }
}

/* interrupt handler for interrupts from source 2 */
static void _int2_handler(void *ctx)
{
    xtimer_t *timer = ctx;
    xtimer_set(timer, TEST_TIME);
    puts("int2 handled");
    mutex_lock(&mutex);
    n_int2_handled++;
    mutex_unlock(&mutex);
}

static void *some_thread(void *arg)
{
    (void)arg;

    puts("some_thread is starting");

    while (1) {
        mutex_lock(&mutex);
        i++;
        mutex_unlock(&mutex);
        xtimer_usleep(US_PER_MS);
    }
    return NULL;
}

int main(void)
{
    _int1_event.isr = _int1_handler;
    _int1_event.ctx = &timer1a;

    _int2_event.isr = _int2_handler;
    _int2_event.ctx = &timer2;

    puts("START");

    thread_create(some_stack, sizeof(some_stack),
                  THREAD_PRIORITY_MAIN + 2, THREAD_CREATE_WOUT_YIELD,
                  some_thread, NULL, "some_thread");
    puts("some_thread created");

    /* wait to let some_thread to start */
    xtimer_usleep(US_PER_SEC);

    i = 0;

    xtimer_set(&timer1a, TEST_TIME);
    xtimer_set(&timer1b, TEST_TIME);
    xtimer_set(&timer2, TEST_TIME);

    xtimer_usleep(TEST_TIME * TEST_REPETITIONS + TEST_TIME/2);

    xtimer_remove(&timer1a);
    xtimer_remove(&timer1b);
    xtimer_remove(&timer2);

    mutex_lock(&mutex);
    if ((i > 0) &&
        (n_int1_handled == TEST_REPETITIONS) &&
        (n_int2_handled == TEST_REPETITIONS) &&
        (n_already_pending == 1)) {
        puts("[SUCCESS]");
    }
    mutex_unlock(&mutex);

    return 0;
}
