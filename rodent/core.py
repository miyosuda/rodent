import rodent_module
import numpy as np

def to_nd_float_array(list_obj):
  """ Convert list to numpy float ndarray """
  if isinstance(list_obj, np.ndarray):
    return list_obj
  else:
    return np.array(list_obj, dtype=np.float32)

def to_nd_int_array(list_obj):
  """ Convert list to numpy int ndarray """
  if isinstance(list_obj, np.ndarray):
    return list_obj
  else:
    return np.array(list_obj, dtype=np.int32)


class Environment(object):
  """
  Environment class that wraps rodent_module.Env class.
  rodent_module.Env class requires numpy nd array for array objet, so this 
  Environment class converts list object as a wrapper.
  """
  def __init__(self, width, height, bg_color=[0.0, 0.0, 0.0]):
    """Create environment.
    Args:
      width: Screen width
      height: Screen height
      bg_color: Background color (RGB value with 0.0 ~ 1.0)
    """
    self.env = rodent_module.Env(width=width,
                                 height=height,
                                 bg_color=to_nd_float_array(bg_color))

  def add_box(self, texture_path, half_extent, pos, rot, detect_collision):
    """Add box object.
    Args:
      texture_path: Path for the texture (.png file)
      half_extent: (x,y,z) float values for half extent size of the box.
      pos: (x,y,z) float values for the center of the box.
      rot: A float value for head angle of the object (in radian)
      detect_collision: A bool value for indicating whether the object is included for collision
                        check result. If this argument is True, object's id is included when 
                        the agenet collides with this object.
    Returns:
      Int value for the object id.
    """
    return self.env.add_box(texture_path=texture_path,
                            half_extent=to_nd_float_array(half_extent),
                            pos=to_nd_float_array(pos),
                            rot=rot,
                            detect_collision=detect_collision)

  def add_sphere(self, texture_path, radius, pos, rot, detect_collision):
    """Add sphere object.
    Args:
      texture_path: Path for the texture (.png file)
      radius: float values for the raius of the shpere.
      pos: (x,y,z) float values for the center of the sphere.
      rot: A float value for head angle of the object (in radian)
      detect_collision: A bool value for indicating whether the object is included for collision
                        check result. If this argument is True, object's id is included when 
                        the agenet collides with this object.
    Returns:
      Int value for the object id.
    """    
    return self.env.add_sphere(texture_path=texture_path,
                               radius=radius,
                               pos=to_nd_float_array(pos),
                               rot=rot,
                               detect_collision=detect_collision)

  def add_model(self, path, scale, pos, rot, detect_collision):
    """Add model object with .obj format.
    Args:
      path: Path for the .obj file.
      scale: (x,y,z) float values for the scaling of the object.
      pos: (x,y,z) float values for the origin of the object.
      rot: A float value for head angle of the model (in radian)
      detect_collision: A bool value for indicating whether the object is included for collision
                        check result. If this argument is True, object's id is included when 
                        the agenet collides with this object.
    Returns:
      Int value for the object id.
    """
    return self.env.add_model(path=path,
                              scale=to_nd_float_array(scale),
                              pos=to_nd_float_array(pos),
                              rot=rot,
                              detect_collision=detect_collision)
  
  def locate_agent(self, pos, rot):
    """Locate agenet to given position and orientataion.
    Args:
      pos: (x,y,z) float values for agent's location.
      rot: A float value for head angle of the model (in radian)
    """
    self.env.locate_agent(pos=to_nd_float_array(pos),
                          rot=rot)

  def set_light_dir(self, dir):
    """Set direction of directional light.
    Args:
      dir: (x,y,z) float values for light direction.
    """
    self.env.set_light_dir(dir=to_nd_float_array(dir))

  def step(self, action, num_steps=1):
    """Step environment process and returns result.
    Args:
      action: Float array with 3 elements.
      num_steps: Int value for iteration count.
    Returns:
      Dictionary which contains the result of this step calculation.
        "screen": numpy nd_array of width * height * 3 (uint8)
        "collided" Int list of object ids that collided with the agent.
    """
    return self.env.step(action=to_nd_int_array(action), num_steps=num_steps)

  def remove_obj(self, id):
    """Remove object from environment.
    Args:
      id: Int value for deleting object's id
    """
    self.env.remove_obj(id=id)
