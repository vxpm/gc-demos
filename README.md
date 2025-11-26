# GameCube demos

These are some GameCube demos I quickly hacked together to test specific features. Uses libogc and
is heavily based on the [gamecube examples](https://github.com/devkitPro/gamecube-examples) repo.

## Demo list

For a more detailed understanding of what each demo does, you should read the source.

- pad: Prints controller state to the console.
- light: A rotating cube with a diffuse light rotating around it.
- light-diffuse: A rotating teapot with a diffuse light rotating around it.
- light-specular: A rotating teapot with a specular light rotating around it. Teapot looks shiny.
- light-spotlight: A rotating teapot with three spotlights (red, green and blue) rotating around it.
- alpha: Two quads moving around the screen. Pressing buttons on the controller changes either the blending
  mode, the blending factors or the background color of the scene.
- indirect: Simplest indirect texture stage usage.
- texgen-post-transform: Simple texgen post transformation matrix usage.
- copy-to-texture: Rotating cube which displays another rotating cube on it's faces. First it renders
  the inner rotating cube to a texture, then draws the outer cube using the texture on it's faces.
