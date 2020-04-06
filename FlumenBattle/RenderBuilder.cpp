#include "FlumenEngine/Core/Engine.hpp"
#include "FlumenEngine/Render/Screen.hpp"
#include "FlumenEngine/Render/BufferManager.hpp"
#include "FlumenEngine/Render/Texture.hpp"
#include "FlumenEngine/Render/TextureManager.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/FrameBuffer.hpp"
#include "FlumenEngine/Render/ShadowFrameBuffer.hpp"
#include "FlumenEngine/Render/StencilFrameBuffer.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"

#include "RenderBuilder.hpp"
#include "FlumenBattle/Types.hpp"

RenderBuilder* RenderBuilder::instance_ = new RenderBuilder();

void RenderBuilder::Initialize()
{
	auto screen = Engine::GetScreen();

	FrameBuffer** frameBuffer = BufferManager::GetFrameBuffers().Add(FrameBuffers::DEFAULT);
	if(frameBuffer)
	{
		*frameBuffer = new FrameBuffer(screen->GetSize());

		RenderManager::SetDefaultFrameBuffer(*frameBuffer);
	}

	//auto screenTexture = new Texture(screen->GetSize(), TextureFormats::FOUR_BYTE);
	//TextureManager::AddTexture(screenTexture, "Screen");

	auto backgroundColor = Color(0.5f, 0.2f, 0.7f, 1.0f);
	RenderManager::SetBackgroundColor(backgroundColor);
}

RenderBuilder::RenderBuilder()
{
	RenderManager::OnInitialize()->Add(&RenderBuilder::Initialize);
}
