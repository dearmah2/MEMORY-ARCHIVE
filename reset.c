#include "memory.h"

/*
 * RESET ALL - 모든 기억과 행복 지수를 지우고 처음 상태로 되돌린다.
 *
 * 되돌릴 수 없는 기능이라서 확인을 두 번 받는다.
 *   1) 예/아니오 질문
 *   2) 직접 "RESET" 을 입력
 */

void menu_reset(void)
{
    int good = 0, bad = 0, trash = 0;
    for (int i = 0; i < memory_count; i++) {
        if (memories[i].status == STATUS_TRASH)    trash++;
        else if (memories[i].type == TYPE_GOOD)    good++;
        else                                       bad++;
    }

    printf("\n========== RESET ALL ==========\n");

    if (memory_count == 0 && happiness == 0) {
        printf("이미 초기화된 상태입니다. 지울 데이터가 없습니다.\n");
        return;
    }

    printf("삭제될 데이터\n");
    printf("  GOOD 기억      : %d건\n", good);
    printf("  BAD 기억       : %d건\n", bad);
    printf("  휴지통 기억    : %d건\n", trash);
    printf("  행복 지수      : %d\n", happiness);

    /* 1단계 확인 */
    if (!ask_yes_no("\n모든 기억과 행복 지수를 지우고 처음 상태로 되돌릴까요?\n되돌릴 수 없습니다.")) {
        printf("취소했습니다. 데이터는 그대로입니다.\n");
        return;
    }

    /* 2단계 확인: 직접 입력 */
    char buf[64];
    printf("정말 지우려면 RESET 을 입력해 주세요. (취소: 0)\n > ");
    read_line(buf, sizeof(buf));
    if (strcmp(buf, "RESET") != 0) {
        printf("입력이 달라서 취소했습니다. 데이터는 그대로입니다.\n");
        return;
    }

    /* 초기화 */
    memset(memories, 0, sizeof(memories));
    memory_count = 0;
    next_id      = 1;
    happiness    = 0;
    save_data();

    printf("-> 모든 데이터를 초기화했습니다. (기억 0건, 행복 지수 0)\n");
}
