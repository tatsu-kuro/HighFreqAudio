// AudioMetryView.cpp : CAudioMetryView クラスの動作の定義を行います。
//リオンのオージオメーターは225msの音と225msのインターバル

#include "stdafx.h"
#include "AudioMetry.h"

#include "AudioMetryDoc.h"
#include "AudioMetryView.h"
#include <math.h>
#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define TABW 16
#define NAe 2.718282
#define PAI 3.141592653589793238
enum Sound_Side
{
    L, 
	R,
    B, 
};
Sound_Side SND_S;
int wave_make(double fq,int vol);//double A=0.95);
void GetWaveVolume(DWORD* vol);
void SetWaveVolume(DWORD vol);
void SetWaveV(int vol);//Sound_Side ss);
void DispIro(CDC *pDC,CBitmap *bit,int DN);
DWORD WVolume;
int	VirHz=100,TrueHz;
int	VirVol=0;
int TrueVol=0;//0-480
#define Beki 400.0//pow(10,double(vol)/80.0) <--この80.0のところ
int oldhz=1000,oldvol=0;
DWORD grayRGB;
bool lbdF=false;
bool lbdmF=false;
#define MAXHZ 22627
#define MAXXN 480
#define HZVOL 1
#define EWAKU 2
#define DOTS 4
int HzUPF=0;
//オージオグラムの枠
#define AUDWX 1
#define AUDWDX 999
#define AUDWY 1
#define AUDWDY 545//665-120
//オージオグラム座標
#define AUDGX 16
#define AUDGY 35
#define AUDGX2 976
#define AUDGY2 515//635-120
#define AUDGDX 128
struct HzVolDots{
	int hz;
	int vol;
};
HzVolDots dots[501];
HzVolDots dots_d[501];
unsigned char snddata[0x5000];

/////////////////////////////////////////////////////////////////////////////
// CAudioMetryView

IMPLEMENT_DYNCREATE(CAudioMetryView, CFormView)

BEGIN_MESSAGE_MAP(CAudioMetryView, CFormView)
	//{{AFX_MSG_MAP(CAudioMetryView)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_SAVEDOTS, OnSavedots)
	ON_COMMAND(ID_READDOTS, OnReaddots)
	ON_COMMAND(ID_READDOTSONLY, OnReaddotsonly)
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONUP()
	ON_WM_DESTROY()
	ON_COMMAND(ID_RETURNDOT, OnReturndot)
	ON_COMMAND(ID_DECRVOL, OnDecrvol)
	ON_COMMAND(ID_DELETEDOTS, OnDeletedots)
	ON_COMMAND(ID_INCVOL, OnIncvol)
	ON_COMMAND(ID_LEFTHZ, OnLefthz)
	ON_COMMAND(ID_RIGHTHZ, OnRighthz)
	ON_COMMAND(ID_HZDOWNTAB, OnHzdowntab)
	ON_COMMAND(ID_HZUPTAB, OnHzuptab)
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_LEFTS, OnLefts)
	ON_UPDATE_COMMAND_UI(ID_LEFTS, OnUpdateLefts)
	ON_COMMAND(ID_RIGHTS, OnRights)
	ON_UPDATE_COMMAND_UI(ID_RIGHTS, OnUpdateRights)
	ON_COMMAND(ID_BOTHS, OnBoths)
	ON_UPDATE_COMMAND_UI(ID_BOTHS, OnUpdateBoths)
	ON_BN_CLICKED(IDC_HZDOWN, OnHzdown)
	ON_BN_CLICKED(IDC_HZUP, OnHzup)
	ON_BN_CLICKED(IDC_SETPOS, OnSetpos)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioMetryView クラスの構築/消滅

CAudioMetryView::CAudioMetryView()
	: CFormView(CAudioMetryView::IDD)
{
	//{{AFX_DATA_INIT(CAudioMetryView)
	//}}AFX_DATA_INIT
	// TODO: この場所に構築用のコードを追加してください。

}

