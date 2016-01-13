#include "UI.h"  

#define SSANIME_TIME 60

#define INFOBAR_COLOR 0x7fefefef


UI_Hiroki_Metro::UI_Hiroki_Metro( class TenLANBase *tenlanbase )
{
	int height;

	SetTenLAN( tenlanbase );

	rtimer = 0;
	waitsecond = 30;

	height = tenlan->GetHeight();

	infobar_height = height / 10;
	icon_size = height * 4 / 10;

	sparse_icon = height / 30;

	sparse_game = ( icon_size - (icon_size * 3 / 4) ) / 2;

	w_num = (tenlan->GetWidth()-sparse_icon) / (icon_size + sparse_icon) + 1;

	// ゲームの数 / 2 + 2で割った余り
	select_w = sys.tenlan->GetGameMax() / 2 + sys.tenlan->GetGameMax() % 2;
	select_x = 0;
	select_y = 0;
	select_size = sparse_icon / 2;
	move_x = 0;
	s_x = 0;

	info_h = height * 9 / 10;
	info_w = info_h * 4 / 3;
	info_x = ( tenlan->GetWidth() - info_w ) / 2;
	info_y = ( height - info_h ) / 2;

	cate_h = tenlan->GetWidth() / 20;
	img_h = info_h - infobar_height * 2 - infobar_height / 5 - cate_h;
	img_w = img_h * 4/ 3;
	img_x = info_x + (info_w - img_w) / 2;
	img_y = info_y + infobar_height + infobar_height / 10;

	info_bt_size = ( info_x - img_x ) * 9 / 10;

	_MikanDraw->CreatePolygon( 0, 7, ::D3DPT_TRIANGLEFAN );
	_MikanDraw->SetPolygonVertex( 0, 0, 0, INFOBAR_COLOR );
	_MikanDraw->SetPolygonVertex( 0, 200, 0, INFOBAR_COLOR );
	_MikanDraw->SetPolygonVertex( 0, 200, 50, INFOBAR_COLOR );
	_MikanDraw->SetPolygonVertex( 0, 100, 50, INFOBAR_COLOR );
	_MikanDraw->SetPolygonVertex( 0, 50, 100, INFOBAR_COLOR );
	_MikanDraw->SetPolygonVertex( 0, 50, 200, INFOBAR_COLOR );
	_MikanDraw->SetPolygonVertex( 0, 0, 200, INFOBAR_COLOR );

	font_size = infobar_height - 10;
	MikanDraw->CreateFont( 3, font_size, 0xff000000 );

	info_num = 0;

	enable_input = 0;
}

char * UI_Hiroki_Metro::UIName( void )
{
	if( sys.tenlan->GetGameMode() < MF_SELECT )
	{
		// タイトル画面
		return "Metro BGM:BigPresent Composed by Grow";
	}else
	{
		// その他プレイ中
		return "Metro";
	}
}

int UI_Hiroki_Metro::Init( void )
{
	tnum = 4;
	MikanDraw->CreateTexture( tnum, "material/metro.png" );
	MikanDraw->CreateTexture( tnum + 1, tenlan->GetWidth(), infobar_height * 2 );

	sstimer = 0;
	istimer = 0;

	enable_input = 1;

	sys.tenlan->SetGameMode( MF_SELECT );
	return 0;
}

int UI_Hiroki_Metro::BackGround( void )
{
	MikanDraw->DrawBox( 0, 0, tenlan->GetWidth(), tenlan->GetHeight(), 0xff000000 );
	return 0;
}

