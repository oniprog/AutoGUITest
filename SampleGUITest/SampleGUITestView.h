
// SampleGUITestView.h : CSampleGUITestView クラスのインターフェイス
//

#pragma once


class CSampleGUITestView : public CView
{
protected: // シリアル化からのみ作成します。
	CSampleGUITestView();
	DECLARE_DYNCREATE(CSampleGUITestView)

// 属性
public:
	CSampleGUITestDoc* GetDocument() const;

// 操作
public:

// オーバーライド
public:
	virtual void OnDraw(CDC* pDC);  // このビューを描画するためにオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 実装
public:
	virtual ~CSampleGUITestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成された、メッセージ割り当て関数
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // SampleGUITestView.cpp のデバッグ バージョン
inline CSampleGUITestDoc* CSampleGUITestView::GetDocument() const
   { return reinterpret_cast<CSampleGUITestDoc*>(m_pDocument); }
#endif

