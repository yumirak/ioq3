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

static void CG_DrawPlaceScore( rectDef_t *rect, float scale, vec4_t color, int textStyle, char *s, int score )
{
    int      spacing;
    int      teamSpacing;
    int      maxNameWidth;
    char     *endOfName;

    teamSpacing = CG_IsTeamGame( cgs.gametype ) ? rect->h / 2 : 0;
    spacing = CG_Text_Width( "00", scale, 0 );

    maxNameWidth = rect->w - spacing - 1;
    endOfName = s + strlen( s );

    while( maxNameWidth > 0 && CG_Text_Width( s, scale, 0 ) > maxNameWidth ) {
        endOfName--;
        *endOfName = '\0';
    }

    // FIXME spacing for scores
    CG_Text_Paint( rect->x + teamSpacing, rect->y, scale, color, s, 0, rect->w - teamSpacing - spacing - 6, textStyle );

    s = va( "%d  ", score );
    if( score == SCORE_NOT_PRESENT ) {
        s = "";
    }

    CG_Text_Paint( rect->x + rect->w - spacing, rect->y, scale, color, s, 0, 0, textStyle );
}

void CG_Draw1stPlaceScore( rectDef_t *rect, float scale, vec4_t color, int textStyle )
{
    char *s;
    int  score;
    int  team;

    if( !CG_IsTeamGame( cgs.gametype ) && !cgs.clientinfo[ cgs.cs[CS_CLIENTNUM1STPLAYER].integer ].infoValid )
        return;

    team = cg.snap->ps.persistant[PERS_TEAM];

    if( CG_IsTeamGame( cgs.gametype )) {
        if( cgs.scores1 > cgs.scores2 ) {
            score = cgs.scores1;
            s = va( "1.%s", CG_GetTeamName( TEAM_RED ));
        } else if( cgs.scores1 < cgs.scores2 ) {
            score = cgs.scores2;
            s = va( "1.%s", CG_GetTeamName( TEAM_BLUE ));
        } else {
            s = va( "1.%s", CG_GetTeamName( team ));
            score = cgs.scores1;
        }
    } else {
        s = va( "1.%s", cgs.cs[CS_FIRSTPLACE].string );
        score = cgs.scores1;
    }

    if( s )
        CG_DrawPlaceScore( rect, scale, color, textStyle, s, score );

}

void CG_Draw2ndPlaceScore( rectDef_t *rect, float scale, vec4_t color, int textStyle )
{
    char *s;
    int  team;
    int  score;
    int  rank;

    if( !CG_IsTeamGame( cgs.gametype ) && !cgs.clientinfo[ cgs.cs[CS_CLIENTNUM2NDPLAYER].integer ].infoValid )
        return;

    team = cg.snap->ps.persistant[PERS_TEAM];
    rank = cg.snap->ps.persistant[PERS_RANK] & ~RANK_TIED_FLAG;
    rank++;

    if( CG_IsTeamGame( cgs.gametype )) {
        if( cgs.scores1 < cgs.scores2 ) {
            score = cgs.scores1;
            s = va( "2.%s", CG_GetTeamName( TEAM_RED ));
        } else if( cgs.scores1 > cgs.scores2 ) {
            score = cgs.scores2;
            s = va( "2.%s", CG_GetTeamName( TEAM_BLUE ));
        } else {
            s = va( "2.%s", CG_GetTeamName( team == TEAM_BLUE ? TEAM_RED : TEAM_BLUE ));
            score = cgs.scores1;
        }
    } else {
        if( cgs.scores1 == cg.snap->ps.persistant[PERS_SCORE] ) {
            if( cgs.scores2 == cgs.scores1 ) {
                s = va( "%d.%s", 1, cgs.cs[CS_SECONDPLACE].string );
            } else {
                s = va( "%d.%s", cgs.scores2 == cgs.scores1 ? 1 : 2, cgs.cs[CS_SECONDPLACE].string );
            }
            score = cgs.scores2;
        } else {
            s = va( "%d.^7%s", rank, cgs.clientinfo[cg.snap->ps.clientNum].name );
            score = cg.snap->ps.persistant[PERS_SCORE];
        }
    }

    if( s )
        CG_DrawPlaceScore( rect, scale, color, textStyle, s, score );
}
