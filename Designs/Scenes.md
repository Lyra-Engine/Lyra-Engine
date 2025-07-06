# Scene Management

This part is largely unimplemented.
This part of the library is at very unstable and unuseful state.

## ECS
Our choice of scene management design is based on ECS. Historically I have relied on hierarchical
scene graph, but this design would gradually couple more operations into the scene node class as
the system gets more complicated. Therefore, we adopted ECS based scene management system to decouple
the scene node entity from all kinds of logic. Components can be added or removed independently
without modifying the existing node class. An implicit scene node hierarchy is still preserved
by adding **ParentNode** component, which records the parent entity.

However, the ECS-based scene management does not naturally support walking the scene hierarchy
like walking down a tree. Common operations such as frustum culling might be not as efficient.
Therefore we might want to implement an OctTree as well for culling purpose.
