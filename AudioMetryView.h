// AudioMetryView.h : CAudioMetryView �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂��B
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIOMETRYVIEW_H__4D9222DB_E012_46C0_8B44_B892DAFA82FD__INCLUDED_)
#define AFX_AUDIOMETRYVIEW_H__4D9222DB_E012_46C0_8B44_B892DAFA82FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAudioMetryView : public CFormView
{
protected: // �V���A���C�Y�@�\�݂̂���쐬���܂��B
	CAudioMetryView();
	DECLARE_DYNCREATE(CAudioMetryView)
public:
	//{{AFX_DATA(CAudioMetryView)
	enum { IDD = IDD_AUDIOMETRY_FORM };
	//}}AFX_DATA
	CBitmap m_bm_herz;
// �A�g���r���[�g
public:
	CAudioMetryDoc* GetDocument();

// �I�y���[�V����
public:

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CAudioMetryView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �̃T�|�[�g
	virtual void OnInitialUpdate(); // �\�z��̍ŏ��̂P�x�����Ăяo����܂��B
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	virtual ~CAudioMetryView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �������ꂽ���b�Z�[�W �}�b�v�֐�
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

#ifndef _DEBUG  // AudioMetryView.cpp �t�@�C�����f�o�b�O���̎��g�p����܂��B
inline CAudioMetryDoc* CAudioMetryView::GetDocument()
   { return (CAudioMetryDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_AUDIOMETRYVIEW_H__4D9222DB_E012_46C0_8B44_B892DAFA82FD__INCLUDED_)
