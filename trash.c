#include "memory.h"

#ifdef _WIN32
#include <windows.h>
static void pause_ms(int ms) { Sleep(ms); }
#else
#include <unistd.h>
static void pause_ms(int ms) { usleep(ms * 1000); }
#endif

/*
 * TRASH BIN
 *
 * [3주차] show_trash()  : 휴지통 목록 확인
 * [4주차] 개별 처리 / 전체 비우기
 *
 * 행복 지수 규칙
 *   원인 해결 후 삭제   +1
 *   파쇄 또는 소각 후 삭제 +0
 *   정화(세탁) 후 삭제  +2
 *   휴지통 전체 비우기  +0
 *
 * 중복 반영 방지: 처리가 끝나면 기억을 배열에서 삭제하므로
 * 같은 기억으로 점수가 두 번 반영될 수 없다.
 */

 /* ---------- 목록 ---------- */

 /* 휴지통 목록 출력 (날짜 -> 번호 순). 출력한 건수를 반환 */
int show_trash(void)
{
    int idx[MAX_MEMORY];
    int n = collect_sorted(TYPE_BAD, STATUS_TRASH, idx);   /* 휴지통에는 BAD 만 들어간다 */

    printf("\n========== TRASH BIN ==========\n");

    if (n == 0) {
        printf("휴지통이 비어 있습니다.\n");
        return 0;
    }

    char current_date[DATE_LEN] = "";
    for (int k = 0; k < n; k++) {
        const Memory* m = &memories[idx[k]];

        if (strcmp(current_date, m->date) != 0) {
            printf("\n[%s]\n", m->date);
            strcpy(current_date, m->date);
        }
        printf("  BAD [%d] %s\n", m->seq, m->content);
    }

    printf("\n총 %d건\n", n);
    return n;
}

/* ---------- 삭제 ---------- */

/* 배열에서 index 번째 기억을 제거 (뒤의 기억을 앞으로 당김) */
static void delete_memory(int index)
{
    for (int i = index; i < memory_count - 1; i++) {
        memories[i] = memories[i + 1];
    }
    memory_count--;
}

/* ---------- 정화(세탁) 연출 : 다마고치 스타일 ----------
 * 작은 화면(박스) 안에서 캐릭터가 세탁 -> 놀람 -> 점프 -> 네잎클로버 발견!
 * 프레임마다 화면을 지우고 다시 그려서 움직이는 것처럼 보이게 한다.
 * (한글 콘솔에서 깨지지 않도록 ASCII 문자만 사용, 안쪽 폭 22칸)
 */

#define SCREEN_W 22
#define FRAME_LINES 8

typedef struct {
    const char* line[FRAME_LINES];
    const char* caption;       /* 화면 아래 한 줄 (ASCII) */
    int         delay_ms;
} Frame;

/* 캐릭터 몸통 부품 (매크로로 조립해서 표정만 바꿈) */
#define TOP         "      .-----.       "
#define EYES(e)     "     ( " e " )      "
#define MOUTH(m)    "     (   " m "   )     "
#define BOT         "      '-----'       "
#define FEET        "       U   U        "
#define BODY(e, m)  TOP, EYES(e), MOUTH(m), BOT, FEET

