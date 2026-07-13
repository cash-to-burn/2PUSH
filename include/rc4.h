#pragma once

#include <iostream>

struct RC4_State {
    unsigned char S[256];
    int i = 0;
    int j = 0;
};


void rc4_init(RC4_State *state, const unsigned char *key, int key_len) {
    for (int i = 0; i < 256; i++) state->S[i] = i;
    int j = 0;
    for (int i = 0; i < 256; i++) {
        j = (j + state->S[i] + key[i % key_len]) % 256;
        std::swap(state->S[i], state->S[j]);
    }
    state->i = 0;
    state->j = 0;
}


void rc4_crypt(RC4_State *state, char *buffer, int buffer_size) {
    int i = state->i;
    int j = state->j;
    unsigned char *data = (unsigned char*)buffer;

    for (int k = 0; k < buffer_size; k++) {
        i = (i + 1) % 256;
        j = (j + state->S[i]) % 256;
        std::swap(state->S[i], state->S[j]);
        unsigned char t = (state->S[i] + state->S[j]) % 256;
        data[k] ^= state->S[t];
    }
    
    state->i = i;
    state->j = j;
}