int UI_Hiroki_Metro::DrawMotionBox( int x, int y, int width, int height, double prog )
{
	if ( prog < 0.5 )
	{
		_MikanDraw->EditPolygonVertex( 0, 1, width * prog * 2.0, 0.0, 0, 0, 0, PVEDIT_XY );
		_MikanDraw->EditPolygonVertex( 0, 2, width * prog * 2.0, 1.0, 0, 0, 0, PVEDIT_XY );
		_MikanDraw->EditPolygonVertex( 0, 3, 1.0, 1.0, 0, 0, 0, PVEDIT_XY );
		_MikanDraw->EditPolygonVertex( 0, 4, 1.0, 1.0, 0, 0, 0, PVEDIT_XY );
		_MikanDraw->EditPolygonVertex( 0, 5, 1.0, height * prog * 2.0, 0, 0, 0, PVEDIT_XY );
		_MikanDraw->EditPolygonVertex( 0, 6, 0.0, height * prog * 2.0, 0, 0, 0, PVEDIT_XY );
	} else if( prog < 0.75 )
	{
		_MikanDraw->EditPolygonVertex( 0, 1, width, 0.0, 0, 0, 0, PVEDIT_XY );
		_MikanDraw->EditPolygonVertex( 0, 2, width, 1.0, 0, 0, 0, PVEDIT_XY );
		_MikanDraw->EditPolygonVertex( 0, 3, width * (prog-0.5)*4.0, 1.0, 0, 0, 0, PVEDIT_XY );
		_MikanDraw->EditPolygonVertex( 0, 4, 1.0, height * (prog-0.5)*4.0, 0, 0, 0, PVEDIT_XY );
		_MikanDraw->EditPolygonVertex( 0, 5, 1.0, height, 0, 0, 0, PVEDIT_XY );
		_MikanDraw->EditPolygonVertex( 0, 6, 0.0, height, 0, 0, 0, PVEDIT_XY );
	} else
	{
		_MikanDraw->EditPolygonVertex( 0, 1, width, 0.0, 0, 0, 0, PVEDIT_XY );
		_MikanDraw->EditPolygonVertex( 0, 2, width, 1.0, 0, 0, 0, PVEDIT_XY );
		_MikanDraw->EditPolygonVertex( 0, 3, width, height * (prog-0.75)*4.0, 0, 0, 0, PVEDIT_XY );
		_MikanDraw->EditPolygonVertex( 0, 4, width * (prog-0.75)*4.0, height, 0, 0, 0, PVEDIT_XY );
		_MikanDraw->EditPolygonVertex( 0, 5, 1.0, height, 0, 0, 0, PVEDIT_XY );
		_MikanDraw->EditPolygonVertex( 0, 6, 0.0, height, 0, 0, 0, PVEDIT_XY );
	}
	//_MikanDraw->DrawPolygon( 0, x, y );
	return 0;
}