CAudioMetryView::~CAudioMetryView()
{
}

void CAudioMetryView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioMetryView)
	//}}AFX_DATA_MAP
}

BOOL CAudioMetryView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	return CFormView::PreCreateWindow(cs);
}
#define Black RGB(0,0,0)
#define White RGB(255,255,255)
void DrawRect(CDC* pDC,int x,int y,int dx,int dy)
{
	pDC->FillSolidRect(x,y,dx,dy,RGB(255,255,255));
	pDC->FillSolidRect(x,y,dx+1,1,RGB(0,0,0));
	pDC->FillSolidRect(x,y,1,dy,RGB(0,0,0));
	pDC->FillSolidRect(x,y+dy,dx+1,1,RGB(0,0,0));
	pDC->FillSolidRect(x+dx,y+1,1,dy,RGB(0,0,0));
}

void DrawRect2(CDC* pDC,int x,int y,int dx,int dy)
{
	pDC->FillSolidRect(x,y,dx,dy,grayRGB);
	pDC->FillSolidRect(x,y,dx+1,1,White);
	pDC->FillSolidRect(x,y,1,dy+1,White);
	pDC->FillSolidRect(x,y+dy,dx+1,1,RGB(155,155,155));
	pDC->FillSolidRect(x+dx,y,1,dy+1,RGB(155,155,155));
}
int X2Hz[1200];
void SetX2Hz()//int x,int *hz)
{
	int x,herz;
	double dhz;
	for(x=0;x<1200;x++){
		dhz=1000*exp(double(x-128*3)/128.0*0.693147);
		herz=int(dhz);
		if(int(dhz*10.0)-herz*10>5)herz+=1;
		X2Hz[x]=herz;
	}
}
int Hz2X(int hz)
{
	int i;
	for(i=0;i<AUDGX2-AUDGX+1;i++)if(hz==X2Hz[i])return i;
	return -1;
}
int GetNewHz(int hz,int x)
{
	int i;
	for(i=0;i<AUDGX2-AUDGX+1;i++)if(X2Hz[i]==hz)break;
	if(i+x<0)return 125;
	else if(i+x>AUDGX2-AUDGX)return 22627;
	return X2Hz[i+x];
}
void CAudioMetryView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	GetWaveVolume(&WVolume);//WVolumeに初期値を保存
	m_bm_herz.LoadBitmap(IDB_HERZ);
	TrueHz=2000;
	TrueVol=0;
	grayRGB=GetSysColor(COLOR_BTNFACE);
	SetX2Hz();
	SND_S=B;
	SetWaveV(TrueVol);//最後にもどす。
	wave_make(TrueHz,0);
	SetTimer(1,450,NULL);	
	for(int i=0;i<500;i++){
		dots[i].hz=0;
		dots[i].vol=0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAudioMetryView クラスの診断

#ifdef _DEBUG
void CAudioMetryView::AssertValid() const
{
	CFormView::AssertValid();
}

void CAudioMetryView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CAudioMetryDoc* CAudioMetryView::GetDocument() // 非デバッグ バージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAudioMetryDoc)));
	return (CAudioMetryDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAudioMetryView クラスのメッセージ ハンドラ
typedef struct
{	
	int fs; // 標本化周波数 //
	int bits; // 量子化精度 //
	int length; // 音データの長さ //
	double *s; // 音データ //
} MONO_PCM;
void mono_wave_write(MONO_PCM *pcm);
int wave_make(double fq,int vol)//0-480
{
	MONO_PCM pcm1;
	int n,k,de;
	double f0;
	//  pcm1.fs = 8000; // 標本化周波数 //
	pcm1.fs = 44100; // 標本化周波数 //
	pcm1.bits = 16; // 量子化精度 //
	pcm1.length = 44100*2/9;///2; // 音データの長さ //
	pcm1.s =(double*) calloc(pcm1.length, sizeof(double)); // メモリの確保 //
	//  pcm1.s = malloc(pcm1.length*sizeof(double));//calloc(pcm1.length, sizeof(double)); // メモリの確保 //
	//  sndb = new unsigned char[SNDB+100];
	double A;// 振幅 //
//	A=pow(10,double(vol)/80.0)/double(256*256);
	A=pow(10,double(vol)/Beki)/pow(10,480.0/Beki);
	f0 = fq; // 基本周波数 //
	de=pcm1.length/4;
	// サイン波 //
	for (n = 0,k=0; n < pcm1.length; n++)
	{
		if(n<de)k++;
		if(n>pcm1.length-de)k--;
		pcm1.s[n] = A * sin(2.0 * PAI * f0 * n / pcm1.fs)*k/de;
	}

	mono_wave_write(&pcm1); // WAVEファイルにモノラルの音データを出力する //

	free(pcm1.s); // メモリの解放 //

	return 0;
}
static unsigned char soundhead[0x40]={
	0x52,0x49,0x46,0x46,0xB4,0x4C,0x00,0x00, 0x57,0x41,0x56,0x45,0x66,0x6d,0x74,0x20,
	0x10,0x00,0x00,0x00,0x01,0x00,0x01,0x00, 0x44,0xAC,0x00,0x00,0x88,0x58,0x01,0x00,
	0x02,0x00,0x10,0x00,0x64,0x61,0x74,0x61, 0x90,0x4C,0x00,0x00,0x00,0x00,0x06,0x00,
};
void mono_wave_write(MONO_PCM *pcm)
{
	int i,n;
	short *sh;
	short data;
	double s;
	for(i=0;i<0x30;i++)snddata[i]=soundhead[i];
	sh=(short*)&snddata[i];
	for (n = 0; n < pcm->length; n++)
	{
		s = (pcm->s[n] + 1.0) / 2.0 * 65536.0; 
		if (s > 65535.0)
		{
			s = 65535.0; // クリッピング //
		}
		else if (s < 0.0)
		{
			s = 0.0; // クリッピング //
		}
		data = (short)(s + 0.5) - 32768; // 四捨五入とオフセットの調節 //
		sh[n]=data;
	}
}
void GetWaveVolume(DWORD* vol)
{
	HWAVEOUT hWaveOut;
	WAVEFORMATEX wfe;
	wfe.wFormatTag = WAVE_FORMAT_PCM;
	wfe.nChannels = 1; //モノラル
	wfe.wBitsPerSample = 16; //量子化ビット数
	wfe.nBlockAlign = wfe.nChannels * wfe.wBitsPerSample/8;
	wfe.nSamplesPerSec = 44100; //標本化周波数
	wfe.nAvgBytesPerSec = wfe.nSamplesPerSec * wfe.nBlockAlign;

	waveOutOpen(&hWaveOut,WAVE_MAPPER,&wfe,NULL,0,CALLBACK_WINDOW);
	waveOutGetVolume(hWaveOut,vol);
}
void SetWaveVolume(DWORD vol)
{

	HWAVEOUT hWaveOut;
	WAVEFORMATEX wfe;
	wfe.wFormatTag = WAVE_FORMAT_PCM;
	wfe.nChannels = 1; //モノラル
	wfe.wBitsPerSample = 16; //量子化ビット数
	wfe.nBlockAlign = wfe.nChannels * wfe.wBitsPerSample/8;
	wfe.nSamplesPerSec = 44100; //標本化周波数
	wfe.nAvgBytesPerSec = wfe.nSamplesPerSec * wfe.nBlockAlign;

	waveOutOpen(&hWaveOut,WAVE_MAPPER,&wfe,NULL,0,CALLBACK_WINDOW);
	waveOutSetVolume(hWaveOut, vol);
}	
void SetWaveV(int vol)//0-480
{
	//db=10log10(x/1)
	//x=10^(vol/40)^(1/2)
	int x=255.0*256.0*pow(10,double(vol)/Beki)/pow(10,double(480.0)/Beki);
	if(SND_S==L)SetWaveVolume(MAKELONG(x,0));
	else if(SND_S==R)SetWaveVolume(MAKELONG(0,x));
	else SetWaveVolume(MAKELONG(x,x));
}

void CAudioMetryView::OnTimer(UINT nIDEvent) 
{
	static int svol=0,shz=0;
	if(nIDEvent==1){
		if(svol!=TrueVol||shz!=TrueHz){
			if(lbdmF==false){//LButtonが押され、かつマウスが動いているときにtrue
				wave_make(TrueHz,TrueVol);
				SetWaveV(TrueVol);
				svol=TrueVol;
				shz=TrueHz;
			}
		}else{
			PlaySound((char*)snddata,NULL,SND_MEMORY|SND_ASYNC);
		}
	}
	CFormView::OnTimer(nIDEvent);
}
void DrawAudioG(CDC* pDC,CPen *pen,HzVolDots ten[],DWORD rgb)
{
	int i;
	for(i=0;i<AUDGX+AUDGX2+1;i++){//聴力像の点を打つ
		if(ten[i].hz==0)break;
		pDC->FillSolidRect(Hz2X(ten[i].hz)+AUDGX-1,ten[i].vol+AUDGY-1,3,3,rgb);//RGB(0,0,255));
	}
	if(ten[1].hz!=0){//聴力像の点を結ぶ
		for(i=1;ten[i].hz!=0;i++){
			pDC->MoveTo(Hz2X(ten[i-1].hz)+AUDGX,ten[i-1].vol+AUDGY);
			pDC->LineTo(Hz2X(ten[i].hz)+AUDGX,ten[i].vol+AUDGY);
		}
	}
}
void DispIro(CDC *pDC,CBitmap *bit,int DN)//システムのボタンの背景色をセットする。
{
	int i;//,x,y;
	char buf[300];
	CDC MemDC;
	CBitmap *pOldBmp;
	CFont *myFont;
	myFont = new CFont;
	myFont->CreatePointFont(100, _T("ＭＳ Ｐゴシック") );
	pDC->SelectObject(myFont);

	MemDC.CreateCompatibleDC(pDC);
	pOldBmp=MemDC.SelectObject(bit);
	if(DN&EWAKU)
		DrawRect(pDC,AUDWX,AUDWY,AUDWDX-10,AUDWDY-10);//聴力図枠の全体を白で塗りつぶす。
	if(DN&DOTS){
		pDC->FillSolidRect(Hz2X(oldhz)+AUDGX-2,oldvol+AUDGY-2,5,5,RGB(255,255,255));//赤点を白で
		for(i=32;i<60;i++)pDC->FillSolidRect(AUDGX+i*AUDGDX/8,AUDGY,1,480,RGB(240,240,240));
		for(i=0;i<8;i++){//縦線表示
			pDC->FillSolidRect(AUDGX+i*AUDGDX,AUDGY,1,480,RGB(200,200,200));
		}
		pDC->FillSolidRect(AUDGX2,AUDGY,1,480,RGB(200,200,200));
		for(i=0;i<13;i++)//横線表示
			pDC->FillSolidRect(AUDGX,AUDGY+i*20*2,AUDGX2-AUDGX+1,1,RGB(200,200,200));
		for(i=0;i<1;i++)pDC->BitBlt(10+AUDGDX*i,16,22,13,&MemDC,i*22,0,SRCCOPY);//hz
		for(;i<8;i++)pDC->BitBlt(AUDGDX*i,16,22,13,&MemDC,i*22,0,SRCCOPY);//hz
		for(i=0;i<1;i++)pDC->BitBlt(10+AUDGDX*i+24,16,26,13,&MemDC,176,0,SRCCOPY);
		for(;i<3;i++)pDC->BitBlt(AUDGDX*i+24,16,26,13,&MemDC,176,0,SRCCOPY);
		for(;i<8;i++)pDC->BitBlt(AUDGDX*i+15,16,26,13,&MemDC,176,0,SRCCOPY);
		pDC->FillSolidRect(Hz2X(TrueHz)+AUDGX-2,TrueVol+AUDGY-1,5,3,RGB(255,10,10));//赤点
		pDC->FillSolidRect(Hz2X(TrueHz)+AUDGX-1,TrueVol+AUDGY-2,3,5,RGB(255,10,10));//赤点
		oldhz=TrueHz;
		oldvol=TrueVol;
		CPen pen1(PS_SOLID,1,RGB(50,50,190));
		CPen* oldpen=pDC->SelectObject(&pen1);
		DrawAudioG(pDC,&pen1,dots,RGB(0,0,255));
		CPen pen2(PS_SOLID,1,RGB(200,200,200));
		pDC->SelectObject(&pen2);
		DrawAudioG(pDC,&pen2,dots_d,RGB(155,155,155));
		pDC->SelectObject(oldpen);
	}
	if(DN&HZVOL){
		int hzvoly=550;
		pDC->SetBkColor(grayRGB);
		sprintf(buf,"(発音中) Hz:%05d 音圧:%03d",TrueHz,TrueVol/4);//周波数・音量の表示
		pDC->TextOut(30,hzvoly,buf);
		sprintf(buf,"(カーソル位置) Hz:%05d 音圧:%03d",VirHz,VirVol/4);
//		sprintf(buf,"%8d",int(pow(10,double(TrueVol)/80.0)));
		pDC->TextOut(780,hzvoly,buf);
	}
	delete myFont; 
	MemDC.SelectObject(pOldBmp);
	DeleteDC(MemDC);
}

void CAudioMetryView::OnDraw(CDC* pDC) 
{
	static bool init=true;
	if(init){
		init=false;
/*		CRect rect,mRect;
		GetDesktopWindow()->GetWindowRect(&mRect);
		rect.top=(mRect.bottom-mRect.top-750)/2;
		rect.bottom=rect.top+750-119;
		rect.left=(mRect.right-mRect.left-AUDWDX-2)/2;
		rect.right=rect.left+AUDWDX+2;
		AfxGetMainWnd()->MoveWindow(rect,TRUE);
*/	}
	DispIro(pDC,&m_bm_herz,255);
}

void PutDot2AG()//Put red dot on audiograph
{
	int i,j;
	for(i=0;i<500;i++){
		if(dots[i].hz==0)break;//最後に追加
		if(dots[i].hz==TrueHz)break;//同じ周波数なら書き換える
	}
	if(dots[i].hz==0){
		for(i=0;i<500;i++){
			if(dots[i].hz==0)break;
			if(TrueHz<dots[i].hz)break;
		}//
		for(j=499;j>i;j--){
			dots[j].hz=dots[j-1].hz;
			dots[j].vol=dots[j-1].vol;
		}
	}
	dots[i].hz=TrueHz;
	dots[i].vol=TrueVol;
}

void CAudioMetryView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int x=point.x,y=point.y,truex;
	if(x>=AUDGX&&y>=AUDGY&&x<=AUDGX2&&y<=AUDGY2){
		lbdF=true;
		if((x-AUDGX)%TABW>(TABW/2))truex=(x-AUDGX)/TABW*TABW+TABW;
		else truex=(x-AUDGX)/TABW*TABW;
		TrueHz=X2Hz[truex];
		if(TrueHz<2000){
			truex=Hz2X(TrueHz);
			if(truex%128>64)truex=truex/128*128+128;
			else truex=truex/128*128;
			TrueHz=X2Hz[truex];
		}
		TrueVol=(y-AUDGY+10)/20*20;;
		CDC MemDC,*pDC;
		pDC=GetDC();
		DispIro(pDC,&m_bm_herz,HZVOL|DOTS);
		ReleaseDC(pDC);
	}
	CFormView::OnLButtonDown(nFlags, point);
}
void CAudioMetryView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	lbdF=false;
	lbdmF=false;
	CFormView::OnLButtonUp(nFlags, point);
}
void CAudioMetryView::OnMouseMove(UINT nFlags, CPoint point) 
{
	int x=point.x,y=point.y;
	if(x>=AUDGX&&y>=AUDGY&&x<=AUDGX2&&y<=AUDGY2){
		VirHz=X2Hz[x-AUDGX];//GetHz(x-AUDGX,&VirHz);
		VirVol=y-AUDGY;
		CDC *pDC;
		pDC=GetDC();
		if(lbdF==true){
			lbdmF=true;
			TrueHz=VirHz;
			TrueVol=VirVol;
			DispIro(pDC,&m_bm_herz,HZVOL|DOTS);
		}else DispIro(pDC,&m_bm_herz,HZVOL);
		ReleaseDC(pDC);
	}
	CFormView::OnMouseMove(nFlags, point);
}


