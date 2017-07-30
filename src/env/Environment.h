// -*- C++ -*-
#ifndef ENVIRONMENT_HEADER
#define ENVIRONMENT_HEADER

#include "btBulletDynamicsCommon.h"
#include <math.h>
#include <map>
#include <set>
using namespace std;

#include "MeshManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "RigidBodyComponent.h"

class Action;
class Matrix4f;
class Vector3f;
class Renderer;
class DebugDrawer;
class Camera;
class EnvironmentObject;
class AgentObject;


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
	CollisionShapeManager collisionShapeManager;
	btBroadphaseInterface* broadPhase;
	btCollisionDispatcher* dispatcher;
	btConstraintSolver*	solver;
	btDefaultCollisionConfiguration* configuration;
	btDiscreteDynamicsWorld* world;

	AgentObject* agent;
	Renderer* renderer;
	int nextObjId;
	set<int> collidedIds;
	map<int, EnvironmentObject*> objectMap; // <obj-id, EnvironmentObject>

	DebugDrawer* debugDrawer;
	MeshManager meshManager;
	TextureManager textureManager;
	ShaderManager shaderManager;

	bool initRenderer(int width, int height, bool offscreen);
	void prepareAgent(int floorObjId);
	void checkCollision();
	int addObject(btCollisionShape* shape,
				  float posX, float posY, float posZ,
				  float rot,
				  bool detectCollision,
				  const Mesh* mesh,
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
		renderer(nullptr),
		nextObjId(0),
		debugDrawer(nullptr) {
	}

	~Environment() {
	}

	bool init(int width, int height, bool offscreen);
	void release();
	void step(const Action& action, int stepNum, bool agentView);
	int addBox(float halfExtentX, float halfExtentY, float halfExtentZ,
			   float posX, float posY, float posZ,
			   float rot,
			   bool detectCollision);
	int addSphere(float radius,
				  float posX, float posY, float posZ,
				  float rot,
				  bool detectCollision);
	int addModel(const char* path,
				 float scaleX, float scaleY, float scaleZ,
				 float posX, float posY, float posZ,
				 float rot,
				 bool detectCollision);
	void removeObject(int id);
	void locateAgent(float posX, float posY, float posZ,
					 float rot);

	const void* getFrameBuffer() const;
	int getFrameBufferWidth() const;
	int getFrameBufferHeight() const;
	int getFrameBufferSize() const;
	void setRenderCamera(const Matrix4f& mat);

	const set<int>& getCollidedIds() const { return collidedIds; }
	void updateCameraToAgentView();
};

#endif
