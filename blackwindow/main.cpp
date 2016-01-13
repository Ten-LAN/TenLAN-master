#include <Window.h>

// 真っ黒で無害な隠す用ウィンドウを生成する。

class Window *win = NULL;

int loop = 1;
int width = 10, height = 10;

int FullScreen( void )
{
  //RECT構造体
  RECT myrect;
  
  HWND myhwnd = NULL;

  //デスクトップのウィンドウハンドル取得
  myhwnd = GetDesktopWindow();
  //デスクトップのサイズなどを取得
  if( myhwnd && GetClientRect( myhwnd, &myrect ) )
  {
    //位置は原点に設定
    win->SetPositionXY( 0, 0 );
    //サイズはデスクトップと同じサイズに設定
    win->SetWindowSize( myrect.right, myrect.bottom );
  }
  width = myrect.right;
  height = myrect.bottom;

  //ウィンドウスタイルの変更
  SetWindowLong( win->GetWindowHandle(), GWL_STYLE, win->GetWindowStyle() );

  SetWindowPos( win->GetWindowHandle(), HWND_TOP, 0, 0, myrect.right, myrect.bottom, SWP_SHOWWINDOW );
  return 0;
}

LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
  HDC hDC;
  PAINTSTRUCT ps;

  switch( uMsg )
  {
  case WM_KEYDOWN:
  case WM_KEYUP:
    if ( wParam == VK_SHIFT )
    {
      // ループを終了する。
      loop = 0;
      if( win )
      {
        delete( win );
        win = NULL;
      }
    }
    return 0;
  case WM_ACTIVATE:
    break;
  case WM_CLOSE://ウィンドウが閉じた時
  case WM_DESTROY://ウィンドウが破棄された時
    if( win )
    {
      delete( win );
      win = NULL;
    }
    loop = 0;
    return 0;
  case WM_GETMINMAXINFO:
    // リサイズなどあればフルスクリーンにするようにする。
    // スクリーンを全て埋め尽くすという意味であって、ディスプレイの解像度は変えない。
    FullScreen();
    break;
  case WM_PAINT:
    // ウィンドウが再描画される時真っ黒にする。
    hDC = BeginPaint( hWnd, &ps );
    SelectObject(hDC , GetStockObject(BLACK_BRUSH));
		Rectangle( hDC, 0, 0, width, height );
		EndPaint( hWnd, &ps );
    return 0;
  case WM_CREATE://ウィンドウが作られた時
    //ここら辺でスクリーン選択画面を出すのもあり。
    //デバイスコンテキストの取得
    win->wd.Screen = CreateCompatibleDC( GetDC( hWnd ) );
    win->wd.WindowHandle = hWnd;
    break;
  case WM_SYSCOMMAND:
    if ( wParam == SC_SCREENSAVE )
    {
      // スクリーンセーバーの起動を阻止。
      return 1;
    }
  }
  //これがないとウィンドウ生成ができない。
  return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
  tagMSG msg;

  //クラスを宣言
  win = new Window();

  if( win )
  {
    win->InitWindowClassEx( hInstance );
    win->SetWindowProc( ::WindowProc );

    if( win->RegistClassEx() )
    {
      //登録失敗
      return 1;
    }

    if( win->MakeWindow( nCmdShow ) || win->wd.Screen == NULL )
    {
      //ウィンドウが作れなかったので終了
      return 2;
    }

    FullScreen();

    while( loop )
    {
      if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
      {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
      }
      Sleep( 200 );
    }

  }

  return 1;
}