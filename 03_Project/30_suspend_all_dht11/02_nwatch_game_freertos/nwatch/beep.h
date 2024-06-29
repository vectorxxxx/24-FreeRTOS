/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#ifndef BEEP_H_
#define BEEP_H_

void buzzer_init(void);
void buzzer_buzz(int freq, int time_ms);

#endif /* BEEP_H_ */
