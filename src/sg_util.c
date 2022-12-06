#include "sg_util.h"


float lerp(float a, float b, float t){
    return a + t * (b - a);
}

Vector3D lerpv(Vector3D a, Vector3D b, float t){
    Vector3D lerpd;
    lerpd.x = lerp(a.x, b.x, t);
    lerpd.y = lerp(a.y, b.y, t);
    lerpd.z = lerp(a.z, b.z, t);
    return lerpd;
}