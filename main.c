#include "memory.h"

static void print_menu(void)
{
    static const char* const items[] = {
        "1. RECORD", "2. GOOD ARCHIVE", "3. BAD ARCHIVE", "4. MEMORY SEARCH",
        "5. TRASH BIN", "6. HAPPINESS INDEX", "7. RESET ALL", "0. EXIT"
    };
    printf("\n");
    print_box("MEMORY ARCHIVE", items, 8, 0);
    printf("선택 > ");
}

int main(void)
{
    load_data();

    int running = 1;
    while (running) {
        print_menu();
        switch (read_int()) {
        case 1: menu_record();       break;
        case 2: menu_good_archive(); break;
        case 3: menu_bad_archive();  break;
        case 4: menu_search();       break;
        case 5: menu_trash();        break;
        case 6: menu_happiness();    break;
        case 7: menu_reset();        break;
        case 0:
            save_data();
            printf("저장 후 종료합니다.\n");
            running = 0;
            break;
        default:
            printf("\n[알림] 0~7 사이의 번호를 입력해 주세요.\n");
        }
    }
    return 0;
}