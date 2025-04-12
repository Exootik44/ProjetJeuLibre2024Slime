#pragma once

class Transform
{
public:
    Transform();

    void Identity();

    void ResetRotation();

    void ResetRoll();

    void FollowTarget(const DirectX::XMFLOAT3& target, float speed);

    void LookAt(const DirectX::XMFLOAT3& target);

    void UpdateMatrix();

    void Rotation(float roll, float pitch, float yaw);

    void Move(float dirFront, float dirRight, float dirUp);

    void Scale(float scaleX, float scaleY, float scaleZ);

    DirectX::XMFLOAT4X4& GetMatrix() { return matrix; }

    DirectX::XMFLOAT3& GetScale() { return vScale; }

    DirectX::XMFLOAT3& GetPositionF3() { return vPosition; }
    float GetPositionX() { return vPosition.x; }
    float GetPositionY() { return vPosition.y; }
    float GetPositionZ() { return vPosition.z; }

    void AddToGlobalPosX(float deltaX);
    void AddToGlobalPosY(float deltaY);
    void AddToGlobalPosZ(float deltaZ);

    // m : matrice, v : vector, q : quaternion
    DirectX::XMFLOAT3 vPosition;
    DirectX::XMFLOAT3 vScale;

    DirectX::XMFLOAT4 qRotation;
    DirectX::XMFLOAT4X4 mRotation;

    DirectX::XMFLOAT3 vFront;
    DirectX::XMFLOAT3 vRight;
    DirectX::XMFLOAT3 vUp;

    DirectX::XMFLOAT4X4 matrix; // matrice contenant les coordonees et rotation

    DirectX::XMFLOAT3 m_respawnPosition;
};

