qboolean CG_IsDuelGame( int gametype );
qboolean CG_IsFlagGame( int gametype );
qboolean CG_IsTeamGame( int gametype );

qboolean CG_IsOurTeam( int team );
qboolean CG_IsEnemyTeam( int team );

qboolean CG_IsUs( const clientInfo_t *ci );
qboolean CG_IsTeammate( const clientInfo_t *ci );
qboolean CG_IsEnemy( const clientInfo_t *ci );
qboolean CG_ScoresEqual (int score1, int score2);

void CG_SetTeamColor( float alpha );
int CG_GetPlayerCount( int team );
void CG_ScaleModel( refEntity_t *re, float size );
const char *CG_GetTimeString (int ms);
const char* CG_GetTeamName (int team);

int CG_GetCurrentTimeWithDirection (qboolean countdown);
const char* CG_GetLocalTimeDateString (void);
const char* CG_GetLocalHourMinuteString (void);
