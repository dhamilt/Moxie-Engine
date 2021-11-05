#include "glPCH.h"
#include "MViewport.h"
#include "GLSetup.h"

extern GLSetup* GGLSPtr;
MViewport::MViewport()
{
	name = typeid(*this).name();

	// Grab the viewport texture/render target
	GGLSPtr->GetViewport(renderingLayers, numOfRenderingLayers, rbo, fbo);	
	// Retrieve the initial dimensions of the viewport
	GGLSPtr->GetViewportDimensions(width, height);	
	// Get the size of the pixel buffer for the viewport
	pixelBufferSize = width * height * colorChannelsPerPixel;
	// Texture for viewport
	//glGenTextures(1, &viewportTexture);	
	// allocate memory to store the pixel buffer
	pixelData.resize(pixelBufferSize);
	// cache the current pixel count
	pixelCount = width * height;
}

MViewport::~MViewport()
{	
	// Remove renderbuffer ref
	if (rbo > 0)
		rbo = NULL;
	// Clear out pixel data cache
	if (pixelData.size() > 0)
		pixelData.clear();
	// Clear out texture refs
	if (renderingLayers.size() > 0)
		renderingLayers.clear();
	
}

void MViewport::Paint()
{	

	// Bind the render buffer
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Viewport", &windowOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
	ImVec2 windowSize = ImGui::GetWindowSize();
	// If the viewport image has been resized
	if ((int)windowSize.x != width || (int)windowSize.y != height)
	{
		// Cache the new dimensions
		width = (int)windowSize.x;
		height = (int)windowSize.y;
		// Change the perspective view to match
		//  that of the new viewport dimensions
		GGLSPtr->SetViewportDimensions(width, height);

		// Store the pixel data for this frame
		pixelCount = width * height;
		// if the cached pixel buffer size is too small
		if (pixelBufferSize < pixelCount * colorChannelsPerPixel)
		{
			// Resize and cache it
			pixelBufferSize = pixelCount * colorChannelsPerPixel;
			pixelData.resize(pixelBufferSize * 2);
		}
	}
	glBindTexture(GL_TEXTURE_2D, viewportTexture);
	// Retrieve the current framebuffer object for the current render pass
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	// Grab pixel data from combined framebuffer
	glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, &pixelData[0]);
	// Pass it into the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, &pixelData[0]);
	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	GLuint fbResult = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fbResult == GL_FRAMEBUFFER_COMPLETE)
		ImGui::Image((ImTextureID)(intptr_t)viewportTexture, ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));
	else
		FrameBufferErrorCheck(glCheckFramebufferStatus(GL_FRAMEBUFFER));

	ImGui::End();
	ImGui::PopStyleVar();
	// Unbind the texture context and layered texture
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

bool MViewport::operator==( GUI_Base& other)
{
	return GetName() == other.GetName();
}

bool MViewport::FrameBufferErrorCheck(GLuint status)
{
	bool result = false;

	switch (status)
	{
		case GL_FRAMEBUFFER_UNDEFINED:
			printf("\rFramebuffer is not defined.");
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			printf("\rFramebuffer attachments are incomplete.");
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			printf("\rFramebuffer does not have at least one image attached to it.");
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			printf("\rDraw buffer cannot not be found in framebuffer.");
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			printf("\rRead buffer cannot not be found in framebuffer.");
			break;

		case GL_FRAMEBUFFER_UNSUPPORTED:
			printf("\rThe internal formats for this framebuffer are not supported");
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			printf("\rFramebuffer samples are not all the same for the attached render buffers.");
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			printf("\rFramebuffer is layered and the attachment is not.");
			break;

		default:
			result = true;
			break;
	}
	return result;
}
