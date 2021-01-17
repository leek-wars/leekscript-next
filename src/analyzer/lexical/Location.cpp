#include "Location.hpp"
#include "../resolver/File.hpp"

namespace ls {

bool Position::operator == (const Position& l) const {
	return raw == l.raw;
}

bool Position::operator < (const Position& p) const {
	return raw < p.raw;
}

Position Position::after() const {
	return { line, column + 1, raw + 1 };
}

Json Position::json() const {
	return Json::array({
		line, column, raw
	});
}

bool Location::operator == (const Location& l) const {
	return &file == &l.file and start == l.start and end == l.end;
}

bool Location::operator < (const Location& l) const {
	if (file.path < l.file.path) return true;
	if (start < l.start) return true;
	if (end < l.end) return true;
	return false;
}

bool Location::contains(size_t position) const {
	return position >= start.raw and position <= end.raw;
}

Location Location::after() const {
	return { file, end.after(), end.after() };
}

Json Location::json() const {
	return Json::array({
		start.json(), end.json()
	});
}

}