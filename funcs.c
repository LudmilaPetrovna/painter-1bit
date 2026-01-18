/* ============================================================
   Простые текстовые функции
   ============================================================ */

/* ID команд меню */
enum{
CMD_FUNC1 = 1,
CMD_FUNC2,
CMD_FUNC3
};


static void function_1(HWND hwnd);
static void function_2(HWND hwnd);


/* ============================================================
   Реализация функций
   ============================================================ */

static void
function_1(HWND hwnd)
{
    MessageBox(
        hwnd,
        L"Это функция 1",
        L"Function 1",
        MB_OK | MB_ICONINFORMATION
    );
    if (g_hwnd_status) {
        SendMessageW(g_hwnd_status, SB_SETTEXTW, 0, (LPARAM)L"Функция 1 выполнена");
    }
}

static void
function_2(HWND hwnd)
{
    MessageBox(
        hwnd,
        L"Это функция 2",
        L"Function 2",
        MB_OK | MB_ICONINFORMATION
    );

    if (g_hwnd_status) {
        SendMessageW(g_hwnd_status, SB_SETTEXTW, 1, (LPARAM)L"Функция 2 выполнена");
    }

}