static const Frame wash_frames[] = {
    /* 1~3 세탁 */
    { { "   o   O      o    ", "      O    o        ", "   o       O   o    ", BODY(">   <", "w") },
      "WASHING...", 700 },
    { { "     '   '   '      ", "   '   '   '   '    ", "     '   '   '      ", BODY("^   ^", "o") },
      "RINSING...", 700 },
    { { "   ~  @  ~  @  ~    ", "  @  ~  @  ~  @     ", "   ~  @  ~  @  ~    ", BODY("@   @", "o") },
      "SPINNING...", 700 },
      /* 4~5 어지러움 -> 앗! */
      { { "", "         ?          ", "", BODY("o   o", "o") },
        "...?", 800 },
      { { "", "         !          ", "", BODY("O   O", "o") },
        "!?", 600 },
        /* 6 점프 */
        { { "    *          +    ", TOP, EYES("^   ^"), MOUTH("v"), BOT, FEET, "       ~~~~~        ", "" },
          "yay!", 500 },
          /* 7~9 클로버가 쑥쑥 */
          { { "", "", "         @          ", BODY("o   o", "o") },
            "something grows...", 800 },
          { { "", "        @ @         ", "         Y          ", BODY("o   o", "o") },
            "oh...?", 800 },
          { { "        @ @         ", "         X          ", "        @ @         ", BODY("O   O", "o") },
            "A CLOVER!", 900 },
            /* 10~12 기쁨 (반짝반짝 번갈아) */
            { { "     *  @ @  +      ", "         X          ", "     +  @ @  *      ", BODY("^   ^", "v") },
              "HAPPINESS +2 !", 600 },
            { { "     +  @ @  *      ", "         X          ", "     *  @ @  +      ", BODY("^   ^", "v") },
              "HAPPINESS +2 !", 600 },
            { { "     *  @ @  +      ", "         X          ", "     +  @ @  *      ", BODY("^   ^", "v") },
              "HAPPINESS +2 !", 900 },
};

/* ---------- 파쇄 연출 : 얼굴 달린 파쇄기 ----------
 * 종이(기억)가 들어가면 잘게 갈려서 조각이 쌓인다.
 */
#define MACH(e, m, ind) \
    ind ".=========.",    \
    ind "| " e " |",      \
    ind "|    " m "    |", \
    ind "'=========='"

static const Frame shred_frames[] = {
    { { "      |memory|      ", MACH("o     o", "w", "     "),
        "", "", "" },
      "INSERT MEMORY", 700 },
    { { "       |memo|       ", MACH(">     <", "o", "     "),
        "       | | | |      ", "", "" },
      "SHREDDING...", 600 },
    { { "        |m|         ", MACH(">     <", "o", "      "),
        "       | | | |      ", "       ' ' ' '      ", "" },
      "SHREDDING...", 600 },
    { { "", MACH(">     <", "o", "    "),
        "       | | | |      ", "       ' ' ' '      ", "     ~ ~ ~ ~ ~      " },
      "SHREDDING...", 500 },
    { { "", MACH(">     <", "O", "      "),
        "       | | | |      ", "     ~ ~ ~ ~ ~ ~    ", "    ~ ~ ~ ~ ~ ~ ~   " },
      "SHREDDING...", 500 },
    { { "     *        *     ", MACH("^     ^", "v", "     "),
        "", "     ~ ~ ~ ~ ~ ~    ", "    ~ ~ ~ ~ ~ ~ ~   " },
      "ALL SHREDDED!", 1000 },
};

/* ---------- 소각 연출 : 얼굴 달린 불꽃 ----------
 * 종이(기억)가 불에 타서 재가 되고 불꽃이 잦아든다.
 */
static const Frame burn_frames[] = {
    { { "      .------.      ", "      |memory|      ", "      '------'      ",
        "", "         .          ", EYES("o   o"), MOUTH("w"), BOT },
      "LIGHTING...", 700 },
    { { "      .-~--~-.      ", "      |mem~~~|      ", "      '~-~--~'      ",
        "        ) (         ", "       ( ) )        ", EYES(">   <"), MOUTH("o"), BOT },
      "BURNING...", 700 },
    { { "", "       .~--~.       ", "       '~~~~'       ",
        "       ) ( )        ", "      ( ) ) (       ", EYES(">   <"), MOUTH("O"), BOT },
      "BURNING...", 700 },
    { { "    *   .    +      ", "         ..         ", "         ::         ",
        "      ) ( ) (       ", "     ( ) ) ( )      ", EYES("^   ^"), MOUTH("v"), BOT },
      "BURNING...!", 600 },
    { { "      .   *   .     ", "         ..         ", "       .:::.        ",
        "", "         )          ", EYES("-   -"), MOUTH("w"), BOT },
      "FLAME FADES...", 800 },
    { { "", "       . : : .      ", "     ..::::::::..   ",
        "", "", EYES("-   -"), MOUTH("v"), BOT },
      "ALL BURNED!", 1000 },
};

