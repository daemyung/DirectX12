//
// This file is part of the "DirectX12" project
// See "LICENSE" for license information.
//

#include <common/window.h>
#include <common/example.h>
#include <iostream>
#include <memory>

//----------------------------------------------------------------------------------------------------------------------

const std::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, UINT> kDescriptorCount = {
        {D3D12_DESCRIPTOR_HEAP_TYPE_RTV,         kSwapChainBufferCount},
        {D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kImGuiFontBufferCount}};

//----------------------------------------------------------------------------------------------------------------------

class Template : public Example {
public:
    Template() : Example("Template", kDescriptorCount) {
    }

protected:
    void OnInit() override {
    }

    void OnTerm() override {
    }

    void OnResize(const Resolution& resolution) override {
        // Update a viewport.
        _viewport.Width = static_cast<float>(GetWidth(resolution));
        _viewport.Height = static_cast<float>(GetHeight(resolution));

        // Update a scissor rect.
        _scissor_rect.right = GetWidth(resolution);
        _scissor_rect.bottom = GetHeight(resolution);

        D3D12_CPU_DESCRIPTOR_HANDLE handle;

        handle = _descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV]->GetCPUDescriptorHandleForHeapStart();
        for (auto i = 0; i != kSwapChainBufferCount; ++i) {
            _device->CreateRenderTargetView(_swap_chain_buffers[i].Get(), nullptr, handle);
            handle.ptr += _descriptor_heap_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_RTV];
        }
    }

    void OnUpdate(UINT index) override {
    }

    void OnRender(UINT index) override {
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = _swap_chain_buffers[index].Get();
        barrier.Transition.Subresource = 0;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

        _command_list->ResourceBarrier(1, &barrier);

        D3D12_CPU_DESCRIPTOR_HANDLE handle;

        handle = _descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV]->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += _descriptor_heap_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] * index;

        _command_list->ClearRenderTargetView(handle, DirectX::Colors::LightSteelBlue, 0, nullptr);
        _command_list->OMSetRenderTargets(1, &handle, true, nullptr);
        _command_list->RSSetViewports(1, &_viewport);
        _command_list->RSSetScissorRects(1, &_scissor_rect);

        RecordDrawImGuiCommands();

        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

        _command_list->ResourceBarrier(1, &barrier);
    }

private:
    D3D12_VIEWPORT _viewport = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    D3D12_RECT _scissor_rect = {0, 0, 0, 0};
};

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    try {
        auto example = std::make_unique<Template>();
        Window::GetInstance()->MainLoop(example.get());
    }
    catch (const std::exception &exception) {
        std::cerr << exception.what() << std::endl;
    }
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------