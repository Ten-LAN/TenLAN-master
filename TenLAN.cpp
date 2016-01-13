#include "common.h"


int op_anime;
int op_timer;
int op_dis = 57;
int op_game = -1;
int op_deftime = -60 * 17;
double op_x, op_basex, op_lastx;

// コンストラクタ
TenLAN::TenLAN( void )
{
	deckey = -1;

	end = 0;

	op_dis = 57;
	op_game = -1;
	op_deftime = -60 * 17;
	op_x, op_basex, op_lastx;
	op_anime = 0;
	op_timer = 0;

	debugmode = 0;
}

// タイトル画面処理
int TenLAN::OP( void )
{
	double opscl;

	MikanDraw->DrawBox( 0, 0, width, height, 0xffffffff );

	if( op_game < 0 )
	{
		op_game = gamemax - 1;
		op_basex = (double)width;
		op_lastx = 0.00001;
		op_timer = op_deftime;
	}
	MikanDraw->DrawTextureScaling( IMG_OP, 0, 0, 0, 0, 1024, 512, (double)width / 1024 );

	MikanDraw->DrawTextureScalingC( IMG_LOGO, width / 2, 100, 0, 0, 1024, 160, (double)width / 1024 * 0.7 );
	MikanDraw->DrawTextureC( IMG_ITEM, width / 2, height - 150, 460, 0 + 70 * (( timer / 5 )%6), 540, 70 );

	if( op_anime > 0 )
	{
		MikanDraw->DrawBox( 0, 0, MikanWindow->GetWindowWidth(), MikanWindow->GetWindowHeight(), MikanDraw->Color( 255 * (30 - op_anime) / 30, 0, 0, 0 ) );
		MikanSound->SetVolume( 0, MikanSound->GetVolume( 0 ) - (int)((double)GetMaxVolume() / 30.0) );
		if( --op_anime <= 0 )
		{
			SetGameMode( MF_SELECT );
			MikanSound->SetVolume( 0, 0 );
			MikanSound->Stop( 0 );
			// 次のアニメーションに向けて値を初期化しておく。
			op_timer = op_deftime;
		}
	}else
	{

		if( GetGameMode() == MF_TITLE )
		{
			if( InputAct() == 1 )
			{
				// 次画面へ遷移するために頑張る。
				op_anime = 30;
			}else if( MikanInput->GetKeyNum( K_K ) == 1)
			{
				// 隠しコマンド:キーコンフィグ
				SetGameMode( MF_KEYCONFIG );
			}else if( MikanInput->GetKeyNum( K_M ) == 1 || InputStart() == 1)
			{
				// 隠しコマンド:音
				SetGameMode( MF_SOUND );
			}
			else if (MikanInput->GetKeyNum( K_T ) == 1 || InputSelect() == 1)
			{
				// デバッグモード
				debugmode = !debugmode;
			}
		}
	}

	if( MikanSound->GetVolume( 0 ) < GetMaxVolume() )//sys.maxvolume )
	{
		MikanSound->SetVolume( 0, (MikanSound->GetVolume( 0 ) + 10) > GetMaxVolume() ? GetMaxVolume() : (MikanSound->GetVolume( 0 ) + 10) );
	}
	if( op_timer >= 0 )
	{
		// ここからスライドショー開始

		switch( (op_timer / op_dis) % 4 )
		{
		case 0:
			MikanDraw->SetColor( GetGameData( op_game )->txnum, MikanDraw->Color( 255 * (op_timer%op_dis) / op_dis, 255, 255, 255 ) );

			if( op_timer % op_dis == 0 )
			{
				op_x = op_basex;
			}else if( op_x > 0 )
			{
				op_lastx = op_x -= op_x / 8.0;
			}else
			{
				op_x = 0;
			}
			opscl = 0.7 + 0.3 * (op_basex - op_x) / op_basex;
			break;
		case 1:
			op_x = 0.0;
			opscl  =1.0;
			break;
		case 2:
			MikanDraw->SetColor( GetGameData( op_game )->txnum, MikanDraw->Color( 255 * (op_dis - op_timer%op_dis) / op_dis, 255, 255, 255 ) );
			if( op_timer % op_dis == 0 )
			{
				op_x = -op_lastx;
			}else if( GetWidth() / 2 + op_x > -320 )
			{
				op_x += op_x * 0.5;
			}else
			{
				op_x = -op_basex;
			}
			opscl = 0.7 + 0.3 * (op_basex + op_x) / op_basex;
			break;
		case 3:
			if( op_timer % op_dis == 0 )
			{
				op_game = op_game + 1 >= GetGameMax() ? 0 : op_game + 1;
			}
			op_x = 0.0;
			opscl = 0.7;
			MikanDraw->SetColor( GetGameData( op_game )->txnum, 0x00ffffff );
			break;
		}

		MikanDraw->DrawTextureScalingC( GetGameData( op_game )->txnum, GetWidth() / 2 + (int)op_x, GetHeight() / 2, 0, 0, 640, 480, opscl );
		MikanDraw->SetColor( GetGameData( op_game )->txnum, 0xffffffff );

	}

	++op_timer;

	// 隠しコマンド
	Command_();

	return 0;
}

