#include "common.h"
#include "UI\UI.h"
#include <time.h>

// イージングいいっすね～
double easeInQuad(double t, double T) {
	double ratio = t / T;
	return ratio * ratio;
}


double easeOutQuad(double t, double T) {
	return 1.0 - easeInQuad(T - t, T);
}


#define SSANIME_TIME 30
#define INFOBAR_COLOR 0x7fefefef

int UI_2015::numEasing(int *a, int b, int c) {
	*a = (*a * c + b) / (c + 1);
	if (abs(*a - b) < c) {
		*a = b;
	}
	return *a;
}

// コンストラクタ
UI_2015::UI_2015(class TenLANBase *tenlanbase) {
	int height;

	SetTenLAN(tenlanbase);

	rtimer = 0;
	waitsecond = 30;

	height = tenlan->GetHeight();

	infobar_height = height / 10;
	icon_size = height * 4 / 12;

	sparse_icon = height / 30;

	sparse_game = (icon_size - (icon_size * 3 / 4)) / 2;

	w_num = (tenlan->GetWidth() - sparse_icon) / (icon_size + sparse_icon) + 1;

	// ゲームの数 / 2 + 2で割った余り
	column = 4;
	select_w = (sys.tenlan->GetGameMax() + column - 1) / column;
	select_x = 0;
	select_y = 0;
	select_size = sparse_icon / column;
	move_x = 0;
	s_y = -icon_size;

	//ゲームをリストに
	for (int i = 0; i < sys.tenlan->GetGameMax(); ++i) {
		showGameList.push_back(&gd[i]);
		gd[i].absoluteNum = i;
	}

	info_h = height * 9 / 10;
	info_w = info_h * 4 / 3;
	info_x = (tenlan->GetWidth() - info_w) / 2;
	info_y = (height - info_h) / 2;

	cate_h = tenlan->GetWidth() / 20;
	img_h = info_h - infobar_height * 2 - infobar_height / 5 - cate_h;
	img_w = img_h * 4 / 3;
	img_x = info_x + (info_w - img_w) / 2;
	img_y = info_y + infobar_height + infobar_height / 10;

	info_bt_size = (info_x - img_x) * 9 / 10;

	_MikanDraw->CreatePolygon(0, 7, ::D3DPT_TRIANGLEFAN);
	_MikanDraw->SetPolygonVertex(0, 0, 0, INFOBAR_COLOR);
	_MikanDraw->SetPolygonVertex(0, 200, 0, INFOBAR_COLOR);
	_MikanDraw->SetPolygonVertex(0, 200, 50, INFOBAR_COLOR);
	_MikanDraw->SetPolygonVertex(0, 100, 50, INFOBAR_COLOR);
	_MikanDraw->SetPolygonVertex(0, 50, 100, INFOBAR_COLOR);
	_MikanDraw->SetPolygonVertex(0, 50, 200, INFOBAR_COLOR);
	_MikanDraw->SetPolygonVertex(0, 0, 200, INFOBAR_COLOR);

	font_size = infobar_height - 10;
	MikanDraw->CreateFont(3, font_size, 0xff000000);

	info_num = 0;

	enable_input = 0;

	select_category = -1;//ジャンル絞り込み
	genreSelect = 0;
	time = 0;

	titlescr_time = 0;// タイトルスクロール初期化
	nowBackgorund = 6;// 背景番号の初期化
	BackgroundNum = 0;// 背景数の初期化
}

// 押しっぱなしの状態で、キーが入力された瞬間を取る。
int UI_2015::InputContinue(int key) {
	return (key == 1 || (key >= 30 && key % 6 == 0));
}

//左が入力された


