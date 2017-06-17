#include <math.h>
#include "TrackBall.h"

#define TRANS_RATE        (0.15f)
#define ZOOM_RATE         (0.04f)

static void setQuatFromAxisAngle(Quat4f& q,
								 const Vector4f& axis,
								 float angle) {
	const float halfAngle = angle * 0.5f;

	float sin = sinf(halfAngle);
	float cos = cosf(halfAngle);

	q.set(sin * axis.x, sin * axis.y, sin * axis.z, cos);

	if( q.lengthSquared() == 0.0f) {
		q.set(0.0f, 0.0f, 0.0f, 1.0f);
	}
}

/**
 * TrackBall():
 *
 * pitchAngle = ��p(+���Ə����������)
 */
TrackBall::TrackBall(float eyex_, float eyey_, float eyez_, 
					 float pitchAngle) {
	eyex = eyex_;
	eyey = eyey_;
	eyez = eyez_;

	lastX = lastY = 0;
	trans.setIdentity();

	const Vector4f axisX(1.0f, 0.0f, 0.0f, 0.0f);

	setQuatFromAxisAngle(q, axisX, pitchAngle);
	width = height = 0;
}

/**
 * resize():
 */
void TrackBall::resize(int width_, int height_) {
	width = width_;
	height = height_;
}

/**
 * getMat():
 */
void TrackBall::getMat(Matrix4f& mat) const {
	Matrix4f eyeTrans;
	eyeTrans.setIdentity();
	eyeTrans.setColumn(3, Vector4f(eyex, eyey, eyez, 1.0f));

	mat.set(q);
	mat *= trans;
	mat *= eyeTrans;
}

/**
 * startRotation():
 */
void TrackBall::startRotation(int x, int y) {
	// �h���b�O�̍ŏ��ɋ��ʏ�̓_lastpos���o���Ă���
	sphericalMap(x, y, lastSpherialPos);
}

/**
 * startZoom():
 */
void TrackBall::startZoom(int x, int y) {
	lastX = x;
	lastY = y;
}

/**
 * startTrans():
 */
void TrackBall::startTrans(int x, int y) {
	lastX = x;
	lastY = y;
}

/**
 * dragRotation():
 */
void TrackBall::dragRotation(int x, int y) {
	// x, y�����ʏ�̓_�Ƀ}�b�s���O���A
	// �O��̃}�b�s���O�_�Ƃ̋�������ъO�ς���
	// axis���o��.
	// axis[]����angle�x��]�������̂ƁAq��
	// �����ĐV����q�ɂ���.
	Vector4f axis;
	float angle;
	calcAxisAngle(x, y, axis, angle);

	axis *= -3.0f;

	Quat4f dq;
	setQuatFromAxisAngle(dq, axis, angle);
	q *= dq;
	q.normalize();
}

/**
 * dragZoom():
 */
void TrackBall::dragZoom(int x, int y) {
	// translation�}�g���N�X���X�V����.
	float len = ZOOM_RATE * (float)(y - lastY);

	float transZ = trans.getElement(2, 3);
	transZ -= len;
	trans.setElement(2, 3, transZ);

	lastX = x;
	lastY = y;
}

/**
 * dragTrans():
 */
void TrackBall::dragTrans(int x, int y) {
	float dx = TRANS_RATE * float(x - lastX);
	float dy = TRANS_RATE * float(y - lastY);

	float tx = trans.getElement(0, 3);
	float ty = trans.getElement(1, 3);

	tx -= dx;
	ty -= dy;

	trans.setElement(0, 3, tx);
	trans.setElement(1, 3, ty);
	
	lastX = x;
	lastY = y;
}

/**
 * calcAxisAngle():
 *
 * x, y��sphericalMap�ɂċ���̓_(curpos)�ɕϊ����A
 * lastpos�Ƃ̋����ɂ��angle���A
 * lastpos�Ƃ̊O�ςɂ��axis�����߂�.
 */
void TrackBall::calcAxisAngle(int x, int y, 
							  Vector4f& axis,
							  float& angle) {

	Vector4f curSphericalPos;
	Vector4f dv;

	sphericalMap(x, y, curSphericalPos);
	dv.sub(curSphericalPos, lastSpherialPos);
	
	angle = PI * 0.5f * dv.length();
	axis.cross(lastSpherialPos, curSphericalPos);

	lastSpherialPos = curSphericalPos;
}

/**
 * sphericalMap():
 * 
 * [x, y�𔼌a1.0�̋����3�����_�ɑΉ�������]
 *
 *      -1.0x        +1.0x
 *        +-----+-----+   1.0y
 *        +     +     +
 *        +-----+-----+
 *        +     +     +
 *        +-----+-----+  -1.0y
 *
 *    ��O+z�����ŁA��O�ɔ����ʂ�����C���[�W
 */
void TrackBall::sphericalMap(int x, int y, Vector4f& v) {
	// windows�̏�[��y=0�ŗ���.
	float d;
	v.x =  (2.0f * x - width)  / width;
	v.y = -(2.0f * y - height) / height;
	d = sqrtf(v.x*v.x + v.y*v.y);
	if(d >= 1.0f) {
		d = 1.0f;
	}
	v.z = cosf(PI * 0.5f * d);
	v.w = 0.0f;

	v.normalize();
}
