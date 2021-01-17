#include "FileResolver.hpp"
#include <iostream>
#include "../../util/Util.hpp"
#include <unordered_map>
#include "../Program.hpp"

namespace ls {

// static std::unordered_map<std::string, File*> file_cache;

File* FileResolver::create(Environment& env, std::string path, std::string code) const {
	auto fspath = std::filesystem::path(path);
	return /* file_cache[path] = */ new File(env, path, code, FileContext(fspath.parent_path()));
}

File* FileResolver::resolve(Environment& env, std::string path, FileContext context) const {
	auto resolvedPath = (context.folder / path).lexically_normal();
	// auto i = file_cache.find(resolvedPath);
	// if (i != file_cache.end()) {
	// 	return i->second;
	// }
	auto code = Util::read_file(resolvedPath);
	auto newContext = FileContext(resolvedPath.parent_path());
	return /* file_cache[path] = */ new File(env, path, code, newContext);
}

}