//一つのゲームの枠の描画
int UI_2015::DrawGameBox(int x, int y, int width, int height, GAMEDATA * game, int time) {
	int info_num = MikanDraw->GetTextureHeight(game->txnum) / 480;
	if (time >= 0 && time < 10) {
		int r = icon_size * easeOutQuad(time, 10);
		MikanDraw->DrawBox(x + (width - r) / 2, y + (height - r) / 2, width* easeOutQuad(time, 10), height * easeOutQuad(time, 10), 0xff000000 | CATE_COLOR[game->category[0]]);
		MikanDraw->DrawTextureScaling(game->txnum, x + (icon_size - r) / 2, y + (icon_size - r) / 2, 0, 0, 640, 480, r, r * 3 / 4);
	}
	else if (time >= 10) {
		MikanDraw->DrawBox(x, y, width, height, 0xff000000 | CATE_COLOR[game->category[0]]);
		if (info_num > 1) {
			time = (time - 10) % (300 * info_num);
			int t = time % 300;
			int w = time / 300;
			if (t > 240) {
				MikanDraw->SetAlpha(game->txnum, 255);
				MikanDraw->DrawTextureScaling(game->txnum, x, y, 0, 480 * ((w + 1) % info_num), 640, 480, icon_size, icon_size * 3 / 4);
				MikanDraw->SetAlpha(game->txnum, 255 * ((300 - t) / 60.0));
				MikanDraw->DrawTextureScaling(game->txnum, x, y, 0, 480 * ((w) % info_num), 640, 480, icon_size, icon_size * 3 / 4);
				MikanDraw->SetAlpha(game->txnum, 255);
			}
			else {
				MikanDraw->DrawTextureScaling(game->txnum, x, y, 0, w * 480, 640, 480, icon_size, icon_size * 3 / 4);
			}
		}
		else if (info_num == 1) {
			MikanDraw->DrawTextureScaling(game->txnum, x, y, 0, 0, 640, 480, icon_size, icon_size * 3 / 4);

		}
	}

	return 0;
}

//一つのゲームの枠の描画
int UI_2015::DrawSpecialBox(int x, int y, int width, int height, int which, int time) {
	switch (which) {
	case 0:
		MikanDraw->DrawBox(x, y, width, height, 0xff002D28);
		MikanDraw->DrawTextureScaling(3, x, y, 0, 11 * 64, 64, 64, cate_h, cate_h);
		MikanDraw->DrawTextureScaling(3, x, y + cate_h, 64 * 3, 11 * 64, 64 * 3, 64, cate_h * 3, cate_h);
		break;

	case 1:
		MikanDraw->DrawBox(x, y, width, height, 0xff000000 | CATE_COLOR[time / 10 % CATEGORY_MAX]);
		MikanDraw->DrawTextureScaling(3, x, y, 0, 12 * 64, 64, 64, cate_h, cate_h);
		MikanDraw->DrawTextureScaling(3, x, y + cate_h, 64 * 3, 12 * 64, 64 * 3, 64, cate_h * 3, cate_h);
		break;
	}
	return 0;
}


//ジャンル枠描画
int UI_2015::DrawGenreBox(int x, int y, int width, int height, int which, int time) {
	if (time >= 0 && time < 10) {
		int r = icon_size * easeOutQuad(time, 10);
		MikanDraw->DrawBox(x + (width - r) / 2, y + (height - r) / 2, width* easeOutQuad(time, 10), height * easeOutQuad(time, 10), 0xff000000 | CATE_COLOR[which]);
		double rate = easeOutQuad(time, 10);
		MikanDraw->DrawTextureScaling(3, x + (64 - 64 * rate) / 2, y + (cate_h - cate_h * rate) / 2, 0, which * 64, 64 * rate, 64 * rate, cate_h, cate_h);
		MikanDraw->DrawTextureScaling(3, x + 3 * (64 - 64 * rate) / 2, y + cate_h + (cate_h - cate_h * rate) / 2, 64 * 3, which * 64, 64 * 3 * rate, 64 * rate, cate_h * 3, cate_h);
	}
	else if (time >= 10) {
		MikanDraw->DrawBox(x, y, width, height, 0xff000000 | CATE_COLOR[which]);
		MikanDraw->DrawTextureScaling(3, x, y, 0, which * 64, 64, 64, cate_h, cate_h);
		MikanDraw->DrawTextureScaling(3, x, y + cate_h, 64 * 3, which * 64, 64 * 3, 64, cate_h * 3, cate_h);
	}
	return 0;
}

