float32_t4x4 MakeAffineMatrix(float32_t3 scale, float32_t3 rotate, float32_t3 translate)
{
    float32_t4x4 scaleMatrix = float32_t4x4(
        scale.x, 0.0f, 0.0f, 0.0f,
        0.0f, scale.y, 0.0f, 0.0f,
        0.0f, 0.0f, scale.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    
    float32_t cosX = cos(rotate.x);
    float32_t sinX = sin(rotate.x);
    float32_t4x4 rotateXMatrix = float32_t4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cosX, sinX, 0.0f,
        0.0f, -sinX, cosX, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    
    float32_t cosY = cos(rotate.y);
    float32_t sinY = sin(rotate.y);
    float32_t4x4 rotateYMatrix = float32_t4x4(
        cosY, 0.0f, -sinY, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sinY, 0.0f, cosY, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    
    float32_t cosZ = cos(rotate.z);
    float32_t sinZ = sin(rotate.z);
    float32_t4x4 rotateZMatrix = float32_t4x4(
        cosZ, sinZ, 0.0f, 0.0f,
        -sinZ, cosZ, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    
    float32_t4x4 rotateMatrix = mul(rotateZMatrix, mul(rotateYMatrix, rotateXMatrix));
    
    float32_t4x4 translateMatrix = float32_t4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        translate.x, translate.y, translate.z, 1.0f);
    
    return mul(translateMatrix, mul(rotateMatrix, scaleMatrix));
}