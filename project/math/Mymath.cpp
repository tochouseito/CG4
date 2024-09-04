#include "Mymath.h"
 
int check(double mat[N][N], double inv[N][N]) {

	double inner_product;
	int i, j, k;
	double ans;
	double diff;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			inner_product = 0;
			for (k = 0; k < N; k++) {
				inner_product += mat[i][k] * inv[k][j];
			}

			ans = (i == j) ? 1 : 0;
			diff = ans - inner_product;


		}
	}

	return 1;
}
// 行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = { 0 };
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			result.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}
	return result;

}
// 行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = { 0 };
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			result.m[i][j] = m1.m[i][j] - m2.m[i][j];
		}
	}
	return result;
}
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}
// 転置行列
Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 result = { 0 };
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			result.m[j][i] = m.m[i][j];
		}
	}
	return result;
}
// 単位行列の作成
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 result = { 0 };
	for (int i = 0; i < N; ++i) {
		result.m[i][i] = 1.0;
	}
	return result;
}
// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = { 0 };
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			for (int k = 0; k < N; ++k) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
}
// １．平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result;
	result.m[0][0] = 1.0f;
	result.m[1][0] = 0.0f;
	result.m[2][0] = 0.0f;
	result.m[3][0] = translate.x;
	result.m[0][1] = 0.0f;
	result.m[1][1] = 1.0f;
	result.m[2][1] = 0.0f;
	result.m[3][1] = translate.y;
	result.m[0][2] = 0.0f;
	result.m[1][2] = 0.0f;
	result.m[2][2] = 1.0f;
	result.m[3][2] = translate.z;
	result.m[0][3] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][3] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}
// ２．拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result;
	result.m[0][0] = scale.x;
	result.m[1][0] = 0.0f;
	result.m[2][0] = 0.0f;
	result.m[3][0] = 0.0f;
	result.m[0][1] = 0.0f;
	result.m[1][1] = scale.y;
	result.m[2][1] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[1][2] = 0.0f;
	result.m[2][2] = scale.z;
	result.m[3][2] = 0.0f;
	result.m[0][3] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][3] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}
// ３．座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}
// ４．X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 result;
	result.m[0][0] = 1.0f;
	result.m[1][0] = 0.0f;
	result.m[2][0] = 0.0f;
	result.m[3][0] = 0.0f;
	result.m[0][1] = 0.0f;
	result.m[1][1] = std::cosf(radian);
	result.m[2][1] = -std::sinf(radian);
	result.m[3][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[1][2] = std::sinf(radian);
	result.m[2][2] = std::cosf(radian);
	result.m[3][2] = 0.0f;
	result.m[0][3] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][3] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}
// ５．Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 result;
	result.m[0][0] = std::cosf(radian);
	result.m[1][0] = 0.0f;
	result.m[2][0] = std::sinf(radian);
	result.m[3][0] = 0.0f;
	result.m[0][1] = 0.0f;
	result.m[1][1] = 1.0f;
	result.m[2][1] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[0][2] = -std::sinf(radian);
	result.m[1][2] = 0.0f;
	result.m[2][2] = std::cosf(radian);
	result.m[3][2] = 0.0f;
	result.m[0][3] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][3] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}
// ６．Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 result;
	result.m[0][0] = std::cosf(radian);
	result.m[1][0] = -std::sinf(radian);
	result.m[2][0] = 0.0f;
	result.m[3][0] = 0.0f;
	result.m[0][1] = std::sinf(radian);
	result.m[1][1] = std::cosf(radian);
	result.m[2][1] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[1][2] = 0.0f;
	result.m[2][2] = 1.0f;
	result.m[3][2] = 0.0f;
	result.m[0][3] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][3] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}
