
// SampleGUITest.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
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
// �_�C�A���O���o��
static int LUA_Test( lua_State *L ) {
	AfxMessageBox(_T("Test"));
	return 0;
}

//[O]
// �I������
static int LUA_Exit( lua_State *L ) {

	theApp.GetMainWnd()->DestroyWindow();
	return 0;
}

//[O]
// Sleep����
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
	// �W���̃t�@�C����{�h�L�������g �R�}���h
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// �W���̈���Z�b�g�A�b�v �R�}���h
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CSampleGUITestApp �R���X�g���N�V����

CSampleGUITestApp::CSampleGUITestApp()
{
	// �ċN���}�l�[�W���[���T�|�[�g���܂�
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// �A�v���P�[�V���������ʌ��ꃉ���^�C�� �T�|�[�g (/clr) ���g�p���č쐬����Ă���ꍇ:
	//     1) ���̒ǉ��ݒ�́A�ċN���}�l�[�W���[ �T�|�[�g������ɋ@�\���邽�߂ɕK�v�ł��B
	//     2) �쐬����ɂ́A�v���W�F�N�g�� System.Windows.Forms �ւ̎Q�Ƃ�ǉ�����K�v������܂��B
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: ���̃A�v���P�[�V���� ID ���������ӂ� ID ������Œu�����܂��B���������
	// ������̌`���� CompanyName.ProductName.SubProduct.VersionInformation �ł�
	SetAppID(_T("SampleGUITest.AppID.NoVersion"));

	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}

// �B��� CSampleGUITestApp �I�u�W�F�N�g�ł��B

CSampleGUITestApp theApp;

//[O] �ǉ�
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
			m_bAutoGUITest = true;	// �����e�X�g�t���O���w�肳��Ă���
			bNextScriptName = true;
		}
		else if ( bNextScriptName ) {
			// �X�N���v�g���𓾂�
			bNextScriptName = false;
			m_strScriptName = pszParam;
		}
	}

	// ����GUI�e�X�g���[�h���H
	bool	IsAutoGUITest() const {
		return m_bAutoGUITest;
	}

	// �X�N���v�g���𓾂�
	CString		GetScriptName() const {
		return m_strScriptName;
	}
private:
	bool	m_bAutoGUITest;
	CString	m_strScriptName;
};



// CSampleGUITestApp ������

BOOL CSampleGUITestApp::InitInstance()
{
	// �A�v���P�[�V���� �}�j�t�F�X�g�� visual �X�^�C����L���ɂ��邽�߂ɁA
	// ComCtl32.dll Version 6 �ȍ~�̎g�p���w�肷��ꍇ�́A
	// Windows XP �� InitCommonControlsEx() ���K�v�ł��B�����Ȃ���΁A�E�B���h�E�쐬�͂��ׂĎ��s���܂��B
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �A�v���P�[�V�����Ŏg�p���邷�ׂẴR���� �R���g���[�� �N���X���܂߂�ɂ́A
	// �����ݒ肵�܂��B
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// OLE ���C�u���������������܂��B
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit �R���g���[�����g�p����ɂ� AfxInitRichEdit2() ���K�v�ł�	
	// AfxInitRichEdit2();

	// �W��������
	// �����̋@�\���g�킸�ɍŏI�I�Ȏ��s�\�t�@�C����
	// �T�C�Y���k���������ꍇ�́A�ȉ�����s�v�ȏ�����
	// ���[�`�����폜���Ă��������B
	// �ݒ肪�i�[����Ă��郌�W�X�g�� �L�[��ύX���܂��B
	// TODO: ��Ж��܂��͑g�D���Ȃǂ̓K�؂ȕ������
	// ���̕������ύX���Ă��������B
	SetRegistryKey(_T("SmapleGuiTest"));
	LoadStdProfileSettings(9);  // �W���� INI �t�@�C���̃I�v�V���������[�h���܂� (MRU ���܂�)


	// �A�v���P�[�V�����p�̃h�L�������g �e���v���[�g��o�^���܂��B�h�L�������g �e���v���[�g
	//  �̓h�L�������g�A�t���[�� �E�B���h�E�ƃr���[���������邽�߂ɋ@�\���܂��B
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSampleGUITestDoc),
		RUNTIME_CLASS(CMainFrame),       // ���C�� SDI �t���[�� �E�B���h�E
		RUNTIME_CLASS(CSampleGUITestView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// DDE�Afile open �ȂǕW���̃V�F�� �R�}���h�̃R�}���h ���C������͂��܂��B
//	CCommandLineInfo cmdInfo;
	// [O]
	CMyComandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// DDE Execute open ���g�p�\�ɂ��܂��B
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// �R�}���h ���C���Ŏw�肳�ꂽ�f�B�X�p�b�` �R�}���h�ł��B�A�v���P�[�V������
	// /RegServer�A/Register�A/Unregserver �܂��� /Unregister �ŋN�����ꂽ�ꍇ�AFalse ��Ԃ��܂��B
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// ���C�� �E�B���h�E�����������ꂽ�̂ŁA�\���ƍX�V���s���܂��B
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �ڔ��������݂���ꍇ�ɂ̂� DragAcceptFiles ���Ăяo���Ă��������B
	//  SDI �A�v���P�[�V�����ł́AProcessShellCommand �̒���ɂ��̌Ăяo�����������Ȃ���΂Ȃ�܂���B
	// �h���b�O/�h���b�v �I�[�v���������܂��B
	m_pMainWnd->DragAcceptFiles();


	//[O]
	if ( cmdInfo.IsAutoGUITest() ) {

		// Lua�X�N���v�g�̎��s
		lua_State *L = luaL_newstate();
		luaL_openlibs(L);	// �W�����C�u�����̏���
		luaL_openlib(L, "Test", FuncTable, 0);

		// �R���\�[�����J��
		AllocConsole();

		//�W���o�͊֘A�t��
		int hStdOut = ::_open_osfhandle((intptr_t)::GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
		*stdout = *::_tfdopen(hStdOut, TEXT("w"));
		::setvbuf(stdout, NULL, _IONBF, 0);

		//�W���G���[�o�͊֘A�t��
		int hStdErr = ::_open_osfhandle((intptr_t)::GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
		*stderr = *::_tfdopen(hStdErr, TEXT("w"));
		::setvbuf(stderr, NULL, _IONBF, 0);

		if ( luaL_dofile(L, CStringA(cmdInfo.GetScriptName()))  ) {
			printf("%s", lua_tostring(L,-1) );
		}

		// Lua�I��
		lua_close(L);
	}

	return TRUE;
}

int CSampleGUITestApp::ExitInstance()
{
	//TODO: �ǉ��������\�[�X������ꍇ�ɂ͂������������Ă�������
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

// CSampleGUITestApp ���b�Z�[�W �n���h���[


// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
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

// �_�C�A���O�����s���邽�߂̃A�v���P�[�V���� �R�}���h
void CSampleGUITestApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSampleGUITestApp ���b�Z�[�W �n���h���[



