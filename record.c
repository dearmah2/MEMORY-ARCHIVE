#include "memory.h"

/*
 * [2주차] RECORD - 날짜/기억 입력, GOOD/BAD 분류, 날짜별 통합 순서 번호
 *
 * TODO 목록
 *  1) 날짜 입력 + 형식 확인 (YYYY-MM-DD)           -> input_date()
 *  2) 같은 날짜의 다음 순서 번호 계산               -> next_seq_for_date()
 *  3) 기억 내용 입력 + GOOD/BAD 선택                -> 내용은 read_line() 사용
 *  4) 한 건 저장 (id = next_id++, status = ARCHIVE)  -> add_memory()
 *  5) 추가 입력 여부 확인 후 반복
 *  6) GOOD 기록 시 add_happiness(+1), BAD는 점수 변화 없음
 *  7) 저장: save_data()
 */

/* 날짜 입력 후 date에 저장. 성공 1 / 실패 0 */
static int input_date(char *date)
{
    (void)date;
    /* TODO */
    return 0;
}

/* 해당 날짜에 이미 입력된 기억 수 + 1 (GOOD/BAD 구분 없이 통합) */
static int next_seq_for_date(const char *date)
{
    (void)date;
    /* TODO */
    return 1;
}

/* 기억 1건 추가. 성공 1 / 실패 0 */
static int add_memory(const char *date, const char *content, MemoryType type)
{
    (void)date; (void)content; (void)type;
    (void)next_seq_for_date;
    /* TODO */
    return 0;
}

void menu_record(void)
{
    (void)input_date; (void)add_memory;
    printf("\n[RECORD] 2주차에 구현 예정\n");
}
