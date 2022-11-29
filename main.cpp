#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
const char kWindowTitle[] = "学籍番号";

float easeInQuad(float t, float b, float c, float d) {
	t /= d;
	if (t < 0.3) {
		return c / 2.0 * t * t + b;
	}
	t = t - 1;
	return c / 2.0 * (t * (t - 2) - 1) + b;
}

int degreeToRadian(float a) {
	float radian;
	radian = a * (M_PI / 180.0f);
	return radian;
}

struct Vec2 {
	float X;
	float Y;
};
struct quad {
	Vec2 LT;/*LeftTop*/
	Vec2 LB;/*LeftBottom*/
	Vec2 RT;/*RightTop*/
	Vec2 RB;/*RightBottom*/
};
struct player {
	Vec2 Pos/*ピクセル単位の座標*/;
	quad BlockPos/*ブロック単位の座標*/;
	int Radius/*半径*/;
	Vec2 Speed/*速度*/;
	quad Quad;
	quad QuadBlock;
};
//ボス関連
struct boss {
	Vec2 Pos/*ピクセル単位の座標*/;
	int Radius/*半径*/;
	Vec2 Speed/*速度*/;
	int Move/*動いてる方向*/;
	int Warp/*ワープ*/;
	int Timer;
	Vec2 Copy/*位置保存*/;
	int CoolDown/*止まる時間*/;
	int ColorChange/*透明度変更*/;
	int Down/*落ちる攻撃*/;
	int DownTimer/*落ちる攻撃用*/;
	float Vel/*落下速度*/;
	int RandTimer/*ランダム変数用*/;
	int Laser/*レーザー攻撃*/;
	int LaserPosX[8]/*レーザー座標X*/;
	int LaserPosY[8]/*レーザー座標Y*/;
	int LaserRadiusX[8]/*半径X*/;
	int LaserRadiusY[8]/*半径Y*/;
	int LaserFlag/*レーザー用フラグ*/;
	int RainLaserFlag/*雨レーザー*/;
	int LaserTimer/*レーザー用タイマー*/;
	int LaserSpeed;
	int Start;
	int StartTimer;
	int HP/*ボスHP*/;
	int Danger/*危険マーク*/;
};

struct playertmp {
	Vec2 Pos/*ピクセル単位の座標*/;
	quad BlockPos/*ブロック単位の座標*/;
	Vec2 Speed;
	quad Quad;
	quad QuadBlock;
};

//弾関連
struct attack4 {
	Vec2 Pos[4]/*ピクセル単位の座標*/;
	quad BlockPos/*ブロック単位の座標*/;
	int Radius/*半径*/;
	Vec2 Speed/*速度*/;
	quad Quad[4];
	int Shotflag/*弾を打ったフラグ*/;
	int Timer/*制限用タイマー*/;
	float Vel/*落下速度*/;
	int Delete[4];
	Vec2 RePos[4];
	int ReRadius/*半径*/;
	Vec2 ReSpeed/*速度*/;
	quad ReQuad[4];
	quad ReQuadBlock[4];
	int ReShotflag;
};

struct attack4tmp {
	Vec2 Pos[4]/*ピクセル単位の座標*/;
	quad BlockPos/*ブロック単位の座標*/;
	Vec2 Speed;
	quad Quad;
	quad QuadBlock[4];
};

const quad Quad = {
		{-32,-32},
		{-32,32},
		{32,-32},
		{32,32},
};

//プロトタイプ宣言

///移動用関数
///(移動する物の座標, キー入力, 速度)
void move(Vec2& Player, char keys[256], const Vec2 speed);
///ブロックサイズへの変換
///(ピクセル単位のポジション, ブロックサイズ)
Vec2 toBlock(const Vec2 Posision, const int BlockSize);
///スクリーン座標系への変換
///(ブロック単位のポジション,ブロックサイズ)
Vec2 toPosision(const Vec2 BlockPos, const int BlockSize);
///中心座標を入れると64x64の矩形を作る
quad makeQuad(Vec2 point);
/// <summary>
/// スペースを押すとジャンプする(判定処理あり)
/// </summary>
/// <param name="PlayerPosY">プレイヤーのY座標</param>
/// <param name="keys">keys</param>
/// /// <param name="preKeys">preKeys</param>
/// <param name="JanpFlag">JanpFlagを入れてね</param>
/// <param name="Velocity">Velocityを入れてね</param>
void janpFanc(float& PlayerPosY, char keys[256], char preKeys[256], int& JanpFlag, float& Velocity);

/// <summary>
/// 二段ジャンプができる方
/// </summary>
/// <param name="PlayerPos">プレイヤーの位置</param>
/// <param name="keys">keysを入力</param>
/// <param name="preKeys">preKeysを入力</param>
/// <param name="JanpFlag">JanpFlagを入力</param>
/// <param name="JanpAir">JanpAirを入力</param>
/// <param name="Velocity">Velocityを入力</param>
void janpFancAir(Vec2& PlayerPos, char keys[256], char preKeys[256], int& JanpFlag, int& JanpAir, float& Velocity);

/// <summary>
/// 描画のあとに入れるの推奨
/// </summary>
/// <param name="FuncFrame">カウント用の変数</param>
/// <param name="AnimeFrame">アニメーション用の変数</param>
/// /// <param name="AnimeChenge">何フレームで次のフレームに行くか</param>
/// <param name="EndFrame">終わりのフレーム</param>
void FrameCount(int& FuncFrame, int& AnimeFrame, int AnimeChenge, const int EndFrame);