void CAudioMetryView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	SetTimer(1,450,NULL);	
	CFormView::OnRButtonUp(nFlags, point);
}
void CAudioMetryView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	int i,x=point.x,y=point.y,dotx;
	if(x>=AUDGX&&y>=AUDGY&&x<=AUDGX2&&y<=AUDGY2){
		for(i=0;i<500;i++){
			if(dots[i].hz==0)break;
			dotx=Hz2X(dots[i].hz);
			if((x-AUDGX>dotx-3&&x-AUDGX<dotx+3)&&
				(y-AUDGY>dots[i].vol-3&&y-AUDGY<dots[i].vol+3))break;
		}
		if(dots[i].hz!=0){
			CDC MemDC,*pDC;
			pDC=GetDC();
			for(;dots[i].hz!=0;i++){
				dots[i].hz=dots[i+1].hz;
				dots[i].vol=dots[i+1].vol;
			}
			DispIro(pDC,&m_bm_herz,255);
			ReleaseDC(pDC);
		}
		for(i=0;i<500;i++){
			if(dots_d[i].hz==0)break;
			dotx=Hz2X(dots_d[i].hz);
			if((x-AUDGX>dotx-2&&x-AUDGX<dotx+2)&&
				(y-AUDGY>dots_d[i].vol-2&&y-AUDGY<dots_d[i].vol+2))break;
		}
		if(dots_d[i].hz!=0){
			CDC MemDC,*pDC;
			pDC=GetDC();
			for(i=0;i<500;i++){
				dots_d[i].hz=dots_d[i].vol=0;
			}
			DispIro(pDC,&m_bm_herz,255);
			ReleaseDC(pDC);
		}
	}
	KillTimer(1);
	CFormView::OnRButtonDown(nFlags, point);
}

