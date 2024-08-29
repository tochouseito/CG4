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
float4x4 MakeBillboardMatrix(float4x4 cameraMatrix)
{
    // Rotate 180 degrees around the Y-axis to make the object face the camera
    float4x4 backToFrontMatrix = float4x4(
        -1.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 1.0f, 0.0f, 0.0f,
         0.0f, 0.0f, -1.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 1.0f
    );

    // Multiply the rotation matrix with the camera matrix to get the billboard matrix
    float4x4 billboardMatrix = mul(backToFrontMatrix, cameraMatrix);

    // Set the translation part of the matrix to zero
    billboardMatrix[3][0] = 0.0f;
    billboardMatrix[3][1] = 0.0f;
    billboardMatrix[3][2] = 0.0f;

    return billboardMatrix;
}