// ゲームを選ぶ画面
int UI_2015::SelectScreen(int enable_input) {

	double prog;
	int i;
	int bx, by, base_x;
	int tx;
	int select_size;

	if (sstimer > SSANIME_TIME) {
		//SkyDraw
		++time;

		MikanDraw->DrawTextureScaling(nowBackgorund, 0, 0, 0, 0, 640, 480, tenlan->GetWidth(), tenlan->GetHeight());
		
		base_x = move_x;
		select_size = (int)((double)this->select_size * ((double)((double)(sstimer % 30) / 60.0 + 0.5)));

		if (genreSelect) {
			select_w = (CATEGORY_MAX + column - 1) / column;
		}
		else {
			select_w = (showGameList.size() + column - 1) / column;
		}

		select_size = sparse_icon / column;
		move_x = 0;

		numEasing(&s_y, select_y * (icon_size + sparse_icon) - icon_size, 3);


		bx = base_x + (select_x)* (sparse_icon + icon_size) + sparse_icon;
		by = select_y * (sparse_icon + icon_size) - s_y;

		// 選択
		//カーソル
		for (int j = 1; j <= 4; ++j) {
			if (time / 2 % 20 >= 4 * j) {
				MikanDraw->DrawBox(bx - j * 4, by - j * 4, icon_size + j * 2 * 4, icon_size + j * 2 * 4, 0x60ffffff);
			}
		}

		// special box
		for (int i = 0; i < 2; ++i) {
			if (i == 1 && (genreSelect || showGameList.size() == 0)) {
				continue;
			}
			DrawSpecialBox(base_x + sparse_icon  * (i + 1) + icon_size * i, -s_y + -1 * (sparse_icon + icon_size), icon_size, icon_size, i, time);

		}
		if (genreSelect) {
			for (i = 0; i < CATEGORY_MAX; ++i) {
				tx = base_x + (i / column) * (sparse_icon + icon_size) + sparse_icon;
				int j = i % column;
				DrawGenreBox(base_x + sparse_icon  * (j + 1) + icon_size * j, -s_y + (i / column) * (sparse_icon + icon_size), icon_size, icon_size, i, time - 4 * i);
			}
		}
		else {
			for (i = 0; i < showGameList.size(); ++i) {
				tx = base_x + (i / column) * (sparse_icon + icon_size) + sparse_icon;
				int j = i % column;
				DrawGameBox(base_x + sparse_icon  * (j + 1) + icon_size * j, -s_y + (i / column) * (sparse_icon + icon_size), icon_size, icon_size, showGameList[i], time - 4 * i);
			}
		}




		//ぐるぐるカーソル
		for (int i = 0; i < 12; ++i) {
			int r = (time * 7 - i * 12) % icon_size;
			MikanDraw->DrawCircleC(bx + r, by, 20, 0x01000000 * (150 - 7 * i) + 0xFFFF88);
			MikanDraw->DrawCircleC(bx + icon_size, by + r, 20, 0x01000000 * (150 - 7 * i) + 0xFFFF88);
			MikanDraw->DrawCircleC(bx + icon_size - r, by + icon_size, 20, 0x01000000 * (150 - 7 * i) + 0xFFFF88);
			MikanDraw->DrawCircleC(bx, by + icon_size - r, 20, 0x01000000 * (150 - 7 * i) + 0xFFFF88);
		}

		// 情報バー。
		MikanDraw->DrawBox(0, tenlan->GetHeight() - infobar_height, tenlan->GetWidth(), infobar_height, 0x7f000000);

		if (showGameList.size() == 0 && genreSelect == 0) {
			MikanDraw->Printf(46, 0, tenlan->GetHeight() - infobar_height, "該当する条件のゲームはありませんでした");
		}
		else if (genreSelect && select_y >= 0) {
			std::string s = "";
			switch (select_y * column + select_x) {
			case 0: s = "アクション"; break;
			case 1: s = "シューティング"; break;
			case 2: s = "スポーツ"; break;
			case 3: s = "レース"; break;
			case 4: s = "パズル"; break;
			case 5: s = "テーブル"; break;
			case 6: s = "シミュレーション"; break;
			case 7: s = "ロールプレイング"; break;
			case 8: s = "多人数"; break;
			case 9: s = "ミニゲーム"; break;
			case 10: s = "その他"; break;
			}
			MikanDraw->Printf(46, 0, tenlan->GetHeight() - infobar_height, s.c_str());
		}
		else if (WhichGame(select_x, select_y) >= 0 && WhichGame(select_x, select_y) < showGameList.size()) {
			// ゲームにカーソルがあっている時画面下にタイトル表示

			// タイトルが短ければそのまま
			if (strlen(showGameList[WhichGame(select_x, select_y)]->title) <= 35)
			{
				MikanDraw->Printf(46, 0, tenlan->GetHeight() - infobar_height, "%s", showGameList[WhichGame(select_x, select_y)]->title);
			}
			// 長ければスクロールする
			else
			{
				int scr_time = strlen(showGameList[WhichGame(select_x, select_y)]->title) * 32;

				MikanDraw->Printf(46, -titlescr_time * 2, tenlan->GetHeight() - infobar_height, "%s", showGameList[WhichGame(select_x, select_y)]->title);
				MikanDraw->Printf(46, -titlescr_time * 2 + scr_time * 2, tenlan->GetHeight() - infobar_height, "%s", showGameList[WhichGame(select_x, select_y)]->title);

				titlescr_time++;
				if (titlescr_time >= scr_time)
				{
					titlescr_time = 0;
				}
			}
		}
		else if (select_y == -1) {
			switch (select_x) {
			case 0:
				switch (genreSelect) {
				case 0:
					MikanDraw->Printf(46, 0, tenlan->GetHeight() - infobar_height, "ゲームジャンルで絞り込む");
					break;
				case 1:
					MikanDraw->Printf(46, 0, tenlan->GetHeight() - infobar_height, "全てのゲームに戻る");
					break;
				}
				break;
			case 1:
				switch (genreSelect) {
				case 0:
					if (showGameList.size() > 0) {
						MikanDraw->Printf(46, 0, tenlan->GetHeight() - infobar_height, "ランダムセレクト！");
					}
					break;
				}
				break;
			}
		}

		++sstimer;

		if (enable_input) {
			//if ( InputAct() == 1 && WhichGame(select_x, select_y) < sys.gamemax )
			if (sys.tenlan->InputAct() == 1) {
				MikanSound->Play(11, 0);
				if (select_x == 0 && select_y == -1) {
					genreSelect = !genreSelect;
					if (!genreSelect) {
						showGameList.clear();
						genreSelect = 0;
						select_category = -1;
						for (int i = 0; i < sys.tenlan->GetGameMax(); ++i) {
							showGameList.push_back(&gd[i]);
						}
					}
					time = 0;
					select_y = 0;
				}
				else {
					if (genreSelect) {
						if (WhichGame(select_x, select_y) >= 0 && WhichGame(select_x, select_y) < CATEGORY_MAX) {
							select_category = WhichGame(select_x, select_y);
							showGameList.clear();
							for (int i = 0; i < sys.tenlan->GetGameMax(); ++i) {
								//該当するカテゴリのゲームだけをリストに。
								for (int n = 0; n < gd[i].cnum; ++n) {
									if (gd[i].category[n] == WhichGame(select_x, select_y)) {
										showGameList.push_back(&gd[i]);
									}
								}
							}
							time = 0;
							select_x = 0;
							if (showGameList.size() > 0) {
								select_y = 0;
							}
							else {
								select_y = -1;
							}
							genreSelect = 0;
						}


					}
					else {
						if (select_x == 1 && select_y == -1) {
							//randomselect
							if (showGameList.size() > 0) {
								int a = rand() % showGameList.size();
								select_x = a % column;
								select_y = a / column;
								istimer = 0;
								info_num = 0;
								sys.tenlan->SetGameMode(MF_OKAY);
							}
						}
						else if (WhichGame(select_x, select_y) >= 0 && WhichGame(select_x, select_y) < showGameList.size()) {
							istimer = 0;
							info_num = 0;
							sys.tenlan->SetGameMode(MF_OKAY);
						}
					}
				}

			}
			else if (InputContinue(sys.tenlan->InputLeft())) {
				select_x = (select_x - 1 + column) % column;
				MikanSound->Play(10, 0);
				titlescr_time = 0;
			}
			else if (InputContinue(sys.tenlan->InputRight())) {
				select_x = (select_x + 1 + column) % column;
				MikanSound->Play(10, 0);
				titlescr_time = 0;
			}
			else if (InputContinue(sys.tenlan->InputUp())) {
				--select_y;
				MikanSound->Play(10, 0);
				if (select_y < -1) {
					select_y = select_w - 1;
				}
				titlescr_time = 0;
			}
			else if (InputContinue(sys.tenlan->InputDown())) {
				++select_y;
				MikanSound->Play(10, 0);
				if (select_y >= select_w) {
					select_y = -1;
				}
				titlescr_time = 0;
			}

		}

	}
	else {
		//ゲームセレクト画面への移行時のフェードイン演出
		MikanDraw->SetAlpha(nowBackgorund, 254 * (sstimer / (double)SSANIME_TIME));
		MikanDraw->DrawTextureScaling(nowBackgorund, 0, 0, 0, 0, 640, 480, tenlan->GetWidth(), tenlan->GetHeight());
		MikanDraw->SetAlpha(nowBackgorund, 255);
		++sstimer;
	}

	if (sstimer == SSANIME_TIME) {
		time = 0;
		select_x = select_y = 0;
		s_y = 0;
		MikanSound->Play(nowPlaying, true);
		showGameList.clear();
		genreSelect = 0;
		select_category = -1;
		for (int i = 0; i < sys.tenlan->GetGameMax(); ++i) {
			showGameList.push_back(&gd[i]);
		}
	}

	return 0;
}