void CAudioMetryView::OnSavedots() 
{
	CStdioFile ff;
	int i;
	char buf[100];
	char fn[300];

	CString         filter("聴力像 (*.audio)|*.audio||");
	CFileDialog     selDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, filter);
    
	if (selDlg.DoModal() == IDOK)
	{	
		strcpy(fn,selDlg.GetPathName());
		if(!strstr(fn,".audio"))strcat(fn,".audio");
		ff.Open(fn,CFile::modeCreate|
				CFile::modeWrite|CFile::modeRead|CFile::typeText);
		for(i=0;i<500;i++){//writeini
			if(dots[i].hz==0)break;
			sprintf(buf,"%05d,%05d\n",dots[i].hz,dots[i].vol);//s%s\n",ptext[i],ptxt[i]);
			ff.WriteString(buf);
		}
    }
}

void CAudioMetryView::OnReaddots() 
{
	CStdioFile ff;
	int i;
	char buf[101];
	char fn[300];

	CString         filter("聴力像 (*.audio)|*.audio||");
	CFileDialog     selDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
    
	if (selDlg.DoModal() == IDOK)
	{
		strcpy(fn,selDlg.GetPathName());
		ff.Open(fn,CFile::modeRead|CFile::typeText);
		i=0;
		while(ff.ReadString(buf,100)){
			sscanf(buf,"%d,%d\n",&dots[i].hz,&dots[i].vol);
			i++;
		}
		dots[i].hz=dots[i].vol=0;
		ff.Abort();
		CDC *pDC=GetDC();
		DispIro(pDC,&m_bm_herz,255);
		ReleaseDC(pDC);
    }
}