int UI_Hiroki_Metro::SelectScreen( int enable_input )
{
	double prog;
	int i;
	int bx, by, base_x;
	int tx;
	int select_size;

	if ( sstimer > SSANIME_TIME )
	{
		base_x = s_x + move_x;
		select_size = (int)((double)this->select_size * ((double)( (double)(sstimer % 30) / 60.0 + 0.5)));

		for ( i = 0 ; i < select_w ; ++i )
		{
			tx = base_x + i * ( sparse_icon + icon_size ) + sparse_icon;

			if( 0 < tx + icon_size || tx < tenlan->GetWidth() )
			{
				MikanDraw->DrawBox( base_x + i * ( sparse_icon + icon_size ) + sparse_icon, sparse_icon, icon_size, icon_size, 0xff000000 | CATE_COLOR[ gd[ i * 2 ].category[ 0 ] ] );//INFOBAR_COLOR );
				MikanDraw->DrawTextureScaling( gd[ i * 2 ].txnum, base_x + i * ( sparse_icon + icon_size ) + sparse_icon, sparse_icon + sparse_game, 0, 0, 640, 480, icon_size, icon_size * 3 / 4 );
				//        if ( i * 2 + 1 < sys.gamemax )
				if ( i * 2 + 1 < sys.tenlan->GetGameMax() )
				{
					MikanDraw->DrawBox( base_x + i * ( sparse_icon + icon_size ) + sparse_icon, sparse_icon * 2 + icon_size, icon_size, icon_size, 0xff000000 | CATE_COLOR[ gd[ i * 2 + 1 ].category[ 0 ] ] );//INFOBAR_COLOR );
					MikanDraw->DrawTextureScaling( gd[ i * 2 + 1 ].txnum, base_x + i * ( sparse_icon + icon_size ) + sparse_icon, sparse_icon * 2 + icon_size + sparse_game, 0, 0, 640, 480, icon_size, icon_size * 3 / 4 );
				}
			}
		}

		bx = base_x + (select_x ) * ( sparse_icon + icon_size ) + sparse_icon;
		by = select_y * ( sparse_icon + icon_size ) + sparse_icon;

		// 選択。
		// 四隅。
		MikanDraw->DrawTextureScaling( tnum, bx - select_size, by - select_size, 100, 0, 5, 5, select_size, select_size );
		MikanDraw->DrawTextureScaling( tnum, bx + icon_size, by - select_size, 145, 0, 5, 5, select_size, select_size );
		MikanDraw->DrawTextureScaling( tnum, bx - select_size, by + icon_size, 100, 45, 5, 5, select_size, select_size );
		MikanDraw->DrawTextureScaling( tnum, bx + icon_size, by + icon_size, 145, 45, 5, 5, select_size, select_size );
		// 四辺。
		MikanDraw->DrawTextureScaling( tnum, bx, by - select_size, 105, 0, 1, 5, icon_size, select_size );
		MikanDraw->DrawTextureScaling( tnum, bx - select_size, by, 100, 5, 5, 1, select_size, icon_size );
		MikanDraw->DrawTextureScaling( tnum, bx, by + icon_size, 105, 45, 1, 5, icon_size, select_size );
		MikanDraw->DrawTextureScaling( tnum, bx + icon_size, by, 145, 5, 5, 1, select_size, icon_size );

		// 情報バー。
		MikanDraw->DrawBox( 0, tenlan->GetHeight() - infobar_height, tenlan->GetWidth(), infobar_height, 0x7fefefef );

		++sstimer;

		if( enable_input && move_x == 0 )
		{
			//if ( InputAct() == 1 && select_x * 2 + select_y < sys.gamemax )
			if ( sys.tenlan->InputAct() == 1 && select_x * 2 + select_y < sys.tenlan->GetGameMax() )
			{
				istimer = 0;
				// ゲーム詳細画面へ
				//sys.modeflag = MF_OKAY;
				sys.tenlan->SetGameMode( MF_OKAY );
			}else if ( sys.tenlan->InputLeft() % 30 == 1 && 0 < select_x )
			{
				// 左に行く事は可能
				--select_x;
				tx = base_x + (select_x ) * ( sparse_icon + icon_size ) + sparse_icon;
				if( tx < 0 )
				{
					// 左にはみ出すのでスクロール開始。
					move_x = (icon_size + sparse_icon) / SSANIME_TIME;
				}

			} else if ( sys.tenlan->InputRight() %30 == 1 && select_x + 1 < select_w )
			{
				// 右に行く事は可能。
				++select_x;

				tx = base_x + (select_x ) * ( sparse_icon + icon_size ) + sparse_icon;
				if( tenlan->GetWidth() < tx + icon_size )
				{
					// 右にはみ出すのでスクロール開始。
					move_x = -(icon_size + sparse_icon) / SSANIME_TIME;
				}
			}else if ( sys.tenlan->InputUp() % 30 == 1 || sys.tenlan->InputDown() % 30 == 1 )
			{
				select_y = ( select_y + 1 ) % 2;
			}
		} else
		{

			if ( move_x < 0 )
			{
				// 選択ゲームのアイコンの右側を基準にする。
				bx = base_x + (select_x) * ( sparse_icon + icon_size ) + sparse_icon;
				if ( bx + icon_size + sparse_icon > tenlan->GetWidth() )
				{
					move_x -= (icon_size + sparse_icon) / (SSANIME_TIME/4);
				} else
				{
					s_x += move_x;
					move_x = 0;
				}
			} else
			{
				// 選択ゲームのアイコンの左側を基準にする。
				bx = base_x + (select_x) * ( sparse_icon + icon_size ) + sparse_icon;
				if ( bx < 0 )
				{
					move_x += (icon_size + sparse_icon) / (SSANIME_TIME/4);
				} else
				{
					s_x += move_x;
					move_x = 0;
				}
			}
		}

	} else
	{
		base_x = s_x;
		prog = (double)sstimer / (double)SSANIME_TIME;

		// ゲームたち
		for ( i = 0 ; i < select_w ; ++i )
		{
			tx = base_x + i * ( sparse_icon + icon_size ) + sparse_icon;

			if( 0 < tx + icon_size || tx < tenlan->GetWidth() )
			{
				DrawMotionBox( tx, sparse_icon, icon_size, icon_size, prog );
				DrawMotionBox( tx, sparse_icon * 2 + icon_size, icon_size, icon_size, prog );
			}
		}

		// 情報バー
		DrawMotionBox( 0, tenlan->GetHeight() - infobar_height, tenlan->GetWidth(), infobar_height, prog );

		++sstimer;
	}

	return 0;
}

