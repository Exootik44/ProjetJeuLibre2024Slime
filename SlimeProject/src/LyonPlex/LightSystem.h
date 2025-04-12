#pragma once

class InitDirect3DApp;

struct DirectionalLight {
    DirectX::XMFLOAT3 Direction;
    float pad1;
    DirectX::XMFLOAT3 Color;
    float pad2;
};

struct PointLight {
    DirectX::XMFLOAT3 Position;
    float pad1;
    DirectX::XMFLOAT3 Color;
    float pad2;
    float ConstantAtt;
    float LinearAtt;
    float QuadraticAtt;
    float pad3;
};

struct PassConstants {
    DirectX::XMFLOAT4 AmbientLight;
    DirectionalLight DirLight;
    PointLight PtLight;
};

class LightSystem {
protected:
    InitDirect3DApp* mp_gameManager;

public:
    void Initialize(InitDirect3DApp* gameManager);
    void Update(float deltaTime);
    const PassConstants& GetPassConstants() const { return m_passConstants;
    }
private:
    PassConstants m_passConstants;
};
