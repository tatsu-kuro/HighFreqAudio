// AudioMetryView.h : CAudioMetryView クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIOMETRYVIEW_H__4D9222DB_E012_46C0_8B44_B892DAFA82FD__INCLUDED_)
#define AFX_AUDIOMETRYVIEW_H__4D9222DB_E012_46C0_8B44_B892DAFA82FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAudioMetryView : public CFormView
{
protected: // シリアライズ機能のみから作成します。
	CAudioMetryView();
	DECLARE_DYNCREATE(CAudioMetryView)
public:
	//{{AFX_DATA(CAudioMetryView)
	enum { IDD = IDD_AUDIOMETRY_FORM };
	//}}AFX_DATA
	CBitmap m_bm_herz;
// アトリビュート
public:
	CAudioMetryDoc* GetDocument();

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CAudioMetryView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV のサポート
	virtual void OnInitialUpdate(); // 構築後の最初の１度だけ呼び出されます。
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~CAudioMetryView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(CAudioMetryView)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSavedots();
	afx_msg void OnReaddots();
	afx_msg void OnReaddotsonly();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnReturndot();
	afx_msg void OnDecrvol();
	afx_msg void OnDeletedots();
	afx_msg void OnIncvol();
	afx_msg void OnLefthz();
	afx_msg void OnRighthz();
	afx_msg void OnHzdowntab();
	afx_msg void OnHzuptab();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLefts();
	afx_msg void OnUpdateLefts(CCmdUI* pCmdUI);
	afx_msg void OnRights();
	afx_msg void OnUpdateRights(CCmdUI* pCmdUI);
	afx_msg void OnBoths();
	afx_msg void OnUpdateBoths(CCmdUI* pCmdUI);
	afx_msg void OnHzdown();
	afx_msg void OnHzup();
	afx_msg void OnSetpos();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // AudioMetryView.cpp ファイルがデバッグ環境の時使用されます。
inline CAudioMetryDoc* CAudioMetryView::GetDocument()
   { return (CAudioMetryDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_AUDIOMETRYVIEW_H__4D9222DB_E012_46C0_8B44_B892DAFA82FD__INCLUDED_)
