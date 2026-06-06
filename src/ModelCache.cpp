#include "ModelCache.hpp"

using namespace ThreeDeeAPI;

ModelCache::ModelCache()
    : m_cache() {}

ModelCache& ModelCache::get() {
    static ModelCache instance;
    return instance;
}

geode::Result<std::shared_ptr<objl::Loader>> ModelCache::getLoaderFromData(geode::ZStringView data, geode::ZStringView mtlSearchPath) {
    auto hashed = fmt::to_string(geode::utils::hash(data));
    if (m_cache.contains(hashed)) {
        geode::log::trace("cache hit for loader with hash {}", hashed);
        return geode::Ok(m_cache[hashed]);
    }

    auto loader = objl::Loader();
    if (!loader.LoadFileRaw(data, mtlSearchPath)) {
        return geode::Err("failed to load file");
    }

    if (loader.LoadedMeshes.size() == 0) {
        return geode::Err("zero meshes found");
    }

    m_cache[hashed] = std::make_shared<objl::Loader>(std::move(loader));

    geode::log::trace("cache miss for loader with hash {}", hashed);
    return geode::Ok(m_cache[hashed]);
}

geode::Result<std::shared_ptr<objl::Loader>> ModelCache::getLoaderFromCache(geode::ZStringView path) {
    if (m_cache.contains(path)) {
        geode::log::trace("loaded loader from cache with path {}", path);
        return geode::Ok(m_cache[path]);
    }

    auto loader = objl::Loader();
    if (!loader.LoadFile(geode::utils::string::pathToString(path))) {
        return geode::Err("failed to load file");
    }

    if (loader.LoadedMeshes.size() == 0) {
        return geode::Err("zero meshes found");
    }

    m_cache[path] = std::make_shared<objl::Loader>(std::move(loader));

    geode::log::trace("cache miss for loader with path {}", path);
    return geode::Ok(m_cache[path]);
}