int UI_Hiroki_Metro::InfoScreen( int enable_input )
{
	double prog;
	int info_max;
	int n;

	//MikanDraw->DrawBox( 0, 0, sys.width, sys.height, 0xaf000000 );
	MikanDraw->DrawTextureScaling( tnum, 0, 0, 0, 50, 50, 50, tenlan->GetWidth(), tenlan->GetHeight() );

	if ( istimer > SSANIME_TIME )
	{
		info_max = MikanDraw->GetTextureHeight( gd[ select_x * 2 + select_y ].txnum ) / 480;

		MikanDraw->DrawBox( info_x, info_y, info_w, info_h, 0x7f000000 | CATE_COLOR[ gd[ select_x * 2 + select_y ].category[ 0 ] ] );//INFOBAR_COLOR );

		MikanDraw->DrawBox( info_x, info_y, info_w, infobar_height, INFOBAR_COLOR );
		MikanDraw->Printf( 3, info_x, info_y, "%s", gd[ select_x * 2 + select_y ].title );

		MikanDraw->DrawBox( info_x, info_y + info_h - infobar_height, info_w, infobar_height, INFOBAR_COLOR );
		MikanDraw->Printf( 3, info_x, info_y + info_h - infobar_height, "%s", gd[ select_x * 2 + select_y ].text );

		// インスト画像
		MikanDraw->DrawBox( img_x, img_y, img_w, img_h, INFOBAR_COLOR );
		//    MikanDraw->DrawTextureScaling( gd[ select_x * 2 + select_y ].txnum, img_x, img_y, 0, info_num * 480, 640, 480, img_w, img_h );
		MikanDraw->DrawTextureScaling( gd[ select_x * 2 + select_y ].txnum, img_x, img_y, 0, info_num * 480, 640, 480, img_w, img_h );
		if ( info_num > 0 )
		{
			MikanDraw->DrawTextureScalingC( 4, img_x - 60, img_y + img_h / 2, 0, 0, 50, 50, 2.0 );
		}
		if ( info_num + 1 < info_max )
		{
			MikanDraw->DrawTextureScalingC( 4, img_x + img_w + 60, img_y + img_h / 2, 50, 0, 50, 50, 2.0 );
		}

		// カテゴリ
		for ( n = 0 ; n < gd[ select_x * 2 + select_y ].cnum ; ++n )
		{
			MikanDraw->DrawBox( img_x + img_w / 2 - (cate_h * 4 + 5) / 2 * gd[ select_x * 2 + select_y ].cnum + (cate_h * 4 + 5) * n - 2, img_y + img_h + 5 - 2, cate_h * 4 + 4, cate_h + 4, 0xffffffff );
			MikanDraw->DrawTextureScaling( 3, img_x + img_w / 2 - (cate_h * 4 + 5) / 2 * gd[ select_x * 2 + select_y ].cnum + (cate_h * 4 + 5) * n, img_y + img_h + 5, 0, gd[ select_x * 2 + select_y ].category[ n ] *  64, 64, 64, cate_h, cate_h );
			MikanDraw->DrawTextureScaling( 3, img_x + img_w / 2 - (cate_h * 4 + 5) / 2 * gd[ select_x * 2 + select_y ].cnum + (cate_h * 4 + 5) * n + cate_h, img_y + img_h + 5, 64 * 3, gd[ select_x * 2 + select_y ].category[ n ] *  64, 64 * 3, 64, cate_h * 3, cate_h );
		}
		//    MikanDraw->DrawTextureScalingC( tnum, info_x + ( img_x - info_x ) / 2, img_y + img_h / 2, 0, 0, 50, 50, info_bt_size, info_bt_size );
		//    MikanDraw->DrawTextureScalingC( tnum, info_x + ( img_x - info_x ) / 2 + img_w, img_y + img_h / 2, 50, 0, 50, 50, info_bt_size, info_bt_size );

		if( sys.tenlan->InputCancel() == 1 )
		{
			//sys.modeflag = MF_SELECT;
			sys.tenlan->SetGameMode( MF_SELECT );
		}

		if( sys.tenlan->InputAct() == 1 )
		{
			if( sys.tenlan->GetTimer() > 60 )
			{
				ExecProgram( select_x * 2 + select_y );
				rtimer = 0;
			}
		}

		if( sys.tenlan->InputLeft() %30 == 1 && info_num > 0)
		{
			--info_num;
		} else if( sys.tenlan->InputRight() % 30 == 1 && info_num + 1 < info_max )
		{
			++info_num;
		}

	} else if( istimer < SSANIME_TIME/2 )
	{
		// アニメーション1
		prog = (double)istimer / (double)(SSANIME_TIME/2);

		DrawMotionBox( info_x, info_y, info_w, info_h, prog );

	} else
	{
		// アニメーション2
		prog = (double)(istimer - SSANIME_TIME/2) / (double)(SSANIME_TIME/2);

		MikanDraw->DrawBox( info_x, info_y, info_w, info_h, INFOBAR_COLOR );

		DrawMotionBox( info_x, info_y, info_w, infobar_height, prog );

		DrawMotionBox( info_x, info_y + info_h - infobar_height, info_w, infobar_height, prog );

		DrawMotionBox( img_x, img_y, img_w, img_h, prog );

		info_num = 0;
	}

	++istimer;

	return 0;
}

