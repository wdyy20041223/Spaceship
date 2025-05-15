
#include "base.h"
#include "CVector.h"  // 包含CVector的完整定义
#include "CMatrix.h"
#include "CEuler.h"
#include "CQuaternion.h"

bool testfunc() {

    /*CQuaternion test;
    float angle1 = 60, angle2;
    CVector axis1 = CVector(0,0,1), axis2;
    test.SetAngle(angle1 * 3 , axis1);
    test.GetAngle(angle2,axis2);
    printf("%f\n", angle2);
    axis2.output();*/

    return true;
}

bool CalculateTest() {
    FILE* fp, * fpOut;
    char str[1024];
    fopen_s(&fp, "test.txt", "rt");
    fopen_s(&fpOut, "out.txt", "wt");

    while (!feof(fp)) {
        str[0] = '\0';
        fscanf_s(fp, "%s\n", str, (unsigned)_countof(str));
        if (strcmp(str, "欧拉角转换向量") == 0) {
            CEuler v1;
            CVector result;
            fscanf_s(fp, "%f,%f,%f", &v1.h, &v1.p, &v1.b);
            result = v1.ToCVector();
            fprintf(fpOut, "%s\n%g,%g,%g \t%g,%g,%g\n", str, v1.h, v1.p, v1.b, result.x, result.y, result.z);
        }
        else if (strcmp(str, "矩阵正交化") == 0) {
            CMatrix mat, mat1, mat2;
            // 读取矩阵元素（列主序存储）
            fscanf_s(fp, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                &mat.m00, &mat.m10, &mat.m20, &mat.m30,
                &mat.m01, &mat.m11, &mat.m21, &mat.m31,
                &mat.m02, &mat.m12, &mat.m22, &mat.m32,
                &mat.m03, &mat.m13, &mat.m23, &mat.m33);
            mat1 = mat;
            mat.Orthogonalize();
            mat2 = mat;
            // 按行主序输出矩阵元素
            fprintf(fpOut, "%s\n%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g \t%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g\n", str,
                // mat1行主序输出
                mat1.m00, mat1.m10, mat1.m20, mat1.m30,
                mat1.m01, mat1.m11, mat1.m21, mat1.m31,
                mat1.m02, mat1.m12, mat1.m22, mat1.m32,
                mat1.m03, mat1.m13, mat1.m23, mat1.m33,
                mat2.m00, mat2.m10, mat2.m20, mat2.m30,
                mat2.m01, mat2.m11, mat2.m21, mat2.m31,
                mat2.m02, mat2.m12, mat2.m22, mat2.m32,
                mat2.m03, mat2.m13, mat2.m23, mat2.m33);
        }
        else if (strcmp(str, "向量转换欧拉角") == 0) {
            CVector vec;
            CEuler euler;
            fscanf_s(fp, "%f,%f,%f", &vec.x, &vec.y, &vec.z);
            euler = vec.ToEuler();
            fprintf(fpOut, "%s\n%g,%g,%g \t%g,%g,%g\n",
                str, vec.x, vec.y, vec.z, euler.h, euler.p, euler.b);
        }
        else if (strcmp(str, "欧拉角转换四元数") == 0) {
            CEuler euler;
            CQuaternion quat;
            fscanf_s(fp, "%f,%f,%f", &euler.h, &euler.p, &euler.b);
            quat = euler.ToQuaternion();
            fprintf(fpOut, "%s\n%g,%g,%g \t%g,%g,%g,%g\n",
                str, euler.h, euler.p, euler.b, quat.w, quat.x, quat.y, quat.z);
        }
        else if (strcmp(str, "欧拉角标准化") == 0) {
            CEuler v1, v2;
            fscanf_s(fp, "%f,%f,%f", &v1.h, &v1.p, &v1.b);
            v2 = v1;
            v2.Normal();
            fprintf(fpOut, "%s\n%g,%g,%g \t%g,%g,%g\n", str, v1.h, v1.p, v1.b, v2.h, v2.p, v2.b);
        }
        else if (strcmp(str, "四元数点乘") == 0) {
            CQuaternion v1, v2;
            float result;
            fscanf_s(fp, "%f,%f,%f,%f %f,%f,%f,%f", &v1.w, &v1.x, &v1.y, &v1.z, &v2.w, &v2.x, &v2.y, &v2.z);
            result = v1.dotMul(v2);
            fprintf(fpOut, "%s\n%g,%g,%g,%g \t%g,%g,%g,%g \t%g\n", str, v1.w, v1.x, v1.y, v1.z, v2.w, v2.x, v2.y, v2.z, result);
        }
        else if (strcmp(str, "四元数求角度和旋转轴") == 0) {
            CQuaternion v1;
            float angle1;
            CVector axis;
            fscanf_s(fp, "%f,%f,%f,%f", &v1.w, &v1.x, &v1.y, &v1.z);
            v1.GetAngle(angle1, axis);
            fprintf(fpOut, "%s\n%g,%g,%g,%g \t%g \t%g,%g,%g\n", str, v1.w, v1.x, v1.y, v1.z, angle1, axis.x, axis.y, axis.z);
        }
        else if (strcmp(str, "四元数转换矩阵") == 0) {
            CQuaternion q;
            fscanf_s(fp, "%f,%f,%f,%f", &q.w, &q.x, &q.y, &q.z);
            CMatrix mat = q.ToMatrix();
            fprintf(fpOut, "%s\n%g,%g,%g,%g \t%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g\n",
                str, q.w, q.x, q.y, q.z,
                mat.m00, mat.m10, mat.m20, mat.m30,
                mat.m01, mat.m11, mat.m21, mat.m31,
                mat.m02, mat.m12, mat.m22, mat.m32,
                mat.m03, mat.m13, mat.m23, mat.m33); // 第四行
        }
        else if (strcmp(str, "欧拉角转换矩阵") == 0) {
            CEuler euler;
            fscanf_s(fp, "%f,%f,%f", &euler.h, &euler.p, &euler.b);
            CMatrix mat = euler.ToCMatrix();
            // 输出格式：欧拉角 + 制表符 + 矩阵16元素（按行展开）
            fprintf(fpOut, "%s\n%g,%g,%g \t%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g\n",
                str, euler.h, euler.p, euler.b,
                mat.m00, mat.m10, mat.m20, mat.m30,
                mat.m01, mat.m11, mat.m21, mat.m31,
                mat.m02, mat.m12, mat.m22, mat.m32,
                mat.m03, mat.m13, mat.m23, mat.m33); // 第四行
        }
        else if (strcmp(str, "四元数转换欧拉角") == 0) {
            CQuaternion q;
            CEuler euler;
            fscanf_s(fp, "%f,%f,%f,%f", &q.w, &q.x, &q.y, &q.z);
            euler = q.ToEuler();
            fprintf(fpOut, "%s\n%g,%g,%g,%g \t%g,%g,%g\n",
                str, q.w, q.x, q.y, q.z, euler.h, euler.p, euler.b);
        }
        else if (strcmp(str, "矩阵转换欧拉角") == 0) {
            CMatrix mat,mat2;// 按行读取矩阵元素
            fscanf_s(fp, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                &mat.m00, &mat.m10, &mat.m20, &mat.m30,
                &mat.m01, &mat.m11, &mat.m21, &mat.m31,
                &mat.m02, &mat.m12, &mat.m22, &mat.m32,
                &mat.m03, &mat.m13, &mat.m23, &mat.m33);
            CEuler euler = mat.ToEuler();
            fprintf(fpOut, "%s\n%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g \t%g,%g,%g\n", str, 
                mat.m00, mat.m10, mat.m20, mat.m30,
                mat.m01, mat.m11, mat.m21, mat.m31,
                mat.m02, mat.m12, mat.m22, mat.m32,
                mat.m03, mat.m13, mat.m23, mat.m33,
                euler.h, euler.p, euler.b);
        }
        else if (strcmp(str, "四元数单位化") == 0) {
            CQuaternion p,q;
            fscanf_s(fp, "%f,%f,%f,%f", &q.w, &q.x, &q.y, &q.z);
            p = q;
            q.Normalize();
            fprintf(fpOut, "%s\n%g,%g,%g,%g \t%g,%g,%g,%g\n",
                str, p.w, p.x, p.y, p.z, q.w, q.x, q.y, q.z);
        }
        else if (strcmp(str, "四元数插值") == 0) {
            CQuaternion q1, q2;
            float t;
            // 格式：四元数1 \t 四元数2 \t 插值参数
            fscanf_s(fp, "%f,%f,%f,%f %f,%f,%f,%f %f",
                &q1.w, &q1.x, &q1.y, &q1.z,
                &q2.w, &q2.x, &q2.y, &q2.z,
                &t);
            CQuaternion result = q1.Slerp(q2, t);
            fprintf(fpOut, "%s\n%g,%g,%g,%g \t%g,%g,%g,%g \t%g \t%g,%g,%g,%g\n",
                str, q1.w, q1.x, q1.y, q1.z, q2.w, q2.x, q2.y, q2.z, t,
                result.w, result.x, result.y, result.z);
        }

        else if (strcmp(str, "矩阵转换四元数") == 0) {
            CMatrix mat;
            fscanf_s(fp, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                &mat.m00, &mat.m10, &mat.m20, &mat.m30,
                &mat.m01, &mat.m11, &mat.m21, &mat.m31,
                &mat.m02, &mat.m12, &mat.m22, &mat.m32,
                &mat.m03, &mat.m13, &mat.m23, &mat.m33);
            CQuaternion q = mat.ToQuaternion();
            fprintf(fpOut, "%s\n%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g \t%g,%g,%g,%g\n", str, 
                mat.m00, mat.m10, mat.m20, mat.m30,
                mat.m01, mat.m11, mat.m21, mat.m31,
                mat.m02, mat.m12, mat.m22, mat.m32,
                mat.m03, mat.m13, mat.m23, mat.m33,
                q.w, q.x, q.y, q.z);
        }
        
        else if (strcmp(str, "四元数求逆") == 0) {
            CQuaternion q, inv;
            fscanf_s(fp, "%f,%f,%f,%f", &q.w, &q.x, &q.y, &q.z);
            inv = q.GetInverse();
            fprintf(fpOut, "%s\n%g,%g,%g,%g \t%g,%g,%g,%g\n",
                str, q.w, q.x, q.y, q.z, inv.w, inv.x, inv.y, inv.z);
        }
        else if (strcmp(str, "四元数求差") == 0) {
            CQuaternion q1, q2, diff;
            fscanf_s(fp, "%f,%f,%f,%f %f,%f,%f,%f",
                &q1.w, &q1.x, &q1.y, &q1.z,
                &q2.w, &q2.x, &q2.y, &q2.z);
            diff = q1.Div(q2); 
            fprintf(fpOut, "%s\n%g,%g,%g,%g \t%g,%g,%g,%g \t%g,%g,%g,%g\n",
                str, q1.w, q1.x, q1.y, q1.z, q2.w, q2.x, q2.y, q2.z,
                diff.w, diff.x, diff.y, diff.z);
        }
        else if (strcmp(str, "四元数相乘") == 0) {
            CQuaternion q1, q2, result;
            fscanf_s(fp, "%f,%f,%f,%f %f,%f,%f,%f",
                &q1.w, &q1.x, &q1.y, &q1.z,
                &q2.w, &q2.x, &q2.y, &q2.z);
            result = q1 * q2;
            fprintf(fpOut, "%s\n%g,%g,%g,%g \t%g,%g,%g,%g \t%g,%g,%g,%g\n",
                str, q1.w, q1.x, q1.y, q1.z, q2.w, q2.x, q2.y, q2.z,
                result.w, result.x, result.y, result.z);
        }
        else {
            fgets(str, _countof(str), fp); // 跳过无法识别的行
        }
    }

    fclose(fp);
    fclose(fpOut);
    return false;
}


