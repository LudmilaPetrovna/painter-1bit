/* ============================================================
   Layout logic
   ============================================================ */

static void layout_frames(HWND hwnd);
static void resize_paired_frames(HWND source);
static void update_scrollbars(HWND frame);

#define CANVAS_MIN_W 64
#define CANVAS_MIN_H 64


/* ----------------------------------------------------
   Frame window class
   ---------------------------------------------------- */

static const wchar_t *FRAME_CLASS = L"ImageFrameWindow";

static LRESULT CALLBACK frame_wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_NCHITTEST:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        ScreenToClient(hwnd, &pt);

        RECT rc;
        GetClientRect(hwnd, &rc);

        if (pt.x >= rc.right - 16 && pt.y >= rc.bottom - 16)
            return HTBOTTOMRIGHT;

        return HTCLIENT;
    }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}


void create_frames(HWND parent);

/* resize support */
BOOL frames_hit_test_resize(HWND parent, int x, int y);
void frames_resize_by_mouse(HWND parent, int x, int y);

BOOL frames_hit_test_resize(HWND parent, int x, int y)
{
    RECT rc;
    GetClientRect(parent, &rc);

    /* нижний правый угол второй рамки */
    int grip_x = g_frame_w * 2 + g_frame_gap;
    int grip_y = g_frame_h;

    if (x >= grip_x - 16 && x <= grip_x &&
        y >= grip_y - 16 && y <= grip_y)
        return TRUE;

    return FALSE;
}

void frames_resize_by_mouse(HWND parent, int x, int y)
{
    int new_w = (x - g_frame_gap) / 2;
    int new_h = y;

    if (new_w < CANVAS_MIN_W) new_w = CANVAS_MIN_W;
    if (new_h < CANVAS_MIN_H) new_h = CANVAS_MIN_H;

    g_frame_w = new_w;
    g_frame_h = new_h;

    layout_frames(parent);
}

/* Флаг resize-режима */
static BOOL g_resizing_frames = FALSE;


static void layout_frames(HWND hwnd){

return;

    if (!g_frame_in || !g_frame_out)
        return;

    RECT rc;
    GetClientRect(hwnd, &rc);

    int edge_x = GetSystemMetrics(SM_CXEDGE);
    int edge_y = GetSystemMetrics(SM_CYEDGE);

    int frame_w = g_canvas_w + edge_x * 2;
    int frame_h = g_canvas_h + edge_y * 2;

    if (g_layout == LAYOUT_HORIZONTAL) {
        SetWindowPos(g_frame_in, NULL,
            0, 0, frame_w, frame_h,
            SWP_NOZORDER);

        SetWindowPos(g_frame_out, NULL,
            frame_w + g_frame_gap, 0, frame_w, frame_h,
            SWP_NOZORDER);
    } else {
        SetWindowPos(g_frame_in, NULL,
            0, 0, frame_w, frame_h,
            SWP_NOZORDER);

        SetWindowPos(g_frame_out, NULL,
            0, frame_h + g_frame_gap, frame_w, frame_h,
            SWP_NOZORDER);
    }

//
    /* НЕ пересчитываем размеры — только позицию */
//     MoveWindow(g_frame_in,  x, y, g_frame_w, g_frame_h, TRUE);
//     MoveWindow(g_frame_out, x + g_frame_w + 20, y, g_frame_w, g_frame_h, TRUE);

}

static void resize_paired_frames(HWND source){
    RECT rc;
    GetClientRect(source, &rc);

    g_canvas_w = rc.right;
    g_canvas_h = rc.bottom;
}

static void update_scrollbars(HWND frame){
    RECT rc;
    GetClientRect(frame, &rc);

    int need_h = g_canvas_w > rc.right;
    int need_v = g_canvas_h > rc.bottom;

    ShowScrollBar(frame, SB_HORZ, need_h);
    ShowScrollBar(frame, SB_VERT, need_v);

    if (need_h) {
        SCROLLINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cbSize = sizeof(si);
        si.fMask  = SIF_RANGE | SIF_PAGE;
        si.nMin   = 0;
        si.nMax   = g_canvas_w - 1;
        si.nPage  = rc.right;
        SetScrollInfo(frame, SB_HORZ, &si, TRUE);
    }

    if (need_v) {
        SCROLLINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cbSize = sizeof(si);
        si.fMask  = SIF_RANGE | SIF_PAGE;
        si.nMin   = 0;
        si.nMax   = g_canvas_h - 1;
        si.nPage  = rc.bottom;
        SetScrollInfo(frame, SB_VERT, &si, TRUE);
    }
}







