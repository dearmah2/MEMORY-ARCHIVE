#include "memory.h"

/*
 * [2주차] GOOD ARCHIVE / BAD ARCHIVE 기본 조회
 *
 * - status 가 ARCHIVE 이고 type 이 일치하는 기억만 출력한다. (TRASH 는 제외)
 * - 출력 순서: 날짜 오름차순 -> 같은 날짜 안에서는 seq(입력 순서) 오름차순
 * - 번호는 GOOD/BAD 를 구분하지 않는 날짜별 통합 번호(seq)를 그대로 보여 준다.
 *   (예: GOOD ARCHIVE 에 [1], [3] 이 나오면 [2] 는 같은 날짜의 BAD 기억이라는 뜻)
 */

 /* a 가 b 보다 앞에 와야 하면 1 : 날짜 -> seq 순 */
static int comes_before(const Memory* a, const Memory* b)
{
    int cmp = strcmp(a->date, b->date);
    if (cmp != 0) return cmp < 0;
    return a->seq < b->seq;
}

/* 조건에 맞는 기억의 index 를 idx[] 에 모아 정렬하고 개수를 반환 */
int collect_sorted(MemoryType type, MemoryStatus status, int* idx)
{
    int n = 0;

    for (int i = 0; i < memory_count; i++) {
        if (memories[i].type == type && memories[i].status == status) {
            idx[n++] = i;
        }
    }

    /* 삽입 정렬 (데이터가 최대 1000건이라 충분) */
    for (int i = 1; i < n; i++) {
        int key = idx[i];
        int j = i - 1;
        while (j >= 0 && comes_before(&memories[key], &memories[idx[j]])) {
            idx[j + 1] = idx[j];
            j--;
        }
        idx[j + 1] = key;
    }
    return n;
}

/* GOOD / BAD 공통 목록 출력. 출력한 건수를 반환 (검색 화면에서도 재사용) */
int show_archive(MemoryType type)
{
    const char* label = (type == TYPE_GOOD) ? "GOOD" : "BAD";
    int idx[MAX_MEMORY];
    int n = collect_sorted(type, STATUS_ARCHIVE, idx);

    printf("\n========== %s ARCHIVE ==========\n", label);

    if (n == 0) {
        printf("보관된 %s 기억이 없습니다.\n", label);
        return 0;
    }

    char current_date[DATE_LEN] = "";
    for (int k = 0; k < n; k++) {
        const Memory* m = &memories[idx[k]];

        /* 날짜가 바뀔 때마다 날짜 제목 출력 */
        if (strcmp(current_date, m->date) != 0) {
            printf("\n[%s]\n", m->date);
            strcpy(current_date, m->date);
        }
        printf("  %s [%d] %s\n", label, m->seq, m->content);
    }

    printf("\n총 %d건\n", n);
    return n;
}

void menu_good_archive(void)
{
    show_archive(TYPE_GOOD);
}

void menu_bad_archive(void)
{
    if (show_archive(TYPE_BAD) == 0) return;

    /* 3주차: 목록을 보여 준 뒤 휴지통으로 보낼지 묻는다 */
    if (ask_yes_no("\n이 중 휴지통으로 보낼 기억이 있나요?")) {
        move_bad_to_trash();
    }
}