bool CalculateMatrix()
{
    FILE* fp, * fpOut;
    char str[1024];
    fopen_s(&fp, "test.txt", "rt");
    fopen_s(&fpOut, "out.txt", "wt");
    while (!feof(fp))
    {
        str[0] = '\0';
        fscanf_s(fp, "%s\n", str, 1024);
        if (strcmp(str, "矩阵相乘") == 0) {
            CMatrix mat1, mat2, result;
            fscanf_s(fp, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f %f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                &mat1[0], &mat1[1], &mat1[2], &mat1[3], &mat1[4], &mat1[5], &mat1[6], &mat1[7],
                &mat1[8], &mat1[9], &mat1[10], &mat1[11], &mat1[12], &mat1[13], &mat1[14], &mat1[15],
                &mat2[0], &mat2[1], &mat2[2], &mat2[3], &mat2[4], &mat2[5], &mat2[6], &mat2[7],
                &mat2[8], &mat2[9], &mat2[10], &mat2[11], &mat2[12], &mat2[13], &mat2[14], &mat2[15]);
            result = mat1 * mat2;
            fprintf(fpOut, "%s\n%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g \t%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g \t%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g\n", str, 
                mat1[0], mat1[1], mat1[2], mat1[3], mat1[4], mat1[5], mat1[6], mat1[7],
                mat1[8], mat1[9], mat1[10],mat1[11], mat1[12], mat1[13], mat1[14], mat1[15],
                mat2[0], mat2[1], mat2[2], mat2[3], mat2[4], mat2[5], mat2[6], mat2[7],
                mat2[8], mat2[9], mat2[10], mat2[11], mat2[12], mat2[13], mat2[14], mat2[15],
                result[0], result[1], result[2], result[3], result[4], result[5], result[6], result[7],
                result[8], result[9], result[10], result[11], result[12], result[13], result[14], result[15]);
        }
        else if (strcmp(str, "矩阵乘向量") == 0) {
            CMatrix mat1;
            CVector vec1,result;
            fscanf_s(fp, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f %f,%f,%f",
                &mat1[0], &mat1[1], &mat1[2], &mat1[3], &mat1[4], &mat1[5], &mat1[6], &mat1[7],
                &mat1[8], &mat1[9], &mat1[10], &mat1[11], &mat1[12], &mat1[13], &mat1[14], &mat1[15],
                &vec1.x, &vec1.y, &vec1.z);
            result = mat1.vecMul(vec1);
            fprintf(fpOut, "%s\n%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g \t%g,%g,%g \t%g,%g,%g\n", str,
                mat1[0], mat1[1], mat1[2], mat1[3], mat1[4], mat1[5], mat1[6], mat1[7],
                mat1[8], mat1[9], mat1[10], mat1[11], mat1[12], mat1[13], mat1[14], mat1[15],
                vec1.x, vec1.y, vec1.z, result.x, result.y, result.z);
        }
        else if (strcmp(str, "矩阵乘位置") == 0) {
            CMatrix mat1;
            CVector vec1, result;
            fscanf_s(fp, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f %f,%f,%f",
                &mat1[0], &mat1[1], &mat1[2], &mat1[3], &mat1[4], &mat1[5], &mat1[6], &mat1[7],
                &mat1[8], &mat1[9], &mat1[10], &mat1[11], &mat1[12], &mat1[13], &mat1[14], &mat1[15],
                &vec1.x, &vec1.y, &vec1.z);
            result = mat1.posMul(vec1);
            fprintf(fpOut, "%s\n%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g \t%g,%g,%g \t%g,%g,%g\n", str,
                mat1[0], mat1[1], mat1[2], mat1[3], mat1[4], mat1[5], mat1[6], mat1[7],
                mat1[8], mat1[9], mat1[10], mat1[11], mat1[12], mat1[13], mat1[14], mat1[15],
                vec1.x, vec1.y, vec1.z, result.x, result.y, result.z);
        }
        else if (strcmp(str, "矩阵设置旋转") == 0) {
            CMatrix result;
            float angle;
            CVector axis;
            fscanf_s(fp, "%f %f,%f,%f", &angle, &axis.x, &axis.y, &axis.z);
            result.SetRotate(angle, axis);
            fprintf(fpOut, "%s\n%g \t%g,%g,%g \t%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g\n", str,
                angle, axis.x, axis.y, axis.z,
                result[0], result[1], result[2], result[3], result[4], result[5], result[6], result[7],
                result[8], result[9], result[10], result[11], result[12], result[13], result[14], result[15]);
        }
        else if (strcmp(str, "矩阵设置平移") == 0) {
            CMatrix result;
            CVector axis;
            fscanf_s(fp, "%f,%f,%f", &axis.x, &axis.y, &axis.z);
            result.SetTrans(axis);
            fprintf(fpOut, "%s\n%g,%g,%g \t%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g\n", str,
                axis.x, axis.y, axis.z,
                result[0], result[1], result[2], result[3], result[4], result[5], result[6], result[7],
                result[8], result[9], result[10], result[11], result[12], result[13], result[14], result[15]);
        }
        else if (strcmp(str, "矩阵设置缩放") == 0) {
            CMatrix result;
            CVector axis;
            fscanf_s(fp, "%f,%f,%f", &axis.x, &axis.y, &axis.z);
            result.SetScale(axis);
            fprintf(fpOut, "%s\n%g,%g,%g \t%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g\n", str,
                axis.x, axis.y, axis.z,
                result[0], result[1], result[2], result[3], result[4], result[5], result[6], result[7],
                result[8], result[9], result[10], result[11], result[12], result[13], result[14], result[15]);
        }
        else if (strcmp(str, "矩阵求逆") == 0) {
            CMatrix mat1, mat2, result;
            fscanf_s(fp, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                &mat1[0], &mat1[1], &mat1[2], &mat1[3], &mat1[4], &mat1[5], &mat1[6], &mat1[7],
                &mat1[8], &mat1[9], &mat1[10], &mat1[11], &mat1[12], &mat1[13], &mat1[14], &mat1[15]);
            result = mat1.GetInverse();
            fprintf(fpOut, "%s\n%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g \t%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g\n", str,
                mat1[0], mat1[1], mat1[2], mat1[3], mat1[4], mat1[5], mat1[6], mat1[7],
                mat1[8], mat1[9], mat1[10], mat1[11], mat1[12], mat1[13], mat1[14], mat1[15],
                result[0], result[1], result[2], result[3], result[4], result[5], result[6], result[7],
                result[8], result[9], result[10], result[11], result[12], result[13], result[14], result[15]);
        }
        else {
            fgets(str, 1024, fp);
        }
    }
    fclose(fp);
    fclose(fpOut);
    return false;
}

