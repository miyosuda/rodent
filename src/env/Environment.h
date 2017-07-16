// -*- C++ -*-
#ifndef ENVIRONMENT_HEADER
#define ENVIRONMENT_HEADER

#include "btBulletDynamicsCommon.h"
#include <math.h>
#include <map>
#include <vector>
using namespace std;

#include "ShaderManager.h"

class Action {
public:
	int look;   // look left=[+], look right=[-]
	int strafe; // strafe left=[+1], strafe right=[-1]
	int move;   // forward=[+1], backward=[-1]

	Action()
		:
		look(0),
		strafe(0),
		move(0) {
	}

	Action(int look_, int strafe_, int move_)
		:
		look(look_),
		strafe(strafe_),
		move(move_) {
	}

	void set(int look_, int strafe_, int move_) {
		look   = look_;
		strafe = strafe_;
		move   = move_;
	}

	static int getActionSize() {
		return 3;
	}
};

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

class Matrix4f;

class Renderer;
class DebugDrawer;

class RigidBodyComponent {
protected:
	btDynamicsWorld* world;
	btRigidBody* body;

public:
	RigidBodyComponent(float mass,
					   float posX, float posY, float posZ,
					   float rot,
					   btCollisionShape* shape,
					   btDynamicsWorld* world,
					   int collisionId);
	virtual ~RigidBodyComponent();
	int getCollisionId() const;
	virtual void control(const Action& action);
	void getMat(Matrix4f& mat) const;
};

class AgentRigidBodyComponent : public RigidBodyComponent {
public:
	AgentRigidBodyComponent(float mass,
							float posX, float posY, float posZ,
							float rot,
							btCollisionShape* shape,
							btDynamicsWorld* world,
							int collisionId);
	virtual void control(const Action& action) override;
	void getMat(Matrix4f& mat) const;
};


class EnvironmentObject {
protected:
	RigidBodyComponent* rigidBodyComponent;

public:
	virtual ~EnvironmentObject();

	EnvironmentObject();
	int getCollisionId() const;
};

class StageObject : public EnvironmentObject {
public:
	StageObject(float posX, float posY, float posZ,
				float rot,
				btCollisionShape* shape,
				btDynamicsWorld* world,
				int collisionId);
};

class AgentObject : public EnvironmentObject {
public:	
	AgentObject(btCollisionShape* shape,
				btDynamicsWorld* world,
				int collisionId);
	void control(const Action& action);
	void getMat(Matrix4f& mat) const;
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
	vector<int> collidedIds;
	map<int, EnvironmentObject*> objectMap; // <obj-id, EnvironmentObject>

	DebugDrawer* debugDrawer;
	ShaderManager shaderManager; //..

	void prepareAgent();
	void checkCollision();

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

	void init();
	void release();
	void step(const Action& action, bool updateCamera=false);
	int addBox(float halfExtentX, float halfExtentY, float halfExtentZ,
			   float posX, float posY, float posZ,
			   float rot,
			   bool detectCollision);
	int addSphere(float radius,
				  float posX, float posY, float posZ,
				  float rot,
				  bool detectCollision);
	void removeObj(int id);
	void locateAgent(float posX, float posY, float posZ,
					 float rot);

	bool initRenderer(int width, int height, bool offscreen);
	const void* getFrameBuffer() const;
	int getFrameBufferWidth() const;
	int getFrameBufferHeight() const;
	int getFrameBufferSize() const;
	void setRenderCamera(const Matrix4f& mat);

	const vector<int>& getCollidedIds() const { return collidedIds; }
	void updateCameraToAgentView();
};

#endif
