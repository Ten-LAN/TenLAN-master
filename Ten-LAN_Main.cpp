//Mikanライブラリ
#include "common.h"
#include "UI\UI.h"

//Ten-LAN関連の関数とかの定義
#include "Ten-LAN.h"

#define TENLANVERSION "3.0"
#define KEEP_FOR_FINISH 60 // ゲーム終了のために長押しする時間(単位:フレーム)

unsigned long CATE_COLOR[ CATEGORY_MAX ] =
{
	0x00ff2828, // アクション
	0x00ff0091, // シューティング
	0x00dbae00, // スポーツ
	0x00c2cb10, // レース
	0x0016f000, // パズル
	0x0000a411, // テーブル
	0x00008f9a, // シミュレーション
	0x00004eff, // ロールプレイング
	0x00cf00ff, // 多人数
	0x00f400ff, // ミニゲーム
	0x00808080  // その他
};

int START_SS = 0;
char readfile[][30] = {
	"IMG_BACK_OP",
	"IMG_LOGO",
	"IMG_ITEM",
	"IMG_CATE_ICON",
	""
};

#define Free(r) { if(r) free(r); r = NULL; }

#define GAMEDIR    "Game"
#define SETTING    "data.ini"
#define SCREENSHOT "ss.png"

struct GAMEDATA *gd;

struct MYSYSTEM sys;

int ret;

time_t starttime;// ゲーム開始時刻

int date[3];//日付

// 統計データ読み込み(ゲームリストを読み込んだ後で)
int GetStatistics()
{
	time_t now;
	struct tm t;
	FILE* file;
	char stfile[512];
	int gamemax = sys.tenlan->GetGameMax();

	// 今日の日付を取得
	time(&now);
	localtime_s(&t, &now);
	date[0] = t.tm_mon + 1;
	date[1] = t.tm_mday;
	date[2] = t.tm_year + 1900;

	sprintf_s(stfile, 512, "%s\\%s_total%s", sys.CurDir, STATISTICS_FILE,STATISTICS_FILETYPE);

	// まずは統計の合計を読み込み
	// 統計データを保存したファイルを開く
	if (fopen_s(&file,stfile,"r") == 0)
	{
		int check;
		while (1)
		{
			int number;// ゲーム識別番号用
			int pnum;// プレイ回数用
			int total_h = 0, total_m = 0, total_s = 0;// 総プレイ時間用
			int ave_m = 0, ave_s = 0;// 平均時間用

			char title[2048];// タイトル用

			// 番号 ゲーム名 プレイ回数 総プレイ時間(時間、分、秒) 平均プレイ時間(分、秒)
			check = fscanf_s(file, "%d %s %d回 %d:%d:%d %d:%d",
				&number, title, sizeof(title), &pnum, &total_h, &total_m, &total_s,
				&ave_m, &ave_s);

			// 読み込みが終われば終了
			if (check == EOF)
			{
				break;
			}

			// 読み込んだゲームにその番号のものを探してデータ代入
			for (int n = 0; n < gamemax; n++)
			{
				if (gd[n].num == number)
				{
					gd[n].playnum_total = pnum;
					gd[n].playtime_total = total_h * 3600 + total_m * 60 + total_s;

					break;
				}
			}
		}
		fclose(file);
	}

	sprintf_s(stfile, 512, "%s\\%s%d_%02d_%04d%s", sys.CurDir, STATISTICS_FILE,
		date[0], date[1], date[2], STATISTICS_FILETYPE);

	// 次にこの日の統計を読み込み
	// 統計データを保存したファイルを開く
	if (fopen_s(&file, stfile, "r") == 0)
	{
		int check;
		while (1)
		{
			int number;// ゲーム識別番号用
			int pnum;// プレイ回数用
			int total_h = 0, total_m = 0, total_s = 0;// 総プレイ時間用
			int ave_m = 0, ave_s = 0;// 平均時間用

			char title[2048];// タイトル用

			// 番号 ゲーム名 プレイ回数 総プレイ時間(時間、分、秒) 平均プレイ時間(分、秒)
			check = fscanf_s(file, "%d %s %d回 %d:%d:%d %d:%d",
				&number, title, sizeof(title), &pnum, &total_h, &total_m, &total_s,
				&ave_m, &ave_s);

			// 読み込みが終われば終了
			if (check == EOF)
			{
				break;
			}

			// 読み込んだゲームにその番号のものを探してデータ代入
			for (int n = 0; n < gamemax; n++)
			{
				if (gd[n].num == number)
				{
					gd[n].playnum = pnum;
					gd[n].playtime = total_h * 3600 + total_m * 60 + total_s;

					break;
				}
			}
		}
		fclose(file);
	}

	return 0;
}

