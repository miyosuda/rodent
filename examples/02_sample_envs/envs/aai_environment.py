import gym
import gym.spaces
import rodentia
import os
import random
import numpy as np

MAX_STEP_NUM = 60 * 60  # 60 seconds * 60 frames


class AAIEnvironment(gym.Env):
    """ Animal-AI Olympics compatible environment example """
    
    metadata = {'render.modes': []}

    def __init__(self, width=84, height=84):
        super().__init__()

        # Gym setting
        self.action_space = gym.spaces.MultiDiscrete((3,3,3))
        self.observation_space = gym.spaces.Box(
            low=0,
            high=255,
            shape=(84,84,3)
        )
        self.reward_range = [-1.0, 1.0]
    
        # Where model and texture data are located
        self.data_path = os.path.dirname(
            os.path.abspath(__file__)) + "/../../data/"

        # Create environment
        self.env = rodentia.Environment(
            width=width, height=height, bg_color=[0.19, 0.3, 0.47])

        # Prepare stage objects
        self._prepare_stage()

        # Object id for collision checking
        self.plus_obj_ids_set = set()
        self.minus_obj_ids_set = set()

        # Add additional camera for top view rendering
        self.additional_camera_id = self.env.add_camera_view(256, 256,
                                                             bg_color=[0.19, 0.3, 0.47],
                                                             far=50.0,
                                                             focal_length=30.0,
                                                             shadow_buffer_width=1024)

        # Reset stage
        self.reset()

    def _prepare_stage(self):
        # Floor
        floor_texture_path = self.data_path + "floor5.png"

        self.env.add_box(
            texture_path=floor_texture_path,
            half_extent=[20.0, 1.0, 20.0],
            pos=[0.0, -1.0, 0.0],
            rot=0.0,
            detect_collision=False)

        # Wall
        wall_distance = 20.0

        wall_texture_path = self.data_path + "wall4.png"

        # -Z
        self.env.add_box(
            texture_path=wall_texture_path,
            half_extent=[wall_distance+1, 1.0, 1.0],
            pos=[0.0, 1.0, -wall_distance],
            rot=0.0,
            detect_collision=False)
        # +Z
        self.env.add_box(
            texture_path=wall_texture_path,
            half_extent=[wall_distance+1, 1.0, 1.0],
            pos=[0.0, 1.0, wall_distance],
            rot=0.0,
            detect_collision=False)
        # -X
        self.env.add_box(
            texture_path=wall_texture_path,
            half_extent=[1.0, 1.0, wall_distance+1],
            pos=[-wall_distance, 1.0, 0.0],
            rot=0.0,
            detect_collision=False)
        # +X
        self.env.add_box(
            texture_path=wall_texture_path,
            half_extent=[1.0, 1.0, wall_distance+1],
            pos=[wall_distance, 1.0, 0.0],
            rot=0.0,
            detect_collision=False)

        invisible_wall_half_height = 10.0
        # -Z (invisible)
        self.env.add_box(
            texture_path=wall_texture_path,
            half_extent=[wall_distance+1, invisible_wall_half_height, 1.0],
            pos=[0.0, invisible_wall_half_height, -wall_distance],
            rot=0.0,
            detect_collision=False,
            visible=False)
        # +Z (invisible)
        self.env.add_box(
            texture_path=wall_texture_path,
            half_extent=[wall_distance+1, invisible_wall_half_height, 1.0],
            pos=[0.0, invisible_wall_half_height, wall_distance],
            rot=0.0,
            detect_collision=False,
            visible=False)
        # -X (invisible)
        self.env.add_box(
            texture_path=wall_texture_path,
            half_extent=[1.0, invisible_wall_half_height, wall_distance+1],
            pos=[-wall_distance, invisible_wall_half_height, 0.0],
            rot=0.0,
            detect_collision=False,
            visible=False)
        # +X (invisible)
        self.env.add_box(
            texture_path=wall_texture_path,
            half_extent=[1.0, invisible_wall_half_height, wall_distance+1],
            pos=[wall_distance, invisible_wall_half_height, 0.0],
            rot=0.0,
            detect_collision=False,
            visible=False)
        
        # Add ramp and cylindar
        ramp_model_path = self.data_path + "ramp0.obj"
        self.env.add_model(path=ramp_model_path,
                           scale=[2.0, 1.0, 2.0],
                           pos=[15.0, 0.0, -2.0],
                           rot=0.0,
                           mass=0.0,
                           detect_collision=False,
                           use_mesh_collision=True)

        cylinder_model_path = self.data_path + "cylinder0.obj"
        self.env.add_model(path=cylinder_model_path,
                           scale=[3.0, 3.0, 7.0],
                           pos=[15.0, 0.0, 12.0],
                           rot=0.0,
                           mass=0.0,
                           detect_collision=False,
                           use_mesh_collision=True)

    def _locate_plus_reward_obj(self, x, y, z):
        """ Locate positive reward object """
        texture_path = self.data_path + "green.png"
        pos = [x, y, z]
        # If the object's mass is not 0, the it is simulated as a rigid body object.
        ball_mass = 0.5
        obj_id = self.env.add_sphere(
            texture_path=texture_path,
            radius=1.0,
            pos=pos,
            rot=0.0,
            mass=ball_mass,
            detect_collision=True)
        self.plus_obj_ids_set.add(obj_id)
        
    def _locate_minus_reward_obj(self, x, y, z):
        """ Locate negative reward object """
        texture_path = self.data_path + "red.png"
        pos = [x, y, z]
        # If the object's mass is not 0, the it is simulated as a rigid body object.
        ball_mass = 0.5
        obj_id = self.env.add_sphere(
            texture_path=texture_path,
            radius=1.0,
            pos=pos,
            rot=0.0,
            mass=ball_mass,
            detect_collision=True)
        self.minus_obj_ids_set.add(obj_id)

    def _reset_sub(self):
        # First clear remaining reward objects
        self._clear_objects()

        # Add reward objects
        self._locate_plus_reward_obj(x=15, y=5, z=-2)
        self._locate_minus_reward_obj(x=-5, y=1, z=-10)

        # Locate agent to default position with randomized orientation
        rot_y = 2.0 * np.pi * -0.25

        self.env.locate_agent(pos=[0, 1, 0], rot_y=rot_y)

        # Reset environment and get screen
        obs = self.env.step(action=[0, 0, 0])
        screen = obs["screen"]
        return screen

    def reset(self):
        self.step_num = 0
        return self._reset_sub()

    def _clear_objects(self):
        # Create reward objects
        for id in self.plus_obj_ids_set:
            self.env.remove_obj(id)
        for id in self.minus_obj_ids_set:
            self.env.remove_obj(id)

        self.plus_obj_ids_set = set()
        self.minus_obj_ids_set = set()

    def _convert_to_real_action(self, action):
        real_action = np.clip(action, 0, 2) - 1
        real_action[0] *= 6
        real_action = real_action.astype(np.int32)
        return real_action

    def step(self, action):
        # Get action value to set to environment
        real_action = self._convert_to_real_action(action)

        # Step environment
        obs = self.env.step(action=real_action)
        self.step_num += 1        

        screen = obs["screen"]
        collided = obs["collided"]

        # Check collision
        reward = 0
        if len(collided) != 0:
            for id in collided:
                if id in self.plus_obj_ids_set:
                    reward += 1
                    self.plus_obj_ids_set.remove(id)
                elif id in self.minus_obj_ids_set:
                    reward -= 1
                    self.minus_obj_ids_set.remove(id)
                # Remove reward object from environment
                self.env.remove_obj(id)

        # Check if all positive rewards are taken
        is_empty = len(self.plus_obj_ids_set) == 0

        # Episode ends when step size exceeds MAX_STEP_NUM
        terminal = self.step_num >= MAX_STEP_NUM

        if (not terminal) and is_empty:
            screen = self._reset_sub()

        return screen, reward, terminal, {}

    def get_top_view(self):
        # Capture stage image from the top view
        pos = [0, 40, 0]
        rot_x = -np.pi * 0.5
        rot = [np.sin(rot_x * 0.5), 0, 0, np.cos(rot_x * 0.5)]

        ret = self.env.render(self.additional_camera_id, pos, rot)
        return ret["screen"]