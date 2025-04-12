#include "pch.h"
#include "TextureManager.h"

bool TextureManager::LoadTexture(const std::wstring& id, const std::wstring& filename)
{
    ComPtr<ID3D12Resource> texture;
    ComPtr<ID3D12Resource> textureUploadHeap;
    HRESULT hr = DirectX::CreateDDSTextureFromFile12(mp_device, mp_cmdList,
        filename.c_str(),
        texture,
        textureUploadHeap);
    if (FAILED(hr))
        return false;

    // Stocker la texture pour eviter qu'elle ne soit detrsuite
    m_textures[id] = texture;
    
    // Conserver l'upload heap dans le vector pour qu'il reste valide
    m_uploadHeaps.push_back(textureUploadHeap);

    // Creer le SRV pour la texture
    // Le Shader Resource View (SRV) est un descripteur qui definit comment une ressource (ici une texture) est vue par les shaders. Il precise notamment 
    // le format de la texture, le nombre de mipmaps et la dimension de la ressource. Gr‚ce au SRV, le shader (souvent le pixel shader) peut echantillonner 
    // la texture pour en extraire les donnees de couleur lors du rendu.
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = texture->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = texture->GetDesc().MipLevels;

    // Calculer l'offset Åa utiliser : on prend le prochain slot libre dans le heap
    UINT offset = static_cast<UINT>(m_textureOffsets.size());
    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_srvHeap->GetCPUDescriptorHandleForHeapStart(), offset, m_descriptorSize);
    mp_device->CreateShaderResourceView(texture.Get(), &srvDesc, handle);

    m_textureOffsets[id] = offset;
    return true;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetTextureHandle(const std::wstring& id) const
{
    auto it = m_textureOffsets.find(id);
    if (it == m_textureOffsets.end())
    {
        // Retour d'un handle invalide si l'id n'existe pas
        return {};
    }
    UINT offset = it->second;
    CD3DX12_GPU_DESCRIPTOR_HANDLE handle(m_srvHeap->GetGPUDescriptorHandleForHeapStart(), offset, m_descriptorSize);
    return handle;
}

void TextureManager::CreateDescriptorHeap(UINT numDescriptors)
{
    // Creer un descriptor heap pour le SRV 
    // Le descriptor heap est une zone memoire reservee sur la carte graphique qui stocke plusieurs descripteurs. Dans le cas du SRV, il contient les 
    // descriptions de ressources (comme les textures) afin que le GPU puisse y acceder directement lors du rendu. Une fois cree et rempli, le heap permet 
    // de regrouper et de gerer ces descripteurs de faÁon efficace et de les rendre visibles aux shaders.
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = numDescriptors;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    // On doit le rendre visible aux shaders
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    HRESULT hr = mp_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_srvHeap));
    if (FAILED(hr))
    {
        // Gerer l'erreur
    }
    m_descriptorSize = mp_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
