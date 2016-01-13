#ifndef CLASS_UIBASE
#define CLASS_UIBASE class UIBase

//ゲームのフラグ
enum
{
	MF_TITLE,     // タイトル
	MF_KEYCONFIG, // キーコンフィグ
	MF_SOUND,     // BGM
	MF_SE,		  // 効果音
	MF_SELECT,    // ゲーム選択画面
	MF_OKAY,      // 確認画面及び決定
	MF_PLAY,      // ゲームが動作中(Ten-LANは背景描画or真っ黒)
};

// カテゴリ数。
#define CATEGORY_MAX 11

// カテゴリの色。
extern unsigned long CATE_COLOR[ CATEGORY_MAX ];

// ゲームデータ。
struct GAMEDATA
{
	int num;                       // ゲーム番号。
	char exe[1024];                // 起動パス。
	char title[2048];              // ゲーム名。
	char text[2048];               // 紹介文。
	unsigned int date;             // 日付。
	int pad2key;                   // Pad2Keyが有効かどうか。
	int txnum;                     // 説明画像のテクスチャ番号。
	char txfile[ 512 ];            // テクスチャのファイルパス。
	int cnum;                      //
	char category[ CATEGORY_MAX ]; // どのカテゴリに所属しているか。
	int absoluteNum;

	int playnum;// その日遊ばれた回数
	long playtime;// その日遊ばれた合計時間
	int playnum_total;// 遊ばれた回数の合計
	long playtime_total;// 遊ばれた合計時間の合計
};

// リソース絡みのお知らせ
// フォント番号0-9はシステムTen-LAN本体で確保します。
// テクスチャは以下のものを標準で使えます。
//  * 0番 - 基本背景
//  * 1番 - ロゴ
//  * 2番 - システムで使う標準アイテム
// 3番以降9番までのテクスチャを使ってください。

// UIを作るときに継承するクラス。
class UIBase
{
protected:
	// Ten-LANのデータ管理などを行うクラスのインスタンス。
	class TenLANBase *tenlan;
public:

	// システム側が使う処理なので、絶対に変更しないこと。
	virtual int SetTenLAN( class TenLANBase *tenlanobj ){ tenlan = tenlanobj; return 0; }

	// UIの名前を返す。
	// ただ一部で描画するときに使うだけ。
	virtual char *UIName( void ){ return ""; }
	// 初期化処理。
	virtual int Init( void ){ return 0; };
	// 共通の背景描画。
	virtual int BackGround( void ) = 0;
	// 選ぶ画面。
	virtual int View( void ) = 0;
	// 開放処理。
	virtual int Release( void ){ return 0; };
	// テクスチャの開放処理。
	virtual int ReleaseTexture( void ){ return 0; };
	// テクスチャの再読み込み処理。
	virtual int ReloadTexture( void ){ return 0; };
};

// Ten-LANの制御用クラス。
// 上で受け取って使える。
class TenLANBase
{
public:

	// 使用可能メソッド。

	// オープニング。
	virtual int OP( void ) = 0;
	// 音量調整。
	virtual int SetVolume( void ) = 0;
	// キーコンフィグ画面
	virtual int KeyConfig( unsigned int padnum ) = 0;
	// コマンド。
	virtual int Command( void ) = 0;

	// システム情報の取得。

	virtual struct GAMEDATA * GetGameData( int gnumber ) = 0;
	virtual int GetWidth( void ) = 0;
	virtual int GetHeight( void ) = 0;
	virtual int GetGameMax( void ) = 0;
	virtual int GetGameMode( void ) = 0;
	virtual int GetTimer( void ) = 0;
	virtual int GetMaxVolume( void ) = 0;

	// システム情報の設定。
	virtual int SetGameMode( int mode ) = 0;

	// 入力メソッド。
	virtual int InputUp( void ) = 0;
	virtual int InputDown( void ) = 0;
	virtual int InputRight( void ) = 0;
	virtual int InputLeft( void ) = 0;
	virtual int InputAct( void ) = 0;
	virtual int InputCancel( void ) = 0;
	virtual int InputESC( void ) = 0;
};

#endif
