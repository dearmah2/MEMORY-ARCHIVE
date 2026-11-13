#include "memory.h"

/* 전역 데이터 정의 */
Memory memories[MAX_MEMORY];
int    memory_count = 0;
int    next_id = 1;
int    happiness = 0;

/*
 * 파일 형식 (텍스트)
 *   1행: 행복지수|다음ID|기억개수
 *   2행~: id|날짜|순서|분류|상태|내용
 */
int save_data(void)
{
    FILE* fp = fopen(DATA_FILE, "w");
    if (fp == NULL) {
        printf("[오류] 파일을 저장할 수 없습니다.\n");
        return 0;
    }
    fprintf(fp, "%d|%d|%d\n", happiness, next_id, memory_count);
    for (int i = 0; i < memory_count; i++) {
        fprintf(fp, "%d|%s|%d|%d|%d|%s\n",
            memories[i].id, memories[i].date, memories[i].seq,
            (int)memories[i].type, (int)memories[i].status,
            memories[i].content);
    }
    fclose(fp);
    return 1;
}

int load_data(void)
{
    FILE* fp = fopen(DATA_FILE, "r");
    if (fp == NULL) return 0;   /* 첫 실행: 오류 아님 */

    int count = 0;
    if (fscanf(fp, "%d|%d|%d\n", &happiness, &next_id, &count) != 3) {
        fclose(fp);
        happiness = 0; next_id = 1; memory_count = 0;
        printf("[경고] 데이터 파일 형식이 올바르지 않아 새로 시작합니다.\n");
        return 0;
    }
    if (count > MAX_MEMORY) count = MAX_MEMORY;

    memory_count = 0;
    char line[CONTENT_LEN + 100];
    while (memory_count < count && fgets(line, sizeof(line), fp) != NULL) {
        Memory m;
        int type, status;
        if (sscanf(line, "%d|%10[^|]|%d|%d|%d|%255[^\n]",
            &m.id, m.date, &m.seq, &type, &status, m.content) == 6) {
            m.type = (MemoryType)type;
            m.status = (MemoryStatus)status;
            memories[memory_count++] = m;
        }
    }
    fclose(fp);
    return 1;
}

/* 숫자 한 개 입력. 숫자가 아니면 -1 반환 */
int read_int(void)
{
    char buf[64];
    if (fgets(buf, sizeof(buf), stdin) == NULL) return -1;
    char* end;
    long v = strtol(buf, &end, 10);
    if (end == buf) return -1;
    return (int)v;
}

/* 공백 포함 한 줄 입력 (끝의 줄바꿈 제거) */
void read_line(char* buf, int size)
{
    if (fgets(buf, size, stdin) == NULL) { buf[0] = '\0'; return; }
    buf[strcspn(buf, "\r\n")] = '\0';
}

/* 질문 후 1.예 2.아니오 를 입력받는다. 예 1 / 아니오 0 */
int ask_yes_no(const char* question)
{
    while (1) {
        printf("%s\n 1. 예  2. 아니오\n > ", question);
        int c = read_int();
        if (c == 1) return 1;
        if (c == 2) return 0;
        printf("[알림] 1 또는 2 를 입력해 주세요.\n");
    }
}

/* 날짜 입력 (형식 검증 포함). 성공 1 / 0 입력으로 취소 0 */
int ask_date(char* date, const char* prompt)
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

int count_archive(MemoryType type)
{
    int n = 0;
    for (int i = 0; i < memory_count; i++) {
        if (memories[i].type == type && memories[i].status == STATUS_ARCHIVE) n++;
    }
    return n;
}

int count_by_date(const char* date)
{
    int n = 0;
    for (int i = 0; i < memory_count; i++) {
        if (strcmp(memories[i].date, date) == 0) n++;
    }
    return n;
}

int find_memory(const char* date, int seq)
{
    for (int i = 0; i < memory_count; i++) {
        if (strcmp(memories[i].date, date) == 0 && memories[i].seq == seq) return i;
    }
    return -1;
}