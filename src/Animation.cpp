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
LIBusMaster *Animation::_libus = NULL;
I2S *Animation::_i2s = NULL;
PCM1774 *Animation::_dac = NULL;
TPA2016 *Animation::_amp = NULL;

Animation::Animation() {
    for (int i = 0; i < 4; i++) { _sound[i] = -1; }
}

Animation::Animation(const uint8_t *prog) {
    for (int i = 0; i < 4; i++) { _sound[i] = -1; }
    setAnimation(prog);
}

Animation *Animation::createAnimation() {
    return createAnimation((uint8_t *)NULL);
}

Animation *Animation::createAnimation(const uint8_t *prog) {
    Animation *anim = new Animation();
    anim->setAnimation(prog);
    addAnimation(anim);
    return(anim);
}

bool Animation::execute() {
    uint8_t value;
    uint8_t value2;
    uint8_t hb, lb;
    uint8_t speed;

    uint32_t address;
    uint8_t command;
    uint8_t qty;

    uint32_t now = millis();

    char filename[128];

    if (_prog == NULL) {
        return false;
    }

    if (_state == sSTOP) {
        return false;
    }

    if (_state == sDELAY) {
        if (now - _delayStart < _delay) {
            return true;
        }
        _state = sRUN;
    }

    uint8_t instruction = _prog[_pc++];

    switch (instruction) {
        case SEND: // Send bytes to a LIBus device
            address = _prog[_pc++];
            address |= _prog[_pc++] << 8;
            address |= _prog[_pc++] << 16;

            command = _prog[_pc++];
            qty = _prog[_pc++];
            
            _libus->send(address, command, &_prog[_pc], qty);
            _pc += qty;
            break;

        case SOUND_SAMPLERATE: {
            uint32_t rate = _prog[_pc++];
            rate |= _prog[_pc++] << 8;
            rate |= _prog[_pc++] << 16;
            rate |= _prog[_pc++] << 24;
            _i2s->end();
            _i2s->setSampleRate(rate);
            _i2s->begin();
        };

        case PLAY_STEREO: {
            value = _prog[_pc++] & 0x03;
            value2 = _prog[_pc++];


            int i = 0;
            char c;
            do {
                c = _prog[_pc++];
                filename[i++] = c;

            } while ((i < 128) && (c != 0));
            if (_sound[value] >= 0) {
                if (_i2s->isPlaying(_sound[value])) {
                    _i2s->stop(_sound[value]);
                }
                _sound[value] = -1;
                if (_soundfile[value]) {
                    _soundfile[value].fsclose();
                }
            }

            FRESULT fr = _soundfile[value].fsopen(filename, FA_READ);
            if (fr == FR_OK) {
                _soundfile[value].fslseek(44);
                _i2s->playStereo(_soundfile[value], (float)value2 / 100.0, 1.0, 44);
            }
        }
        break;

        case PLAY_MONO_LEFT: {
            value = _prog[_pc++] & 0x03;
            value2 = _prog[_pc++];


            int i = 0;
            char c;
            do {
                c = _prog[_pc++];
                filename[i++] = c;

            } while ((i < 128) && (c != 0));
            if (_sound[value] >= 0) {
                if (_i2s->isPlaying(_sound[value])) {
                    _i2s->stop(_sound[value]);
                }
                _sound[value] = -1;
                if (_soundfile[value]) {
                    _soundfile[value].fsclose();
                }
            }

            FRESULT fr = _soundfile[value].fsopen(filename, FA_READ);
            if (fr == FR_OK) {
                _soundfile[value].fslseek(44);
                _i2s->playMonoLeft(_soundfile[value], (float)value2 / 100.0, 1.0, 44);
            }
        }
        break;

        case PLAY_MONO_RIGHT: {
            value = _prog[_pc++] & 0x03;
            value2 = _prog[_pc++];


            int i = 0;
            char c;
            do {
                c = _prog[_pc++];
                filename[i++] = c;

            } while ((i < 128) && (c != 0));
            if (_sound[value] >= 0) {
                if (_i2s->isPlaying(_sound[value])) {
                    _i2s->stop(_sound[value]);
                }
                _sound[value] = -1;
                if (_soundfile[value]) {
                    _soundfile[value].fsclose();
                }
            }

            FRESULT fr = _soundfile[value].fsopen(filename, FA_READ);
            if (fr == FR_OK) {
                _soundfile[value].fslseek(44);
                _i2s->playMonoRight(_soundfile[value], (float)value2 / 100.0, 1.0, 44);
            }
        }
        break;

        case PLAY_MONO_BOTH: {
            value = _prog[_pc++] & 0x03;
            value2 = _prog[_pc++];


            int i = 0;
            char c;
            do {
                c = _prog[_pc++];
                filename[i++] = c;

            } while ((i < 128) && (c != 0));
            if (_sound[value] >= 0) {
                if (_i2s->isPlaying(_sound[value])) {
                    _i2s->stop(_sound[value]);
                }
                _sound[value] = -1;
                if (_soundfile[value]) {
                    _soundfile[value].fsclose();
                }
            }

            FRESULT fr = _soundfile[value].fsopen(filename, FA_READ);
            if (fr == FR_OK) {
                _soundfile[value].fslseek(44);
                _i2s->playMono(_soundfile[value], (float)value2 / 100.0, 1.0, 44);
            }
        }
        break;

        case PLAY_STOP: {
            value = _prog[_pc++] & 0x03;
            if (_sound[value] >= 0) {
                if (_i2s->isPlaying(_sound[value])) {
                    _i2s->stop(_sound[value]);
                }
                _sound[value] = -1;
                if (_soundfile[value]) {
                    _soundfile[value].fsclose();
                }
            }
        }
        break;

        case SOUND_VOLUME:
            value = _prog[_pc++];
            value2 = _prog[_pc++];
            _dac->setVolume(value, value2);
            break;
        case SOUND_DGAIN:
            _dac->setGain(_prog[_pc++]);
            break;
        case SOUND_AGAIN:
            _amp->setGain(_prog[_pc++]);
            break;
        case SOUND_BASS:
            _dac->setBass(0 - _prog[_pc++]);
            break;
        case SOUND_MID:
            _dac->setMid(0 - _prog[_pc++]);
            break;
        case SOUND_TREBLE:
            _dac->setTreble(0 - _prog[_pc++]);
            break;
        case SOUND_3D:
            value = _prog[_pc++];
            value2 = _prog[_pc++];
            switch (value) {
                case 0: _dac->disable3D(); break;
                case 1: _dac->enable3D(); _dac->setNarrow3D(); break;
                case 2: _dac->enable3D(); _dac->setWide3D(); break;
            }
            _dac->set3DDepth(value2);
            break;



        case DELAY:
            lb = _prog[_pc++];
            hb = _prog[_pc++];
            _delay = (hb << 8) | lb;
            _delayStart = now;
            _state = sDELAY;
            break;
        case RDELAY:
            lb = _prog[_pc++];
            hb = _prog[_pc++];
            value = (hb << 8) | lb;

            lb = _prog[_pc++];
            hb = _prog[_pc++];
            value2 = (hb << 8) | lb;

            _delay = random(value, value2);
            _delayStart = now;
            _state = sDELAY;
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
            return false;
            break;
    }
    return true;
}

void Animation::setAnimation(const uint8_t *anim) {
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

void Animation::nudge() {
    _forever = false;
}