// BGM音量設定画面
int TenLAN::SetVolume( void )
{
	int width = 500;
	int w;
	int h;

	w = GetWidth();
	h = GetHeight();
	MikanDraw->DrawTexture( 2, (w - width) / 2, h / 2 - 20, 0, 900, width, 30 );
	MikanDraw->DrawTexture( 2, (w - width) / 2, h / 2 - 20, 0, 870, width * GetMaxVolume() / 100, 30 );

	MikanDraw->Printf(2, w / 2 - 15 * 7, h / 2 - 60, "BGM");
	MikanDraw->Printf( 2, w / 2 -  15 * 7, h / 2 + 40, "%3d/100", GetMaxVolume() );

	int musicNum = 9;// BGMの数 追加の際に1増やす

	if( InputRight() % 2 == 1 && GetMaxVolume() < 100 )
	{
		SetMaxVolume(GetMaxVolume() + 1);
		MikanSound->SetVolume(0, GetMaxVolume());
		for (int i = 2; i < 2 + musicNum; i++)
		{
			MikanSound->SetVolume(i, GetMaxVolume() * 3 / 4);
		}
	}
	if( InputLeft() % 2 == 1 && GetMaxVolume() > 0 )
	{
		SetMaxVolume( GetMaxVolume() - 1 );
		MikanSound->SetVolume(0, GetMaxVolume());
		for (int i = 2; i < 2 + musicNum; i++)
		{
			MikanSound->SetVolume(i, GetMaxVolume() * 3 / 4);
		}
	}
	if( InputAct() == 1 )
	{
		SaveConfig( SAVE_SOUND_VOLUME, GetMaxVolume() );
		return 1;
	}
	return 0;
}

// 効果音音量設定画面
int TenLAN::SetVolumeSE(void)
{
	int width = 500;
	int w;
	int h;

	w = GetWidth();
	h = GetHeight();
	MikanDraw->DrawTexture(2, (w - width) / 2, h / 2 - 20, 0, 900, width, 30);
	MikanDraw->DrawTexture(2, (w - width) / 2, h / 2 - 20, 0, 870, width * GetMaxSEVolume() / 100, 30);

	MikanDraw->Printf(2, w / 2 - 15 * 7, h / 2 - 60, "SE");
	MikanDraw->Printf(2, w / 2 - 15 * 7, h / 2 + 40, "%3d/100", GetMaxSEVolume());

	if (InputRight() % 2 == 1 && GetMaxSEVolume() < 100)
	{
		SetMaxSEVolume(GetMaxSEVolume() + 1);
		MikanSound->SetVolume(10, GetMaxSEVolume());
		MikanSound->SetVolume(11, GetMaxSEVolume());
	}
	if (InputLeft() % 2 == 1 && GetMaxSEVolume() > 0)
	{
		SetMaxSEVolume(GetMaxSEVolume() - 1);
		MikanSound->SetVolume(10, GetMaxSEVolume());
		MikanSound->SetVolume(11, GetMaxSEVolume());
	}
	if (InputAct() == 1)
	{
		SaveConfig(SAVE_SOUND_VOLUME, GetMaxVolume());
		return 1;
	}
	return 0;
}

