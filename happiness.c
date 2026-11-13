#include "memory.h"

/*
 * [4주차] HAPPINESS INDEX
 * 점수 변경은 add_happiness() 로만 한다 (중복 반영 방지, 한곳에서 관리)
 */

void add_happiness(int delta)
{
    happiness += delta;
}

void menu_happiness(void)
{
    printf("\n[HAPPINESS INDEX] 현재 행복 지수: %d\n", happiness);
}
