"# PhysicVolume" 

Physic Volumes are conteainer of different shapes that overrides or add functionality to Unreal physic system.
New functionality is added to Physic Volumes through components.

Physic Volumes can be nested as long as the Priority is set correctly.
A tool to automatically set priorities and add components to all volumes will be added later.


![PhysicVolumesShowcase](https://github.com/ILMatthew/PhysicVolumes/assets/46683546/2c1d96f9-eeb3-48d1-968b-e482de7d4df5)


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


![Rooms with Physic Volumes linked by Connector](https://github.com/ILMatthew/PhysicVolumes/assets/46683546/c5a74984-8cf8-459d-85d5-14cc556034fe)