// キーコンフィグ設定画面
int TenLAN::KeyConfig( unsigned int padnum )
{
	int button;
	int w, h;
	int timer;

	w = GetWidth();
	h = GetHeight();
	timer = GetTimer();

	if( deckey < 0 )
	{
		for( button = 0 ; button < KEY_MAX ; ++button )
		{
			keys[ button ] = -1;
		}
		++deckey;
	}

	MikanDraw->DrawTextureC( IMG_ITEM, w / 2, h / 2 - 200, 460, 760, 480, 80 );

	MikanDraw->DrawTextureC( IMG_ITEM, w / 2, h / 2, 580, 450, 390, 310 );

	if( deckey <= 0 )
	{
		MikanDraw->DrawTextureRotationAngleC( IMG_ITEM, w / 2 + 115, h / 2 - 55, 460, 450, 120, 120, 0.0 );
	}
	if( deckey == 0 && (timer / 30) % 2 == 0 )
	{
		MikanDraw->DrawTextureRotationAngleC( IMG_ITEM, w / 2 + 115, h / 2 - 55, 460, 570, 120, 120, 0.0 );
	}

	if( deckey <= 1 )
	{
		MikanDraw->DrawTextureRotationAngleC( IMG_ITEM, w / 2 + 115, h / 2 - 55, 460, 450, 120, 120, 90.0 );
	}
	if( deckey == 1 && (timer / 30) % 2 == 0 )
	{
		MikanDraw->DrawTextureRotationAngleC( IMG_ITEM, w / 2 + 115, h / 2 - 55, 460, 570, 120, 120, 90.0 );
	}

	if( deckey <= 2 )
	{
		MikanDraw->DrawTextureRotationAngleC( IMG_ITEM, w / 2 + 115, h / 2 - 55, 460, 450, 120, 120, 180.0 );
	}
	if( deckey == 2 && (timer / 30) % 2 == 0 )
	{
		MikanDraw->DrawTextureRotationAngleC( IMG_ITEM, w / 2 + 115, h / 2 - 55, 460, 570, 120, 120, 180.0 );
	}

	if( deckey <= 3 )
	{
		MikanDraw->DrawTextureRotationAngleC( IMG_ITEM, w / 2 + 115, h / 2 - 55, 460, 450, 120, 120, 270.0 );
	}
	if( deckey == 3 && (timer / 30) % 2 == 0 )
	{
		MikanDraw->DrawTextureRotationAngleC( IMG_ITEM, w / 2 + 115, h / 2 - 55, 460, 570, 120, 120, 270.0 );
	}

	if( deckey <= 4 )
	{
		MikanDraw->DrawTextureC( IMG_ITEM, w / 2 - 115, h / 2 - 132, 460, 690, 60, 20 );
	}
	if( deckey == 4 && (timer / 30) % 2 == 0 )
	{
		MikanDraw->DrawTextureC( IMG_ITEM, w / 2 - 115, h / 2 - 132, 460, 710, 60, 20 );
	}

	if( deckey <= 5 )
	{
		MikanDraw->DrawTextureC( IMG_ITEM, w / 2 + 115, h / 2 - 132, 460, 690, 60, 20 );
	}
	if( deckey == 5 && (timer / 30) % 2 == 0 )
	{
		MikanDraw->DrawTextureC( IMG_ITEM, w / 2 + 115, h / 2 - 132, 460, 710, 60, 20 );
	}

	if (deckey <= 6)
	{
		MikanDraw->DrawTextureC(IMG_ITEM, w / 2 - 25, h / 2 - 75, 530, 690, 40, 20);
	}
	if (deckey == 6 && (timer / 30) % 2 == 0)
	{
		MikanDraw->DrawTextureC(IMG_ITEM, w / 2 - 25, h / 2 - 75, 530, 710, 40, 20);
	}

	if (deckey <= 7)
	{
		MikanDraw->DrawTextureC(IMG_ITEM, w / 2 + 25, h / 2 - 75, 530, 690, 40, 20);
	}
	if (deckey == 7 && (timer / 30) % 2 == 0)
	{
		MikanDraw->DrawTextureC(IMG_ITEM, w / 2 + 25, h / 2 - 75, 530, 710, 40, 20);
	}

	if( MikanInput->GetPadWhichButton( padnum ) >= 0 )
	{
		keys[ deckey ] = MikanInput->GetPadWhichButton( padnum );
	}else if( keys[ deckey ] >= 0 )
	{
		if( ++deckey == KEY_MAX )
		{
			SetKeyConfig( padnum, 1 );
			return 1;
		}
	}

	AddTimer();
	return 0;
}

