#include <stdio.h>

int tracking=0;
int splitter_id0=-1;
int splitter_id=-1;
int current_dock=-1;
int rmin=-1,rmax=-1;
int g_drag_splitter=0;


/* splitter */
void splitter_on_mouse_move(HWND parent, WPARAM wParam, LPARAM lParam);
void splitter_on_lbutton_down(HWND parent, WPARAM wParam, LPARAM lParam);
void splitter_on_lbutton_up(HWND parent, WPARAM wParam, LPARAM lParam);
void splitter_leave(HWND parent);

typedef enum docktype{
DOCKING_UNUSED=0,
DOCKING_HORIZONTAL,
DOCKING_VERTICAL,
DOCKING_FLOAT,
DOCKING_ENDWINDOW,
DOCKING_TAB
} DOCKING_TYPE;

void splitter_add_window(HWND w, int parent);
void splitter_redraw();

typedef struct{
HWND hwnd;
DOCKING_TYPE type;
int parent;
int childs[16];
int childs_used;
int flags;
int width;
int height;
int offset_x;
int offset_y;

float weight;
float childs_weight;
} DOCK;

DOCK docks[256]={0};
int docks_used=1;

#define SPLITTER_W 6

//        SetCursor(LoadCursor(NULL, IDC_SIZEWE));

void splitter_init(){
}

void splitter_set_parent(int width, int height){
docks[0].type=DOCKING_HORIZONTAL;
docks[0].width=width;
docks[0].height=height;
docks[0].weight=1.0;
docks[0].offset_x=0;
docks[0].offset_y=0;
}

void splitter_add_window(HWND w, int parent){
docks[docks_used].hwnd=w;
docks[docks_used].type=DOCKING_ENDWINDOW;
docks[docks_used].weight=1.0;
docks[docks_used].parent=parent;
docks[parent].childs[docks[parent].childs_used++]=docks_used;
docks_used++;
}

void splitter_redraw(){
int root=0,node=0;
int child_id;
int q;

// step 1: calculate real size
double weight_sum=0;
for(q=0;q<docks[node].childs_used;q++){
child_id=docks[node].childs[q];
weight_sum+=docks[child_id].weight;
}
docks[node].childs_weight=weight_sum;

// step 2:
int child_size;
if(docks[node].type==DOCKING_HORIZONTAL){
int child_size=docks[node].width-SPLITTER_W*(docks[node].childs_used-1);

int offset_x=docks[node].offset_x;
int offset_y=docks[node].offset_y;

for(q=0;q<docks[node].childs_used;q++){
child_id=docks[node].childs[q];

int new_width=(int)((double)child_size/weight_sum*docks[child_id].weight);
int new_height=docks[node].height;

docks[child_id].width=new_width;
docks[child_id].height=new_height;
docks[child_id].offset_x=offset_x;
docks[child_id].offset_y=offset_y;

SetWindowPos(docks[child_id].hwnd, NULL, offset_x, offset_y, new_width, new_height, SWP_NOZORDER);
offset_x+=SPLITTER_W+new_width;
}

}

}

