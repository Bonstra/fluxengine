#include <stdint.h>
#include <stdbool.h>
#include "crunch.h"

void crunch(crunch_state_t* state)
{
    while (state->inputlen && state->outputlen)
    {
        uint8_t data = *state->inputptr++;
        state->inputlen--;

        state->fifo = (state->fifo << 8) | data;
        state->fifolen += 8;

        if (state->fifolen >= 8)
        {
            data = state->fifo >> (state->fifolen - 8);
            *state->outputptr++ = data;
            state->outputlen--;
            state->fifolen -= 8;
        }
    }
}

void donecrunch(crunch_state_t* state)
{
    if (state->fifolen > 0)
    {
        uint8_t b = 0;
        state->inputptr = &b;
        state->inputlen = 1;
        crunch(state);
    }
}

void uncrunch(crunch_state_t* state)
{
    while (state->inputlen && state->outputlen)
    {
        /* Drain the FIFO */
        if (state->fifolen > 0) {
            uint8_t fifodata = (uint8_t)(state->fifo >> (state->fifolen - 8));
            *state->outputptr++ = fifodata;
            state->outputlen--;
            state->fifolen -= 8;
            continue;
        }
        uint8_t data = *state->inputptr++;
        state->inputlen--;

        state->fifo = (state->fifo << 8) | data & 0x3f;
        state->fifolen += 8;
        if (data & 0x40) {
            state->fifo = (state->fifo << 8) | 0x80;
            state->fifolen += 8;
        }
        if (data & 0x80) {
            state->fifo = (state->fifo << 8) | 0x81;
            state->fifolen += 8;
        }
    }
}

void doneuncrunch(crunch_state_t* state)
{
    if (state->fifolen > 0)
    {
        uint8_t b = 0;
        state->inputptr = &b;
        state->inputlen = 1;
        uncrunch(state);
    }
}
