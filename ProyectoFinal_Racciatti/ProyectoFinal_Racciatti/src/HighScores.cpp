#include "HighScores.h"
#include <fstream>
#include <sstream>
#include <algorithm>

std::ostream& operator<<(std::ostream& os, const ScoreEntry& entry)
{
	std::string safe = entry.name;
	std::replace(safe.begin(), safe.end(), '|', ' ');

	os << safe << '|' << entry.score << '|' << entry.time;
	return os;
}

std::istream& operator>>(std::istream& is, ScoreEntry& entry)
{
	std::string line;
	
	if (!std::getline(is, line))
		return is;

	std::istringstream ls(line);
	std::string scoreStr, timeStr;

	if (!std::getline(ls, entry.name, '|'))
	{ 
		is.setstate(std::ios::failbit); 
		return is; 
	}

	if (!std::getline(ls, scoreStr, '|'))
	{ 
		is.setstate(std::ios::failbit); 
		return is; 
	}

	if (!std::getline(ls, timeStr))
	{ 
		is.setstate(std::ios::failbit); 
		return is; 
	}

	try 
	{
		entry.score = std::stoi(scoreStr);
		entry.time = std::stof(timeStr);
	}
	catch (...) 
	{
		is.setstate(std::ios::failbit);
	}
	return is;
}


bool HighScores::Load()
{
	_entries.clear();

	std::ifstream file(_path);
	if (!file.is_open()) return false;

	ScoreEntry row;
	while (file >> row) 
		_entries.push_back(row);

	std::sort(_entries.begin(), _entries.end()); // Aca basicamente usamos el operador sobrecargado de <
	
	if (_entries.size() > maxEntries) 
		_entries.resize(maxEntries);

	return true;
}

bool HighScores::Save() const
{
	std::ofstream file(_path, std::ios::trunc);
	if (!file.is_open()) return false;

	for (std::size_t i = 0; i < _entries.size(); ++i) {
		file << _entries[i];
		if (i + 1 < _entries.size()) file << '\n';
	}
	return true;
}

void HighScores::Submit(const ScoreEntry& e)
{
	_entries.push_back(e);
	std::sort(_entries.begin(), _entries.end());
	if (_entries.size() > maxEntries) _entries.resize(maxEntries);
}