int TenLAN::Command( void )
{
	if( MikanInput->GetKeyNum( K_P ) )
	{
		// パッド再認識
		_MikanInput->RecognitionGamepad( 0 );
	}else if( MikanInput->GetKeyNum( K_R ) )
	{
		// 全テクスチャ再ロード
		_MikanDraw->RecoverFromDeviceLost( 1 );
		//ReleaseTexture();
		//ReloadTexture();
	}else if( MikanInput->GetKeyNum( K_DELETE ) )
	{
	}
	return 0;
}

// タイトル画面でのキーコンフィグ、音量設定の処理のまとめ
int TenLAN::Command_( void )
{
	int w, h;

	w = GetWidth();
	h = GetHeight();

	switch( GetGameMode() )
	{
	case MF_KEYCONFIG:

		MikanDraw->DrawTextureScaling( IMG_OP, 0, 0, 0, 0, 1024, 512, (double)w / 1024 );

		MikanDraw->DrawTextureScalingC( IMG_LOGO, w / 2, 100, 0, 0, 1024, 160, (double)h / 1024 * 0.7 );
		MikanDraw->DrawTextureC( IMG_ITEM, w / 2, h - 150, 460, 0 + 70 * (( GetTimer() / 5 )%6), 540, 70 );

		if( KeyConfig( 0 ) )
		{
			// キーコンフィグ終了。
			SetGameMode( MF_TITLE );
		}
		break;
	case MF_SOUND:
		MikanDraw->DrawTextureScaling( IMG_OP, 0, 0, 0, 0, 1024, 512, (double)w / 1024 );

		MikanDraw->DrawTextureScalingC( IMG_LOGO, w / 2, 100, 0, 0, 1024, 160, (double)w / 1024 * 0.7 );
		MikanDraw->DrawTextureC( IMG_ITEM, w / 2, h - 150, 460, 0 + 70 * (( GetTimer() / 5 )%6), 540, 70 );

		MikanDraw->DrawBox( 0, 0, w, h, 0xcf000000 );

		if( SetVolume() )
		{
			// 効果音設定へ
			SetGameMode( MF_SE );
		}
		break;
	case MF_SE:
		MikanDraw->DrawTextureScaling(IMG_OP, 0, 0, 0, 0, 1024, 512, (double)w / 1024);

		MikanDraw->DrawTextureScalingC(IMG_LOGO, w / 2, 100, 0, 0, 1024, 160, (double)w / 1024 * 0.7);
		MikanDraw->DrawTextureC(IMG_ITEM, w / 2, h - 150, 460, 0 + 70 * ((GetTimer() / 5) % 6), 540, 70);

		MikanDraw->DrawBox(0, 0, w, h, 0xcf000000);

		if (SetVolumeSE())
		{
			// 音量設定終了。
			SetGameMode(MF_TITLE);
		}
		break;
	}
	return 0;
}

// このクラスのインスタンスに保存されたゲームデータを一つ得る
struct GAMEDATA * TenLAN::GetGameData( int gnumber )
{
	if ( gnumber < 0 || gamemax <= gnumber )
	{
		return NULL;
	}
	return &(gamedata[ gnumber ]);
}

int TenLAN::GetWidth( void )
{
	return width;
}
int TenLAN::GetHeight( void )
{
	return height;
}
int TenLAN::GetGameMax( void )
{
	return gamemax;
}
int TenLAN::GetGameMode( void )
{
	return gamemode;
}
int TenLAN::GetTimer( void )
{
	return timer;
}
int TenLAN::GetMaxVolume( void )
{
	return maxvolume;
}
int TenLAN::GetMaxSEVolume(void)
{
	return maxsevolume;
}
int TenLAN::GetEndMode( void )
{
	return end;
}

