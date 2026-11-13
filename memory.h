/*
 * memory.h - 모든 .c 파일이 공유하는 헤더
 * 규칙: 각 .c 파일의 맨 첫 줄에서 #include "memory.h" 를 할 것
 *       (Visual Studio용 _CRT_SECURE_NO_WARNINGS 가 다른 include보다 먼저 정의돼야 함)
 */
#ifndef MEMORY_H
#define MEMORY_H

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MEMORY   1000
#define DATE_LEN     11     /* "YYYY-MM-DD" + '\0' */
#define CONTENT_LEN  256
#define DATA_FILE    "memory_data.txt"

typedef enum { TYPE_GOOD = 0, TYPE_BAD = 1 } MemoryType;
typedef enum { STATUS_ARCHIVE = 0, STATUS_TRASH = 1 } MemoryStatus;

typedef struct {
    int          id;                   /* 고유 ID (삭제해도 재사용 안 함) */
    char         date[DATE_LEN];       /* 기록 날짜 */
    int          seq;                  /* 같은 날짜 안의 입력 순서 */
    char         content[CONTENT_LEN]; /* 기억 내용 */
    MemoryType   type;                 /* GOOD / BAD */
    MemoryStatus status;               /* ARCHIVE / TRASH */
} Memory;

/* ---- 전역 데이터 (실제 정의는 data.c) ---- */
extern Memory memories[MAX_MEMORY];
extern int    memory_count;
extern int    next_id;
extern int    happiness;

/* ---- data.c : 공통 데이터/입력 처리 (1주차) ---- */
int  load_data(void);
int  save_data(void);
int  read_int(void);
void read_line(char *buf, int size);   /* 공백 포함 한 줄 입력 */

/* ---- record.c : RECORD (2주차) ---- */
void menu_record(void);

/* ---- archive.c : GOOD / BAD ARCHIVE (2주차) ---- */
void menu_good_archive(void);
void menu_bad_archive(void);

/* ---- search.c : MEMORY SEARCH (3주차) ---- */
void menu_search(void);

/* ---- trash_move.c : BAD -> TRASH 이동 (3주차) ---- */
void move_bad_to_trash(void);

/* ---- trash.c : TRASH BIN 처리 (4주차) ---- */
void menu_trash(void);

/* ---- happiness.c : HAPPINESS INDEX (4주차) ---- */
void add_happiness(int delta);   /* 점수 변경은 반드시 이 함수로만 */
void menu_happiness(void);

#endif
