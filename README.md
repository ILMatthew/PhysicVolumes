"# PhysicVolume" 

Physic Volumes are conteainer of different shapes that overrides or add functionality to Unreal physic system.
New functionality is added to Physic Volumes through components.

Physic Volumes can be nested as long as the Priority is set correctly.
A tool to automatically set priorities and add components to all volumes will be added later.

![PhysicVolumeShowcase](https://github.com/ILMatthew/MyPlugins/assets/46683546/b15b1ae0-e1b2-4c21-970a-2547390264df)


Shape currently implemented:
- Box
- Sphere
- Custom geometry

COMPONENTS

Gravity Field:
Overrides Unreal gravity based on the shape of the volume and Gravity intensity set by user.

Atmosphere:
Adds an atmosphere to the volume that can be used for various gameplay purposes.
Atmosphere of different Physic Volumes can interact with each other when connected.
A connector actor is used to create this connections.
Atmosphere parameters are:
- Atmospheric pressure (in bar)
- Air composition (gasses percentages)
- Temperature

Other parameters can be added if needed.

![Rooms with Physic Volumes linked by connector](https://github.com/ILMatthew/MyPlugins/assets/46683546/2fb3e80b-fed0-4a42-8791-62d7069f8343)
