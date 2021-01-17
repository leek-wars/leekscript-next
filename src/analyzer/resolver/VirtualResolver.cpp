#include "VirtualResolver.hpp"
#include <iostream>
#include "../../util/Util.hpp"
#include <unordered_map>
#include "../Program.hpp"
#include "../../colors.h"

namespace ls {

static std::unordered_map<std::string, std::unique_ptr<File>> file_cache;

File* VirtualResolver::create(Environment& env, std::string path, std::string code) const {
	auto fspath = std::filesystem::path(path);
	return file_cache.emplace(path, new File(env, path, code, FileContext(fspath.parent_path()))).first->second.get();
}

File* VirtualResolver::delete_(std::string path) const {
	auto fspath = std::filesystem::path(path);
	auto file = file_cache[path].release();
	file_cache.erase(path);
	return file;
}

File* VirtualResolver::resolve(Environment& env, std::string path, FileContext context) const {
	auto resolvedPath = (context.folder / path).lexically_normal();
	auto i = file_cache.find(resolvedPath);
	if (i != file_cache.end()) {
		return i->second.get();
	}
	// std::cout << "[" << C_YELLOW << "warning" << END_COLOR << "] Virtual file " << BOLD << resolvedPath << END_STYLE << " does not exist in context " << context.folder << std::endl;
	return nullptr;
}

std::unordered_map<std::string, std::unique_ptr<File>>& VirtualResolver::get_cache() {
	return file_cache;
}

}