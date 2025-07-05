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

	void Set(float* p);	//������16��Ԫ�ظ�ֵ
	CMatrix& operator=(const CMatrix& p);//����ֵ
	CMatrix operator*(float d);//��������
	CMatrix operator*(const CMatrix& p) const;//�������
	CVector vecMul(const CVector& p) const; //�������������
	CVector posMul(const CVector& p) const;//������λ�����
	void SetRotate(float theta, CVector axis);	//����Ϊ��ת����
	void SetTrans(CVector trans);		//����Ϊƽ�ƾ���
	void SetScale(CVector p);		//����Ϊ���ž���
	float Inverse();//��������,�ɹ���������ʽ��ֵ�����򷵻�0
	CMatrix GetInverse() const;//���������
	operator float* ();
	void output();//�������
	CEuler ToEuler() const;        // ת��Ϊŷ����
	CQuaternion ToQuaternion() const; // ת��Ϊ��Ԫ��
	void Orthogonalize();//������

	operator const float* () const; // ����const�汾

};


#endif // CMATRIX_H
