//Mikan���C�u����
#include "common.h"
#include "UI\UI.h"

//Ten-LAN�֘A�̊֐��Ƃ��̒�`
#include "Ten-LAN.h"

#define TENLANVERSION "3.0"
#define KEEP_FOR_FINISH 60 // �Q�[���I���̂��߂ɒ��������鎞��(�P��:�t���[��)

unsigned long CATE_COLOR[ CATEGORY_MAX ] =
{
	0x00ff2828, // �A�N�V����
	0x00ff0091, // �V���[�e�B���O
	0x00dbae00, // �X�|�[�c
	0x00c2cb10, // ���[�X
	0x0016f000, // �p�Y��
	0x0000a411, // �e�[�u��
	0x00008f9a, // �V�~�����[�V����
	0x00004eff, // ���[���v���C���O
	0x00cf00ff, // ���l��
	0x00f400ff, // �~�j�Q�[��
	0x00808080  // ���̑�
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

time_t starttime;// �Q�[���J�n����

int date[3];//���t

// ���v�f�[�^�ǂݍ���(�Q�[�����X�g��ǂݍ��񂾌��)
int GetStatistics()
{
	time_t now;
	struct tm t;
	FILE* file;
	char stfile[512];
	int gamemax = sys.tenlan->GetGameMax();

	// �����̓��t���擾
	time(&now);
	localtime_s(&t, &now);
	date[0] = t.tm_mon + 1;
	date[1] = t.tm_mday;
	date[2] = t.tm_year + 1900;

	sprintf_s(stfile, 512, "%s\\%s_total%s", sys.CurDir, STATISTICS_FILE,STATISTICS_FILETYPE);

	// �܂��͓��v�̍��v��ǂݍ���
	// ���v�f�[�^��ۑ������t�@�C�����J��
	if (fopen_s(&file,stfile,"r") == 0)
	{
		int check;
		while (1)
		{
			int number;// �Q�[�����ʔԍ��p
			int pnum;// �v���C�񐔗p
			int total_h = 0, total_m = 0, total_s = 0;// ���v���C���ԗp
			int ave_m = 0, ave_s = 0;// ���ώ��ԗp

			char title[2048];// �^�C�g���p

			// �ԍ� �Q�[���� �v���C�� ���v���C����(���ԁA���A�b) ���σv���C����(���A�b)
			check = fscanf_s(file, "%d %s %d�� %d:%d:%d %d:%d",
				&number, title, sizeof(title), &pnum, &total_h, &total_m, &total_s,
				&ave_m, &ave_s);

			// �ǂݍ��݂��I���ΏI��
			if (check == EOF)
			{
				break;
			}

			// �ǂݍ��񂾃Q�[���ɂ��̔ԍ��̂��̂�T���ăf�[�^���
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

	// ���ɂ��̓��̓��v��ǂݍ���
	// ���v�f�[�^��ۑ������t�@�C�����J��
	if (fopen_s(&file, stfile, "r") == 0)
	{
		int check;
		while (1)
		{
			int number;// �Q�[�����ʔԍ��p
			int pnum;// �v���C�񐔗p
			int total_h = 0, total_m = 0, total_s = 0;// ���v���C���ԗp
			int ave_m = 0, ave_s = 0;// ���ώ��ԗp

			char title[2048];// �^�C�g���p

			// �ԍ� �Q�[���� �v���C�� ���v���C����(���ԁA���A�b) ���σv���C����(���A�b)
			check = fscanf_s(file, "%d %s %d�� %d:%d:%d %d:%d",
				&number, title, sizeof(title), &pnum, &total_h, &total_m, &total_s,
				&ave_m, &ave_s);

			// �ǂݍ��݂��I���ΏI��
			if (check == EOF)
			{
				break;
			}

			// �ǂݍ��񂾃Q�[���ɂ��̔ԍ��̂��̂�T���ăf�[�^���
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

// ���v�f�[�^�̕ۑ�
int SaveStatistics()
{
	FILE* file;
	char stfile[512];

	int gamemax = sys.tenlan->GetGameMax();

	//�܂��͓��v�̍��v��ۑ�
	sprintf_s(stfile, 512, "%s\\%s_total%s", sys.CurDir, STATISTICS_FILE, STATISTICS_FILETYPE);

	if (fopen_s(&file, stfile, "w") == 0)
	{
		for (int n = 0; n < gamemax; n++)
		{
			// ���σv���C����(�b)
			int ave = 0;
			if (gd[n].playnum_total > 0)
			{
				ave = gd[n].playtime_total / gd[n].playnum_total;
			}

			// �^�C�g��(���s�R�[�h���C��)
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

			// �ԍ� �Q�[���� �v���C�� ���v���C����(���ԁA���A�b) ���σv���C����(���A�b)
			fprintf_s(file, "%d %s %03d�� %02d:%02d:%02d %02d:%02d\n\0",
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
			// ���σv���C����(�b)
			int ave = 0;
			if (gd[n].playnum > 0)
			{
				ave = gd[n].playtime / gd[n].playnum;
			}

			// �^�C�g��(���s�R�[�h���C��)
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

			// �ԍ� �Q�[���� �v���C�� ���v���C����(���ԁA���A�b) ���σv���C����(���A�b)
			fprintf_s(file, "%d %s %03d�� %02d:%02d:%02d %02d:%02d\n",
				gd[n].num, title, gd[n].playnum,
				gd[n].playtime / 3600, (gd[n].playtime % 3600) / 60, gd[n].playtime % 60,
				ave / 60, ave % 60);
		}

		fclose(file);
	}
	return 1;
}

// ���t�`�F�b�N
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
	// ���t���ς���Ă��Ȃ����`�F�b�N
	for (int i = 0; i < 3; i++)
	{
		if (date[i] != ndate[i])
		{
			// ���t���X�V
			for (int j = i; j < 3; j++)
			{
				date[j] = ndate[j];
			}

			//�Q�[���̓��v��������
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

// �Q�[�����X�g�̏�����
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
				//�f�B���N�g��
				if ( strcmp( status.cFileName, "." ) != 0 &&
				     strcmp( status.cFileName, ".." ) != 0)
				{
					sprintf_s( filepath, 1023, "%s\\%s\\%s", GAMEDIR, status.cFileName, SETTING );
					stat( filepath, &fstat );
					if ( MikanFile->Open( 0, filepath, "r" ) >= 0 )
					{
						buf = ( char * )calloc( 2048, sizeof( char ) );
						// ������
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
								// �J�e�S���ԍ����擾�B
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
						gd[msel].playnum = 0;// ���v�p�f�[�^������
						gd[msel].playtime = 0;// ���v�p�f�[�^������
						gd[msel].playnum_total = 0;// ���v�p�f�[�^������
						gd[msel].playtime_total = 0;// ���v�p�f�[�^������
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

// �E�B���h�E������
int Init( void )
{
	//�E�B���h�E�n���h��
	HWND myhwnd = NULL;
	//RECT�\����
	RECT myrect;

	//�Q�[���p�b�h�ԍ��̏�����
	sys.GAMEPAD = -1;

	//�f�X�N�g�b�v�̃E�B���h�E�n���h���擾
	myhwnd = GetDesktopWindow();
	if ( myhwnd && GetClientRect( myhwnd, &myrect ) )
	{
		//�f�X�N�g�b�v�̑傫���̎擾
		//sys.width  = myrect.right;
		//sys.height = myrect.bottom;
		sys.tenlan->SetWindowSize( myrect.right, myrect.bottom );
	}

	_MikanWindow->SetWindow( WT_NORESIZEFULLSCREEN );
	return 0;
}

//�E�B���h�E�̃T�C�Y���Z�b�g������
//�����`���[���A��ɍs��
int ResetWindow(void)
{
	//�E�B���h�E�n���h��
	HWND myhwnd = NULL;
	//RECT�\����
	RECT myrect;

	//�f�X�N�g�b�v�̃E�B���h�E�n���h���擾
	myhwnd = GetDesktopWindow();
	if (myhwnd && GetClientRect(myhwnd, &myrect))
	{
		//�f�X�N�g�b�v�̑傫���̎擾
		//sys.width  = myrect.right;
		//sys.height = myrect.bottom;
		sys.tenlan->SetWindowSize(myrect.right, myrect.bottom);
	}

	_MikanWindow->SetPositionXY( 0, 0 );
	_MikanWindow->SetWindow();
return 0;
}

// ����E�B���h�E�̍ő剻
int FullWindow(HWND hwnd)
{
	//�E�B���h�E�n���h��
	HWND myhwnd = NULL;
	//RECT�\����
	RECT myrect;
	RECT rect;
	RECT newrect;

	//�f�X�N�g�b�v�̃E�B���h�E�n���h���擾
	myhwnd = GetDesktopWindow();

	// �f�X�N�g�b�v�̑傫���A�E�B���h�E�̑傫���擾
	if (myhwnd && hwnd && GetClientRect(myhwnd, &myrect) &&
		GetWindowRect(hwnd, &rect))
	{
		int x,y,width, height;

		// �Q�[���E�B���h�E�̑傫���擾
		double w = rect.right - rect.left;
		double h = rect.bottom - rect.top;

		//�f�X�N�g�b�v�̑傫���̎擾
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

//�E�B���h�E�̈ʒu�C��
int ModifyPos(HWND hwnd)
{
	//�E�B���h�E�n���h��
	HWND myhwnd = NULL;
	//RECT�\����
	RECT myrect;
	RECT rect;
	RECT rectclient;

	//�f�X�N�g�b�v�̃E�B���h�E�n���h���擾
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

// �E�B���h�E�n���h���̎擾
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

// �^�X�N�o�[�̉B��
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
			hWnd = FindWindowEx( NULL, NULL, "Button", "�X�^�[�g" );
		}else
		{
			hWnd = FindWindow( "Button", "Start" );
		}
		ShowWindow( hWnd, SW_HIDE );
		return 0;
	}
	return 1;
}

// �^�X�N�o�[�̕���
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
		hWnd = FindWindowEx( NULL, NULL, "Button", "�X�^�[�g" );
	}else
	{
		hWnd = FindWindow( "Button", "Start" );
	}
	ShowWindow( hWnd, SW_RESTORE );
	return 0;
}

//�Q�[���̎��s
int ExecProgram( int num )
{
	char *exe, dir[512];
	DWORD startpid, errcode;
	HWND hw;

	// ���t�`�F�b�N
	CheckDate();

	sys.gamewin = NULL;
	// ���[�h�ύX
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
	// �Q�[���̂���f�B���N�g�����J�����g�f�B���N�g����
	if ( SetCurrentDirectory( dir ) )
	{
		//  *exe = '\\';
		++exe;

		// �p�b�h�ɃL�[�����蓖��
		if ( gd[ num ].pad2key )
		{
			sys.tenlan->SetPad2Key( 0, gd[ num ].pad2key );
		}

		// �L�[�̓��͂����O����Ȃ���
		_MikanInput->UpdateKeyInput();
		_MikanInput->UpdateKeyInput();

		//memset( &(sys.pi), 0, sizeof( PROCESS_INFORMATION ) );

		// �Q�[���N��
		if ( CreateProcess(NULL,exe,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,NULL,NULL,&(sys.si),&(sys.pi)) )
		{
			//sys.gamewin = GetWindowHandle( sys.pi.dwProcessId );
			CountUp( gd[ num ].num );
			startpid = sys.pi.dwProcessId;

			// �V��ł���Q�[�����z��̉��Ԗڂ���^����
			sys.currentgame = num;

			// �Q�[���J�n�����擾
			time(&starttime);

			// �^�C�����O����̂ł����Ńe�N�X�`�����
			//ReleaseTexture();

			// �N�������Q�[���̃E�B���h�E�n���h���擾
			do
			{
				hw = GetWindowHandle( startpid );
			} while( hw == NULL );
			FullWindow(hw);
			sys.gamewin = hw;
			SetForegroundWindow( sys.gamewin );// �Q�[���̃E�B���h�E��O��
			// �܂��p�b�h�ւ̃L�[�̊��蓖�Ă��Q�[������폜
			_MikanInput->AttachPad2Key( sys.gamewin, FALSE );
			// ���̂��ƃp�b�h�ւ̃L�[�̊��蓖�Ă��Q�[���֓K�p
			if ( gd[ num ].pad2key )
			{
				_MikanInput->AttachPad2Key( sys.gamewin );
				//hw = MikanWindow->GetWindowHandle();
				SetForegroundWindow( sys.gamewin );
			}

			sys.MODE = 1;
		}
		// �Q�[���N�����s
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

//�Q�[���I�����̏���
int CheckEndProcess(void)
{
	unsigned long ExitCode;
	time_t now;

	if ( sys.process == 0 )
	{
		GetExitCodeProcess( sys.pi.hProcess, &ExitCode );

		// �Q�[�����܂������Ă���Ȃ�
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

	// �E�B���h�E���t���X�N���[����
	ResetWindow();
	//_MikanWindow->SetWindow(WT_NORESIZEFULLSCREEN);
	//MikanSystem->SetInactiveWindow(0);

	// �J�����g�f�B���N�g����Ten-LAN�̂���ꏊ��
	SetCurrentDirectory( sys.CurDir );

	// �f�o�b�O���[�h�łȂ����
	// �v���C���Ԃ𑪒肵�ĉ��Z����
	if (sys.tenlan->DebugMode() == 0)
	{
		// �v���C�񐔂𑝂₷
		gd[sys.currentgame].playnum++;
		gd[sys.currentgame].playnum_total++;

		time(&now);// ���ݎ����擾

		// �J�n���ԂƂ̍������v�Z���ĉ��Z
		gd[sys.currentgame].playtime += static_cast<long>(now - starttime);
		gd[sys.currentgame].playtime_total += static_cast<long>(now - starttime);

		// ���v��ۑ�
		SaveStatistics();
	}

	sys.currentgame = -1;

	return 1;
}

// Ten-Lan�I������
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
				//�Q�[���I��
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

// �f�o�C�X���X�g���A��̏���
int AfterDeviceLost( void )
{
	// �摜�̎擾���Ȃ���
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

// �o�[�W�����̕\��
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

//�E�B���h�E�����O��1�x�������s
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

	// �f�o�C�X���X�g��̕��A�����ݒ�
	_MikanSystem->SetAfterDeviceLostFunction( AfterDeviceLost );
	ShowCursor( 0 );
}

//DirectX���������1�x�������s
void UserInit( void )
{
	MikanDraw->CreateFont( 1, 10, 0xff000000 );
	MikanDraw->CreateFont( 2, 30, 0xffffffff );

	_MikanSystem->SetPermitScreenSaver( 0 );

	//sys.maxvolume = 100;
	// config.txt���Ȃ��Ƃ��̂��߂ɉ��ʏ�����
	// �����傫���ƓW�����̓Q�[���̕��̉��ʂ��������Ȃ��Ȃ��Ă��܂��A
	// �ڗ����Ȃ��Ȃ��Ă��܂��̂ŉ��ʂ�20(�ő��100)
	sys.tenlan->SetMaxVolume(20);
	sys.tenlan->SetMaxSEVolume(20);

	do
	{
		MikanDraw->CreateTexture( START_SS, NULL, readfile[ START_SS ] );
	} while( *readfile[ ++START_SS ] );
	START_SS = 10;
	sys.MODE = 0;
	//sys.end = 0;

	// �Q�[�����X�g�̏�����
	InitGamelist();

	// ���݂̃J�����g�f�B���N�g��(�܂�Ten-LAN�̂���f�B���N�g��)��
	// sys.CurDir��
	GetCurrentDirectory( 1023, sys.CurDir );

	// �Q�[���̓��v�f�[�^�ǂݍ���
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
	// UI�͂����Ŏw�肷��B
	// UI��UIBase�N���X���p�������N���X�ł���Ή��ł��ǂ��B
	sys.ui = new UI_2015( sys.tenlan );//new UI_Hiroki();//new UI_Tour();

	// config.txt�ǂݍ���
	sys.tenlan->LoadConfig();

	// UI������
	sys.ui->Init();

	// �^�C�g����ʂ���n�܂�
	sys.tenlan->SetGameMode(MF_TITLE);

	// BGM������
	MikanSound->Load( 0, NULL, "BGM_TITLE" );
	MikanSound->SetVolume( 0, sys.tenlan->GetMaxVolume() );
	MikanSound->Play( 0, true );

	sys.searchserver = 0;
	MikanSystem->CreateLock( SERVER_LOCK );
	SearchTenLanServer();

	sys.tenlan->SetPad2Key( 0, 1 );

	SetForegroundWindow( MikanWindow->GetWindowHandle() );
}

//1�b�Ԃ�60����s�����
int MainLoop( void )
{
	MikanDraw->ClearScreen( 0xffffffff );

	// �Q�[�������s����Ă��郂�[�h
	if( sys.MODE )
	{
		// �Q�[�����I�������Ƃ��̏���
		if( CheckEndProcess() )
		{
			Sleep( 100 );
			sys.MODE = 0;
			//ReloadTexture();
			//ResetWindow();
			//_MikanDraw->SetScreenSize( sys.width, sys.height );
			_MikanDraw->SetScreenSize( sys.tenlan->GetWidth(), sys.tenlan->GetHeight() );
			// Pad2Key�̃o�b�t�@���J���ƌ������ADown����Up���ĂȂ����Ԃ�h���B
			_MikanInput->UpdateKeyInput();
			// Ten-LAN��Pad2Key�L���Ȃ̂Ō��ɖ߂��B
			sys.tenlan->SetPad2Key( 0, 1 );
			sys.ui->Init();
		}
		// �Q�[�������s����Ă���Ԃ̏���
		else if( sys.gamewin != NULL )
		{
			// �E�B���h�E�̈ʒu�C��
			ModifyPos(sys.gamewin);

			// �X�^�[�g + �Z���N�g�������ςȂ��ŃQ�[���I��
			if (MikanInput->GetPadNum(0, sys.tenlan->PadSelect()) > KEEP_FOR_FINISH &&
				MikanInput->GetPadNum(0, sys.tenlan->PadStart()) > KEEP_FOR_FINISH)
			{
				SendMessage(sys.gamewin, WM_SYSCOMMAND, SC_CLOSE, 0);
			}

			// �Q�[����ʂ���O��
			SetForegroundWindow(sys.gamewin);
		}
		sys.ui->View();

	}
	// �Q�[�������s����Ă��Ȃ����[�h
	else
	{
		// �Q�[��������Ƃ��̏���
		if ( sys.tenlan->GetGameMax() )
		{
			/*if( InputAct()>0)
			{
			sys.modeflag=MF_PLAY;
			ExecProgram( 0 );
			}*/
			sys.ui->View();

			// �Q�[���̊m�F��ʋy�ю��s���ł͂Ȃ�
			if( sys.tenlan->GetGameMode() <= MF_SELECT )
			{
				//SetForegroundWindow( MikanWindow->GetWindowHandle() );
			}
			// �Q�[�����N�����Ă���ꍇ�̏���
			else if( sys.gamewin != NULL )
			{
				// �E�B���h�E�̈ʒu�C��
				ModifyPos(sys.gamewin);

				// �X�^�[�g + �Z���N�g�������ςȂ��ŃQ�[���I��
				if (MikanInput->GetPadNum(0, sys.tenlan->PadSelect()) > KEEP_FOR_FINISH &&
					MikanInput->GetPadNum(0, sys.tenlan->PadStart()) > KEEP_FOR_FINISH)
				{
					SendMessage(sys.gamewin, WM_SYSCOMMAND, SC_CLOSE, 0);
				}

				// �Q�[����ʂ���O��
				SetForegroundWindow(sys.gamewin);
			}

		}
		// �Q�[�����Ȃ��ꍇ�̏���
		else
		{
			if( sys.tenlan->InputAct() == 1 )
			{
				if( sys.error == 0 )
				{
					//�Q�[���I��
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

//�Ō�Ɉ�x�������s�����
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

