#pragma once
#include <string>
#include <vector>
#include <cstddef>
#include <iosfwd>
#include "ScoreEntry.h"

class HighScores
{
public:
	explicit HighScores(std::string savePath = "../save/highscores.txt")
		: _path(std::move(savePath)) {}

	bool Load();
	bool Save() const;

	void Submit(const ScoreEntry& entry);
	void Clear() { _entries.clear(); }

	const std::vector<ScoreEntry>& Top() const { return _entries; }
	static constexpr std::size_t MaxEntries() { return maxEntries; }

private:
	std::string _path;
	std::vector<ScoreEntry> _entries;
	static constexpr std::size_t maxEntries = 10;
};

std::ostream& operator<<(std::ostream& os, const ScoreEntry& entry);
std::istream& operator>>(std::istream& is, ScoreEntry& entry);