/*
 * 화면을 지우지 않고 "제자리에서 덮어쓰기" 방식으로 프레임을 갱신한다.
 * 한 프레임이 차지하는 줄 수만큼 커서를 위로 올린 뒤 다시 그리므로,
 * 애니메이션 위쪽의 기존 내용(휴지통 목록, 질문 등)은 그대로 남는다.
 */
#define FRAME_HEIGHT 13   /* 빈 줄1 + 위 테두리1 + 화면8 + 구분선1 + 문구1 + 아래 테두리1 */

static int ansi_ok = 1;   /* 커서 이동(ANSI) 사용 가능 여부 */

static void enable_ansi(void)
{
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    ansi_ok = 0;
    if (h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode)) {
        /* 0x0004 = ENABLE_VIRTUAL_TERMINAL_PROCESSING */
        if (SetConsoleMode(h, mode | 0x0004)) ansi_ok = 1;
    }
#endif
}

static void draw_frame(const Frame* f)
{
    printf("\n  +----------------------+\n");
    for (int i = 0; i < FRAME_LINES; i++) {
        printf("  |%-*s|\n", SCREEN_W, f->line[i]);
    }
    printf("  |%-*s|\n", SCREEN_W, " - - - - - - - - - - -");

    int len = (int)strlen(f->caption);
    int pad = (SCREEN_W - len) / 2;
    printf("  |%*s%-*s|\n", pad, "", SCREEN_W - pad, f->caption);
    printf("  +----------------------+\n");
    fflush(stdout);
}

static void play_frames(const Frame* frames, int count)
{
    enable_ansi();

    for (int i = 0; i < count; i++) {
        /* 두 번째 프레임부터는 이전 프레임 위치로 올라가서 덮어쓴다 */
        if (i > 0 && ansi_ok) printf("\033[%dA", FRAME_HEIGHT);
        draw_frame(&frames[i]);
        pause_ms(frames[i].delay_ms);
    }
    printf("\n");
}

#define FRAME_COUNT(arr) ((int)(sizeof(arr) / sizeof((arr)[0])))

static void play_wash_animation(void) { play_frames(wash_frames, FRAME_COUNT(wash_frames)); }
static void play_shred_animation(void) { play_frames(shred_frames, FRAME_COUNT(shred_frames)); }
static void play_burn_animation(void) { play_frames(burn_frames, FRAME_COUNT(burn_frames)); }

/* ---------- 처리 방식 3가지 ----------
 * 반환: 처리 완료 1 / 사용자가 "아니오"로 물러남 0
 */

 /* 원인 해결: 삭제, 행복 지수 +1 */
static int process_solve(int index)
{
    if (!ask_yes_no("\n원인이 해결되었나요?")) return 0;

    delete_memory(index);
    add_happiness(1);
    printf("-> 원인이 해결된 기억을 삭제했습니다.\n");
    printf("   HAPPINESS +1 (현재 %d)\n", happiness);
    return 1;
}

/* 파쇄 / 소각: 삭제, 행복 지수 변화 없음 */
static int process_destroy(int index)
{
    if (!ask_yes_no("\n그냥 없애고 싶은가요?")) return 0;

    int way;
    while (1) {
        static const char* const ways[] = { "1. 파쇄", "2. 소각" };
        printf("어떤 방식으로 없앨까요?\n");
        print_box(NULL, ways, 2, 1);
        printf(" > ");
        way = read_int();
        if (way == 1 || way == 2) break;
        printf("[알림] 1 또는 2 를 입력해 주세요.\n");
    }

    if (way == 1) play_shred_animation();   /* 파쇄기 연출 */
    else          play_burn_animation();    /* 불꽃 연출 */

    delete_memory(index);
    printf("-> 기억이 완전히 사라졌습니다. (행복 지수 변화 없음)\n");
    return 1;
}

