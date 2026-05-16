#include "inline-defs.hpp"

#ifdef GEODE_IS_IOS

kmMat4* const unlinked::kmMat4RotationPitchYawRoll(kmMat4* pOut, const kmScalar pitch, const kmScalar yaw, const kmScalar roll) {
    kmMat4 yaw_matrix;
    kmMat4 roll_matrix;
    kmMat4 pitch_matrix;

    kmMat4RotationY(&yaw_matrix, yaw);

    kmMat4RotationX(&pitch_matrix, pitch);

    kmMat4RotationZ(&roll_matrix, roll);

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