// セーブ系。
int TenLAN::SetKeyConfig( unsigned int padnum, int save )
{

	if( MikanDraw->DrawBox( 0, 0, GetWidth(), GetHeight(), 0xcf000000 ) )
	{
		// デバイスロストっぽい
		//_MikanDraw->RecoverFromDeviceLost( 1 );
	}

	// 十字キーの設定
	_MikanInput->SetPad2KeyCode( padnum, PAD_UP, K_UP );
	_MikanInput->SetPad2KeyCode( padnum, PAD_DOWN, K_DOWN );
	_MikanInput->SetPad2KeyCode( padnum, PAD_LEFT, K_LEFT );
	_MikanInput->SetPad2KeyCode( padnum, PAD_RIGHT, K_RIGHT );

	// 各ボタンの設定
	// 上のボタン
	_MikanInput->SetPad2KeyCode( padnum, keys[ 0 ], K_A );
	// 右のボタン
	_MikanInput->SetPad2KeyCode( padnum, keys[ 1 ], K_Z );
	// 下のボタン
	_MikanInput->SetPad2KeyCode( padnum, keys[ 2 ], K_X );
	// 左のボタン
	_MikanInput->SetPad2KeyCode( padnum, keys[ 3 ], K_S );

	// L
	_MikanInput->SetPad2KeyCode( padnum, keys[ 4 ], K_D );
	// R
	_MikanInput->SetPad2KeyCode( padnum, keys[ 5 ], K_C );
	// セレクト
	_MikanInput->SetPad2KeyCode(padnum, keys[6], K_F); pad_select = keys[6];
	// スタート
	_MikanInput->SetPad2KeyCode(padnum, keys[7], K_V); pad_start = keys[7];

	if( save )
	{
		SaveConfig( SAVE_PAD, 0 );
	}

	deckey = -1;
	return 0;
}
int TenLAN::SaveConfig( int mode, int arg )
{
	FILE *fp;
	char buf[256], *tok, *str;
	int maxvolume = 100, maxsevolume = 100;
	int pad2keysleep = 20;
	char PAD[64] = "";


	if( fopen_s( &fp, CONFIG_FILE, "r" ) == 0 )
	{
		while ( fgets( buf, 256, fp ) != NULL )
		{
			str = strtok_s( buf, "=", &tok );
			if( strcmp( str, "SOUND_VOLUME" ) == 0 )
			{
				// 音量
				maxvolume = (mode == SAVE_SOUND_VOLUME) ? GetMaxVolume() : atoi(strtok_s(NULL, ",", &tok));
				maxsevolume = (mode == SAVE_SOUND_VOLUME) ? GetMaxSEVolume() : atoi(strtok_s(NULL, ",", &tok));
			}else if( strcmp( str, "PAD" ) == 0 )
			{
				// パッド
				if( mode == SAVE_PAD )
				{
					sprintf_s(PAD, 64, "%d,%d,%d,%d,%d,%d,%d,%d,%d", 0, keys[0], keys[1], keys[2], keys[3], keys[4], keys[5], keys[6], keys[7]);
				}else
				{
					strcpy_s( PAD, 64, strtok_s( NULL, "=", &tok ) );
				}
			}else if( strcmp( str, "PAD2KEY_DIS" ) == 0 )
			{
				// PAD2KEY DISTANCE
				pad2keysleep = ( mode == SAVE_PAD2KEY_DISTANCE ) ? arg : atoi( strtok_s( NULL, "=", &tok ) ) ;
			}
		}

		fclose( fp );
	}else
	{
		// 音量
		maxvolume = ( mode == SAVE_SOUND_VOLUME ) ? arg : GetMaxVolume();
		// パッド
		sprintf_s(PAD, 64, "%d,%d,%d,%d,%d,%d,%d,%d,%d", 0, keys[0], keys[1], keys[2], keys[3], keys[4], keys[5], keys[6], keys[7]);
	}

	if( fopen_s( &fp, CONFIG_FILE, "w" ) == 0 )
	{
		fprintf( fp, "SOUND_VOLUME=%d,%d\r\n", maxvolume, maxsevolume );
		fprintf( fp, "PAD=%s\r\n", PAD );
		fprintf( fp, "PAD2KEY_DIS=%d\r\n", pad2keysleep );
	}
	fclose( fp );
	return 0;
}
int TenLAN::LoadConfig( void )
{
	FILE *fp;
	unsigned int buttonnum, pad;
	int button;
	char buf[ 256 ], *str, *tok;

	pad2keysleep = 10;
	for( button = 0 ; button < 6 ; ++button )
	{
		keys[ button ] = -1;
	}
	keys[ 1 ] = PAD_B;
	keys[ 2 ] = PAD_C;
	pad_select = keys[ 6 ] = PAD_J;
	pad_start = keys[ 7 ] = PAD_I;

	if( fopen_s( &fp, CONFIG_FILE, "r" ) == 0 )
	{
		while ( fgets( buf, 256, fp ) != NULL )
		{
			str = strtok_s( buf, "=", &tok );
			if( strcmp( str, "SOUND_VOLUME" ) == 0 )
			{
				SetMaxVolume( atoi( strtok_s( NULL, ",", &tok ) ) );
				SetMaxSEVolume(atoi(strtok_s(NULL, ",", &tok)));
			}else if( strcmp( str, "PAD" ) == 0 )
			{

				// パッド番号
				pad = atoi( strtok_s( NULL, ",", &tok ) );

				button = 0;
				while( (str = strtok_s( NULL, ",", &tok )) != NULL )
				{
					buttonnum = atoi( str );
					// パッドのキー登録。
					keys[ button ] = buttonnum;
					if (++button >= KEY_MAX){ pad_select = keys[6]; pad_start = keys[7]; break; }
				}

				SetKeyConfig( pad, 0 );
				// 複数設定が有効になれば頑張る。
				break;
			}else if( strcmp( str, "PAD2KEY_DIS" ) == 0 )
			{
				pad2keysleep = atoi( strtok_s( NULL, ",", &tok ) );
				if( pad2keysleep < 1 )
				{
					pad2keysleep = 10;
				}
			}

		}
		fclose( fp );
	}

	return 0;
}


