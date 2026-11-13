#include "memory.h"

/*
 * [4주차] TRASH BIN - 개별 처리 / 전체 비우기
 *
 * 행복 지수 규칙
 *   원인 해결 +1 / 파쇄·소각 +0 / 정화(세탁) +2 / 전체 비우기 +0
 *
 * TODO 목록
 *  1) TRASH 상태 기억 목록 출력, 번호 선택, 내용 확인
 *  2) process_solve()  : "원인이 해결되었나요?" -> 삭제, add_happiness(1)
 *  3) process_destroy(): "그냥 없애고 싶은가요?" -> SHREDDING.../BURNING... -> 삭제, +0
 *  4) process_wash()   : WASHING... RINSING... SPINNING... -> 네잎클로버, HAPPINESS +2 -> 삭제
 *  5) empty_trash()    : 확인 메시지 -> 전체 삭제 (점수 변화 없음, 취소 시 유지)
 *  6) 삭제는 delete_memory(index) 하나로 통일 (배열 당기기), 처리 후 save_data()
 */

static void delete_memory(int index)
{
    (void)index;
    /* TODO: memories 배열에서 index 제거, memory_count 감소 */
}

static void process_solve(int index)   { (void)index; (void)delete_memory; /* TODO */ }
static void process_destroy(int index) { (void)index; /* TODO */ }
static void process_wash(int index)    { (void)index; /* TODO */ }
static void empty_trash(void)          { /* TODO */ }

void menu_trash(void)
{
    (void)process_solve; (void)process_destroy; (void)process_wash; (void)empty_trash;
    printf("\n[TRASH BIN] 4주차에 구현 예정\n");
}
