
// BezierTestDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "BezierTest.h"
#include "BezierTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBezierTestDlg 대화 상자

PVector Points[3] = { PVector(-200, 0), PVector(0, 0), PVector(200, 0) };

PVector CpPoints[4] = { PVector(-150, 100), PVector(-50, 100), PVector(50, -100), PVector(150, -100) };

CBezierTestDlg::CBezierTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BEZIERTEST_DIALOG, pParent)
	, m_iPointSlide(0)
	, m_iPointCount(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBezierTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Slider(pDX, IDC_POINT_SLIDER, m_iPointSlide);
	DDX_Text(pDX, IDC_POINT_EDIT, m_iPointCount);
}

void DrawEllipse(CDC* pDC, PVector p, int r = 5)
{
	pDC->Ellipse(int(p.x - r), int(p.y - r), int(p.x + r), int(p.y + r));
}

PVector ConvViewport(const PVector& p, const PVector& viewport)
{
	return PVector(p.x + viewport.x, -p.y + viewport.y);
}

PVector ConvDeviceCoord(const PVector& p, const PVector& viewport)
{
	return PVector(p.x - viewport.x, -(p.y - viewport.y));
}

void CBezierTestDlg::Draw(CDC *pDC)
{
	CBrush brPoint, * pOld;
	CBrush brWay, brCP;

	brPoint.CreateSolidBrush(RGB(255, 0, 0));
	brCP.CreateSolidBrush(RGB(0, 0, 255));
	brWay.CreateSolidBrush(RGB(255, 255, 0));


	CRect rect;
	GetClientRect(&rect);
	PVector viewport(rect.Width() / 2.f, rect.Height() / 2.f);


	pOld = (CBrush *)pDC->SelectObject(brWay);

	//local -> world
	PVector _Points[3] = { VectorTransform(Points[0], m_matRot), VectorTransform(Points[1], m_matRot), VectorTransform(Points[2], m_matRot)};
	PVector _CpPoints[4] = { VectorTransform(CpPoints[0], m_matRot), VectorTransform(CpPoints[1], m_matRot), VectorTransform(CpPoints[2], m_matRot), VectorTransform(CpPoints[3], m_matRot) };

	m_vecArcLength.clear();

	PVector arc_old;
	ARC_LENGTH arc;

	int point_index = 0;
	int cp_index = 0;

	//베지어 곡선 그리기
	while(point_index < 2)
	{
		for (int i = 1; i <= m_iPointCount; ++i)
		{
			float amt = i / (float)m_iPointCount;

			if (m_iPointType == 0)
			{
				PVector r = BezierPoint(_Points[point_index], _Points[point_index + 1], _CpPoints[cp_index], amt);
				DrawEllipse(pDC, ConvViewport(r, viewport), 5);

				if (i == 1)
					arc_old = _Points[point_index];

				arc = ARC_LENGTH{ arc_old, r, 1 };

				arc_old = r;

				m_vecArcLength.push_back(arc);
			}
			else
			{
				PVector r = BezierPoint2(_Points[point_index], _Points[point_index+1], _CpPoints[cp_index], _CpPoints[cp_index+1], amt);
				DrawEllipse(pDC, ConvViewport(r, viewport), 5);

				if (i == 1)
					arc_old = _Points[point_index];

				arc = ARC_LENGTH{ arc_old, r, 1 };

				arc_old = r;
				m_vecArcLength.push_back(arc);
			}
		}

		point_index++;
		cp_index += 2;
	}


	//포인트와 콘트롤 그리기

	pDC->SelectObject(brPoint);

	for (int i = 0; i < 3; ++i)
		DrawEllipse(pDC, ConvViewport(_Points[i], viewport), 5);

	pDC->SelectObject(brCP);

	//1 point control
	if (m_iPointType == 0)
	{
		DrawEllipse(pDC, ConvViewport(_CpPoints[0], viewport), 5);
		DrawEllipse(pDC, ConvViewport(_CpPoints[2], viewport), 5);
	}
	//2 point control
	else
	{
		for (int i = 0; i < 4; ++i)
			DrawEllipse(pDC, ConvViewport(_CpPoints[i], viewport), 5);
	}

	printf("arc len\r\n");
	for (size_t i = 0; i < m_vecArcLength.size(); ++i)
	{
		PVector p1 = ConvViewport(m_vecArcLength[i].s, viewport);
		PVector p2 = ConvViewport(m_vecArcLength[i].e, viewport);

		pDC->MoveTo(p1.x, p1.y);
		pDC->LineTo(p2.x, p2.y);

		printf("[%lu] : len = %f\n", i, m_vecArcLength[i].Length());
	}
	printf("\r\n");

}

