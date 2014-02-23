/* ChibiOS/GFX demo - "neko"
 * A port of the classic japanese computer desktop toy (although most of
 * us know it from the Macintosh Desk Accessory that came a year later..)
 * --
 * Copyright Chris Baird <cjb@brushtail.apana.org.au> April 2013
 */

/* This file is part of stmneko.
 *
 * stmneko is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * stmndeko distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ch.h"
#include "hal.h"
#include "gfx.h"
#include <stdio.h>

/* ---------------------------------------------------------------------- */

static GHandle ghLabel1;

static void createWidgets(void)
{
    GWidgetInit wi;

    wi.customDraw = 0;
    wi.customParam = 0;
    wi.customStyle = 0;
    wi.g.show = TRUE;

    wi.g.y = 10;
    wi.g.x = 10;
    wi.g.width = 60;
    wi.g.height = 20;
    wi.text = "00000";

    ghLabel1 = gwinLabelCreate(NULL,&wi);
}

int main(void)
{

  halInit();
  chSysInit();
  gfxInit();
  char buffer[8];

  gwinSetDefaultFont(gdispOpenFont("SMALL"));
  gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
  gdispClear(White);

  stm32_tim_t *enc = STM32_TIM4;

  palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(2) | PAL_STM32_OTYPE_PUSHPULL | PAL_STM32_PUDR_PULLUP);
  palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(2) | PAL_STM32_OTYPE_PUSHPULL | PAL_STM32_PUDR_PULLUP);

  rccEnableTIM4(TRUE);

  enc->SMCR = 3;        // encoder mode 3
  enc->CCER = 0;        // rising edge polarity
  enc->ARR = 0xffff;    // count from 0-ARR or ARR-0
  enc->CCMR1 = 0xc1c1;  // f_DTS/16, N=8 IC1->TI1, IC2->TI2
  enc->CNT = 0;         // initialize counter
  enc->EGR = 1;         // generate an update event
  enc->CR1 = 1;         // enable the counter

  createWidgets();

  uint32_t counter = 0;

  while(TRUE) {
      if (counter != (enc->CNT >> 2)) {
        sprintf(buffer, "%05d", (unsigned int)enc->CNT >> 2);
        gwinSetText(ghLabel1, buffer, TRUE);
        counter = enc->CNT >> 2;
      }
  }
}
