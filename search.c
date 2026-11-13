#include "memory.h"

/*
 * [3주차] MEMORY SEARCH - 분류별 / 날짜별 검색
 *
 *  1. GOOD 기억 전체 조회  -> 목록 출력 후, 날짜+번호로 상세 내용 확인
 *  2. BAD  기억 전체 조회  -> 위와 동일
 *  3. 날짜별 검색          -> 해당 날짜의 GOOD/BAD 를 입력 순서(seq)대로 함께 출력
 *
 * 번호(seq)는 날짜마다 1부터 다시 시작하므로, 상세 확인은 "날짜 + 번호"로 찾는다.
 *
 * (count_archive / count_by_date / find_memory 는 data.c 의 공용 함수를 사용한다)
 */

 /* 날짜 입력 (형식 검증 포함). 성공 1 / 0 입력으로 취소 0 */
static int input_date_for_search(char* date, const char* prompt)
{
    char buf[64];
    while (1) {
        printf("%s (취소: 0) \n > ", prompt);
        read_line(buf, sizeof(buf));

        if (strcmp(buf, "0") == 0) return 0;

        if (is_valid_date(buf)) {
            strcpy(date, buf);
            return 1;
        }
        printf("[알림] 날짜 형식이 올바르지 않습니다. YYYY-MM-DD 로 입력해 주세요.\n");
    }
}

/* 기억 1건의 상세 내용 출력 */
static void print_detail(const Memory* m)
{
    printf("\n---------- 기억 상세 ----------\n");
    printf(" ID     : %d\n", m->id);
    printf(" 날짜   : %s\n", m->date);
    printf(" 번호   : %d\n", m->seq);
    printf(" 분류   : %s\n", m->type == TYPE_GOOD ? "GOOD" : "BAD");
    printf(" 상태   : %s\n", m->status == STATUS_ARCHIVE ? "ARCHIVE" : "TRASH BIN");
    printf(" 내용   : %s\n", m->content);
    printf("-------------------------------\n");
}

/* 분류별 전체 조회 + 상세 확인 */
static void search_by_type(MemoryType type)
{
    /* 목록 출력은 archive.c 의 show_archive() 를 재사용 (휴지통 이동 질문 없이 목록만) */
    show_archive(type);

    if (count_archive(type) == 0) return;   /* 비어 있으면 상세 확인 불필요 */

    char date[DATE_LEN];
    while (1) {
        printf("\n상세 내용을 볼 기억의 날짜를 입력하세요");
        if (!input_date_for_search(date, "")) return;

        /* 번호를 묻기 전에, 그 날짜에 기록 자체가 있는지부터 확인 */
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
        /* 이 화면은 선택한 분류의 목록이므로 분류가 다르면 안내 */
        if (memories[idx].type != type || memories[idx].status != STATUS_ARCHIVE) {
            printf("[알림] 현재 목록(%s ARCHIVE)에 있는 기억이 아닙니다.\n",
                type == TYPE_GOOD ? "GOOD" : "BAD");
            continue;
        }
        print_detail(&memories[idx]);
    }
}

/* 날짜별 검색: GOOD/BAD 함께, seq 순서대로 */
static void search_by_date(void)
{
    char date[DATE_LEN];
    if (!input_date_for_search(date, "검색할 날짜를 입력하세요 \n ex) 2025-11-10 ")) return;

    int idx[MAX_MEMORY];
    int n = 0;
    for (int i = 0; i < memory_count; i++) {
        if (strcmp(memories[i].date, date) == 0) idx[n++] = i;
    }

    if (n == 0) {
        printf("\n[알림] %s 에 기록된 기억이 없습니다.\n", date);
        return;
    }

    /* seq 오름차순 정렬 (삽입 정렬) */
    for (int i = 1; i < n; i++) {
        int key = idx[i];
        int j = i - 1;
        while (j >= 0 && memories[idx[j]].seq > memories[key].seq) {
            idx[j + 1] = idx[j];
            j--;
        }
        idx[j + 1] = key;
    }

    printf("\n========== %s 의 기억 ==========\n", date);
    for (int k = 0; k < n; k++) {
        const Memory* m = &memories[idx[k]];
        printf("  %s [%d] %s%s\n",
            m->type == TYPE_GOOD ? "GOOD" : "BAD ",
            m->seq, m->content,
            m->status == STATUS_TRASH ? "  (휴지통)" : "");
    }
    printf("\n총 %d건\n", n);
}

void menu_search(void)
{
    while (1) {
        printf("\n========== MEMORY SEARCH ==========\n");
        printf(" 1. GOOD 기억 전체 조회\n");
        printf(" 2. BAD 기억 전체 조회\n");
        printf(" 3. 날짜별 검색\n");
        printf(" 0. 돌아가기\n");
        printf("선택 > ");

        switch (read_int()) {
        case 1: search_by_type(TYPE_GOOD); break;
        case 2: search_by_type(TYPE_BAD);  break;
        case 3: search_by_date();          break;
        case 0: return;
        default: printf("[알림] 0~3 사이의 번호를 입력해 주세요.\n");
        }
    }
}