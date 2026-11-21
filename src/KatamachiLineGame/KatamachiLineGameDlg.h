class TTrack {      // 線路情報型の定義
public:
	int  zS;          // 線路の開始点のＺ座標
	int  zE;          // 線路の終了点のＺ座標
	int  xS;          // 線路の開始点のＸ座標
	int  xE;          // 線路の終了点のＸ座標
	bool isConn;      // 線路の両端が他の線路の流れを横取りした状態ならtrue
};

class TObstacle {   // 障害物情報型の定義
public:
	int z;            // 障害物のいるＺ座標
	int trackNo;      // 障害物のいる線路の番号
};

class TBalloon {    // 風船情報型の定義
public:
	int      x;       // 風船のいるＸ座標
	int      y;       // 風船のいるＹ座標
	int      z;       // 風船のいるＺ座標
	COLORREF color;   // 風船の色
};

class CKatamachiLineGameDlg : public CDialogEx {

public:
	CKatamachiLineGameDlg(CWnd* pParent = nullptr);	// 標準コンストラクター

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KATAMACHILINEGAME_DIALOG };
#endif

protected:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

	CClientDC* pDC;              // デバイスコンテキストへのポインタ
	TTrack     trackTbl[120];    // 線路テーブル
	int        trackNoE;         // 線路の最終番号
	int        trackNoNow;       // 現在の視点の線路の番号
	double     probObst;         // 分岐器間に障害物がいる確率
	int        zSpeed;           // 現在の視点の速度のＺ成分
	TObstacle  obstacleTbl[50];  // 障害物テーブル
	int        obstacleNoE;      // 障害物の最終番号
	TBalloon   balloonTbl[400];  // 風船テーブル
	int        balloonNoE;       // 風船の最終番号
	int        xDirNow;          // 現在の視点の方向のＸ成分
	int        zNow;             // 現在の視点のＺ座標
	int        xNow;             // 現在の視点のＸ座標
	void GenerateBalloon();
	void DrawAll(int zOld, int xOld);
	void DrawBallastArea(int pYDraw, int zDraw);
	void DrawTie(int pYDraw, int zDraw, int trackNo);
	void DrawRail(int pYDraw, int zDraw, int trackNo, int sign);
	void DrawPlatform(int pYDraw);
	void DrawElectricPole(int pYDraw);
	void DrawElectricLine(int pYDraw);
	void DrawObstacle(int pYDraw, int obstacleNo);
	void DrawBalloon(int pYDraw, int balloonNo);
	void DrawWindowCrack();
	bool IsJointed(int z, int trackNo, int trackNoBra, int& zEdge, int& dZ);
	bool IsCrossTrack(int trackNoA, int trackNoB);
	int CalcTrackXPos(int z, int trackNo);
	bool IsTrackInSight(int trackNo);
	bool IsInTrack(int z, int trackNo);
	void DrawHorizontalLine(int pY, int pXS, int pXE, int col);
};

