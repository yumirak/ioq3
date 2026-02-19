#include "cg_local.h"
#include "cg_utils.h"

static const char *MonthAbbrev[] = {
	"Jan","Feb","Mar",
	"Apr","May","Jun",
	"Jul","Aug","Sep",
	"Oct","Nov","Dec"
};

void CG_ScaleModel( refEntity_t *re, float size )
{
	re->radius = size;
	VectorScale( re->axis[0], re->radius, re->axis[0] );
	VectorScale( re->axis[1], re->radius, re->axis[1] );
	VectorScale( re->axis[2], re->radius, re->axis[2] );
	re->nonNormalizedAxes = qtrue;
}

void CG_SetTeamColor( float alpha )
{
	vec4_t color;
	int    clientNum = cg.snap->ps.clientNum;

	switch( cgs.clientinfo[clientNum].team )
	{
	case TEAM_RED:
		Vector4Copy( colorRed, color );
		break;
	case TEAM_BLUE:
		Vector4Copy( colorBlue, color );
		break;
	default:
		Vector4Copy( colorYellow, color );
		break;
	}

	color[3] = alpha;
	trap_R_SetColor( color );
}

qboolean CG_IsEnemy( const clientInfo_t *ci )
{
	int ourClientNum;
	const clientInfo_t *us;

	ourClientNum = cg.snap->ps.clientNum;

	us = &cgs.clientinfo[ourClientNum];

	if( us == ci )
		return qfalse;

	if( CG_IsTeamGame( cgs.gametype ) && ci->team != us->team ) {
		return qtrue;
	} else if( !CG_IsTeamGame( cgs.gametype )) {
		return qtrue;
	}

	return qfalse;
}

qboolean CG_IsTeammate( const clientInfo_t *ci )
{
	int ourClientNum;
	const clientInfo_t *us;

	if( !CG_IsTeamGame( cgs.gametype )) {
		return qfalse;
	}

	ourClientNum = cg.snap->ps.clientNum;

	us = &cgs.clientinfo[ourClientNum];

	if( ci->team != us->team ) {
		return qfalse;
	}

	return qtrue;
}

qboolean CG_IsEnemyTeam( int team )
{
	int ourClientNum;
	const clientInfo_t *us;

	ourClientNum = cg.snap->ps.clientNum;

	us = &cgs.clientinfo[ourClientNum];

	if( CG_IsTeamGame( cgs.gametype ) && team != us->team ) {
		return qtrue;
	} else if( !CG_IsTeamGame( cgs.gametype )) {
		return qtrue;
	}

	return qfalse;
}

qboolean CG_IsOurTeam( int team )
{
	int ourClientNum;
	const clientInfo_t *us;

	if( !CG_IsTeamGame( cgs.gametype )) {
		return qfalse;
	}

	ourClientNum = cg.snap->ps.clientNum;

	us = &cgs.clientinfo[ourClientNum];

	if( team != us->team ) {
		return qfalse;
	}

	return qtrue;
}

qboolean CG_IsUs( const clientInfo_t *ci )
{
	int ourClientNum;

	ourClientNum = cg.snap->ps.clientNum;

	if( ci == &cgs.clientinfo[ourClientNum] ) {
		return qtrue;
	}

	return qfalse;
}

qboolean CG_IsTeamGame( int gametype )
{
	if( gametype == GT_TEAM || gametype == GT_CTF || gametype == GT_1FCTF || gametype == GT_OBELISK || gametype == GT_HARVESTER ) {
		return qtrue;
	}

	return qfalse;
}

qboolean CG_IsFlagGame( int gametype )
{
	if( gametype == GT_CTF || gametype == GT_1FCTF ) {
		return qtrue;
	}

	return qfalse;
}

qboolean CG_IsDuelGame( int gametype )
{
	if( gametype == GT_TOURNAMENT ) {
		return qtrue;
	}

	return qfalse;
}

qboolean CG_ScoresEqual (int score1, int score2)
{
	if (score1 == score2) {
		return qtrue;
	}

	return qfalse;
}

int CG_GetPlayerCount( int team )
{
	int i, count;
	const clientInfo_t *ci;
	count = 0;

	for( i = 0; i < cgs.maxclients; i++ ) {
		ci = &cgs.clientinfo[i];
		if (!ci->infoValid)
			continue;
		if( team >= TEAM_FREE && cgs.clientinfo[i].team != team )
			continue;

		count++;
	}

	return count;
}

const char *CG_GetTimeString( int ms )
{
	static char timeString[16];
	int hours, minutes, seconds;

	hours = ( ms / 1000 ) / 3600;
	ms -= ( hours * 3600 * 1000 );

	minutes = ( ms / 1000 ) / 60;
	ms -= ( minutes * 60 * 1000 );

	seconds = ( ms / 1000 );

	if( hours ) {
		Com_sprintf( timeString, sizeof( timeString ), "%i:%02i:%02i", hours, minutes, seconds );
	} else {
		Com_sprintf( timeString, sizeof( timeString ), "%i:%02i", minutes, seconds );
	}

	return timeString;
}

const char* CG_GetTeamName (int team)
{
	switch (team) {
		case TEAM_RED:
			return "Red Team";
		case TEAM_BLUE:
			return "Blue Team";
		default:
			return "";
	}
}

int CG_GetCurrentTimeWithDirection (qboolean countdown)
{
	int msec, cgtime, levelStartTime;

	levelStartTime = cgs.levelStartTime;
	cgtime = cg.time;

	if (cg.warmup || cgs.timelimit <= 0) {
		msec = cg.time - levelStartTime;
		return msec;
	}

	switch(countdown) {
		case qtrue:
			msec = (levelStartTime + (cgs.timelimit * 60 * 1000)) - cgtime;
			break;
		case qfalse:
			msec = cgtime - levelStartTime;
			break;
	}

	return msec;
}

const char* CG_GetLocalTimeDateString (void)
{
	qtime_t now;
	static char szDate[128];

	trap_RealTime(&now);
	Com_sprintf(szDate, sizeof(szDate), "%02d:%02d (%s %d, %d)", now.tm_hour, now.tm_min, MonthAbbrev[now.tm_mon], now.tm_mday, 1900 + now.tm_year);

	return szDate;
}

const char* CG_GetLocalHourMinuteString (void)
{
	qtime_t now;
	static char szDate[128];

	trap_RealTime(&now);
	Com_sprintf(szDate, sizeof(szDate), "%02d:%02d", now.tm_hour, now.tm_min);

	return szDate;
}

int CG_GetWeaponFovOffset( int fov )
{
	int fovOffset = 0;

	if ( fov > 90 ) {
		fovOffset = -0.2 * ( fov - 90 );
	}

	return fovOffset;
}
