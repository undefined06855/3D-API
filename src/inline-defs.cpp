#include "inline-defs.hpp"

#ifdef GEODE_IS_IOS

kmMat4* unlinked::kmMat4RotationX(kmMat4* pOut, const kmScalar radians) {
	/*
		 |  1  0       0       0 |
	 M = |  0  cos(A) -sin(A)  0 |
	     |  0  sin(A)  cos(A)  0 |
	     |  0  0       0       1 |

	*/

	pOut->mat[0] = 1.0f;
	pOut->mat[1] = 0.0f;
	pOut->mat[2] = 0.0f;
	pOut->mat[3] = 0.0f;

	pOut->mat[4] = 0.0f;
	pOut->mat[5] = cosf(radians);
	pOut->mat[6] = sinf(radians);
	pOut->mat[7] = 0.0f;

	pOut->mat[8] = 0.0f;
	pOut->mat[9] = -sinf(radians);
	pOut->mat[10] = cosf(radians);
	pOut->mat[11] = 0.0f;

	pOut->mat[12] = 0.0f;
	pOut->mat[13] = 0.0f;
	pOut->mat[14] = 0.0f;
	pOut->mat[15] = 1.0f;

	return pOut;
}

kmMat4* unlinked::kmMat4RotationY(kmMat4* pOut, const kmScalar radians) {
	/*
	     |  cos(A)  0   sin(A)  0 |
	 M = |  0       1   0       0 |
	     | -sin(A)  0   cos(A)  0 |
	     |  0       0   0       1 |
	*/

	pOut->mat[0] = cosf(radians);
	pOut->mat[1] = 0.0f;
	pOut->mat[2] = -sinf(radians);
	pOut->mat[3] = 0.0f;

	pOut->mat[4] = 0.0f;
	pOut->mat[5] = 1.0f;
	pOut->mat[6] = 0.0f;
	pOut->mat[7] = 0.0f;

	pOut->mat[8] = sinf(radians);
	pOut->mat[9] = 0.0f;
	pOut->mat[10] = cosf(radians);
	pOut->mat[11] = 0.0f;

	pOut->mat[12] = 0.0f;
	pOut->mat[13] = 0.0f;
	pOut->mat[14] = 0.0f;
	pOut->mat[15] = 1.0f;

	return pOut;
}

kmMat4* unlinked::kmMat4RotationZ(kmMat4* pOut, const kmScalar radians) {
	/*
	     |  cos(A)  -sin(A)   0   0 |
	 M = |  sin(A)   cos(A)   0   0 |
	     |  0        0        1   0 |
	     |  0        0        0   1 |
	*/

	pOut->mat[0] = cosf(radians);
	pOut->mat[1] = sinf(radians);
	pOut->mat[2] = 0.0f;
	pOut->mat[3] = 0.0f;

	pOut->mat[4] = -sinf(radians);
	pOut->mat[5] = cosf(radians);
	pOut->mat[6] = 0.0f;
	pOut->mat[7] = 0.0f;

	pOut->mat[8] = 0.0f;
	pOut->mat[9] = 0.0f;
	pOut->mat[10] = 1.0f;
	pOut->mat[11] = 0.0f;

	pOut->mat[12] = 0.0f;
	pOut->mat[13] = 0.0f;
	pOut->mat[14] = 0.0f;
	pOut->mat[15] = 1.0f;

	return pOut;
}

kmMat4* const unlinked::kmMat4RotationPitchYawRoll(kmMat4* pOut, const kmScalar pitch, const kmScalar yaw, const kmScalar roll) {
    kmMat4 yaw_matrix;
    kmMat4 roll_matrix;
    kmMat4 pitch_matrix;

    unlinked::kmMat4RotationY(&yaw_matrix, yaw);

    unlinked::kmMat4RotationX(&pitch_matrix, pitch);

    unlinked::kmMat4RotationZ(&roll_matrix, roll);

    kmMat4Multiply(pOut, &pitch_matrix, &roll_matrix);
    kmMat4Multiply(pOut, &yaw_matrix, pOut);

    return pOut;
}

kmMat4* const unlinked::kmMat4Scaling(kmMat4* pOut, const kmScalar x, const kmScalar y, const kmScalar z) {
    memset(pOut->mat, 0, sizeof(kmScalar) * 16);
    pOut->mat[0] = x;
    pOut->mat[5] = y;
    pOut->mat[10] = z;
    pOut->mat[15] = 1.0f;

    return pOut;
}

#else

kmMat4* const unlinked::kmMat4RotationPitchYawRoll(kmMat4* pOut, const kmScalar pitch, const kmScalar yaw, const kmScalar roll) {
    return ::kmMat4RotationPitchYawRoll(pOut, pitch, yaw, roll);
}

kmMat4* const unlinked::kmMat4Scaling(kmMat4* pOut, const kmScalar x, const kmScalar y, const kmScalar z) {
    return ::kmMat4Scaling(pOut, x, y, z);
}

#endif
