/**
 * midi.c - MIDI input
 * Dominik Neugebauer
 *
 * Reads MIDI bytes from the UART (31250 baud) with an interrupt,
 * one byte at a time. The parser understands note on and note off
 * on all channels. The synth is mono, the last pressed key is played.
 *
 * A MIDI message is a status byte (bit 7 = 1) followed by data bytes
 * (bit 7 = 0). Note on:  0x9n note velocity
 *             Note off: 0x8n note velocity   (n = channel)
 * Note on with velocity 0 also means note off.
 * Running status: the status byte can be left out if it is the same
 * as in the last message, then only the data bytes are sent.
 */
#include "midi.h"
#include <math.h>

#define MIDI_NOTE_OFF  0x80
#define MIDI_NOTE_ON   0x90

static UART_HandleTypeDef *s_huart;
static uint8_t s_rx_byte;             // UART writes the received byte here

static uint8_t s_status = 0;          // last status byte, 0 = none
static uint8_t s_data[2];             // data bytes of the current message
static uint8_t s_data_count = 0;      // how many data bytes we have

static volatile uint8_t s_note = 69;  // last pressed key, 69 = A4 = 440 Hz
static volatile int     s_gate = 0;   // 1 = key pressed, 0 = key released

static void note_on(uint8_t note)
{
    s_note = note;
    s_gate = 1;
}

static void note_off(uint8_t note)
{
    // only close the gate if the released key is the one that is playing
    if (note == s_note) {
        s_gate = 0;
    }
}

// Parse one received byte
static void parse_byte(uint8_t byte)
{
    if (byte >= 0xF8) {
        // realtime messages (clock, start, stop, ...) can come at any time, ignore them
        return;
    }

    if (byte & 0x80) {
        // status byte
        if (byte >= 0xF0) {
            // system messages (sysex, ...) are not used, they also clear running status
            s_status = 0;
        } else {
            s_status = byte;
        }
        s_data_count = 0;
        return;
    }

    // data byte
    if (s_status == 0) {
        return; // no status yet, we do not know what this byte means
    }

    s_data[s_data_count++] = byte;

    uint8_t type = s_status & 0xF0;  // upper 4 bits = message type, lower 4 bits = channel

    if (type == MIDI_NOTE_ON || type == MIDI_NOTE_OFF) {
        if (s_data_count < 2) {
            return; // wait for the velocity byte
        }

        if (type == MIDI_NOTE_ON && s_data[1] > 0) {
            note_on(s_data[0]);
        } else {
            note_off(s_data[0]);
        }
    } else if (type == 0xC0 || type == 0xD0) {
        // program change and channel pressure have 1 data byte, not used
    } else if (s_data_count < 2) {
        // all other channel messages have 2 data bytes, not used
        return;
    }

    // message complete, the next data bytes start a new message (running status)
    s_data_count = 0;
}

void Midi_Init(UART_HandleTypeDef *huart)
{
    s_huart = huart;
    HAL_UART_Receive_IT(s_huart, &s_rx_byte, 1);
}

void Midi_RxCallback(UART_HandleTypeDef *huart)
{
    if (huart != s_huart) {
        return;
    }

    parse_byte(s_rx_byte);
    HAL_UART_Receive_IT(s_huart, &s_rx_byte, 1);  // wait for the next byte
}

void Midi_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart != s_huart) {
        return;
    }

    // after an error (for example a lost byte) the HAL stops receiving, so start again
    s_data_count = 0;
    HAL_UART_Receive_IT(s_huart, &s_rx_byte, 1);
}

int Midi_GetGate(void)
{
    return s_gate;
}

float Midi_GetFrequency(void)
{
    // 12 keys per octave, key 69 = 440 Hz
    return 440.0f * powf(2.0f, (s_note - 69) / 12.0f);
}
