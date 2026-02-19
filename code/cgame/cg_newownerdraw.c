#ifndef MISSIONPACK
#error This file not be used for classic Q3A.
#endif
#include "cg_local.h"
#include "cg_utils.h"
#include "cg_newownerdraw.h"

void CG_DrawHorizontalPlayerStatus(rectDef_t *rect, qhandle_t shader, float num, float max_num, qboolean lefttoright)
{
    float value;
    float s1, t1, s2, t2;
    value = Com_Clamp(0, max_num, num);
    CG_SetTeamColor(1);
    CG_AdjustFrom640( &rect->x, &rect->y, &rect->w, &rect->h );

    // drain from left to right
    if( lefttoright ) {
        s1 = 1.0 - (value / 100.0);
        t1 = 0;
        s2 = 1; t2 = 1;
        trap_R_DrawStretchPic( rect->x + rect->w * (1 - value / 100.0), rect->y, rect->w * (value / 100.0), rect->h, s1, t1, s2, t2, shader );
    } else {
        s1 = 0; t1 = 0;
        s2 = (value / 100.0);
        t2 = 1;
        trap_R_DrawStretchPic( rect->x, rect->y, rect->w * (value / 100.0), rect->h, s1, t1, s2, t2, shader );
    }
}

// The current ql image (a200.png) has spacing for more than 200 health so that needs to be taken into account.
// Also, 145 (not 150) matches ql behavior.
void CG_DrawVerticalPlayerStatus(rectDef_t *rect, qhandle_t shader, float num, float max_num)
{
    float value, max_value;
    float s1, t1, s2, t2;
    max_value = max_num * 1.45;
    value = Com_Clamp(0, max_value, num);

    CG_SetTeamColor(1);
    CG_AdjustFrom640( &rect->x, &rect->y, &rect->w, &rect->h );
    s1 = 0;
    t1 = 1.0 - (value / (float)max_value);
    s2 = 1;
    t2 = 1;
    trap_R_DrawStretchPic( rect->x, rect->y + rect->h * (1 - value / (float)max_value), rect->w, rect->h * (value / (float)max_value), s1, t1, s2, t2, shader );
}
