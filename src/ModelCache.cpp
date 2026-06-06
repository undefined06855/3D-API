#include "ModelCache.hpp"

using namespace ThreeDeeAPI;

ModelCache::ModelCache()
    : m_cache() {}

ModelCache& ModelCache::get() {
    static ModelCache instance;
    return instance;
}

// is it just me or is this code like really ugly? it's like not really if you really read it but it just feels bad and
// i really cant tell why
// ...maybe im just tired

geode::Result<std::shared_ptr<Model>> ModelCache::getModelFromData(geode::ZStringView data, geode::ZStringView mtlSearchPath) {
    auto hashed = fmt::to_string(geode::utils::hash(data));
    if (m_cache.contains(hashed)) {
        geode::log::trace("cache hit for model with hash {}", hashed);
        return geode::Ok(m_cache[hashed]);
    }

    auto loader = objl::Loader();
    if (!loader.LoadFileRaw(data, mtlSearchPath)) {
        return geode::Err("failed to load file");
    }

    GEODE_UNWRAP_INTO(m_cache[hashed], this->handleLoader(loader));

    geode::log::trace("cache miss for model with hash {}", hashed);
    return geode::Ok(m_cache[hashed]);
}

geode::Result<std::shared_ptr<Model>> ModelCache::getModelFromCache(geode::ZStringView path) {
    if (m_cache.contains(path)) {
        geode::log::trace("loaded model from cache with path {}", path);
        return geode::Ok(m_cache[path]);
    }

    auto loader = objl::Loader();
    if (!loader.LoadFile(geode::utils::string::pathToString(path))) {
        return geode::Err("failed to load file");
    }

    GEODE_UNWRAP_INTO(m_cache[path], this->handleLoader(loader));

    geode::log::trace("cache miss for model with path {}", path);
    return geode::Ok(m_cache[path]);
}

geode::Result<std::shared_ptr<Model>> ModelCache::handleLoader(objl::Loader& loader) {
    if (loader.LoadedMeshes.size() == 0) {
        return geode::Err("zero meshes found");
    }

    Model model;
    model.mesh = std::make_shared<objl::Mesh>(std::move(loader.LoadedMeshes[0]));

    if (loader.LoadedMaterials.size() > 0) {
        model.material = std::make_shared<objl::Material>(std::move(loader.LoadedMaterials[0]));
    } else {
        model.material = nullptr;
    }

    return geode::Ok(std::make_shared<Model>(std::move(model)));
}
