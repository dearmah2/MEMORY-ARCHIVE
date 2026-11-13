#include "memory.h"

/*
 * [3주차] MEMORY SEARCH - 분류별 / 날짜별 검색
 *
 * TODO 목록
 *  1) 하위 메뉴: 1.GOOD 전체  2.BAD 전체  3.날짜별 검색  0.돌아가기
 *  2) 분류별 조회 후 번호로 상세 내용 확인
 *  3) 날짜 입력 -> 해당 날짜 기억을 seq 순서로 GOOD/BAD 함께 출력
 *  4) 존재하지 않는 날짜, 잘못된 번호 예외 처리
 */

static void search_by_type(MemoryType type)
{
    (void)type;
    /* TODO */
}

static void search_by_date(void)
{
    /* TODO */
}

void menu_search(void)
{
    (void)search_by_type; (void)search_by_date;
    printf("\n[MEMORY SEARCH] 3주차에 구현 예정\n");
}