void CAudioMetryView::OnReaddotsonly() 
{
	CStdioFile ff;
	int i;
	char buf[101];
	char fn[300];

	CString         filter("聴力像 (*.audio)|*.audio||");
	CFileDialog     selDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
    
	if (selDlg.DoModal() == IDOK)
	{
		strcpy(fn,selDlg.GetPathName());
		ff.Open(fn,CFile::modeRead|CFile::typeText);
		i=0;
		while(ff.ReadString(buf,100)){
			sscanf(buf,"%d,%d\n",&dots_d[i].hz,&dots_d[i].vol);
			i++;
		}
		dots_d[i].hz=dots_d[i].vol=0;
		ff.Abort();
		CDC *pDC=GetDC();
		DispIro(pDC,&m_bm_herz,255);
		ReleaseDC(pDC);
    }
}
BOOL CAudioMetryView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{//MK_MBUTTON
	CDC *pDC=GetDC();
	if(zDelta<0){
		TrueVol+=20;
		TrueVol-=TrueVol%20;
	}else{
		if(TrueVol%20==0)TrueVol-=20;
		else TrueVol-=TrueVol%20;
	}
	if(TrueVol<0)TrueVol=0;
	else if(TrueVol>480)TrueVol=480;
	DispIro(pDC,&m_bm_herz,HZVOL|DOTS);
	ReleaseDC(pDC);
	return CFormView::OnMouseWheel(nFlags, zDelta, pt);
}

