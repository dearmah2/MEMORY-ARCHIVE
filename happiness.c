#include "memory.h"

/*
 * [4주차] HAPPINESS INDEX
 * 점수 변경은 add_happiness() 로만 한다 (한 곳에서 관리)
 */

void add_happiness(int delta)
{
    happiness += delta;
}

void menu_happiness(void)
{
    printf("\n========== HAPPINESS INDEX ==========\n");
    printf(" 현재 행복 지수 : %d\n", happiness);
    printf("\n [점수 기준]\n");
    printf("  GOOD 기억 기록          +1\n");
    printf("  BAD 기억 기록           +0\n");
    printf("  원인 해결 후 삭제       +1\n");
    printf("  파쇄 / 소각 후 삭제     +0\n");
    printf("  정화(세탁) 후 삭제      +2\n");
    printf("  휴지통 전체 비우기      +0\n");
}