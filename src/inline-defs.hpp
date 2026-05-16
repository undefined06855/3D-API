#pragma once

namespace unlinked {

kmMat4* const kmMat4RotationPitchYawRoll(kmMat4* pOut, const kmScalar pitch, const kmScalar yaw, const kmScalar roll);
kmMat4* const kmMat4Scaling(kmMat4* pOut, const kmScalar x, const kmScalar y, const kmScalar z);

}