void CAudioMetryView::OnDestroy() 
{
	CFormView::OnDestroy();
	
	KillTimer(1);
	Sleep(500);
	SetWaveVolume(WVolume);
}

void CAudioMetryView::OnReturndot() 
{
	CDC *pDC=GetDC();
	PutDot2AG();
	DispIro(pDC,&m_bm_herz,255);
	ReleaseDC(pDC);
}
void CAudioMetryView::OnDeletedots() 
{
	int i,dotx;
	for(i=0;i<500;i++){
		if(dots[i].hz==0)break;
		dotx=Hz2X(dots[i].hz);
		if((Hz2X(TrueHz)>dotx-3&&Hz2X(TrueHz)<dotx+3)&&
			(TrueVol>dots[i].vol-3&&TrueVol<dots[i].vol+3))break;
	}
	if(dots[i].hz!=0){
		CDC MemDC,*pDC;
		pDC=GetDC();
		for(;dots[i].hz!=0;i++){
			dots[i].hz=dots[i+1].hz;
			dots[i].vol=dots[i+1].vol;
		}
		DispIro(pDC,&m_bm_herz,255);
		ReleaseDC(pDC);
	}
	for(i=0;i<500;i++){
		if(dots_d[i].hz==0)break;
		dotx=Hz2X(dots_d[i].hz);
		if((Hz2X(TrueHz)>dotx-2&&Hz2X(TrueHz)<dotx+2)&&
			(TrueVol>dots_d[i].vol-2&&TrueVol<dots_d[i].vol+2))break;
	}
	if(dots_d[i].hz!=0){
		CDC MemDC,*pDC;
		pDC=GetDC();
		for(i=0;i<500;i++){
			dots_d[i].hz=dots_d[i].vol=0;
		}
		DispIro(pDC,&m_bm_herz,255);
		ReleaseDC(pDC);
	}
}
void CAudioMetryView::OnDecrvol() 
{
//	TrueVol-=20;
	if(TrueVol%20==0)TrueVol-=20;
	else TrueVol-=TrueVol%20;
	if(TrueVol<0)TrueVol=0;
	CDC *pDC=GetDC();
	DispIro(pDC,&m_bm_herz,HZVOL|DOTS);
	ReleaseDC(pDC);
}

