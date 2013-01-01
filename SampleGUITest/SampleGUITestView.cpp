
// SampleGUITestView.cpp : CSampleGUITestView クラスの実装
//

#include "stdafx.h"
//[O]
#include <Gdiplusimaging.h>

// SHARED_HANDLERS は、プレビュー、サムネイル、および検索フィルター ハンドラーを実装している ATL プロジェクトで定義でき、
// そのプロジェクトとのドキュメント コードの共有を可能にします。
#ifndef SHARED_HANDLERS
#include "SampleGUITest.h"
#endif

#include "SampleGUITestDoc.h"
#include "SampleGUITestView.h"

//[O]
DEFINE_GUID(ImageFormatPNG, 0xb96b3caf,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSampleGUITestView

IMPLEMENT_DYNCREATE(CSampleGUITestView, CView)

BEGIN_MESSAGE_MAP(CSampleGUITestView, CView)
	// 標準印刷コマンド
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CSampleGUITestView コンストラクション/デストラクション

CSampleGUITestView::CSampleGUITestView()
{
	// TODO: 構築コードをここに追加します。

}

CSampleGUITestView::~CSampleGUITestView()
{
}

BOOL CSampleGUITestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	return CView::PreCreateWindow(cs);
}

// CSampleGUITestView 描画

void CSampleGUITestView::OnDraw(CDC* pDC)
{
	CSampleGUITestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//[O]
	srand( time(0) );

	CRect rectDraw;
	GetClientRect(&rectDraw);

	pDC->FillRect( &rectDraw, &CBrush( rand() ) );
}


// CSampleGUITestView 印刷

BOOL CSampleGUITestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 既定の印刷準備
	return DoPreparePrinting(pInfo);
}

void CSampleGUITestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷前の特別な初期化処理を追加してください。
}

void CSampleGUITestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷後の後処理を追加してください。
}


// CSampleGUITestView 診断

#ifdef _DEBUG
void CSampleGUITestView::AssertValid() const
{
	CView::AssertValid();
}

void CSampleGUITestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSampleGUITestDoc* CSampleGUITestView::GetDocument() const // デバッグ以外のバージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSampleGUITestDoc)));
	return (CSampleGUITestDoc*)m_pDocument;
}
#endif //_DEBUG


#include "SampleGUITestView.h"
#include "lua.hpp"

//[O] 画面キャプチャ
int LUA_Capture( lua_State *L ) {

	luaL_checkstring(L, 1);

	CString strFileName( lua_tostring(L,1) );

	CSampleGUITestView *pView = static_cast<CSampleGUITestView*>( theApp.GetMainWnd()->GetActiveWindow() );
	
	CRect rectDraw;
	pView->GetClientRect(&rectDraw);

	CImage image;
	image.Create( rectDraw.Width(), rectDraw.Height(), 24 );
	

	CClientDC dcSrc( pView );
	CDC dcDest;
	dcDest.Attach( image.GetDC() );

	// 画像をコピー
	dcDest.BitBlt(0, 0, rectDraw.Width(), rectDraw.Height(), &dcSrc, 0, 0, SRCCOPY );

	// PNG形式で保存
	image.Save( strFileName, ImageFormatPNG );

	dcDest.Detach();
	image.ReleaseDC();

	return 0;
}
