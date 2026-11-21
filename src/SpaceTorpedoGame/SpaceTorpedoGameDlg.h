// 位置情報型の定義
class TPos {
public:
	double   x;        // Ｘ座標
	double   y;        // Ｙ座標
	double   z;        // Ｚ座標
};

// 恒星情報型の定義
class TStar {
public:
	int      xL;       // 論理Ｘ座標
	int      yL;       // 論理Ｙ座標
	COLORREF col;      // 恒星の色
	COLORREF rndCol;   // 恒星の周囲の色
};

// 敵宇宙船情報型の定義
class TEnemyShip : public TPos {
public:
	double   xVel;     // 敵宇宙船の速度のＸ成分
	double   yVel;     // 敵宇宙船の速度のＹ成分
	double   xAcc;     // 敵宇宙船の加速度のＸ成分
	double   yAcc;     // 敵宇宙船の加速度のＹ成分
	COLORREF col;      // 敵宇宙船の色
	int      cllpDeg;  // 崩壊進行度
};

// 魚雷情報型の定義
class TTorpedo : public TPos {
public:
	double   xVel;     // 魚雷の速度のＸ成分
	double   yVel;     // 魚雷の速度のＹ成分
};

class CSpaceTorpedoGameDlg : public CDialogEx {

public:
	CSpaceTorpedoGameDlg(CWnd* pParent = nullptr);	// 標準コンストラクター

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SpaceTorpedoGAME_DIALOG };
#endif

protected:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

	CClientDC* pDC;               // デバイスコンテキストへのポインタ
	TStar      starTbl[1000];     // 恒星テーブル
	CImage     imgEnemyEShip;     // 敵宇宙船の画像データ
	TEnemyShip enemyShipTbl[10];  // 敵宇宙船テーブル
	TTorpedo   enemyTorpd;        // 敵の魚雷
	TTorpedo   torpedo;           // 魚雷
	int        torpedoRemainTim;  // 魚雷発射可能までの残り時間
	double     xSelfVel;          // 自分の移動速度のＸ成分
	double     ySelfVel;          // 自分の移動速度のＹ成分
	int        selfDmgRemainTim;  // 自分へのダメージが収まる残り時間
	int        score;             // 得点
	void controlSelfPerpVel();
	bool IsHit(TPos& oldOrg, TPos& oldSbj, TPos& newOrg, TPos& newSbj);
	void InitEnemyShip(TEnemyShip& eS, double z);
	void DrawAll();
	void DrawEnemyShip(TEnemyShip & eS);
	void DrawTorpedo(TTorpedo& torpd);
};

