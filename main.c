#define UNICODE
#define _UNICODE

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <commctrl.h>



/* ============================================================
   Константы и глобальные состояния
   ============================================================ */

static const wchar_t *WINDOW_CLASS_NAME = L"SimplePaintLikeWindow";

static const int WINDOW_DEFAULT_WIDTH  = 640;
static const int WINDOW_DEFAULT_HEIGHT = 480;

static const int WINDOW_DEFAULT_X = 1200;
static const int WINDOW_DEFAULT_Y = 500;

static HINSTANCE g_hInstance = NULL;

static HWND g_hwnd_status = NULL;
static HWND g_frame_in = NULL;
static HWND g_frame_out = NULL;

static int g_canvas_w = 320;
static int g_canvas_h = 240;
static int g_frame_gap = 20;

static int g_frame_w = 330;
static int g_frame_h = 250;

#include "splitter.c"

typedef enum {
    LAYOUT_HORIZONTAL,
    LAYOUT_VERTICAL
} layout_mode_t;

static layout_mode_t g_layout = LAYOUT_HORIZONTAL;





/* ============================================================
   Forward declarations
   ============================================================ */

static LRESULT CALLBACK
window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static ATOM
register_window_class(HINSTANCE hInstance);

static HWND
create_main_window(HINSTANCE hInstance);

#include "funcs.c"
#include "layout_frames.c"


/* ============================================================
   WinMain — точка входа
   ============================================================ */

int WINAPI
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR     lpCmdLine,
        int       nCmdShow)
{
    (void)hPrevInstance;
    (void)lpCmdLine;

    g_hInstance = hInstance;


    if (!register_window_class(hInstance)) {
        MessageBox(NULL, L"Failed to register window class", L"Error", MB_ICONERROR);
        return 1;
    }

    HWND hwnd = create_main_window(hInstance);
    if (!hwnd) {
        MessageBox(NULL, L"Failed to create window", L"Error", MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    /* ========================================================
       Message loop
       ======================================================== */

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

/* ============================================================
   Регистрация класса окна
   ============================================================ */

static ATOM
register_window_class(HINSTANCE hInstance)
{
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(wc));

    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = window_proc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    return RegisterClassEx(&wc);
}

/* ============================================================
   Создание главного окна
   ============================================================ */

static HWND
create_main_window(HINSTANCE hInstance)
{
    DWORD style = WS_OVERLAPPEDWINDOW;

    RECT rect = {
        0,
        0,
        WINDOW_DEFAULT_WIDTH,
        WINDOW_DEFAULT_HEIGHT
    };

    /* Подгоняем размеры под стиль окна */
    AdjustWindowRect(&rect, style, FALSE);

    int width  = rect.right  - rect.left;
    int height = rect.bottom - rect.top;

    HWND hwnd = CreateWindowEx(
        0,
        WINDOW_CLASS_NAME,
        L"Minimal Paint-like App (Win32)",
        style,
        WINDOW_DEFAULT_X,
        WINDOW_DEFAULT_Y,
        width,
        height,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    return hwnd;
}

/* ============================================================
   Процедура окна
   ============================================================ */

static LRESULT CALLBACK
window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {

        case WM_CREATE:
        {
            /* Создание простого меню */
            HMENU hMenu = CreateMenu();

            AppendMenu(hMenu, MF_STRING, CMD_FUNC1, L"Функция 1");
            AppendMenu(hMenu, MF_STRING, CMD_FUNC2, L"Функция 2");
            AppendMenu(hMenu, MF_STRING, CMD_FUNC3, L"Добавить окошко");
            SetMenu(hwnd, hMenu);

            /* Создание статус-бара */
            g_hwnd_status = CreateWindowExW(
                0,
                STATUSCLASSNAMEW,
                NULL,
                WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
                0, 0, 0, 0,
                hwnd,
                NULL,
                g_hInstance,
                NULL
            );

            /* Три секции */
            int parts[3] = { 200, 400, -1 };
            SendMessageW(g_hwnd_status, SB_SETPARTS, 3, (LPARAM)parts);

            SendMessageW(g_hwnd_status, SB_SETTEXTW, 0, (LPARAM)L"Готово");
            SendMessageW(g_hwnd_status, SB_SETTEXTW, 1, (LPARAM)L"");
            SendMessageW(g_hwnd_status, SB_SETTEXTW, 2, (LPARAM)L"");

            /* ----------------------------------------------------
               Создание рамочек под изображения
               ---------------------------------------------------- */

    static BOOL class_registered = FALSE;

    if (!class_registered) {
        WNDCLASS wc = {0};
        wc.lpfnWndProc   = frame_wndproc;
        wc.hInstance     = GetModuleHandle(NULL);
        wc.lpszClassName = FRAME_CLASS;
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        RegisterClass(&wc);
        class_registered = TRUE;
    }


            g_frame_in = CreateWindowEx(
                WS_EX_CLIENTEDGE,
            FRAME_CLASS,
                NULL,
                WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL,
                0,0,0,0,
                hwnd,
                NULL,
                g_hInstance,
                NULL
            );

            g_frame_out = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                FRAME_CLASS,
                NULL,
                WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL,
                0,0,0,0,
                hwnd,
                NULL,
                g_hInstance,
                NULL
            );

splitter_add_window(g_frame_in,0);
splitter_add_window(g_frame_out,0);
splitter_redraw();

            return 0;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 1:
                    function_1(hwnd);
                    return 0;
                case 2:
                    function_2(hwnd);
                    return 0;
                case 3:
{
HWND w=CreateWindowEx(
                WS_EX_CLIENTEDGE,
                FRAME_CLASS,
                NULL,
                WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL,
                0,0,0,0,
                hwnd,
                NULL,
                g_hInstance,
                NULL
            );

splitter_add_window(w,0);
splitter_redraw();
}


                    return 0;
            }
            break;

            /* Здесь позже:
               - создание backbuffer
               - инициализация UI
               - меню / статус-бар
            */
            return 0;

    case WM_MOUSEMOVE:
        splitter_on_mouse_move(hwnd, wParam, lParam);
        break;

    case WM_LBUTTONDOWN:
	 splitter_on_lbutton_down(hwnd, wParam, lParam);
        break;

    case WM_LBUTTONUP:
        splitter_on_lbutton_up(hwnd, wParam, lParam);
        break;

    case WM_MOUSELEAVE:

	splitter_leave(hwnd);
break;



        case WM_SIZE:
            if (g_hwnd_status) {
                SendMessageW(g_hwnd_status, WM_SIZE, 0, 0);
            }
{
RECT rc_status;
GetWindowRect(g_hwnd_status, &rc_status);
splitter_set_parent(LOWORD(lParam),HIWORD(lParam)-(rc_status.bottom-rc_status.top));
splitter_redraw();
}

            /* Здесь позже:
               - переразмеривание backbuffer
               - перерасчёт layout
            */
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            /* Пока просто заливаем фон стандартным цветом */
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}



