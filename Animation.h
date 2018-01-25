/*
 * Copyright (c) , Majenko Technologies
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * 
 *  1. Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 * 
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 * 
 *  3. Neither the name of Majenko Technologies nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without 
 *     specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef _ANIMATION_H
#define _ANIMATION_H

#include <Arduino.h>
#include <LIBus.h>
#include <PCM1774.h>
#include <TPA2016.h>
#include <I2S.h>
#include <DFATFS.h>

class Animation;

#define U16(X) (X) & 0xFF, (X) >> 8
#define U24(X) (X) & 0xFF, (X) >> 8, (X) >> 16
#define U32(X) (X) & 0xFF, (X) >> 8, (X) >> 16, (X)  >> 24

typedef struct _AnimationList {
    Animation *anim;
    struct _AnimationList *next;
} AnimationList;

class Animation {
    private:
        // Private functions and variables here.  They can only be accessed
        // by functions within the class.

        static AnimationList *_list;

        const uint8_t *_prog;
        uint32_t _pc;
        uint8_t _state;
        uint32_t _delayStart;
        uint32_t _delay;
        uint32_t _repeatPC;
        uint32_t _repeatCount;
        bool _forever;
        static LIBusMaster *_libus;
        static I2S *_i2s;
        static PCM1774 *_dac;
        static TPA2016 *_amp;

        DFILE _soundfile[4];
        int _sound[4];


        const static uint8_t sSTOP = 0;
        const static uint8_t sRUN = 1;
        const static uint8_t sDELAY = 2;

    public:
        // LIBus control
        const static uint8_t SEND               = 0x11;

        // Sound (playback)
        const static uint8_t PLAY_MONO_LEFT     = 0x20;
        const static uint8_t PLAY_MONO_RIGHT    = 0x21;
        const static uint8_t PLAY_MONO_BOTH     = 0x22;
        const static uint8_t PLAY_STEREO        = 0x23;
        const static uint8_t PLAY_STOP          = 0x24;
   
        // Sound (control)
        const static uint8_t SOUND_VOLUME       = 0x30;
        const static uint8_t SOUND_DGAIN        = 0x31;
        const static uint8_t SOUND_AGAIN        = 0x32;
        const static uint8_t SOUND_BASS         = 0x33;
        const static uint8_t SOUND_MID          = 0x34;
        const static uint8_t SOUND_TREBLE       = 0x35;
        const static uint8_t SOUND_3D           = 0x36;

        // Flow control
        const static uint8_t DELAY              = 0xf0;
        const static uint8_t RDELAY             = 0xf1;
        const static uint8_t REPEAT             = 0xf2;
        const static uint8_t FOREVER            = 0xf3;
        const static uint8_t LOOP               = 0xf4;
        const static uint8_t END                = 0xFF;

        // Public functions and variables.  These can be accessed from
        // outside the class.
        Animation();
        Animation(const uint8_t *program);
        bool execute();
        void setAnimation(const uint8_t *prog);
        void start();
        void stop();
        void nudge();

        static void process();
        static Animation *createAnimation();
        static Animation *createAnimation(const uint8_t *program);
        static void addAnimation(Animation *anim);

        static void setLIBus(LIBusMaster &m) { _libus = &m; }
        static void setI2S(I2S &m) { _i2s = &m; }
        static void setDAC(PCM1774 &m) { _dac = &m; }
        static void setAMP(TPA2016 &m) { _amp = &m; }
};
#endif