/// <summary>
/// 当たっていると1を返します。それ以外は0
/// </summary>
/// <param name="Point1">1つ目のPosの値</param>
/// <param name="P1H">Point1縦の半径</param>
/// <param name="P1W">Point1横の半径</param>
/// <param name="Point2">2つ目のPosの値</param>
/// <param name="P2H">Point2縦の半径</param>
/// <param name="P2W">Point2横の半径</param>
/// <returns></returns>
int QuadCollision(Vec2 Point1, int P1H, int P1W, Vec2 Point2, int P2H, int P2W);


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 640);

	//ブロックの画像
	int GLblock = Novice::LoadTexture("./Resource/block.png");
	//プレイヤーの画像
	int GLplayerStayR = Novice::LoadTexture("./Resource/playerIdleR.png");
	int GLplayerStayL = Novice::LoadTexture("./Resource/playerIdleL.png");
	int GLplayerRunR = Novice::LoadTexture("./Resource/playerRunR.png");
	int GLplayerRunL = Novice::LoadTexture("./Resource/playerRunL.png");
	int GLplayerAttackR = Novice::LoadTexture("./Resource/playerAttackR.png");
	int GLplayerAttackL = Novice::LoadTexture("./Resource/playerAttackL.png");
	//プレイヤーの攻撃
	int GLplayerAttackEffectR = Novice::LoadTexture("./Resource/AttackwaveR.png");
	int GLplayerAttackEffectL = Novice::LoadTexture("./Resource/AttackwaveL.png");
	//敵の画像
	int GLEnemyStay = Novice::LoadTexture("./Resource/monster.png");
	//背景
	int GLBack = Novice::LoadTexture("./Resource/background.png");
	int GLBack1 = Novice::LoadTexture("./Resource/TD_background2.png");
	//レーザー
	int Laser= Novice::LoadTexture("./Resource/Laser.png");
	int Laser1 = Novice::LoadTexture("./Resource/Laser!1.png");
	int Laser2 = Novice::LoadTexture("./Resource/Laser!2.png");
	//弾
	int Bullet= Novice::LoadTexture("./Resource/Bullet.png");
	int SmallBullet= Novice::LoadTexture("./Resource/SmallBullet.png");
	int TriangleBullet= Novice::LoadTexture("./Resource/TriangleBullet.png");
	int STriangleBullet = Novice::LoadTexture("./Resource/STriangleBullet.png");
	//ブロックサイズ
	const int BlockSize = 32;
	//マップの大きさ
	const int MapSizeX = 40;
	const int MapSizeY = 20;
	//MAP(二次元配列)
	int map[MapSizeY][MapSizeX] = {
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	};

	int map1[MapSizeY][MapSizeX+40] = {
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2.2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2.2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2.2},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2},
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	};
	enum Map {
		none,
		Block,
	};
	//プレイヤー
	player Player = {
		{96,576}/*初期位置が[1][1]*/,
		{0,0},
		{32},
		{4,4},
	};
	boss Boss = {
		{0,0},
		{64},
		{2,2},
		{1}
	};

	//弾
	attack4 Attack4{
		{100,100},
		{0,0},
		{64},
		{4,4}
	};

	attack4tmp Attack4Tmp = {
		{0,0},
		{0,0},
		{4,4}
	};
	playertmp PlayerTmp = {
		{0,0},
		{0,0},
		{4,4}
	};

	const int Attack3Max = 6; //召喚弾数
	struct attack3
	{
		Vec2 Pos[Attack3Max];
		float Theta[Attack3Max]; //回転
		float t[Attack3Max];
		Vec2 Target[Attack3Max]; //発射するときにいたプレイヤーの位置
		int Time[Attack3Max] = { 0 }; //発射までの時間カウント
		int Attack[Attack3Max]; //攻撃発射してないならfalse
		Vec2 Start[Attack3Max]; //発射するときにいた弾の位置
		int ON[Attack3Max] = { false };
		int Check = 0; //一個ずつ発射するのに使うやつ
		int Color[Attack3Max]; //色(召喚時点:青 発射時:白)
		int AttackMove = 0;
		float tMove = 0;
	};
	attack3 Attack3;
	int Attack3CoolTime[Attack3Max] = { 0 }; //弾召喚までの時間カウント
	//-敵の攻撃３初期化などここまで---------------
	//ジャンプフラグ
	int JanpFlag = 0;
	int JanpAir = 0;
	float Velocity = 0;

	float Acceleration = 0.8f;

	int RandX[4] = { 0 };//ランダム用変数
	float RandY[4] = { 0 };
	int Rand[4] = { 0 };
	int LaserRand[4] = { 0 };

	
	float Theta = 0;
	float amplitude = 32.0f;

	float t = 0;//イージング用
	float b = 4;
	float c = 10;
	float d = 1;

	int WorldRandX = 0;//ワールド全体
	int WorldRandY = 0;
	int PlayerRandX = 0;//プレイヤー
	int PlayerRandY = 0;

	int scene = 0;
	int Color = WHITE;
	int AColor = 0x00000000;
	int ColorVal = 15;
	
	Boss.LaserFlag = 4;
	Boss.Pos.X = 1150;
	Boss.Pos.Y = -100;

	int InvincibleTimer = 0;
	int InvincibleFlag = 0;
	int Damage = 0/*ダメージ*/;
	int PlayerHP = 10000;

	int Scroll = 0;
	int MonitorX = 0;
	int StartFlag = 0;

	//アニメーションフレーム
	enum Flag {
		Left = -1,
		Right = 1,
	};
	int PlayerStayFrame = 0;
	int PlayerStayAnime = 0;
	int PlayerRunFrame = 0;
	int PlayerRunAnime = 0;
	int PlayerGLAnime = GLplayerStayR;
	int PlayerAnimeFlag = 1;
	int PlayerAttackFreme = 0;
	int PlayerAttackAnime = 0;
	int AttackFlag = 0;
	int AttackCount = 0;
	//攻撃用変数
	int AttackEffect = 0;
	int AttackEffectFlag = 0;
	int AttackEffectFreme = 0;
	int AttackEffectAnime = 0;
	int AttackEffectCount = 0;
	Vec2 EffectPos = { 0 };
	int EffectMove=10;
	int AttackHitFlag = 0;

	int EnemyStayFrame = 0;
	int EnemyStayAnime = 0;


	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };


	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		//プレイヤーの位置の変数更新
		Player.Speed = { 0,0 };


		PlayerTmp.Pos = Player.Pos;

		//スタートシーン
		if (scene == 0) {
			Rand[0] = rand() % 6 + 1;
			switch (Rand[0]) {
			case 1:
				RandX[0] = rand() % 12 + 1;
				break;

			case 2:
				RandX[0] = rand() % 12 + 1;
				break;

			case 3:
				RandX[0] = rand() % 12 + 1;
				break;

			case 4:
				RandX[0] = rand() % 12 + 1;
				break;

			default:
				RandX[0] = rand() % 12 + 1;
				break;
			}
			if (keys[DIK_C]) {
				
				
				Boss.Pos.X = 1130;
				Boss.Pos.Y = -80;
				Boss.Move = 2;
				Boss.HP = 10000;
				StartFlag = 1;
			}
		}

		//フェードイン
		if (StartFlag == 1) {
			AColor -= 0xFFFFFFFF;
			if (AColor == BLACK) {
				scene = 1;
				StartFlag = 2;
			}
		}

		if (scene == 1) {
			//フェードアウト
			if (StartFlag == 2) {
				AColor += 0xFFFFFFFF;
				if (AColor == 0x00000000) {
					StartFlag = 0;
				}
			}
			MonitorX = Player.Pos.X  - Scroll;
			if (keys[DIK_LEFT]) {
				PlayerTmp.Speed.X = 4;
				PlayerAnimeFlag = Left;
			}
			else if (keys[DIK_RIGHT]) {
				PlayerTmp.Speed.X = 4;
				PlayerAnimeFlag = Right;
			}
			if (keys[DIK_LEFT] && keys[DIK_RIGHT]) {
				PlayerTmp.Speed.X = 0;
			}
			//仮変数移動
			move(PlayerTmp.Pos, keys, PlayerTmp.Speed);
			PlayerTmp.Quad = makeQuad(PlayerTmp.Pos);
			//実数
			PlayerTmp.QuadBlock.LT = { (PlayerTmp.Quad.LT.X) / BlockSize, (PlayerTmp.Quad.LT.Y) / BlockSize };
			PlayerTmp.QuadBlock.LB = { (PlayerTmp.Quad.LB.X) / BlockSize,(PlayerTmp.Quad.LB.Y - 1) / BlockSize };
			PlayerTmp.QuadBlock.RT = { (PlayerTmp.Quad.RT.X - 1) / BlockSize,(PlayerTmp.Quad.RT.Y) / BlockSize };
			PlayerTmp.QuadBlock.RB = { (PlayerTmp.Quad.RB.X - 1) / BlockSize,(PlayerTmp.Quad.RB.Y - 1) / BlockSize };
			if (keys[DIK_LEFT]) {
				if (keys[DIK_S]) {
					if (map1[(int)PlayerTmp.QuadBlock.LT.Y][(int)PlayerTmp.QuadBlock.LT.X] == none && map1[(int)PlayerTmp.QuadBlock.LB.Y - 1][(int)PlayerTmp.QuadBlock.LB.X] == none) {
						Player.Speed.X = 2;
					}
				}
				else {
					if (map1[(int)PlayerTmp.QuadBlock.LT.Y][(int)PlayerTmp.QuadBlock.LT.X] == none && map1[(int)PlayerTmp.QuadBlock.LB.Y][(int)PlayerTmp.QuadBlock.LB.X] == none) {
						Player.Speed.X = 4;
						
						if (Player.Pos.X <= 2240) {
							
							Scroll -= Player.Speed.X;
						}
						if (Scroll <= 0) {
							
							Scroll += Player.Speed.X;
						}
					}
				}

			}
			else if (keys[DIK_RIGHT]) {

				if (keys[DIK_S]) {
					if (map1[(int)PlayerTmp.QuadBlock.RT.Y][(int)PlayerTmp.QuadBlock.RT.X] == none && map1[(int)PlayerTmp.QuadBlock.RB.Y - 1][(int)PlayerTmp.QuadBlock.RB.X] == none) {
						Player.Speed.X = 2;
					}
				}
				else {
					if (map1[(int)PlayerTmp.QuadBlock.RT.Y][(int)PlayerTmp.QuadBlock.RT.X] == none && map1[(int)PlayerTmp.QuadBlock.RB.Y][(int)PlayerTmp.QuadBlock.RB.X] == none) {
						Player.Speed.X = 4;
						
						if (Scroll >= 1200) {
							Scroll -= Player.Speed.X;
							
						}
						if (Player.Pos.X >= 400) {
							Scroll += Player.Speed.X;
							
						}
					}
				}
			}

			if (map1[(int)PlayerTmp.QuadBlock.RT.Y][(int)PlayerTmp.QuadBlock.RT.X] == 2 && map1[(int)PlayerTmp.QuadBlock.RB.Y][(int)PlayerTmp.QuadBlock.RB.X] == 2) {
				StartFlag = 1;
			}

			//フェードイン
			if (StartFlag == 1) {
				AColor -= 0xFFFFFFFF;
				if (AColor == BLACK) {
					scene = 2;
					StartFlag = 2;
					Player.Pos.X = 96;
					Player.Pos.Y = 576;
				}
			}

			if (keys[DIK_LEFT] && keys[DIK_RIGHT]) {
				Player.Speed.X = 0;
			}
			//実数値移動
			move(Player.Pos, keys, Player.Speed);
			Player.Quad = makeQuad(Player.Pos);
			//ジャンプ
			janpFancAir(Player.Pos, keys, preKeys, JanpFlag, JanpAir, Velocity);
			//ステージの判定
			if (Player.Pos.Y + Player.Radius >= 608) {
				Player.Pos.Y = 608 - Player.Radius;
			}
			
			if (Player.Pos.X + Player.Radius > 2648) {
				Player.Pos.X = 2648 - Player.Radius;
			}
			
			if (Player.Pos.X - Player.Radius < 32) {
				Player.Pos.X = 32 + Player.Radius;
			}
			//アニメフレーム
			FrameCount(PlayerRunFrame, PlayerRunAnime, 10, 60);
			FrameCount(PlayerStayFrame, PlayerStayAnime, 10, 50);
			FrameCount(EnemyStayFrame, EnemyStayAnime, 10, 50);
			FrameCount(PlayerAttackFreme, PlayerAttackAnime, 4, 16);
			FrameCount(AttackEffectFreme, AttackEffectAnime, 2, 14);
			if (AttackFlag == 1) {
				AttackCount++;
			}
			if (AttackCount >= 16) {
				AttackFlag = 0;
				AttackCount = 0;
			}
			if (preKeys[DIK_Z] == 0 && keys[DIK_Z] != 0 && AttackFlag == 0 && AttackEffectFlag == 0) {
				AttackFlag = 1;
				PlayerAttackFreme = 0;
				PlayerAttackAnime = 0;
				EffectPos.X = Player.Pos.X + 16-Scroll;
				EffectPos.Y = Player.Pos.Y - 32;
			}
			if (AttackEffectFlag == 1) {
				AttackEffectCount++;
				EffectPos.X += EffectMove;
			}
			if (AttackEffectCount >= 14) {
				AttackEffectFlag = 0;
			}
			if (AttackCount == 12) {
				AttackEffectFlag = 1;
				AttackEffectAnime = 0;
				AttackEffectFreme = 0;
				AttackEffectCount = 0;
				if (PlayerAnimeFlag == Left) {
					EffectMove = -10;
				}
				else if (PlayerAnimeFlag == Right) {
					EffectMove = 10;
				}
			}
		}

		//仮変数
		if (scene == 2) {
		
			//フェードアウト
			if (StartFlag == 2) {
				AColor += 0xFFFFFFFF;
				if (AColor == 0x00000000) {
					StartFlag = 0;
				}
			}

			if (keys[DIK_LEFT]) {
					PlayerTmp.Speed.X = 4;
					PlayerAnimeFlag = Left;
			}
			else if (keys[DIK_RIGHT]) {
					PlayerTmp.Speed.X = 4;
					PlayerAnimeFlag = Right;
			}
			if (keys[DIK_LEFT] && keys[DIK_RIGHT]) {
				PlayerTmp.Speed.X = 0;
			}
			//仮変数移動
			move(PlayerTmp.Pos, keys, PlayerTmp.Speed);
			PlayerTmp.Quad = makeQuad(PlayerTmp.Pos);
			//実数
			PlayerTmp.QuadBlock.LT = { (PlayerTmp.Quad.LT.X) / BlockSize, (PlayerTmp.Quad.LT.Y) / BlockSize };
			PlayerTmp.QuadBlock.LB = { (PlayerTmp.Quad.LB.X) / BlockSize,(PlayerTmp.Quad.LB.Y - 1) / BlockSize };
			PlayerTmp.QuadBlock.RT = { (PlayerTmp.Quad.RT.X - 1) / BlockSize,(PlayerTmp.Quad.RT.Y) / BlockSize };
			PlayerTmp.QuadBlock.RB = { (PlayerTmp.Quad.RB.X - 1) / BlockSize,(PlayerTmp.Quad.RB.Y - 1) / BlockSize };
			if (keys[DIK_LEFT]) {
				if (keys[DIK_S]) {
					if (map[(int)PlayerTmp.QuadBlock.LT.Y][(int)PlayerTmp.QuadBlock.LT.X] == none && map[(int)PlayerTmp.QuadBlock.LB.Y - 1][(int)PlayerTmp.QuadBlock.LB.X] == none) {
						Player.Speed.X = 2;
					}
				}
				else {
					if (map[(int)PlayerTmp.QuadBlock.LT.Y][(int)PlayerTmp.QuadBlock.LT.X] == none && map[(int)PlayerTmp.QuadBlock.LB.Y][(int)PlayerTmp.QuadBlock.LB.X] == none) {
						Player.Speed.X = 4;
					}
				}

			}
			else if (keys[DIK_RIGHT]) {

				if (keys[DIK_S]) {
					if (map[(int)PlayerTmp.QuadBlock.RT.Y][(int)PlayerTmp.QuadBlock.RT.X] == none && map[(int)PlayerTmp.QuadBlock.RB.Y - 1][(int)PlayerTmp.QuadBlock.RB.X] == none) {
						Player.Speed.X = 2;
					}
				}
				else {
					if (map[(int)PlayerTmp.QuadBlock.RT.Y][(int)PlayerTmp.QuadBlock.RT.X] == none && map[(int)PlayerTmp.QuadBlock.RB.Y][(int)PlayerTmp.QuadBlock.RB.X] == none) {
						Player.Speed.X = 4;
					}
				}
			}
			if (keys[DIK_LEFT] && keys[DIK_RIGHT]) {
				Player.Speed.X = 0;
			}
			//実数値移動
			move(Player.Pos, keys, Player.Speed);
			Player.Quad = makeQuad(Player.Pos);
			//ジャンプ
			janpFancAir(Player.Pos, keys, preKeys, JanpFlag, JanpAir, Velocity);
			//ステージの判定
			if (Player.Pos.Y + Player.Radius >= 608) {
				Player.Pos.Y = 608 - Player.Radius;
			}
			if (Player.Pos.X + Player.Radius > 1248) {
				Player.Pos.X = 1248 - Player.Radius;
			}
			if (Player.Pos.X - Player.Radius < 32) {
				Player.Pos.X = 32 + Player.Radius;
			}

			//レーザー攻撃
			if (Boss.LaserFlag == 4){
				
				for (int i = 0; i < 8; i++) {
					Boss.LaserPosX[i] = -100;
					Boss.LaserPosY[i] = -100;
				}
			}
			if (Boss.LaserFlag == 1) {
				Boss.LaserPosX[0] = Boss.Pos.X - 155;
				Boss.LaserPosY[0] = Boss.Pos.Y - 90;
				Boss.LaserRadiusX[0] = 100;
			}
			else if(Boss.LaserRadiusX[0] > 0 && Boss.LaserRadiusY[0] > 0) {
				if (Boss.LaserRadiusY[0] <= 0) {
					Boss.LaserRadiusX[0] = 0;
					Boss.LaserRadiusY[0] = 0;
				}
			}
			if (Boss.LaserFlag == 2) {
				Boss.LaserPosX[1] = Boss.Pos.X - 155;
				Boss.LaserPosY[1] = Boss.Pos.Y - 90;
				Boss.LaserRadiusX[1] = 100;
			}
			else if (Boss.LaserRadiusX[1] > 0 && Boss.LaserRadiusY[1] > 0) {
				if (Boss.LaserRadiusY[1] <= 0) {
					Boss.LaserRadiusX[1] = 0;
					Boss.LaserRadiusY[1] = 0;
				}
			}
			if (Boss.LaserFlag == 3) {
				Boss.LaserPosX[2] = Boss.Pos.X - 155;
				Boss.LaserPosY[2] = Boss.Pos.Y - 90;
				Boss.LaserRadiusX[2] = 100;
			}
			else if (Boss.LaserRadiusX[2] > 0 && Boss.LaserRadiusY[2] > 0) {
				Boss.LaserRadiusY[1] -= 5;
				Boss.LaserRadiusY[2] -= 5;
				Boss.LaserRadiusY[0] -= 5;
				if (Boss.LaserRadiusY[2] <= 0) {
					Boss.LaserRadiusX[2] = 0;
					Boss.LaserRadiusY[2] = 0;
					Boss.LaserFlag = 4;
				}
			}

			//雨レーザー
			if (Boss.RainLaserFlag == 1) {
				Boss.LaserFlag = 0;
				if (Boss.ColorChange == 0) {
					Color += ColorVal;
					Boss.Pos.X = 600;
				}
				if (Color >= -1) {
					Boss.ColorChange = 2;
				}

				Boss.LaserPosX[0] = 100;
				Boss.LaserPosX[1] = 300;
				Boss.LaserPosX[2] = 500;
				Boss.LaserPosX[3] = 700;
				Boss.LaserPosX[4] = 900;
				Boss.LaserPosX[5] = 1100;
				Boss.LaserTimer += 1;

				for (int i = 0; i < 6; i++) {
					Boss.LaserPosY[i] = 0;
				}

				if (Boss.LaserTimer == 20) {
					Boss.Danger = 1;
				}
				if (Boss.LaserTimer == 40) {
					Boss.Danger = 0;
				}
				if (Boss.LaserTimer == 60) {
					Boss.Danger = 1;
				}
				if (Boss.LaserTimer >= 80) {

					for (int i = 0; i < 6; i++) {
						Boss.LaserRadiusY[i] += 5;

						if (Boss.LaserRadiusY[i] >= 700) {
							Boss.RainLaserFlag = 2;
						}
					}
				}
			}

			if (Boss.RainLaserFlag == 2) {
				for (int i = 0; i < 6; i++) {
					Boss.LaserRadiusY[i] -= 10;
					if (Boss.LaserRadiusY[i] <= 0) {
						Boss.RainLaserFlag = 3;
						Boss.LaserTimer = 0;
					}
				}
			}

			if (Boss.RainLaserFlag == 3) {
				Boss.LaserPosX[0] = 0;
				Boss.LaserPosX[1] = 200;
				Boss.LaserPosX[2] = 400;
				Boss.LaserPosX[3] = 600;
				Boss.LaserPosX[4] = 800;
				Boss.LaserPosX[5] = 1000;
				Boss.LaserPosX[6] = 1200;
				Boss.LaserTimer += 1;

				for (int i = 0; i < 7; i++) {
					Boss.LaserPosY[i] = 0;
				}

				if (Boss.LaserTimer == 20) {
					Boss.Danger = 1;
				}
				if (Boss.LaserTimer == 40) {
					Boss.Danger = 0;
				}
				if (Boss.LaserTimer == 60) {
					Boss.Danger = 1;
				}
				if (Boss.LaserTimer >= 80) {

					for (int i = 0; i < 7; i++) {
						Boss.LaserRadiusY[i] += 5;

						if (Boss.LaserRadiusY[i] >= 700) {
							Boss.RainLaserFlag = 4;
						}
					}
				}
			}

			if (Boss.RainLaserFlag == 4) {
				for (int i = 0; i < 7; i++) {
					Boss.LaserRadiusY[i] -= 10;
					if (Boss.LaserRadiusY[i] <= 0) {

						Boss.LaserTimer = 0;
						Boss.ColorChange = 1;
					}
				}
				
				if (Boss.ColorChange == 1) {
					Color -= ColorVal;
				}
				if (Color <= -255) {
					Boss.ColorChange = 2;
					for (int i = 0; i < 7; i++) {
						Boss.LaserRadiusY[i] = 0;
						Boss.LaserRadiusX[i] = 0;
						Boss.LaserPosX[i] = 0;
						Boss.LaserPosY[i] = 0;
					}
					if (Boss.Move == 1) {
						Boss.Pos.X = Boss.Copy.X;
						Boss.Pos.Y = Boss.Copy.Y;
						if (Boss.Pos.X >= 1150) {
							Boss.Move = 2;
							Attack4.Shotflag = 6;
							Boss.RainLaserFlag = 0;
							Boss.Down = 0;
							Boss.Warp = 0;
							Boss.LaserFlag = 4;
						}
					}
					if (Boss.Move == 2) {
						Boss.Pos.X = Boss.Copy.X;
						Boss.Pos.Y = Boss.Copy.Y;
						if (Boss.Pos.X <= 150) {
							Boss.Move = 1;
							Attack4.Shotflag = 6;
							Boss.RainLaserFlag = 0;
							Boss.Down = 0;
							Boss.Warp = 0;
							Boss.LaserFlag = 4;
						}
					}
				}
			}

			//ボス登場シーン
			if (StartFlag == 0) {
				if (Boss.Start == 0) {

					Boss.Pos.Y += 1;
					WorldRandX = rand() % 10 - 5;
					WorldRandY = rand() % 10 - 5;
					PlayerRandX = rand() % 10 - 5;

					if (Boss.Pos.Y == 145) {
						Boss.Start = 1;
						WorldRandX = 0;
						WorldRandY = 0;
						PlayerRandX = 0;
						PlayerRandY = 0;
					}
				}
			}

			if (Boss.Start == 1) {
				Boss.StartTimer += 1;
				if (Boss.StartTimer == 50) {
					Boss.Start = 2;
				}
			}

			//ボスの動き実装部分-----------------------------------
			if (Boss.Start == 2) {
				if (Attack4.Shotflag == 0) {
					if (Boss.Down == 0) {
						if (Boss.Pos.X >= 150 && Boss.Pos.X <= 1130) {
							if (Boss.RandTimer <= 20) {
								Boss.RandTimer += 1;
							}

							if (Boss.RandTimer >= 20) {
								RandX[0] =rand() % 12 + 1;
								Boss.RandTimer += 1;
								if (Boss.RandTimer == 50) {
									Boss.RandTimer = 0;
								}
							}
						}

						if (Boss.Move == 1) {
							t += 0.003;
							Boss.Pos.X += easeInQuad(t, b, c, d);

							Boss.Pos.Y = sinf(Theta) * amplitude + 150;
							Theta += M_PI / 180.0f;
						}

						if (Boss.Move == 2) {
							t += 0.003;
							Boss.Pos.X -= easeInQuad(t, b, c, d);

							Boss.Pos.Y = sinf(Theta) * amplitude + 150;
							Theta += M_PI / 180.0f;
						}
					}
				}

				//たたきつけ
				if (Boss.Down == 1) {
					if (Boss.ColorChange == 0) {
						Color += ColorVal;
						
					}
					if (Color >= -1) {
						Boss.ColorChange = 1;
					}
					Boss.Pos.X = Player.Pos.X;
					Boss.Pos.Y = 100;
					
					if (Color >= -1) {
						Boss.Down = 2;
						Boss.Vel = 7;
					}
				}

				if (Boss.Down == 2) {
					Boss.Pos.Y -= Boss.Vel;
					Boss.Vel -= 0.7f;
					if (Boss.Pos.Y >= 554) {
						Boss.Pos.Y = Boss.Pos.Y + Boss.Vel - 5.1;
						Boss.Down = 3;
						
					}
				}

				if (Boss.Down == 3) {
					Boss.DownTimer += 1;
					if (Boss.DownTimer <= 5) {
						WorldRandX = rand() % 10 - 5;
						WorldRandY = rand() % 10 - 5;
					}
					else {
						WorldRandX = 0;
						WorldRandY = 0;
					}
					if (Boss.DownTimer >= 30) {
						Color -= ColorVal;
						if (Color <= -255) {
							Boss.Down = 4;
							Boss.DownTimer = 0;
							Boss.ColorChange = 0;
						}
					}
				}

				if (Boss.Down == 4) {
					if (Boss.ColorChange == 0) {
						Color += ColorVal;
					}
					if (Color >= -1) {
						Boss.ColorChange = 1;
					}
					Boss.Pos.X = Player.Pos.X;
					Boss.Pos.Y = 100;
					
					if (Color >= -1) {
						Boss.Down = 5;
						Boss.Vel = 7;
					}
				}

				if (Boss.Down == 5) {
					Boss.Pos.Y -= Boss.Vel;
					Boss.Vel -= 0.7f;
					if (Boss.Pos.Y >= 554) {
						Boss.Pos.Y = Boss.Pos.Y + Boss.Vel - 5.1;
						Boss.Down = 6;
					}
				}

				if (Boss.Down == 6) {
					Boss.DownTimer += 1;
					if (Boss.DownTimer <= 5) {
						WorldRandX = rand() % 10 - 5;
						WorldRandY = rand() % 10 - 5;
					}
					else {
						WorldRandX = 0;
						WorldRandY = 0;
					}
					if (Boss.DownTimer >= 30) {
						Color -= ColorVal;
						Boss.ColorChange = 2;
						if (Color <= -255) {
							Boss.Down = 0;
							Boss.DownTimer = 0;

							if (Boss.Move == 1) {
								Boss.Pos.X = Boss.Copy.X;
								Boss.Pos.Y = Boss.Copy.Y;
								if (Boss.Pos.X >= 1150) {
									Boss.Move = 2;
									Attack4.Shotflag = 6;
									Boss.Warp = 0;

								}
							}
							if (Boss.Move == 2) {
								Boss.Pos.X = Boss.Copy.X;
								Boss.Pos.Y = Boss.Copy.Y;
								if (Boss.Pos.X <= 150) {
									Boss.Move = 1;
									Attack4.Shotflag = 6;
									Boss.Warp = 0;
								}
							}
						}
					}
				}

				
				Attack4.Quad[0] = makeQuad(Attack4.Pos[0]);
				
				if (InvincibleFlag == 0) {
					//弾の判定
					for (int i = 0; i < 4; i++) {
						if (QuadCollision(Attack4.Pos[i], Attack4.Radius, Attack4.Radius, Player.Pos, Player.Radius, Player.Radius) == 1) {
							InvincibleFlag = 1;
							Damage = 1;
							PlayerHP = PlayerHP - Damage;
							
						}
					}

					//ボス本体の判定
					if (QuadCollision(Boss.Pos, Boss.Radius, Boss.Radius, Player.Pos, Player.Radius, Player.Radius) == 1) {
						InvincibleFlag = 1;
						Damage = 2;
						PlayerHP = PlayerHP - Damage;

					}

					//レーザーの判定
					for (int i = 0; i < 8; i++) {
						if (Player.Pos.X + 20 >= Boss.LaserPosX[i] && Player.Pos.X <= Boss.LaserPosX[i] + 120) {
							if (Player.Pos.Y <= Boss.LaserRadiusY[i] + Boss.LaserPosY[i]) {
								InvincibleFlag = 1;
								Damage = 1;
								PlayerHP = PlayerHP - Damage;
								
							}
						}
					}
				}

				//無敵時間
				if (InvincibleFlag == 1) {
					InvincibleTimer += 1;
					PlayerRandX = rand() % 10 - 5;
					if (InvincibleTimer >= 20) {
						InvincibleFlag = 2;
						InvincibleTimer = 0;
					}
				}

				if (InvincibleFlag == 2|| InvincibleFlag == 3) {
					InvincibleTimer += 1;
					if (InvincibleTimer == 5) {
						InvincibleFlag = 3;
					}
					if (InvincibleTimer == 10) {
						InvincibleFlag = 2;
					}
					if (InvincibleTimer == 15) {
						InvincibleFlag = 3;
					}
					if (InvincibleTimer == 25) {
						InvincibleFlag = 2;
					}
					if (InvincibleTimer == 30) {
						InvincibleFlag = 3;
					}
					if (InvincibleTimer == 35) {
						InvincibleFlag = 2;
					}
					if (InvincibleTimer == 40) {
						InvincibleFlag = 3;
					}
					if (InvincibleTimer == 45) {
						InvincibleFlag = 2;
					}
					if (InvincibleTimer == 50) {
						InvincibleFlag = 0;
					}
				}

				//ボスが右にいるとき
				if (Boss.Down == 0) {
					if (Boss.Move == 1) {
						if (Boss.Warp == 0) {
							if (Boss.ColorChange == 2) {
								Color += ColorVal;
							}
							if (Color >= -1) {
								Boss.ColorChange = 1;
								Boss.Timer = 0;
								Attack4.Shotflag = 0;
							}
						}

						if (Boss.Pos.X >= 1150) {
							//弾攻撃
							if (RandX[0] == 2 || RandX[0] == 3
								|| RandX[0] == 6) {
								if (Attack4.Shotflag == 0) {
									Attack4.Pos[0].X = Boss.Pos.X + 25;//ボスの位置
									Attack4.Pos[0].Y = Boss.Pos.Y + 25;
									Attack4.Shotflag = 1;
									Attack4.Speed.X = 8;//弾のスピード
									Attack4.Speed.Y = 8;
									Attack4.Radius = 64;//弾の大きさ
									for (int i = 0; i < 4; i++) {
										Attack4Tmp.Pos[i].X = Attack4.Pos[0].X;
										Attack4Tmp.Pos[i].Y = Attack4.Pos[0].Y;
									}
									Boss.Warp = 5;
									Boss.Down = 9;
									t = 0;
								}
							}
							//ボスワープ
							if (Boss.Warp == 0) {
								for (int i = 0; i < 3; i++) {
									LaserRand[i] = rand() % 1000 + 100;
								}
								if (RandX[0] == 4 || RandX[0] == 5 || RandX[0] == 12) {
									Attack4.Shotflag = 5;
									Boss.Copy.X = Boss.Pos.X;
									Boss.Copy.Y = Boss.Pos.Y;
									Color -= ColorVal;


									if (Color <= -255) {
										
										Boss.Warp = 1;
										Boss.ColorChange = 0;
										Boss.Down = 9;
										t = 0;
										
									}
								}
							}
							
							//まわる攻撃
							if (RandX[0] == 1) {
								for (int i = 0; i < Attack3Max; i++) {
									if (Attack3.ON[Attack3Max - 1] == false) {
										Attack3CoolTime[i] = 10;
										Attack4.Shotflag = 5;
										Boss.Warp = 5;
										Boss.Down = 9;
										t = 0;
									}
								}
							}

							//たたきつけ
							if (Boss.Down == 0) {
								if (RandX[0] == 7 || RandX[0] == 10 || RandX[0] == 11) {
									Attack4.Shotflag = 5;
									Boss.Copy.X = Boss.Pos.X;
									Boss.Copy.Y = Boss.Pos.Y;
									Color -= ColorVal;
									//Boss.Radius -= 4;
									t = 0;
									if (Color<=-255) {
										Boss.Down = 1;
										Boss.ColorChange = 0;
									}
								}
							}

							//雨レーザー
							if (Boss.RainLaserFlag == 0) {
								if (RandX[0] == 9 || RandX[0] == 8) {
									
									Boss.Copy.X = Boss.Pos.X;
									Boss.Copy.Y = Boss.Pos.Y;
									
									t = 0;
									Color -= ColorVal;
									if (Color <= -255) {
										Boss.RainLaserFlag = 1;
										Boss.ColorChange = 0;
										Attack4.Shotflag = 5;
										Boss.Warp = 5;
										Boss.Down = 9;
									}
								}
							}
						}
					}

					//ボスが左にいるとき
					if (Boss.Move == 2) {
						if (Boss.Warp == 0) {
							if (Boss.ColorChange == 2) {
								Color += ColorVal;
							}
							if (Color >= -1) {
								Boss.ColorChange = 1;
								Boss.Timer = 0;
								Attack4.Shotflag = 0;
							}
						}
						if (Boss.Pos.X <= 150) {

							//弾攻撃
							if (RandX[0] == 2 || RandX[0] == 3
								|| RandX[0] == 6 ) {
								if (Attack4.Shotflag == 0) {
									Attack4.Pos[0].X = Boss.Pos.X + 25;//ボスの位置
									Attack4.Pos[0].Y = Boss.Pos.Y + 25;
									Attack4.Shotflag = 1;
									Attack4.Speed.X = 8;//弾のスピード
									Attack4.Speed.Y = 8;
									Attack4.Radius = 64;//弾の大きさ
									for (int i = 0; i < 4; i++) {
										Attack4Tmp.Pos[i].X = Attack4.Pos[0].X;
										Attack4Tmp.Pos[i].Y = Attack4.Pos[0].Y;
									}
									Boss.Warp = 5;
									Boss.Down = 9;
									t = 0;
								}
							}
							//ボスワープ
							if (Boss.Warp == 0) {
								for (int i = 0; i < 3; i++) {
									LaserRand[i] = rand() % 1000 + 100;
								}
								if (RandX[0] == 4 || RandX[0] == 5 || RandX[0] == 12) {
									Boss.Copy.X = Boss.Pos.X;
									Boss.Copy.Y = Boss.Pos.Y;
									Color -= ColorVal;
									Attack4.Shotflag = 5;

									if (Color <= -255) {
										
										Boss.Warp = 1;
										Boss.ColorChange = 0;
										Boss.Down = 9;
										t = 0;
										
									}
								}
							}
							//まわる攻撃
							if (RandX[0] == 1) {
								for (int i = 0; i < Attack3Max; i++) {
									if (Attack3.ON[Attack3Max - 1] == false) {
										Attack3CoolTime[i] = 10;
										Attack4.Shotflag = 5;
										Boss.Warp = 5;
										Boss.Down = 9;
										t = 0;
									}
								}
							}
							//たたきつけ
							if (Boss.Down == 0) {
								if (RandX[0] == 7 || RandX[0] == 10 || RandX[0] == 11) {
									Attack4.Shotflag = 5;
									Boss.Copy.X = Boss.Pos.X;
									Boss.Copy.Y = Boss.Pos.Y;
									Color -= ColorVal;
									//Boss.Radius -= 4;
									t = 0;
									if (Color <= -255) {
										Boss.Down = 1;
										Boss.ColorChange = 0;
									}
								}
							}
							//雨レーザー
							if (Boss.RainLaserFlag == 0) {
								if (RandX[0] == 9 || RandX[0] == 8) {

									Boss.Copy.X = Boss.Pos.X;
									Boss.Copy.Y = Boss.Pos.Y;

									t = 0;
									Color -= ColorVal;
									if (Color <= -255) {
										Boss.RainLaserFlag = 1;
										Boss.ColorChange = 0;

										Attack4.Shotflag = 5;
										Boss.Warp = 5;
										Boss.Down = 9;
									}
								}
							}
						}
					}
				}

				//ボスワープ
				//右スタート
				if (Boss.Move == 2) {
					if (Boss.Warp == 1) {
						if (Boss.ColorChange == 0) {
							Color += ColorVal;
						}
						Boss.Pos.X = LaserRand[0];
						Boss.Pos.Y = 120;

						if (Color >= -1) {
							Boss.ColorChange = 3;
							Boss.LaserFlag = 1;
							Boss.LaserRadiusY[0] += 20;
						}

						if (Boss.LaserRadiusY[0] >= 700) {
							Boss.ColorChange = 1;
							Boss.LaserFlag = 0;
						}

						if (Boss.ColorChange == 1) {
							Color -= ColorVal;
						}
						if (Color <= -255) {
							Boss.Warp = 2;
							Boss.ColorChange = 0;
							LaserRand[1] = rand() % 1000 + 100;
						}
					}

					if (Boss.Warp == 2) {
						if (Boss.ColorChange == 0) {
							Color += ColorVal;
						}
						Boss.Pos.X = LaserRand[1];
						Boss.Pos.Y = 120;

						if (Color >= -1) {
							Boss.ColorChange = 3;
							Boss.LaserFlag = 2;
							Boss.LaserRadiusY[1] += 20;
						}
						if (Boss.LaserRadiusY[1] >= 700) {
							Boss.ColorChange = 1;
							Boss.LaserFlag = 0;
						}
						if (Boss.ColorChange == 1) {
							Color -= ColorVal;
						}
						if (Color <= -255) {
							Boss.Warp = 3;
							Boss.ColorChange = 0;
							LaserRand[2] = rand() % 1000 + 100;
						}
					}

					if (Boss.Warp == 3) {
						if (Boss.ColorChange == 0) {
							Color += ColorVal;
						}
						Boss.Pos.X = LaserRand[2];
						Boss.Pos.Y = 120;

						if (Color >= -1) {
							Boss.ColorChange = 3;
							Boss.LaserFlag = 3;
							Boss.LaserRadiusY[2] += 20;
						}
						if (Boss.LaserRadiusY[2] >= 700) {
							Boss.ColorChange = 1;
							Boss.LaserFlag = 0;
						}

						if (Boss.ColorChange == 1) {
							Color -= ColorVal;
						}
						if (Color <= -255) {
							Boss.ColorChange = 2;
							if (Boss.Move == 1) {
								Boss.Pos.X = Boss.Copy.X;
								Boss.Pos.Y = Boss.Copy.Y;
								if (Boss.Pos.X >= 1150) {
									Boss.Move = 2;
									Attack4.Shotflag = 6;
									Boss.Down = 0;
									Boss.Warp = 0;
								}
							}
							if (Boss.Move == 2) {
								Boss.Pos.X = Boss.Copy.X;
								Boss.Pos.Y = Boss.Copy.Y;
								if (Boss.Pos.X <= 150) {
									Boss.Move = 1;
									Attack4.Shotflag = 6;
									Boss.Down = 0;
									Boss.Warp = 0;
								}
							}
						}
					}
				}

				//左スタート
				if (Boss.Move == 1) {
					if (Boss.Warp == 1) {
						if (Boss.ColorChange == 0) {
							Color += ColorVal;
						}
						Boss.Pos.X = LaserRand[0];
						Boss.Pos.Y = 120;

						if (Color >= -1) {
							Boss.ColorChange = 3;
							Boss.LaserFlag = 1;
							Boss.LaserRadiusY[0] += 20;
						}

						if (Boss.LaserRadiusY[0] >= 700) {
							Boss.ColorChange = 1;
							Boss.LaserFlag = 0;
						}

						if (Boss.ColorChange == 1) {
							Color -= ColorVal;
						}
						if (Color <= -255) {
							Boss.Warp = 2;
							Boss.ColorChange = 0;
							LaserRand[1] = rand() % 1000 + 100;
						}
					}

					if (Boss.Warp == 2) {
						if (Boss.ColorChange == 0) {
							Color += ColorVal;
						}
						Boss.Pos.X = LaserRand[1];
						Boss.Pos.Y = 120;

						if (Color >= -1) {
							Boss.ColorChange = 3;
							Boss.LaserFlag = 2;
							Boss.LaserRadiusY[1] += 20;
						}
						if (Boss.LaserRadiusY[1] >= 700) {
							Boss.ColorChange = 1;
							Boss.LaserFlag = 0;
						}
						if (Boss.ColorChange == 1) {
							Color -= ColorVal;
						}
						if (Color <= -255) {
							Boss.Warp = 3;
							Boss.ColorChange = 0;
							LaserRand[2] = rand() % 1000 + 100;
						}
					}

					if (Boss.Warp == 3) {
						if (Boss.ColorChange == 0) {
							Color += ColorVal;
						}
						Boss.Pos.X = LaserRand[2];
						Boss.Pos.Y = 120;

						if (Color >= -1) {
							Boss.ColorChange = 3;
							Boss.LaserFlag = 3;
							Boss.LaserRadiusY[2] += 20;
						}
						if (Boss.LaserRadiusY[2] >= 700) {
							Boss.ColorChange = 1;
							Boss.LaserFlag = 0;
						}

						if (Boss.ColorChange == 1) {
							Color -= ColorVal;
						}
						if (Color <= -255) {
							Boss.ColorChange = 2;
							if (Boss.Move == 1) {
								Boss.Pos.X = Boss.Copy.X;
								Boss.Pos.Y = Boss.Copy.Y;
								if (Boss.Pos.X >= 1150) {
									Boss.Move = 2;
									Attack4.Shotflag = 6;
									Boss.Down = 0;
									Boss.Warp = 0;
								}
							}
							if (Boss.Move == 2) {
								Boss.Pos.X = Boss.Copy.X;
								Boss.Pos.Y = Boss.Copy.Y;
								if (Boss.Pos.X <= 150) {
									Boss.Move = 1;
									Attack4.Shotflag = 6;
									Boss.Down = 0;
									Boss.Warp = 0;
								}
							}
						}
					}
				}

				if (Attack4.Shotflag == 1) {
					//ボスが右にいるとき
					if (Boss.Move == 1) {
						if (Boss.Pos.X >= 1150) {
							Attack4.Pos[0].X -= Attack4.Speed.X;
							Attack4.Pos[0].Y += Attack4.Speed.Y;
						}
					}

					//ボスが左にいるとき
					if (Boss.Move == 2) {
						if (Boss.Pos.X <= 150) {
							Attack4.Pos[0].X += Attack4.Speed.X;
							Attack4.Pos[0].Y += Attack4.Speed.Y;
						}
					}

					Attack4Tmp.QuadBlock[0].LB.X = (Attack4.Quad[0].LB.X) / BlockSize;
					Attack4Tmp.QuadBlock[0].LB.Y = (Attack4.Quad[0].LB.Y - 1) / BlockSize;
					Attack4Tmp.QuadBlock[0].RB.X = (Attack4.Quad[0].RB.X - 1) / BlockSize;
					Attack4Tmp.QuadBlock[0].RB.Y = (Attack4.Quad[0].RB.Y - 1) / BlockSize;
					for (int i = 1; i < 4; i++) {
						RandY[i] = rand() % 9 + 7;//弾を上に飛ばす処理
						RandX[i] = rand() % 7 + 2;//弾の広がり幅
					}
					if (map[(int)Attack4Tmp.QuadBlock[0].LB.Y][(int)Attack4Tmp.QuadBlock[0].LB.X] == Block &&
						map[(int)Attack4Tmp.QuadBlock[0].RB.Y][(int)Attack4Tmp.QuadBlock[0].RB.X] == Block) {
						Attack4.Shotflag = 2;
						for (int i = 1; i < 4; i++) {
							Attack4.Pos[i].X = Attack4.Pos[0].X;
							Attack4.Pos[i].Y = Attack4.Pos[0].Y;
						}
						Attack4.Vel = 0.3f;
						Attack4.Radius = 32;
					}
				}

				if (Attack4.Shotflag == 2) {

					Attack4.Pos[0].X = 600;
					Attack4.Pos[0].Y = 100;
					Attack4.Pos[2].X += RandX[2];
					Attack4.Pos[3].X -= RandX[3];
					Attack4.Timer += 1;
					//玉があたったら判定をとる
					for (int i = 1; i < 4; i++) {
						Attack4.Quad[i] = makeQuad(Attack4.Pos[i]);
						Attack4.Pos[i].Y -= RandY[i];
						RandY[i] -= Attack4.Vel;
					}

					if (Attack4.Timer >= 10) {
						for (int i = 1; i < 4; i++) {
							Attack4Tmp.QuadBlock[i].LB.X = (Attack4.Quad[i].LB.X) / BlockSize;
							Attack4Tmp.QuadBlock[i].LB.Y = (Attack4.Quad[i].LB.Y - 1) / BlockSize;
							Attack4Tmp.QuadBlock[i].RB.X = (Attack4.Quad[i].RB.X - 1) / BlockSize;
							Attack4Tmp.QuadBlock[i].RB.Y = (Attack4.Quad[i].RB.Y - 1) / BlockSize;
						}

						if (map[(int)Attack4Tmp.QuadBlock[1].LB.Y][(int)Attack4Tmp.QuadBlock[1].LB.X] == Block ||
							map[(int)Attack4Tmp.QuadBlock[1].RB.Y][(int)Attack4Tmp.QuadBlock[1].RB.X] == Block) {
							Attack4.Delete[1] = 1;
						}

						if (map[(int)Attack4Tmp.QuadBlock[2].LB.Y][(int)Attack4Tmp.QuadBlock[2].LB.X] == Block ||
							map[(int)Attack4Tmp.QuadBlock[2].RB.Y][(int)Attack4Tmp.QuadBlock[2].RB.X] == Block) {
							Attack4.Delete[2] = 1;
						}

						if (map[(int)Attack4Tmp.QuadBlock[3].LB.Y][(int)Attack4Tmp.QuadBlock[3].LB.X] == Block ||
							map[(int)Attack4Tmp.QuadBlock[3].RB.Y][(int)Attack4Tmp.QuadBlock[3].RB.X] == Block) {
							Attack4.Delete[3] = 1;
						}

						//全部落ちたら終了
						if (Attack4.Delete[1] == 1 && Attack4.Delete[2] == 1 && Attack4.Delete[3] == 1) {
							Attack4.Shotflag = 3;
						}
					}
				}

				//地面についたら戻す
				if (Attack4.Shotflag == 3) {
					Attack4.Shotflag = 0;
					Attack4.Speed.X = 2;
					Attack4.Speed.Y = 4;
					for (int i = 0; i < 4; i++) {
						Attack4Tmp.Pos[i].X = Attack4.Pos[0].X;
						Attack4Tmp.Pos[i].Y = Attack4.Pos[0].Y;
						Attack4.Pos[i].X = Attack4.Pos[0].X;
						Attack4.Pos[i].Y = Attack4.Pos[0].Y;
						Attack4.Delete[i] = 0;
					}
					Boss.Warp = 0;
					Attack4.Timer = 0;
					Boss.Down = 0;

					if (Boss.Move == 1) {
						if (Boss.Pos.X >= 1150) {
							Boss.Move = 2;
						}
					}
					if (Boss.Move == 2) {
						if (Boss.Pos.X <= 150) {
							Boss.Move = 1;
						}
					}
				}
			}

			if (Attack4.ReShotflag == 1) {
				//ボスが右にいるとき
				if (Boss.Move == 1) {
					if (Boss.Pos.X >= 1150) {
						Attack4.RePos[0].X -= Attack4.ReSpeed.X;
						Attack4.RePos[0].Y += Attack4.ReSpeed.Y;
					}
				}

				//ボスが左にいるとき
				if (Boss.Move == 2) {
					if (Boss.Pos.X <= 150) {
						Attack4.RePos[0].X += Attack4.ReSpeed.X;
						Attack4.RePos[0].Y += Attack4.ReSpeed.Y;
					}
				}
			}

			Attack4Tmp.Quad = makeQuad(Attack4.Pos[0]);
			Attack4.ReQuad[0] = makeQuad(Attack4.RePos[0]);
			//ここまで--------------------------------------

			//-敵の攻撃３更新処理---------
			for (int i = 0; i < Attack3Max; i++) {
				

				
				if (Attack3CoolTime[i] >= 10) {
					Attack3.ON[i] = true;
					int a = i * 60; //角度
					Attack3.Theta[i] = degreeToRadian(a);
					Attack3CoolTime[i] = 0;
					Attack3.Attack[i] = false;
					Attack3.Time[i] = 0;
					Attack3.Color[i] = BLUE;

				}
				if (Attack3.ON[i] == true && Attack3.Attack[i] == false) {
					Attack3.Theta[i] -= 0.05; //回転速度と方向(+-)
					Attack3.Pos[i].X = (100 * cosf(Attack3.Theta[i]) - 100 * sinf(Attack3.Theta[i])) + Player.Pos.X;
					Attack3.Pos[i].Y = (100 * cosf(Attack3.Theta[i]) + 100 * sinf(Attack3.Theta[i])) + Player.Pos.Y;
					Attack3.Time[i]++;
				}
				if (Attack3.Time[i] >= 240 && Attack3.Attack[i] == false && Attack3.ON[i] == true && Attack3.Check == 0) {
					Attack3.Attack[i] = true;
					Attack3.Target[i].X = Player.Pos.X;
					Attack3.Target[i].Y = Player.Pos.Y;
					Attack3.Start[i].X = Attack3.Pos[i].X;
					Attack3.Start[i].Y = Attack3.Pos[i].Y;
					Attack3.Time[i] = 0;
					Attack3.t[i] = 0;
					Attack3.Color[i] = WHITE;
					Attack3.Check += 10 + i; //下のcheckと同じ数足す
					Attack3.tMove = 0.10;
				}
				if (Attack3.Attack[i] == true) {
					Attack3.AttackMove++;
					
					//弾の判定
					if (InvincibleFlag == 0) {
						if (QuadCollision(Attack3.Pos[i], 10, 10, Player.Pos, Player.Radius, Player.Radius) == 1) {
							InvincibleFlag = 1;
							Damage = 1;
							PlayerHP = PlayerHP - Damage;
						}
					}

					if (Attack3.AttackMove < 10) {
						//演出っぽいもの
						Attack3.t[i] -= Attack3.tMove;
						Attack3.tMove -= 0.01;
					}
					else {
						Attack3.t[i] += Attack3.tMove; //弾の移動速度にあたる
						Attack3.tMove += 0.005;
					}
					Attack3.Pos[i].X = (1.0 - Attack3.t[i]) * Attack3.Start[i].X + Attack3.t[i] * Attack3.Target[i].X;
					Attack3.Pos[i].Y = (1.0 - Attack3.t[i]) * Attack3.Start[i].Y + Attack3.t[i] * Attack3.Target[i].Y;
					if ((Attack3.Pos[i].X >= 900 || Attack3.Pos[i].X <= -100) || (Attack3.Pos[i].Y >= 740 || Attack3.Pos[i].Y < -100)) {
						Attack3.Attack[i] = false;
						Attack3.ON[i] = false;
						Attack3.Check -= 10 + i; //上のcheckと同じ数引く
						Attack3.AttackMove = 0;
						
					}
					
					if (Attack3.ON[5] == false && Attack3.Attack[5] == false) {
						if (Boss.Move == 1) {
							if (Boss.Pos.X >= 1150) {
								Boss.Move = 2;
								Attack4.Shotflag = 6;
								Boss.Warp = 0;
								Boss.Down = 0;
							}
						}
						if (Boss.Move == 2) {
							if (Boss.Pos.X <= 150) {
								Boss.Move = 1;
								Attack4.Shotflag = 6;
								Boss.Warp = 0;
								Boss.Down = 0;
							}
						}
					}
				}
			}
			//-敵の攻撃３更新処理ここまで-----------
		//アニメフレーム
			FrameCount(PlayerRunFrame,PlayerRunAnime,10,60);
			FrameCount(PlayerStayFrame, PlayerStayAnime,10,50);
			FrameCount(EnemyStayFrame,EnemyStayAnime,10,50);
			FrameCount(PlayerAttackFreme,PlayerAttackAnime,4,16);
			FrameCount(AttackEffectFreme,AttackEffectAnime,2,14);
			//プレイヤーの攻撃とアニメ-------------------------
			if (AttackFlag == 1){
				AttackCount++;
			}
			if (AttackCount >= 16) {
				AttackFlag = 0;
				AttackCount = 0;
			}
			if (preKeys[DIK_Z]==0&&keys[DIK_Z]!=0&&AttackFlag==0&&AttackEffectFlag==0) {
				AttackFlag = 1;
				PlayerAttackFreme = 0;
				PlayerAttackAnime = 0;
				
			}
			if (AttackEffectFlag == 1) {
				AttackEffectCount++;
				EffectPos.X += EffectMove;
			}
			if (AttackEffectCount >= 14) {
				AttackEffectFlag = 0;
			}
			if (AttackCount == 10) {
				AttackEffectFlag = 1;
				AttackEffectAnime = 0;
				AttackEffectFreme = 0;
				AttackEffectCount = 0;
				EffectPos.X = Player.Pos.X;
				EffectPos.Y = Player.Pos.Y;
				if (PlayerAnimeFlag==Left) {
					EffectMove = -10;
				}
				else if (PlayerAnimeFlag==Right) {
					EffectMove = 10;
				}
			}

			//判定部分
			AttackHitFlag = QuadCollision(EffectPos,32,16,Boss.Pos,Boss.Radius,Boss.Radius);
			if (AttackHitFlag == 1) {
				Boss.HP -= 500;
				AttackHitFlag = 0;
				EffectPos = {
					-100,-100
				};
			}
			//ここまで-------------------------


		}
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//背景の描画
		if (scene == 1) {
			Novice::DrawSprite(0 + WorldRandX-Scroll , 0 - WorldRandY, GLBack1, 1, 1, 0.0f, WHITE);
		}
		if (scene == 2) {
			Novice::DrawSprite(-20 + WorldRandX, -20 - WorldRandY, GLBack, 1, 1, 0.0f, WHITE);
		}
		//レーザー
		for (int i = 0; i < 7; i++) {
			Novice::DrawQuad(Boss.LaserPosX[i], Boss.LaserPosY[i], Boss.LaserPosX[i] + 100, Boss.LaserPosY[i]
				, Boss.LaserPosX[i], Boss.LaserRadiusY[i] + Boss.LaserPosY[i], Boss.LaserPosX[i] + 100, Boss.LaserRadiusY[i] + Boss.LaserPosY[i]
				, 0, 0, 100, 500, Laser, WHITE);
			if (Boss.RainLaserFlag == 1 || Boss.RainLaserFlag == 3) {
				if (Boss.LaserTimer <= 80) {
					if (Boss.Danger == 0) {
						Novice::DrawSprite(Boss.LaserPosX[i], Boss.LaserPosY[i], Laser1, 1, 1, 0.0f, WHITE);
					}
					if (Boss.Danger == 1) {
						Novice::DrawSprite(Boss.LaserPosX[i], Boss.LaserPosY[i], Laser2, 1, 1, 0.0f, WHITE);
					}
				}
			}
		}
		if (scene == 1) {
			for (int y = 0; y < MapSizeY; y++) {
				for (int x = 0; x < MapSizeX+40; x++) {
					if (map1[y][x] == Block) {
						Novice::DrawSprite(x * BlockSize + WorldRandX-Scroll, y * BlockSize - WorldRandY, GLblock, 1, 1, 0.0f, WHITE);
					}
				}
			}
		}
		//ブロックの描画
		if (scene == 2) {
			for (int y = 0; y < MapSizeY; y++) {
				for (int x = 0; x < MapSizeX; x++) {
					if (map[y][x] == Block) {
						Novice::DrawSprite(x * BlockSize + WorldRandX, y * BlockSize - WorldRandY, GLblock, 1, 1, 0.0f, WHITE);
					}
				}
			}
		}

		//ボスの描画
		Novice::DrawSpriteRect(Boss.Pos.X - Boss.Radius - WorldRandX, Boss.Pos.Y - Boss.Radius - WorldRandY
			, EnemyStayAnime* (BlockSize * 4), 0, Boss.Radius * 2, Boss.Radius * 2, GLEnemyStay, 1 / 5.0f, 1, 0.0f, Color);

		//弾の描画
		if (Attack4.Shotflag == 1) {
			Novice::DrawSprite(Attack4.Pos[0].X-Attack4.Radius, Attack4.Pos[0].Y - Attack4.Radius, Bullet, 1, 1, 0.0f, WHITE);
		}

		if (Attack4.Shotflag == 2) {
			if (Attack4.Delete[1] == 0) {
				Novice::DrawSprite(Attack4.Pos[1].X - Attack4.Radius, Attack4.Pos[1].Y - Attack4.Radius, SmallBullet,1,1, 0.0f, WHITE);
			}
			if (Attack4.Delete[2] == 0) {
				Novice::DrawSprite(Attack4.Pos[2].X - Attack4.Radius, Attack4.Pos[2].Y - Attack4.Radius, SmallBullet, 1, 1, 0.0f, WHITE);
			}
			if (Attack4.Delete[3] == 0) {
				Novice::DrawSprite(Attack4.Pos[3].X - Attack4.Radius, Attack4.Pos[3].Y - Attack4.Radius, SmallBullet, 1, 1, 0.0f, WHITE);
			}
		}
		//プレイヤーの斬撃
		if (AttackEffectFlag == 1 && EffectMove == -10) {
			Novice::DrawSpriteRect(EffectPos.X - 16,EffectPos.Y - 32,AttackEffectAnime*BlockSize,0,BlockSize,BlockSize*2,GLplayerAttackEffectL,1/7.0f,1,0.0f,WHITE);
		}
		else if (AttackEffectFlag == 1 && EffectMove == 10) {
			Novice::DrawSpriteRect(EffectPos.X-16, EffectPos.Y-32, AttackEffectAnime * BlockSize, 0, BlockSize, BlockSize * 2, GLplayerAttackEffectR, 1 / 7.0f, 1, 0.0f, WHITE);
		}

		//-敵の攻撃３描画処理-----
		for (int i = 0; i < Attack3Max; i++) {
			//Novice::ScreenPrintf(0, 20*i, "%d %d", Attack3.ON[i],Attack3.Pos[i].x);
			if (Attack3.ON[i] == true) {
				if (Attack3.Attack[i] == false) {
					Novice::DrawSprite(Attack3.Pos[i].X, Attack3.Pos[i].Y, TriangleBullet, 2, 2, Attack3.Theta[i], WHITE);
				}
			}
			if (Attack3.ON[i] == true) {
				if (Attack3.Attack[i] == true) {
					Novice::DrawSprite(Attack3.Pos[i].X, Attack3.Pos[i].Y, STriangleBullet, 2, 2, Attack3.Theta[i], WHITE);
				}
			}
		}
		//-敵の攻撃３描画処理ここまで-----
		if (scene == 1) {
			//プレイヤーの描画
			if (InvincibleFlag == 1 || InvincibleFlag == 2 || InvincibleFlag == 0) {
				if (keys[DIK_LEFT] != 0 && PlayerAnimeFlag == Left) {

					if (AttackFlag == 1) {
						Novice::DrawSpriteRect(MonitorX - Player.Radius, Player.Pos.Y - Player.Radius - PlayerRandY, PlayerAttackAnime * 72, 0, 64, 64, GLplayerAttackL, 1 / 4.0f, 1, 0.0f, WHITE);
					}
					else {
						Novice::DrawSpriteRect(MonitorX - Player.Radius, Player.Pos.Y - Player.Radius - PlayerRandY, PlayerRunAnime * (BlockSize * 2), 0, 64, 64, GLplayerRunL, 1 / 6.0f, 1, 0.0f, WHITE);
					}
				}
				else if (keys[DIK_RIGHT] != 0 && PlayerAnimeFlag == Right) {

					if (AttackFlag == 1) {
						Novice::DrawSpriteRect(MonitorX - Player.Radius - PlayerRandX, Player.Pos.Y - Player.Radius - PlayerRandY, PlayerAttackAnime * 72, 0, 64, 64, GLplayerAttackR, 1 / 4.0f, 1, 0.0f, WHITE);
					}
					else {
						Novice::DrawSpriteRect(MonitorX - Player.Radius - PlayerRandX, Player.Pos.Y - Player.Radius - PlayerRandY, PlayerRunAnime * (BlockSize * 2), 0, 64, 64, GLplayerRunR, 1 / 6.0f, 1, 0.0f, WHITE);
					}
				}
				else {
					if (PlayerAnimeFlag == Left) {

						if (AttackFlag == 1) {
							Novice::DrawSpriteRect(MonitorX - Player.Radius - PlayerRandX, Player.Pos.Y - Player.Radius - PlayerRandY, PlayerAttackAnime * 72, 0, 64, 64, GLplayerAttackL, 1 / 4.0f, 1, 0.0f, WHITE);
						}
						else {
							Novice::DrawSpriteRect(MonitorX - Player.Radius - PlayerRandX, Player.Pos.Y - Player.Radius - PlayerRandY, PlayerStayAnime * (BlockSize * 2), 0, 64, 64, GLplayerStayL, 1 / 5.0f, 1, 0.0f, WHITE);
						}
					}
					else if (PlayerAnimeFlag == Right) {

						if (AttackFlag == 1) {
							Novice::DrawSpriteRect(MonitorX - Player.Radius - PlayerRandX, Player.Pos.Y - Player.Radius - PlayerRandY, PlayerAttackAnime * 72, 0, 64, 64, GLplayerAttackR, 1 / 4.0f, 1, 0.0f, WHITE);
						}
						else {
							Novice::DrawSpriteRect(MonitorX - Player.Radius - PlayerRandX, Player.Pos.Y - Player.Radius - PlayerRandY, PlayerStayAnime * (BlockSize * 2), 0, 64, 64, GLplayerStayR, 1 / 5.0f, 1, 0.0f, WHITE);
						}
					}
				}
			}
		}
		
		if (scene == 2) {
			//プレイヤーの描画
			if (InvincibleFlag == 1 || InvincibleFlag == 2 || InvincibleFlag == 0) {
				if (keys[DIK_LEFT] != 0 && PlayerAnimeFlag == Left) {

					if (AttackFlag == 1) {
						Novice::DrawSpriteRect(Player.Pos.X - Player.Radius, Player.Pos.Y - Player.Radius - PlayerRandY, PlayerAttackAnime * 72, 0, 64, 64, GLplayerAttackL, 1 / 4.0f, 1, 0.0f, WHITE);
					}
					else {
						Novice::DrawSpriteRect(Player.Pos.X - Player.Radius, Player.Pos.Y - Player.Radius - PlayerRandY, PlayerRunAnime * (BlockSize * 2), 0, 64, 64, GLplayerRunL, 1 / 6.0f, 1, 0.0f, WHITE);
					}
				}
				else if (keys[DIK_RIGHT] != 0 && PlayerAnimeFlag == Right) {

					if (AttackFlag == 1) {
						Novice::DrawSpriteRect(Player.Pos.X - Player.Radius - PlayerRandX, Player.Pos.Y - Player.Radius - PlayerRandY, PlayerAttackAnime * 72, 0, 64, 64, GLplayerAttackR, 1 / 4.0f, 1, 0.0f, WHITE);
					}
					else {
						Novice::DrawSpriteRect(Player.Pos.X - Player.Radius - PlayerRandX, Player.Pos.Y - Player.Radius - PlayerRandY, PlayerRunAnime * (BlockSize * 2), 0, 64, 64, GLplayerRunR, 1 / 6.0f, 1, 0.0f, WHITE);
					}
				}
				else {
					if (PlayerAnimeFlag == Left) {

						if (AttackFlag == 1) {
							Novice::DrawSpriteRect(Player.Pos.X - Player.Radius - PlayerRandX, Player.Pos.Y - Player.Radius - PlayerRandY, PlayerAttackAnime * 72, 0, 64, 64, GLplayerAttackL, 1 / 4.0f, 1, 0.0f, WHITE);
						}
						else {
							Novice::DrawSpriteRect(Player.Pos.X - Player.Radius - PlayerRandX, Player.Pos.Y - Player.Radius - PlayerRandY, PlayerStayAnime * (BlockSize * 2), 0, 64, 64, GLplayerStayL, 1 / 5.0f, 1, 0.0f, WHITE);
						}
					}
					else if (PlayerAnimeFlag == Right) {

						if (AttackFlag == 1) {
							Novice::DrawSpriteRect(Player.Pos.X - Player.Radius - PlayerRandX, Player.Pos.Y - Player.Radius - PlayerRandY, PlayerAttackAnime * 72, 0, 64, 64, GLplayerAttackR, 1 / 4.0f, 1, 0.0f, WHITE);
						}
						else {
							Novice::DrawSpriteRect(Player.Pos.X - Player.Radius - PlayerRandX, Player.Pos.Y - Player.Radius - PlayerRandY, PlayerStayAnime * (BlockSize * 2), 0, 64, 64, GLplayerStayR, 1 / 5.0f, 1, 0.0f, WHITE);
						}
					}
				}
			}
		}

		//デバッグ用
		Novice::DrawLine(Player.QuadBlock.LT.X, Player.QuadBlock.LT.Y, Player.QuadBlock.LB.X, Player.QuadBlock.LB.Y, RED);
		Novice::DrawLine(Player.QuadBlock.LB.X, Player.QuadBlock.LB.Y, Player.QuadBlock.RB.X, Player.QuadBlock.RB.Y, RED);
		Novice::DrawLine(Player.QuadBlock.RB.X, Player.QuadBlock.RB.Y, Player.QuadBlock.RT.X, Player.QuadBlock.RT.Y, RED);
		Novice::DrawLine(Player.QuadBlock.RT.X, Player.QuadBlock.RT.Y, Player.QuadBlock.LT.X, Player.QuadBlock.LT.Y, RED);
		Novice::DrawLine(PlayerTmp.Quad.LT.X, PlayerTmp.Quad.LT.Y, PlayerTmp.Quad.LB.X, PlayerTmp.Quad.LB.Y, RED);
		Novice::DrawLine(PlayerTmp.Quad.LB.X, PlayerTmp.Quad.LB.Y, PlayerTmp.Quad.RB.X, PlayerTmp.Quad.RB.Y, RED);
		Novice::DrawLine(PlayerTmp.Quad.RB.X, PlayerTmp.Quad.RB.Y, PlayerTmp.Quad.RT.X, PlayerTmp.Quad.RT.Y, RED);
		Novice::DrawLine(PlayerTmp.Quad.RT.X, PlayerTmp.Quad.RT.Y, PlayerTmp.Quad.LT.X, PlayerTmp.Quad.LT.Y, RED);

		//ジャンプ系
		Novice::ScreenPrintf(64, 64, "Velocity = %f", Velocity);
		Novice::ScreenPrintf(64, 92, "JanpFlag = %d", JanpFlag);
		Novice::ScreenPrintf(64, 128, "JanpAir = %d", JanpAir);
		Novice::ScreenPrintf(64, 162, "RandX = %d", RandX[0]);
		Novice::ScreenPrintf(64, 202, "scene = %d", scene);
		Novice::ScreenPrintf(64, 242, "LaserTimer = %d", Boss.LaserTimer);
		Novice::ScreenPrintf(64, 282, "Color = %d", Color);
		Novice::ScreenPrintf(64, 322, "PlayerY = %f", Boss.Pos.Y);
		Novice::ScreenPrintf(64, 342, "PlayerX = %f", Boss.Pos.X);
		Novice::ScreenPrintf(64, 362, "PlayerX = %f", Player.Pos.X);
		Novice::ScreenPrintf(64, 382, "PlayerY = %f", Player.Pos.Y);
		Novice::ScreenPrintf(64, 402, "LaserRadiusX = %d", Boss.LaserRadiusX[0]);
		Novice::ScreenPrintf(64, 422, "LaserRadiusY = %d", Boss.LaserRadiusY[0]);
		Novice::ScreenPrintf(64, 442, "BossHP = %d", Boss.HP);
		Novice::ScreenPrintf(64, 462, "PlayerHP = %d", PlayerHP);
		Novice::ScreenPrintf(64, 482, "Laserflag = %d", Boss.RainLaserFlag);
		Novice::DrawEllipse(EffectPos.X,EffectPos.Y,16,16,0.0f,RED,kFillModeSolid);
		Novice::DrawBox(0, 0, 1280, 720, 0.0f, AColor, kFillModeSolid);
		///
		/// ↑描画処理ここまで
		///


		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