Matrix4x4 MakeRotateXYZMatrix(Vector3& rotate)
{
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));
	return rotateXYZMatrix;
}
// 3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 result;
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);
	//result = Multiply(rotateXYZMatrix, Multiply(scaleMatrix, translateMatrix));
	result = Multiply(scaleMatrix, Multiply(rotateXYZMatrix, translateMatrix));
	//result = Multiply(translateMatrix, Multiply(scaleMatrix, rotateXYZMatrix));
	return result;
}
// 3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate) {
	Matrix4x4 result;
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotate);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);
	//result = Multiply(rotateXYZMatrix, Multiply(scaleMatrix, translateMatrix));
	result = Multiply(scaleMatrix, Multiply(rotateMatrix, translateMatrix));
	//result = Multiply(translateMatrix, Multiply(scaleMatrix, rotateMatrix));
	return result;
}
// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result;
	result.m[0][0] = (1 / std::tanf(fovY / 2)) / aspectRatio;
	result.m[1][0] = 0.0f;
	result.m[2][0] = 0.0f;
	result.m[3][0] = 0.0f;
	result.m[0][1] = 0.0f;
	result.m[1][1] = (1 / std::tanf(fovY / 2));
	result.m[2][1] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[1][2] = 0.0f;
	result.m[2][2] = farClip / farClip - nearClip;
	result.m[3][2] = (-farClip * nearClip) / (farClip - nearClip);
	result.m[0][3] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][3] = 1.0f;
	result.m[3][3] = 0.0f;
	return result;
}

// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result;
	result.m[0][0] = 2 / (right - left);
	result.m[1][0] = 0.0f;
	result.m[2][0] = 0.0f;
	result.m[3][0] = (left + right) / (left - right);
	result.m[0][1] = 0.0f;
	result.m[1][1] = 2 / (top - bottom);
	result.m[2][1] = 0.0f;
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[0][2] = 0.0f;
	result.m[1][2] = 0.0f;
	result.m[2][2] = 1 / (farClip - nearClip);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[0][3] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][3] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}

// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result;
	result.m[0][0] = width / 2;
	result.m[1][0] = 0.0f;
	result.m[2][0] = 0.0f;
	result.m[3][0] = left + (width / 2);
	result.m[0][1] = 0.0f;
	result.m[1][1] = -(height / 2);
	result.m[2][1] = 0.0f;
	result.m[3][1] = top + (height / 2);
	result.m[0][2] = 0.0f;
	result.m[1][2] = 0.0f;
	result.m[2][2] = maxDepth - minDepth;
	result.m[3][2] = minDepth;
	result.m[0][3] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][3] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}
// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result = { 0 };
	/* 逆行列を求める行列用の２次元配列 */
	double mat[N][N];

	/* 逆行列用の２次元配列 */
	double inv[N][N];

	/* 掃き出し法を行う行列 */
	double sweep[N][N * 2];

	int i; /* 行 */
	int j; /* 列 */
	int k; /* 注目対角成分が存在する列 */

	double a; /* 定数倍用 */

	/* 正方行列の各成分をセット */
	mat[0][0] = m.m[0][0]; mat[0][1] = m.m[0][1]; mat[0][2] = m.m[0][2]; mat[0][3] = m.m[0][3];
	mat[1][0] = m.m[1][0]; mat[1][1] = m.m[1][1]; mat[1][2] = m.m[1][2]; mat[1][3] = m.m[1][3];
	mat[2][0] = m.m[2][0]; mat[2][1] = m.m[2][1]; mat[2][2] = m.m[2][2]; mat[2][3] = m.m[2][3];
	mat[3][0] = m.m[3][0]; mat[3][1] = m.m[3][1]; mat[3][2] = m.m[3][2]; mat[3][3] = m.m[3][3];

	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			/* sweepの左側に逆行列を求める行列をセット */
			sweep[i][j] = mat[i][j];

			/* sweepの右側に単位行列をセット */
			sweep[i][N + j] = (i == j) ? 1 : 0;
		}
	}


	/* 全ての列の対角成分に対する繰り返し */
	for (k = 0; k < N; k++) {

		/* 最大の絶対値を注目対角成分の絶対値と仮定 */
		double max = fabs(sweep[k][k]);
		int max_i = k;

		/* k列目が最大の絶対値となる行を探す */
		for (i = k + 1; i < N; i++) {
			if (fabs(sweep[i][k]) > max) {
				max = fabs(sweep[i][k]);
				max_i = i;
			}
		}



		/* 操作（１）：k行目とmax_i行目を入れ替える */
		if (k != max_i) {
			for (j = 0; j < N * 2; j++) {
				double tmp = sweep[max_i][j];
				sweep[max_i][j] = sweep[k][j];
				sweep[k][j] = tmp;
			}
		}

		/* sweep[k][k]に掛けると1になる値を求める */
		a = 1 / sweep[k][k];

		/* 操作（２）：k行目をa倍する */
		for (j = 0; j < N * 2; j++) {
			/* これによりsweep[k][k]が1になる */
			sweep[k][j] *= a;
		}

		/* 操作（３）によりk行目以外の行のk列目を0にする */
		for (i = 0; i < N; i++) {
			if (i == k) {
				/* k行目はそのまま */
				continue;
			}

			/* k行目に掛ける値を求める */
			a = -sweep[i][k];

			for (j = 0; j < N * 2; j++) {
				/* i行目にk行目をa倍した行を足す */
				/* これによりsweep[i][k]が0になる */
				sweep[i][j] += sweep[k][j] * a;
			}
		}
	}

	/* sweepの右半分がmatの逆行列 */
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			inv[i][j] = sweep[i][N + j];
		}
	}

	/* 逆行列invを表示 */
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			result.m[i][j] = float(inv[i][j]);
			//printf("%0.2f, ", inv[i][j]);
		}
		//printf("\n");
	}

	/* 検算 */
	if (check(mat, inv)) {
		//printf("invはmatの逆行列です！！\n");
	} else {
		//printf("invはmatの逆行列になってません...\n");
	}
	return result;
}
// 長さ（ノルム）
float Length(const Vector3& v) {
	float result;
	result = float(sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)));
	return result;
}
// 正規化
Vector3 Normalize(const Vector3& v) {
	if (!v.x && !v.y && !v.z) {
		return Vector3(0.0f, 0.0f, 0.0f);
	}
	Vector3 result;
	result.x = float(v.x / sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)));
	result.y = float(v.y / sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)));
	result.z = float(v.z / sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z)));

	return result;
}
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = (v1.y * v2.z) - (v1.z * v2.y);
	result.y = (v1.z * v2.x) - (v1.x * v2.z);
	result.z = (v1.x * v2.y) - (v1.y * v2.x);
	return result;
}
float Dot(const Vector3& v1, const Vector3& v2) {
	float result;
	result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	return result;
}
bool IsCollision(const Vector3& center, const AABB& aabb) {
	// 最近接点を求める
	Vector3 closestPoint{ std::clamp(center.x,aabb.min.x,aabb.max.x),
	std::clamp(center.y,aabb.min.y,aabb.max.y) ,
	std::clamp(center.z,aabb.min.z,aabb.max.z) };
	// 最近接点と球の中心との距離を求める
	float distance = Length(closestPoint - center);

	// 中心間の距離が2つの円の半径の合計よりも小さい場合、衝突しているとみなす
	if (distance <=0) {
		return true;
	} else {
		return false;
	}
}

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m)
{
	Vector3 result{ v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0], v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1], v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] };
	return result;
}

Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle)
{
	Vector3 normAxis = axis;
	float axisLength = std::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
	if (axisLength != 0.0f) {
		normAxis.x /= axisLength;
		normAxis.y /= axisLength;
		normAxis.z /= axisLength;
	}

	float cosTheta = std::cos(angle);
	float sinTheta = std::sin(angle);
	float oneMinusCosTheta = 1.0f - cosTheta;

	Matrix4x4 rotateMatrix;

	rotateMatrix.m[0][0] = cosTheta + normAxis.x * normAxis.x * oneMinusCosTheta;
	rotateMatrix.m[0][1] = normAxis.x * normAxis.y * oneMinusCosTheta - normAxis.z * sinTheta;
	rotateMatrix.m[0][2] = normAxis.x * normAxis.z * oneMinusCosTheta + normAxis.y * sinTheta;
	rotateMatrix.m[0][3] = 0.0f;

	rotateMatrix.m[1][0] = normAxis.y * normAxis.x * oneMinusCosTheta + normAxis.z * sinTheta;
	rotateMatrix.m[1][1] = cosTheta + normAxis.y * normAxis.y * oneMinusCosTheta;
	rotateMatrix.m[1][2] = normAxis.y * normAxis.z * oneMinusCosTheta - normAxis.x * sinTheta;
	rotateMatrix.m[1][3] = 0.0f;

	rotateMatrix.m[2][0] = normAxis.z * normAxis.x * oneMinusCosTheta - normAxis.y * sinTheta;
	rotateMatrix.m[2][1] = normAxis.z * normAxis.y * oneMinusCosTheta + normAxis.x * sinTheta;
	rotateMatrix.m[2][2] = cosTheta + normAxis.z * normAxis.z * oneMinusCosTheta;
	rotateMatrix.m[2][3] = 0.0f;

	rotateMatrix.m[3][0] = 0.0f;
	rotateMatrix.m[3][1] = 0.0f;
	rotateMatrix.m[3][2] = 0.0f;
	rotateMatrix.m[3][3] = 1.0f;

	rotateMatrix = Transpose(rotateMatrix);

	return rotateMatrix;
}

Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to)
{
	Matrix4x4 rotateMatrix = { 0.0f };
	Vector3 n = Normalize(Cross(from, to));
	if (from == -to) {
		if (from.x != 0.0f || from.y != 0.0f) {
			n = Vector3{ from.y,-from.x,0.0f };
		} else if (from.x != 0.0f || from.z != 0.0f) {
			n = Vector3{ from.z,0.0f,-from.x };
		}
	}
	float cosTheta = Dot(from, to);
	float sinTheta = Length(Cross(from, to));
	rotateMatrix = {
		(n.x * n.x) * (1 - cosTheta) + cosTheta,(n.x * n.y) * (1 - cosTheta) + (n.z * sinTheta),(n.x * n.z) * (1 - cosTheta) - (n.y * sinTheta),0.0f,
		(n.x * n.y) * (1 - cosTheta) - (n.z * sinTheta),(n.y * n.y) * (1 - cosTheta) + cosTheta,(n.y * n.z) * (1 - cosTheta) + (n.x * sinTheta),0.0f,
		(n.x * n.z) * (1 - cosTheta) + (n.y * sinTheta),(n.y * n.z) * (1 - cosTheta) - (n.x * sinTheta),(n.z * n.z) * (1 - cosTheta) + cosTheta,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	return rotateMatrix;
}

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs)
{
	return {
		lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
		lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
		lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w,
		lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
	};
}