// 統計データの保存
int SaveStatistics()
{
	FILE* file;
	char stfile[512];

	int gamemax = sys.tenlan->GetGameMax();

	//まずは統計の合計を保存
	sprintf_s(stfile, 512, "%s\\%s_total%s", sys.CurDir, STATISTICS_FILE, STATISTICS_FILETYPE);

	if (fopen_s(&file, stfile, "w") == 0)
	{
		for (int n = 0; n < gamemax; n++)
		{
			// 平均プレイ時間(秒)
			int ave = 0;
			if (gd[n].playnum_total > 0)
			{
				ave = gd[n].playtime_total / gd[n].playnum_total;
			}

			// タイトル(改行コードを修正)
			char title[2048];
			strcpy_s(title, sizeof(title), gd[n].title);
			for (int i = 0; title[i] != '\0'; i++)
			{
				if (title[i] == '\n')
				{
					title[i] = '\0';
					break;
				}

				if (title[i] == ' ')
				{
					title[i] = '_';
				}
			}

			// 番号 ゲーム名 プレイ回数 総プレイ時間(時間、分、秒) 平均プレイ時間(分、秒)
			fprintf_s(file, "%d %s %03d回 %02d:%02d:%02d %02d:%02d\n\0",
				gd[n].num, title, gd[n].playnum_total,
				gd[n].playtime_total / 3600, (gd[n].playtime_total % 3600) / 60, gd[n].playtime_total % 60,
				ave / 60, ave % 60);
		}

		fclose(file);
	}

	sprintf_s(stfile, 512, "%s\\%s%d_%02d_%04d%s", sys.CurDir, STATISTICS_FILE,
		date[0], date[1], date[2], STATISTICS_FILETYPE);

	if (fopen_s(&file, stfile, "w") == 0)
	{
		for (int n = 0; n < gamemax; n++)
		{
			// 平均プレイ時間(秒)
			int ave = 0;
			if (gd[n].playnum > 0)
			{
				ave = gd[n].playtime / gd[n].playnum;
			}

			// タイトル(改行コードを修正)
			char title[2048];
			strcpy_s(title, sizeof(title), gd[n].title);
			for (int i = 0; title[i] != '\0'; i++)
			{
				if (title[i] == '\n')
				{
					title[i] = '\0';
					break;
				}

				if (title[i] == ' ')
				{
					title[i] = '_';
				}
			}

			// 番号 ゲーム名 プレイ回数 総プレイ時間(時間、分、秒) 平均プレイ時間(分、秒)
			fprintf_s(file, "%d %s %03d回 %02d:%02d:%02d %02d:%02d\n",
				gd[n].num, title, gd[n].playnum,
				gd[n].playtime / 3600, (gd[n].playtime % 3600) / 60, gd[n].playtime % 60,
				ave / 60, ave % 60);
		}

		fclose(file);
	}
	return 1;
}

// 日付チェック
int CheckDate()
{
	time_t now;
	struct tm t;
	int ndate[3];

	time(&now);
	localtime_s(&t, &now);
	ndate[0] = t.tm_mon + 1;
	ndate[1] = t.tm_mday;
	ndate[2] = t.tm_year + 1900;
	// 日付が変わっていないかチェック
	for (int i = 0; i < 3; i++)
	{
		if (date[i] != ndate[i])
		{
			// 日付を更新
			for (int j = i; j < 3; j++)
			{
				date[j] = ndate[j];
			}

			//ゲームの統計を初期化
			int gamemax = sys.tenlan->GetGameMax();
			for (int n = 0; n < gamemax; n++)
			{
				gd[n].playnum = 0;
				gd[n].playtime = 0;
			}

			return 1;
		}
	}

	return 0;
}

