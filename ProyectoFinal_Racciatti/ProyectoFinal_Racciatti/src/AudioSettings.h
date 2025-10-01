#pragma once

struct AudioSettings 
{
    float master = 50.f;
    float music = 100.f;
    float sfx = 100.f;

    // Hacer funciones publicas de getters y 
    // setters para los sliders y audios y 
    // dejar los otros 3 en priv
};