int UI_Hiroki_Metro::View( void )
{

	//switch( sys.modeflag )
	switch( sys.tenlan->GetGameMode() )
	{
	case MF_TITLE:
	case MF_KEYCONFIG:
	case MF_SOUND:
		tenlan->OP();

		break;
	case MF_SELECT:
		BackGround();

		SelectScreen( enable_input );

		//デバッグ用
		if(  sys.tenlan->InputCancel() == 1 )
		{
			//sys.modeflag = MF_TITLE;
			sys.tenlan->SetGameMode( MF_TITLE );
			sstimer = 0;
		}
		break;
	case MF_OKAY:
		BackGround();

		SelectScreen();

		InfoScreen( enable_input );

		break;
	case MF_PLAY:
		BackGround();
		//MikanDraw->Printf(0,0,0,"%d %d", MikanInput->GetKeyNum(K_Z),MikanInput->GetPadNum(0,PAD_A));
		break;
	}

	/*if( rtimer == 60 * waitsecond )
	{
	sys.modeflag = MF_TITLE;
	}else if( MF_SELECT <= sys.modeflag && sys.modeflag < MF_PLAY )
	{
	++rtimer;
	}*/

	//++sys.timer;
	sys.tenlan->AddTimer();

	// 隠しコマンド
	tenlan->Command();

	return 0;
}

int UI_Hiroki_Metro::Release( void )
{
	return 0;
}

int UI_Hiroki_Metro::ReleaseTexture( void )
{
	// 3番
	return 0;
}

int UI_Hiroki_Metro::ReloadTexture( void )
{
	return 0;
}


