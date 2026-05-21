#include "ThirdDimensionNode.hpp"
#include "ThirdDimensionObject.hpp"
#include <geode.devtools/include/API.hpp>

using namespace ThreeDeeAPI;

// taken from gd-render-texture
struct HackCCTexture2D : cocos2d::CCTexture2D {
    bool initWithGLName(GLuint name, GLsizei pixelsWidth, GLsizei pixelsHeight, const cocos2d::CCSize& contentSize) {
        m_uName = name;

        m_tContentSize = contentSize;
        m_uPixelsWide = pixelsWidth;
        m_uPixelsHigh = pixelsHeight;
        m_ePixelFormat = cocos2d::kCCTexture2DPixelFormat_RGBA8888;
        m_fMaxS = contentSize.width / (float)(pixelsWidth);
        m_fMaxT = contentSize.height / (float)(pixelsHeight);

        m_bHasPremultipliedAlpha = false;
        m_bHasMipmaps = false;

        this->setShaderProgram(cocos2d::CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTexture));
        return true;
    }
};

// if you update defaults here, make sure to update the docs in the header file
// also you probably shouldn't be updating defaults anyway since people might rely on them
struct ThirdDimensionNode::Impl final {
    bool dirty = true;
    kmMat4 projectionViewMatrix = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

    cocos2d::CCPoint3D pos = { 5.f, 5.f, 5.f };
    cocos2d::CCPoint3D aim = { 0.f, 0.f, 0.f };
    float fov = 90.f;

    CameraType type = CameraType::Perspective;
    CameraClippingBounds clippingBounds = { .1f, 2000.f };

    GLuint frameBuffer = 0;
    GLuint colorTexture = 0;
    GLuint depthTexture = 0;

    bool resolutionLocked = false;
    bool textureDirty = false;
    cocos2d::CCSize textureSize = { 0.f, 0.f };
};

ThirdDimensionNode::ThirdDimensionNode() : m_impl(std::make_unique<Impl>()) {};
ThirdDimensionNode::~ThirdDimensionNode() {
    this->deleteManagedTextures();
}

void ThirdDimensionNode::deleteManagedTextures() {
    // note: ccsprite's cctexture2d will release the colour texture for us

    glDeleteFramebuffers(1, &m_impl->frameBuffer);
    glDeleteRenderbuffers(1, &m_impl->depthTexture);
}

ThirdDimensionNode* ThirdDimensionNode::createWithResolution(cocos2d::CCSize resolution) {
    auto ret = new ThirdDimensionNode;
    if (ret->init(resolution)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

ThirdDimensionNode* ThirdDimensionNode::create() {
    auto director = cocos2d::CCDirector::get();
    return ThirdDimensionNode::createWithResolution(director->getWinSize() * director->getContentScaleFactor());
}

bool ThirdDimensionNode::init(cocos2d::CCSize resolution) {
    m_impl->textureSize = resolution;
    auto texture = this->createTextures();

    if (!texture) return false;
    if (!CCSprite::initWithTexture(texture)) return false;

    return true;
}

cocos2d::CCTexture2D* ThirdDimensionNode::createTextures() {
    auto resolution = m_impl->textureSize;

    GLint origFramebuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &origFramebuffer);

    glGenFramebuffers(1, &m_impl->frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_impl->frameBuffer);

    glGenTextures(1, &m_impl->colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_impl->colorTexture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        resolution.width,
        resolution.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        m_impl->colorTexture,
        0
    );

    glGenRenderbuffers(1, &m_impl->depthTexture);
    glBindRenderbuffer(GL_RENDERBUFFER, m_impl->depthTexture);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GEODE_DESKTOP(GL_DEPTH_COMPONENT24) GEODE_MOBILE(GL_DEPTH_COMPONENT16),
        resolution.width,
        resolution.height
    );

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        m_impl->depthTexture
    );

    glBindFramebuffer(GL_FRAMEBUFFER, origFramebuffer);

    auto texture = new HackCCTexture2D;
    if (texture->initWithGLName(m_impl->colorTexture, resolution.width, resolution.height, resolution)) {
        texture->autorelease();
        return texture;
    }

    delete texture;
    return nullptr;
}

void ThirdDimensionNode::setCameraPosition(cocos2d::CCPoint3D pos) {
    m_impl->pos = pos;
    m_impl->dirty = true;
}

void ThirdDimensionNode::setCameraAim(cocos2d::CCPoint3D pos) {
    m_impl->aim = pos;
    m_impl->dirty = true;
}

void ThirdDimensionNode::setCameraFOV(float fov) {
    m_impl->fov = fov;
    m_impl->dirty = true;
}

void ThirdDimensionNode::setCameraType(CameraType type) {
    m_impl->type = type;
    m_impl->dirty = true;
}