// ゲームリストの初期化
int InitGamelist( void )
{
	HANDLE hdir;
	WIN32_FIND_DATA status;
	char filepath[ 1024  ] = "";
	char *buf,*tok, *str;
	int n = 0, msel = 0, r, w;
	struct stat fstat;

	sprintf_s( filepath, 1023, "%s\\*", GAMEDIR );
	gd = (GDATA *)calloc( 255, sizeof( GDATA ) );
	sys.tenlan->SetGameMax( 0 );
	if ( ( hdir = FindFirstFile( filepath, &status ) ) != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( ( status.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
			{
				//ディレクトリ
				if ( strcmp( status.cFileName, "." ) != 0 &&
				     strcmp( status.cFileName, ".." ) != 0)
				{
					sprintf_s( filepath, 1023, "%s\\%s\\%s", GAMEDIR, status.cFileName, SETTING );
					stat( filepath, &fstat );
					if ( MikanFile->Open( 0, filepath, "r" ) >= 0 )
					{
						buf = ( char * )calloc( 2048, sizeof( char ) );
						// 初期化
						strcpy_s( gd[ msel ].exe, 1024, "" );
						strcpy_s( gd[ msel ].title, 2048, "NO TITLE" );
						strcpy_s( gd[ msel ].text, 2048, "" );
						gd[ msel ].date = 0;
						gd[ msel ].pad2key = 1;
						gd[ msel ].cnum = 0;
						gd[ msel ].category[ 0 ] = 0;
						while ( MikanFile->ReadLine( 0, buf, 512 ) )
						{
							str = strtok_s( buf, "=", &tok);
							if ( strcmp( str, "exe" ) == 0 )
							{
								r = w = 0;
								while ( tok[ r ] != '\0' )
								{
									gd[ msel ].exe[ w++ ] = tok[ r ];
									if ( tok[ r ] == '\n' )
									{
										break;
									}
									++r;
								}
								gd[ msel ].exe[ w - 1 ] = '\0';
								++w;
							} else if ( strcmp( str, "title" ) == 0 )
							{
								strcpy_s( gd[ msel ].title, 2048, strtok_s( NULL, "=", &tok) );
							} else if ( strcmp( str, "text" ) == 0 )
							{
								strcpy_s( gd[ msel ].text, 2048, strtok_s( NULL, "=", &tok) );
							} else if ( strcmp( str, "date" ) == 0 )
							{
								gd[ msel ].date = atoi( strtok_s( NULL, "=", &tok) );
							} else if ( strcmp( str, "pad2key" ) == 0 )
							{
								gd[ msel ].pad2key = atoi( strtok_s( NULL, "=", &tok) );
							} else if ( strcmp( str, "cate" ) == 0 )
							{
								// カテゴリ番号を取得。
								w = 0;
								for ( r = 0 ; tok[ r ] != '\0' ; ++r )
								{
									if ( '0' <= tok[ r ] && tok[ r ] <= '9' )
									{
										gd[ msel ].category[ gd[ msel ].cnum ] = gd[ msel ].category[ gd[ msel ].cnum ] * 10 + tok[ r ] - '0';
									} else if ( tok[ r ] == ',' )
									{
										++gd[ msel ].cnum;
									}
								}
								++gd[ msel ].cnum;
							}
						}
						MikanFile->Close( 0 );
						//fclose( fp );

						gd[ msel ].num = atoi( status.cFileName );
						gd[ msel ].txnum = START_SS + msel;
						GetCurrentDirectory( 512, gd[ msel ].txfile );
						sprintf_s( gd[ msel ].txfile, 512, "%s\\%s\\%s\\%s", gd[ msel ].txfile, GAMEDIR, status.cFileName, SCREENSHOT );
						MikanDraw->CreateTexture( gd[ msel ].txnum, gd[ msel ].txfile, TRC_NONE );
						Free( buf );
						gd[msel].playnum = 0;// 統計用データ初期化
						gd[msel].playtime = 0;// 統計用データ初期化
						gd[msel].playnum_total = 0;// 統計用データ初期化
						gd[msel].playtime_total = 0;// 統計用データ初期化
						++msel;
						//++sys.gamemax;
						sys.tenlan->SetGameMax( sys.tenlan->GetGameMax() + 1 );
					}
				}
			}
		} while ( FindNextFile( hdir, &status ) );
		FindClose( hdir );
	}

	sys.tenlan->SetGameData( gd );

	sys.currentgame = -1;

	return 0;
}

// ウィンドウ初期化
int Init( void )
{
	//ウィンドウハンドル
	HWND myhwnd = NULL;
	//RECT構造体
	RECT myrect;

	//ゲームパッド番号の初期化
	sys.GAMEPAD = -1;

	//デスクトップのウィンドウハンドル取得
	myhwnd = GetDesktopWindow();
	if ( myhwnd && GetClientRect( myhwnd, &myrect ) )
	{
		//デスクトップの大きさの取得
		//sys.width  = myrect.right;
		//sys.height = myrect.bottom;
		sys.tenlan->SetWindowSize( myrect.right, myrect.bottom );
	}

	_MikanWindow->SetWindow( WT_NORESIZEFULLSCREEN );
	return 0;
}

//ウィンドウのサイズをセットし直す
//ランチャー復帰後に行う
int ResetWindow(void)
{
	//ウィンドウハンドル
	HWND myhwnd = NULL;
	//RECT構造体
	RECT myrect;

	//デスクトップのウィンドウハンドル取得
	myhwnd = GetDesktopWindow();
	if (myhwnd && GetClientRect(myhwnd, &myrect))
	{
		//デスクトップの大きさの取得
		//sys.width  = myrect.right;
		//sys.height = myrect.bottom;
		sys.tenlan->SetWindowSize(myrect.right, myrect.bottom);
	}

	_MikanWindow->SetPositionXY( 0, 0 );
	_MikanWindow->SetWindow();
return 0;
}

// 特定ウィンドウの最大化
int FullWindow(HWND hwnd)
{
	//ウィンドウハンドル
	HWND myhwnd = NULL;
	//RECT構造体
	RECT myrect;
	RECT rect;
	RECT newrect;

	//デスクトップのウィンドウハンドル取得
	myhwnd = GetDesktopWindow();

	// デスクトップの大きさ、ウィンドウの大きさ取得
	if (myhwnd && hwnd && GetClientRect(myhwnd, &myrect) &&
		GetWindowRect(hwnd, &rect))
	{
		int x,y,width, height;

		// ゲームウィンドウの大きさ取得
		double w = rect.right - rect.left;
		double h = rect.bottom - rect.top;

		//デスクトップの大きさの取得
		double dw = myrect.right;
		double dh = myrect.bottom;

		if (w == dw || h == dh)
		{
			return 0;
		}

		double rw = dw / w;
		double rh = dh / h;

		if (rw < rh)
		{
			width = myrect.right;
			height = static_cast<int>(rw * h);
			x = 0;
			y = (myrect.bottom - height) / 2;
		}
		else
		{
			width = static_cast<int>(rh * w);
			height = myrect.bottom;
			x = (myrect.right - width) / 2;
			y = 0;
		}

		newrect.left = x; newrect.right = x + width;
		newrect.top = y; newrect.bottom = y + height;
		AdjustWindowRectEx(&newrect, GetWindowLong(hwnd, GWL_STYLE),
			false, GetWindowLong(hwnd, GWL_EXSTYLE));

		MoveWindow(hwnd, newrect.left, newrect.top,
			newrect.right, newrect.bottom, true);
	}

	return 1;
}

//ウィンドウの位置修正
int ModifyPos(HWND hwnd)
{
	//ウィンドウハンドル
	HWND myhwnd = NULL;
	//RECT構造体
	RECT myrect;
	RECT rect;
	RECT rectclient;

	//デスクトップのウィンドウハンドル取得
	myhwnd = GetDesktopWindow();

	if (myhwnd && hwnd && GetClientRect(myhwnd, &myrect) &&
		GetClientRect(hwnd, &rectclient) && GetWindowRect(hwnd, &rect))
	{
		if (myrect.right - rectclient.right == 0 &&
			myrect.bottom - rectclient.bottom == 0)
		{
			return 0;
		}

		int x = (myrect.right - rect.right + rect.left) / 2;
		int y = (myrect.bottom - rect.bottom + rect.top) / 2;

		if (x == rect.left && y == rect.top)
		{
			return 0;
		}

		MoveWindow(hwnd, x, y, rect.right-rect.left, rect.bottom-rect.top, true);
	}

	return 1;
}

// ウィンドウハンドルの取得
HWND GetWindowHandle ( unsigned long TargetID)
{
	HWND hWnd = GetTopWindow(NULL);
	DWORD ProcessID;
	do
	{
		if ( GetWindowLong( hWnd, GWL_HWNDPARENT) != 0 || !IsWindowVisible( hWnd))
		{
			continue;
		}
		GetWindowThreadProcessId(hWnd,&ProcessID);
		if ( TargetID == ProcessID )
		{
			return hWnd;
		}
	} while((hWnd = GetNextWindow( hWnd, GW_HWNDNEXT)) != NULL);

	return NULL;
}

// タスクバーの隠蔽
int HideTaskbar( void )
{
	OSVERSIONINFO OSver;
	HWND hWnd;

	OSver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	GetVersionEx( &OSver );

	hWnd = FindWindow( "Shell_TrayWnd", NULL );

	if ( ShowWindow(hWnd, SW_HIDE) )
	{
		if ( (OSver.dwMajorVersion == 6 && OSver.dwMinorVersion >= 1) || OSver.dwMajorVersion > 6 )
		{
			hWnd = FindWindowEx( NULL, NULL, "Button", "スタート" );
		}else
		{
			hWnd = FindWindow( "Button", "Start" );
		}
		ShowWindow( hWnd, SW_HIDE );
		return 0;
	}
	return 1;
}

// タスクバーの復活
int RestoreTaskbar( void )
{
	OSVERSIONINFO OSver;
	HWND hWnd;

	OSver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	GetVersionEx( &OSver );

	hWnd = FindWindow( "Shell_TrayWnd", NULL );
	ShowWindow( hWnd, SW_RESTORE );

	if( ( OSver.dwMajorVersion == 6 && OSver.dwMinorVersion >= 1 ) || OSver.dwMajorVersion > 6 )
	{
		hWnd = FindWindowEx( NULL, NULL, "Button", "スタート" );
	}else
	{
		hWnd = FindWindow( "Button", "Start" );
	}
	ShowWindow( hWnd, SW_RESTORE );
	return 0;
}

//ゲームの実行
int ExecProgram( int num )
{
	char *exe, dir[512];
	DWORD startpid, errcode;
	HWND hw;

	// 日付チェック
	CheckDate();

	sys.gamewin = NULL;
	// モード変更
	//sys.modeflag = MF_PLAY;
	sys.tenlan->SetGameMode( MF_PLAY );

	ZeroMemory( &( sys.si ), sizeof( sys.si ) );
	sys.si.cb = sizeof( sys.si );

	sys.process = 0;

	//sprintf_s( filepath, 2047, "%s\\%04d", GAMEDIR, gd[num].num );
	exe = gd[ num ].exe + strlen( gd[ num ].exe );
	while ( *exe != '\\' )
	{
		--exe;
	}
	//  *exe = '\0';
	strncpy_s( dir, 512, gd[ num ].exe, strlen( gd[ num ].exe ) - strlen( exe ) + 1 );
	// ゲームのあるディレクトリをカレントディレクトリに
	if ( SetCurrentDirectory( dir ) )
	{
		//  *exe = '\\';
		++exe;

		// パッドにキーを割り当て
		if ( gd[ num ].pad2key )
		{
			sys.tenlan->SetPad2Key( 0, gd[ num ].pad2key );
		}

		// キーの入力をログからなくす
		_MikanInput->UpdateKeyInput();
		_MikanInput->UpdateKeyInput();

		//memset( &(sys.pi), 0, sizeof( PROCESS_INFORMATION ) );

		// ゲーム起動
		if ( CreateProcess(NULL,exe,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,NULL,NULL,&(sys.si),&(sys.pi)) )
		{
			//sys.gamewin = GetWindowHandle( sys.pi.dwProcessId );
			CountUp( gd[ num ].num );
			startpid = sys.pi.dwProcessId;

			// 遊んでいるゲームが配列の何番目かを与える
			sys.currentgame = num;

			// ゲーム開始時刻取得
			time(&starttime);

			// タイムラグあるのでここでテクスチャ解放
			//ReleaseTexture();

			// 起動したゲームのウィンドウハンドル取得
			do
			{
				hw = GetWindowHandle( startpid );
			} while( hw == NULL );
			FullWindow(hw);
			sys.gamewin = hw;
			SetForegroundWindow( sys.gamewin );// ゲームのウィンドウを前へ
			// まずパッドへのキーの割り当てをゲームから削除
			_MikanInput->AttachPad2Key( sys.gamewin, FALSE );
			// そのあとパッドへのキーの割り当てをゲームへ適用
			if ( gd[ num ].pad2key )
			{
				_MikanInput->AttachPad2Key( sys.gamewin );
				//hw = MikanWindow->GetWindowHandle();
				SetForegroundWindow( sys.gamewin );
			}

			sys.MODE = 1;
		}
		// ゲーム起動失敗
		else
		{
			errcode = GetLastError();
			GetCurrentDirectory( 512, dir );
			sys.process = 1;
			//sys.modeflag = MF_OKAY;
			sys.tenlan->SetGameMode( MF_OKAY );
		}
	}
	return 0;
}

//ゲーム終了時の処理
int CheckEndProcess(void)
{
	unsigned long ExitCode;
	time_t now;

	if ( sys.process == 0 )
	{
		GetExitCodeProcess( sys.pi.hProcess, &ExitCode );

		// ゲームがまだ続いているなら
		if ( ExitCode == STILL_ACTIVE )
		{
			return 0;
		}
		SetForegroundWindow( MikanWindow->GetWindowHandle() );
		CloseHandle( sys.pi.hThread );
		CloseHandle( sys.pi.hProcess );
	} else
	{
		SetForegroundWindow( MikanWindow->GetWindowHandle() );
	}

	// ウィンドウをフルスクリーンへ
	ResetWindow();
	//_MikanWindow->SetWindow(WT_NORESIZEFULLSCREEN);
	//MikanSystem->SetInactiveWindow(0);

	// カレントディレクトリをTen-LANのある場所へ
	SetCurrentDirectory( sys.CurDir );

	// デバッグモードでなければ
	// プレイ時間を測定して加算する
	if (sys.tenlan->DebugMode() == 0)
	{
		// プレイ回数を増やす
		gd[sys.currentgame].playnum++;
		gd[sys.currentgame].playnum_total++;

		time(&now);// 現在時刻取得

		// 開始時間との差分を計算して加算
		gd[sys.currentgame].playtime += static_cast<long>(now - starttime);
		gd[sys.currentgame].playtime_total += static_cast<long>(now - starttime);

		// 統計を保存
		SaveStatistics();
	}

	sys.currentgame = -1;

	return 1;
}

// Ten-Lan終了処理
int GameEnd( void )
{
	if ( sys.tenlan->InputESC() > 60 )
	{
		//sys.end = 1;
		sys.tenlan->SetEndMode( 1 );
	}
	if ( sys.tenlan->GetEndMode() )//sys.end )
	{
		if ( InputAct_() == 1 )
		{
			if ( sys.tenlan->GetEndMode() == 2 )//sys.end == 2 )
			{
				//ゲーム終了
				return 1;
			}
			//sys.end = 0;
			sys.tenlan->SetEndMode( 0 );
		}
		if ( sys.tenlan->InputRight() % 20 == 1 ||
		     sys.tenlan->InputLeft()  % 20 == 1 ||
		     sys.tenlan->InputUp()    % 20 == 1 ||
		     sys.tenlan->InputDown()  % 20 == 1 )
		{
			//sys.end = ( sys.end + 2 ) % 2 + 1;
			sys.tenlan->SetEndMode( ( sys.tenlan->GetEndMode() + 2 ) % 2 + 1 );
		}
		MikanDraw->DrawTextureC( IMG_ITEM, sys.tenlan->GetWidth() / 2, sys.tenlan->GetHeight() / 2, 0, 150, 450, 280 );
		MikanDraw->DrawTextureC( IMG_ITEM, sys.tenlan->GetWidth() / 2 + (sys.tenlan->GetEndMode()==1?105:-105), sys.tenlan->GetHeight() / 2 + 90,
			250, 0 + (sys.tenlan->GetEndMode()==1?60:0), 200, 60 );
		/*    MikanDraw->DrawTextureC( IMG_ITEM, sys.width / 2, sys.height / 2, 20, 490, 680, 400 );
		MikanDraw->DrawTextureC( IMG_ITEM, sys.width / 2 + (sys.end==1?160:-160), sys.height / 2 + 10 + 120,
		710, 490 + (sys.end==1?80:0), 300, 80 );*/
	}

	return 0;
}

// デバイスロスト復帰後の処理
int AfterDeviceLost( void )
{
	// 画像の取得しなおし
	START_SS = 0;
	do
	{
		MikanDraw->ReleaseTexture(START_SS);
		MikanDraw->CreateTexture(START_SS, NULL, readfile[START_SS]);
	} while (*readfile[++START_SS]);
	START_SS = 10;
	for (int msel = 0; msel < sys.tenlan->GetGameMax(); msel++)
	{
		MikanDraw->ReleaseTexture(gd[msel].txnum);
		MikanDraw->CreateTexture(gd[msel].txnum, gd[msel].txfile, TRC_NONE);
	}

	return 0;
}

// バージョンの表示
int PrintVersion( void )
{
#ifdef _DEBUG
	char buf[ 256 ];
	sprintf_s( buf, 256, "Ten-LAN ver %s. UI:%s", TENLANVERSION, sys.ui->UIName() );
	//MikanDraw->Printf( 1, sys.width - 5 * strlen( buf ) - 5, sys.height - 15, buf );
#endif
	return 0;
}

char WTIT[] = TITLE;
HWND wnd;

//ウィンドウ生成前に1度だけ実行
void SystemInit( void )
{
	char a[] = MikanVersion;
#ifndef _DEBUG
	//    char *exe, dir[512];
	DWORD startpid, errcode;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
#endif
	SetEnvironmentVariable("TENLAN", "1");

#ifndef _DEBUG
	ZeroMemory( &( si ), sizeof( si ) );
	si.cb=sizeof( si );

	if ( CreateProcess( NULL, "./blackwindow.exe", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &(si), &(pi) ) )
	{
		Sleep( 1000 );
		startpid = pi.dwProcessId;
		do
		{
			wnd = GetWindowHandle( startpid );

		}while( wnd == NULL );

		/*    _MikanInput->AttachPad2Key( GetWindowHandle( pi.dwProcessId ) );
		_MikanInput->SendKey( VK_SHIFT, 1 );
		_MikanInput->SendKey( VK_SHIFT, 0 );*/

		//    SendMessage ( wnd, WM_CLOSE, 0, 0 );
	} else
	{
		errcode = GetLastError();
		errcode = 0;
	}
#endif

	sys.tenlan = new TenLAN();

	Init();

	MikanWindow->SetWindowIcon( "ICON_32" );
	MikanWindow->SetWindowName( WTIT );
	MikanSystem->SetInactiveWindow( 0 );

	// デバイスロスト後の復帰処理設定
	_MikanSystem->SetAfterDeviceLostFunction( AfterDeviceLost );
	ShowCursor( 0 );
}

//DirectX初期化後に1度だけ実行
void UserInit( void )
{
	MikanDraw->CreateFont( 1, 10, 0xff000000 );
	MikanDraw->CreateFont( 2, 30, 0xffffffff );

	_MikanSystem->SetPermitScreenSaver( 0 );

	//sys.maxvolume = 100;
	// config.txtがないときのために音量初期化
	// 音が大きいと展示中はゲームの方の音量が聞こえなくなってしまう、
	// 目立たなくなってしまうので音量は20(最大は100)
	sys.tenlan->SetMaxVolume(20);
	sys.tenlan->SetMaxSEVolume(20);

	do
	{
		MikanDraw->CreateTexture( START_SS, NULL, readfile[ START_SS ] );
	} while( *readfile[ ++START_SS ] );
	START_SS = 10;
	sys.MODE = 0;
	//sys.end = 0;

	// ゲームリストの初期化
	InitGamelist();

	// 現在のカレントディレクトリ(つまりTen-LANのあるディレクトリ)を
	// sys.CurDirへ
	GetCurrentDirectory( 1023, sys.CurDir );

	// ゲームの統計データ読み込み
	GetStatistics();

	//sys.timer = 0;
	sys.tenlan->SetTimer( 0 );
	if( MikanInput->GetPadMount() > 0 )
	{
		sys.GAMEPAD = 0;
	}

#ifndef _DEBUG
	HideTaskbar();
#endif
	// UIはここで指定する。
	// UIはUIBaseクラスを継承したクラスであれば何でも良い。
	sys.ui = new UI_2015( sys.tenlan );//new UI_Hiroki();//new UI_Tour();

	// config.txt読み込み
	sys.tenlan->LoadConfig();

	// UI初期化
	sys.ui->Init();

	// タイトル画面から始まる
	sys.tenlan->SetGameMode(MF_TITLE);

	// BGM初期化
	MikanSound->Load( 0, NULL, "BGM_TITLE" );
	MikanSound->SetVolume( 0, sys.tenlan->GetMaxVolume() );
	MikanSound->Play( 0, true );

	sys.searchserver = 0;
	MikanSystem->CreateLock( SERVER_LOCK );
	SearchTenLanServer();

	sys.tenlan->SetPad2Key( 0, 1 );

	SetForegroundWindow( MikanWindow->GetWindowHandle() );
}

//1秒間に60回実行される
int MainLoop( void )
{
	MikanDraw->ClearScreen( 0xffffffff );

	// ゲームが実行されているモード
	if( sys.MODE )
	{
		// ゲームが終了したときの処理
		if( CheckEndProcess() )
		{
			Sleep( 100 );
			sys.MODE = 0;
			//ReloadTexture();
			//ResetWindow();
			//_MikanDraw->SetScreenSize( sys.width, sys.height );
			_MikanDraw->SetScreenSize( sys.tenlan->GetWidth(), sys.tenlan->GetHeight() );
			// Pad2Keyのバッファを開放と言うか、DownしてUpしてない事態を防ぐ。
			_MikanInput->UpdateKeyInput();
			// Ten-LANもPad2Key有効なので元に戻す。
			sys.tenlan->SetPad2Key( 0, 1 );
			sys.ui->Init();
		}
		// ゲームが実行されている間の処理
		else if( sys.gamewin != NULL )
		{
			// ウィンドウの位置修正
			ModifyPos(sys.gamewin);

			// スタート + セレクト押しっぱなしでゲーム終了
			if (MikanInput->GetPadNum(0, sys.tenlan->PadSelect()) > KEEP_FOR_FINISH &&
				MikanInput->GetPadNum(0, sys.tenlan->PadStart()) > KEEP_FOR_FINISH)
			{
				SendMessage(sys.gamewin, WM_SYSCOMMAND, SC_CLOSE, 0);
			}

			// ゲーム画面を手前に
			SetForegroundWindow(sys.gamewin);
		}
		sys.ui->View();

	}
	// ゲームが実行されていないモード
	else
	{
		// ゲームがあるときの処理
		if ( sys.tenlan->GetGameMax() )
		{
			/*if( InputAct()>0)
			{
			sys.modeflag=MF_PLAY;
			ExecProgram( 0 );
			}*/
			sys.ui->View();

			// ゲームの確認画面及び実行中ではない
			if( sys.tenlan->GetGameMode() <= MF_SELECT )
			{
				//SetForegroundWindow( MikanWindow->GetWindowHandle() );
			}
			// ゲームが起動している場合の処理
			else if( sys.gamewin != NULL )
			{
				// ウィンドウの位置修正
				ModifyPos(sys.gamewin);

				// スタート + セレクト押しっぱなしでゲーム終了
				if (MikanInput->GetPadNum(0, sys.tenlan->PadSelect()) > KEEP_FOR_FINISH &&
					MikanInput->GetPadNum(0, sys.tenlan->PadStart()) > KEEP_FOR_FINISH)
				{
					SendMessage(sys.gamewin, WM_SYSCOMMAND, SC_CLOSE, 0);
				}

				// ゲーム画面を手前に
				SetForegroundWindow(sys.gamewin);
			}

		}
		// ゲームがない場合の処理
		else
		{
			if( sys.tenlan->InputAct() == 1 )
			{
				if( sys.error == 0 )
				{
					//ゲーム終了
					return 1;
				}
			}
			if( sys.tenlan->InputRight() % 20 == 1 ||
				sys.tenlan->InputLeft()  % 20 == 1 ||
				sys.tenlan->InputUp()    % 20 == 1 ||
				sys.tenlan->InputDown()  % 20 == 1 )
			{
				sys.error = ( sys.error + 1 ) % 2;
			}
			sys.ui->BackGround();
			sys.tenlan->AddTimer();
			MikanDraw->DrawTextureScalingC( IMG_LOGO, sys.tenlan->GetWidth() / 2, 100, 0, 0, 1024, 160, (double)sys.tenlan->GetHeight() / 1024 * 0.7 );
			MikanDraw->DrawTextureC( IMG_ITEM, sys.tenlan->GetWidth() / 2, sys.tenlan->GetHeight() / 2, 0, 450, 450, 420 );
			MikanDraw->DrawTextureC( IMG_ITEM, sys.tenlan->GetWidth() / 2 + (sys.error?105:-105), sys.tenlan->GetHeight() / 2 + 160,
				250, 0 + (sys.error?60:0), 200, 60 );
		}
	}

	PrintVersion();
	return GameEnd();
}

//最後に一度だけ実行される
void CleanUp( void )
{
#ifndef _DEBUG
	SendMessage ( wnd, WM_CLOSE, 0, 0 );
	RestoreTaskbar();
#endif
	sys.ui->Release();
	delete( sys.tenlan );
	delete( sys.ui );
	free( gd );
}