bool Calculate()
{
    FILE* fp, * fpOut;
    char str[1024];
    fopen_s(&fp, "test.txt", "rt");
    fopen_s(&fpOut, "out.txt", "wt");
    while (!feof(fp))
    {
        str[0] = '\0';
        fscanf_s(fp, "%s\n", str, 1024);
        if (strcmp(str, "向量相加") == 0) {
            CVector v1, v2, vout;
            fscanf_s(fp, "%f,%f,%f %f,%f,%f", &v1.x, &v1.y, &v1.z, &v2.x, &v2.y, &v2.z);
            vout = v1 + v2;
            fprintf(fpOut, "%s\n%g,%g,%g \t%g,%g,%g \t%g,%g,%g\n", str, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, vout.x, vout.y, vout.z);
        }
        else if (strcmp(str, "向量点乘") == 0) {
            CVector v1, v2;
            fscanf_s(fp, "%f,%f,%f %f,%f,%f", &v1.x, &v1.y, &v1.z, &v2.x, &v2.y, &v2.z);
            fprintf(fpOut, "%s\n%g,%g,%g \t%g,%g,%g \t%g\n", str, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v1.dotMul(v2));
        }
        else if (strcmp(str, "向量叉乘") == 0) {
            CVector v1, v2, vout;
            fscanf_s(fp, "%f,%f,%f %f,%f,%f", &v1.x, &v1.y, &v1.z, &v2.x, &v2.y, &v2.z);
            vout = v1.crossMul(v2);
            fprintf(fpOut, "%s\n%g,%g,%g \t%g,%g,%g \t%g,%g,%g\n", str, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, vout.x, vout.y, vout.z);
        }
        else if (strcmp(str, "向量标准化") == 0) {
            CVector v1, temp;
            fscanf_s(fp, "%f,%f,%f", &v1.x, &v1.y, &v1.z);
            temp = v1;
            temp.Normalize();
            fprintf(fpOut, "%s\n%g,%g,%g \t%g,%g,%g\n", str, v1.x, v1.y, v1.z, temp.x, temp.y, temp.z);
        }
        else if (strcmp(str, "向量求模") == 0) {
            CVector v1;
            fscanf_s(fp, "%f,%f,%f", &v1.x, &v1.y, &v1.z);
            fprintf(fpOut, "%s\n%g,%g,%g \t%g\n", str, v1.x, v1.y, v1.z, v1.len());
        }
        else if (strcmp(str, "向量投影") == 0) {
            CVector v1, v2, vout;
            fscanf_s(fp, "%f,%f,%f %f,%f,%f", &v1.x, &v1.y, &v1.z, &v2.x, &v2.y, &v2.z);
            vout = v1.project(v2);
            fprintf(fpOut, "%s\n%g,%g,%g \t%g,%g,%g \t%g,%g,%g\n", str, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, vout.x, vout.y, vout.z);
        }
        else {
            fgets(str, 1024, fp);
        }
    }
    fclose(fp);
    fclose(fpOut);
    return false;
}