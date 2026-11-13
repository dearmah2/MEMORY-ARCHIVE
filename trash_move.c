#include "memory.h"

/*
 * [3주차] BAD 기억 -> TRASH BIN 이동
 *
 *  - BAD ARCHIVE 목록을 본 뒤 호출된다. (archive.c 의 menu_bad_archive)
 *  - 날짜 + 번호로 기억을 고른다. (번호는 날짜마다 1부터 시작하므로 날짜가 필요)
 *  - 이동 = status 를 STATUS_TRASH 로 바꾸는 것뿐이다. 행복 지수는 변하지 않는다.
 */

void move_bad_to_trash(void)
{
    if (count_archive(TYPE_BAD) == 0) {
        printf("\n[알림] 휴지통으로 이동할 BAD 기억이 없습니다.\n");
        return;
    }

    char date[DATE_LEN];

    while (1) {
        if (!ask_date(date, "\n휴지통으로 보낼 기억의 날짜를 입력하세요")) {
            printf("이동을 마칩니다.\n");
            return;
        }

        if (count_by_date(date) == 0) {
            printf("[알림] %s 에 기록된 기억이 없습니다.\n", date);
            continue;
        }

        printf("기억 번호를 입력하세요 > ");
        int seq = read_int();

        int idx = find_memory(date, seq);
        if (idx < 0) {
            printf("[알림] %s 의 [%d]번 기억이 없습니다.\n", date, seq);
            continue;
        }

        Memory* m = &memories[idx];

        if (m->type != TYPE_BAD) {
            printf("[알림] GOOD 기억은 휴지통으로 보낼 수 없습니다. BAD 기억만 선택해 주세요.\n");
            continue;
        }
        if (m->status == STATUS_TRASH) {
            printf("[알림] 이미 휴지통에 있는 기억입니다.\n");
            continue;
        }

        printf("\n선택한 기억: %s BAD [%d] %s\n", m->date, m->seq, m->content);
        if (!ask_yes_no("이 기억을 휴지통으로 보낼까요?")) {
            printf("취소했습니다.\n");
            continue;
        }

        m->status = STATUS_TRASH;   /* 이동 = 상태 변경만 (행복 지수 변화 없음) */
        save_data();
        printf("-> TRASH BIN 으로 이동했습니다. (행복 지수는 변하지 않습니다)\n");

        if (count_archive(TYPE_BAD) == 0) {
            printf("[알림] 더 이상 이동할 BAD 기억이 없습니다.\n");
            return;
        }
        if (!ask_yes_no("\n다른 기억도 휴지통으로 보낼까요?")) return;
    }
}