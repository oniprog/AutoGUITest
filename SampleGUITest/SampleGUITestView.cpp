
// SampleGUITestView.cpp : CSampleGUITestView �N���X�̎���
//

#include "stdafx.h"
//[O]
#include <Gdiplusimaging.h>

// SHARED_HANDLERS �́A�v���r���[�A�T���l�C���A����ь����t�B���^�[ �n���h���[���������Ă��� ATL �v���W�F�N�g�Œ�`�ł��A
// ���̃v���W�F�N�g�Ƃ̃h�L�������g �R�[�h�̋��L���\�ɂ��܂��B
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
	// �W������R�}���h
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CSampleGUITestView �R���X�g���N�V����/�f�X�g���N�V����

CSampleGUITestView::CSampleGUITestView()
{
	// TODO: �\�z�R�[�h�������ɒǉ����܂��B

}

CSampleGUITestView::~CSampleGUITestView()
{
}

BOOL CSampleGUITestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ���̈ʒu�� CREATESTRUCT cs ���C������ Window �N���X�܂��̓X�^�C����
	//  �C�����Ă��������B

	return CView::PreCreateWindow(cs);
}

// CSampleGUITestView �`��

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


// CSampleGUITestView ���

BOOL CSampleGUITestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// ����̈������
	return DoPreparePrinting(pInfo);
}

void CSampleGUITestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ����O�̓��ʂȏ�����������ǉ����Ă��������B
}

void CSampleGUITestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �����̌㏈����ǉ����Ă��������B
}


// CSampleGUITestView �f�f

#ifdef _DEBUG
void CSampleGUITestView::AssertValid() const
{
	CView::AssertValid();
}

void CSampleGUITestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSampleGUITestDoc* CSampleGUITestView::GetDocument() const // �f�o�b�O�ȊO�̃o�[�W�����̓C�����C���ł��B
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSampleGUITestDoc)));
	return (CSampleGUITestDoc*)m_pDocument;
}
#endif //_DEBUG


#include "SampleGUITestView.h"
#include "lua.hpp"

//[O] ��ʃL���v�`��
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

	// �摜���R�s�[
	dcDest.BitBlt(0, 0, rectDraw.Width(), rectDraw.Height(), &dcSrc, 0, 0, SRCCOPY );

	// PNG�`���ŕۑ�
	image.Save( strFileName, ImageFormatPNG );

	dcDest.Detach();
	image.ReleaseDC();

	return 0;
}
