# -*- coding: utf-8 -*-
from . import rodentia_module
import numpy as np


def to_nd_float_array(list_obj):
    """ Convert list to numpy float ndarray """
    if isinstance(list_obj, np.ndarray):
        return list_obj
    else:
        return np.array(list_obj, dtype=np.float32)


def to_nd_float_array_for_rot(value):
    """ Convert list or single float to numpy float ndarray for rotation"""
    if isinstance(value, float) or isinstance(value, int):
        # if value is a single float, then it is treated as Y rotation value.
        value = [0.0, np.sin(value * 0.5), 0.0, np.cos(value * 0.5)]
        # Otherwise treat it as a quaternion
    return to_nd_float_array(value)


def to_nd_int_array(list_obj):
    """ Convert list to numpy int ndarray """
    if isinstance(list_obj, np.ndarray):
        return list_obj
    else:
        return np.array(list_obj, dtype=np.int32)


class Environment(object):
    """
    Environment class that wraps rodentia_module.Env class.
    rodentia_module.Env class requires numpy nd array for array objet, so this 
    Environment class converts list object as a wrapper.
    """

    def __init__(self, width, height, bg_color=[0.0, 0.0, 0.0]):
        """Create environment.
        Args:
          width: Screen width
          height: Screen height
        """
        # TODO: linux上にてwidth, heightが(1,1)対応で消せるかどうか調査
        self.env = rodentia_module.Env(width=width, height=height)
        self.main_camera_id = self.add_camera_view(width, height, bg_color)

    def add_camera_view(self, width, height, bg_color=[0.0, 0.0, 0.0]):
        """Add camera view.
        Args:
          width: Screen width
          height: Screen height
          bg_color: Background color (RGB value with 0.0 ~ 1.0)
        Returns:
          Int value for the camera id.
        """
        return self.env.add_camera_view(width=width, height=height,
                                        bg_color=to_nd_float_array(bg_color))

    def add_box(self,
                texture_path,
                half_extent,
                pos,
                rot=0.0,
                angle=0.0,
                mass=0.0,
                detect_collision=False):
        """Add box object.
        Args:
          texture_path: Path for the texture (.png file)
          half_extent: (x,y,z) float values for half extent size of the box.
          pos: (x,y,z) float values for the center of the box.
          rot: A float value for head angle (rot_y) or list (rx,ry,rz,rw) as the rotation quaternion of the object
               (in radian)
          mass: A float value for mass of the object. if mass == 0, the object is treated as static object,
                but if mass > 0, the object is physically simulated.
          detect_collision: A bool value for indicating whether the object is included for collision
                            check result. If this argument is True, object's id is included when 
                            the agenet collides with this object.
        Returns:
          Int value for the object id.
        """
        return self.env.add_box(
            texture_path=texture_path,
            half_extent=to_nd_float_array(half_extent),
            pos=to_nd_float_array(pos),
            rot=to_nd_float_array_for_rot(rot),
            mass=mass,
            detect_collision=detect_collision)

    def add_sphere(self,
                   texture_path,
                   radius,
                   pos,
                   rot=0.0,
                   mass=0.0,
                   detect_collision=False):
        """Add sphere object.
        Args:
          texture_path: Path for the texture (.png file)
          radius: float values for the raius of the shpere.
          pos: (x,y,z) float values for the center of the sphere.
          rot: A float value for head angle (rot_y) or list (rx,ry,rz,rw) as the rotation quaternion of the object
               (in radian)
          mass: A float value for mass of the object. if mass == 0, the object is treated as static object,
                but if mass > 0, the object is physically simulated.
          detect_collision: A bool value for indicating whether the object is included for collision
                            check result. If this argument is True, object's id is included when 
                            the agenet collides with this object.
        Returns:
          Int value for the object id.
        """
        return self.env.add_sphere(
            texture_path=texture_path,
            radius=radius,
            pos=to_nd_float_array(pos),
            rot=to_nd_float_array_for_rot(rot),
            mass=mass,
            detect_collision=detect_collision)

    def add_model(self,
                  path,
                  scale,
                  pos,
                  rot=0.0,
                  mass=0.0,
                  detect_collision=False):
        """Add model object with .obj format.
        Args:
          path: Path for the .obj file.
          scale: (x,y,z) float values for the scaling of the object.
          pos: (x,y,z) float values for the origin of the object.
          rot: A float value for head angle (rot_y) or list (rx,ry,rz,rw) as the rotation quaternion of the object
               (in radian)
          mass: A float value for mass of the object. if mass == 0, the object is treated as static object,
                but if mass > 0, the object is physically simulated.
          detect_collision: A bool value for indicating whether the object is included for collision
                            check result. If this argument is True, object's id is included when 
                            the agenet collides with this object.
        Returns:
          Int value for the object id.
        """
        return self.env.add_model(
            path=path,
            scale=to_nd_float_array(scale),
            pos=to_nd_float_array(pos),
            rot=to_nd_float_array_for_rot(rot),
            mass=mass,
            detect_collision=detect_collision)

    def locate_object(self, id, pos, rot=0.0):
        """Locate object to given position and orientataion.
        Args:
          id: Int value for object's id
          pos: (x,y,z) float values for agent's location.
          rot: A float value for head (rot_y) angle or list (rx,ry,rz,rw) as the rotation quaternion of the object
               (in radian)
        """
        self.env.locate_object(
            id=id,
            pos=to_nd_float_array(pos),
            rot=to_nd_float_array_for_rot(rot))

    def locate_agent(self, pos, rot_y=0.0):
        """Locate agenet to given position and orientataion.
        Args:
          pos: (x,y,z) float values for agent's location.
          rot_y: A float value for head angle of the model (in radian)
        """
        self.env.locate_agent(pos=to_nd_float_array(pos), rot_y=rot_y)

    def set_light(self,
                  dir=[-0.5, -1.0, -0.4],
                  color=[1.0, 1.0, 1.0],
                  ambient_color=[0.4, 0.4, 0.4],
                  shadow_rate=0.2):
        """Set light parameters.
        Args:
          dir: (x,y,z) float values for light direction.
          color: (r,g,b) float values for light color.
          ambient_color: (r,g,b) float values for ambient color.
          shadow_rate: a float, shadow color rate
        """        
        self.env.set_light(
            dir=to_nd_float_array(dir),
            color=to_nd_float_array(color),
            ambient_color=to_nd_float_array(ambient_color),
            shadow_rate=shadow_rate)

    def step(self, action, num_steps=1):
        """Step environment process and returns result.
        Args:
          action: Int array with 3 elements.
          num_steps: Int value for iteration count.
        Returns:
          Dictionary which contains the result of this step calculation.
            "collided" Int list of object ids that collided with the agent.
            "screen": numpy nd_array of width * height * 3 (uint8)
        """
        # TODO: step()とrender()分けるかどうか検討
        obs_step = self.env.step(action=to_nd_int_array(action),
                                 num_steps=num_steps)
        agent_info = self.get_agent_info()
        obs_render = self.render(self.main_camera_id,
                                 pos=agent_info["pos"],
                                 rot=agent_info["rot"])
        obs_step["screen"] = obs_render["screen"]
        return obs_step

    def render(self, camera_id, pos, rot):
        """Step environment process and returns result.
        Args:
          camera_id: Int array with 3 elements.
          
        Returns:
          Dictionary which contains the result of this step calculation.
            "screen": numpy nd_array of width * height * 3 (uint8)
        """
        return self.env.render(
            camera_id=camera_id,
            pos=to_nd_float_array(pos),
            rot=to_nd_float_array_for_rot(rot))

    def remove_obj(self, id):
        """Remove object from environment.
        Args:
          id: Int value for deleting object's id
        """
        self.env.remove_obj(id=id)

    def get_obj_info(self, id):
        """Get object information.
        Args:
          id: Int value for object's id
        Returns:
          Dictionary which contains the object's current state info.
            "pos": numpy nd_array (float32)
            "velocity" numpy nd_array (float32)
            "rot" numpy nd_array (float32)
        """
        return self.env.get_obj_info(id=id)

    def get_agent_info(self):
        """Get agent information.
        Returns:
          Dictionary which contains the agent's current state info.
            "pos": numpy nd_array (float32)
            "velocity" numpy nd_array (float32)
            "rot" numpy nd_array (float32)
            "rot_y" float
        """
        ret = self.env.get_agent_info()
        rot = ret["rot"]
        # Calculate rotation around Y-axis
        rot_y = np.arctan2(rot[1], rot[3]) * 2.0
        ret["rot_y"] = rot_y
        return ret

    def replace_obj_texture(self, id, texture_path):
        """Replace object texture(s).
           If object is consist of multiple meshes, textures of these meshes can be replaced by applying list of texture pathes.
        Args:
          id: Int value for object's id
          texture_path: A string or list of string. path of the texture(s)
        """
        if isinstance(texture_path, list):
            self.env.replace_obj_texture(id, texture_path)
        else:
            self.env.replace_obj_texture(id, [texture_path])
