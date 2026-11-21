#include <iterator>
#include <cmath>
#include "pch.h"
#include "SpaceTorpedoGame.h"
#include "SpaceTorpedoGameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

inline int MyRound(double v) { return int(round(v)); }
inline double MyRandom() { return (rand() / double(0x8000)); }

const int    PXOrg     =      400;  // 物理座標系の原点の画面の座標系でのＸ座標
const int    PYOrg     =      200;  // 物理座標系の原点の画面の座標系でのＹ座標
const int    TimeUnit  =      100;  // 時間単位(ｍ秒)
const double ZSight    =  5000000;  // Ｚ方向の検知可能距離
const double ZUnit     =    10000;  // 見掛けの大きさを決める基準距離
const double EShipRad  =      200;  // 敵宇宙船の半径
const double ZVel      =    10000;  // 敵宇宙船の速度のＺ成分
const double PerpAcc   =       20;  // 敵宇宙船の進む方向の直角方向の加速度
const double TrnDcsVel =       30;  // 敵宇宙船の進む方向の直角方向の折返し決断速度
const double ShtDcsDst =   200000;  // 敵宇宙船が魚雷を発射する距離
const double AsideDist =   100000;  // 敵宇宙船がよける距離
const double SlfPrpVel =      100;  // 自分の進む方向の直角方向の最大速度
const double EFlashRad =     2000;  // 爆発時の閃光の半径
const double TorpdRad  =       40;  // 魚雷の半径
const double ZTorpdVel =    10000;  // 魚雷の速度のＺ成分
const double YTorpdAcc =       10;  // 魚雷の発射直後補正用加速度のＹ成分
const double TrpAccTim =        5;  // 魚雷の発射直後補正用加速時間
const int    TrpRldTim =       20;  // 魚雷の装填時間
const int    TrpRcvTim =       50;  // 魚雷の復旧時間
const int    DmgDwnTim =        5;  // ダメージが収まるまでの時間
const int    ShieldCol = 0xFF8000;  // シールドの色
const double Pi        = 3.14159265;  // 円周率

CSpaceTorpedoGameDlg::CSpaceTorpedoGameDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SPACETORPEDOGAME_DIALOG, pParent) {
}

