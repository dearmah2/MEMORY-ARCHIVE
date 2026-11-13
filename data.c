#include "memory.h"
#include <stdarg.h>

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

/* ---------- 출력 간격 처리 ----------
 * read_int / read_line 으로 입력을 받으면 blank_pending 을 켜 둔다.
 * 다음에 출력할 때, 문장이 줄바꿈으로 시작하지 않으면 빈 줄 한 줄을 먼저 넣는다.
 * (이미 "\n" 으로 시작하는 문장은 그대로 두므로 빈 줄이 두 번 생기지 않는다)
 */
static int blank_pending = 0;

int out_printf(const char* fmt, ...)
{
    char buf[2048];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    if (blank_pending) {
        blank_pending = 0;
        if (buf[0] != '\n') fputc('\n', stdout);
    }
    fputs(buf, stdout);
    return n;
}

/* 숫자 한 개 입력. 숫자가 아니면 -1 반환 */
int read_int(void)
{
    char buf[64];
    if (fgets(buf, sizeof(buf), stdin) == NULL) return -1;
    blank_pending = 1;
    char* end;
    long v = strtol(buf, &end, 10);
    if (end == buf) return -1;
    return (int)v;
}

/* 공백 포함 한 줄 입력 (끝의 줄바꿈 제거) */
void read_line(char* buf, int size)
{
    if (fgets(buf, size, stdin) == NULL) { buf[0] = '\0'; return; }
    blank_pending = 1;
    buf[strcspn(buf, "\r\n")] = '\0';
}


/* ---------- 선택지 박스 ----------
 * 한글은 콘솔에서 2칸을 차지하므로, 글자 폭을 직접 계산해서 오른쪽 테두리를 맞춘다.
 * 소스가 UTF-8 이든 CP949(Visual Studio 기본) 이든 둘 다 처리한다.
 */
static int is_utf8_string(const char* s)
{
    const unsigned char* p = (const unsigned char*)s;
    while (*p) {
        int extra;
        if (*p < 0x80)              extra = 0;
        else if ((*p & 0xE0) == 0xC0) extra = 1;
        else if ((*p & 0xF0) == 0xE0) extra = 2;
        else if ((*p & 0xF8) == 0xF0) extra = 3;
        else return 0;
        p++;
        for (int i = 0; i < extra; i++, p++) {
            if ((*p & 0xC0) != 0x80) return 0;
        }
    }
    return 1;
}

static int display_width(const char* s)
{
    const unsigned char* p = (const unsigned char*)s;
    int w = 0;

    if (is_utf8_string(s)) {
        while (*p) {
            unsigned int cp;
            int len;
            if (*p < 0x80) { cp = *p;        len = 1; }
            else if ((*p & 0xE0) == 0xC0) { cp = *p & 0x1F; len = 2; }
            else if ((*p & 0xF0) == 0xE0) { cp = *p & 0x0F; len = 3; }
            else { cp = *p & 0x07; len = 4; }
            for (int i = 1; i < len; i++) cp = (cp << 6) | (p[i] & 0x3F);
            p += len;

            /* 한글/한자/전각 문자는 2칸 */
            if ((cp >= 0x1100 && cp <= 0x115F) || (cp >= 0x2E80 && cp <= 0xA4CF) ||
                (cp >= 0xAC00 && cp <= 0xD7A3) || (cp >= 0xF900 && cp <= 0xFAFF) ||
                (cp >= 0xFE30 && cp <= 0xFE6F) || (cp >= 0xFF00 && cp <= 0xFF60) ||
                (cp >= 0xFFE0 && cp <= 0xFFE6)) w += 2;
            else w += 1;
        }
    }
    else {
        /* CP949: 0x80 이상이면 2바이트 문자(2칸) */
        while (*p) {
            if (*p < 0x80) { w += 1; p++; }
            else if (p[1]) { w += 2; p += 2; }
            else { w += 1; p++; }
        }
    }
    return w;
}

void print_box(const char* title, const char* const* items, int n, int inline_mode)
{
    const char* rows[32];
    char joined[512] = "";
    int nrows = 0;

    if (inline_mode) {
        for (int i = 0; i < n; i++) {
            if (i > 0) strcat(joined, "   ");
            strcat(joined, items[i]);
        }
        rows[nrows++] = joined;
    }
    else {
        for (int i = 0; i < n && nrows < 32; i++) rows[nrows++] = items[i];
    }

    int w = 16;   /* 안쪽 최소 폭 */
    if (title && display_width(title) > w) w = display_width(title);
    for (int i = 0; i < nrows; i++) {
        if (display_width(rows[i]) > w) w = display_width(rows[i]);
    }

    char border[160];
    int len = w + 2;
    if (len > 150) len = 150;
    border[0] = '+';
    for (int i = 0; i < len; i++) border[1 + i] = '-';
    border[1 + len] = '+';
    border[2 + len] = '\0';

    printf(" %s\n", border);
    if (title) {
        int tw = display_width(title);
        int left = (w - tw) / 2;
        printf(" | %*s%s%*s |\n", left, "", title, w - tw - left, "");
        printf(" %s\n", border);
    }
    for (int i = 0; i < nrows; i++) {
        printf(" | %s%*s |\n", rows[i], w - display_width(rows[i]), "");
    }
    printf(" %s\n", border);
}

/* 질문 후 1.예 2.아니오 를 입력받는다. 예 1 / 아니오 0 */
int ask_yes_no(const char* question)
{
    while (1) {
        static const char* const yn[] = { "1. 예", "2. 아니오" };
        printf("%s\n", question);
        print_box(NULL, yn, 2, 1);
        printf(" > ");
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