#include "../ui/ui_shared.h"

void CG_DrawHorizontalPlayerStatus(rectDef_t *rect, qhandle_t shader, float num, float max_num, qboolean lefttoright);
void CG_DrawVerticalPlayerStatus(rectDef_t *rect, qhandle_t shader, float num, float max_num);
void CG_Draw1stPlaceScore( rectDef_t *rect, float scale, vec4_t color, int textStyle );
void CG_Draw2ndPlaceScore( rectDef_t *rect, float scale, vec4_t color, int textStyle );
void CG_DrawObit( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle );
void CG_DrawAreaNewChat( rectDef_t *rect, float scale, vec4_t color, int textStyle );
void CG_DrawMatchWinnerString( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align );
void CG_EndGameScoreString( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align );
void CG_PlayerCountsString( int team, int type, rectDef_t *rect, float scale, vec4_t color, int textStyle, int align );
void CG_GameLimitString( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align );
void CG_MatchStateString( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align );
void CG_ServerOwnerString( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align );
void CG_Match_Details( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align );
void CG_MatchEndCondition( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align );
void CG_PlayerBestWeaponName( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align );
void CG_SelectedPlayerAccuracy( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align );
void CG_Draw1stPlacePlayerModel (float x, float y, float w, float h);
void CG_DrawFollowNameString( rectDef_t *rect, float scale, vec4_t color, int textStyle, int align, qboolean drawfollowing );
