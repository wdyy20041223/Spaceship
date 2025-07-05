#pragma once
#ifndef CMATRIX_H    
#define CMATRIX_H 

class CVector;
class CEuler;
class CQuaternion;

class CMatrix {
public:
	float m00, m10, m20, m30;
	float m01, m11, m21, m31;
	float m02, m12, m22, m32;
	float m03, m13, m23, m33;

	CVector TransformPoint(const CVector& point) const;

	void Set(float* p);	//给矩阵16个元素赋值
	CMatrix& operator=(const CMatrix& p);//矩阵赋值
	CMatrix operator*(float d);//矩阵数乘
	CMatrix operator*(const CMatrix& p) const;//矩阵相乘
	CVector vecMul(const CVector& p) const; //矩阵与向量相乘
	CVector posMul(const CVector& p) const;//矩阵与位置相乘
	void SetRotate(float theta, CVector axis);	//设置为旋转矩阵
	void SetTrans(CVector trans);		//设置为平移矩阵
	void SetScale(CVector p);		//设置为缩放矩阵
	float Inverse();//矩阵求逆,成功返回行列式的值，否则返回0
	CMatrix GetInverse() const;//返回逆矩阵
	operator float* ();
	void output();//输出矩阵
	CEuler ToEuler() const;        // 转换为欧拉角
	CQuaternion ToQuaternion() const; // 转换为四元数
	void Orthogonalize();//正交化

	operator const float* () const; // 新增const版本

};


#endif // CMATRIX_H
