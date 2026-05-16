#pragma once
#include "structs.hpp"

namespace ThreeDeeAPI {

/**
 * A 3D node wrapper, all ThirdDimensionObjects should go inside one of these.
 * Set camera properties on this too, and ensure it has a non-zero content size!
 */
class THREEDEE_API_DLL ThirdDimensionNode : public cocos2d::CCSprite {
protected:
    ThirdDimensionNode();
    ~ThirdDimensionNode();

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    cocos2d::CCTexture2D* createTextures();
    void deleteManagedTextures();

public:
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

    void setCameraPosition(cocos2d::CCPoint3D pos);
    void setCameraAim(cocos2d::CCPoint3D pos);
    void setCameraFOV(float fov);

    /**
     * Sets whether the resolution of the texture is "locked", as in, resizing the node won't recreate the texture and
     * resize it.
     * @param locked Whether it's locked or not.
     */
    void setResolutionLocked(bool locked);

    cocos2d::CCPoint3D getCameraPosition();
    cocos2d::CCPoint3D getCameraAim();
    float getCameraFOV();
    bool getResolutionLocked();

    GLuint getFBO();
    GLuint getColorTexture();
    GLuint getDepthTexture();

    virtual void setContentSize(const cocos2d::CCSize& size) override;

    virtual void draw() override;
};

}
