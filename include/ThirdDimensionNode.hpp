#pragma once
#include "structs.hpp"

// fuck you
#undef near
#undef far

namespace ThreeDeeAPI {

enum class CameraType { Perspective, Orthographic };
struct CameraClippingBounds { float near, far; };

/**
 * A 3D node wrapper, all ThirdDimensionObjects should go inside one of these.
 * Set camera properties on this too, and ensure it has a non-zero content size!
 */
class THREEDEE_API_DLL ThirdDimensionNode : public cocos2d::CCSprite {
protected:
    ThirdDimensionNode();

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    cocos2d::CCTexture2D* createTextures();
    void deleteManagedTextures();

public:
    ~ThirdDimensionNode();

    /**
     * Creates a ThirdDimensionNode with a pre-specified resolution for the texture. You should specify a resolution in
     * points then multiply it by the CCDirector contentScaleFactor.
     * @param resolution The resolution, in pixels.
     */
    static ThirdDimensionNode* createWithResolution(cocos2d::CCSize resolution);

    /**
     * Creates a ThirdDimensionNode and sets the size to be the window size, in pixels.
     */
    static ThirdDimensionNode* create();
    virtual bool init(cocos2d::CCSize resolution);

    /**
     * Sets the position of the camera in 3D space.
     * Default is { 5.f, 5.f, 5.f }.
     * @param pos The position.
     */
    void setCameraPosition(cocos2d::CCPoint3D pos);

    /**
     * Sets the point in 3D space that the camera aims at.
     * Default is { 0.f, 0.f, 0.f }.
     * @param pos The position.
     */
    void setCameraAim(cocos2d::CCPoint3D pos);

    /**
     * Sets the FOV in degrees. Keep in mind this is the vertical FOV, not the horizontal FOV, like it is in other
     * programs. When using an orthographic camera, this will be the height of the camera in cocos units.
     * Default is 90.f.
     * @param fov The FOV in degrees, or height of the camera view.
     */
    void setCameraFOV(float fov);

    /**
     * Sets the camera type to either be a perspective camera, or orthographic camera.
     * Default is CameraType::Perspective.
     * @param type The type of camera.
     */
    void setCameraType(CameraType type);

    /**
     * Sets the near and far clip planes for the camera.
     * Default is { .1f, 2000.f }.
     * @param bounds The clipping plane distances.
     */
    void setClippingBounds(CameraClippingBounds bounds);

    /**
     * Sets whether the resolution of the texture is "locked", as in, resizing the node won't recreate the texture and
     * resize it.
     * Default is false.
     * @param locked Whether it's locked or not.
     */
    void setResolutionLocked(bool locked);

    cocos2d::CCPoint3D getCameraPosition();
    cocos2d::CCPoint3D getCameraAim();
    float getCameraFOV();
    bool getResolutionLocked();
    CameraType getCameraType();
    CameraClippingBounds getClippingBounds();

    GLuint getFBO();
    GLuint getColorTexture();
    GLuint getDepthTexture();

    virtual void setContentSize(const cocos2d::CCSize& size) override;

    virtual void draw() override;
};

}
