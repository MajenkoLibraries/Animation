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

#if (ARDUINO >= 100) 
# include <Arduino.h>
#else
# include <WProgram.h>
#endif

#include <ULK.h>

class Animation;

#define PAIR(X) (X) >> 8, (X) & 0xFF

typedef struct _AnimationList {
    Animation *anim;
    struct _AnimationList *next;
} AnimationList;

typedef struct _Fade {
    uint8_t group;
    uint8_t target;
    uint32_t start;
    uint8_t delay;
    struct _Fade *next;
} Fade;

class Animation {
    private:
        // Private functions and variables here.  They can only be accessed
        // by functions within the class.

        static AnimationList *_list;

        Fade *_fade;
        uint8_t *_prog;
        uint32_t _pc;
        uint8_t *_groups[56];
        uint8_t _state;
        uint32_t _delayStart;
        uint32_t _delay;
        uint32_t _repeatPC;
        uint32_t _repeatCount;
        boolean _forever;

        void execute();
        void addFade(Fade *f);
        void delFade(Fade *f);
        void processFades();

        const static uint8_t sSTOP = 0;
        const static uint8_t sRUN = 1;
        const static uint8_t sDELAY = 2;
        const static uint8_t sWAIT = 3;

    public:
        const static uint8_t GROUP = 1;
        const static uint8_t SET = 2;
        const static uint8_t FADE = 3;
        const static uint8_t DELAY = 4;
        const static uint8_t WAITEQ = 5;
        const static uint8_t RDELAY = 6;
        const static uint8_t REPEAT = 7;
        const static uint8_t FOREVER = 8;

        const static uint8_t LOOP = 254;
        const static uint8_t END = 255;

        // Public functions and variables.  These can be accessed from
        // outside the class.
        Animation();
        void run();
        void setAnimation(uint8_t *prog);
        void start();
        void stop();
        void nudge();

        static void process();
        static Animation *createAnimation();
        static Animation *createAnimation(uint8_t *program);
        static Animation *createAnimation(const uint8_t *program);
        static void addAnimation(Animation *anim);
};
#endif