int UI_2015::WhichGame(int x, int y) {
	return y * column + x;
}

// ゲーム選択時のタイトル、スクショ、紹介文の表示
int UI_2015::InfoScreen(int enable_input) {
	double prog;
	int info_max;
	int n;

	MikanDraw->DrawTextureScaling(tnum, 0, 0, 0, 50, 50, 50, tenlan->GetWidth(), tenlan->GetHeight());

	// 描画領域の取得と設定
	D3DVIEWPORT9 vp, vp_info;
	_MikanDraw->D3DDev->GetViewport(&vp);
	vp_info = vp;
	vp_info.X = info_x; vp_info.Y = info_y;
	vp_info.Width = info_w; vp_info.Height = info_h;
	_MikanDraw->D3DDev->SetViewport(&vp_info);

	int sst = 10;
	if (istimer > sst) {
		int which = showGameList[WhichGame(select_x, select_y)]->absoluteNum;
		auto pt = showGameList[WhichGame(select_x, select_y)];
		info_max = MikanDraw->GetTextureHeight(pt->txnum) / 480;

		MikanDraw->DrawBox(info_x, info_y, info_w, info_h, 0x7f000000 | CATE_COLOR[pt->category[0]]);

		int textsize = 30;

		// タイトル
		MikanDraw->DrawBox(info_x, info_y, info_w, infobar_height, INFOBAR_COLOR);
		// タイトルが短ければそのまま
		if (strlen(pt->title) <= 30)
		{
			MikanDraw->Printf(3, info_x, info_y, "%s", pt->title);
		}
		// 長ければスクロールする
		else
		{
			int scr_time = strlen(pt->title) * textsize;

			MikanDraw->Printf(3, info_x - ((istimer - 10) % scr_time) * 2, info_y, "%s", pt->title);
			MikanDraw->Printf(3, info_x - ((istimer - 10) % scr_time) * 2 + scr_time * 2, info_y, "%s", pt->title);
		}

		// 説明文
		MikanDraw->DrawBox(info_x, info_y + info_h - infobar_height, info_w, infobar_height, INFOBAR_COLOR);
		// 説明が短ければそのまま
		if (strlen(pt->text) <= 30)
		{
			MikanDraw->Printf(3, info_x, info_y + info_h - infobar_height, "%s", pt->text);
		}
		// 長ければスクロールする
		else
		{
			int scr_time = strlen(pt->text) * textsize;

			MikanDraw->Printf(3, info_x - ((istimer - 10) % scr_time) * 2, info_y + info_h - infobar_height, "%s", pt->text);
			MikanDraw->Printf(3, info_x - ((istimer - 10) % scr_time) * 2 + scr_time * 2, info_y + info_h - infobar_height, "%s", pt->text);
		}

		// インスト画像
		MikanDraw->DrawBox(img_x, img_y, img_w, img_h, INFOBAR_COLOR);
		MikanDraw->DrawTextureScaling(pt->txnum, img_x, img_y, 0, info_num * 480, 640, 480, img_w, img_h);
		if (info_num > 0) {
			MikanDraw->DrawTextureScalingC(4, img_x - 60, img_y + img_h / 2, 0, 0, 50, 50, 2.0);
		}
		if (info_num + 1 < info_max) {
			MikanDraw->DrawTextureScalingC(4, img_x + img_w + 60, img_y + img_h / 2, 50, 0, 50, 50, 2.0);
		}

		// カテゴリ
		for (n = 0; n < showGameList[WhichGame(select_x, select_y)]->cnum; ++n) {
			MikanDraw->DrawBox(img_x + img_w / 2 - (cate_h * 4 + 5) / 2 * pt->cnum + (cate_h * 4 + 5) * n - 2, img_y + img_h + 5 - 2, cate_h * 4 + 4, cate_h + 4, 0xffffffff);
			MikanDraw->DrawTextureScaling(3, img_x + img_w / 2 - (cate_h * 4 + 5) / 2 * pt->cnum + (cate_h * 4 + 5) * n, img_y + img_h + 5, 0, pt->category[n] * 64, 64, 64, cate_h, cate_h);
			MikanDraw->DrawTextureScaling(3, img_x + img_w / 2 - (cate_h * 4 + 5) / 2 * pt->cnum + (cate_h * 4 + 5) * n + cate_h, img_y + img_h + 5, 64 * 3, pt->category[n] * 64, 64 * 3, 64, cate_h * 3, cate_h);
		}

		if (sys.tenlan->InputCancel() == 1) {
			//sys.modeflag = MF_SELECT;
			MikanSound->Play(10, 0);
			sys.tenlan->SetGameMode(MF_SELECT);
		}

		if (sys.tenlan->InputAct() == 1) {
			if (sys.tenlan->GetTimer() > 60) {
				MikanSound->Stop(nowPlaying);
				ExecProgram(which);
				MikanSound->Play(11, 0);
				rtimer = 0;
			}
		}

		if (InputContinue(sys.tenlan->InputLeft()) && info_num > 0) {
			--info_num;
			MikanSound->Play(10, 0);
		}
		else if (InputContinue(sys.tenlan->InputRight() % 30) && info_num + 1 < info_max) {
			++info_num;
			MikanSound->Play(10, 0);
		}

	}
	else {
		double rate = easeOutQuad(istimer, sst);
		double uRate = 1 - rate;

		auto pt = showGameList[WhichGame(select_x, select_y)];
		info_max = MikanDraw->GetTextureHeight(pt->txnum) / 480;

		MikanDraw->DrawBox(info_x, info_y + uRate * info_h / 2, info_w, info_h * rate, 0x7f000000 | CATE_COLOR[pt->category[0]]);

		MikanDraw->DrawBox(info_x, info_y + uRate * info_h / 2, info_w, infobar_height, INFOBAR_COLOR);
		MikanDraw->Printf(3, info_x, info_y + uRate * info_h / 2, "%s", pt->title);

		MikanDraw->DrawBox(info_x, info_y + info_h * (rate / 2 + 0.5) - infobar_height, info_w, infobar_height, INFOBAR_COLOR);
		MikanDraw->Printf(3, info_x, info_y + info_h * (rate / 2 + 0.5) - infobar_height, "%s", pt->text);
	}

	// 描画領域を元に戻す
	_MikanDraw->D3DDev->SetViewport(&vp);

	++istimer;

	return 0;
}

