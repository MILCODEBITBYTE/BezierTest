
// BezierTestDlg.h: 헤더 파일
//

#pragma once

#include "Bezier.h"
#include <vector>

// CBezierTestDlg 대화 상자
class CBezierTestDlg : public CDialogEx
{
// 생성입니다.
public:
	CBezierTestDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BEZIERTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


	void Draw(CDC *pDC);

	int m_iPointType;
	PVector* m_pSelected;
	BOOL m_bPick;
	BOOL m_bRotate;
	float m_fRotate = 0;
	MATRIX3x3 m_matRot;

	CPoint m_ptPrev;
	CPoint m_ptMoveOld;

	std::vector<ARC_LENGTH> m_vecArcLength;

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	

	DECLARE_MESSAGE_MAP()
public:
	int m_iPointSlide;
	int m_iPointCount;
	afx_msg void OnNMCustomdrawPointSlider(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedOkBtn();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedPointOnRadio();
	afx_msg void OnBnClickedPoint2Radio();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};
