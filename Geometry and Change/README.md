## Learn to create arbitrary transformation matrices from simple transformations

### Gasket3D
<img  style =" height : 300px" src ="../photo/Gasket.png">

### Cubev
  <img style =" height : 300px" src ="../photo/Cube.png">
  
 Callback function used to control object rotation
   
  ```cpp
    void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
  {
    if (action == GLFW_PRESS)
    {
      switch (button)
      {
      case GLFW_MOUSE_BUTTON_LEFT: //左键按下
        axis = xAxis;
        break;
      case GLFW_MOUSE_BUTTON_MIDDLE: //中健按下
        axis = yAxis;
        break;
      case GLFW_MOUSE_BUTTON_RIGHT: //右键按下
        axis = zAxis;
        break;
      default:
        break;
      }
    }
  }
  ```
### Sphere
1. Made the earth and >textured it


### Stack
<img src ="../photo/Stack1.png">

Only Foucs on *Geometry and Change*

1. Use C++ stack library
2. The sun rotates around its own center (rotation around a central point)
3. The Earth revolves around the Sun (rotation around a fixed point)
4. The moon revolves around the earth (rotation about a fixed point)

<a href ="../Texture/">
      Learn Texture
</a>
