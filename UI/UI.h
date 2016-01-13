#ifndef UI_CLASS_BASE
#define UI_CLASS_BASE

#include "../common.h"

#include "../UIBase.h"
#include <vector>
#include <string.h>

extern struct MYSYSTEM sys;
extern struct GAMEDATA *gd;
extern struct VISUAL vis;

class UI_Hiroki_Metro : public UIBase
{
protected:
	int rtimer;
	int waitsecond;

	int enable_input;

	int infobar_height;
	int icon_size;
	int sparse_icon;
	int sparse_game;
	int w_num;

	int select_x, select_y, select_w, select_size;
	int move_x, s_x;

	int tnum;

	int sstimer;
	int istimer;

	int info_w, info_h, info_x, info_y;
	int img_x, img_y, img_w, img_h, cate_h;
	int info_num;
	int info_bt_size;

	int font_size;


public:
	UI_Hiroki_Metro(class TenLANBase *tenlanbase);
	virtual char *UIName(void);
	virtual int Init(void);
	virtual int BackGround(void);
	virtual int SelectScreen(int enable_input = 0);
	virtual int InfoScreen(int enable_input = 0);
	virtual int DrawMotionBox(int x, int y, int width, int height, double prog);
	virtual int View(void);
	virtual int Release(void);
	virtual int ReleaseTexture(void);
	virtual int ReloadTexture(void);

};

class UI_2015 :public UIBase{
	int rtimer;
	int waitsecond;

	int enable_input;

	int infobar_height;
	int icon_size;
	int sparse_icon;
	int sparse_game;
	int w_num;

	int select_x, select_y, select_w, select_size;
	int move_x, s_y;

	int tnum;

	int sstimer;
	int istimer;
	int titlescr_time;// タイトルのスクロール用

	int info_w, info_h, info_x, info_y;
	int img_x, img_y, img_w, img_h, cate_h;
	int info_num;
	int info_bt_size;
	int font_size;
public:
	int time;
	int select_category;
	int column;
	int genreSelect;
	int nowPlaying;
	int musicNum;
	int nowBackgorund;
	int BackgroundNum;
	std::vector <GAMEDATA *> showGameList;
	UI_2015(class TenLANBase *tenlanbase);

	int DrawGameBox(int x, int y, int width, int height, GAMEDATA * game, int time);
	int DrawGenreBox(int x, int y, int width, int height, int which, int time);
	int DrawSpecialBox(int x, int y, int width, int height, int which, int time);
	int InputContinue(int key);
	int WhichGame(int x, int y);
	int numEasing(int *a, int b, int c);

	virtual char *UIName(void);
	virtual int Init(void);
	virtual int BackGround(void);
	virtual int SelectScreen(int enable_input = 0);
	virtual int InfoScreen(int enable_input = 0);
	virtual int DrawMotionBox(int x, int y, int width, int height, double prog);
	virtual int View(void);
	virtual int Release(void);
	virtual int ReleaseTexture(void);
	virtual int ReloadTexture(void);

};

#endif