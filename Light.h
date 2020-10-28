#pragma once
#include "PointShadowMap.h"
#include "vec3.h"

struct DirectionalLight {
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    PointShadowMap ShadowMap;
};