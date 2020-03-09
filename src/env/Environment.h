// -*- C++ -*-
#ifndef ENVIRONMENT_HEADER
#define ENVIRONMENT_HEADER

#include "btBulletDynamicsCommon.h"
#include <math.h>
#include <map>
#include <set>
#include <vector>
#include <string>
using namespace std;

#include "MeshManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "RigidBodyComponent.h"
#include "RenderingContext.h"
#include "glinc.h"
#include "GLContext.h"

class Action;
class Matrix4f;
class Vector3f;
class Camera;
class EnvironmentObject;
class EnvironmentObjectInfo;
class AgentObject;
class CameraView;


// TODO: collision shapes sould be cached
class CollisionShapeManager {
private:
	btAlignedObjectArray<btCollisionShape*>	collisionShapes;

public:
	~CollisionShapeManager();
	
	btCollisionShape* getSphereShape(float radius);
	btCollisionShape* getBoxShape(float halfExtentX,
								  float halfExtentY,
								  float halfExtentZ);
	btCollisionShape* getCylinderShape(float halfExtentX,
									   float halfExtentY,
									   float halfExtentZ);
};

class Environment {
private:
	CollisionShapeManager collisionShapeManager;
	btBroadphaseInterface* broadPhase;
	btCollisionDispatcher* dispatcher;
	btConstraintSolver*	solver;
	btDefaultCollisionConfiguration* configuration;
	btDiscreteDynamicsWorld* world;

	AgentObject* agent;
	int nextObjId;
	set<int> collidedIds;
	map<int, EnvironmentObject*> objectMap; // <obj-id, EnvironmentObject>

	MeshManager meshManager;
	TextureManager textureManager;
	ShaderManager shaderManager;
    GLContext glContext;
	RenderingContext renderingContext;
    vector<CameraView*> cameraViews;

	//bool initRenderer(int width, int height, const Vector3f& bgColor);
	void prepareAgent();
	void checkCollision();
	void prepareShadow();
	int addObject(btCollisionShape* shape,
				  const Vector3f& pos,
				  const Quat4f& rot,
				  float mass,
				  const Vector3f& relativeCenter,
				  bool detectCollision,
				  Mesh* mesh,
				  const Vector3f& scale);
	EnvironmentObject* findObject(int id);

public:
	Environment()
		:
		broadPhase(nullptr),
		dispatcher(nullptr),
		solver(nullptr),
		configuration(nullptr),
		world(nullptr),
		agent(nullptr),
		nextObjId(0) {
	}

	~Environment() {
	}

	bool init();
    int addCameraView(int width, int height, const Vector3f& bgColor,
                      float nearClip, float farClip, float focalLength,
                      int shadowBufferWidth);
	void release();
	void step(const Action& action, int stepNum);
	int addBox(const char* texturePath,
			   const Vector3f& halfExtent,
			   const Vector3f& pos,
			   const Quat4f& rot,
			   float mass,
			   bool detectCollision);
	int addSphere(const char* texturePath,
				  float radius,
				  const Vector3f& pos,
				  const Quat4f& rot,
				  float mass,
				  bool detectCollision);
	int addModel(const char* path,
				 const Vector3f& sale,
				 const Vector3f& pos,
				 const Quat4f& rot,
				 float mass,
				 bool detectCollision);
	void removeObject(int id);
	void locateObject(int id, const Vector3f& pos, const Quat4f& rot);
	void locateAgent(const Vector3f& pos, float rotY);
	void setLight(const Vector3f& lightDir,
				  const Vector3f& lightColor,
				  const Vector3f& ambientColor,
				  float shadowColorRate);
	bool getObjectInfo(int id, EnvironmentObjectInfo& info) const;
	bool getAgentInfo(EnvironmentObjectInfo& info) const;
	void replaceObjectTextures(int id, const vector<string>& texturePathes);

    void render(int cameraId, const Vector3f& pos, const Quat4f& rot);
	const void* getFrameBuffer(int cameraId) const;
	int getFrameBufferWidth(int cameraId) const;
	int getFrameBufferHeight(int cameraId) const;
	int getFrameBufferSize(int cameraId) const;

	const set<int>& getCollidedIds() const { return collidedIds; }
};

#endif
