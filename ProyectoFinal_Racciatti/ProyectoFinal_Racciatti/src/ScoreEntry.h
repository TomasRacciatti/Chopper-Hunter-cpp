#pragma once
#include <string>

struct ScoreEntry
{
    std::string name;
    int   score = 0; 
    float time = 0.f;

    bool operator<(const ScoreEntry& highScore) const
    {
        if (score != highScore.score) return score > highScore.score;
        if (time != highScore.time) return time > highScore.time;
        return name < highScore.name;
    }
};
