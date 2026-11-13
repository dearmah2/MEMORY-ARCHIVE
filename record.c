#include "memory.h"

/*
 * [2주차] RECORD - 날짜/기억 입력, GOOD/BAD 분류, 날짜별 통합 순서 번호
 *
 * Memory 구조체 필드 채우는 방법
 *   id      : next_id++ (고유 ID, 삭제해도 재사용하지 않음)
 *   date    : 사용자가 입력한 날짜 (YYYY-MM-DD)
 *   seq     : 같은 날짜 안의 입력 순서 (GOOD/BAD 구분 없이 통합)
 *   content : 사용자가 입력한 기억 내용
 *   type    : TYPE_GOOD / TYPE_BAD
 *   status  : 새로 기록한 기억은 항상 STATUS_ARCHIVE
 */

 /* ---------- 날짜 검증 ---------- */

static int is_leap_year(int y)
{
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

/* "YYYY-MM-DD" 형식이고 실제로 존재하는 날짜면 1, 아니면 0 */
int is_valid_date(const char* s)
{
    if (strlen(s) != 10) return 0;

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            if (s[i] != '-') return 0;
        }
        else {
            if (s[i] < '0' || s[i] > '9') return 0;
        }
    }

    int y = atoi(s);          /* 앞의 숫자만 읽음: 2025 */
    int m = atoi(s + 5);      /* 05 -> 5 */
    int d = atoi(s + 8);

    if (y < 1900 || y > 2100) return 0;
    if (m < 1 || m > 12) return 0;

    int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (is_leap_year(y)) days[1] = 29;

    return d >= 1 && d <= days[m - 1];
}

/* 날짜 입력. 성공 1 / 사용자가 0을 입력해 취소 0 */
static int input_date(char* date)
{
    char buf[64];
    while (1) {
        printf("오늘의 날짜를 입력하시오... \n ex): 2025-11-10, 취소: 0 \n > ");
        read_line(buf, sizeof(buf));

        if (strcmp(buf, "0") == 0) return 0;

        if (is_valid_date(buf)) {
            strcpy(date, buf);
            return 1;
        }
        printf("[알림] 날짜 형식이 올바르지 않습니다. YYYY-MM-DD 로 입력해 주세요.\n");
    }
}

/* ---------- 순서 번호 ---------- */

/*
 * 해당 날짜의 가장 큰 seq + 1.
 * (개수 + 1 이 아니라 최댓값 + 1 을 쓰는 이유: 나중에 휴지통에서 삭제돼도
 *  번호가 겹치지 않게 하기 위함. 휴지통 상태의 기억도 번호 계산에 포함한다.)
 */
static int next_seq_for_date(const char* date)
{
    int max_seq = 0;
    for (int i = 0; i < memory_count; i++) {
        if (strcmp(memories[i].date, date) == 0 && memories[i].seq > max_seq) {
            max_seq = memories[i].seq;
        }
    }
    return max_seq + 1;
}

/* ---------- 기억 1건 추가 ---------- */

/* 성공 시 배열에 추가된 기억의 seq 반환, 실패 시 0 */
static int add_memory(const char* date, const char* content, MemoryType type)
{
    if (memory_count >= MAX_MEMORY) {
        printf("[오류] 더 이상 기억을 저장할 수 없습니다. (최대 %d건)\n", MAX_MEMORY);
        return 0;
    }

    Memory* m = &memories[memory_count];

    m->id = next_id++;
    strcpy(m->date, date);
    m->seq = next_seq_for_date(date);
    strncpy(m->content, content, CONTENT_LEN - 1);
    m->content[CONTENT_LEN - 1] = '\0';
    m->type = type;
    m->status = STATUS_ARCHIVE;

    memory_count++;

    if (type == TYPE_GOOD) {
        add_happiness(1);   /* GOOD 기록 +1, BAD 는 변화 없음 */
    }
    return m->seq;
}

/* ---------- 입력 보조 ---------- */

static int input_content(char* content)
{
    while (1) {
        printf("보관하고 싶은 기억을 기록해 주세요! \n > ");
        read_line(content, CONTENT_LEN);

        if (content[0] == '\0') {
            printf("[알림] 내용이 비어 있습니다. 다시 입력해 주세요.\n");
            continue;
        }
        /* 파일 형식의 구분자 '|' 는 공백으로 바꿔서 저장 파일이 깨지지 않게 함 */
        for (char* p = content; *p; p++) {
            if (*p == '|') *p = ' ';
        }
        return 1;
    }
}

static MemoryType input_type(void)
{
    while (1) {
        printf("어떤 기억인가요? \n 1. GOOD  2. BAD \n > ");
        int c = read_int();
        if (c == 1) return TYPE_GOOD;
        if (c == 2) return TYPE_BAD;
        printf("[알림] 1 또는 2 를 입력해 주세요.\n");
    }
}

/* 계속 입력할지 확인. 계속 1 / 종료 0 */
static int ask_continue(void)
{
    while (1) {
        printf("같은 날짜에 기억을 더 입력할까요? \n 1. 예  2. 아니오 \n > ");
        int c = read_int();
        if (c == 1) return 1;
        if (c == 2) return 0;
        printf("[알림] 1 또는 2 를 입력해 주세요.\n");
    }
}

/* ---------- 메뉴 ---------- */

void menu_record(void)
{
    char date[DATE_LEN];
    char content[CONTENT_LEN];
    int added = 0;

    printf("\n========== RECORD ==========\n");

    if (!input_date(date)) {
        printf("기록을 취소했습니다.\n");
        return;
    }

    do {
        printf("\n[%s] %d번째 기억\n", date, next_seq_for_date(date));

        input_content(content);
        MemoryType type = input_type();

        int seq = add_memory(date, content, type);
        if (seq == 0) break;   /* 저장 공간 부족 */

        added++;
        printf("-> %s [%d] 저장 완료\n", type == TYPE_GOOD ? "GOOD" : "BAD", seq);
        if (type == TYPE_GOOD) {
            printf("   HAPPINESS +1 (현재 %d)\n", happiness);
        }
    } while (ask_continue());

    if (added > 0) {
        save_data();
        printf("\n%d건의 기억을 저장했습니다.\n", added);
    }
}