void CAudioMetryView::OnIncvol() 
{
	TrueVol+=20;
	TrueVol-=TrueVol%20;
	if(TrueVol>480)TrueVol=480;
	CDC *pDC=GetDC();
	DispIro(pDC,&m_bm_herz,HZVOL|DOTS);
	ReleaseDC(pDC);
}

void CAudioMetryView::OnLefthz() 
{
	OnHzdowntab();
	return;
	CDC *pDC=GetDC();
	TrueHz=GetNewHz(TrueHz,-1);
	DispIro(pDC,&m_bm_herz,HZVOL|DOTS);
	ReleaseDC(pDC);
}

void CAudioMetryView::OnRighthz() 
{
	OnHzuptab();
	return;
	CDC *pDC=GetDC();
	TrueHz=GetNewHz(TrueHz,1);
	DispIro(pDC,&m_bm_herz,HZVOL|DOTS);
	ReleaseDC(pDC);
}

void CAudioMetryView::OnHzdowntab() 
{
	int truex=Hz2X(TrueHz);
	truex=(truex+TABW-1)/TABW*TABW-TABW;
	if(truex<0)truex=0;
	if(TrueHz==125)TrueHz=MAXHZ;
	else TrueHz=X2Hz[truex];
	if(TrueHz<2000){
		truex=Hz2X(TrueHz)/128*128;
		TrueHz=X2Hz[truex];
	}
	CDC *pDC=GetDC();
	DispIro(pDC,&m_bm_herz,HZVOL|DOTS);
	ReleaseDC(pDC);
}