void ThirdDimensionNode::setClippingBounds(CameraClippingBounds bounds) {
    m_impl->clippingBounds = bounds;
    m_impl->dirty = true;
}

void ThirdDimensionNode::setResolutionLocked(bool locked) {
    m_impl->resolutionLocked = locked;
}

void ThirdDimensionNode::setContentSize(const cocos2d::CCSize& size) {
    CCNode::setContentSize(size);
    if (!m_impl->resolutionLocked) {
        m_impl->textureSize = size * cocos2d::CCDirector::get()->getContentScaleFactor();
        m_impl->dirty = true;
        m_impl->textureDirty = true;
    }
}

cocos2d::CCPoint3D ThirdDimensionNode::getCameraPosition() { return m_impl->pos; }
cocos2d::CCPoint3D ThirdDimensionNode::getCameraAim() { return m_impl->aim; }
float ThirdDimensionNode::getCameraFOV() { return m_impl->fov; }
CameraType ThirdDimensionNode::getCameraType() { return m_impl->type; }
CameraClippingBounds ThirdDimensionNode::getClippingBounds() { return m_impl->clippingBounds; }
bool ThirdDimensionNode::getResolutionLocked() { return m_impl->resolutionLocked; }

GLuint ThirdDimensionNode::getFBO() { return m_impl->frameBuffer; }
GLuint ThirdDimensionNode::getColorTexture() { return m_impl->colorTexture; }
GLuint ThirdDimensionNode::getDepthTexture() { return m_impl->depthTexture; }

void ThirdDimensionNode::draw() {
    if (m_impl->dirty) {
        // create projection matrix
        kmMat4 proj;
        auto winSize = cocos2d::CCDirector::get()->getWinSizeInPixels();

        if (m_impl->type == CameraType::Perspective) {
            kmMat4PerspectiveProjection(&proj, m_impl->fov, m_impl->textureSize.aspect(), m_impl->clippingBounds.near, m_impl->clippingBounds.far);
        } else {
            kmMat4OrthographicProjection(&proj, 0.f, m_impl->fov * m_impl->textureSize.aspect(), 0.f, m_impl->fov, m_impl->clippingBounds.near, m_impl->clippingBounds.far);
        }

        // create view matrix
        kmMat4 view;
        kmVec3 eye = { m_impl->pos.x, m_impl->pos.y, m_impl->pos.z };
        kmVec3 center = { m_impl->aim.x, m_impl->aim.y, m_impl->aim.z };
        kmVec3 up = { 0, 1, 0 };
        kmMat4LookAt(&view, &eye, &center, &up);

        kmMat4Multiply(&m_impl->projectionViewMatrix, &proj, &view);

        m_impl->dirty = false;
    }

    if (m_impl->textureDirty) {
        this->deleteManagedTextures();
        auto texture = this->createTextures();
        this->setTexture(texture);
        auto scaleFactor = cocos2d::CCDirector::get()->getContentScaleFactor();
        this->setTextureRect({ 0.f, 0.f, m_impl->textureSize.width / scaleFactor, m_impl->textureSize.height / scaleFactor });

        m_impl->textureDirty = false;
    }

    GLint origFramebuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &origFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_impl->frameBuffer);

    GLint origViewport[4];
    glGetIntegerv(GL_VIEWPORT, origViewport);

    glViewport(0.f, 0.f, m_impl->textureSize.width, m_impl->textureSize.height);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto child : geode::cocos::CCArrayExt<ThirdDimensionObject>(this->getChildren())) {
        child->draw(m_impl->projectionViewMatrix);
    }
    glDisable(GL_DEPTH_TEST);

    glViewport(origViewport[0], origViewport[1], origViewport[2], origViewport[3]);

    glBindFramebuffer(GL_FRAMEBUFFER, origFramebuffer);

    CCSprite::draw();
}

$on_mod(Loaded) {
    devtools::waitForDevTools([] {
        devtools::registerNode<ThirdDimensionNode>([](ThirdDimensionNode* node) {
            auto pos = node->getCameraPosition();
            if (devtools::property("X Position", pos.x)) { node->setCameraPosition(pos); }
            if (devtools::property("Y Position", pos.y)) { node->setCameraPosition(pos); }
            if (devtools::property("Z Position", pos.z)) { node->setCameraPosition(pos); }

            auto aim = node->getCameraAim();
            if (devtools::property("X Aim", aim.x)) { node->setCameraAim(aim); }
            if (devtools::property("Y Aim", aim.y)) { node->setCameraAim(aim); }
            if (devtools::property("Z Aim", aim.z)) { node->setCameraAim(aim); }

            auto fov = node->getCameraFOV();
            if (devtools::property("FOV", fov)) { node->setCameraFOV(fov); }
        });
    });
}
