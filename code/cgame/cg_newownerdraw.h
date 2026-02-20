#include "../ui/ui_shared.h"

void CG_DrawHorizontalPlayerStatus(rectDef_t *rect, qhandle_t shader, float num, float max_num, qboolean lefttoright);
void CG_DrawVerticalPlayerStatus(rectDef_t *rect, qhandle_t shader, float num, float max_num);
void CG_Draw1stPlaceScore( rectDef_t *rect, float scale, vec4_t color, int textStyle );
void CG_Draw2ndPlaceScore( rectDef_t *rect, float scale, vec4_t color, int textStyle );
void CG_DrawObit( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle );
void CG_DrawAreaNewChat( rectDef_t *rect, float scale, vec4_t color, int textStyle );
