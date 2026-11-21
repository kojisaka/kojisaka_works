#include <iterator>
#include <cmath>
#include "pch.h"
#include "KatamachiLineGame.h"
#include "KatamachiLineGameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

inline int MyRound(double v) { return int(round(v)); }
inline double MyRandom() { return (rand() / double(0x8000)); }

const double ProbObstS =     0.60;  // 分岐器間に障害物がいる標準確率
const int    PXOrg     =      400;  // 物理座標系の原点の画面の座標系でのＸ座標
const int    PYOrg     =      200;  // 物理座標系の原点の画面の座標系でのＹ座標
const int    TimeUnit  =      100;  // 時間単位(ｍ秒)
const int    ZSpeedStd =     2000;  // 標準速度
const int    ZAccelStd =       25;  // 標準加速度
const int    ZSpeedApr =      800;  // ホームへの進入速度
const int    ZAccelApr =       10;  // ホーム内での減速度
const int    YGround   =     2000;  // 地面のＹ座標
const int    ZUnit     =      200;  // Ｚ方向の単位距離
const int    ZSight    =   200000;  // Ｚ方向の目視可能距離
const int    XTrcDiff  =     4000;  // 隣の線路とのＸ方向の距離
const int    ZStart    =    -5000;  // 本路線の開始Ｚ座標
const int    ZEnd      =  2160000;  // 本路線の終了Ｚ座標
const int    ZGoal     =  2000000;  // ゴールのＺ座標
const int    ZSwitTerm =    40000;  // 分岐器のＺ方向の挿入周期
const int    ZFormSize =    40000;  // ホームのＺ方向の大きさ
const int    XFormIn   =     5500;  // ホームの内側のＸ座標
const int    XFormSize =     4000;  // ホームのＸ方向の大きさ
const int    YFormSize =     1000;  // ホームのＹ方向の大きさ
const int    ZPoleTerm =    20000;  // 電柱のＺ方向の設置周期
const int    XPoleDiff =     7500;  // バラスト領域の中心から電柱のＸ方向の距離
const int    XPoleSize =      200;  // 電柱のＸ方向の大きさ
const int    YPoleSize =     5000;  // 電柱のＹ方向の大きさ
const int    YElecLine =    -2800;  // 電線のＹ座標
const int    XBlstSize =    12000;  // バラスト領域のＸ方向の大きさ
const int    ZTieTerm  =      600;  // 枕木のＺ方向の挿入周期
const int    ZTieSize  =      200;  // 枕木のＺ方向の大きさ
const int    XTieSize  =     1800;  // 枕木のＸ方向の大きさ
const int    XRailIn   =      500;  // 線路の中心からレールの内側までのＸ方向の距離
const int    XRailSize =       80;  // レールのＸ方向の大きさ（幅）
const int    XJointGap =      160;  // 分岐器の接合部分のレールの隙間のＸ方向の大きさ
const int    XCrossGap =       30;  // クロッシングのレールの隙間のＸ方向の大きさ
const int    XObstSize =     2400;  // 障害物のＸ方向の大きさ
const int    YObstSize =     1000;  // 障害物のＹ方向の大きさ
const int    XBallSize =      200;  // 風船のＸ方向の大きさ
const int    YBallSize =      300;  // 風船のＹ方向の大きさ
const int    ColorSky  = 0xFF9900;  // 空の色
const int    ColorFrmS = 0x999999;  // ホームの側面の色
const int    ColorFrmF = 0xAAAAAA;  // ホームの前面の色
const int    ColorFrmT = 0xBBBBBB;  // ホームの上面の色
const int    ColorPole = 0xCCCCCC;  // 電柱の色
const int    ColorLine = 0x333333;  // 電線の色
const int    ColorOut  = 0x009900;  // 用地の外側の色
const int    ColorBlst = 0xAAAAAA;  // バラスト領域の色
const int    ColorTie  = 0x001199;  // 枕木の色
const int    ColorRail = 0x000011;  // レールの色
const int    ColorObst = 0x0033CC;  // 障害物の色
const int    ColorEye  = 0x111111;  // 障害物の目の色
const int    ColorStri = 0x333333;  // 紐の色
const int    ColorCrac = 0x000000;  // ひび割れの色
const double Pi        = 3.14159265;  // 円周率

const int ZStaSwit1S = (ZEnd - ZFormSize) - MyRound(ZSwitTerm * 1.7);
const int ZStaSwit1E = (ZEnd - ZFormSize) - MyRound(ZSwitTerm * 0.9);
const int ZStaSwit2S = (ZEnd - ZFormSize) - MyRound(ZSwitTerm * 0.9);
const int ZStaSwit2E = (ZEnd - ZFormSize) - MyRound(ZSwitTerm * 0.1);
const int ZStaSwit3S = MyRound(ZSwitTerm * 0.1);
const int ZStaSwit3E = MyRound(ZSwitTerm * 0.9);
const TTrack BasicTrackTbl[] = {
	{ZStart    , ZEnd      , (- XTrcDiff), (- XTrcDiff), false},
	{ZStart    , ZEnd      ,    0        ,    0        , false},
	{ZStart    , ZEnd      ,    XTrcDiff ,    XTrcDiff , false},
	{ZStaSwit1S, ZStaSwit1E, (- XTrcDiff),    0        , true },
	{ZStaSwit2S, ZStaSwit2E,    0        ,    XTrcDiff , true },
	{ZStaSwit3S, ZStaSwit3E,    XTrcDiff ,    0        , true }
};

CKatamachiLineGameDlg::CKatamachiLineGameDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KATAMACHILINEGAME_DIALOG, pParent) {
}

