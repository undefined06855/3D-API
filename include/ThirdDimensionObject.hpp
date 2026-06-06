#pragma once
#include "structs.hpp"

namespace ThreeDeeAPI {

/**
 * A 3D object with a mesh and optional texture. This should be put inside a ThirdDimensionNode!
 */
class THREEDEE_API_DLL ThirdDimensionObject : public cocos2d::CCNode {
protected:
    ThirdDimensionObject();

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    // loader is std::shared_ptr<obj::Loader> but i dont want to include the header in a public header file
    geode::Result<> loadObjectFromLoader(auto loader);
public:
    ~ThirdDimensionObject();

    static void registerWithDevTools();

    static ThirdDimensionObject* create();
    virtual bool init() override;

    /**
     * Loads a 3D object given a path (e.g. undefined0.3d-api/teapot.obj). You should call this after create, and check
     * the return value.
     * @param object The resource to load the object from, prefixed with mod id using _spr.
     */
    geode::Result<> loadObject(geode::ZStringView object);

    /**
     * Loads a 3D object given the contents of a (text) .obj file. You should call this after create, and check
     * the return value.
     * @param data The data of the .obj file.
     * @param mtlSearchPathParent The path of the parent directory of the mtl file, probably your mod's resources
     * directory.
     */
    geode::Result<> loadRawObjFile(geode::ZStringView data, std::filesystem::path mtlSearchPathParent);

    virtual void setPositionZ(float positionZ);
    virtual void setRotationZ(float rotationZ);
    virtual void setScaleZ(float scaleZ);

    virtual void setPosition3D(const cocos2d::CCPoint3D& position);
    virtual void setRotation3D(const cocos2d::CCPoint3D& rotation);
    virtual void setScale3D(const cocos2d::CCPoint3D& scale);

    void setTexture(cocos2d::CCTexture2D* texture);

    virtual float getPositionZ();
    virtual float getRotationZ();
    virtual float getScaleZ();

    virtual cocos2d::CCPoint3D getPosition3D();
    virtual cocos2d::CCPoint3D getRotation3D();
    virtual cocos2d::CCPoint3D getScale3D();

    GLuint getVertexBuffer();
    GLuint getIndexBuffer();
    GLuint getVertexArray();

    cocos2d::CCTexture2D* getTexture();

    virtual void setPosition(const cocos2d::CCPoint& newPosition) override;
    virtual void setRotation(float newRotation) override;
    virtual void setRotationX(float fRotationX) override;
    virtual void setRotationY(float fRotationY) override;
    virtual void setScale(float scale) override;
    virtual void setScale(float fScaleX, float fScaleY) override;
    virtual void setScaleX(float newScaleX) override;
    virtual void setScaleY(float newScaleY) override;

    /**
     * Draws the 3D object with a projection * view matrix (passed from ThirdDimensionNode).
     * You probably don't need to call this yourself!
     * @param projection The projection view matrix.
     */
    void draw(const kmMat4& projection);
};

}
