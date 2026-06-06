#pragma once
#include <OBJ_Loader.h>

namespace ThreeDeeAPI {

struct Model {
    std::shared_ptr<objl::Mesh> mesh;
    std::shared_ptr<objl::Material> material;
};

class ModelCache {
    ModelCache();
public:
    static ModelCache& get();

    geode::utils::StringMap<std::shared_ptr<Model>> m_cache;

    geode::Result<std::shared_ptr<Model>> getModelFromData(geode::ZStringView data, geode::ZStringView mtlSearchPath);
    geode::Result<std::shared_ptr<Model>> getModelFromCache(geode::ZStringView path);

    geode::Result<std::shared_ptr<Model>> handleLoader(objl::Loader& loader);
};

}

