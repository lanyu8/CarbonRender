#include "..\Inc\CRCamera.h"

Camera::Camera()
{
	objType = ObjectType::eCamera;
}

Camera::~Camera()
{
}

void Camera::SetPerspectiveCamera(float iFov, float iNearClip, float iFarClip)
{
	fov = iFov;
	nearClip = iNearClip;
	farClip = iFarClip;
	WindowSize wSize = WindowManager::Instance()->GetWindowSize();
	wInPixel = wSize.w;
	hInPixel = wSize.h;
	curCameraMode = CameraProjectMode::ePerspective;

	UpdateProjectionMatrix();
}

void Camera::SetPerspectiveCamera(float iFov, float iNearClip, float iFarClip, int size)
{
	fov = iFov;
	nearClip = iNearClip;
	farClip = iFarClip;
	wInPixel = size;
	hInPixel = size;
	curCameraMode = CameraProjectMode::ePerspective;

	UpdateProjectionMatrix();
}

void Camera::SetNearClip(float iNearClip)
{
	nearClip = iNearClip;

	UpdateProjectionMatrix();
}

float Camera::GetNearClip()
{
	return nearClip;
}

void Camera::SetFarClip(float iFarClip)
{
	farClip = iFarClip;

	UpdateProjectionMatrix();
}

float Camera::GetFarClip()
{
	return farClip;
}

void Camera::SetFov(float iFov)
{
	fov = iFov;

	UpdateProjectionMatrix();
}

float Camera::GetFov()
{
	return fov;
}

void Camera::SetOrthoSize(float size)
{
	orthoSize = size;

	UpdateProjectionMatrix();
}

float3 Camera::GetCameraPara()
{
	return float3(fov, nearClip, farClip);
}

void Camera::SetOrthoCamera(float size, float iNearClip, float iFarClip)
{
	orthoSize = size;
	nearClip = iNearClip;
	farClip = iFarClip;
	WindowSize wSize = WindowManager::Instance()->GetWindowSize();
	wInPixel = wSize.w;
	hInPixel = wSize.h;
	curCameraMode = CameraProjectMode::eOrtho;

	UpdateProjectionMatrix();
}

void Camera::SetOrthoCamera(float size, float iNearClip, float iFarClip, float sideSize)
{
	orthoSize = size;
	nearClip = iNearClip;
	farClip = iFarClip;
	wInPixel = sideSize;
	hInPixel = sideSize;
	curCameraMode = CameraProjectMode::eOrtho;

	UpdateProjectionMatrix();
}

void Camera::UpdateProjectionMatrix()
{
	switch (curCameraMode)
	{
	default:
		break;
	case CameraProjectMode::ePerspective:
	{
		float halfFOV = fov * 0.5f;
		float hT = 1.0f / (nearClip * tan(halfFOV * A2R) * 2.0f);
		float wT = hInPixel*hT / wInPixel;

		float m[16] = { 2.0f*nearClip*wT, 0.0f, 0.0f, 0.0f,
						0.0f, 2.0f*nearClip*hT, 0.0f, 0.0f,
						0.0f, 0.0f, (farClip + nearClip) / (nearClip - farClip), -1.0f,
						0.0f, 0.0f, 2 * nearClip*farClip / (nearClip - farClip), 0.0f };

		projectionMatrix = m;
	}
	break;
	case CameraProjectMode::eOrtho:
	{
		float halfW = (orthoSize * wInPixel) / hInPixel;

		float m[16] = { 1.0f / halfW, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f / orthoSize, 0.0f, 0.0f,
						0.0f, 0.0f, 2.0f / (nearClip - farClip), 0.0f,
						0.0f, 0.0f, (farClip + nearClip) / (nearClip - farClip), 1.0f };
		
		projectionMatrix = m;
	}
	break;
	}
}

void Camera::UpdateViewMatrix()
{
	{
		float4 xAxis(1.0f, 0.0f, 0.0f, 0.0f);
		float4 yAxis(0.0f, 1.0f, 0.0f, 0.0f);
		float4 zAxis(0.0f, 0.0f, 1.0f, 0.0f);

		modelMatrix = Math::Translate(-transform[0], -transform[1], -transform[2]);
		modelMatrix = modelMatrix * Math::Rotate(xAxis, -transform[6]).Normailze().ToMatrix();

		yAxis = yAxis * Math::Rotate(xAxis, -transform[6]).Normailze().ToMatrix();
		zAxis = zAxis * Math::Rotate(xAxis, -transform[6]).Normailze().ToMatrix();

		modelMatrix = modelMatrix * Math::Rotate(float3(yAxis), -transform[7]).Normailze().ToMatrix();

		xAxis = xAxis * Math::Rotate(float3(yAxis), -transform[7]).Normailze().ToMatrix();
		zAxis = zAxis * Math::Rotate(float3(yAxis), -transform[7]).Normailze().ToMatrix();

		modelMatrix = modelMatrix * Math::Rotate(float3(zAxis), -transform[8]).Normailze().ToMatrix();

		xAxis = xAxis * Math::Rotate(float3(zAxis), -transform[8]).Normailze().ToMatrix();
		yAxis = zAxis * Math::Rotate(float3(zAxis), -transform[8]).Normailze().ToMatrix();
	}
	
	//update localCoord
	{
		float4 xAxis(1.0f, 0.0f, 0.0f, 0.0f);
		float4 yAxis(0.0f, 1.0f, 0.0f, 0.0f);
		float4 zAxis(0.0f, 0.0f, 1.0f, 0.0f);

		//yAxis = yAxis * Math::Rotate(xAxis, transform[6]).Normailze().ToMatrix();
		zAxis = zAxis * Math::Rotate(xAxis, transform[6]).Normailze().ToMatrix();

		xAxis = xAxis * Math::Rotate(yAxis, transform[7]).Normailze().ToMatrix();
		zAxis = zAxis * Math::Rotate(yAxis, transform[7]).Normailze().ToMatrix();

		xAxis = xAxis * Math::Rotate(zAxis, transform[8]).Normailze().ToMatrix();
		//yAxis = zAxis * Math::Rotate(zAxis, transform[8]).Normailze().ToMatrix();

		localCoord[0] = xAxis.x;
		localCoord[1] = xAxis.y;
		localCoord[2] = xAxis.z;

		localCoord[3] = yAxis.x;
		localCoord[4] = yAxis.y;
		localCoord[5] = yAxis.z;

		localCoord[6] = zAxis.x;
		localCoord[7] = zAxis.y;
		localCoord[8] = zAxis.z;
	}
}

Matrix4x4 Camera::GetViewMatrix()
{
	return modelMatrix;
}

Matrix4x4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}