BEGIN_MESSAGE_MAP(CBezierTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_POINT_SLIDER, &CBezierTestDlg::OnNMCustomdrawPointSlider)
	ON_BN_CLICKED(IDC_OK_BTN, &CBezierTestDlg::OnBnClickedOkBtn)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_POINT_ON_RADIO, &CBezierTestDlg::OnBnClickedPointOnRadio)
	ON_BN_CLICKED(IDC_POINT2_RADIO, &CBezierTestDlg::OnBnClickedPoint2Radio)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// CBezierTestDlg 메시지 처리기

BOOL CBezierTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	m_iPointType = 1;
	m_iPointCount = 10;

	CSliderCtrl* pSlider = reinterpret_cast<CSliderCtrl*>(GetDlgItem(IDC_POINT_SLIDER));
	pSlider->SetRange(1, 100);
	pSlider->SetPos(10);

	GetDlgItem(IDC_POINT2_RADIO)->SendMessage(BM_SETCHECK, BST_CHECKED, 0);

	m_matRot.Identity();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CBezierTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CBezierTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		Draw(&dc);


		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CBezierTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}






void CBezierTestDlg::OnNMCustomdrawPointSlider(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CSliderCtrl* pSlider = reinterpret_cast<CSliderCtrl*>(GetDlgItem(IDC_POINT_SLIDER));

	SetDlgItemInt(IDC_POINT_EDIT, pSlider->GetPos());

	UpdateWindow();
	*pResult = 0;
}


void CBezierTestDlg::OnBnClickedOkBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);

	Invalidate();
	UpdateWindow();
}


void CBezierTestDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CRect rect;
	GetClientRect(&rect);

	PVector viewport(rect.Width() / 2.f, rect.Height() / 2.f);

	m_pSelected = nullptr;

	//viewport -> world
	PVector pick = ConvDeviceCoord(PVector((float)point.x, (float)point.y), viewport);


	PVector target;
	for (int i = 0; i < 3; ++i)
	{
		//local -> world
		target = VectorTransform(Points[i], m_matRot);

		if (Distance(target, pick) < 5)
		{
			m_pSelected = &Points[i];
			m_bPick = TRUE;
			return;
		}
	}

	for (int i = 0; i < 5; ++i)
	{
		//local -> world
		target = VectorTransform(CpPoints[i], m_matRot);
		if (Distance(target, pick) < 5)
		{
			m_pSelected = &CpPoints[i];
			m_bPick = TRUE;
			return;
		}
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CBezierTestDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	m_bPick = FALSE;
	m_pSelected = nullptr;
	m_ptMoveOld = CPoint(0, 0);
	Invalidate();

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CBezierTestDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CRect rect;
	GetClientRect(&rect);

	PVector viewport(rect.Width() / 2.f, rect.Height() / 2.f);


	if (m_bPick)
	{
		if(m_ptMoveOld.x == 0 && m_ptMoveOld.y == 0)
			m_ptMoveOld = point;

		//world -> local
		MATRIX3x3 rRot;
		rRot.Rotate(-m_fRotate);

		//viewport -> world
		PVector or , old;
		or = ConvDeviceCoord(PVector((float)point.x, (float)point.y), viewport);
		old = ConvDeviceCoord(PVector((float)m_ptMoveOld.x, (float)m_ptMoveOld.y), viewport);
		PVector move = or - old;
		//world -> local
		move = VectorTransform(move, rRot);
		
		m_ptMoveOld = point;

		*m_pSelected += move;
		Invalidate();
	}

	if (m_bRotate)
	{
		m_fRotate += (float)(point.x - m_ptPrev.x) * 0.1f;
		m_ptPrev = point;

		m_matRot.Rotate(m_fRotate);
		Invalidate();
	}
	

	CDialogEx::OnMouseMove(nFlags, point);
}


void CBezierTestDlg::OnBnClickedPointOnRadio()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iPointType = 0;
	Invalidate();
}


void CBezierTestDlg::OnBnClickedPoint2Radio()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iPointType = 1;
	Invalidate();
}


void CBezierTestDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	SetCapture();
	m_ptPrev = point;
	m_bRotate = TRUE;

	CDialogEx::OnRButtonDown(nFlags, point);
}


void CBezierTestDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	m_bRotate = FALSE;
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnRButtonUp(nFlags, point);
}
