#include "glPCH.h"
#include "MViewport.h"
#include "GLSetup.h"

extern GLSetup* GGLSPtr;
MViewport::MViewport()
{
	name = typeid(*this).name();

	// Grab the viewport texture/render target
	GGLSPtr->GetViewportTextureID(textureID, rbo);
	// Retrieve the initial dimensions of the viewport
	GGLSPtr->GetViewportDimensions(width, height);	
}

MViewport::~MViewport()
{
	if (fbo > 0)
		glDeleteFramebuffers(1, &fbo);
	if (rbo > 0)
		glDeleteRenderbuffers(1, &rbo);
	if (pixelData)
		free(pixelData);
	if (textureID > 0)
		glDeleteTextures(1, &textureID);
	
}

void MViewport::Paint()
{	
	// Bind the texture
	glBindTexture(GL_TEXTURE_2D, textureID);	
	
	// Bind the render buffer
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);	
	
	ImGui::Begin("Viewport", &windowOpen);
	ImVec2 windowSize = ImGui::GetWindowSize();
	// If the viewport image has been resized
	if ((int)windowSize.x != width || (int)windowSize.y != height)
	{
		// Cache the new dimensions
		width = (int)windowSize.x;
		height = (int)windowSize.y;
		// Store the pixel data for this frame
		pixelCount = width * height;
		if (!pixelData)
			pixelData = (GLubyte*)malloc(sizeof(char) * pixelCount * bytesPerPixel);
		else
			pixelData = (GLubyte*)realloc(pixelData,sizeof(char) * pixelCount * bytesPerPixel);

		assert(pixelData);

		// Resize the color attachment texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
		// As well as the depth attachment
		// Store the depth and stencil data into the render buffer
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		ImGui::Image((ImTextureID)textureID, ImGui::GetWindowSize(),ImVec2(0, 0), ImVec2(1,-1));
	
	else
		FrameBufferErrorCheck(glCheckFramebufferStatus(GL_FRAMEBUFFER));

	ImGui::End();

	
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
