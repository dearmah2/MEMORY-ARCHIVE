#include "memory.h"

/* 전역 데이터 정의 */
Memory memories[MAX_MEMORY];
int    memory_count = 0;
int    next_id      = 1;
int    happiness    = 0;

/*
 * 파일 형식 (텍스트)
 *   1행: 행복지수|다음ID|기억개수
 *   2행~: id|날짜|순서|분류|상태|내용
 */
int save_data(void)
{
    FILE *fp = fopen(DATA_FILE, "w");
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
    FILE *fp = fopen(DATA_FILE, "r");
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
            m.type   = (MemoryType)type;
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
    char *end;
    long v = strtol(buf, &end, 10);
    if (end == buf) return -1;
    return (int)v;
}

/* 공백 포함 한 줄 입력 (끝의 줄바꿈 제거) */
void read_line(char *buf, int size)
{
    if (fgets(buf, size, stdin) == NULL) { buf[0] = '\0'; return; }
    buf[strcspn(buf, "\r\n")] = '\0';
}
