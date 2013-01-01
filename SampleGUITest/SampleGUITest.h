
// SampleGUITest.h : SampleGUITest アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"       // メイン シンボル


// CSampleGUITestApp:
// このクラスの実装については、SampleGUITest.cpp を参照してください。
//

class CSampleGUITestApp : public CWinApp
{
public:
	CSampleGUITestApp();


// オーバーライド
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 実装
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSampleGUITestApp theApp;