void splitter_on_mouse_move(HWND parent, WPARAM wParam, LPARAM lParam)
{

int x=LOWORD(lParam);
int y=HIWORD(lParam);
static wchar_t msg[256];
static wchar_t msg2[2560];

int root=0,node=0;
int child_id;
int q;
int parent_dock=0;

if(!g_drag_splitter){

// Find window, where mouse point to...
current_dock=-1;
if(docks[node].offset_x<x && docks[node].offset_x+docks[node].width>x){
current_dock=node;
}

if(current_dock<0){return;}

// print offsets for debug
int soffset=0;
soffset+=wsprintf(msg2+soffset,L"node %d: %dx%d+%d+%d, ",node,
docks[node].width,
docks[node].height,
docks[node].offset_x,
docks[node].offset_y
);
for(q=0;q<docks[node].childs_used;q++){
int child_id=docks[node].childs[q];
soffset+=wsprintf(msg2+soffset,L"ch %d: %dx%d+%d+%d, ",child_id,
docks[child_id].width,
docks[child_id].height,
docks[child_id].offset_x,
docks[child_id].offset_y
);
}
SendMessageW(g_hwnd_status, SB_SETTEXTW, 2, (LPARAM)msg2);



// find splitter_id;
node=current_dock;
splitter_id=-1;
splitter_id0=-1;
for(q=0;q<docks[node].childs_used;q++){
int child_id=docks[node].childs[q];

if(docks[child_id].offset_x-SPLITTER_W-1<x && docks[child_id].offset_x>=x){
splitter_id=child_id;
splitter_id0=docks[node].childs[q-1];
break;
}
}

if(splitter_id<0 || splitter_id0<0){return;}

wsprintf(msg,L"move: %dx%d, dock:%d, split:%d",x,y,current_dock,splitter_id);
SendMessageW(g_hwnd_status, SB_SETTEXTW, 1, (LPARAM)msg);

// we found our splitter


// calc resize region
rmin=docks[splitter_id0].offset_x+SPLITTER_W;
rmax=docks[splitter_id].offset_x+docks[splitter_id].width-SPLITTER_W;


// enable hovering
if(!tracking){
TRACKMOUSEEVENT tme = {0};
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = parent;

        TrackMouseEvent(&tme);
        tracking=1;

}


  HDC hdc = GetDC(parent);
RECT r = {
docks[splitter_id].offset_x-SPLITTER_W,docks[splitter_id].offset_y,
docks[splitter_id].offset_x,docks[splitter_id].offset_y+docks[splitter_id].height-1};

HBRUSH hbr = CreateSolidBrush(RGB(0, 0, 255));

    FillRect(hdc, &r, hbr);

    DeleteObject(hbr);
    ReleaseDC(parent, hdc);

        SetCursor(LoadCursor(NULL, IDC_SIZEWE));


} else {
// g_drag_splitter==1

if(x>=rmin && x<rmax){
// calc sum
//float w1=docks[splitter_id].weight;
//float w2=docks[splitter_id0].weight;
//float ws=w1+w2;

int p=wsprintf(msg,L"dragging: %d and %d",splitter_id0,splitter_id);


int offset_x=docks[splitter_id0].offset_x;
int offset_y=docks[splitter_id0].offset_y;
int new_width=x-offset_x-SPLITTER_W/2;
int new_height=offset_y+docks[splitter_id0].height;
SetWindowPos(docks[splitter_id0].hwnd, NULL, offset_x, offset_y, new_width, new_height, SWP_NOZORDER);

p+=wsprintf(msg+p,L", lpane:%dx%d+%d+%d",new_width, new_height,offset_x, offset_y);


offset_x=x+SPLITTER_W/2;
offset_y=docks[splitter_id].offset_y;
new_width=docks[splitter_id].offset_x+docks[splitter_id].width-x-SPLITTER_W/2;
new_height=offset_y+docks[splitter_id].height;
SetWindowPos(docks[splitter_id].hwnd, NULL, offset_x, offset_y, new_width, new_height, SWP_NOZORDER);

p+=wsprintf(msg+p,L", rpane:%dx%d+%d+%d",new_width, new_height,offset_x, offset_y);

SendMessageW(g_hwnd_status, SB_SETTEXTW, 1, (LPARAM)msg);


HDC hdc = GetDC(parent);
RECT r = {
x-SPLITTER_W/2,docks[splitter_id].offset_y,
x+SPLITTER_W/2,docks[splitter_id].offset_y+docks[splitter_id].height-1};

HBRUSH hbr = CreateSolidBrush(RGB(0, 0, 255));
FillRect(hdc, &r, hbr);
DeleteObject(hbr);
ReleaseDC(parent, hdc);



}


}

}




void splitter_on_lbutton_down(HWND parent, WPARAM wParam, LPARAM lParam)
{
int x=LOWORD(lParam);
int y=HIWORD(lParam);
    g_drag_splitter =1;
    SetCapture(parent);



}

void splitter_on_lbutton_up(HWND parent, WPARAM wParam, LPARAM lParam)
{
int x=LOWORD(lParam);
int y=HIWORD(lParam);
if(g_drag_splitter){
float w1=docks[splitter_id0].weight;
float w2=docks[splitter_id].weight;
float wsum=w1+w2;
float range=rmax-rmin;
float mult=wsum/range;
docks[splitter_id0].weight=(x-rmin)*mult;
docks[splitter_id].weight=(rmax-x)*mult;
    g_drag_splitter = 0;
}

    ReleaseCapture();
InvalidateRect(parent, NULL, FALSE);
    tracking=1;
splitter_redraw();


}


void splitter_leave(HWND parent){
tracking=0;

if(splitter_id>=0){
  HDC hdc = GetDC(parent);
RECT r = {
docks[splitter_id].offset_x-SPLITTER_W,docks[splitter_id].offset_y,
docks[splitter_id].offset_x,docks[splitter_id].offset_y+docks[splitter_id].height-1};

HBRUSH hbr = CreateSolidBrush(RGB(255, 255, 255));

    FillRect(hdc, &r, hbr);

    DeleteObject(hbr);
    ReleaseDC(parent, hdc);
splitter_id=-1;

        SetCursor(LoadCursor(NULL, IDC_ARROW));


}

/*
       - закончить рисование
       - скрыть hover
       - сбросить drag
*/
}