/*UI_Hirokiからだいたい持ってきた人たち*/

char * UI_2015::UIName(void) {
	if (sys.tenlan->GetGameMode() < MF_SELECT) {
		// タイトル画面
		return "Metro BGM:BigPresent Composed by Grow";
	}
	else {
		// その他プレイ中
		return "Metro";
	}
}

void SetRand() {
	srand((unsigned)time(0));
}

// 初期化  ゲームの実行が終わるたびに呼び出される
int UI_2015::Init(void) {
	tnum = 4;// テクスチャの番号0,1,2,3は既に使われている
	//テクスチャ番号は9までしか確保されてない(未確認)ため、背景のテクスチャ番号は6,7,8,9を使用する
	MikanDraw->CreateTexture(tnum, "material/metro.png");
	MikanDraw->CreateTexture(tnum + 1, tenlan->GetWidth(), infobar_height * 2);
	MikanDraw->CreateTexture(tnum + 2, "material/umi.png");
	MikanDraw->CreateTexture(tnum + 3, "material/sora.png");
	MikanDraw->CreateTexture(tnum + 4, "material/space.png");

	BackgroundNum = 3;// 背景数 追加する際に1増やす
	nowBackgorund = 6 + rand() % BackgroundNum;// ゲーム終了時に背景を変更する


	sstimer = 0;
	istimer = 0;

	MikanDraw->CreateFont(46, infobar_height);

	enable_input = 1;
	genreSelect = 0;

	sys.tenlan->SetGameMode(MF_SELECT);
	time = 0;

	//srand
	SetRand();//timeとかいうメンバを作ったばかりに

	//bgm
	/*MikanSound->Load(2, NULL, "SELECT_BGM0");
	MikanSound->SetVolume(2, sys.tenlan->GetMaxVolume() * 3 / 4);
	MikanSound->Load(3, NULL, "SELECT_BGM1");
	MikanSound->SetVolume(3, sys.tenlan->GetMaxVolume() * 3 / 4);
	MikanSound->Load(4, NULL, "SELECT_BGM2");
	MikanSound->SetVolume(4, sys.tenlan->GetMaxVolume());
	MikanSound->Load(5, NULL, "SELECT_BGM3");
	MikanSound->SetVolume(5, sys.tenlan->GetMaxVolume() * 3 / 4);
	MikanSound->Load(6, NULL, "SELECT_BGM4");
	MikanSound->SetVolume(6, sys.tenlan->GetMaxVolume() * 3 / 4);*/
	MikanSound->Load(2, NULL, "SELECT_BGM5");
	MikanSound->SetVolume(2, sys.tenlan->GetMaxVolume() / 2);// * 3 / 4);
	MikanSound->Load(3, NULL, "SELECT_BGM6");
	MikanSound->SetVolume(3, sys.tenlan->GetMaxVolume() / 2);// * 3 / 4);
	MikanSound->Load(4, NULL, "SELECT_BGM7");
	MikanSound->SetVolume(4, sys.tenlan->GetMaxVolume() / 2);// * 3 / 4);

	musicNum = 3;// BGMの数 追加の際に1増やす
	nowPlaying = 2 + rand() % musicNum;// ゲーム終了時にBGMを変更する

	//se
	MikanSound->Load(10, NULL, "SELECT_SE");
	MikanSound->SetVolume(10, sys.tenlan->GetMaxSEVolume());
	MikanSound->Load(11, NULL, "OK_SE");
	MikanSound->SetVolume(11, sys.tenlan->GetMaxSEVolume());

	return 0;
}

