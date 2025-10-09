#include "Artillery.h"
#include "Utils.h"
#include "Combat.h"
#include "ExplosionEffect.h"
#include <algorithm>
#include <cmath>

namespace
{
    constexpr float groundSafety = 32.f;
}