Quaternion IdentityQuaternion()
{
	return { 0.0f, 0.0f, 0.0f, 1.0f };
}

Quaternion Conjugate(const Quaternion& quaternion)
{
	return { -quaternion.x, -quaternion.y, -quaternion.z, quaternion.w };
}

float Norm(const Quaternion& quaternion)
{
	return std::sqrt(quaternion.x * quaternion.x +
		quaternion.y * quaternion.y +
		quaternion.z * quaternion.z +
		quaternion.w * quaternion.w);
}

Quaternion Normalize(const Quaternion& quaternion)
{
	float norm = Norm(quaternion);
	if (norm == 0.0f) {
		return { 0.0f, 0.0f, 0.0f, 1.0f };
	}
	return { quaternion.x / norm, quaternion.y / norm, quaternion.z / norm, quaternion.w / norm };
}

Quaternion Inverse(const Quaternion& quaternion)
{
	Quaternion conjugate = Conjugate(quaternion);
	float norm = Norm(quaternion);
	float normSq = norm * norm;
	if (normSq == 0.0f) {
		return { 0.0f, 0.0f, 0.0f, 1.0f };
	}
	return { conjugate.x / normSq, conjugate.y / normSq, conjugate.z / normSq, conjugate.w / normSq };
}

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle)
{
	Vector3 normAxis = Normalize(axis);
	float sinHalfAngle = std::sin(angle / 2.0f);
	float cosHalfAngle = std::cos(angle / 2.0f);
	return { normAxis.x * sinHalfAngle, normAxis.y * sinHalfAngle, normAxis.z * sinHalfAngle, cosHalfAngle };
}

Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion)
{
	Quaternion qVector = { vector.x, vector.y, vector.z, 0.0f };
	Quaternion qConjugate = { -quaternion.x, -quaternion.y, -quaternion.z, quaternion.w };
	Quaternion qResult = Multiply(Multiply(quaternion, qVector), qConjugate);
	return { qResult.x, qResult.y, qResult.z };
}

Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion)
{
	Matrix4x4 matrix = {};

	float xx = quaternion.x * quaternion.x;
	float yy = quaternion.y * quaternion.y;
	float zz = quaternion.z * quaternion.z;
	float xy = quaternion.x * quaternion.y;
	float xz = quaternion.x * quaternion.z;
	float yz = quaternion.y * quaternion.z;
	float wx = quaternion.w * quaternion.x;
	float wy = quaternion.w * quaternion.y;
	float wz = quaternion.w * quaternion.z;

	matrix.m[0][0] = 1.0f - 2.0f * (yy + zz);
	matrix.m[0][1] = 2.0f * (xy - wz);
	matrix.m[0][2] = 2.0f * (xz + wy);
	matrix.m[0][3] = 0.0f;

	matrix.m[1][0] = 2.0f * (xy + wz);
	matrix.m[1][1] = 1.0f - 2.0f * (xx + zz);
	matrix.m[1][2] = 2.0f * (yz - wx);
	matrix.m[1][3] = 0.0f;

	matrix.m[2][0] = 2.0f * (xz - wy);
	matrix.m[2][1] = 2.0f * (yz + wx);
	matrix.m[2][2] = 1.0f - 2.0f * (xx + yy);
	matrix.m[2][3] = 0.0f;

	matrix.m[3][0] = 0.0f;
	matrix.m[3][1] = 0.0f;
	matrix.m[3][2] = 0.0f;
	matrix.m[3][3] = 1.0f;

	matrix = Transpose(matrix);

	return matrix;
}