// 背景真っ黒
int UI_2015::BackGround(void) {
	MikanDraw->DrawBox(0, 0, tenlan->GetWidth(), tenlan->GetHeight(), 0xff000000);
	return 0;
}

int UI_2015::DrawMotionBox(int x, int y, int width, int height, double prog) {
	if (prog < 0.5) {
		_MikanDraw->EditPolygonVertex(0, 1, width * prog * 2.0, 0.0, 0, 0, 0, PVEDIT_XY);
		_MikanDraw->EditPolygonVertex(0, 2, width * prog * 2.0, 1.0, 0, 0, 0, PVEDIT_XY);
		_MikanDraw->EditPolygonVertex(0, 3, 1.0, 1.0, 0, 0, 0, PVEDIT_XY);
		_MikanDraw->EditPolygonVertex(0, 4, 1.0, 1.0, 0, 0, 0, PVEDIT_XY);
		_MikanDraw->EditPolygonVertex(0, 5, 1.0, height * prog * 2.0, 0, 0, 0, PVEDIT_XY);
		_MikanDraw->EditPolygonVertex(0, 6, 0.0, height * prog * 2.0, 0, 0, 0, PVEDIT_XY);
	}
	else if (prog < 0.75) {
		_MikanDraw->EditPolygonVertex(0, 1, width, 0.0, 0, 0, 0, PVEDIT_XY);
		_MikanDraw->EditPolygonVertex(0, 2, width, 1.0, 0, 0, 0, PVEDIT_XY);
		_MikanDraw->EditPolygonVertex(0, 3, width * (prog - 0.5)*4.0, 1.0, 0, 0, 0, PVEDIT_XY);
		_MikanDraw->EditPolygonVertex(0, 4, 1.0, height * (prog - 0.5)*4.0, 0, 0, 0, PVEDIT_XY);
		_MikanDraw->EditPolygonVertex(0, 5, 1.0, height, 0, 0, 0, PVEDIT_XY);
		_MikanDraw->EditPolygonVertex(0, 6, 0.0, height, 0, 0, 0, PVEDIT_XY);
	}
	else {
		_MikanDraw->EditPolygonVertex(0, 1, width, 0.0, 0, 0, 0, PVEDIT_XY);
		_MikanDraw->EditPolygonVertex(0, 2, width, 1.0, 0, 0, 0, PVEDIT_XY);
		_MikanDraw->EditPolygonVertex(0, 3, width, height * (prog - 0.75)*4.0, 0, 0, 0, PVEDIT_XY);
		_MikanDraw->EditPolygonVertex(0, 4, width * (prog - 0.75)*4.0, height, 0, 0, 0, PVEDIT_XY);
		_MikanDraw->EditPolygonVertex(0, 5, 1.0, height, 0, 0, 0, PVEDIT_XY);
		_MikanDraw->EditPolygonVertex(0, 6, 0.0, height, 0, 0, 0, PVEDIT_XY);
	}
	_MikanDraw->DrawPolygon(0, x, y);
	return 0;
}

