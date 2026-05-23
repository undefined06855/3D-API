#include "ThirdDimensionObject.hpp"
#include <geode.devtools/include/API.hpp>
#include <OBJ_Loader.h>
#include "inline-defs.hpp"

using namespace ThreeDeeAPI;

struct ThirdDimensionObject::Impl final {
    bool dirty = true;
    kmMat4 transformation = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

    float positionZ = 0.f;
    float rotationZ = 0.f;
    float scaleZ = 1.f;

    std::optional<objl::Mesh> mesh;
    std::optional<objl::Material> material;

    GLuint vertexBuffer = 0;
    GLuint indexBuffer = 0;
    GLuint vertexArray = 0;

    geode::Ref<cocos2d::CCTexture2D> texture = nullptr;
};

ThirdDimensionObject::ThirdDimensionObject() : m_impl(std::make_unique<Impl>()) {};
ThirdDimensionObject::~ThirdDimensionObject() {
    glDeleteBuffers(1, &m_impl->vertexBuffer);
    glDeleteBuffers(1, &m_impl->indexBuffer);
    glDeleteVertexArrays(1, &m_impl->vertexArray);
};

ThirdDimensionObject* ThirdDimensionObject::create() {
    auto ret = new ThirdDimensionObject;
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool ThirdDimensionObject::init() {
    if (!CCNode::init()) return false;

    glGenBuffers(1, &m_impl->vertexBuffer);
    glGenBuffers(1, &m_impl->indexBuffer);
    glGenVertexArrays(1, &m_impl->vertexArray);

    this->setShaderProgram(cocos2d::CCShaderCache::sharedShaderCache()->programForKey("default_shader"_spr));

    return true;
}

geode::Result<> ThirdDimensionObject::loadObject(geode::ZStringView object, bool flipUVs) {
    std::string path = cocos2d::CCFileUtils::get()->fullPathForFilename(object.c_str(), true);

    auto loader = objl::Loader();
    if (!loader.LoadFile(geode::utils::string::pathToString(path))) {
        return geode::Err("failed to load file");
    }

    if (loader.LoadedMeshes.size() == 0) {
        return geode::Err("zero meshes found");
    }

    m_impl->mesh = std::move(loader.LoadedMeshes.at(0));

    if (flipUVs) {
        for (auto& vertex : m_impl->mesh->Vertices) {
            vertex.TextureCoordinate.Y = 1.f - vertex.TextureCoordinate.Y;
        }
    }

    glBindVertexArray(m_impl->vertexArray);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_impl->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_impl->mesh->Indices.size() * sizeof(unsigned int), m_impl->mesh->Indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_impl->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_impl->mesh->Vertices.size() * sizeof(objl::Vertex), m_impl->mesh->Vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(cocos2d::kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)offsetof(objl::Vertex, Position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)offsetof(objl::Vertex, Normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(cocos2d::kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)offsetof(objl::Vertex, TextureCoordinate));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    if (loader.LoadedMaterials.size() != 0) {
        m_impl->material = std::move(loader.LoadedMaterials.at(0));
        m_impl->texture = cocos2d::CCTextureCache::get()->addImage(/* diffuse */ m_impl->material->map_Kd.c_str(), false);
    }

    if (!m_impl->texture) {
        m_impl->texture = cocos2d::CCTextureCache::get()->addImage("default.png"_spr, false);
    }

    return geode::Ok();
}

void ThirdDimensionObject::setPositionZ(float positionZ) {
    m_impl->positionZ = positionZ;
    m_impl->dirty = true;
}

void ThirdDimensionObject::setRotationZ(float rotationZ) {
    m_impl->rotationZ = rotationZ;
    m_impl->dirty = true;
}

void ThirdDimensionObject::setScaleZ(float scaleZ) {
    m_impl->scaleZ = scaleZ;
    m_impl->dirty = true;
}

void ThirdDimensionObject::setPosition3D(const cocos2d::CCPoint3D& position) {
    this->setPositionX(position.x);
    this->setPositionY(position.y);
    this->setPositionZ(position.z);
}

void ThirdDimensionObject::setRotation3D(const cocos2d::CCPoint3D& rotation) {
    this->setRotationX(rotation.x);
    this->setRotationY(rotation.y);
    this->setRotationZ(rotation.z);
}

void ThirdDimensionObject::setScale3D(const cocos2d::CCPoint3D& scale) {
    this->setScaleX(scale.x);
    this->setScaleY(scale.y);
    this->setScaleZ(scale.z);
}

void ThirdDimensionObject::setTexture(cocos2d::CCTexture2D* texture) {
    // since texture is a ref this should safely release the previous one and retain this new one
    m_impl->texture = texture;
}

float ThirdDimensionObject::getPositionZ() { return m_impl->positionZ; }
float ThirdDimensionObject::getRotationZ() { return m_impl->rotationZ; }
float ThirdDimensionObject::getScaleZ() { return m_impl->scaleZ; }

cocos2d::CCPoint3D ThirdDimensionObject::getPosition3D() { return { this->getPositionX(), this->getPositionY(), this->getPositionZ() }; }
cocos2d::CCPoint3D ThirdDimensionObject::getRotation3D() { return { this->getRotationX(), this->getRotationY(), this->getRotationZ() }; }
cocos2d::CCPoint3D ThirdDimensionObject::getScale3D() { return { this->getScaleX(), this->getScaleY(), this->getScaleZ() }; }

GLuint ThirdDimensionObject::getVertexBuffer() { return m_impl->vertexBuffer; }
GLuint ThirdDimensionObject::getIndexBuffer() { return m_impl->indexBuffer; }
GLuint ThirdDimensionObject::getVertexArray() { return m_impl->vertexArray; }

cocos2d::CCTexture2D* ThirdDimensionObject::getTexture() { return m_impl->texture.data(); }

void ThirdDimensionObject::setPosition(const cocos2d::CCPoint& newPosition) {
    CCNode::setPosition(newPosition);
    m_impl->dirty = true;
}

void ThirdDimensionObject::setRotation(float newRotation) {
    CCNode::setRotation(newRotation);
    m_impl->rotationZ = newRotation;
    m_impl->dirty = true;
}

void ThirdDimensionObject::setRotationX(float fRotationX) {
    CCNode::setRotationX(fRotationX);
    m_impl->dirty = true;
}

void ThirdDimensionObject::setRotationY(float fRotationY) {
    CCNode::setRotationY(fRotationY);
    m_impl->dirty = true;
}

void ThirdDimensionObject::setScale(float scale) {
    CCNode::setScale(scale);
    m_impl->scaleZ = scale;
    m_impl->dirty = true;
}

void ThirdDimensionObject::setScale(float fScaleX, float fScaleY) {
    CCNode::setScale(fScaleX, fScaleY);
    m_impl->dirty = true;
}

void ThirdDimensionObject::setScaleX(float newScaleX) {
    CCNode::setScaleX(newScaleX);
    m_impl->dirty = true;
}

void ThirdDimensionObject::setScaleY(float newScaleY) {
    CCNode::setScaleY(newScaleY);
    m_impl->dirty = true;
}

void ThirdDimensionObject::draw(const kmMat4& projection) {
    if (!m_impl->mesh) return;

    auto shader = this->getShaderProgram();
    shader->use();
    shader->setUniformsForBuiltins();

    if (m_impl->dirty) {
        kmMat4Identity(&m_impl->transformation);

        kmMat4 translation;
        kmMat4Translation(&translation, this->getPositionX(), this->getPositionY(), this->getPositionZ());

        kmMat4 rotation;
        unlinked::kmMat4RotationPitchYawRoll(&rotation, kmDegreesToRadians(this->getRotationX()), kmDegreesToRadians(this->getRotationY()), kmDegreesToRadians(this->getRotationZ()));

        kmMat4 scale;
        unlinked::kmMat4Scaling(&scale, this->getScaleX(), this->getScaleY(), this->getScaleZ());

        // translation * (rotation * scale)
        // https://gamedev.stackexchange.com/a/16721
        kmMat4Multiply(&m_impl->transformation, &rotation, &scale);
        kmMat4Multiply(&m_impl->transformation, &translation, &m_impl->transformation);

        m_impl->dirty = false;
    }

    kmMat4 mvpMatrix;
    kmMat4Multiply(&mvpMatrix, &projection, &m_impl->transformation);
    auto loc = shader->getUniformLocationForName("u_mvpMatrix");
    shader->setUniformLocationWithMatrix4fv(loc, mvpMatrix.mat, 1);

    cocos2d::ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    cocos2d::ccGLBindTexture2D(m_impl->texture->getName());

    glBindVertexArray(m_impl->vertexArray);
    glDrawElements(
        GL_TRIANGLES,
        m_impl->mesh->Indices.size(),
        GL_UNSIGNED_INT,
        0
    );
    glBindVertexArray(0);
}

void ThirdDimensionObject::registerWithDevTools() {
    devtools::registerNode<ThirdDimensionObject>([](ThirdDimensionObject* object) {
        auto pos = object->getPosition3D();
        if (devtools::property("X Position", pos.x)) { object->setPosition3D(pos); }
        if (devtools::property("Y Position", pos.y)) { object->setPosition3D(pos); }
        if (devtools::property("Z Position", pos.z)) { object->setPosition3D(pos); }

        auto rot = object->getRotation3D();
        if (devtools::property("X Rotation", rot.x)) { object->setRotation3D(rot); }
        if (devtools::property("Y Rotation", rot.y)) { object->setRotation3D(rot); }
        if (devtools::property("Z Rotation", rot.z)) { object->setRotation3D(rot); }

        auto scale = object->getScale3D();
        if (devtools::property("X Scale", scale.x)) { object->setScale3D(scale); }
        if (devtools::property("Y Scale", scale.y)) { object->setScale3D(scale); }
        if (devtools::property("Z Scale", scale.z)) { object->setScale3D(scale); }

        auto mat = object->m_impl->transformation.mat;
        devtools::label(fmt::format(
            "Transformation Matrix:\n    {} {} {} {}\n    {} {} {} {}\n    {} {} {} {}\n    {} {} {} {}",
            mat[0], mat[4], mat[8],  mat[12],
            mat[1], mat[5], mat[9],  mat[13],
            mat[2], mat[6], mat[10], mat[14],
            mat[3], mat[7], mat[11], mat[15]
        ));
    });
}

$on_mod(Loaded) {
    devtools::waitForDevTools([] {
        ThirdDimensionObject::registerWithDevTools();
    });
}