float Dot(const Quaternion& q0, const Quaternion& q1)
{
	return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
}

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t)
{
	// クォータニオンの内積を計算
	float dot = Dot(q0, q1);

	// クォータニオンが反対向きの場合、内積が負になるので符号を反転
	const float THRESHOLD = 0.9995f;
	if (dot < 0.0f) {
		dot = -dot;
		Quaternion negQ1 = { -q1.x, -q1.y, -q1.z, -q1.w };
		return Slerp(q0, negQ1, t);
	}

	// 内積が閾値以上の場合、線形補間を使用
	if (dot > THRESHOLD) {
		Quaternion result = {
			q0.x + t * (q1.x - q0.x),
			q0.y + t * (q1.y - q0.y),
			q0.z + t * (q1.z - q0.z),
			q0.w + t * (q1.w - q0.w)
		};
		// 正規化
		float norm = std::sqrt(result.x * result.x + result.y * result.y + result.z * result.z + result.w * result.w);
		return { result.x / norm, result.y / norm, result.z / norm, result.w / norm };
	}

	// 角度を計算
	float theta_0 = std::acos(dot);  // θ0 = angle between input vectors
	float theta = theta_0 * t;       // θ = angle between q0 and result
	float sin_theta = std::sin(theta); // Compute this value only once
	float sin_theta_0 = std::sin(theta_0); // Compute this value only once

	float s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;  // s0 = sin((1 - t) * theta) / sin(theta)
	float s1 = sin_theta / sin_theta_0; // s1 = sin(t * theta) / sin(theta)

	return {
		s0 * q0.x + s1 * q1.x,
		s0 * q0.y + s1 * q1.y,
		s0 * q0.z + s1 * q1.z,
		s0 * q0.w + s1 * q1.w
	};
}

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t)
{
	return Vector3{
		v1.x + t * (v2.x - v1.x),
		v1.y + t * (v2.y - v1.y),
		v1.z + t * (v2.z - v1.z)
	};
}
