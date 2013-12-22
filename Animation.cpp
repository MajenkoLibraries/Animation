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


#include <Animation.h>

AnimationList *Animation::_list = NULL;

Animation::Animation() {
}

Animation *Animation::createAnimation() {
    return createAnimation((uint8_t *)NULL);
}

Animation *Animation::createAnimation(uint8_t *prog) {
    Animation *anim = new Animation();
    anim->_fade = NULL;
    anim->setAnimation(prog);
    addAnimation(anim);
    return(anim);
}

Animation *Animation::createAnimation(const uint8_t *prog) {
    return createAnimation((uint8_t *)prog);
}

void Animation::execute() {
    uint8_t group;
    uint8_t qty;
    uint8_t value;
    uint8_t value2;
    uint8_t hb, lb;
    uint8_t speed;
    Fade *fade;

    uint32_t now = millis();

    if (_prog == NULL) {
        return;
    }

    if (_state == sSTOP) {
        return;
    }

    processFades();

    if (_state == sDELAY) {
        if (now - _delayStart < _delay) {
            return;
        }
        _state = sRUN;
    }

    if (_state == sWAIT) {
        if (_fade != NULL) {
            return;
        }
        _state = sRUN;
    }

    uint8_t instruction = _prog[_pc++];

    switch (instruction) {
        case GROUP:
            group = _prog[_pc++];
            qty = _prog[_pc++];


            // Groups are between 200 and 255.  If a number less than
            // 56 is provided, assume it is + 200.
            if (group <= 55) {
                group += 200;
            }

            // Any other group numbers are an error.  Break here.  Who
            // knows what will happen now?
            if (group < 200) {
                break;
                _state = sRUN;
            }

            group = group - 200;

            // Delete the old group, if there is one and it's not big enough.
            if (_groups[group] != NULL) {
                if (_groups[group][0] < qty) {
                    free(_groups[group]);
                    _groups[group] = (uint8_t *)malloc(qty + 1);
                }
            } else {
                _groups[group] = (uint8_t *)malloc(qty + 1);
            }

            _groups[group][0] = qty;

            // Fill the group from the animation.
            for (int i = 0; i < qty; i++) {
                _groups[group][i+1] = _prog[_pc++];
            }

            _state = sRUN;
            
            break;
        case SET:
            group = _prog[_pc++];
            value = _prog[_pc++];

            if (group < 200) {
                ULK.analogWrite(group, value);
            } else {
                if (_groups[group - 200] == NULL) {
                    break;
                    _state = sRUN;
                }
                for (int i = 0; i < _groups[group - 200][0]; i++) {
                    ULK.analogWrite(_groups[group - 200][i+1], value);
                }
            }
        
            _state = sRUN;
            break;
        case FADE:
            group = _prog[_pc++];
            value = _prog[_pc++];
            speed = _prog[_pc++];
            _state = sRUN;
            fade = (Fade *)malloc(sizeof(Fade));
            fade->group = group;
            fade->start = now;
            fade->delay = speed;
            fade->target = value;
            fade->next = NULL;
            addFade(fade);
            break;
        case DELAY:
            hb = _prog[_pc++];
            lb = _prog[_pc++];
            _delay = (hb << 8) | lb;
            _delayStart = now;
            _state = sDELAY;
            break;
        case RDELAY:
            hb = _prog[_pc++];
            lb = _prog[_pc++];
            value = (hb << 8) | lb;

            hb = _prog[_pc++];
            lb = _prog[_pc++];
            value2 = (hb << 8) | lb;

            _delay = random(value, value2);
            _delayStart = now;
            _state = sDELAY;
            break;
        case WAITEQ:
            _state = sWAIT;
            break;
        case REPEAT:
            _repeatCount = _prog[_pc++];
            _repeatPC = _pc;
            break;
        case FOREVER:
            _forever = true;
            _repeatPC = _pc;
            break;
        case LOOP:
            if (_forever) {
                _pc = _repeatPC;
                _state = sRUN;
                break;
            }

            if (_repeatCount > 0) {
                _repeatCount--;
                _pc = _repeatPC;
                _state = sRUN;
                break;
            } 

            if (_repeatPC != 0) {
                _repeatPC = 0;
                _state = sRUN;
                break;
            }

            _pc = 0;
            _state = sRUN;
            break;
        case END:
            _pc = 0;
            _state = sSTOP;
            break;
    }
}

void Animation::setAnimation(uint8_t *anim) {
    _repeatCount = 0;
    _repeatPC = 0;
    _pc = 0;
    _prog = anim;
    _state = sSTOP;
}

void Animation::addAnimation(Animation *anim) {
    AnimationList *scan;
    AnimationList *n = (AnimationList *)malloc(sizeof(AnimationList));
    n->anim = anim;
    n->next = NULL;

    if (_list == NULL) {
        _list = n;
        return;
    }

    for (scan = _list; scan->next; scan = scan->next);
    scan->next = n;
}

void Animation::stop() {
    _state = sSTOP;
}

void Animation::start() {
    _pc = 0;
    _state = sRUN;
}

void Animation::process() {
    AnimationList *scan;
    if (_list == NULL) {
        return;
    }
    for (scan = _list; scan; scan = scan->next) {
        scan->anim->execute();
    }
}

void Animation::addFade(Fade *f) {
    Fade *scan;
    if (_fade == NULL) {
        _fade = f;
        return;
    }

    for (scan = _fade; scan->next; scan = scan->next);
    scan->next = f;
}

void Animation::delFade(Fade *f) {
    Fade *scan;
    if (_fade == f) {
        _fade = _fade->next;
        free(f);
        return;
    }

    for (scan = _fade; scan; scan = scan->next) {
        if (scan->next == f) {
            scan->next = scan->next->next;
            free(f);
            return;
        }
    }
}

void Animation::processFades() {
    Fade *scan;
    uint32_t now = millis();

    for (scan = _fade; scan; scan = scan->next) {
        if (now - scan->start >= scan->delay) {
            scan->start = now;
            if (scan->group < 200) {
                uint8_t current = ULK.get(scan->group);
                if (current == scan->target) {
                    delFade(scan);
                    scan = _fade;
                    return;
                }
                if (current < scan->target) {
                    ULK.analogWrite(scan->group, current + 1);
                } else {
                    ULK.analogWrite(scan->group, current - 1);
                }
            } else {
                uint8_t group = scan->group - 200;
                uint8_t eq = 1;
                if (_groups[group] != NULL) {
                    for (int i = 0; i < _groups[group][0]; i++) {
                        uint8_t pin = _groups[group][i + 1];
                        uint8_t current = ULK.get(_groups[group][i + 1]);
                        if (current == scan->target) {
                            continue;
                        }
                        if (current < scan->target) {
                            ULK.analogWrite(_groups[group][i + 1], current + 1);
                            eq = 0;
                        } else {
                            ULK.analogWrite(_groups[group][i + 1], current - 1);
                            eq = 0;
                        }
                    }
                }
                if (eq == 1) {
                    delFade(scan);
                    return;
                    scan = _fade;
                    continue;
                }
            }
        }
    }
}

void Animation::nudge() {
    _forever = false;
}