BEGIN_MESSAGE_MAP(CKatamachiLineGameDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CKatamachiLineGameDlg::OnInitDialog() {
	const LPCTSTR Msg = _T(
		"[ このゲームの遊び方 ]\n\n"
		"３つの線路の間を渡り線を使って転線しながら、\n"
		"イノシシにぶつからずどこまで行けるかを競う。\n\n"
		"左矢印キーと右矢印キーを押すと、次に通るポイントが切り替わる。\n\n"
		"ＥＳＣキーを押すとゲームの途中であっても終了する。");
	int dX;  // 渡り線のＸ方向の移動量
	int z;   // 線路のＺ座標

	CDialog::OnInitDialog();

	MoveWindow(0, 0, (PXOrg * 2 + 16), (PYOrg * 2 + 39));

	pDC = new CClientDC(this);

	srand(static_cast<unsigned int>(time(nullptr)));

	probObst = ProbObstS;

	trackNoE = -1;
	while (trackNoE < int(size(BasicTrackTbl)) - 1) {
		trackNoE++;
		trackTbl[trackNoE] = BasicTrackTbl[trackNoE];
	}
	z = ZSwitTerm;
	while (z < ZGoal) {
		if (MyRandom() < 0.5)
			dX = - XTrcDiff;
		else
			dX =   XTrcDiff;
		if (trackNoE >= int(size(trackTbl)) - 1)
			break;
		trackNoE++;
		trackTbl[trackNoE].zS = z + MyRound(ZSwitTerm * 0.1);
		trackTbl[trackNoE].zE = z + MyRound(ZSwitTerm * 0.9);
		trackTbl[trackNoE].xS = 0;
		trackTbl[trackNoE].xE = dX;
		if (trackNoE >= int(size(trackTbl)) - 1)
			break;
		trackNoE++;
		if (MyRandom() < 0.5)
			dX = - XTrcDiff;
		else
			dX =   XTrcDiff;
		trackTbl[trackNoE].zS = z + MyRound(ZSwitTerm * 0.1);
		trackTbl[trackNoE].zE = z + MyRound(ZSwitTerm * 0.9);
		trackTbl[trackNoE].xS = dX;
		trackTbl[trackNoE].xE = 0;
		z += ZSwitTerm;
	}

	obstacleNoE = -1;
	z = ZSwitTerm * 2;
	while (z < ZGoal) {
		if (MyRandom() < probObst) {
			if (obstacleNoE >= int(size(obstacleTbl)) - 1)
				break;
			obstacleNoE++;
			obstacleTbl[obstacleNoE].z = z;
			obstacleTbl[obstacleNoE].trackNo = int(3 * MyRandom());
		}
		z += ZSwitTerm;
	}

	balloonNoE = -1;

	trackNoNow = 2;
	trackTbl[trackNoNow + 2].isConn = false;
	zSpeed = 0;
	zNow = ZStart;
	xNow = CalcTrackXPos(zNow, trackNoNow);

	AfxMessageBox(Msg);

	SetTimer(1, TimeUnit, NULL);

	return false;
}

void CKatamachiLineGameDlg::OnClose() {
	KillTimer(1);
	CDialogEx::OnClose();
}

BOOL CKatamachiLineGameDlg::PreTranslateMessage(MSG* pMsg) {
	int tNoZSMin;  // 開始点のＺ座標が最小である線路の番号
	int zSMin;     // 線路の開始点のＺ座標の最小値
	int tNo;       // 線路の番号
	int zS;        // 線路の開始点のＺ座標

	if (pMsg->message != WM_KEYDOWN) return false;

	if ((GetKeyState(VK_CONTROL) & 0x8000) != 0 &&
		(GetKeyState(VK_SHIFT  ) & 0x8000) != 0 &&
		pMsg->wParam == VK_SPACE                  ) {
		if (! KillTimer(1))
			SetTimer(1, TimeUnit, NULL);
	} else if (pMsg->wParam == VK_ESCAPE)
		PostMessage(WM_CLOSE);
	else if ((pMsg->wParam == VK_LEFT) || (pMsg->wParam == VK_RIGHT)) {
		tNoZSMin = trackNoE + 1;
		zSMin = INT_MAX;
		for (tNo = 0; tNo <= trackNoE; tNo++)
			if (tNo != trackNoNow) {
				zS = trackTbl[tNo].zS;
				if ((zNow < zS) && (zS < ZGoal))
					if (trackTbl[tNo].xS == CalcTrackXPos(zS, trackNoNow))
						if (zS < zSMin) {
							tNoZSMin = tNo;
							zSMin = zS;
						}
			}
		if (tNoZSMin <= trackNoE)
			trackTbl[tNoZSMin].isConn
			= ((trackTbl[tNoZSMin].xE < trackTbl[tNoZSMin].xS) == (pMsg->wParam == VK_LEFT));
	}
	return false;
}

void CKatamachiLineGameDlg::OnPaint() {
	CDialog::OnPaint();
	DrawAll(zNow, xNow);
}

void CKatamachiLineGameDlg::OnTimer(UINT_PTR nIDEvent) {
	int zOld;  // 直前の視点のＺ座標
	int xOld;  // 直前の視点のＸ座標
	int zS;    // 線路の開始点のＺ座標
	int zE;    // 線路の終了点のＺ座標
	int tNo;   // 線路の番号
	int oNo;   // 障害物の番号
	int z;     // 障害物のいるＺ座標
	int bNo;   // 風船の番号
	int idx;   // ループの指標
	wchar_t title[256];

	zOld = zNow;
	xOld = xNow;
	zNow += zSpeed;

	zE = trackTbl[trackNoNow].zE;
	if (zNow >= zE) {
		for (tNo = 0; tNo <= trackNoE; tNo++)
			if (tNo != trackNoNow)
				if (IsInTrack(zE, tNo))
					if (CalcTrackXPos(zE, tNo) == trackTbl[trackNoNow].xE)
						break;
		trackNoNow = tNo;
		trackTbl[3].isConn = true;
		trackTbl[4].isConn = true;
		if ((1 <= trackNoNow) || (trackNoNow <= 2))
			trackTbl[trackNoNow + 2].isConn = false;
	}

	for (tNo = 0; tNo <= trackNoE; tNo++)
		if (tNo != trackNoNow) {
			zS = trackTbl[tNo].zS;
			if ((zOld <= zS) && (zS < zNow))
				if (trackTbl[tNo].xS == CalcTrackXPos(zS, trackNoNow))
					if (trackTbl[tNo].isConn) {
						trackNoNow = tNo;
						break;
					}
		}

	xNow = CalcTrackXPos(zNow, trackNoNow);

	DrawAll(zOld, xOld);

	swprintf(title, size(title), L"速度 : 時速 % 3d km　　走破距離 : % 6d m", (zSpeed / 40), (zNow / 1000));
	SetWindowText(title);

	for (oNo = 0; oNo <= obstacleNoE; oNo++) {
		z = obstacleTbl[oNo].z;
		if ((zNow - zSpeed * 2 <= z) && (z < zNow) &&
			(obstacleTbl[oNo].trackNo == trackNoNow) )
			break;
	}
	if ((zNow >= ZEnd) || (oNo <= obstacleNoE)) {
		DrawWindowCrack();
		zSpeed = 0;
		KillTimer(1);
	} else if (zNow >= ZGoal) {
		if (zSpeed > ZSpeedApr)
			zSpeed -= ZAccelStd;
		else if (zNow > ZEnd - ZSpeedApr * (ZSpeedApr / double(ZAccelApr) + 1) / 2.0 - 5000)
			zSpeed -= ZAccelApr;
		if (zSpeed <= 0)
			zSpeed = 0;
		for (bNo = 0; bNo <= balloonNoE; bNo++)
			balloonTbl[bNo].y -= 100;
		GenerateBalloon();
		if (zNow >= ZEnd - ZFormSize * 2)
			for (idx = 1; idx <= 2; idx++)
				GenerateBalloon();
	} else if (zSpeed < ZSpeedStd)
		zSpeed += ZAccelStd;
}

// 風船を生成する
void CKatamachiLineGameDlg::GenerateBalloon() {
	const COLORREF ColorTbl[]
		= { RGB(255,0,0), RGB(0,0,255), RGB(255,255,0), RGB(255,0,255) };

	int z;    // 風船のいるＺ座標
	int bNo;  // 風船の番号

	if (balloonNoE >= int(size(balloonTbl)) - 1)
		return;

	balloonNoE++;

	z = ZEnd - ZFormSize + int((ZFormSize + 10000) * MyRandom());
	bNo = balloonNoE;
	while ((bNo >= 1) && (balloonTbl[bNo - 1].z < z)) {
		balloonTbl[bNo] = balloonTbl[bNo - 1];
		bNo--;
	}
	balloonTbl[bNo].z = z;
	balloonTbl[bNo].x = int((XFormIn + XFormSize) * MyRandom());
	balloonTbl[bNo].y = YGround - YFormSize;
	balloonTbl[bNo].color = ColorTbl[int(4 * MyRandom())];
}

// すべてを描く
void CKatamachiLineGameDlg::DrawAll(int zOld, int xOld) {
	int pYDraw;  // 描画位置の物理Ｙ座標
	int zDraw;   // 描画位置のＺ座標
	int tNo;     // 線路の番号
	int oNo;     // 障害物の番号
	int bNo;     // 風船の番号

	if (xNow == xOld)
		xDirNow = 0;
	else
		xDirNow = MyRound(ZUnit * ((xNow - xOld) / double(zNow - zOld)));

	for (pYDraw = (- PYOrg); pYDraw <= 0; pYDraw++) {
		DrawHorizontalLine(pYDraw, (- PXOrg), PXOrg, ColorSky);
		DrawElectricPole(pYDraw);
		DrawElectricLine(pYDraw);
		for (bNo = 0; bNo <= balloonNoE; bNo++)
			DrawBalloon(pYDraw, bNo);
	}
	for (pYDraw = 1; pYDraw <= PYOrg; pYDraw++) {
		DrawHorizontalLine(pYDraw, (- PXOrg), PXOrg, ColorOut);
		zDraw = zNow + MyRound(ZUnit * (YGround / double(pYDraw)));
		DrawBallastArea(pYDraw, zDraw);
		for (tNo = 0; tNo <= trackNoE; tNo++)
			DrawTie(pYDraw, zDraw, tNo);
		for (tNo = 0; tNo <= trackNoE; tNo++) {
			DrawRail(pYDraw, zDraw, tNo, (-1));
			DrawRail(pYDraw, zDraw, tNo,   1 );
		}
		DrawPlatform(pYDraw);
		DrawElectricPole(pYDraw);
		for (oNo = 0; oNo <= obstacleNoE; oNo++)
			DrawObstacle(pYDraw, oNo);
		for (bNo = 0; bNo <= balloonNoE; bNo++)
			DrawBalloon(pYDraw, bNo);
	}
}

// バラスト領域を描く
void CKatamachiLineGameDlg::DrawBallastArea(int pYDraw, int zDraw) {
	double zRate;    // 単位距離に対する描画位置の距離のＺ方向の比率
	int    pXLeft;   // バラスト領域の左端の物理Ｘ座標
	int    pXRight;  // バラスト領域の右端の物理Ｘ座標

	if (zDraw > ZEnd)
		return;

	zRate = ZUnit / double(zDraw - zNow);

	pXLeft  = MyRound((- XBlstSize / 2.0 - xNow) * zRate - xDirNow);
	pXRight = MyRound((  XBlstSize / 2.0 - xNow) * zRate - xDirNow);
	DrawHorizontalLine(pYDraw, pXLeft, pXRight, ColorBlst);
}

// 枕木を描く
void CKatamachiLineGameDlg::DrawTie(int pYDraw, int zDraw, int trackNo) {
	int    x;        // 枕木の中央のＸ座標
	double zRate;    // 単位距離に対する描画位置の距離のＺ方向の比率
	int    pXLeft;   // 枕木の左端の物理Ｘ座標
	int    pXRight;  // 枕木の右端の物理Ｘ座標

	if ((! IsTrackInSight(trackNo)) || (! IsInTrack(zDraw, trackNo)))
		return;
	if (((zDraw + ZTieTerm * 100) % ZTieTerm) > ZTieSize)
		return;

	zRate = ZUnit / double(zDraw - zNow);

	x = CalcTrackXPos(zDraw, trackNo);
	pXLeft  = MyRound((x - XTieSize / 2.0 - xNow) * zRate - xDirNow);
	pXRight = MyRound((x + XTieSize / 2.0 - xNow) * zRate - xDirNow);
	DrawHorizontalLine(pYDraw, pXLeft, pXRight, ColorTie);
}

// レールを描く
void CKatamachiLineGameDlg::DrawRail(int pYDraw, int zDraw, int trackNo, int sign) {
	int    x;         // 線路の中央のＸ座標
	double zRate;     // 単位距離に対する描画位置の距離のＺ方向の比率
	bool   IsLeft;    // 左へ分岐している状態ならtrue
	int    xBra;      // 分岐している線路の中央のＸ座標
	int    zEdge;     // 線路の端のＺ座標
	int    dZJoint;   // 線路の接続部分のＺ方向の長さ
	int    xIn;       // レールの内側のＸ座標
	int    xOut;      // レールの外側のＸ座標
	int    xInBra;    // 分岐しているレールの内側のＸ座標
	int    dX;        // 位置補正量のＸ成分
	int    xCross;    // 交差している線路の中央のＸ座標
	int    xDist;     // 交差している線路とのＸ方向の距離
	int    xOutCut;   // レールの外側の削除位置のＸ座標
	int    xSupport;  // クロッシングで車輪を支えるレールの支える側の側面のＸ座標
	int    xProtect;  // 脱線防止用レールの保護用側面のＸ座標
	int    tNo;       // 線路の番号
	int    pXIn;      // レールの内側の物理Ｘ座標
	int    pXOut;     // レールの外側の物理Ｘ座標

	if ((! IsTrackInSight(trackNo)) || (! IsInTrack(zDraw, trackNo)))
		return;

	x = CalcTrackXPos(zDraw, trackNo);
	zRate = ZUnit / double(zDraw - zNow);

	IsLeft = false;
	dZJoint = zEdge = 0;
	xBra = x;
	for (tNo = 0; tNo <= trackNoE; tNo++)
		if (IsTrackInSight(tNo) && IsInTrack(zDraw, tNo)) {
			if (IsJointed(zDraw, trackNo, tNo, zEdge, dZJoint)) {
				IsLeft = (trackTbl[tNo].isConn == (CalcTrackXPos((zEdge + dZJoint), tNo) < CalcTrackXPos((zEdge + dZJoint), trackNo)));
				xBra = CalcTrackXPos(zDraw, tNo);
				break;
			}
			if (IsJointed(zDraw, tNo, trackNo, zEdge, dZJoint)) {
				IsLeft = (trackTbl[trackNo].isConn == (CalcTrackXPos((zEdge + dZJoint), trackNo) < CalcTrackXPos((zEdge + dZJoint), tNo)));
				xBra = CalcTrackXPos(zDraw, tNo);
				break;
			}
		}

	xIn = x + XRailIn * sign;
	xOut = xIn + XRailSize * sign;
	xInBra = xBra + XRailIn * sign;
	dX = 0;
	if (((xInBra - xIn) * sign > 0) && ((sign > 0) != IsLeft)) {
		if ((xInBra - xOut) * sign < 0)
			xOut = xInBra;
		dX = - MyRound(XJointGap * ((zEdge + dZJoint - zDraw) / double(dZJoint)));
	} else
		for (tNo = 0; tNo <= trackNoE; tNo++)
			if (tNo != trackNo)
				if (IsTrackInSight(tNo) && IsInTrack(zDraw, tNo)) {
					xCross = CalcTrackXPos(zDraw, tNo);
					xDist = (xCross - x) * sign;
					if ((XRailIn * 2 - XRailSize * 2 <= xDist) && (xDist <= XRailIn * 2 + XRailSize * 2)) {
						if (XRailIn * 2 - XCrossGap <= xDist) {
							xOutCut = xCross - XRailIn * sign;
							if ((xOutCut - xOut) * sign < 0)
								xOut = xOutCut;
							xSupport = xOutCut + XCrossGap * sign;
							if ((xOutCut - xOut) * sign > 0)
								xSupport += (xOutCut - xOut);
							pXIn  = MyRound((xSupport                    - xNow) * zRate - xDirNow);
							pXOut = MyRound((xSupport + XRailSize * sign - xNow) * zRate - xDirNow);
							DrawHorizontalLine(pYDraw, pXIn, pXOut, ColorRail);
						}
						if (! IsCrossTrack(trackNo, tNo)) {
							xProtect = x - (XRailIn - XCrossGap) * sign;
							if (xDist < XRailIn * 2 - XRailSize)
								xProtect += (((XRailIn * 2 - XRailSize) - xDist) * sign);
							else if (xDist > XRailIn * 2 + XRailSize)
								xProtect += ((xDist - (XRailIn * 2 + XRailSize)) * sign);
							pXIn  = MyRound((xProtect                    - xNow) * zRate - xDirNow);
							pXOut = MyRound((xProtect + XRailSize * sign - xNow) * zRate - xDirNow);
							DrawHorizontalLine(pYDraw, pXIn, pXOut, ColorRail);
						}
						break;
					} else if ((0 <= xDist) && (xDist <= XRailSize * 5) && IsCrossTrack(trackNo, tNo)) {
						xOutCut = xCross + (XRailIn - XCrossGap) * sign;
						if ((xOutCut - xOut) * sign < 0)
							xOut = xOutCut;
						if (xDist >= XRailSize + XCrossGap) {
							xSupport = xOutCut;
							if ((xOutCut - xOut) * sign > XRailSize * 3)
								xSupport -= (xOutCut - xOut - XRailSize * 3 * sign);
							pXIn  = MyRound((xSupport                    - xNow) * zRate - xDirNow);
							pXOut = MyRound((xSupport - XRailSize * sign - xNow) * zRate - xDirNow);
							DrawHorizontalLine(pYDraw, pXIn, pXOut, ColorRail);
						}
						break;
					}
				}

	if ((xOut - xIn) * sign >= 0) {
		pXIn  = MyRound((xIn  + dX * sign - xNow) * zRate - xDirNow);
		pXOut = MyRound((xOut + dX * sign - xNow) * zRate - xDirNow);
		DrawHorizontalLine(pYDraw, pXIn, pXOut, ColorRail);
	}
}

// ホームを描く
void CKatamachiLineGameDlg::DrawPlatform(int pYDraw) {
	int    zE;       // ホームの終了Ｚ座標
	int    zS;       // ホームの開始Ｚ座標
	double yDraw;    // 描画位置のＹ座標
	int    zBottom;  // 描画位置のホーム底面でのＺ座標
	int    zTop;     // 描画位置のホーム上面でのＺ座標
	double zRate;    // 単位距離に対する描画位置の距離のＺ方向の比率
	int    pXLeft;   // ホームの左端の物理Ｘ座標
	int    pXRight;  // ホームの右端の物理Ｘ座標

	zE = 0;
	while (zE <= ZEnd) {
		zS = zE - ZFormSize;
		zBottom = zNow + MyRound(ZUnit * ( YGround              / double(pYDraw)));
		zTop    = zNow + MyRound(ZUnit * ((YGround - YFormSize) / double(pYDraw)));
		if ((zS <= zTop) && (zTop <= zE)) {
			zRate = ZUnit / double(zTop - zNow);
			pXLeft  = MyRound((XFormIn             - xNow) * zRate - xDirNow);
			pXRight = MyRound((XFormIn + XFormSize - xNow) * zRate - xDirNow);
			DrawHorizontalLine(pYDraw, pXLeft, pXRight, ColorFrmT);
		}
		if (zBottom > zE)
			zBottom = zE;
		if (zTop < zS)
			zTop = zS;
		if (zBottom >= zTop) {
			pXLeft  = MyRound((XFormIn - xNow) * (ZUnit / double(zBottom - zNow)) - xDirNow);
			pXRight = MyRound((XFormIn - xNow) * (ZUnit / double(zTop    - zNow)) - xDirNow);
			DrawHorizontalLine(pYDraw, pXLeft, pXRight, ColorFrmS);
		}
		yDraw = MyRound(pYDraw * ((zS - zNow) / double(ZUnit)));
		if ((YGround - YFormSize <= yDraw) && (yDraw <= YGround)) {
			zRate = ZUnit / double(zS - zNow);
			pXLeft  = MyRound((XFormIn             - xNow) * zRate - xDirNow);
			pXRight = MyRound((XFormIn + XFormSize - xNow) * zRate - xDirNow);
			DrawHorizontalLine(pYDraw, pXLeft, pXRight, ColorFrmF);
		}
		zE += ZEnd;
	}
}

// 電柱を描く
void CKatamachiLineGameDlg::DrawElectricPole(int pYDraw) {
	int    z;        // 電柱のＺ座標
	double yDraw;    // 描画位置のＹ座標
	double zRate;    // 単位距離に対する描画位置の距離のＺ方向の比率
	int    pXLeft;   // 電柱の左端の物理Ｘ座標
	int    pXRight;  // 電柱の右端の物理Ｘ座標

	z = ZPoleTerm * int(ceil((zNow + ZUnit) / double(ZPoleTerm)));
	while ((z <= ZEnd) && (z <= zNow + ZSight)) {
		zRate = ZUnit / double(z - zNow);
		yDraw = pYDraw * ((z - zNow) / double(ZUnit));
		if ((YGround - YPoleSize <= yDraw) && (yDraw <= YGround)) {
			pXLeft  = MyRound((- XPoleDiff - XPoleSize / 2.0 - xNow) * zRate - xDirNow);
			pXRight = MyRound((- XPoleDiff + XPoleSize / 2.0 - xNow) * zRate - xDirNow);
			DrawHorizontalLine(pYDraw, pXLeft, pXRight, ColorPole);
			if (((0 < z) && (z <= ZEnd - ZFormSize)) || (yDraw <= YGround - YFormSize)) {
				pXLeft  = MyRound((XPoleDiff - XPoleSize / 2.0 - xNow) * zRate - xDirNow);
				pXRight = MyRound((XPoleDiff + XPoleSize / 2.0 - xNow) * zRate - xDirNow);
				DrawHorizontalLine(pYDraw, pXLeft, pXRight, ColorPole);
			}
		}
		z += ZPoleTerm;
	}
}

// 電線を描く
void CKatamachiLineGameDlg::DrawElectricLine(int pYDraw) {
	double zDraw;   // 描画位置のＺ座標
	int    pXBack;  // 電線の向う隣の描画位置の物理Ｘ座標
	int    pXForw;  // 電線の手前隣の描画位置の物理Ｘ座標

	if (pYDraw > -1)
		return;

	zDraw = zNow + ZUnit * (YElecLine / double(pYDraw));
	if ((zDraw > ZEnd) || (zDraw > zNow + ZSight))
		return;

	pXBack = MyRound((- XPoleDiff - xNow) * ((pYDraw - 1) / double(YElecLine)) - xDirNow);
	pXForw = MyRound((- XPoleDiff - xNow) * ( pYDraw      / double(YElecLine)) - xDirNow);
	DrawHorizontalLine(pYDraw, pXBack, pXForw, ColorLine);
	pXBack = MyRound((  XPoleDiff - xNow) * ((pYDraw - 1) / double(YElecLine)) - xDirNow);
	pXForw = MyRound((  XPoleDiff - xNow) * ( pYDraw      / double(YElecLine)) - xDirNow);
	DrawHorizontalLine(pYDraw, pXBack, pXForw, ColorLine);
}

// 障害物を描く
void CKatamachiLineGameDlg::DrawObstacle(int pYDraw, int obstacleNo) {
	int    z;          // 障害物のＺ座標
	int    yDistDraw;  // 描画位置の地面からのＹ方向の距離
	double zRate;      // 単位距離に対する描画位置の距離のＺ方向の比率
	double yDistRate;  // 障害物のＹ方向の半径に対する中心から描画位置までの距離の比率
	int    xDist;      // 障害物のＸ方向の中心から描画位置までの距離
	int    xLeft;      // 障害物の左端のＸ座標
	int    xRight;     // 障害物の右端のＸ座標
	int    pXLeft;     // 障害物の左端の物理Ｘ座標
	int    pXRight;    // 障害物の右端の物理Ｘ座標
	double dXLeg;      // 障害物の足のＸ方向の補正量
	int    x;          // 障害物のいる線路のＸ座標

	z = obstacleTbl[obstacleNo].z;
	if ((z < zNow + ZUnit) || (zNow + ZSight < z))
		return;

	yDistDraw = YGround - MyRound(pYDraw * ((z - zNow) / double(ZUnit)));
	if ((yDistDraw < 0) || (YObstSize < yDistDraw))
		return;

	x = CalcTrackXPos(z, (obstacleTbl[obstacleNo].trackNo));
	zRate = ZUnit / double(z - zNow);

	if (YObstSize * 0.1 <= yDistDraw) {
		yDistRate = (yDistDraw - YObstSize * 0.55) / double(YObstSize * 0.45);
		xDist = MyRound(XObstSize * sqrt(1.0 - yDistRate * yDistRate));
		pXLeft  = MyRound((x - XObstSize * 0.1 - xDist * 0.3 - xNow) * zRate - xDirNow);
		pXRight = MyRound((x - XObstSize * 0.1 + xDist * 0.5 - xNow) * zRate - xDirNow);
		DrawHorizontalLine(pYDraw, pXLeft, pXRight, ColorObst);
	}
	if (YObstSize * 0.20 <= yDistDraw) {
		yDistRate = (abs((yDistDraw - YObstSize * 0.55) / double(YObstSize * 0.45)) - 0.1) / 0.8;
		if (yDistRate < 0)
			yDistRate = 0;
		xLeft  = x + MyRound(XObstSize * 0.30 - XObstSize * 0.10 * yDistRate);
		xRight = x + MyRound(XObstSize * 0.45 - XObstSize * 0.22 * yDistRate);
		pXLeft  = MyRound((xLeft  - xNow) * zRate - xDirNow);
		pXRight = MyRound((xRight - xNow) * zRate - xDirNow);
		DrawHorizontalLine(pYDraw, pXLeft, pXRight, ColorObst);
	}
	if (yDistDraw <= YObstSize * 0.20) {
		yDistRate = (YObstSize * 0.2 - yDistDraw) / double(YObstSize * 0.2);
		dXLeg = XObstSize * 0.03 - MyRound(XObstSize * 0.03 * (yDistRate * yDistRate));
		pXLeft  = MyRound((x - XObstSize * 0.18 - dXLeg - xNow) * zRate - xDirNow);
		pXRight = MyRound((x - XObstSize * 0.11 - dXLeg - xNow) * zRate - xDirNow);
		DrawHorizontalLine(pYDraw, pXLeft, pXRight, ColorObst);
		pXLeft  = MyRound((x + XObstSize * 0.11 + dXLeg - xNow) * zRate - xDirNow);
		pXRight = MyRound((x + XObstSize * 0.18 + dXLeg - xNow) * zRate - xDirNow);
		DrawHorizontalLine(pYDraw, pXLeft, pXRight, ColorObst);
	}
	if ((YObstSize * 0.5 <= yDistDraw) && (yDistDraw <= YObstSize * 0.7)) {
		yDistRate = (YObstSize * 0.7 - yDistDraw) / double(YObstSize * 0.2);
		xLeft  = x + MyRound(- XObstSize * 0.53 + XObstSize * 0.20 * (yDistRate * yDistRate));
		xRight = x + MyRound(- XObstSize * 0.60 + XObstSize * 0.15 * (yDistRate * yDistRate));
		pXLeft  = MyRound((xLeft  - xNow) * zRate - xDirNow);
		pXRight = MyRound((xRight - xNow) * zRate - xDirNow);
		DrawHorizontalLine(pYDraw, pXLeft, pXRight, ColorObst);
	}
	if ((YObstSize * 0.58 <= yDistDraw) && (yDistDraw <= YObstSize * 0.63)) {
		yDistRate = abs((yDistDraw - YObstSize * 0.58) / double(YObstSize * 0.05));
		xLeft  = x + MyRound(XObstSize * 0.38 - XObstSize * 0.05 * yDistRate);
		xRight = x + MyRound(XObstSize * 0.40 - XObstSize * 0.03 * yDistRate);
		pXLeft  = MyRound((xLeft  - xNow) * zRate - xDirNow);
		pXRight = MyRound((xRight - xNow) * zRate - xDirNow);
		DrawHorizontalLine(pYDraw, pXLeft, pXRight, ColorEye);
	}
}

// 風船を描く
void CKatamachiLineGameDlg::DrawBalloon(int pYDraw, int balloonNo) {
	int    z;          // 風船のいるＺ座標
	int    xRadius;    // 風船のＸ方向の補正後の大きさ
	int    yRadius;    // 風船のＹ方向の補正後の大きさ
	int    y;          // 風船のいるＹ座標
	int    yDraw;      // 描画位置のＹ座標
	int    x;          // 風船のいるＸ座標
	double zRate;      // 単位距離に対する描画位置の距離のＺ方向の比率
	double yDistRate;  // 風船のＹ方向の半径に対する中心から描画位置までの距離の比率
	int    xDist;      // 風船のＸ方向の中心から描画位置までの距離
	int    pXLeft;     // 風船の左端の物理Ｘ座標
	int    pXRight;    // 風船の右端の物理Ｘ座標
	int    pX;         // 紐の右端の物理Ｘ座標

	z = balloonTbl[balloonNo].z;
	if (z < zNow + ZUnit)
		return;

	y = balloonTbl[balloonNo].y;
	if (y >= YGround - YFormSize)
		return;

	xRadius = (XBallSize / 2);
	yRadius = (YBallSize / 2);
	if (YGround - YFormSize - y < yRadius * 2) {
		xRadius = MyRound(xRadius * ((YGround - YFormSize - y) / double(yRadius * 2)));
		yRadius = ((YGround - YFormSize - y) / 2);
	}

	yDraw = MyRound(pYDraw * ((z - zNow) / double(ZUnit)));
	if (yDraw < y - yRadius)
		return;

	x = balloonTbl[balloonNo].x;
	zRate = ZUnit / double(z - zNow);

	if (yDraw <= y + yRadius) {
		yDistRate = (yDraw - y) / double(yRadius);
		xDist = MyRound(xRadius * sqrt(1.0 - yDistRate * yDistRate));
		pXLeft  = MyRound((x - xDist - xNow) * zRate - xDirNow);
		pXRight = MyRound((x + xDist - xNow) * zRate - xDirNow);
		DrawHorizontalLine(pYDraw, pXLeft, pXRight, (balloonTbl[balloonNo].color));
	} else if ((yDraw <= y + yRadius * 3) && (50 * zRate >= 1)) {
		if (yDraw <= y + yRadius + 30) {
			xDist = yDraw - (y + yRadius);
			pXLeft  = MyRound((x - xDist - xNow) * zRate - xDirNow);
			pXRight = MyRound((x + xDist - xNow) * zRate - xDirNow);
			DrawHorizontalLine(pYDraw, pXLeft, pXRight, (balloonTbl[balloonNo].color));
		}
		xDist = MyRound((xRadius * 0.3) * sin(Pi * ((yDraw - (y + yRadius)) / double(yRadius))));
		pX = MyRound((x + xDist - xNow) * zRate - xDirNow);
		DrawHorizontalLine(pYDraw, pX, pX, ColorStri);
	}
}

// 窓のひび割れを描く
void CKatamachiLineGameDlg::DrawWindowCrack() {
	const int NumOfCenters = 2;                      // ひび割れの個数
	const int NumOfCracks = 36;                      // ひび割れの個数
	const int LenMin = 80;                           // ひび割れの１片の最小長
	const int LenMax = 100;                          // ひび割れの１片の最大長
	const int DDirMin = MyRound((Pi / 180) * 45);  // ひび割れの方向の変位の最小値
	const int DDirMax = MyRound((Pi / 180) * 50);  // ひび割れの方向の変位の最大値
	int    idxC;  // ひび割れの中心の指標
	int    idx;   // ひび割れの指標
	int    pXC;   // ひび割れの中心の物理Ｘ座標
	int    pYC;   // ひび割れの中心の物理Ｙ座標
	double dir;   // ひび割れの方向
	double dDir;  // ひび割れの方向の変位
	double len;   // ひび割れの１片の長さ
	int    pX;    // ひび割れの節点の物理Ｘ座標
	int    pY;    // ひび割れの節点の物理Ｙ座標

	CPen pen(PS_SOLID, 1, COLORREF(ColorCrac));
	pDC->SelectObject(pen);
	for (idxC = 1; idxC <= NumOfCenters; idxC++) {
		pXC = int(PXOrg * MyRandom() - PXOrg / 2.0);
		pYC = int(PYOrg * MyRandom() - PYOrg / 2.0);
		for (idx = 1; idx <= NumOfCracks; idx++) {
			pX = pXC;
			pY = pYC;
			pDC->MoveTo((PXOrg + pX), (PYOrg + pY));
			dir = (Pi * 2) * ((idx - 1) / double(NumOfCracks));
			while ((abs(pX) <= PXOrg) && (abs(pY) <= PYOrg)) {
				len  = LenMin  + (LenMax  - LenMin ) * MyRandom();
				dDir = DDirMin + (DDirMax - DDirMin) * MyRandom();
				if (MyRandom() < 0.5)
					dir = dir - dDir;
				else
					dir = dir + dDir;
				pX += MyRound(len * cos(dir));
				pY += MyRound(len * sin(dir));
				pDC->LineTo((PXOrg + pX), (PYOrg + pY));
			}
		}
	}
}

// 指定線路同士が指定Ｚ座標で接合しているかを判別する
bool CKatamachiLineGameDlg::IsJointed(int z, int trackNo, int trackNoBra, int& zEdge, int& dZ) {
	int zS;  // 線路の開始点のＺ座標
	int zE;  // 線路の終了点のＺ座標
	int xS;  // 線路の開始点のＸ座標
	int xE;  // 線路の終了点のＸ座標

	if (trackNo == trackNoBra)
		return false;

	xS = trackTbl[trackNoBra].xS;
	xE = trackTbl[trackNoBra].xE;
	if (xS == xE)
		return false;

	zS = trackTbl[trackNoBra].zS;
	zE = trackTbl[trackNoBra].zE;
	if (IsInTrack(zS, trackNo) && (xS == CalcTrackXPos(zS, trackNo)) && (z >= zS)) {
		dZ = MyRound((zE - zS) * sqrt((XRailSize + XJointGap) / double(abs(xE - xS)) / 2.0));
		if (z > zS + dZ)
			return false;
		zEdge = zS;
	} else if (IsInTrack(zE, trackNo) && (xE == CalcTrackXPos(zE, trackNo)) && (z <= zE)) {
		dZ = - MyRound((zE - zS) * sqrt((XRailSize + XJointGap) / double(abs(xE - xS)) / 2.0));
		if (z < zE + dZ)
			return false;
		zEdge = zE;
	} else
		return false;

	return true;
}

// 指定線路同士が交差しているかを判別する
bool CKatamachiLineGameDlg::IsCrossTrack(int trackNoA, int trackNoB) {
	return (((trackTbl[trackNoA].xS > trackTbl[trackNoB].xS) &&
			 (trackTbl[trackNoA].xE < trackTbl[trackNoB].xE)   ) ||
			((trackTbl[trackNoA].xS < trackTbl[trackNoB].xS) &&
			 (trackTbl[trackNoA].xE > trackTbl[trackNoB].xE)   )   );
}

// 指定Ｚ座標から指定線路のＸ座標を求める
int CKatamachiLineGameDlg::CalcTrackXPos(int z, int trackNo) {
	int    xS;    // 線路の開始点のＸ座標
	int    xE;    // 線路の終了点のＸ座標
	int    zS;    // 線路の開始点のＺ座標
	int    zE;    // 線路の終了点のＺ座標
	double xC;    // 線路の中間点のＸ座標
	double zC;    // 線路の中間点のＺ座標
	int    tNo;   // 線路の番号
	double rate;  // 線路全体の長さに対する接続部分の長さの比率

	xS = trackTbl[trackNo].xS;
	xE = trackTbl[trackNo].xE;
	if (xS == xE) {
		return xS;
	}

	zS = trackTbl[trackNo].zS;
	zE = trackTbl[trackNo].zE;
	xC = (xS + xE) / 2.0;
	zC = (zS + zE) / 2.0;
	if (z < zS) {
		for (tNo = 0; tNo <= trackNoE; tNo++)
			if (tNo != trackNo)
				if (IsInTrack(zS, tNo))
					if (CalcTrackXPos(zS, tNo) == xS)
						break;
		return CalcTrackXPos(z, tNo);
	}
	if (z > zE) {
		for (tNo = 0; tNo <= trackNoE; tNo++)
			if (tNo != trackNo)
				if (IsInTrack(zE, tNo))
					if (CalcTrackXPos(zE, tNo) == xE)
						break;
		return CalcTrackXPos(z, tNo);
	}
	if (z < zC) {
		rate = (z - zS) / double(zC - zS);
		return xS + MyRound((xC - xS) * (rate * rate));
	} else {
		rate = (z - zE) / double(zC - zE);
		return xE + MyRound((xC - xE) * (rate * rate));
	}
}

// 指定線路の一部が目視可能範囲にいるかを判別する
bool CKatamachiLineGameDlg::IsTrackInSight(int trackNo) {
	return ((zNow + ZUnit < trackTbl[trackNo].zE) && (trackTbl[trackNo].zS <= zNow + ZSight));
}

// 指定Ｚ座標が指定線路に含まれるかを判別する
bool CKatamachiLineGameDlg::IsInTrack(int z, int trackNo) {
	return ((trackTbl[trackNo].zS <= z) && (z <= trackTbl[trackNo].zE));
}

// 水平線を描く
void CKatamachiLineGameDlg::DrawHorizontalLine(int pY, int pXS, int pXE, int col) {
	int pX;  // Ｘ座標値入替え用変数

	if (pXE < pXS) {
		pX = pXS;
		pXS = pXE;
		pXE = pX;
	}
	if ((pXE < - PXOrg) || (pXS > PXOrg))
		return;
	if (pXS < - PXOrg)
		pXS = - PXOrg;
	if (pXE > PXOrg)
		pXE = PXOrg;
	pDC->FillSolidRect((PXOrg + pXS), (PYOrg + pY),
					   (pXE + 1 - pXS), 1, COLORREF(col));
}

