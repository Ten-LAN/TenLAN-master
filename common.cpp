//#include "Socket.h"
#include "common.h"
#include "UI\UI.h"

extern struct MYSYSTEM sys;


extern char readfile[][30];
extern struct GAMEDATA *gd;

//#define SERVER "192.168.0.1"
char SERVER[ 16 ] = "";
#define PORT        49326
#define PORT_SEARCH 49327

#define SERVER_COUNTUP      "COUNTUP"
#define SERVER_RECEIVE      "OK"
#define SERVER_JOIN         "JOIN"

char WEEK[ 7 ][ 4 ] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

int ThreadCountUp( void *vdParam )
{
	int num = (int)vdParam;
	char buf[6];

	if ( strcmp( SERVER, "" ) )
	{
		MikanNet->CreateTCPClient( 0, SERVER, PORT );
		// ここら辺でカウントアップ用の処理
		MikanNet->Send( 0, SERVER_COUNTUP );
		sprintf_s( buf, 6, "%d", num );
		MikanNet->Send( 0, buf );
		MikanNet->Receive( 0, buf, 6 );
		MikanNet->Close( 0 );
	}

	return 0;
}



int CountUp( int num )
{
	time_t now;
	struct tm jpt;
	char logfile[ 512 ];

	sprintf_s( logfile, 512, "%s\\%s", sys.CurDir, LOG_FILE );
	if ( MikanFile->Open( 0, logfile, "a+" ) >= 0 )//何故かaだと動かない
	{
		// 時間の取得
		time( &now );

		// 時間の変換
		localtime_s( &jpt, &now );

		_MikanFile->Printf( 0, 256, "%d/%02d/%02d(%s)%02d:%02d:%02d|%d\n",
			jpt.tm_year + 1900, jpt.tm_mon + 1, jpt.tm_mday, WEEK[ jpt.tm_wday ],
			jpt.tm_hour, jpt.tm_min, jpt.tm_sec, num
			);
		//ファイルを閉じる
		MikanFile->Close( 0 );
	}

	MikanSystem->RunThread( 2, ThreadCountUp, (void *)num );

	return 0;
}

int ReleaseTexture( void )
{
	int i;

	if( sys.ui )
	{
		sys.ui->ReleaseTexture();
	}

	for( i = 0 ; i < IMG_MAX ; ++i )
	{
		MikanDraw->ReleaseTexture( i );
	}

	for( i = 0 ; i < sys.tenlan->GetGameMax() ; ++i)
	{
		MikanDraw->ReleaseTexture( gd[ i ].txnum );
	}

	return 0;
}

int ReloadTexture( void )
{
	char filepath[ 1024  ] = "";
	int msel = 0;
	int i;

	if( sys.ui )
	{
		sys.ui->ReloadTexture();
	}

	for( i = 0 ; i < IMG_MAX ; ++i )
	{
		MikanDraw->CreateTexture( i, NULL, readfile[ i ] );
	}

	for( i = 0 ; i < sys.tenlan->GetGameMax() ; ++i )
	{
		//MikanDraw->CreateTexture( gd[ i ].txnum, gd[ i ].txfile, TRC_NONE );
	}

	/*sprintf_s( filepath, 1023, "%s\\*", GAMEDIR );
	if( ( hdir = FindFirstFile( filepath, &status ) ) != INVALID_HANDLE_VALUE )
	{
	do
	{
	if( ( status.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
	{
	//ディレクトリ
	if( strcmp( status.cFileName, "." ) != 0 &&
	strcmp( status.cFileName, ".." ) != 0)
	{
	//          GAMEDIR
	sprintf_s( filepath, 1023, "%s\\%s\\%s", GAMEDIR, status.cFileName, SCREENSHOT );
	MikanDraw->CreateTexture( gd[ msel ].txnum, filepath, 0x00000000 );
	++msel;
	}
	}
	}while( FindNextFile( hdir,&status ) );
	FindClose( hdir );
	}*/

	return 0;
}


int SortGameList( int type )
{
	return 0;
}

// サーバー検索本体
int SearchTenLanServer_( void )
{
	int i;
	char buf[ 64 ], *tmp;
	struct sockaddr_in server;

	MikanSystem->Lock( SERVER_LOCK );
	sys.searchserver = 1;

	MikanNet->CreateBloadcastClient( NET_SEARCH, "255.255.255.255", PORT_SEARCH );

	for( i = 0 ; i < SERVER_SEARCH_COUNT ; ++i )
	{
		Sleep( SERVER_SEARCH_TIMER );
		MikanNet->Send( NET_SEARCH, SERVER_JOIN );
		MikanNet->Receive( NET_SEARCH, buf, 64, (struct sockaddr *)&server );
		break; // 本当はタイムアウト処理を入れてここを外す。
	}

	MikanNet->Close( NET_SEARCH );

	if ( strcmp( buf, "OK" ) == 0 )
	{
		tmp = inet_ntoa( server.sin_addr );
		strcmp( SERVER, tmp );
	}
	sys.searchserver = 0;
	MikanSystem->Unlock( SERVER_LOCK );
	return 0;
}

// サーバー検索
int SearchTenLanServer( void )
{
	if ( sys.searchserver == 0 )
	{
		MikanSystem->RunThread( THREAD_SEARCH, SearchTenLanServer_ );
	}

	return 0;
}

// サーバー検索画面
int SearchServerNow( void )
{

	MikanDraw->Printf( 2, sys.tenlan->GetWidth() / 2 -  15 * 7, sys.tenlan->GetHeight() / 2 + 40, "決定ボタンでTen-LANサーバーを検索します。\n現在の状態:[ %s ]", sys.searchserver == 0 ? SERVER : "検索中" );

	if ( sys.tenlan->InputAct() == 1 && sys.searchserver == 0 )
	{
		SearchTenLanServer();
	}

	return 0;
}

int InputAct_( void )
{
	return  MikanInput->GetKeyNum( K_Z );
}

int InputCancel_( void )
{
	return MikanInput->GetKeyNum( K_X );
}