/* 정화(세탁): 삭제, 행복 지수 +2 */
static int process_wash(int index)
{
    if (!ask_yes_no("\n정화하고 싶은가요?")) return 0;

    play_wash_animation();   /* WASHING -> RINSING -> SPINNING -> 네잎클로버 */

    delete_memory(index);
    add_happiness(2);

    printf("-> 기억이 깨끗하게 정화되었습니다.\n");
    printf("   HAPPINESS +2 (현재 %d)\n", happiness);
    return 1;
}

/* ---------- 개별 처리 ---------- */

static void handle_one_memory(void)
{
    char date[DATE_LEN];

    while (1) {
        if (!ask_date(date, "\n처리할 기억의 날짜를 입력하세요")) return;

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
        if (memories[idx].status != STATUS_TRASH) {
            printf("[알림] 휴지통에 있는 기억이 아닙니다.\n");
            continue;
        }

        printf("\n선택한 기억: %s BAD [%d] %s\n",
            memories[idx].date, memories[idx].seq, memories[idx].content);

        /* 처리 방식 선택: 한 가지가 끝나면 저장하고 목록으로 돌아간다 */
        while (1) {
            static const char* const items[] = {
                "1. 원인이 해결되었나요?  (삭제, HAPPINESS +1)",
                "2. 그냥 없애고 싶은가요? (파쇄 / 소각, 변화 없음)",
                "3. 정화하고 싶은가요?    (세탁, HAPPINESS +2)",
                "0. 다른 기억 선택"
            };
            printf("\n이 기억을 어떻게 할까요?\n");
            print_box(NULL, items, 4, 0);
            printf("선택 > ");

            int choice = read_int();
            int done = 0;

            if (choice == 0) break;
            else if (choice == 1) done = process_solve(idx);
            else if (choice == 2) done = process_destroy(idx);
            else if (choice == 3) done = process_wash(idx);
            else printf("[알림] 0~3 사이의 번호를 입력해 주세요.\n");

            if (done) {
                save_data();
                return;   /* 처리 완료 -> 휴지통 목록으로 */
            }
        }
    }
}

/* ---------- 전체 비우기 (행복 지수 변화 없음) ---------- */

static void empty_trash(void)
{
    int trash_count = 0;
    for (int i = 0; i < memory_count; i++) {
        if (memories[i].status == STATUS_TRASH) trash_count++;
    }
    if (trash_count == 0) {
        printf("\n[알림] 휴지통이 이미 비어 있습니다.\n");
        return;
    }

    printf("\n휴지통의 기억 %d건이 모두 삭제됩니다. 되돌릴 수 없습니다.", trash_count);
    if (!ask_yes_no("\n정말 비울까요?")) {
        printf("취소했습니다. 기억은 그대로 남아 있습니다.\n");
        return;
    }

    /* 뒤에서부터 지워야 index 가 밀려도 안전하다 */
    for (int i = memory_count - 1; i >= 0; i--) {
        if (memories[i].status == STATUS_TRASH) delete_memory(i);
    }
    save_data();
    printf("-> 휴지통을 비웠습니다. (행복 지수 변화 없음)\n");
}

/* ---------- 메뉴 ---------- */

void menu_trash(void)
{
    while (1) {
        if (show_trash() == 0) return;   /* 비어 있으면 바로 메인 메뉴로 */

        static const char* const items[] = {
            "1. 기억 하나 처리하기", "2. 휴지통 전체 비우기", "0. 돌아가기"
        };
        printf("\n");
        print_box(NULL, items, 3, 0);
        printf("선택 > ");

        switch (read_int()) {
        case 1: handle_one_memory(); break;
        case 2: empty_trash();       break;
        case 0: return;
        default: printf("[알림] 0~2 사이의 번호를 입력해 주세요.\n");
        }
    }
}