// 描画処理とか
int UI_2015::View(void) {

	//switch( sys.modeflag )
	switch (sys.tenlan->GetGameMode()) {
	// タイトル画面その他
	case MF_TITLE:
	case MF_KEYCONFIG:
	case MF_SOUND:
	case MF_SE:
		tenlan->OP();

		break;
	// ゲームを選ぶ画面
	case MF_SELECT:
		BackGround();

		SelectScreen(enable_input);

		//デバッグ用
		if (sys.tenlan->InputCancel() == 1) {
			MikanSound->Play(10, 0);
			if (genreSelect) {
				time = 0;
				genreSelect = 0;
				select_category = -1;
				select_x = 0;
				select_y = 0;
				showGameList.clear();
				for (int i = 0; i < sys.tenlan->GetGameMax(); ++i) {
					showGameList.push_back(&gd[i]);
				}
			}
			else {
				//カテゴリされた結果の時はキャンセルでタイトルに戻らない
				if (select_category == -1) {
					MikanSound->Stop(nowPlaying);
					sys.tenlan->SetGameMode(MF_TITLE);
					sstimer = 0;
					MikanSound->Play(0, 1);
				}
				else {
					genreSelect = 1;
					select_x = 0;
					select_y = 0;
					time = 0;
					showGameList.clear();
					MikanSound->Play(0, 1);
				}
			}
		}
		break;
		// ゲーム選んで紹介分とか表示されてるとき
	case MF_OKAY:
		BackGround();

		SelectScreen();

		InfoScreen(enable_input);

		break;
	case MF_PLAY:
		BackGround();
		//MikanDraw->Printf(0,0,0,"%d %d", MikanInput->GetKeyNum(K_Z),MikanInput->GetPadNum(0,PAD_A));
		break;
	}

	// デバッグモードの表示(OP)
	if (sys.tenlan->DebugMode() != 0 && sys.tenlan->GetGameMode() != MF_PLAY)
	{
		MikanDraw->Printf(46, 0, 0, "デバッグモード");
	}

	//++sys.timer;
	sys.tenlan->AddTimer();

	// 隠しコマンド
	tenlan->Command();

	return 0;
}

int UI_2015::Release(void) {
	return 0;
}

int UI_2015::ReleaseTexture(void) {
	// 3番
	return 0;
}

int UI_2015::ReloadTexture(void) {
	return 0;
}