// システムが使うメソッド。
int TenLAN::SetWindowSize( int w, int h )
{
	width = w;
	height = h;
	return 0;
}
int TenLAN::SetPad2Key( unsigned int padnum, int flag )
{
	// PAD2Key有効化。
	_MikanInput->SetPad2KeyInput( flag );
	_MikanInput->RecognitionGamepad( 0 );
	SetKeyConfig( 0, 0 );
	return 0;
}
int TenLAN::SetGameMax( int max )
{
	gamemax = max;
	return 0;
}
int TenLAN::SetGameData( struct GAMEDATA *gd )
{
	gamedata = gd;
	return 0;
}
int TenLAN::SetGameMode( int mode )
{
	gamemode = mode;
	return 0;
}
int TenLAN::SetTimer( int newtimer )
{
	timer = newtimer;
	return 0;
}
int TenLAN::AddTimer( void )
{
	++timer;
	return timer;
}
int TenLAN::SetMaxVolume( int newmaxvolume )
{
	maxvolume = newmaxvolume;
	return 0;
}
int TenLAN::SetMaxSEVolume(int newmaxvolume)
{
	maxsevolume = newmaxvolume;
	return 0;
}
int TenLAN::SetEndMode( int newend )
{
	end = newend;
	return 0;
}

int TenLAN::InputUp( void )
{
	return MikanInput->GetKeyNum( K_UP );
}

int TenLAN::InputDown( void )
{
	return MikanInput->GetKeyNum( K_DOWN );
}

int TenLAN::InputRight( void )
{
	return MikanInput->GetKeyNum( K_RIGHT );
}

int TenLAN::InputLeft( void )
{
	return MikanInput->GetKeyNum( K_LEFT );
}

int TenLAN::InputAct( void )
{
	return GetEndMode() == 0 ? MikanInput->GetKeyNum( K_Z ) : 0;
}

int TenLAN::InputCancel( void )
{
	return GetEndMode() == 0 ? MikanInput->GetKeyNum( K_X ) : 0;
}

int TenLAN::InputSelect(void)
{
	return GetEndMode() == 0 ? MikanInput->GetKeyNum(K_F) : 0;
}

int TenLAN::InputStart(void)
{
	return GetEndMode() == 0 ? MikanInput->GetKeyNum(K_V) : 0;
}

int TenLAN::InputESC( void )
{
	return MikanInput->GetKeyNum( K_ESC );
}

// パッドのセレクトボタンとして割り当てられているボタンの番号を渡す
// 入力状態を得られるわけではない
int TenLAN::PadSelect( void )
{
	return pad_select;
}

// パッドのスタートボタンとして割り当てられているボタンの番号を渡す
// 入力状態を得られるわけではない
int TenLAN::PadStart( void )
{
	return pad_start;
}

// 現在デバッグモードかどうかを返す
int TenLAN::DebugMode(void)
{
	return debugmode;
}
