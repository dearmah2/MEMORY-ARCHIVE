#include "memory.h"

/*
 * [2주차] GOOD ARCHIVE / BAD ARCHIVE 기본 조회
 *
 * TODO 목록
 *  1) 상태가 ARCHIVE 이고 type 이 GOOD 인 기억만 출력 (번호 + 내용)
 *  2) BAD ARCHIVE 도 같은 방식 (TRASH 상태는 출력하지 않음)
 *  3) 비어 있으면 "기억이 없습니다" 출력
 *  4) 공통 출력 함수 print_archive(MemoryType) 하나로 GOOD/BAD 처리
 *  5) (3주차) BAD ARCHIVE 에서 move_bad_to_trash() 호출 연결
 */

static void print_archive(MemoryType type)
{
    (void)type;
    /* TODO */
}

void menu_good_archive(void)
{
    (void)print_archive;
    printf("\n[GOOD ARCHIVE] 2주차에 구현 예정\n");
}

void menu_bad_archive(void)
{
    printf("\n[BAD ARCHIVE] 2주차에 구현 예정\n");
    /* TODO(3주차): 목록 출력 후 move_bad_to_trash() 연결 */
}