void CAudioMetryView::OnHzuptab() 
{
	int truex=Hz2X(TrueHz);
	truex=(truex+TABW)/TABW*TABW;
	if(TrueHz==MAXHZ)TrueHz=125;
	else if(X2Hz[truex]>=MAXHZ)TrueHz=MAXHZ;
	else TrueHz=X2Hz[truex];
	if(TrueHz<2000){
		truex=(Hz2X(TrueHz)+127)/128*128;
		TrueHz=X2Hz[truex];
	}
	CDC *pDC=GetDC();
	DispIro(pDC,&m_bm_herz,HZVOL|DOTS);
	ReleaseDC(pDC);
}

void CAudioMetryView::OnLefts() 
{
	SND_S=L;// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	SetWaveV(TrueVol);	
}

void CAudioMetryView::OnUpdateLefts(CCmdUI* pCmdUI) 
{
	if(SND_S==L)pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CAudioMetryView::OnRights() 
{
	SND_S=R;// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	SetWaveV(TrueVol);
}

void CAudioMetryView::OnUpdateRights(CCmdUI* pCmdUI) 
{
	if(SND_S==R)pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CAudioMetryView::OnBoths() 
{
	SND_S=B;// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	SetWaveV(TrueVol);
}

void CAudioMetryView::OnUpdateBoths(CCmdUI* pCmdUI) 
{
	if(SND_S==B)pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CAudioMetryView::OnHzdown() 
{
	CDC *pDC=GetDC();
	TrueHz=GetNewHz(TrueHz,-1);
	DispIro(pDC,&m_bm_herz,HZVOL|DOTS);
	ReleaseDC(pDC);
}

void CAudioMetryView::OnHzup() 
{
	CDC *pDC=GetDC();
	TrueHz=GetNewHz(TrueHz,1);
	DispIro(pDC,&m_bm_herz,HZVOL|DOTS);
	ReleaseDC(pDC);
}

void CAudioMetryView::OnSetpos() 
{
	CDC MemDC,*pDC;
	pDC=GetDC();
	PutDot2AG();
	DispIro(pDC,&m_bm_herz,255);
	ReleaseDC(pDC);
}
