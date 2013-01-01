
// SampleGUITest.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "SampleGUITest.h"
#include "MainFrm.h"

#include "SampleGUITestDoc.h"
#include "SampleGUITestView.h"

//[O]
#include <fcntl.h>
#include <io.h>
#include "lua.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//[O]
// ダイアログを出す
static int LUA_Test( lua_State *L ) {
	AfxMessageBox(_T("Test"));
	return 0;
}

//[O]
// 終了する
static int LUA_Exit( lua_State *L ) {

	theApp.GetMainWnd()->DestroyWindow();
	return 0;
}

//[O]
// Sleepする
static int LUA_Sleep( lua_State *L ) {
	luaL_checkinteger(L,1);
	DWORD nSleepTime = lua_tointeger(L,1);

	DWORD nBeginTime = GetTickCount();
	while( GetTickCount() - nBeginTime < nSleepTime ) {
		theApp.OnIdle(0);
		Sleep(1);
	}
	return 0;	
}

//[O]
extern int LUA_Capture( lua_State *L );

//[O]
static const luaL_Reg FuncTable[] = {
	{"Test", LUA_Test},
	{"Capture", LUA_Capture},
	{"Sleep", LUA_Sleep },
	{"Exit", LUA_Exit},
	{0,0}
};

// CSampleGUITestApp

BEGIN_MESSAGE_MAP(CSampleGUITestApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CSampleGUITestApp::OnAppAbout)
	// 標準のファイル基本ドキュメント コマンド
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// 標準の印刷セットアップ コマンド
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CSampleGUITestApp コンストラクション

CSampleGUITestApp::CSampleGUITestApp()
{
	// 再起動マネージャーをサポートします
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// アプリケーションが共通言語ランタイム サポート (/clr) を使用して作成されている場合:
	//     1) この追加設定は、再起動マネージャー サポートが正常に機能するために必要です。
	//     2) 作成するには、プロジェクトに System.Windows.Forms への参照を追加する必要があります。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 下のアプリケーション ID 文字列を一意の ID 文字列で置換します。推奨される
	// 文字列の形式は CompanyName.ProductName.SubProduct.VersionInformation です
	SetAppID(_T("SampleGUITest.AppID.NoVersion"));

	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}

// 唯一の CSampleGUITestApp オブジェクトです。

CSampleGUITestApp theApp;

//[O] 追加
class CMyComandLineInfo : public CCommandLineInfo {
public:

	CMyComandLineInfo() : m_bAutoGUITest(false) {}

	virtual void ParseParam( 
		const TCHAR* pszParam,  
		BOOL bFlag, 
		BOOL bLast
	) {
		static bool bNextScriptName = false;
		if ( bFlag && _tcsicmp( pszParam, _T("autotest") ) == 0) {
			m_bAutoGUITest = true;	// 自動テストフラグが指定されている
			bNextScriptName = true;
		}
		else if ( bNextScriptName ) {
			// スクリプト名を得る
			bNextScriptName = false;
			m_strScriptName = pszParam;
		}
	}

	// 自動GUIテストモードか？
	bool	IsAutoGUITest() const {
		return m_bAutoGUITest;
	}

	// スクリプト名を得る
	CString		GetScriptName() const {
		return m_strScriptName;
	}
private:
	bool	m_bAutoGUITest;
	CString	m_strScriptName;
};



// CSampleGUITestApp 初期化

BOOL CSampleGUITestApp::InitInstance()
{
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// OLE ライブラリを初期化します。
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit コントロールを使用するには AfxInitRichEdit2() が必要です	
	// AfxInitRichEdit2();

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	SetRegistryKey(_T("SmapleGuiTest"));
	LoadStdProfileSettings(9);  // 標準の INI ファイルのオプションをロードします (MRU を含む)


	// アプリケーション用のドキュメント テンプレートを登録します。ドキュメント テンプレート
	//  はドキュメント、フレーム ウィンドウとビューを結合するために機能します。
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSampleGUITestDoc),
		RUNTIME_CLASS(CMainFrame),       // メイン SDI フレーム ウィンドウ
		RUNTIME_CLASS(CSampleGUITestView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// DDE、file open など標準のシェル コマンドのコマンド ラインを解析します。
//	CCommandLineInfo cmdInfo;
	// [O]
	CMyComandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// DDE Execute open を使用可能にします。
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// コマンド ラインで指定されたディスパッチ コマンドです。アプリケーションが
	// /RegServer、/Register、/Unregserver または /Unregister で起動された場合、False を返します。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// メイン ウィンドウが初期化されたので、表示と更新を行います。
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 接尾辞が存在する場合にのみ DragAcceptFiles を呼び出してください。
	//  SDI アプリケーションでは、ProcessShellCommand の直後にこの呼び出しが発生しなければなりません。
	// ドラッグ/ドロップ オープンを許可します。
	m_pMainWnd->DragAcceptFiles();


	//[O]
	if ( cmdInfo.IsAutoGUITest() ) {

		// Luaスクリプトの実行
		lua_State *L = luaL_newstate();
		luaL_openlibs(L);	// 標準ライブラリの準備
		luaL_openlib(L, "Test", FuncTable, 0);

		// コンソールを開く
		AllocConsole();

		//標準出力関連付け
		int hStdOut = ::_open_osfhandle((intptr_t)::GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
		*stdout = *::_tfdopen(hStdOut, TEXT("w"));
		::setvbuf(stdout, NULL, _IONBF, 0);

		//標準エラー出力関連付け
		int hStdErr = ::_open_osfhandle((intptr_t)::GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
		*stderr = *::_tfdopen(hStdErr, TEXT("w"));
		::setvbuf(stderr, NULL, _IONBF, 0);

		if ( luaL_dofile(L, CStringA(cmdInfo.GetScriptName()))  ) {
			printf("%s", lua_tostring(L,-1) );
		}

		// Lua終了
		lua_close(L);
	}

	return TRUE;
}

int CSampleGUITestApp::ExitInstance()
{
	//TODO: 追加したリソースがある場合にはそれらも処理してください
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

// CSampleGUITestApp メッセージ ハンドラー


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// ダイアログを実行するためのアプリケーション コマンド
void CSampleGUITestApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSampleGUITestApp メッセージ ハンドラー



