#include "Graphics.h"
#include <stdio.h>
#include <GLUT/glut.h>
#include <assert.h>

#include "Vector4f.h"
#include "Matrix4f.h"

Graphics Graphics::g; // �V���O���g���̃C���X�^���X

/**
 * init():
 */
void Graphics::init() {
	// �P�ʍs�񂾂ƌ��_����+z�����𒭂߂Ă��邱�ƂɂȂ�.
	camera.setIdentity();
}

/**
 * setCamera():
 */
void Graphics::setCamera(const Matrix4f& camera_) {
	camera = camera_;
	worldCamera.invertRT(camera);
}

/**
 * drawLine():
 */
void Graphics::drawLine( const Vector4f& pos0, 
						 const Vector4f& pos1, 
						 const Vector4f& color ) {

	Vector4f pos0_;
	Vector4f pos1_;

	worldCamera.transform(pos0, pos0_);
	worldCamera.transform(pos1, pos1_);

	glColor3f(color.x, color.y, color.z);
	glBegin(GL_LINES);
	glVertex3f(pos0_.x, pos0_.y, pos0_.z);
	glVertex3f(pos1_.x, pos1_.y, pos1_.z);
	glEnd();
}