BEGIN_MESSAGE_MAP(CSpaceTorpedoGameDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

BOOL CSpaceTorpedoGameDlg::OnInitDialog() {

	CDialog::OnInitDialog();

	MoveWindow(0, 0, (PXOrg * 2 + 16), (PYOrg * 2 + 39));

	pDC = new CClientDC(this);

	srand(static_cast<unsigned int>(time(nullptr)));

	for (int idx = 0; idx < size(starTbl); idx++) {
		starTbl[idx].xL = int(round(PXOrg * 2 * (MyRandom())));
		starTbl[idx].yL = int(round(PYOrg * 2 * (MyRandom())));
		int r = int(MyRandom()) * 0x40 + 0xC0;
		int g = int(MyRandom()) * 0x40 + 0xC0;
		int b = int(MyRandom()) * 0x40 + 0xC0;
		double k = MyRandom() * 0.5;
		starTbl[idx].col    = RGB(r    , g    , b    );
		starTbl[idx].rndCol = RGB(r * k, g * k, b * k);
	}
	imgEnemyEShip.Load(_T("Klingon_K't'inga.png"));
	for (int idx = 0; idx < size(enemyShipTbl); idx++) {
		double z = ZSight * (1 - double(idx) / size(enemyShipTbl));
		InitEnemyShip(enemyShipTbl[idx], z);
	}
	enemyTorpd.z = - (ZVel + ZTorpdVel);
	torpedo.z = - ZTorpdVel;
	torpedoRemainTim = 0;
	ySelfVel = xSelfVel = 0;
	selfDmgRemainTim = 0;
	score = 0;

	const LPCTSTR Msg = _T(
		"[ このゲームの遊び方 ]\n\n"
		"敵宇宙船を光子魚雷で撃沈する。\n\n"
		"マウスの左ボタンを押すと光子魚雷が発射される。\n\n"
		"ＷＡＳＤキーを押すと、居る位置が上左下右にずれる。\n\n"
		"敵の光子魚雷が当たると光子魚雷発射可能までの時間が延びる。\n\n"
		"ＥＳＣキーを押すとゲームの途中であっても終了する。");
	AfxMessageBox(Msg);

	SetTimer(1, TimeUnit, NULL);

	return false;
}

void CSpaceTorpedoGameDlg::OnClose() {

	KillTimer(1);
	delete pDC;
	CDialogEx::OnClose();
}

BOOL CSpaceTorpedoGameDlg::PreTranslateMessage(MSG* pMsg) {

	if (pMsg->message != WM_KEYDOWN) return false;

	if ((GetKeyState(VK_CONTROL) & 0x8000) != 0 &&
		(GetKeyState(VK_SHIFT  ) & 0x8000) != 0 &&
		pMsg->wParam == VK_SPACE                  ) {
		if (! KillTimer(1))
			SetTimer(1, TimeUnit, NULL);
	} else if (pMsg->wParam == VK_ESCAPE) {
		PostMessage(WM_CLOSE);
	}
	return false;
}

void CSpaceTorpedoGameDlg::OnLButtonDown(UINT nFlags, CPoint point) {

	if (torpedoRemainTim > 0) return;

	torpedo.xVel = (point.x - PXOrg) * (ZTorpdVel / ZUnit);
	torpedo.yVel = (point.y - PYOrg) * (ZTorpdVel / ZUnit);
	torpedo.x = 0;
	torpedo.y = YTorpdAcc * (TrpAccTim * (TrpAccTim + 1) / 2);
	torpedo.z = 0;
	
	torpedoRemainTim = TrpRldTim;
}

void CSpaceTorpedoGameDlg::OnPaint() {

	CDialog::OnPaint();
	DrawAll();
}

void CSpaceTorpedoGameDlg::OnTimer(UINT_PTR nIDEvent) {

	controlSelfPerpVel();
	TTorpedo prevEnemyTorpd = enemyTorpd;
	if (enemyTorpd.z < - ZTorpdVel) {
		enemyTorpd.z = - (ZVel + ZTorpdVel);
	} else {
		enemyTorpd.x += enemyTorpd.xVel - xSelfVel;
		enemyTorpd.y += enemyTorpd.yVel - ySelfVel;
		enemyTorpd.z -= ZVel + ZTorpdVel;
	}
	static TPos self = {0, 0, 0};
	if (prevEnemyTorpd.z > 0 && enemyTorpd.z <= 0 &&
		IsHit(self, prevEnemyTorpd, self, enemyTorpd)) {
		selfDmgRemainTim = DmgDwnTim;
		torpedoRemainTim = TrpRcvTim;
	}
	if (selfDmgRemainTim > 0)
		selfDmgRemainTim--;
	TTorpedo prevTorpd = torpedo;
	if (torpedo.z >= 0) {
		torpedo.z += ZTorpdVel;
		torpedo.x += torpedo.xVel - xSelfVel;
		torpedo.y += torpedo.yVel - ySelfVel;
		int n = int(round(torpedo.z / ZTorpdVel));
		if (n <= TrpAccTim)
			torpedo.y -= YTorpdAcc * (TrpAccTim + 1 - n);
	}
	if (torpedoRemainTim > 0) {
		torpedoRemainTim--;
		if (torpedoRemainTim <= 0)
			torpedo.z = - ZTorpdVel;
	}
	for (int idx = 0; idx < size(enemyShipTbl); idx++) {
		TEnemyShip* pEs = &enemyShipTbl[idx];
		TEnemyShip prevEs = *pEs;
		double x = pEs->x = pEs->x + enemyShipTbl[idx].xVel - xSelfVel;
		double y = pEs->y = pEs->y + enemyShipTbl[idx].yVel - ySelfVel;
		double z = pEs->z = pEs->z - ZVel;
		if (z > AsideDist) {
			double zRate = ZVel / z;
			double turnBrdr = TrnDcsVel / PerpAcc;
			if ((pEs->xVel + x * zRate * 1.5) / pEs->xAcc > turnBrdr) {
				if (MyRandom() < 0.5)
					pEs->xAcc = - (pEs->xAcc);
			}
			if ((pEs->yVel + y * zRate * 1.5) / pEs->yAcc > turnBrdr) {
				if (MyRandom() < 0.5)
					pEs->yAcc = - (pEs->yAcc);
			}
			if (pEs->cllpDeg <= 0 && enemyTorpd.z < - ZTorpdVel && z <= ShtDcsDst) {
				enemyTorpd.x = pEs->x;
				enemyTorpd.y = pEs->y;
				enemyTorpd.z = pEs->z;
				enemyTorpd.xVel = - pEs->x * ((ZVel + ZTorpdVel) / pEs->z);
				enemyTorpd.yVel = - pEs->y * ((ZVel + ZTorpdVel) / pEs->z);
			}
		}
		pEs->xVel += pEs->xAcc;
		pEs->yVel += pEs->yAcc;
		if (pEs->cllpDeg > 0) {
			pEs->cllpDeg += 0x20;
		} else if (torpedo.z >= 0) {
			if (prevTorpd.z < prevEs.z && torpedo.z >= pEs->z &&
				IsHit(prevEs, prevTorpd, *pEs, torpedo)         ) {
				torpedo.z = - ZTorpdVel;
				pEs->cllpDeg = 0x80;
				score += 100;
			}
		}
		if (pEs->z < ZUnit) {
			for (int j = idx; j > 0; j--)
				enemyShipTbl[j] = enemyShipTbl[j - 1];
			InitEnemyShip(enemyShipTbl[0], ZSight);
		}
	}
	DrawAll();

	wchar_t title[256];
	swprintf(title, size(title),
			 L"    得点 %u      光子魚雷発射可能まで %4dｍ秒",
			 score, torpedoRemainTim * 100                    );
	SetWindowText(title);
}

// 自分の進む方向の直角方向の速度を制御する
void CSpaceTorpedoGameDlg::controlSelfPerpVel() {

	bool isLeft  = (GetAsyncKeyState('A') < 0);
	bool isRight = (GetAsyncKeyState('D') < 0);
	if (isLeft && isRight) {
	} else if (isLeft) {
		xSelfVel = max((xSelfVel - PerpAcc), (- SlfPrpVel));
	} else if (isRight) {
		xSelfVel = min((xSelfVel + PerpAcc),    SlfPrpVel );
	} else if (xSelfVel < - PerpAcc) {
		xSelfVel += PerpAcc;
	} else if (xSelfVel > PerpAcc) {
		xSelfVel -= PerpAcc;
	} else {
		xSelfVel = 0;
	}
	bool isUp   = (GetAsyncKeyState('W') < 0);
	bool isDown = (GetAsyncKeyState('S') < 0);
	if (isUp && isDown) {
	} else if (isUp) {
		ySelfVel = max((ySelfVel - PerpAcc), (- SlfPrpVel));
	} else if (isDown) {
		ySelfVel = min((ySelfVel + PerpAcc),    SlfPrpVel );
	} else if (ySelfVel < - PerpAcc) {
		ySelfVel += PerpAcc;
	} else if (ySelfVel > PerpAcc) {
		ySelfVel -= PerpAcc;
	} else {
		ySelfVel = 0;
	}
}

// 敵宇宙船に当たったか判定する
bool CSpaceTorpedoGameDlg::IsHit(TPos& oldOrg, TPos& oldSbj, TPos& newOrg, TPos& newSbj) {

	double x = oldSbj.x - oldOrg.x;
	double y = oldSbj.y - oldOrg.y;
	double z = oldSbj.z - oldOrg.z;
	double a = newSbj.x - newOrg.x - x;
	double b = newSbj.y - newOrg.y - y;
	double c = newSbj.z - newOrg.z - z;
	double innPrd = a * x + b * y + c * z;
	double distSqr = x * x + y * y + z * z - innPrd * innPrd / (a * a + b * b + c * c);
	const double tchDist = EShipRad + TorpdRad;
	return (distSqr < tchDist * tchDist);
}

// 敵宇宙船を初期化する
void CSpaceTorpedoGameDlg::InitEnemyShip(TEnemyShip& eS, double z) {

	eS.x    = PXOrg * (z / ZUnit) * (MyRandom() * 2 - 1);
	eS.y    = PYOrg * (z / ZUnit) * (MyRandom() * 2 - 1);
	eS.z    = z;
	eS.xAcc = PerpAcc * (MyRandom() * 0.5 + 0.5);
	eS.yAcc = PerpAcc * (MyRandom() * 0.5 + 0.5);
	eS.xVel = TrnDcsVel * (MyRandom() * 2 - 1) - eS.x * (ZVel / z);
	eS.yVel = TrnDcsVel * (MyRandom() * 2 - 1) - eS.y * (ZVel / z);

	eS.xVel = - eS.x * (ZVel / z);
	eS.yVel = - eS.y * (ZVel / z);

	int pat = int(MyRandom() * (1 << 21));
	eS.col = RGB((((pat >> 14) & 0x7F) + 0x80),
				 (((pat >>  7) & 0x7F) + 0x80),
				 (( pat        & 0x7F) + 0x80) );

	eS.cllpDeg = 0;
}

// すべてを描く
void CSpaceTorpedoGameDlg::DrawAll() {

	if (selfDmgRemainTim > 0) {
		pDC->FillSolidRect(0, 0, (PXOrg * 2 + 1), (PYOrg * 2 + 1), ShieldCol);
		return;
	}

	pDC->FillSolidRect(0, 0, (PXOrg * 2 + 1), (PYOrg * 2 + 1), 0);
	for (int idx = 0; idx < size(starTbl); idx++) {
		int xL = starTbl[idx].xL;
		int yL = starTbl[idx].yL;
		COLORREF col    = starTbl[idx].col;
		COLORREF rndCol = starTbl[idx].rndCol;
		pDC->FillSolidRect((xL - 1),  yL     , 3, 1, rndCol);
		pDC->FillSolidRect( xL     , (yL - 1), 1, 3, rndCol);
		pDC->FillSolidRect( xL     ,  yL     , 1, 1, col   );
	}
	int idx;
	for (idx = 0; idx < size(enemyShipTbl); idx++) {
		TEnemyShip* pEs = &enemyShipTbl[idx];
		if (pEs->z < torpedo.z) break;
		DrawEnemyShip(*pEs);
	}
	if (enemyTorpd.z >= 0)
		DrawTorpedo(enemyTorpd);
	if (torpedo.z >= 0)
		DrawTorpedo(torpedo);
	for (; idx < size(enemyShipTbl); idx++)
		DrawEnemyShip(enemyShipTbl[idx]);
}

// 敵宇宙船を描く
void CSpaceTorpedoGameDlg::DrawEnemyShip(TEnemyShip& eS) {

	if (eS.cllpDeg >= 0x100) return;

	int xL = int(round((eS.x / eS.z) * ZUnit)) + PXOrg;
	int yL = int(round((eS.y / eS.z) * ZUnit)) + PYOrg;

	int rL = int(round((EShipRad * 1.5 / eS.z) * ZUnit));
	int size = rL * 2 + 1;
	imgEnemyEShip.Draw(pDC->GetSafeHdc(), (xL - rL), (yL - rL), size, size);

	if (eS.cllpDeg <= 0) return;

	rL = int(round((EFlashRad / eS.z) * ZUnit * eS.cllpDeg / 0x100));
	pDC->SelectStockObject(NULL_BRUSH);
	for (int r = rL; r > 0; r--) {
		int brg = int((rL + 1 - r) * 0xFF / rL);
		COLORREF col = RGB(brg, brg, brg);
		CPen pen(PS_SOLID, 2, col);
		pDC->SelectObject(&pen);
		pDC->Ellipse((xL - r), (yL - r), (xL + r), (yL + r));
	}
}

// 魚雷を描く
void CSpaceTorpedoGameDlg::DrawTorpedo(TTorpedo& torpd) {

	int xL = int(round((torpd.x / torpd.z) * ZUnit)) + PXOrg;
	int yL = int(round((torpd.y / torpd.z) * ZUnit)) + PYOrg;
	int rL = int(round((TorpdRad / torpd.z) * ZUnit));

	pDC->SelectStockObject(NULL_BRUSH);
	for (int r = rL; r > 0; r--) {
		int brg = int((rL + 1 - r) * 0xFF / rL);
		COLORREF col = RGB(brg, (brg >> 1), 0);
		CPen pen(PS_SOLID, 2, col);
		pDC->SelectObject(&pen);
		pDC->Ellipse((xL - r), (yL - r), (xL + r), (yL + r));
	}
}

