#pragma once
#include <OBJ_Loader.h>

namespace ThreeDeeAPI {

class ModelCache {
    ModelCache();
public:
    static ModelCache& get();

    geode::utils::StringMap<std::shared_ptr<objl::Loader>> m_cache;

    geode::Result<std::shared_ptr<objl::Loader>> getLoaderFromData(geode::ZStringView data, geode::ZStringView mtlSearchPath);
    geode::Result<std::shared_ptr<objl::Loader>> getLoaderFromCache(geode::ZStringView path);
};

}