//関数定義

void move(Vec2& Player, char keys[256], const Vec2 speed) {
	Vec2 Press = { 0,0 };
	if (keys[DIK_UP]) {
		Press.Y = -1;
	}
	if (keys[DIK_DOWN]) {
		Press.Y = 1;
	}
	if (keys[DIK_LEFT]) {
		Press.X = -1;
	}
	if (keys[DIK_RIGHT]) {
		Press.X = 1;
	}
	int	NewX = Press.X * speed.X;
	int	NewY = Press.Y * speed.Y;
	Player.X += NewX;
	Player.Y += NewY;
};
Vec2 toBlock(const Vec2 Posision, const int BlockSize) {
	Vec2 Result = { 0 };
	Result.X = Posision.X / BlockSize;
	Result.Y = Posision.Y / BlockSize;
	return Result;
};
Vec2 toPosision(const Vec2 BlockPos, const int BlockSize) {
	Vec2 Result = { 0 };
	Result.X = BlockPos.X * BlockSize;
	Result.Y = BlockPos.Y * BlockSize;
	return Result;
};
quad makeQuad(Vec2 point) {
	quad result = { 0 };
	result.LT = { (point.X + Quad.LT.X),(point.Y + Quad.LT.Y) };
	result.LB = { (point.X + Quad.LB.X),(point.Y + Quad.LB.Y) };
	result.RT = { (point.X + Quad.RT.X),(point.Y + Quad.RT.Y) };
	result.RB = { (point.X + Quad.RB.X),(point.Y + Quad.RB.Y) };
	return result;
}
void janpFanc(float& PlayerPosY, char keys[256], char preKeys[256], int& JanpFlag, float& Velocity) {
	float Acceleration = 0.8f;
	if (preKeys[DIK_SPACE] == 0 && keys[DIK_SPACE] != 0 && JanpFlag == 0) {
		JanpFlag = 1;
		Velocity = 15.0f;
	}
	if (JanpFlag == 1) {
		PlayerPosY -= Velocity;
		Velocity -= Acceleration;
	}
	if (PlayerPosY + 32 >= 608) {
		JanpFlag = 0;
		Velocity = 0.0f;
	}
}
void janpFancAir(Vec2& PlayerPos, char keys[256], char preKeys[256], int& JanpFlag, int& JanpAir, float& Velocity) {
	float Acceleration = 0.8f;
	int frame = 0;
	if (preKeys[DIK_SPACE] == 0 && keys[DIK_SPACE] != 0 && JanpFlag == 1 && JanpAir == 0 && Velocity <= 5) {
		JanpAir = 1;
		Velocity = 10.0f;
	}
	if (preKeys[DIK_SPACE] == 0 && keys[DIK_SPACE] != 0 && JanpFlag == 0) {
		JanpFlag = 1;
		Velocity = 20.0f;
	}

	if (JanpFlag == 1) {
		PlayerPos.Y -= Velocity;
		Velocity -= Acceleration;
	}
	if (JanpAir == 1) {
		if (keys[DIK_LEFT]) {
			PlayerPos.X -= 10;
		}
		else if (keys[DIK_RIGHT]) {
			PlayerPos.X += 10;
		}
	}

	if (PlayerPos.Y + 32 >= 608) {
		JanpFlag = 0;
		JanpAir = 0;
		Velocity = 0.0f;
	}
}
void FrameCount(int& FuncFrame, int& AnimeFrame, int AnimeChenge, const int EndFrame) {
	FuncFrame++;
	
	if (FuncFrame % AnimeChenge == 0) {
		AnimeFrame++;
	}
	if (FuncFrame >= EndFrame) {
		AnimeFrame = 0;
		FuncFrame = 0;
	}
}
int QuadCollision(Vec2 Point1, int P1H, int P1W, Vec2 Point2, int P2H, int P2W) {
	float Point1Top = Point1.Y-P1H;
	float Point1Bottom = Point1.Y+P1H;
	float Point1Left = Point1.X-P1W;
	float Point1Right = Point1.X+P1W;
	float Point2Top = Point2.Y-P2H;
	float Point2Bottom = Point2.Y+P2H;
	float Point2Left = Point2.X-P2W;
	float Point2Right = Point2.X+P2W;
	if (Point1Top <= Point2Bottom && Point2Top <= Point1Bottom) {
		if (Point1Left <= Point2Right && Point2Left <= Point1Right) {
			return 1;
		}
	}
	return 0;
}

